/*
    csas - console file manager
    Copyright (C) 2020 TUVIMEN <suchora.dominik7@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "main.h"
#include "draw.h"
#include "load.h"
#include "preview.h"
#include "useful.h"
#include "inits.h"
#include "console.h"
#include "loading.h"

extern Key* keys;
extern size_t keys_t;
extern Settings* settings;

void ChangeWorkSpace(Basic* grf, const int num)
{
    register int temp = grf->current_workspace;
    grf->current_workspace = num;

    if (!grf->workspaces[num].exists)
    {
        grf->workspaces[num].exists = 1;
        grf->workspaces[num].show_message = false;
        grf->workspaces[num].path = strcpy(malloc(PATH_MAX),GET_DIR(temp,1)->path);
        CD(GET_DIR(temp,1)->path,num,grf);
    }
    else
        CD(GET_DIR(num,1)->path,num,grf);
}

int UpdateEvent(Basic* grf)
{
    int Event = getch();
    if (Event == -1) { return -1; }

    memset(grf->typed_keys,0,63);
    grf->was_typed = true;

    while (Event == -1 || Event == KEY_RESIZE || (Event > 47 && Event < 58))
    {
        if (Event == KEY_RESIZE) { UpdateSizeBasic(grf); DrawBasic(grf,-1); }
        if (Event > 47 && Event < 58) { grf->typed_keys[strlen(grf->typed_keys)] = Event; DrawBasic(grf,4); }
        Event = getch();
    }

    int* theyPass = NULL;
    size_t theyPass_t = 0;

    if ((int)Event == 27) { grf->was_typed = false; return -1; }
    grf->typed_keys[strlen(grf->typed_keys)] = Event;

    for (size_t i = 0; i < keys_t; i++)
    {
        if (Event == keys[i].keys[0])
        {
            theyPass = (int*)realloc(theyPass,(theyPass_t+1)*sizeof(int));
            theyPass_t++;
            theyPass[theyPass_t-1] = i;
        }
    }

    bool StartsTheString = 0;

    for (int i = 1; theyPass_t > 1; i++)
    {
        DrawBasic(grf,-1);
        do {
            if (Event == KEY_RESIZE) { UpdateSizeBasic(grf); DrawBasic(grf,-1); }
            if (Event > 47 && Event < 58) { grf->typed_keys[strlen(grf->typed_keys)] = Event; DrawBasic(grf,4); }
            Event = getch();
        } while (Event == -1 || Event == KEY_RESIZE || (StartsTheString && Event > 47 && Event < 58));
        if (Event == 27) { grf->was_typed = false; return -1; }
        grf->typed_keys[strlen(grf->typed_keys)] = Event;
        StartsTheString = 1;

        int* abcs = NULL;
        size_t abcs_t = 0;

        for (size_t j = 0; j < theyPass_t; j++)
        {
            if (Event == keys[theyPass[j]].keys[i])
            {
                abcs = (int*)realloc(abcs,(abcs_t+1)*sizeof(int));
                abcs_t++;
                abcs[abcs_t-1] = theyPass[j];
            }
        }

        free(theyPass);
        theyPass = abcs;
        theyPass_t = abcs_t;
    }

    grf->was_typed = false;
    return (theyPass_t == 0) ? -1 : theyPass[0];
}

static void GoDown(Basic* grf, const int workspace, const int which)
{
    if (grf->workspaces[workspace].visual)
    {
        for (size_t i = GET_SELECTED(workspace,which); i < GET_DIR(workspace,which)->size; i++)
            GET_DIR(workspace,which)->el[i].list[workspace] |= 1<<grf->workspaces[workspace].sel_group;
    }
    if (GET_DIR(workspace,which)->size > (size_t)grf->win[which]->_maxy-!settings->Borders+settings->Borders)
        GET_DIR(workspace,which)->ltop[workspace] = GET_DIR(workspace,which)->size-grf->win[which]->_maxy-!settings->Borders+settings->Borders;
    else
        GET_DIR(workspace,which)->ltop[workspace] = 0;
    GET_SELECTED(workspace,which) = GET_DIR(workspace,which)->size-1;
}

static void GoTop(Basic* grf, const int workspace, const int which)
{
    if (grf->workspaces[workspace].visual)
    {
        for (int i = GET_SELECTED(workspace,which); i > -1; i--)
            GET_DIR(workspace,which)->el[i].list[workspace] |= 1<<grf->workspaces[workspace].sel_group;
    }
    GET_SELECTED(workspace,which) = 0;
    GET_DIR(workspace,which)->ltop[workspace] = 0;
}

static void MoveD(const char how, Basic* grf, const int workspace, const int which)
{
    if (GET_SELECTED(workspace,which) == GET_DIR(workspace,which)->size-1 && how == 1)
    {
        if (settings->WrapScroll) GoTop(grf,workspace,which);
        return;
    }
    else if (GET_SELECTED(workspace,which) == 0 && how == -1)
    {
        if (settings->WrapScroll) GoDown(grf,workspace,which);
        return;
    }

    GET_SELECTED(workspace,which) += how;

    if ((how == 1)*((grf->win[which]->_maxy+GET_DIR(workspace,which)->ltop[workspace]-(settings->Borders*2)) < GET_DIR(workspace,which)->size-1)+(how == -1)*((grf->win[which]->_maxy+GET_DIR(workspace,which)->ltop[workspace]-(settings->Borders*2)) > 0) &&
        (how == 1)*(grf->win[which]->_maxy+GET_DIR(workspace,which)->ltop[workspace]-(settings->Borders*2) < GET_SELECTED(workspace,which)+(int)(grf->win[which]->_maxy*settings->MoveOffSet))+(how == -1)*(GET_DIR(workspace,which)->ltop[workspace] > GET_SELECTED(workspace,which)-(int)(grf->win[which]->_maxy*settings->MoveOffSet)))
    {
        if (settings->JumpScroll)
        {
            if ((how == 1)*((grf->win[which]->_maxy+GET_DIR(workspace,which)->ltop[workspace]-(settings->Borders*2)+(int)(grf->win[which]->_maxy*settings->JumpScrollValue)) > GET_DIR(workspace,which)->size-1)
                +(how == -1)*(GET_DIR(workspace,which)->ltop[workspace] > GET_SELECTED(workspace,which)-(int)(grf->win[which]->_maxy*settings->MoveOffSet)))
                GET_DIR(workspace,which)->ltop[workspace] = (how == 1)*(GET_DIR(workspace,which)->size-grf->win[which]->_maxy-!settings->Borders+settings->Borders);
            else
                GET_DIR(workspace,which)->ltop[workspace] += (int)(how*grf->win[which]->_maxy*settings->JumpScrollValue);
        }
        else
            GET_DIR(workspace,which)->ltop[workspace] += how;
    }

    GET_ESELECTED(workspace,which).list[workspace] |= (1<<grf->workspaces[workspace].sel_group)*grf->workspaces[workspace].visual;
}

void ExitBasic(Basic* grf, const bool force)
{
    if (force)
        goto END;

    int count = 0;
    for (int i = 0; i < WORKSPACE_N; i++)
        count += grf->workspaces[i].exists;

    grf->workspaces[grf->current_workspace].exists = 0;

    if (count > 1)
    {
        for (int i = grf->current_workspace+1; i < WORKSPACE_N; i++)
            if (grf->workspaces[i].exists)
            {
                ChangeWorkSpace(grf,i);
                return;
            }
        for (int i = 0; i < grf->current_workspace; i++)
            if (grf->workspaces[i].exists)
            {
                ChangeWorkSpace(grf,i);
                return;
            }
    }

    END: ;
    grf->exit_time = true;
}

void move_to(struct Dir* dir, const int workspace, const char* name)
{
    register ll found = -1;
    register size_t i;
    register bool temp;

    for (i = 0; i < dir->size && found == -1; i++)
    {
        temp = (strcmp(name,dir->el[i].name) == 0);
        found = i*temp+(-1*!temp);
    }

    if (found == -1) return;

    temp = workspace != -1;
    for (i = workspace*temp; i < (size_t)(workspace+1)*temp+(WORKSPACE_N*!temp); i++)
        dir->selected[i] = found;
}

static void bulk(Basic* grf, const int workspace, const int selected, char** args, const uchar flag)
{
    int fd[2];
    char buf[2][PATH_MAX];
    {
        strcpy(buf[0],TEMPTEMP);
        strcpy(buf[1],TEMPTEMP);
        
        int try[2];

        for (int i = 0; i < 2; i++)
        {
            try[i] = 0;
            while(try[i] < 5 && (fd[i] = mkstemp(buf[i])) == -1) try[i]++;
            if (try[i] > 4)
            {
                if (i == 1)
                    unlink(buf[0]);
                return;
            }
        }
    }

    size_t path_t = strlen(args[0]);
    bool CommentWrite;
    char* temp;

    for (size_t i = 0; i < grf->size; i++)
    {
        if (!(
        #ifdef __THREADS_FOR_DIR_ENABLE__
        !grf->base[i]->enable &&
        #endif
        grf->base[i]->size > 0))
            continue;

        if (path_t)
        {
            if (flag & 0x2)
            {
                if (!strstr(grf->base[i]->path,args[0]))
                    continue;
            }
            else if ((strcmp(grf->base[i]->path,args[0]) != 0))
                continue;
        }

        CommentWrite = 0;
        for (size_t j = 0; j < grf->base[i]->size; j++)
        {
            if (selected == -1 ? 1 : (grf->base[i]->el[j].list[workspace]&selected))
            {
                if (!CommentWrite)
                {
                    write(fd[0],"//\t",3);
                    write(fd[0],grf->base[i]->path,strlen(grf->base[i]->path));
                    write(fd[0],"\n",1);
                    CommentWrite = 1;
                }
                temp = (flag & 0x1) ? MakePath(grf->base[i]->path,grf->base[i]->el[j].name) : grf->base[i]->el[j].name;
                write(fd[0],temp,strlen(temp));
                write(fd[0],"\n",1);
            }
        }
    }

    spawn(args[2],buf[0],NULL,F_NORMAL|F_WAIT);

    size_t pos = 0, x;
    lseek(fd[0],0,SEEK_SET);

    struct stat sFile;
    fstat(fd[0],&sFile);
    char* file = (char*)malloc(sFile.st_size+1);
    read(fd[0],file,sFile.st_size);

    char buffer[PATH_MAX];

    write(fd[1],"#!",2);
    write(fd[1],args[1],strlen(args[1]));
    write(fd[1],"\n\n",2);

    if (file[pos])
    {
        for (size_t i = 0; i < grf->size; i++)
        {
            if (!(
            #ifdef __THREADS_FOR_DIR_ENABLE__
            !grf->base[i]->enable &&
            #endif
            grf->base[i]->size > 0))
                continue;

            if (path_t)
            {
                if (flag & 0x2)
                {
                    if (!strstr(grf->base[i]->path,args[0]))
                        continue;
                }
                else if ((strcmp(grf->base[i]->path,args[0]) != 0))
                    continue;
            }

            for (size_t j = 0; j < grf->base[i]->size; j++)
            {
                if (selected == -1 ? 1 : (grf->base[i]->el[j].list[workspace]&selected))
                {
                    while (file[pos] == '\n')
                    {
                        pos++;
                        j--;
                    }
                    if (!file[pos])
                        break;

                    if (file[pos] == '/' && file[pos+1] && file[pos+1] == '/')
                    {
                        while (file[pos] && file[pos] != '\n') pos++;
                        j--;
                    }
                    else
                    {
                        x = 0;
                        while (file[pos] && file[pos] != '\n') buffer[x++] = file[pos++];
                        buffer[x] = '\0';
                        temp = (flag & 0x1) ? MakePath(grf->base[i]->path,grf->base[i]->el[j].name) : grf->base[i]->el[j].name;
                        if (x > 0 && strcmp(temp,buffer) != 0)
                        {
                            if (write(fd[1],args[3],strlen(args[3])) > 0) write(fd[1]," ",1);

                            if (!(flag & 0x1)) temp = MakePath(grf->base[i]->path,temp);
                            MakePathRunAble(temp);
                            if (write(fd[1],temp,strlen(temp)) > 0) write(fd[1]," ",1);

                            if (write(fd[1],args[4],strlen(args[4])) > 0) write(fd[1]," ",1);

                            temp = MakePath(grf->base[i]->path,buffer);
                            MakePathRunAble(temp);
                            if (write(fd[1],temp,strlen(temp)) > 0) write(fd[1]," ",1);

                            write(fd[1],args[5],strlen(args[5]));

                            write(fd[1],"\n",1);
                        }
                    }

                    pos++;
                }
            }
        }
    }

    free(file);

    spawn(args[2],buf[1],NULL,F_NORMAL|F_WAIT);
    spawn(args[1],buf[1],NULL,F_NORMAL|F_WAIT|F_CONFIRM);

    for (int i = 0; i < 2; i++)
    {
        unlink(buf[i]);
        close(fd[i]);
    }
}

void ___SET(const char* src, Basic* grf)
{
    size_t pos = 0;

    struct SetEntry SetEntries[] = {
        {"FileOpener",&settings->FileOpener},
        {"shell",&settings->shell},{"editor",&settings->editor},{"Bar1Settings",&settings->Bar1Settings},
        {"Bar2Settings",&settings->Bar2Settings},{"C_Borders",&settings->C_Borders},
        {"UserHostPattern",&settings->UserHostPattern},{"CopyBufferSize",&settings->CopyBufferSize},
        {"MoveOffSet",&settings->MoveOffSet},{"SDelayBetweenFrames",&settings->SDelayBetweenFrames},
        {"WrapScroll",&settings->WrapScroll},{"JumpScrollValue",&settings->JumpScrollValue},
        {"StatusBarOnTop",&settings->StatusBarOnTop},{"WinSizeMod",&settings->WinSizeMod},
        {"Win1Enable",&settings->Win1Enable},{"Win3Enable",&settings->Win3Enable},{"UserRHost",&settings->UserRHost},
        {"Bar1Enable",&settings->Bar1Enable},{"Bar2Enable",&settings->Bar2Enable},
        {"Borders",&settings->Borders},{"FillBlankSpace",&settings->FillBlankSpace},
        {"WindowBorder",&settings->WindowBorder},{"EnableColor",&settings->EnableColor},
        {"DelayBetweenFrames",&settings->DelayBetweenFrames},{"NumberLines",&settings->NumberLines},
        {"NumberLinesOff",&settings->NumberLinesOff},{"NumberLinesFromOne",&settings->NumberLinesFromOne},
        {"DisplayingC",&settings->DisplayingC},{"JumpScroll",&settings->JumpScroll},{"Values",&settings->Values},
        {"DirLoadingMode",&settings->DirLoadingMode},{"PreviewSettings",&settings->PreviewSettings},
        #ifdef __SHOW_HIDDEN_FILES_ENABLE__
        {"ShowHiddenFiles",&settings->ShowHiddenFiles},
        #endif
        #ifdef __SORT_ELEMENTS_ENABLE__
        {"SortMethod",&settings->SortMethod},{"BetterFiles",&settings->BetterFiles},
        #endif
        {"DirSizeMethod",&settings->DirSizeMethod},{"C_Error",&settings->C_Error},
        #ifdef __COLOR_FILES_BY_EXTENSION__
        {"C_FType_A",&settings->C_FType_A},{"C_FType_I",&settings->C_FType_I},{"C_FType_V",&settings->C_FType_V},
        #endif
        {"C_Selected",&settings->C_Selected},{"C_Exec_set",&settings->C_Exec_set},{"C_Exec_col",&settings->C_Exec_col},
        {"C_Dir",&settings->C_Dir},{"C_Reg",&settings->C_Reg},{"C_Fifo",&settings->C_Fifo},
        {"C_Sock",&settings->C_Sock},{"C_Dev",&settings->C_Dev},{"C_BDev",&settings->C_BDev},
        {"C_SymLink",&settings->C_SymLink},{"C_FileMissing",&settings->C_FileMissing},
        {"C_Other",&settings->C_Other},{"C_User_S_D",&settings->C_User_S_D},
        {"C_Bar_Dir",&settings->C_Bar_Dir},{"C_Bar_Name",&settings->C_Bar_Name},{"C_Bar_WorkSpace",&settings->C_Bar_WorkSpace},
        {"C_Bar_WorkSpace_Selected",&settings->C_Bar_WorkSpace_Selected},{"C_Group",&settings->C_Group},
        {"C_Bar_F",&settings->C_Bar_F},{"C_Bar_E",&settings->C_Bar_E},
        #ifdef __THREADS_FOR_DIR_ENABLE__
        {"ThreadsForDir",&settings->ThreadsForDir},
        #endif
        #ifdef __THREADS_FOR_FILE_ENABLE__
        {"ThreadsForFile",&settings->ThreadsForFile},
        #endif
        {NULL,NULL}
    };

    size_t end = 0;
    while (src[pos+end] && !isspace(src[pos+end])) end++;
    int gga = -1;

    for (int i = 0; SetEntries[i].name; i++)
        if (strlen(SetEntries[i].name) == end && strncmp(src+pos,SetEntries[i].name,end) == 0) { gga = i; break; }

    if (gga == -1) return;

    pos += end;
    pos += FindFirstCharacter(src+pos);

    pos += StrToValue(SetEntries[gga].value,src+pos);
}

#ifdef __LOAD_CONFIG_ENABLE__
void ___INCLUDE(const char* src, Basic* grf)
{
    static char temp[8192];

    StrToPath(temp,src);
    LoadConfig(temp,grf);
}
#endif

void ___MAP(const char* src, Basic* grf)
{
    size_t pos = 0, end = 0;
    static char temp[8192];
    Key TempKey;
    TempKey.keys = (char*)malloc(64);
    while (src[pos+end] && !isspace(src[pos+end])) end++;
    strncpy(TempKey.keys,src+pos,end);
    TempKey.keys[end] = '\0';
    pos += end;

    pos += FindFirstCharacter(src+pos);

    pos += StrToPath(temp,src+pos);
    TempKey.value = temp;

    addKey(TempKey);
    free(TempKey.keys);
}

void ___MOVE(const char* src, Basic* grf)
{
    size_t pos = 0;
    char rot = -1;
    int workspace = grf->current_workspace, mul1 = 1, mul2 = 0;

    while (src[pos])
    {
        if (src[pos] && src[pos] == '-')
        {
            do {
                pos++;

                switch (src[pos])
                {
                    case 'd':
                        rot = 1;
                        break;
                    case 'u':
                        rot = -1;
                        break;
                    case 'l':
                        rot = 3;
                        break;
                    case 'r':
                        rot = 4;
                        break;
                    case 'c':
                    case 'w':
                        {
                            char ctemp = src[pos];
                            pos++;
                            pos += FindFirstCharacter(src+pos);
                            if (ctemp == 'c')
                                mul1 = atoi(src+pos);
                            else
                                workspace = atoi(src+pos);
                            while (isdigit(src[pos])) pos++;
                        }
                        break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }
        pos++;
    }

    if (!grf->workspaces[workspace].exists)
        return;

    switch (rot)
    {
        case -1:
        case 1:
            if (
                #ifdef __THREADS_FOR_DIR_ENABLE__
                !GET_DIR(workspace,1)->enable &&
                #endif
                GET_DIR(workspace,1)->size > 0)
            {
                mul2 = atoi(grf->typed_keys);
                for (register int i = 0; i < (mul2+(mul2 == 0))*mul1; i++)
                    MoveD(rot,grf,workspace,1);
                if (settings->Win3Enable)
                    Preview(grf);
            }
            break;
        case 3:
            if (grf->workspaces[workspace].path[0] == '/' && grf->workspaces[grf->current_workspace].path[1] == '\0')
                break;
            CD("..",workspace,grf);
            grf->workspaces[workspace].visual = 0;
            break;
        case 4:
            if (GET_DIR(workspace,1)->size > 0)
            {
                switch (GET_ESELECTED(workspace,1).type&T_GT)
                {
                    case T_DIR:
                        CD(GET_ESELECTED(workspace,1).name,workspace,grf);
                        grf->workspaces[workspace].visual = 0;
                        break;
                    case T_REG:
                        RunFile(GET_ESELECTED(workspace,1).name);
                        break;
                }
            }
            break;
    }
}

void ___QUIT(const char* src, Basic* grf)
{
    bool force = false;
    if (src[0] && src[0] == '-' && src[1] && src[1] == 'f')
        force = true;
    ExitBasic(grf,force);
}

void ___CD(const char* src, Basic* grf)
{
    size_t pos = 0;
    int workspace = grf->current_workspace;
    static char path[8192];
    memset(path,0,8191);

    while (src[pos])
    {
        if (src[pos] == '-' && src[pos+1] && src[pos+1] == 'w')
        {
            pos += 2;
            pos += FindFirstCharacter(src+pos);
            workspace = atoi(src+pos);
            while (isdigit(src[pos])) pos++;
        }
        else
            pos += StrToPath(path,src+pos);

        pos += FindFirstCharacter(src+pos);
    }

    grf->workspaces[workspace].exists = true;
    ChangeWorkSpace(grf,workspace);

    CD(path,workspace,grf);
}

void ___GOTOP(const char* src, Basic* grf)
{
    size_t pos = 0, target = atol(grf->typed_keys);
    int workspace = grf->current_workspace;

    while (src[pos])
    {
        if (src[pos] == '-')
        {
            do {
                pos++;
                switch (src[pos])
                {
                    case 't':
                    case 'w':
                        pos++;
                        pos += FindFirstCharacter(src+pos);
                        if (src[pos] == 't')
                            target = atol(src+pos);
                        else
                            workspace = atoi(src+pos);
                        while (isdigit(src[pos])) pos++;
                        break;
                }
            } while (src[pos] && !isspace(src[pos]));
        }

        pos += FindFirstCharacter(src+pos);
        if (src[pos+1] == '\0')
            break;
    }

    if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        !GET_DIR(workspace,1)->enable &&
        #endif
        GET_DIR(workspace,1)->size > 0)
    {
        if (target == 0)
            GoTop(grf,workspace,1);
        else if (target > GET_SELECTED(workspace,1))
        {
            for (size_t i = GET_SELECTED(workspace,1); i < target-1; i++)
                MoveD(1,grf,workspace,1);
        }
        else if (target < GET_SELECTED(workspace,1))
        {
            for (size_t i = GET_SELECTED(workspace,1); i > target-1; i--)
                MoveD(-1,grf,workspace,1);
        }
        if (workspace == grf->current_workspace && settings->Win3Enable)
            Preview(grf);
    }
}

void ___GODOWN(const char* src, Basic* grf)
{
    size_t pos = 0, target = atol(grf->typed_keys);
    int workspace = grf->current_workspace;

    while (src[pos])
    {
        if (src[pos] == '-')
        {
            do {
                pos++;
                switch (src[pos])
                {
                    case 't':
                    case 'w':
                        pos++;
                        pos += FindFirstCharacter(src+pos);
                        if (src[pos] == 't')
                            target = atol(src+pos);
                        else
                            workspace = atoi(src+pos);
                        while (isdigit(src[pos])) pos++;
                        break;
                }
            } while (src[pos] && !isspace(src[pos]));
        }

        pos += FindFirstCharacter(src+pos);
        if (src[pos+1] == '\0') break;
    }

    if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        !GET_DIR(workspace,1)->enable &&
        #endif
        GET_DIR(workspace,1)->size > 0)
    {
        if (target == 0)
            GoDown(grf,target,1);
        else if (target > GET_SELECTED(workspace,1))
        {
            for (size_t i = GET_SELECTED(workspace,1); i < target-1; i++)
                MoveD(1,grf,workspace,1);
        }
        else if (target < GET_SELECTED(workspace,1))
        {
            for (size_t i = GET_SELECTED(workspace,1); i > target-1; i--)
                MoveD(-1,grf,workspace,1);
        }
        if (workspace == grf->current_workspace && settings->Win3Enable)
            Preview(grf);
    }
}

void ___CHANGEWORKSPACE(const char* src, Basic* grf)
{
    ChangeWorkSpace(grf,atoi(src));
}

#ifdef __FILE_SIZE_ENABLE__

static void GETSIZE(struct Element* el, const int fd, const uchar flag)
{
    if ((el->type&T_GT) == T_DIR)
    {
        int tfd;
        if ((tfd = openat(fd,el->name,O_DIRECTORY)) != -1)
        {
            if ((flag&0x8) != 0x8)
            {
                ull size = 0, count = 0;
                GetDirSize(tfd,&count,&size,(flag&0x2)==0x2);
                el->size = (flag&0x4)==0x4 ? count : size;
            }
            else
            {
                struct stat sFile;
                stat(el->name,&sFile);
                el->size = sFile.st_size;
            }
            close(tfd);
        }
    }
}

void ___GETSIZE(const char* src, Basic* grf)
{
    size_t pos = 0;
    uchar flag = 0;
    int selected = -1, workspace = grf->current_workspace;
    char* path = (char*)calloc(sizeof(char),PATH_MAX);

    while (src[pos])
    {
        pos += FindFirstCharacter(src+pos);

        if (src[pos] == '-')
        {
            do {
                pos++;
                switch (src[pos])
                {
                    case 'w':
                    case 's':
                        {
                            int itemp = (src[pos] == 'w' ? 1 : 0);
                            pos++;
                            pos += FindFirstCharacter(src+pos);
                            if (itemp)
                                workspace = atoi(src+pos);
                            else
                            {
                                switch (src[pos])
                                {
                                    case '-': selected = -1; pos++; break;
                                    case '.': selected = -2; pos++; break;
                                    case 's': selected = -3; pos++; break;
                                    default: selected = 1<<atoi(src+pos); break;
                                }
                            }
                            while (isdigit(src[pos])) pos++;
                        }
                        break;
                    case 'R': flag |= 0x1; break;
                    case 'r': flag |= 0x2; break;
                    case 'c': flag |= 0x4; break;
                    case 'f': flag |= 0x8; break;
                }
            } while (src[pos] && !isspace(src[pos]));
        }
        else
            pos += StrToPath(path,src+pos);

        if (src[pos+1] == '\0')
            break;
    }

    if (!grf->workspaces[grf->current_workspace].exists)
    {
        free(path);
        return;
    }

    if (workspace != grf->current_workspace)
		if (chdir(GET_DIR(workspace,1)->path) != 0)
			return;

    if (selected == -2)
        selected = grf->workspaces[workspace].sel_group;
    if (selected == -3)
    {
        int fd;
        if (
            #ifdef __THREADS_FOR_DIR_ENABLE__
            !GET_DIR(workspace,1)->enable &&
            #endif
            GET_DIR(workspace,1)->size > 0 && (fd = open(GET_DIR(workspace,1)->path,O_DIRECTORY)) != -1)
        GETSIZE(&GET_ESELECTED(workspace,1),fd,flag);
        if (!(flag&0x1))
        {
            free(path);
            return;
        }
        else
            strcpy(path,MakePath(GET_DIR(workspace,1)->path,GET_ESELECTED(workspace,1).name));
    }

    if (path[0])
    {
        char* stemp = (char*)malloc(PATH_MAX);
        realpath(path,stemp);
        free(path);
        path = stemp;
    }

    int temp;

    for (size_t i = 0; i < grf->size; i++)
    {
        if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        grf->base[i]->enable ||
        #endif
        grf->base[i]->size < 1)
            continue;
        if (path[0])
        {
            if (flag&0x1)
            {
                if (!strstr(grf->base[i]->path,path))
                    continue;
            }
            else if (strcmp(grf->base[i]->path,path) != 0)
                continue;
        }

        if ((temp = open(grf->base[i]->path,O_DIRECTORY)) == -1)
            continue;

        for (size_t j = 0; j < grf->base[i]->size; j++)
            if (selected < 0 ? 1 : (grf->base[i]->el[j].list[workspace]&(1<<selected)))
                GETSIZE(&grf->base[i]->el[j],temp,flag);
        close(temp);
    }

    free(path);
    if (workspace != grf->current_workspace)
        chdir(GET_DIR(grf->current_workspace,1)->path);
}

#endif

void ___SETGROUP(const char* src, Basic* grf)
{
    grf->workspaces[grf->current_workspace].sel_group = atoi(src);
    if (grf->workspaces[grf->current_workspace].visual && GET_DIR(grf->current_workspace,1)->size > 0)
        GET_ESELECTED(grf->current_workspace,1).list[grf->current_workspace] |= 1<<grf->workspaces[grf->current_workspace].sel_group;
}

void ___FASTSELECT(const char* src, Basic* grf)
{
    if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        !GET_DIR(grf->current_workspace,1)->enable &&
        #endif
        GET_DIR(grf->current_workspace,1)->size > 0)
    {
        GET_ESELECTED(grf->current_workspace,1).list[grf->current_workspace] ^= 1<<grf->workspaces[grf->current_workspace].sel_group;
        MoveD(1,grf,grf->current_workspace,1);
        if (settings->Win3Enable)
            Preview(grf);
    }
}

void ___TOGGLEVISUAL(const char* src, Basic* grf)
{
    grf->workspaces[grf->current_workspace].visual = !grf->workspaces[grf->current_workspace].visual;
    if (grf->workspaces[grf->current_workspace].visual && GET_DIR(grf->current_workspace,1)->size > 0)
        GET_ESELECTED(grf->current_workspace,1).list[grf->current_workspace] |= 1<<grf->workspaces[grf->current_workspace].sel_group;
}

void ___F_MOD(const char* src, Basic* grf)
{
    uchar Action = 0;
    mode_t arg = 0;
    size_t pos = 0;

    switch (src[0])
    {
        case 'd': case 'D': Action = 1; break;
        case 'c': case 'C': Action = 2; break;
        case 'm': case 'M': Action = 3; break;
        default: return;
    }

    pos++;
    pos += FindFirstCharacter(src+pos);

    int selected = -1, workspace = grf->current_workspace;
    char* path = (char*)calloc(sizeof(char),PATH_MAX);
    char* target = (char*)calloc(sizeof(char),PATH_MAX);
    int fd1, fd2, fd3;
    char* buffer = Action == 1 ? NULL : (char*)malloc(settings->CopyBufferSize);

    while (src[pos])
    {
        pos += FindFirstCharacter(src+pos);

        if (src[pos] == '-')
        {
            do {
                pos++;
                switch (src[pos])
                {
                    case 'o':
                        pos++;
                        pos += FindFirstCharacter(src+pos);
                        pos += StrToPath(target,src+pos);
                        break;
                    case 'w':
                    case 's':
                        {
                            int itemp = (src[pos] == 'w' ? 1 : 0);
                            pos++;
                            pos += FindFirstCharacter(src+pos);
                            if (itemp)
                                workspace = atoi(src+pos);
                            else
                            {
                                switch (src[pos])
                                {
                                    case '-': selected = -1; pos++; break;
                                    case '.': selected = -2; pos++; break;
                                    case 's': selected = -3; pos++; break;
                                    default: selected = 1<<atoi(src+pos); break;
                                }
                            }
                            while (isdigit(src[pos])) pos++;
                        }
                        break;
                    case 'c': arg |= M_CHNAME; break;
                    case 'r': arg |= M_REPLACE; break;
                    case 'd': arg |= M_DCPY; break;
                    case 'm': arg |= M_MERGE; break;
                }
            } while (src[pos] && !isspace(src[pos]));
        }
        else
            pos += StrToPath(path,src+pos);

        if (src[pos+1] == '\0')
            break;
    }

    if (!grf->workspaces[grf->current_workspace].exists)
        goto END;

    if (workspace != grf->current_workspace)
        if (chdir(GET_DIR(workspace,1)->path) != 0)
			return;

    if (selected == -2)
        selected = grf->workspaces[workspace].sel_group;

    if (!target[0])
        strcpy(target,".");

    if ((fd1 = open(target,O_DIRECTORY)) == -1)
        goto END;

    char* ActionName = NULL;
    switch (Action)
    {
        case 1: ActionName = "delete"; break;
        case 2: ActionName = "copy"; break;
        case 3: ActionName = "move"; break;
    }

    struct stat ST;
    ull count = 0, size = 0;

    if (selected == -3)
    {
        if (
            #ifdef __THREADS_FOR_DIR_ENABLE__
            !GET_DIR(workspace,1)->enable &&
            #endif
            GET_DIR(workspace,1)->size > 0 && (fd2 = open(GET_DIR(workspace,1)->path,O_DIRECTORY)) != -1)
        {
            if (fstatat(fd2,GET_ESELECTED(workspace,1).name,&ST,AT_SYMLINK_NOFOLLOW) == -1)
                goto END2;

            if ((ST.st_mode&S_IFMT) == S_IFDIR)
            {
				if ((fd3 = openat(fd2,GET_ESELECTED(workspace,1).name,O_RDONLY)) == -1)
					goto END2;
                GetDirSize(fd3,&count,&size,true);
				close(fd3);
                count++;
                size += ST.st_size;
                SetMessage(grf,0,"Do you want to %s %ld files(%s)? (Y/n)",ActionName,count,MakeHumanReadAble(size));
            }
            else
                #ifdef __FILE_SIZE_ENABLE__
                SetMessage(grf,0,"Do you want to %s \"%s\" (%s)? (Y/n)",ActionName,GET_ESELECTED(workspace,1).name,MakeHumanReadAble(GET_ESELECTED(workspace,1).size));
                #else
                SetMessage(grf,0,"Do you want to %s \"%s\"? (Y/n)",ActionName,GET_ESELECTED(workspace,1).name);
                #endif

            wrefresh(grf->win[5]);
            int si = -1;
            for (;;)
            {
                si = getch();
                if (si == KEY_RESIZE)
                {
                    UpdateSizeBasic(grf);
                    DrawBasic(grf,-1);
                    if ((ST.st_mode&S_IFMT) == S_IFDIR)
                        SetMessage(grf,0,"Do you want to %s %ld files(%s)? (Y/n)",ActionName,count,MakeHumanReadAble(size));
                    else
                        #ifdef __FILE_SIZE_ENABLE__
                        SetMessage(grf,0,"Do you want to %s \"%s\" (%s)? (Y/n)",ActionName,GET_ESELECTED(workspace,1).name,MakeHumanReadAble(GET_ESELECTED(workspace,1).size));
                        #else
                        SetMessage(grf,0,"Do you want to %s \"%s\"? (Y/n)",ActionName,GET_ESELECTED(workspace,1).name);
                        #endif
                    wrefresh(grf->win[5]);
                }
                else if (si == 'y' || si == 'Y')
                    break;
                else if (si != -1)
                    goto END2;
            }

            switch (Action)
            {
                case 1: DeleteFile(fd2,GET_ESELECTED(workspace,1).name); break;
                case 2: CopyFile(fd1,fd2,GET_ESELECTED(workspace,1).name,buffer,arg); break;
                case 3: MoveFile(fd1,fd2,GET_ESELECTED(workspace,1).name,buffer,arg); break;
            }
            END2: ;
            close(fd2);
        }
        close(fd1);
        goto END;
    }

    if (path[0])
    {
        char* stemp = (char*)malloc(PATH_MAX);
        realpath(path,stemp);
        free(path);
        path = stemp;
    }

    for (size_t i = 0; i < grf->size; i++)
    {
        if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        grf->base[i]->enable ||
        #endif
        grf->base[i]->size < 1)
            continue;
        if (path[0] && strcmp(grf->base[i]->path,path) != 0)
                continue;

        if ((fd2 = open(grf->base[i]->path,O_DIRECTORY)) == -1)
            continue;

        for (size_t j = 0; j < grf->base[i]->size; j++)
        {
            if (selected < 0 ? 1 : (grf->base[i]->el[j].list[workspace]&(1<<selected)))
            {
                if (fstatat(fd2,grf->base[i]->el[j].name,&ST,AT_SYMLINK_NOFOLLOW) == -1)
                    continue;
                count++;
                size += ST.st_size;
                if ((ST.st_mode&S_IFMT) == S_IFDIR)
				{
					if ((fd3 = openat(fd2,grf->base[i]->el[j].name,O_RDONLY)) != -1)
					{
						GetDirSize(fd3,&count,&size,true);
						close(fd3);
					}
				}
            }
        }
        close(fd2);
    }

    SetMessage(grf,0,"Do you want to %s %ld files(%s)? (Y/n)",ActionName,count,MakeHumanReadAble(size));
    wrefresh(grf->win[5]);
    int si = -1;
    for (;;)
    {
        si = getch();
        if (si == KEY_RESIZE)
        {
            UpdateSizeBasic(grf);
            DrawBasic(grf,-1);
            SetMessage(grf,0,"Do you want to %s %ld files(%s)? (Y/n)",ActionName,count,MakeHumanReadAble(size));
            wrefresh(grf->win[5]);
        }
        else if (si == 'y' || si == 'Y')
            break;
        else if (si != -1)
            goto END;
    }

    for (size_t i = 0; i < grf->size; i++)
    {
        if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        grf->base[i]->enable ||
        #endif
        grf->base[i]->size < 1)
            continue;
        if (path[0] && strcmp(grf->base[i]->path,path) != 0)
                continue;

        if ((fd2 = open(grf->base[i]->path,O_DIRECTORY)) == -1)
            continue;

        for (size_t j = 0; j < grf->base[i]->size; j++)
        {
            if (selected < 0 ? 1 : (grf->base[i]->el[j].list[workspace]&(1<<selected)))
            {
                switch (Action)
                {
                    case 1: DeleteFile(fd2,grf->base[i]->el[j].name); break;
                    case 2: CopyFile(fd1,fd2,grf->base[i]->el[j].name,buffer,arg); break;
                    case 3: MoveFile(fd1,fd2,grf->base[i]->el[j].name,buffer,arg); break;
                }
            }
        }
        close(fd2);
    }
    close(fd1);

    UpdateSizeBasic(grf);
    CD(".",grf->current_workspace,grf);

    END: ;
    grf->workspaces[grf->current_workspace].show_message = false;
    free(buffer);
    free(path);
    free(target);
    if (workspace != grf->current_workspace)
        chdir(GET_DIR(grf->current_workspace,1)->path);
}

static void GETDIR(char* path, Basic* grf, uchar mode
#ifdef __THREADS_FOR_DIR_ENABLE__
, bool threads
#endif
)
{
    DIR* d;
    if ((d = opendir(path)) == NULL)
        return;
    struct dirent* dir;
    while ((dir = readdir(d)))
    {
        if (dir->d_name[0] == '.' && (dir->d_name[1] == '\0' || (dir->d_name[1] == '.' && dir->d_name[2] == '\0')))
            continue;
        if (dir->d_type == 4)
        {
            char path1[PATH_MAX];
            strcpy(path1,path);
            strcat(path1,"/");
            strcat(path1,dir->d_name);
            GetDir(path1,grf,grf->current_workspace,1,mode
            #ifdef __THREADS_FOR_DIR_ENABLE__
            ,threads
            #endif
            );
            GETDIR(path1,grf,mode
            #ifdef __THREADS_FOR_DIR_ENABLE__
            ,threads
            #endif
            );
       }
    }
    closedir(d);
}

void ___LOAD(const char* src, Basic* grf)
{
    size_t pos = 0;
    int mode = settings->DirLoadingMode;
    uchar flag = 0;
	werase(grf->win[0]);
	if (settings->Win1Enable)
		werase(grf->win[1]);
	if (settings->Win3Enable)
		werase(grf->win[2]);

    while (src[pos])
    {
        if (src[pos] == '-')
        {
            do {
                pos++;

                switch (src[pos])
                {
                    case 'm':
                        pos++;
                        pos += FindFirstCharacter(src+pos);
                        mode += atoi(src+pos);
                        while (isdigit(src[pos])) pos++;
                        break;
                    #ifdef __THREADS_FOR_DIR_ENABLE__
                    case 't': flag |= 0x1; break;
                    #endif
                    case 'R': flag |= 0x2; break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }

        if (src[pos+1] == '\0')
            break;
        else
            pos += FindFirstCharacter(src+pos);
    }

    GetDir(".",grf,grf->current_workspace,1,mode
    #ifdef __THREADS_FOR_DIR_ENABLE__
    ,(flag&0x1)
    #endif
    );
    if (flag&0x2)
    {
        char path[PATH_MAX];
        strcpy(path,".");
        GETDIR(path,grf,mode
        #ifdef __THREADS_FOR_DIR_ENABLE__
        ,(flag&0x1)
        #endif
        );
    }
}

void ___SELECT(const char* src, Basic* grf)
{
    size_t pos = 0;
    int mode = 1;
    bool recursive = 0;
    int selected = -1, workspace1 = grf->current_workspace, workspace2 = grf->current_workspace, toselected = -1;
    char* path = (char*)calloc(sizeof(char),PATH_MAX);

    while (src[pos])
    {
        pos += FindFirstCharacter(src+pos);

        if (src[pos] == '-')
        {
            do {
                pos++;

                switch (src[pos])
                {
                    case 'R':
                        recursive = 1;
                        break;
                    case 'o':
                        pos++;
                        pos+= FindFirstCharacter(src+pos);
                        switch(src[pos])
                        {
                            case '.':
                                toselected = -1;
                                pos++;
                                break;
                            default:
                                toselected = atoi(src+pos);
                                while (isdigit(src[pos])) pos++;
                                break;
                        }
                        break;
                    case 'w':
                    case 'W':
                        {
                            int itemp = (src[pos] == 'w' ? 1 : 0);
                            pos++;
                            pos += FindFirstCharacter(src+pos);
                            if (itemp)
                                workspace1 = atoi(src+pos);
                            else
                                workspace2 = atoi(src+pos);
                            while (isdigit(src[pos])) pos++;
                        }
                        break;
                    case 's':
                        pos++;
                        pos += FindFirstCharacter(src+pos);
                        switch (src[pos])
                        {
                            case '-':
                                selected = -1;
                                pos++;
                                break;
                            case '.':
                                selected = -2;
                                pos++;
                                break;
                            case 's':
                                selected = -3;
                                pos++;
                                break;
                            default:
                                selected = 1<<atoi(src+pos);
                                while (isdigit(src[pos])) pos++;
                                break;
                        }
                        break;
                    case 'e': mode = 1; break;
                    case 'd': mode = 0; break;
                    case 't': mode = -1; break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }
        else
            pos += StrToPath(path,src+pos);

        if (src[pos+1] == '\0')
            break;
    }

    if (!grf->workspaces[grf->current_workspace].exists)
    {
        free(path);
        return;
    }

    if (workspace1 != grf->current_workspace)
        if (chdir(GET_DIR(workspace1,1)->path) != 0)
			return;

    if (selected == -2)
        selected = grf->workspaces[workspace1].sel_group;
    if (toselected == -1)
        toselected = grf->workspaces[workspace2].sel_group;
    if (selected == -3)
    {
        if (!recursive)
        {
            free(path);
            return;
        }

        strcpy(path,MakePath(GET_DIR(workspace1,1)->path,GET_ESELECTED(workspace1,1).name));
        switch (mode)
        {
            case -1: GET_ESELECTED(workspace1,1).list[workspace2] ^= 1<<toselected; break;
            case 0: GET_ESELECTED(workspace1,1).list[workspace2] ^= (1<<toselected)*((GET_ESELECTED(workspace1,1).list[workspace2]&(1<<toselected)) != toselected); break;
            case 1: GET_ESELECTED(workspace1,1).list[workspace2] |= 1<<toselected; break;
        }
    }

    if (path[0])
    {
        char* stemp = (char*)malloc(PATH_MAX);
        realpath(path,stemp);
        free(path);
        path = stemp;
    }

    for (size_t i = 0; i < grf->size; i++)
    {
        if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        grf->base[i]->enable ||
        #endif
        grf->base[i]->size < 1)
            continue;
        if (path[0])
        {
            if (recursive)
            {
                if (!strstr(grf->base[i]->path,path))
                    continue;
            }
            else if (strcmp(grf->base[i]->path,path) != 0)
                continue;
        }

        for (size_t j = 0; j < grf->base[i]->size; j++)
        {
            if (selected < 0 ? 1 : (grf->base[i]->el[j].list[workspace1]&(1<<selected)))
            {
                switch (mode)
                {
                    case -1: grf->base[i]->el[j].list[workspace2] ^= 1<<toselected; break;
                    case 0: grf->base[i]->el[j].list[workspace2] ^= (1<<toselected)*((grf->base[i]->el[j].list[workspace2]&(1<<toselected)) != toselected); break;
                    case 1: grf->base[i]->el[j].list[workspace2] |= 1<<toselected; break;
                }
            }
        }
    }

    free(path);
    if (workspace1 != grf->current_workspace)
        chdir(GET_DIR(grf->current_workspace,1)->path);
}

void ___EXEC(const char* src, Basic* grf)
{
    size_t pos = 0;
    int background = 0;
    char temp[PATH_MAX];

    while (src[pos])
    {
        if (src[pos] == '-')
        {
            do {
                pos++;

                switch (src[pos])
                {
                    case 'b':
                        background = 1;
                        break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }
        else
            pos += StrToPath(temp,src+pos);

        pos += FindFirstCharacter(src+pos);
    }

    spawn(temp,NULL,NULL,background ? F_SILENT : F_NORMAL|F_WAIT);
}

void ___BULK(const char* src, Basic* grf)
{
    size_t pos = 0;
    int workspace = grf->current_workspace, selected = -1;
    uchar flag = 0;
    char** temp = (char**)malloc(6*sizeof(char*));
    for (int i = 0; i < 6; i++)
        temp[i] = (char*)calloc(sizeof(char),PATH_MAX);
    char* path = (char*)calloc(sizeof(char),PATH_MAX);

    while (src[pos])
    {
        if (src[pos] && src[pos] == '-')
        {
            do {
                pos++;
                switch (src[pos])
                {
                    case 'f':
                        flag |= 0x1;
                        break;
                    case 'w':
                        pos++;
                        pos += FindFirstCharacter(src+pos);
                        workspace = atoi(src+pos);
                        while (isdigit(src[pos])) pos++;
                        break;
                    case 's':
                        pos++;
                        pos += FindFirstCharacter(src+pos);
                        if (src[pos] == '-')
                        {
                            selected = -1;
                            pos++;
                        } else if (src[pos] == 's')
                        {
                            selected = grf->workspaces[workspace].sel_group;
                            pos++;
                        }
                        else
                        {
                            selected = 1<<atoi(src+pos);
                            while (isdigit(src[pos])) pos++;
                        }
                        break;
                    case 'R':
                        flag |= 0x2;
                        break;
                    case 'S':
                    case 'E':
                    case 'b':
                    case 'm':
                    case 'e':
                        {
                            int itemp;
                            switch (src[pos])
                            {
                                case 'S': itemp = 1; break;
                                case 'E': itemp = 2; break;
                                case 'b': itemp = 3; break;
                                case 'm': itemp = 4; break;
                                default: itemp = 5; break;
                            }
                            pos++;
                            pos += FindFirstCharacter(src+pos);
                            pos += StrToPath(temp[itemp],src+pos);
                        }
                        break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }
        else
            pos += StrToPath(path,src+pos);
        pos++;
    }

    if (!grf->workspaces[workspace].exists)
        return;

    if (grf->current_workspace != workspace)
        if (chdir(GET_DIR(workspace,1)->path) != 0)
			return;

    if (path[0])
        realpath(path,temp[0]);
    free(path);
    bulk(grf,workspace,selected,temp,flag);
    for (int i = 0; i < 6; i++)
        free(temp[i]);
    free(temp);
    if (grf->current_workspace != workspace)
        chdir(GET_DIR(workspace,1)->path);
}

void ___CONSOLE(const char* src, Basic* grf)
{
    size_t pos = 0;
    static char temp[1024];
    temp[0] = '\0';

    while (src[pos])
    {
        if (src[pos] && src[pos] == '-')
        {
            do {
                pos++;
                switch (src[pos])
                {
                    case 'a':
                        pos++;
                        pos += FindFirstCharacter(src+pos);
                        pos += StrToPath(temp,src+pos);
                        break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }
        pos++;
    }

    if (grf->ConsoleHistory.size == grf->ConsoleHistory.max_size)
    {
        memset(grf->ConsoleHistory.history[0],0,grf->ConsoleHistory.alloc_r-1);
        char* temp = grf->ConsoleHistory.history[0];
        for (size_t i = 0; i < grf->ConsoleHistory.size-1; i++)
            grf->ConsoleHistory.history[i] = grf->ConsoleHistory.history[i+1];
        grf->ConsoleHistory.history[grf->ConsoleHistory.size-1] = temp;
        grf->ConsoleHistory.size--;
    }

    if (grf->ConsoleHistory.size == grf->ConsoleHistory.allocated)
    {
        grf->ConsoleHistory.history = (char**)realloc(grf->ConsoleHistory.history,(grf->ConsoleHistory.allocated += grf->ConsoleHistory.inc_r)*sizeof(char*));
        for (size_t i = grf->ConsoleHistory.allocated-grf->ConsoleHistory.inc_r; i < grf->ConsoleHistory.allocated; i++)
            grf->ConsoleHistory.history[i] = (char*)calloc(sizeof(char),grf->ConsoleHistory.alloc_r);
    }
    grf->ConsoleHistory.size++;

    struct WinArgs args = {stdscr,
    {-1,1},{1,-1},{-1,-1},{-1,-1},
    {-1,-1},{-1,1},{-1,-1},{-1,-1},
    0};
    ConsoleGetLine(grf->win[5],grf,grf->ConsoleHistory.history,grf->ConsoleHistory.size,grf->ConsoleHistory.alloc_r-1,args,":",temp[0] ? temp : NULL);
    RunCommand(grf->ConsoleHistory.history[grf->ConsoleHistory.size-1],grf);
}

void ___SEARCH(char* src, Basic* grf)
{
    if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        GET_DIR(grf->current_workspace,1)->enable ||
        #endif
        GET_DIR(grf->current_workspace,1)->size < 1)
        return;
    size_t pos = 0, mul = 1;
    int selected = -1;
    int action = -1;
    static char temp[1024];

    while (src[pos])
    {
        if (src[pos] && src[pos] == '-')
        {
            do {
                pos++;
                switch (src[pos])
                {
                    case 's':
                        pos++;
                        pos += FindFirstCharacter(src+pos);
                        if (src[pos] == '-')
                        {
                            selected = -1;
                            pos++;
                        }
                        else
                        {
                            selected = 1<<atoi(src+pos);
                            while (isdigit(src[pos])) pos++;
                        }
                        break;
                    case 'b':
                    case 'n':
                        action = (src[pos] == 'n' ? 2 : 1);
                        pos++;
                        pos += FindFirstCharacter(src+pos);
                        mul = atoi(src+pos);
                        while (isdigit(src[pos])) pos++;
                        break;
                    case 'N':
                    case 'E':
                    case 'e':
                        switch (src[pos])
                        {
                            case 'N': action = 3; break;
                            case 'e': action = 4; break;
                            case 'E': action = 5; break;
                        }
                        pos++;
                        pos += FindFirstCharacter(src+pos);
                        pos += StrToPath(temp,src+pos);
                        break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }
        pos++;
    }

    switch (action)
    {
        case 1:
        case 2:
            if (!grf->SearchList.size)
                break;
            for (size_t i = 0; i < mul; i++)
            {
                if (GET_ESELECTED(grf->current_workspace,1).name == grf->SearchList.list[grf->SearchList.pos])
                {
                    if (action == 1)
                    {
                        if (grf->SearchList.pos == 0)
                            grf->SearchList.pos = grf->SearchList.size-1;
                        else
                            grf->SearchList.pos--;
                    }
                    else
                    {
                        if (grf->SearchList.pos == grf->SearchList.size-1)
                            grf->SearchList.pos = 0;
                        else
                            grf->SearchList.pos++;
                    }
                }

                for (size_t j = 0; j < GET_DIR(grf->current_workspace,1)->size; j++)
                {
                    if (GET_DIR(grf->current_workspace,1)->el[j].name == grf->SearchList.list[grf->SearchList.pos])
                    {
                        if (j > GET_SELECTED(grf->current_workspace,1))
                            for (size_t g = GET_SELECTED(grf->current_workspace,1); g < j; g++)
                                MoveD(1,grf,grf->current_workspace,1);
                        else if (j < GET_SELECTED(grf->current_workspace,1))
                            for (size_t g = GET_SELECTED(grf->current_workspace,1); g > j; g--)
                                MoveD(-1,grf,grf->current_workspace,1);
                        break;
                    }
                }
            }
            break;
        case 3:
        case 4:
        case 5:
            free(grf->SearchList.list);
            grf->SearchList.list = NULL;
            grf->SearchList.allocated = 0;
            grf->SearchList.size = 0;
            grf->SearchList.pos = 0;
            int reti;

            for (size_t i = 0; i < GET_DIR(grf->current_workspace,1)->size; i++)
            {
                if (selected == -1 ? 1 : (GET_DIR(grf->current_workspace,1)->el[i].list[grf->current_workspace]&selected))
                {
                    if (action == 4 || action == 5)
                    {
                        regex_t regex;
                        reti = 0;
                        reti = regcomp(&regex,temp,0);
                        if (reti) continue;
                        reti = regexec(&regex,GET_DIR(grf->current_workspace,1)->el[i].name,0,NULL,action == 5 ? REG_EXTENDED : 0);
                        if (reti) continue;
                        regfree(&regex);
                    }
                    else if (!strstr(GET_DIR(grf->current_workspace,1)->el[i].name,temp))
                        continue;

                    if (grf->SearchList.size == grf->SearchList.allocated)
                        grf->SearchList.list = (char**)realloc(grf->SearchList.list,(grf->SearchList.allocated+=grf->SearchList.inc_r)*(sizeof(char*)));
                    grf->SearchList.size++;
                    grf->SearchList.list[grf->SearchList.size-1] = GET_DIR(grf->current_workspace,1)->el[i].name;
                }
            }
            ___SEARCH("-n 1",grf);
            break;
    }

}

void ___SHELL(char* src, Basic* grf)
{
	spawn(settings->shell,"-c",src,F_NORMAL|F_WAIT|F_CONFIRM);

	return;
}

void ___FILTER(char* src, Basic* grf)
{
    if (GET_DIR(grf->current_workspace,1)->oldsize == 0)
        GET_DIR(grf->current_workspace,1)->oldsize = GET_DIR(grf->current_workspace,1)->size;
    else
        GET_DIR(grf->current_workspace,1)->size = GET_DIR(grf->current_workspace,1)->oldsize;

    if (src[0] == '/')
    {
        GET_DIR(grf->current_workspace,1)->filter_set = false;
        return;
    }

    GET_DIR(grf->current_workspace,1)->filter_set = true;
    if (!GET_DIR(grf->current_workspace,1)->filter)
        GET_DIR(grf->current_workspace,1)->filter = (char*)malloc(NAME_MAX);

    strcpy(GET_DIR(grf->current_workspace,1)->filter,src);

    struct Element temp;

    for (size_t i = 0; i < GET_DIR(grf->current_workspace,1)->size; i++)
    {
        if (!strstr(GET_DIR(grf->current_workspace,1)->el[i].name,src))
        {
            temp = GET_DIR(grf->current_workspace,1)->el[i];
            for (size_t j = i; j < GET_DIR(grf->current_workspace,1)->size-1; j++)
                GET_DIR(grf->current_workspace,1)->el[j] = GET_DIR(grf->current_workspace,1)->el[j+1];
            GET_DIR(grf->current_workspace,1)->el[GET_DIR(grf->current_workspace,1)->size-1] = temp;
            --GET_DIR(grf->current_workspace,1)->size;
            i--;
        }
    }

}
