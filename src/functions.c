/*
    csas - console file manager
    Copyright (C) 2020-2021 TUVIMEN <suchora.dominik7@gmail.com>

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

extern Key *keys;
extern size_t keys_t;
extern Settings *cfg;

void change_workspace(Csas *cs, const int num)
{
    register int temp = cs->current_ws;
    cs->current_ws = num;

    if (!cs->ws[num].exists)
    {
        cs->ws[num].exists = 1;
        cs->ws[num].show_message = false;
        cs->ws[num].path = strcpy(malloc(PATH_MAX),G_D(temp,1)->path);
        csas_cd(G_D(temp,1)->path,num,cs);
    }
    else
        csas_cd(G_D(num,1)->path,num,cs);
}

int update_event(Csas *cs)
{
    int event = getch();
    if (event == -1) { return -1; }

    memset(cs->typed_keys,0,63);
    cs->was_typed = true;

    while (event == -1 || event == KEY_RESIZE || (event > 47 && event < 58))
    {
        if (event == KEY_RESIZE) { update_size(cs); csas_draw(cs,-1); }
        if (event > 47 && event < 58) { cs->typed_keys[strlen(cs->typed_keys)] = event; csas_draw(cs,4); }
        event = getch();
    }

    int *passed = NULL;
    size_t passedl = 0;

    if ((int)event == 27) { cs->was_typed = false; return -1; }
    cs->typed_keys[strlen(cs->typed_keys)] = event;

    for (size_t i = 0; i < keys_t; i++)
    {
        if (event == keys[i].keys[0])
        {
            passed = (int*)realloc(passed,(passedl+1)*sizeof(int));
            passedl++;
            passed[passedl-1] = i;
            #ifdef __SHOW_KEY_BINDINGS__
            for (int j = 0; j < cs->wx; j++)
                mvaddch(cs->wy-cfg->Bar2Enable-passedl,j,' ');
            mvprintw(cs->wy-cfg->Bar2Enable-passedl,0," %c\t%s",wctob(keys[i].keys[1]),keys[i].value);
            #endif
        }
    }

    bool starts_string = 0;

    for (int i = 1; passedl > 1; i++)
    {
        csas_draw(cs,-1);
        do {
            if (event == KEY_RESIZE)
            {
                update_size(cs);
                #ifdef __SHOW_KEY_BINDINGS__
                erase();
                refresh();
                #endif
                csas_draw(cs,-1);
                #ifdef __SHOW_KEY_BINDINGS__
                for (size_t j = 0; j < passedl; j++)
                {
                    for (int g = 0; g < cs->wx; g++)
                        mvaddch(cs->wy-1-cfg->Bar2Enable-j,g,' ');
                    mvprintw(cs->wy-1-cfg->Bar2Enable-j,0," %c\t%s",wctob(keys[passed[j]].keys[i]),keys[passed[j]].value);
                }
                #endif
            }
            if (event > 47 && event < 58) { cs->typed_keys[strlen(cs->typed_keys)] = event; csas_draw(cs,4); }
            event = getch();
        } while (event == -1 || event == KEY_RESIZE || (starts_string && event > 47 && event < 58));
        if (event == 27) { cs->was_typed = false; return -1; }
        cs->typed_keys[strlen(cs->typed_keys)] = event;
        starts_string = 1;

        int *tmp_passed = NULL;
        size_t tmp_passedl = 0;

        #ifdef __SHOW_KEY_BINDINGS__
        erase();
        refresh();
        #endif

        for (size_t j = 0; j < passedl; j++)
        {
            if (event == keys[passed[j]].keys[i])
            {
                tmp_passed = (int*)realloc(tmp_passed,(tmp_passedl+1)*sizeof(int));
                tmp_passedl++;
                tmp_passed[tmp_passedl-1] = passed[j];
                #ifdef __SHOW_KEY_BINDINGS__
                for (int g = 0; g < cs->wx; g++)
                    mvaddch(cs->wy-cfg->Bar2Enable-tmp_passedl,g,' ');
                mvprintw(cs->wy-cfg->Bar2Enable-tmp_passedl,0," %c\t%s",wctob(keys[passed[j]].keys[i+1]),keys[passed[j]].value);
                #endif
            }
        }

        free(passed);
        passed = tmp_passed;
        passedl = tmp_passedl;
    }

    cs->was_typed = false;
    #ifdef __SHOW_KEY_BINDINGS__
    erase();
    refresh();
    csas_draw(cs,-1);
    #endif
    return (passedl == 0) ? -1 : passed[0];
}

static void godown(Csas *cs, const int ws, const int which)
{
    if (cs->ws[ws].visual)
    {
        for (size_t i = G_S(ws,which); i < G_D(ws,which)->size; i++)
            G_D(ws,which)->el[i].list[ws] |= 1<<cs->ws[ws].sel_group;
    }
    if (G_D(ws,which)->size > (size_t)cs->win[which]->_maxy-!cfg->Borders+cfg->Borders)
        G_D(ws,which)->ltop[ws] = G_D(ws,which)->size-cs->win[which]->_maxy-!cfg->Borders+cfg->Borders;
    else
        G_D(ws,which)->ltop[ws] = 0;
    G_S(ws,which) = G_D(ws,which)->size-1;
}

static void gotop(Csas *cs, const int ws, const int which)
{
    if (cs->ws[ws].visual)
    {
        for (int i = G_S(ws,which); i > -1; i--)
            G_D(ws,which)->el[i].list[ws] |= 1<<cs->ws[ws].sel_group;
    }
    G_S(ws,which) = 0;
    G_D(ws,which)->ltop[ws] = 0;
}

static void move_d(const char how, Csas *cs, const int ws, const int which)
{
    if (G_S(ws,which) == G_D(ws,which)->size-1 && how == 1)
    {
        if (cfg->WrapScroll) gotop(cs,ws,which);
        return;
    }
    else if (G_S(ws,which) == 0 && how == -1)
    {
        if (cfg->WrapScroll) godown(cs,ws,which);
        return;
    }

    G_S(ws,which) += how;

    if ((how == 1)*((cs->win[which]->_maxy+G_D(ws,which)->ltop[ws]-(cfg->Borders*2)) < G_D(ws,which)->size-1)+(how == -1)*((cs->win[which]->_maxy+G_D(ws,which)->ltop[ws]-(cfg->Borders*2)) > 0) &&
        (how == 1)*(cs->win[which]->_maxy+G_D(ws,which)->ltop[ws]-(cfg->Borders*2) < G_S(ws,which)+(int)(cs->win[which]->_maxy*cfg->MoveOffSet))+(how == -1)*(G_D(ws,which)->ltop[ws] > G_S(ws,which)-(int)(cs->win[which]->_maxy*cfg->MoveOffSet)))
    {
        if (cfg->JumpScroll)
        {
            if ((how == 1)*((cs->win[which]->_maxy+G_D(ws,which)->ltop[ws]-(cfg->Borders*2)+(int)(cs->win[which]->_maxy*cfg->JumpScrollValue)) > G_D(ws,which)->size-1)
                +(how == -1)*(G_D(ws,which)->ltop[ws] > G_S(ws,which)-(int)(cs->win[which]->_maxy*cfg->MoveOffSet)))
                G_D(ws,which)->ltop[ws] = (how == 1)*(G_D(ws,which)->size-cs->win[which]->_maxy-!cfg->Borders+cfg->Borders);
            else
                G_D(ws,which)->ltop[ws] += (int)(how*cs->win[which]->_maxy*cfg->JumpScrollValue);
        }
        else
            G_D(ws,which)->ltop[ws] += how;
    }

    G_ES(ws,which).list[ws] |= (1<<cs->ws[ws].sel_group)*cs->ws[ws].visual;
}

void csas_exit(Csas *cs, const bool force)
{
    if (force)
        goto END;

    int count = 0;
    for (int i = 0; i < WORKSPACE_N; i++)
        count += cs->ws[i].exists;

    cs->ws[cs->current_ws].exists = 0;

    if (count > 1)
    {
        for (int i = cs->current_ws+1; i < WORKSPACE_N; i++)
            if (cs->ws[i].exists)
            {
                change_workspace(cs,i);
                return;
            }
        for (int i = 0; i < cs->current_ws; i++)
            if (cs->ws[i].exists)
            {
                change_workspace(cs,i);
                return;
            }
    }

    END: ;
    cs->exit_time = true;
}

void move_to(Csas *cs, const int ws, const int which, const char *name)
{
    if (strcmp(G_ES(ws,which).name,name) == 0)
        return;

    register size_t i, j;
    register char found = 0;

    for (i = 0; i < G_D(ws,which)->size; i++)
        if (strcmp(G_D(ws,which)->el[i].name,name) == 0)
        {
            found = 1;
            break;
        }
    
    if (!found)
        return;

    if (i > G_S(ws,which))
        for (j = G_S(ws,which); j < i; j++)
            move_d(1,cs,ws,which);
    else if (i < G_S(ws,which))
        for (j = G_S(ws,which); j > i; j--)
            move_d(-1,cs,ws,which);
}

static void bulk(Csas *cs, const int ws, const int selected, char **args, const uchar flag)
{
    char tmpfile[PATH_MAX];
    strcpy(tmpfile,TEMPTEMP);
    int fd = mkstemp(tmpfile);
    FILE *file = fdopen(fd,"w+");
    if (!file)
        return;

    size_t pathl = strlen(args[0]);
    bool comment_write;
    char *temp;

    for (size_t i = 0; i < cs->size; i++)
    {
        if (!(
        #ifdef __THREADS_FOR_DIR_ENABLE__
        !cs->base[i]->enable &&
        #endif
        cs->base[i]->size > 0))
            continue;

        if (pathl)
        {
            if (flag & 0x2)
            {
                if (!strstr(cs->base[i]->path,args[0]))
                    continue;
            }
            else if ((strcmp(cs->base[i]->path,args[0]) != 0))
                continue;
        }

        comment_write = 0;
        for (size_t j = 0; j < cs->base[i]->size; j++)
        {
            if (selected == -1 ? 1 : (cs->base[i]->el[j].list[ws]&selected))
            {
                if (!comment_write)
                {
                    fprintf(file,"//\t%s\n",cs->base[i]->path);
                    comment_write = 1;
                }
                temp = (flag & 0x1) ? mkpath(cs->base[i]->path,cs->base[i]->el[j].name) : cs->base[i]->el[j].name;
                fprintf(file,"%s\n",temp);
            }
        }
    }

    fflush(file);
    spawn(args[2],tmpfile,NULL,F_NORMAL|F_WAIT);

    size_t pos = 0, x;
    rewind(file);

    struct stat sfile;
    fstat(fd,&sfile);
    char *filecopy = (char*)malloc(sfile.st_size+1);
    fread(filecopy,1,sfile.st_size,file);
    
    freopen(tmpfile,"w+",file);
    char buffer[PATH_MAX];
    bool writed = 0;

    fprintf(file,"#!%s\n\n",args[1]);

    if (filecopy[pos])
    {
        for (size_t i = 0; i < cs->size; i++)
        {
            if (!(
            #ifdef __THREADS_FOR_DIR_ENABLE__
            !cs->base[i]->enable &&
            #endif
            cs->base[i]->size > 0))
                continue;

            if (pathl)
            {
                if (flag & 0x2)
                {
                    if (!strstr(cs->base[i]->path,args[0]))
                        continue;
                }
                else if ((strcmp(cs->base[i]->path,args[0]) != 0))
                    continue;
            }

            for (size_t j = 0; j < cs->base[i]->size; j++)
            {
                if (selected == -1 ? 1 : (cs->base[i]->el[j].list[ws]&selected))
                {
                    while (filecopy[pos] == '\n')
                    {
                        pos++;
                        j--;
                    }
                    if (!filecopy[pos])
                        break;

                    if (filecopy[pos] == '/' && filecopy[pos+1] && filecopy[pos+1] == '/')
                    {
                        while (filecopy[pos] && filecopy[pos] != '\n') pos++;
                        j--;
                    }
                    else
                    {
                        x = 0;
                        while (filecopy[pos] && filecopy[pos] != '\n') buffer[x++] = filecopy[pos++];
                        buffer[x] = '\0';
                        temp = (flag & 0x1) ? mkpath(cs->base[i]->path,cs->base[i]->el[j].name) : cs->base[i]->el[j].name;
                        if (x > 0 && strcmp(temp,buffer) != 0)
                        {
                            fprintf(file,"%s ",args[3]);
                            if (!(flag & 0x1))
                                temp = mkpath(cs->base[i]->path,temp);
                            atob(temp);
                            fprintf(file,"%s %s ",temp,args[4]);
                            temp = mkpath(cs->base[i]->path,buffer);
                            atob(temp);
                            fprintf(file,"%s %s\n",temp,args[5]);
                            writed = 1;
                        }
                    }
                    pos++;
                }
            }
        }
    }

    fflush(file);
    free(filecopy);

    if (writed == 0)
        goto END;

    spawn(args[2],tmpfile,NULL,F_NORMAL|F_WAIT);
    spawn(args[1],tmpfile,NULL,F_NORMAL|F_WAIT|F_CONFIRM);

    END:
    fclose(file);
    unlink(tmpfile);
}

void ___SET(const char *src, Csas *cs)
{
    size_t pos = 0;

    struct SetEntry SetEntries[] = {
        {"FileOpener",&cfg->FileOpener},{"BinaryPreview",&cfg->BinaryPreview},
        {"shell",&cfg->shell},{"editor",&cfg->editor},{"Bar1Settings",&cfg->Bar1Settings},
        {"Bar2Settings",&cfg->Bar2Settings},{"C_Borders",&cfg->C_Borders},
        {"UserHostPattern",&cfg->UserHostPattern},{"CopyBufferSize",&cfg->CopyBufferSize},
        {"MoveOffSet",&cfg->MoveOffSet},{"SDelayBetweenFrames",&cfg->SDelayBetweenFrames},
        {"WrapScroll",&cfg->WrapScroll},{"JumpScrollValue",&cfg->JumpScrollValue},
        {"StatusBarOnTop",&cfg->StatusBarOnTop},{"WinSizeMod",&cfg->WinSizeMod},
        {"Win1Enable",&cfg->Win1Enable},{"Win3Enable",&cfg->Win3Enable},{"UserRHost",&cfg->UserRHost},
        {"Bar1Enable",&cfg->Bar1Enable},{"Bar2Enable",&cfg->Bar2Enable},
        {"Borders",&cfg->Borders},{"FillBlankSpace",&cfg->FillBlankSpace},
        {"WindowBorder",&cfg->WindowBorder},{"EnableColor",&cfg->EnableColor},
        {"DelayBetweenFrames",&cfg->DelayBetweenFrames},{"NumberLines",&cfg->NumberLines},
        {"NumberLinesOff",&cfg->NumberLinesOff},{"NumberLinesFromOne",&cfg->NumberLinesFromOne},
        {"DisplayingC",&cfg->DisplayingC},{"JumpScroll",&cfg->JumpScroll},{"Values",&cfg->Values},
        {"DirLoadingMode",&cfg->DirLoadingMode},{"PreviewSettings",&cfg->PreviewSettings},
        #ifdef __SORT_ELEMENTS_ENABLE__
        {"SortMethod",&cfg->SortMethod},{"BetterFiles",&cfg->BetterFiles},
        #endif
        {"DirSizeMethod",&cfg->DirSizeMethod},{"C_Error",&cfg->C_Error},
        #ifdef __COLOR_FILES_BY_EXTENSION__
        {"C_FType_A",&cfg->C_FType_A},{"C_FType_I",&cfg->C_FType_I},{"C_FType_V",&cfg->C_FType_V},
        #endif
        {"C_Selected",&cfg->C_Selected},{"C_Exec_set",&cfg->C_Exec_set},{"C_Exec_col",&cfg->C_Exec_col},
        {"C_Dir",&cfg->C_Dir},{"C_Reg",&cfg->C_Reg},{"C_Fifo",&cfg->C_Fifo},
        {"C_Sock",&cfg->C_Sock},{"C_Dev",&cfg->C_Dev},{"C_BDev",&cfg->C_BDev},
        {"C_SymLink",&cfg->C_SymLink},{"C_FileMissing",&cfg->C_FileMissing},
        {"C_Other",&cfg->C_Other},{"C_User_S_D",&cfg->C_User_S_D},
        {"C_Bar_Dir",&cfg->C_Bar_Dir},{"C_Bar_Name",&cfg->C_Bar_Name},{"C_Bar_WorkSpace",&cfg->C_Bar_WorkSpace},
        {"C_Bar_WorkSpace_Selected",&cfg->C_Bar_WorkSpace_Selected},{"C_Group",&cfg->C_Group},
        {"C_Bar_F",&cfg->C_Bar_F},{"C_Bar_E",&cfg->C_Bar_E},
        #ifdef __THREADS_FOR_DIR_ENABLE__
        {"ThreadsForDir",&cfg->ThreadsForDir},
        #endif
        #ifdef __THREADS_FOR_FILE_ENABLE__
        {"ThreadsForFile",&cfg->ThreadsForFile},
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
    pos += findfirst(src+pos,isspace);

    pos += atov(SetEntries[gga].value,src+pos);
}

#ifdef __LOAD_CONFIG_ENABLE__
void ___INCLUDE(const char *src, Csas *cs)
{
    char temp[8192];

    atop(temp,src);
    config_load(temp,cs);
}
#endif

void ___MAP(const char *src, Csas *cs)
{
    size_t pos = 0, end = 0;
    char temp1[64];
    char temp2[PATH_MAX];
    while (src[pos+end] && !isspace(src[pos+end])) end++;
    strncpy(temp1,src+pos,end);
    temp1[end] = '\0';
    pos += end;

    pos += findfirst(src+pos,isspace);

    pos += atop(temp2,src+pos);

    addkey(temp1,temp2);
}

void ___MOVE(const char *src, Csas *cs)
{
    size_t pos = 0;
    char rot = -1;
    int ws = cs->current_ws, mul1 = 1, mul2 = 0;

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
                            pos += findfirst(src+pos,isspace);
                            if (ctemp == 'c')
                                mul1 = atoi(src+pos);
                            else
                                ws = atoi(src+pos);
                            while (isdigit(src[pos])) pos++;
                        }
                        break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }
        pos++;
    }

    if (!cs->ws[ws].exists)
        return;

    switch (rot)
    {
        case -1:
        case 1:
            if (
                #ifdef __THREADS_FOR_DIR_ENABLE__
                !G_D(ws,1)->enable &&
                #endif
                G_D(ws,1)->size > 0)
            {
                mul2 = atoi(cs->typed_keys);
                for (register int i = 0; i < (mul2+(mul2 == 0))*mul1; i++)
                    move_d(rot,cs,ws,1);
                if (cfg->Win3Enable)
                    get_preview(cs);
            }
            break;
        case 3:
            if (cs->ws[ws].path[0] == '/' && cs->ws[cs->current_ws].path[1] == '\0')
                break;
            csas_cd("..",ws,cs);
            cs->ws[ws].visual = 0;
            break;
        case 4:
            if (G_D(ws,1)->size > 0)
            {
                switch (G_ES(ws,1).type&T_GT)
                {
                    case T_DIR:
                        csas_cd(G_ES(ws,1).name,ws,cs);
                        cs->ws[ws].visual = 0;
                        break;
                    case T_REG:
                        file_run(G_ES(ws,1).name);
                        break;
                }
            }
            break;
    }
}

void ___QUIT(const char *src, Csas *cs)
{
    bool force = false;
    if (src[0] && src[0] == '-' && src[1] && src[1] == 'f')
        force = true;
    csas_exit(cs,force);
}

void ___CD(const char *src, Csas *cs)
{
    size_t pos = 0;
    int ws = cs->current_ws;
    char path[8192];
    memset(path,0,8191);

    while (src[pos])
    {
        if (src[pos] == '-' && src[pos+1] && src[pos+1] == 'w')
        {
            pos += 2;
            pos += findfirst(src+pos,isspace);
            ws = atoi(src+pos);
            while (isdigit(src[pos])) pos++;
        }
        else
            pos += atop(path,src+pos);

        pos += findfirst(src+pos,isspace);
    }

    cs->ws[ws].exists = true;
    change_workspace(cs,ws);

    csas_cd(path,ws,cs);
}

void ___GOTOP(const char *src, Csas *cs)
{
    size_t pos = 0, target = atol(cs->typed_keys);
    int ws = cs->current_ws;

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
                        pos += findfirst(src+pos,isspace);
                        if (src[pos] == 't')
                            target = atol(src+pos);
                        else
                            ws = atoi(src+pos);
                        while (isdigit(src[pos])) pos++;
                        break;
                }
            } while (src[pos] && !isspace(src[pos]));
        }

        pos += findfirst(src+pos,isspace);
        if (src[pos+1] == '\0')
            break;
    }

    if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        !G_D(ws,1)->enable &&
        #endif
        G_D(ws,1)->size > 0)
    {
        if (target == 0)
            gotop(cs,ws,1);
        else if (target > G_S(ws,1))
        {
            for (size_t i = G_S(ws,1); i < target-1; i++)
                move_d(1,cs,ws,1);
        }
        else if (target < G_S(ws,1))
        {
            for (size_t i = G_S(ws,1); i > target-1; i--)
                move_d(-1,cs,ws,1);
        }
        if (ws == cs->current_ws && cfg->Win3Enable)
            get_preview(cs);
    }
}

void ___GODOWN(const char *src, Csas *cs)
{
    size_t pos = 0, target = atol(cs->typed_keys);
    int ws = cs->current_ws;

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
                        pos += findfirst(src+pos,isspace);
                        if (src[pos] == 't')
                            target = atol(src+pos);
                        else
                            ws = atoi(src+pos);
                        while (isdigit(src[pos])) pos++;
                        break;
                }
            } while (src[pos] && !isspace(src[pos]));
        }

        pos += findfirst(src+pos,isspace);
        if (src[pos+1] == '\0') break;
    }

    if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        !G_D(ws,1)->enable &&
        #endif
        G_D(ws,1)->size > 0)
    {
        if (target == 0)
            godown(cs,target,1);
        else if (target > G_S(ws,1))
        {
            for (size_t i = G_S(ws,1); i < target-1; i++)
                move_d(1,cs,ws,1);
        }
        else if (target < G_S(ws,1))
        {
            for (size_t i = G_S(ws,1); i > target-1; i--)
                move_d(-1,cs,ws,1);
        }
        if (ws == cs->current_ws && cfg->Win3Enable)
            get_preview(cs);
    }
}

void ___CHANGEWORKSPACE(const char *src, Csas *cs)
{
    change_workspace(cs,atoi(src));
}

#ifdef __FILE_SIZE_ENABLE__

static void GETSIZE(struct Element *el, const int fd, const uchar flag)
{
    if ((el->type&T_GT) == T_DIR)
    {
        int tfd;
        if ((tfd = openat(fd,el->name,O_DIRECTORY)) != -1)
        {
            if ((flag&0x8) != 0x8)
            {
                ull size = 0, count = 0;
                get_dirsize(tfd,&count,&size,(flag&0x2)==0x2);
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

void ___GETSIZE(const char *src, Csas *cs)
{
    size_t pos = 0;
    uchar flag = 0;
    int selected = -1, ws = cs->current_ws;
    char *path = (char*)calloc(sizeof(char),PATH_MAX);

    while (src[pos])
    {
        pos += findfirst(src+pos,isspace);

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
                            pos += findfirst(src+pos,isspace);
                            if (itemp)
                                ws = atoi(src+pos);
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
            pos += atop(path,src+pos);

        if (src[pos+1] == '\0')
            break;
    }

    if (!cs->ws[cs->current_ws].exists)
    {
        free(path);
        return;
    }

    if (ws != cs->current_ws)
		if (chdir(G_D(ws,1)->path) != 0)
			return;

    if (selected == -2)
        selected = cs->ws[ws].sel_group;
    if (selected == -3)
    {
        int fd;
        if (
            #ifdef __THREADS_FOR_DIR_ENABLE__
            !G_D(ws,1)->enable &&
            #endif
            G_D(ws,1)->size > 0 && (fd = open(G_D(ws,1)->path,O_DIRECTORY)) != -1)
        GETSIZE(&G_ES(ws,1),fd,flag);
        if (!(flag&0x1))
        {
            free(path);
            return;
        }
        else
            strcpy(path,mkpath(G_D(ws,1)->path,G_ES(ws,1).name));
    }

    if (path[0])
    {
        char *stemp = (char*)malloc(PATH_MAX);
        realpath(path,stemp);
        free(path);
        path = stemp;
    }

    int temp;

    for (size_t i = 0; i < cs->size; i++)
    {
        if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        cs->base[i]->enable ||
        #endif
        cs->base[i]->size < 1)
            continue;
        if (path[0])
        {
            if (flag&0x1)
            {
                if (!strstr(cs->base[i]->path,path))
                    continue;
            }
            else if (strcmp(cs->base[i]->path,path) != 0)
                continue;
        }

        if ((temp = open(cs->base[i]->path,O_DIRECTORY)) == -1)
            continue;

        for (size_t j = 0; j < cs->base[i]->size; j++)
            if (selected < 0 ? 1 : (cs->base[i]->el[j].list[ws]&(1<<selected)))
                GETSIZE(&cs->base[i]->el[j],temp,flag);
        close(temp);
    }

    free(path);
    if (ws != cs->current_ws)
        chdir(G_D(cs->current_ws,1)->path);
}

#endif

void ___SETGROUP(const char *src, Csas *cs)
{
    cs->ws[cs->current_ws].sel_group = atoi(src);
    if (cs->ws[cs->current_ws].visual && G_D(cs->current_ws,1)->size > 0)
        G_ES(cs->current_ws,1).list[cs->current_ws] |= 1<<cs->ws[cs->current_ws].sel_group;
}

void ___FASTSELECT(const char *src, Csas *cs)
{
    if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        !G_D(cs->current_ws,1)->enable &&
        #endif
        G_D(cs->current_ws,1)->size > 0)
    {
        G_ES(cs->current_ws,1).list[cs->current_ws] ^= 1<<cs->ws[cs->current_ws].sel_group;
        move_d(1,cs,cs->current_ws,1);
        if (cfg->Win3Enable)
            get_preview(cs);
    }
}

void ___TOGGLEVISUAL(const char *src, Csas *cs)
{
    cs->ws[cs->current_ws].visual = !cs->ws[cs->current_ws].visual;
    if (cs->ws[cs->current_ws].visual && G_D(cs->current_ws,1)->size > 0)
        G_ES(cs->current_ws,1).list[cs->current_ws] |= 1<<cs->ws[cs->current_ws].sel_group;
}

void ___F_MOD(const char *src, Csas *cs)
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
    pos += findfirst(src+pos,isspace);

    int selected = -1, ws = cs->current_ws;
    char *path = (char*)calloc(sizeof(char),PATH_MAX);
    char *target = (char*)calloc(sizeof(char),PATH_MAX);
    int fd1, fd2, fd3;
    char *buffer = Action == 1 ? NULL : (char*)malloc(cfg->CopyBufferSize);

    while (src[pos])
    {
        pos += findfirst(src+pos,isspace);

        if (src[pos] == '-')
        {
            do {
                pos++;
                switch (src[pos])
                {
                    case 'o':
                        pos++;
                        pos += findfirst(src+pos,isspace);
                        pos += atop(target,src+pos);
                        break;
                    case 'w':
                    case 's':
                        {
                            int itemp = (src[pos] == 'w' ? 1 : 0);
                            pos++;
                            pos += findfirst(src+pos,isspace);
                            if (itemp)
                                ws = atoi(src+pos);
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
            pos += atop(path,src+pos);

        if (src[pos+1] == '\0')
            break;
    }

    if (!cs->ws[cs->current_ws].exists)
        goto END;

    if (ws != cs->current_ws)
        if (chdir(G_D(ws,1)->path) != 0)
			return;

    if (selected == -2)
        selected = cs->ws[ws].sel_group;

    if (!target[0])
        strcpy(target,".");

    if ((fd1 = open(target,O_DIRECTORY)) == -1)
        goto END;

    char *ActionName = NULL;
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
            !G_D(ws,1)->enable &&
            #endif
            G_D(ws,1)->size > 0 && (fd2 = open(G_D(ws,1)->path,O_DIRECTORY)) != -1)
        {
            if (fstatat(fd2,G_ES(ws,1).name,&ST,AT_SYMLINK_NOFOLLOW) == -1)
                goto END2;

            if ((ST.st_mode&S_IFMT) == S_IFDIR)
            {
				if ((fd3 = openat(fd2,G_ES(ws,1).name,O_RDONLY)) == -1)
					goto END2;
                get_dirsize(fd3,&count,&size,true);
				close(fd3);
                count++;
                size += ST.st_size;
                set_message(cs,0,"Do you want to %s %ld files(%s)? (Y/n)",ActionName,count,stoa(size));
            }
            else
                #ifdef __FILE_SIZE_ENABLE__
                set_message(cs,0,"Do you want to %s \"%s\" (%s)? (Y/n)",ActionName,G_ES(ws,1).name,stoa(G_ES(ws,1).size));
                #else
                set_message(cs,0,"Do you want to %s \"%s\"? (Y/n)",ActionName,G_ES(ws,1).name);
                #endif

            wrefresh(cs->win[5]);
            int si = -1;
            for (;;)
            {
                si = getch();
                if (si == KEY_RESIZE)
                {
                    update_size(cs);
                    csas_draw(cs,-1);
                    if ((ST.st_mode&S_IFMT) == S_IFDIR)
                        set_message(cs,0,"Do you want to %s %ld files(%s)? (Y/n)",ActionName,count,stoa(size));
                    else
                        #ifdef __FILE_SIZE_ENABLE__
                        set_message(cs,0,"Do you want to %s \"%s\" (%s)? (Y/n)",ActionName,G_ES(ws,1).name,stoa(G_ES(ws,1).size));
                        #else
                        set_message(cs,0,"Do you want to %s \"%s\"? (Y/n)",ActionName,G_ES(ws,1).name);
                        #endif
                    wrefresh(cs->win[5]);
                }
                else if (si == 'y' || si == 'Y')
                    break;
                else if (si != -1)
                    goto END2;
            }

            switch (Action)
            {
                case 1: file_rm(fd2,G_ES(ws,1).name); break;
                case 2: file_cp(fd1,fd2,G_ES(ws,1).name,buffer,arg); break;
                case 3: file_mv(fd1,fd2,G_ES(ws,1).name,buffer,arg); break;
            }
            END2: ;
            close(fd2);
        }
        close(fd1);
        goto END;
    }

    if (path[0])
    {
        char *stemp = (char*)malloc(PATH_MAX);
        realpath(path,stemp);
        free(path);
        path = stemp;
    }

    for (size_t i = 0; i < cs->size; i++)
    {
        if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        cs->base[i]->enable ||
        #endif
        cs->base[i]->size < 1)
            continue;
        if (path[0] && strcmp(cs->base[i]->path,path) != 0)
                continue;

        if ((fd2 = open(cs->base[i]->path,O_DIRECTORY)) == -1)
            continue;

        for (size_t j = 0; j < cs->base[i]->size; j++)
        {
            if (selected < 0 ? 1 : (cs->base[i]->el[j].list[ws]&(1<<selected)))
            {
                if (fstatat(fd2,cs->base[i]->el[j].name,&ST,AT_SYMLINK_NOFOLLOW) == -1)
                    continue;
                count++;
                size += ST.st_size;
                if ((ST.st_mode&S_IFMT) == S_IFDIR)
				{
					if ((fd3 = openat(fd2,cs->base[i]->el[j].name,O_RDONLY)) != -1)
					{
						get_dirsize(fd3,&count,&size,true);
						close(fd3);
					}
				}
            }
        }
        close(fd2);
    }

    set_message(cs,0,"Do you want to %s %ld files(%s)? (Y/n)",ActionName,count,stoa(size));
    wrefresh(cs->win[5]);
    int si = -1;
    for (;;)
    {
        si = getch();
        if (si == KEY_RESIZE)
        {
            update_size(cs);
            csas_draw(cs,-1);
            set_message(cs,0,"Do you want to %s %ld files(%s)? (Y/n)",ActionName,count,stoa(size));
            wrefresh(cs->win[5]);
        }
        else if (si == 'y' || si == 'Y')
            break;
        else if (si != -1)
            goto END;
    }

    for (size_t i = 0; i < cs->size; i++)
    {
        if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        cs->base[i]->enable ||
        #endif
        cs->base[i]->size < 1)
            continue;
        if (path[0] && strcmp(cs->base[i]->path,path) != 0)
                continue;

        if ((fd2 = open(cs->base[i]->path,O_DIRECTORY)) == -1)
            continue;

        for (size_t j = 0; j < cs->base[i]->size; j++)
        {
            if (selected < 0 ? 1 : (cs->base[i]->el[j].list[ws]&(1<<selected)))
            {
                switch (Action)
                {
                    case 1: file_rm(fd2,cs->base[i]->el[j].name); break;
                    case 2: file_cp(fd1,fd2,cs->base[i]->el[j].name,buffer,arg); break;
                    case 3: file_mv(fd1,fd2,cs->base[i]->el[j].name,buffer,arg); break;
                }
            }
        }
        close(fd2);
    }
    close(fd1);

    update_size(cs);
    csas_cd(".",cs->current_ws,cs);

    END: ;
    cs->ws[cs->current_ws].show_message = false;
    free(buffer);
    free(path);
    free(target);
    if (ws != cs->current_ws)
        chdir(G_D(cs->current_ws,1)->path);
}

static void GETDIR(char *path, Csas *cs, uchar mode
#ifdef __THREADS_FOR_DIR_ENABLE__
, bool threads
#endif
)
{
    DIR *d;
    if ((d = opendir(path)) == NULL)
        return;
    struct dirent *dir;
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
            getdir(path1,cs,cs->current_ws,1,mode
            #ifdef __FOLLOW_PARENT_DIR__
            ,NULL
            #endif
            #ifdef __THREADS_FOR_DIR_ENABLE__
            ,threads
            #endif
            );
            GETDIR(path1,cs,mode
            #ifdef __THREADS_FOR_DIR_ENABLE__
            ,threads
            #endif
            );
       }
    }
    closedir(d);
}

void ___LOAD(const char *src, Csas *cs)
{
    size_t pos = 0;
    int mode = cfg->DirLoadingMode;
    uchar flag = 0;
	werase(cs->win[0]);
	if (cfg->Win1Enable)
		werase(cs->win[1]);
	if (cfg->Win3Enable)
		werase(cs->win[2]);

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
                        pos += findfirst(src+pos,isspace);
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
            pos += findfirst(src+pos,isspace);
    }

    getdir(".",cs,cs->current_ws,1,mode
    #ifdef __FOLLOW_PARENT_DIR__
    ,NULL
    #endif
    #ifdef __THREADS_FOR_DIR_ENABLE__
    ,(flag&0x1)
    #endif
    );
    if (flag&0x2)
    {
        char path[PATH_MAX];
        strcpy(path,".");
        GETDIR(path,cs,mode
        #ifdef __THREADS_FOR_DIR_ENABLE__
        ,(flag&0x1)
        #endif
        );
    }
}

void ___SELECT(const char *src, Csas *cs)
{
    size_t pos = 0;
    int mode = 1;
    bool recursive = 0;
    int selected = -1, workspace1 = cs->current_ws, workspace2 = cs->current_ws, toselected = -1;
    char *path = (char*)calloc(sizeof(char),PATH_MAX);

    while (src[pos])
    {
        pos += findfirst(src+pos,isspace);

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
                        pos+= findfirst(src+pos,isspace);
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
                            pos += findfirst(src+pos,isspace);
                            if (itemp)
                                workspace1 = atoi(src+pos);
                            else
                                workspace2 = atoi(src+pos);
                            while (isdigit(src[pos])) pos++;
                        }
                        break;
                    case 's':
                        pos++;
                        pos += findfirst(src+pos,isspace);
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
            pos += atop(path,src+pos);

        if (src[pos+1] == '\0')
            break;
    }

    if (!cs->ws[cs->current_ws].exists)
    {
        free(path);
        return;
    }

    if (workspace1 != cs->current_ws)
        if (chdir(G_D(workspace1,1)->path) != 0)
			return;

    if (selected == -2)
        selected = cs->ws[workspace1].sel_group;
    if (toselected == -1)
        toselected = cs->ws[workspace2].sel_group;
    if (selected == -3)
    {
        if (!recursive)
        {
            free(path);
            return;
        }

        strcpy(path,mkpath(G_D(workspace1,1)->path,G_ES(workspace1,1).name));
        switch (mode)
        {
            case -1: G_ES(workspace1,1).list[workspace2] ^= 1<<toselected; break;
            case 0: G_ES(workspace1,1).list[workspace2] &= ~(1<<toselected); break;
            case 1: G_ES(workspace1,1).list[workspace2] |= 1<<toselected; break;
        }
    }

    if (path[0])
    {
        char *stemp = (char*)malloc(PATH_MAX);
        realpath(path,stemp);
        free(path);
        path = stemp;
    }

    for (size_t i = 0; i < cs->size; i++)
    {
        if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        cs->base[i]->enable ||
        #endif
        cs->base[i]->size < 1)
            continue;
        if (path[0])
        {
            if (recursive)
            {
                if (!strstr(cs->base[i]->path,path))
                    continue;
            }
            else if (strcmp(cs->base[i]->path,path) != 0)
                continue;
        }

        for (size_t j = 0; j < cs->base[i]->size; j++)
        {
            if (selected < 0 ? 1 : (cs->base[i]->el[j].list[workspace1]&(1<<selected)))
            {
                switch (mode)
                {
                    case -1: cs->base[i]->el[j].list[workspace2] ^= 1<<toselected; break;
                    case 0: cs->base[i]->el[j].list[workspace2] &= ~(1<<toselected); break;
                    case 1: cs->base[i]->el[j].list[workspace2] |= 1<<toselected; break;
                }
            }
        }
    }

    free(path);
    if (workspace1 != cs->current_ws)
        chdir(G_D(cs->current_ws,1)->path);
}

void ___EXEC(const char *src, Csas *cs)
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
            pos += atop(temp,src+pos);

        pos += findfirst(src+pos,isspace);
    }

    spawn(temp,NULL,NULL,background ? F_SILENT : F_NORMAL|F_WAIT);
}

void ___BULK(const char *src, Csas *cs)
{
    size_t pos = 0;
    int ws = cs->current_ws, selected = -1;
    uchar flag = 0;
    char *temp[6];
    for (int i = 0; i < 6; i++)
        temp[i] = (char*)calloc(sizeof(char),PATH_MAX);
    char path[PATH_MAX] = {0};

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
                        pos += findfirst(src+pos,isspace);
                        ws = atoi(src+pos);
                        while (isdigit(src[pos])) pos++;
                        break;
                    case 's':
                        pos++;
                        pos += findfirst(src+pos,isspace);
                        if (src[pos] == '-')
                        {
                            selected = -1;
                            pos++;
                        } else if (src[pos] == 's')
                        {
                            selected = cs->ws[ws].sel_group;
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
                            pos += findfirst(src+pos,isspace);
                            pos += atop(temp[itemp],src+pos);
                        }
                        break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }
        else
            pos += atop(path,src+pos);
        pos++;
    }

    if (!cs->ws[ws].exists)
        return;

    if (cs->current_ws != ws)
        if (chdir(G_D(ws,1)->path) != 0)
			return;

    if (path[0])
        realpath(path,temp[0]);
    bulk(cs,ws,selected,temp,flag);
    for (int i = 0; i < 6; i++)
        free(temp[i]);
    if (cs->current_ws != ws)
        chdir(G_D(ws,1)->path);
}

void ___CONSOLE(const char *src, Csas *cs)
{
    size_t pos = 0;
    char temp[1024];
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
                        pos += findfirst(src+pos,isspace);
                        pos += atop(temp,src+pos);
                        break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }
        pos++;
    }

    if (cs->consolehistory.size == cs->consolehistory.max_size)
    {
        memset(cs->consolehistory.history[0],0,cs->consolehistory.alloc_r-1);
        char *temp = cs->consolehistory.history[0];
        for (size_t i = 0; i < cs->consolehistory.size-1; i++)
            cs->consolehistory.history[i] = cs->consolehistory.history[i+1];
        cs->consolehistory.history[cs->consolehistory.size-1] = temp;
        cs->consolehistory.size--;
    }

    if (cs->consolehistory.size == cs->consolehistory.allocated)
    {
        cs->consolehistory.history = (char**)realloc(cs->consolehistory.history,(cs->consolehistory.allocated += cs->consolehistory.inc_r)*sizeof(char*));
        for (size_t i = cs->consolehistory.allocated-cs->consolehistory.inc_r; i < cs->consolehistory.allocated; i++)
            cs->consolehistory.history[i] = (char*)calloc(sizeof(char),cs->consolehistory.alloc_r);
    }
    cs->consolehistory.size++;

    struct WinArgs args = {stdscr,
    {-1,1},{1,-1},{-1,-1},{-1,-1},
    {-1,-1},{-1,1},{-1,-1},{-1,-1},
    0};
    console_getline(cs->win[5],cs,cs->consolehistory.history,cs->consolehistory.size,cs->consolehistory.alloc_r-1,args,":",temp[0] ? temp : NULL);
    command_run(cs->consolehistory.history[cs->consolehistory.size-1],cs);
}

void ___SEARCH(char *src, Csas *cs)
{
    if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        G_D(cs->current_ws,1)->enable ||
        #endif
        G_D(cs->current_ws,1)->size < 1)
        return;
    size_t pos = 0, mul = 1;
    int selected = -1;
    int action = -1;
    char temp[1024];

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
                        pos += findfirst(src+pos,isspace);
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
                        pos += findfirst(src+pos,isspace);
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
                        pos += findfirst(src+pos,isspace);
                        pos += atop(temp,src+pos);
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
            if (!cs->SearchList.size)
                break;
            for (size_t i = 0; i < mul; i++)
            {
                if (G_ES(cs->current_ws,1).name == cs->SearchList.list[cs->SearchList.pos])
                {
                    if (action == 1)
                    {
                        if (cs->SearchList.pos == 0)
                            cs->SearchList.pos = cs->SearchList.size-1;
                        else
                            cs->SearchList.pos--;
                    }
                    else
                    {
                        if (cs->SearchList.pos == cs->SearchList.size-1)
                            cs->SearchList.pos = 0;
                        else
                            cs->SearchList.pos++;
                    }
                }

                for (size_t j = 0; j < G_D(cs->current_ws,1)->size; j++)
                {
                    if (G_D(cs->current_ws,1)->el[j].name == cs->SearchList.list[cs->SearchList.pos])
                    {
                        if (j > G_S(cs->current_ws,1))
                            for (size_t g = G_S(cs->current_ws,1); g < j; g++)
                                move_d(1,cs,cs->current_ws,1);
                        else if (j < G_S(cs->current_ws,1))
                            for (size_t g = G_S(cs->current_ws,1); g > j; g--)
                                move_d(-1,cs,cs->current_ws,1);
                        break;
                    }
                }
            }
            break;
        case 3:
        case 4:
        case 5:
            free(cs->SearchList.list);
            cs->SearchList.list = NULL;
            cs->SearchList.allocated = 0;
            cs->SearchList.size = 0;
            cs->SearchList.pos = 0;
            int reti;

            for (size_t i = 0; i < G_D(cs->current_ws,1)->size; i++)
            {
                if (selected == -1 ? 1 : (G_D(cs->current_ws,1)->el[i].list[cs->current_ws]&selected))
                {
                    if (action == 4 || action == 5)
                    {
                        regex_t regex;
                        reti = 0;
                        reti = regcomp(&regex,temp,0);
                        if (reti) continue;
                        reti = regexec(&regex,G_D(cs->current_ws,1)->el[i].name,0,NULL,action == 5 ? REG_EXTENDED : 0);
                        if (reti) continue;
                        regfree(&regex);
                    }
                    else if (!strstr(G_D(cs->current_ws,1)->el[i].name,temp))
                        continue;

                    if (cs->SearchList.size == cs->SearchList.allocated)
                        cs->SearchList.list = (char**)realloc(cs->SearchList.list,(cs->SearchList.allocated+=cs->SearchList.inc_r)*(sizeof(char*)));
                    cs->SearchList.size++;
                    cs->SearchList.list[cs->SearchList.size-1] = G_D(cs->current_ws,1)->el[i].name;
                }
            }
            ___SEARCH("-n 1",cs);
            break;
    }

}

void ___SHELL(char *src, Csas *cs)
{
	spawn(cfg->shell,"-c",src,F_NORMAL|F_WAIT|F_CONFIRM);

	return;
}

void ___FILTER(char *src, Csas *cs)
{
    if (G_D(cs->current_ws,1)->oldsize == 0)
        G_D(cs->current_ws,1)->oldsize = G_D(cs->current_ws,1)->size;
    else
        G_D(cs->current_ws,1)->size = G_D(cs->current_ws,1)->oldsize;

    if (src[0] == '/')
    {
        G_D(cs->current_ws,1)->filter_set = false;
        return;
    }

    G_D(cs->current_ws,1)->filter_set = true;
    if (!G_D(cs->current_ws,1)->filter)
        G_D(cs->current_ws,1)->filter = (char*)malloc(NAME_MAX);

    strcpy(G_D(cs->current_ws,1)->filter,src);

    struct Element temp;

    for (size_t i = 0; i < G_D(cs->current_ws,1)->size; i++)
    {
        if (!strstr(G_D(cs->current_ws,1)->el[i].name,src))
        {
            temp = G_D(cs->current_ws,1)->el[i];
            for (size_t j = i; j < G_D(cs->current_ws,1)->size-1; j++)
                G_D(cs->current_ws,1)->el[j] = G_D(cs->current_ws,1)->el[j+1];
            G_D(cs->current_ws,1)->el[G_D(cs->current_ws,1)->size-1] = temp;
            --G_D(cs->current_ws,1)->size;
            i--;
        }
    }

}
