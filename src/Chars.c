/*
    csas - terminal file manager
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
#include "Draw.h"
#include "Load.h"
#include "FastRun.h"
#include "Usefull.h"
#include "Functions.h"
#include "Console.h"
#include "Loading.h"
#include "Bulk.h"

extern Key* keys;
extern size_t keys_t;
extern Settings* settings;

void ChangeWorkSpace(Basic* grf, const int num)
{
    register int temp = grf->inW;
    grf->inW = num;

    if (!grf->Work[num].exists)
    {
        grf->Work[num].exists = 1;
        CD(GET_DIR(temp,1).path,num,grf);
    }
    else
        CD(GET_DIR(num,1).path,num,grf);
}

int UpdateEvent(Basic* grf)
{
    memset(grf->cSF,0,63);
    int Event = getch();
    if (Event == -1) { return -1; }

    grf->cSF_E = true;

    while (Event == -1 || Event == 410 || (Event > 47 && Event < 58))
    {
        if (Event == KEY_RESIZE) { UpdateSizeBasic(grf); DrawBasic(grf,-1); }
        if (Event > 47 && Event < 58) { grf->cSF[strlen(grf->cSF)] = Event; DrawBasic(grf,4); }
        Event = getch();
    }

    int* theyPass = NULL;
    size_t theyPass_t = 0;

    if ((int)Event == 27) { grf->cSF_E = false; return -1; }
    grf->cSF[strlen(grf->cSF)] = Event;

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
            if (Event > 47 && Event < 58) { grf->cSF[strlen(grf->cSF)] = Event; DrawBasic(grf,4); }
            Event = getch();
        } while (Event == -1 || Event == KEY_RESIZE || (StartsTheString && Event > 47 && Event < 58));
        if (Event == 27) { grf->cSF_E = false; return -1; }
        grf->cSF[strlen(grf->cSF)] = Event;
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

    grf->cSF_E = false;
    return (theyPass_t != 0) ? theyPass[0] : -1;
}

static void GoDown(Basic* grf, const int workspace)
{
    if (grf->Work[workspace].Visual)
    {
        for (long long int i = GET_SELECTED(workspace,1); i < GET_DIR(workspace,1).El_t; i++)
            GET_DIR(workspace,1).El[i].List[workspace] |= grf->Work[workspace].SelectedGroup;
    }
    if (GET_DIR(workspace,1).El_t > (long long int)grf->win[1]->_maxy-!settings->Borders+settings->Borders)
        GET_DIR(workspace,1).Ltop[workspace] = GET_DIR(workspace,1).El_t-grf->win[1]->_maxy-!settings->Borders+settings->Borders;
    else
        GET_DIR(workspace,1).Ltop[workspace] = 0;
    GET_SELECTED(workspace,1) = GET_DIR(workspace,1).El_t-1;
}

static void GoTop(Basic* grf, const int workspace)
{
    if (grf->Work[workspace].Visual)
    {
        for (int i = GET_SELECTED(workspace,1); i > -1; i--)
            GET_DIR(workspace,1).El[i].List[workspace] |= grf->Work[workspace].SelectedGroup;
    }
    GET_SELECTED(workspace,1) = 0;
    GET_DIR(workspace,1).Ltop[workspace] = 0;
}

static void MoveD(const int how, Basic* grf, const int workspace)
{
    if (how == 1) //down
    {
        if (settings->WrapScroll)
        {
            if ((long long int)GET_SELECTED(workspace,1) == GET_DIR(workspace,1).El_t-1)
            {
                GoTop(grf,workspace);
                return;
            }
        }
        if (GET_DIR(workspace,1).El_t-1 > (long long int)GET_SELECTED(workspace,1))
            GET_SELECTED(workspace,1)++;
        
        if ((long long int)(grf->win[1]->_maxy+GET_DIR(workspace,1).Ltop[workspace]-(settings->Borders*2)) < GET_DIR(workspace,1).El_t-1 &&
            grf->win[1]->_maxy+GET_DIR(workspace,1).Ltop[workspace]-(settings->Borders*2) < GET_SELECTED(workspace,1)+(int)(grf->win[1]->_maxy*settings->MoveOffSet))
        {
            if (settings->JumpScroll)
            {
                if ((long long int)(grf->win[1]->_maxy+GET_DIR(workspace,1).Ltop[workspace]-(settings->Borders*2)+(int)(grf->win[1]->_maxy*settings->JumpScrollValue)) > GET_DIR(workspace,1).El_t-1)
                    GET_DIR(workspace,1).Ltop[workspace] = GET_DIR(workspace,1).El_t-grf->win[1]->_maxy-!settings->Borders+settings->Borders;
                else
                    GET_DIR(workspace,1).Ltop[workspace] += (int)(grf->win[1]->_maxy*settings->JumpScrollValue);
            }
            else
                GET_DIR(workspace,1).Ltop[workspace]++;
        }
    }
    if (how == 2) //up
    {
        if (settings->WrapScroll)
        {
            if (GET_SELECTED(workspace,1) == 0)
            {
                GoDown(grf,workspace);
                return;
            }
        }
        if (0 < GET_SELECTED(workspace,1))
            GET_SELECTED(workspace,1)--;
        if (grf->win[1]->_maxy+GET_DIR(workspace,1).Ltop[workspace]-(settings->Borders*2) > 0 &&
            GET_DIR(workspace,1).Ltop[workspace] > GET_SELECTED(workspace,1)-(int)(grf->win[1]->_maxy*settings->MoveOffSet))
        {
            if (settings->JumpScroll)
            {
                if (grf->win[1]->_maxy+GET_DIR(workspace,1).Ltop[workspace]-(settings->Borders*2)-(int)(grf->win[1]->_maxy*settings->JumpScrollValue) < (size_t)(grf->win[1]->_maxy*settings->JumpScrollValue)*2)
                    GET_DIR(workspace,1).Ltop[workspace] = 0;
                else
                    GET_DIR(workspace,1).Ltop[workspace] -= (int)(grf->win[1]->_maxy*settings->JumpScrollValue);
            }
            else
                GET_DIR(workspace,1).Ltop[workspace]--;
        }
    }
    if (grf->Work[workspace].Visual)
        GET_ESELECTED(workspace,1).List[workspace] |= grf->Work[workspace].SelectedGroup;
}

void ExitBasic(Basic* grf, const bool force)
{
    if (force)
        goto END;

    int count = 0;
    for (int i = 0; i < WORKSPACE_N; i++)
        count += grf->Work[i].exists;

    grf->Work[grf->inW].exists = 0;

    if (count > 1)
    {
        for (int i = grf->inW+1; i < WORKSPACE_N; i++)
            if (grf->Work[i].exists)
            {
                ChangeWorkSpace(grf,i);
                return;
            }
        for (int i = 0; i < grf->inW; i++)
            if (grf->Work[i].exists)
            {
                ChangeWorkSpace(grf,i);
                return;
            }
    }

    END: ;
    grf->ExitTime = true;
}

static void bulk(Basic* grf, const int workspace, const int selected, const bool full_path, char** args)
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
            while(try[i] < 50 && (fd[i] = mkstemp(buf[i])) == -1) try[i]++;
            if (try[i] > 49)
            {
                if (i == 1)
                    unlink(buf[0]);
                return;
            }
        }
    }

    size_t path_t = strlen(args[0]);

    for (size_t i = 0; i < grf->ActualSize; i++)
    {
        if (path_t == 0 ? 1 : (strcmp(grf->Base[i].path,args[0]) == 0))
        {
            write(fd[0],"//\t",3);
            write(fd[0],grf->Base[i].path,strlen(grf->Base[i].path));
            write(fd[0],"\n",1);
            for (long long int j = 0; j < grf->Base[i].El_t; j++)
            {
                if (selected == -1 ? 1 : (grf->Base[i].El[j].List[workspace]&selected))
                {
                    char* temp = full_path ? MakePath(grf->Base[i].path,grf->Base[i].El[j].name) : grf->Base[i].El[j].name;
                    write(fd[0],temp,strlen(temp));
                    write(fd[0],"\n",1);
                }
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

    while (file[pos])
    {
        for (size_t i = 0; i < grf->ActualSize; i++)
        {
            if (path_t == 0 ? 1 : (strcmp(grf->Base[i].path,args[0]) == 0))
            {
                for (long long int j = 0; j < grf->Base[i].El_t; j++)
                {
                    if (selected == -1 ? 1 : (grf->Base[i].El[j].List[workspace]&selected))
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
                            char* temp = full_path ? MakePath(grf->Base[i].path,grf->Base[i].El[j].name) : grf->Base[i].El[j].name;
                            char stemp[PATH_MAX];
                            if (x > 0 && strcmp(temp,buffer) != 0)
                            {
                                write(fd[1],args[3],strlen(args[3]));
                                write(fd[1]," ",1);

                                strcpy(stemp,temp);
                                MakePathRunAble(stemp);
                                write(fd[1],stemp,strlen(stemp));
                                write(fd[1]," ",1);

                                write(fd[1],args[4],strlen(args[4]));
                                write(fd[1]," ",1);

                                strcpy(stemp,buffer);
                                MakePathRunAble(stemp);
                                write(fd[1],stemp,strlen(stemp));
                                write(fd[1]," ",1);

                                write(fd[1],args[5],strlen(args[5]));
                                write(fd[1],"\n",1);
                            }
                        }

                        pos++;
                    }
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

typedef struct {
    int x;
    int y;
} Vector2i;

typedef struct {
    float x;
    float y;
} Vector2f;

struct WinArgs {
    WINDOW* place;
    Vector2i S_Size;
    Vector2f PercentSize;
    Vector2i MinSize;
    Vector2i MaxSize;
    Vector2i S_Pos;
    Vector2f PercentPos;
    Vector2i MinPos;
    Vector2i MaxPos;
    int settings;
};

void ConsoleResize(WINDOW* win, const struct WinArgs args)
{
    Vector2i Size = {0,0}, Pos = {0,0};
    
    if (args.S_Size.x != -1)
        Size.x = args.S_Size.x;
    if (args.S_Size.y != -1)
        Size.y = args.S_Size.y;
    if (args.PercentSize.x > 0)
        Size.x = args.place->_maxx*args.PercentSize.x;
    if (args.PercentSize.y > 0)
        Size.y = args.place->_maxy*args.PercentSize.y;
    if (args.MinSize.x != -1 && Size.x < args.MinSize.x)
        Size.x = args.MinSize.x;
    if (args.MinSize.y != -1 && Size.y < args.MinSize.y)
        Size.y = args.MinSize.y;
    if (args.MaxSize.x != -1 && Size.x > args.MaxSize.x)
        Size.x = args.MaxSize.x;
    if (args.MaxSize.y != -1 && Size.y > args.MaxSize.y)
        Size.y = args.MaxSize.y;

    if (args.S_Pos.x != -1)
        Pos.x = args.S_Pos.x;
    if (args.S_Pos.y != -1)
        Pos.y = args.S_Pos.y;
    if (args.PercentPos.x > 0)
        Pos.x = (args.place->_begx+args.place->_maxx)*args.PercentPos.x;
    if (args.PercentPos.y > 0)
        Pos.y = (args.place->_begy+args.place->_maxy)*args.PercentPos.y;
    if (args.MinPos.x != -1 && Pos.x < args.MinPos.x)
        Pos.x = args.MinPos.x;
    if (args.MinPos.y != -1 && Pos.y < args.MinPos.y)
        Pos.y = args.MinPos.y;
    if (args.MaxPos.x != -1 && Pos.x > args.MaxPos.x)
        Pos.x = args.MaxPos.x;
    if (args.MaxPos.y != -1 && Pos.y > args.MaxPos.y)
        Pos.y = args.MaxPos.y;
    
    wresize(win,Size.y,Size.x);
    mvwin(win,Pos.y,Pos.x);
    refresh();
    if (args.settings&0x1)
        wborder(win,0,0,0,0,0,0,0,0);
    wrefresh(win);
}

void ConsoleGetLine(WINDOW* win, Basic* grf, char* buffer, struct WinArgs args, char* first, char* add)
{
    curs_set(1);
    int Event;
    ConsoleResize(win,args);

    short int x = 0, y = 0, off = 0;
    int border = (args.settings&0x1) == 0x1;
    size_t first_t = strlen(first);
    if (add)
    {
        strcpy(buffer,add);
        x = strlen(add);
        while (x-off >= win->_maxx) off++; 
    }

    for (;;)
    {
        for (int i = border; i < win->_maxx-border; i++)
            mvwaddch(win,border+y,i,' ');
        mvwaddstr(win,border+y,border,first);
        mvwaddnstr(win,border+y,border+first_t,buffer+off,win->_maxx-border*2-first_t);
        wmove(win,border+y,border+x+first_t-off);
        wrefresh(win);
        
        switch (Event = getch())
        {
            case -1:
                continue;
            case 10:
            case ('x'&0x1f):
                goto END;
                break;
            case ('l'&0x1f):
                werase(win);
                y = 0;
                ConsoleResize(win,args);
                break;
            case ('a'&0x1f):
                x = 0;
                off = 0;
                break;
            case ('e'&0x1f):
                x = strlen(buffer);
                while (x-off >= win->_maxx) off++; 
                break;
            case 27:
            case ('r'&0x1f):
                goto END;
            case KEY_RESIZE:
                UpdateSizeBasic(grf);
                DrawBasic(grf,-1);
                werase(win);
                ConsoleResize(win,args);
                break;
            case KEY_BACKSPACE:
            case ('w'&0x1f):
                if (x > 0)
                {
                    for (size_t i = x-1; i <= strlen(buffer); i++)
                        buffer[i] = buffer[i+1];
                    x--;
                    if (off != 0)
                        off--;
                }
                break;
            case KEY_LEFT:
            case ('b'&0x1f):
                if (x > 0)
                {
                    if (off != 0 && border+x+first_t-off == (size_t)win->_maxx/2)
                        off--;
                    x--;
                }
                break;
            case KEY_RIGHT:
            case ('f'&0x1f):
                if (buffer[x])
                {
                    if (border+x+first_t-off >= (size_t)win->_maxx)
                        off++;
                    x++;
                }
                break;
            case ('d'&0x1f):
                while (buffer[x] && !isalnum(buffer[x])) x++;
                while (isalnum(buffer[x])) x++;
                while (buffer[x] && !isalnum(buffer[x])) x++;
                break;
            case ('s'&0x1f):
                while (buffer[x-1] && !isalnum(buffer[x-1])) x--;
                while (isalnum(buffer[x-1])) x--;
                while (buffer[x-1] && !isalnum(buffer[x-1])) x--;
                break;
            default:
                for (int i = strlen(buffer); i >= x; i--)
                    buffer[i] = buffer[i-1];
                buffer[x] = (char)Event;
                x++;
                if (border+x+first_t-off >= (size_t)win->_maxx)
                    off++;
                break;
        }
    }

    END: ;

    curs_set(0);
    werase(win);
    wrefresh(win);
}

void ___SET(const char* src, Basic* grf)
{
    size_t pos = 0;

    struct SetEntry SetEntries[] = {
        {"FileOpener",&settings->FileOpener},
        {"shell",&settings->shell},{"editor",&settings->editor},{"Bar1Settings",&settings->Bar1Settings},
        {"Bar2Settings",&settings->Bar2Settings},{"C_Borders",&settings->C_Borders},
        {"UserHostPattern",&settings->UserHostPattern},{"CopyBufferSize",&settings->CopyBufferSize},
        #ifdef __INOTIFY_ENABLE__
        {"INOTIFY_MASK",&settings->INOTIFY_MASK},{"DirLoadingMode",&settings->DirLoadingMode},
        #endif
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
        {"C_Selected",&settings->C_Selected},{"C_Exec_set",&settings->C_Exec_set},{"C_Exec",&settings->C_Exec},
        {"C_BLink",&settings->C_BLink},{"C_Dir",&settings->C_Dir},{"C_Reg",&settings->C_Reg},{"C_Fifo",&settings->C_Fifo},
        {"C_Sock",&settings->C_Sock},{"C_Dev",&settings->C_Dev},{"C_BDev",&settings->C_BDev},{"C_LDir",&settings->C_LDir},
        {"C_LReg",&settings->C_LReg},{"C_LFifo",&settings->C_LFifo},{"C_LSock",&settings->C_LSock},{"C_LDev",&settings->C_LDev},
        {"C_LBDev",&settings->C_LBDev},{"C_Other",&settings->C_Other},{"C_User_S_D",&settings->C_User_S_D},
        {"C_Bar_Dir",&settings->C_Bar_Dir},{"C_Bar_Name",&settings->C_Bar_Name},{"C_Bar_WorkSpace",&settings->C_Bar_WorkSpace},
        {"C_Bar_WorkSpace_Selected",&settings->C_Bar_WorkSpace_Selected},{"C_Group_0",&settings->C_Group_0},
        {"C_Group_1",&settings->C_Group_1},{"C_Group_2",&settings->C_Group_2},{"C_Group_3",&settings->C_Group_3},
        {"C_Group_4",&settings->C_Group_4},{"C_Group_5",&settings->C_Group_5},{"C_Group_6",&settings->C_Group_6},
        {"C_Group_7",&settings->C_Group_7},{"C_Bar_F",&settings->C_Bar_F},{"C_Bar_E",&settings->C_Bar_E},
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
    size_t pos = 0, end = 0;
    char temp[8192];

    if (src[pos] != '\'')
        return;
    pos++;
    end = FindEndOf(src+pos,'\'');
    strncpy(temp,src+pos,end);
    LoadConfig(temp,grf);
}
#endif

void ___MAP(const char* src, Basic* grf)
{
    size_t pos = 0, end = 0;
    char temp[8192];
    Key TempKey;
    TempKey.keys = (char*)malloc(64);
    while (src[pos+end] && !isspace(src[pos+end])) end++;
    strncpy(TempKey.keys,src+pos,end);
    TempKey.keys[end] = '\0';
    pos += end;
    StrToKeys(TempKey.keys);

    pos += FindFirstCharacter(src+pos);
    if (src[pos] != '\'')
        return;
    pos++;
    end = FindEndOf(src+pos,'\'');
    strncpy(temp,src+pos,end);
    temp[end] = '\0';
    pos += end;
    TempKey.value = temp;

    addKey(TempKey);
    free(TempKey.keys);
}

void ___MOVE(const char* src, Basic* grf)
{
    size_t pos = 0;
    int workspace = grf->inW, rot = 1, mul1 = 1, mul2 = 0;

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
                        rot = 2;
                        break;
                    case 'l':
                        rot = 3;
                        break;
                    case 'r':
                        rot = 4;
                        break;
                    case 'c':
                        pos++;
                        pos += FindFirstCharacter(src+pos);
                        mul1 = atoi(src+pos);
                        while (isdigit(src[pos])) pos++;
                        break;
                    case 'w':
                        pos++;
                        pos += FindFirstCharacter(src+pos);
                        workspace = atoi(src+pos);
                        while (isdigit(src[pos])) pos++;
                        break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }
        pos++;
    }

    if (!grf->Work[workspace].exists)
        return;

    switch (rot)
    {
        case 1:
        case 2:
            if (
                #ifdef __THREADS_FOR_DIR_ENABLE__
                !GET_DIR(workspace,1).enable &&
                #endif
                GET_DIR(workspace,1).El_t > 0)
            {
                mul2 = atoi(grf->cSF);
                for (int i = 0; i < (mul2+(mul2 == 0))*mul1; i++)
                    MoveD(rot,grf,workspace);
                if (settings->Win3Enable)
                    FastRun(grf);
            }
            break;
        case 3:
            CD("..",workspace,grf);
            grf->Work[workspace].Visual = 0;
            break;
        case 4:
            if (GET_DIR(workspace,1).El_t > 0)
            {
                switch (GET_ESELECTED(workspace,1).Type)
                {
                    case T_DIR:
                    case T_LDIR:
                        CD(GET_ESELECTED(workspace,1).name,workspace,grf);
                        grf->Work[workspace].Visual = 0;
                        break;
                    case T_REG:
                    case T_LREG:
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
    int workspace = grf->inW;
    char temp[8192];
    memset(temp,0,8191);

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
            pos += StrToPath(temp,src+pos);

        pos += FindFirstCharacter(src+pos);
    }

    grf->Work[workspace].exists = true;
    CD(temp,workspace,grf);
}

void ___GOTOP(const char* src, Basic* grf)
{
    size_t pos = 0, target = atol(grf->cSF);
    int workspace = grf->inW;

    while (src[pos])
    {
        if (src[pos] == '-')
        {
            do {
                pos++;

                switch (src[pos])
                {
                    case 't':
                        pos++;
                        pos += FindFirstCharacter(src+pos);
                        target = atol(src+pos);
                        while (isdigit(src[pos])) pos++;
                        break;
                    case 'w':
                        pos++;
                        pos += FindFirstCharacter(src+pos);
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
        !GET_DIR(workspace,1).enable &&
        #endif
        GET_DIR(workspace,1).El_t > 0)
    {
        if (target == 0)
            GoTop(grf,workspace);
        else if (target > GET_SELECTED(workspace,1))
        {
            for (size_t i = GET_SELECTED(workspace,1); i < target-1; i++)
                MoveD(1,grf,workspace);
        }
        else if (target < GET_SELECTED(workspace,1))
        {
            for (size_t i = GET_SELECTED(workspace,1); i > target-1; i--)
                MoveD(2,grf,workspace);
        }
        if (workspace == grf->inW && settings->Win3Enable)
            FastRun(grf);
    }
}

void ___GODOWN(const char* src, Basic* grf)
{
    size_t pos = 0, target = atol(grf->cSF);
    int workspace = grf->inW;

    while (src[pos])
    {
        if (src[pos] == '-')
        {
            do {
                pos++;

                switch (src[pos])
                {
                    case 't':
                        pos++;
                        pos += FindFirstCharacter(src+pos);
                        target = atol(src+pos);
                        while (isdigit(src[pos])) pos++;
                        break;
                    case 'w':
                        pos++;
                        pos += FindFirstCharacter(src+pos);
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
        !GET_DIR(workspace,1).enable &&
        #endif
        GET_DIR(workspace,1).El_t > 0)
    {
        if (target == 0)
            GoDown(grf,target);
        else if (target > GET_SELECTED(workspace,1))
        {
            for (size_t i = GET_SELECTED(workspace,1); i < target-1; i++)
                MoveD(1,grf,workspace);
        }
        else if (target < GET_SELECTED(workspace,1))
        {
            for (size_t i = GET_SELECTED(workspace,1); i > target-1; i--)
                MoveD(2,grf,workspace);
        }
        if (workspace == grf->inW && settings->Win3Enable)
            FastRun(grf);
    }
}

void ___CHANGEWORKSPACE(const char* src, Basic* grf)
{
    ChangeWorkSpace(grf,atoi(src));
}

#ifdef __GET_DIR_SIZE_ENABLE__
void ___GETSIZE(const char* src, Basic* grf)
{
    size_t pos = 0;
    bool File = 0
        ,Count = 0
        #ifdef __HUMAN_READABLE_SIZE_ENABLE__
        ,HumanReadAble = 0
        #endif
        ,Recursive = 0;

    while (src[pos])
    {
        if (src[pos] == '-')
        {
            do {
                pos++;
                switch (src[pos])
                {
                    case 'r':
                        Recursive = 1;
                        break;
                    case 'c':
                        Count = 1;
                        break;
                    case 'f':
                        File = 1;
                        break;
                    #ifdef __HUMAN_READABLE_SIZE_ENABLE__
                    case 'h':
                        HumanReadAble = 1;
                        break;
                    #endif
                }
            } while (src[pos] && !isspace(src[pos]));
        }

        pos += FindFirstCharacter(src+pos);
        if (src[pos+1] == '\0')
            break;
    }

    int temp;
    if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        !GET_DIR(grf->inW,1).enable &&
        #endif
        GET_DIR(grf->inW,1).El_t > 0 && (temp = open(GET_DIR(grf->inW,1).path,O_DIRECTORY)) != -1)
    {
        int tfd;
        int counter = 0;

        for (long long int i = 0; i < GET_DIR(grf->inW,1).El_t; i++)
        {
            if ((GET_DIR(grf->inW,1).El[i].List[grf->inW]&grf->Work[grf->inW].SelectedGroup) == grf->Work[grf->inW].SelectedGroup)
            {
                counter++;
                if (GET_DIR(grf->inW,1).El[i].Type == T_DIR ||
                    GET_DIR(grf->inW,1).El[i].Type == T_LDIR)
                {
                    if (faccessat(temp,GET_DIR(grf->inW,1).El[i].name,R_OK,0) == 0)
                    {
                        if ((tfd = openat(temp,GET_DIR(grf->inW,1).El[i].name,O_DIRECTORY)) != -1)
                        {
                            #ifdef __GET_DIR_SIZE_ENABLE__
                            if (!File)
                                GET_DIR(grf->inW,1).El[i].size = GetDirSize(tfd,Recursive,Count);
                            else
                            #endif
                            {
                                struct stat sFile;
                                stat(GET_DIR(grf->inW,1).El[i].name,&sFile);
                                GET_DIR(grf->inW,1).El[i].size = sFile.st_size;
                            }
                            #ifdef __HUMAN_READABLE_SIZE_ENABLE__
                            if (GET_DIR(grf->inW,1).El[i].SizErrToDisplay == NULL)
                                GET_DIR(grf->inW,1).El[i].SizErrToDisplay = (char*)malloc(16);
                            MakeHumanReadAble(GET_DIR(grf->inW,1).El[i].SizErrToDisplay
                                ,GET_DIR(grf->inW,1).El[i].size,HumanReadAble);
                            #endif
                            close(tfd);
                        }
                    }
                }
            }
        }

        if (counter == 0 && (GET_ESELECTED(grf->inW,1).Type == T_DIR ||
            GET_ESELECTED(grf->inW,1).Type == T_LDIR))
        {
            if (faccessat(temp,GET_ESELECTED(grf->inW,1).name,R_OK,0) == 0)
            {
                if ((tfd = openat(temp,GET_ESELECTED(grf->inW,1).name,O_DIRECTORY)) != -1)
                {
                    #ifdef __GET_DIR_SIZE_ENABLE__
                    if (!File)
                        GET_ESELECTED(grf->inW,1).size = GetDirSize(tfd,Recursive,Count);
                    else
                    #endif
                    {
                        struct stat sFile;
                        stat(GET_ESELECTED(grf->inW,1).name,&sFile);
                        GET_ESELECTED(grf->inW,1).size = sFile.st_size;
                    }
                    #ifdef __HUMAN_READABLE_SIZE_ENABLE__
                    if (GET_ESELECTED(grf->inW,1).SizErrToDisplay == NULL)
                        GET_ESELECTED(grf->inW,1).SizErrToDisplay = (char*)malloc(16);
                    MakeHumanReadAble(GET_ESELECTED(grf->inW,1).SizErrToDisplay
                        ,GET_ESELECTED(grf->inW,1).size,HumanReadAble);
                    #endif
                    close(tfd);
                }
            }
        }
        close(temp);
    }
}
#endif

void ___SETGROUP(const char* src, Basic* grf)
{
    grf->Work[grf->inW].SelectedGroup = 1<<atoi(src);
    if (grf->Work[grf->inW].Visual && GET_DIR(grf->inW,1).El_t > 0)
        GET_ESELECTED(grf->inW,1).List[grf->inW] |= grf->Work[grf->inW].SelectedGroup;
}

void ___FASTSELECT(const char* src, Basic* grf)
{
    if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        !GET_DIR(grf->inW,1).enable &&
        #endif
        GET_DIR(grf->inW,1).El_t > 0)
    {
        GET_ESELECTED(grf->inW,1).List[grf->inW] ^= grf->Work[grf->inW].SelectedGroup;
        MoveD(1,grf,grf->inW);
        if (settings->Win3Enable)
            FastRun(grf);
    }
}

void ___TOGGLEVISUAL(const char* src, Basic* grf)
{
    grf->Work[grf->inW].Visual = !grf->Work[grf->inW].Visual;
    if (grf->Work[grf->inW].Visual && GET_DIR(grf->inW,1).El_t > 0)
        GET_ESELECTED(grf->inW,1).List[grf->inW] |= grf->Work[grf->inW].SelectedGroup;
}

void ___F_COPY(const char* src, Basic* grf)
{
    size_t pos = 0;
    mode_t arg = 0;

    while (src[pos])
    {
        if (src[pos] == '-')
        {
            do {
                pos++;

                switch (src[pos])
                {
                    case 'c':
                        arg |= M_CHNAME;
                        break;
                    case 'r':
                        arg |= M_REPLACE;
                        break;
                    case 'd':
                        arg |= M_DCPY;
                        break;
                    case 'm':
                        arg |= M_MERGE;
                        break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }

        pos += FindFirstCharacter(src+pos);
        if (src[pos+1] == '\0')
            break;
    }

    CopyGroup(grf,".",arg);
    UpdateSizeBasic(grf);
    CD(".",grf->inW,grf);
}

void ___F_MOVE(const char* src, Basic* grf)
{
    size_t pos = 0;
    mode_t arg = 0;

    while (src[pos])
    {
        if (src[pos] == '-')
        {
            do {
                pos++;

                switch (src[pos])
                {
                    case 'c':
                        arg |= M_CHNAME;
                        break;
                    case 'r':
                        arg |= M_REPLACE;
                        break;
                    case 'd':
                        arg |= M_DCPY;
                        break;
                    case 'm':
                        arg |= M_MERGE;
                        break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }

        pos += FindFirstCharacter(src+pos);
        if (src[pos+1] == '\0')
            break;
    }

    MoveGroup(grf,".",arg);
    UpdateSizeBasic(grf);
    CD(".",grf->inW,grf);
}

void ___F_DELETE(const char* src, Basic* grf)
{
    size_t pos = 0;
    bool here = 1;

    while (src[pos])
    {
        if (src[pos] == '-')
        {
            do {
                pos++;

                switch (src[pos])
                {
                    case 'a':
                        here = 0;
                        break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }

        pos += FindFirstCharacter(src+pos);
        if (src[pos+1] == '\0')
            break;
    }

    DeleteGroup(grf,here);
    UpdateSizeBasic(grf);
    CD(".",grf->inW,grf);
}

void ___LOAD(const char* src, Basic* grf)
{
    size_t pos = 0;
    int mode = 2;
    bool threaded = 0;

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
                    case 't':
                        threaded = 1;
                        break;
                    #endif
                }

            } while (src[pos] && !isspace(src[pos]));
        }

        pos += FindFirstCharacter(src+pos);
    }

    GetDir(".",grf,grf->inW,1,mode
    #ifdef __THREADS_FOR_DIR_ENABLE__
    ,threaded
    #endif
    );
}

void ___SELECT(const char* src, Basic* grf)
{
    size_t pos = 0;
    int mode = 1;
    bool here = 1;

    while (src[pos])
    {
        if (src[pos] == '-')
        {
            do {
                pos++;

                switch (src[pos])
                {
                    case 'a':
                        here = 0;
                        break;
                    case 'e':
                        mode = 1;
                        break;
                    case 'd':
                        mode = 0;
                        break;
                    case 't':
                        mode = -1;
                        break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }

        pos += FindFirstCharacter(src+pos);
        if (src[pos+1] == '\0')
            break;
    }

    if (!here)
    {
        if (mode == -1)
        {
            for (size_t i = 0; i < grf->ActualSize; i++)
                for (long long int j = 0; j < grf->Base[i].El_t; j++)
                    grf->Base[i].El[j].List[grf->inW] ^= grf->Work[grf->inW].SelectedGroup;
        }
        else if (mode == 0)
        {
            for (size_t i = 0; i < grf->ActualSize; i++)
                for (long long int j = 0; j < grf->Base[i].El_t; j++)
                    grf->Base[i].El[j].List[grf->inW] ^= grf->Work[grf->inW].SelectedGroup*((grf->Base[i].El[j].List[grf->inW]&grf->Work[grf->inW].SelectedGroup) == grf->Work[grf->inW].SelectedGroup);
        }
        else
        {
            for (size_t i = 0; i < grf->ActualSize; i++)
                for (long long int j = 0; j < grf->Base[i].El_t; j++)
                    grf->Base[i].El[j].List[grf->inW] |= grf->Work[grf->inW].SelectedGroup;
        }
    }
    else
    {
        if (mode == -1)
        {
            for (long long int i = 0; i < GET_DIR(grf->inW,1).El_t; i++)
                GET_DIR(grf->inW,1).El[i].List[grf->inW] ^= grf->Work[grf->inW].SelectedGroup;
        }
        else if (mode == 0)
        {
            for (long long int i = 0; i < GET_DIR(grf->inW,1).El_t; i++)
                GET_DIR(grf->inW,1).El[i].List[grf->inW] ^= grf->Work[grf->inW].SelectedGroup*((GET_DIR(grf->inW,1).El[i].List[grf->inW]&grf->Work[grf->inW].SelectedGroup) == grf->Work[grf->inW].SelectedGroup);
        }
        else
        {
            for (long long int i = 0; i < GET_DIR(grf->inW,1).El_t; i++)
                GET_DIR(grf->inW,1).El[i].List[grf->inW] |= grf->Work[grf->inW].SelectedGroup;
        }
    }
}

void ___EXEC(const char* src, Basic* grf)
{
    size_t pos = 0, x = 0, end;
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
        else if (src[pos] == '\'')
        {
            pos++;
            end = FindEndOf(src+pos,'\'');
            strncpy(temp,src+pos,end);
            temp[end] = '\0';
            pos += end;
        }
        else
        {
            while (src[pos] && !isspace(src[pos]))
            {
                if (src[pos] == '\\')
                    temp[x] = src[++pos];
                else
                    temp[x] = src[pos];
                x++;
                pos++;
            }
            temp[x] = '\0';
        }


        pos += FindFirstCharacter(src+pos);
    }

    spawn(temp,NULL,NULL,background ? F_SILENT : F_NORMAL|F_WAIT);
}

void ___BULK(const char* src, Basic* grf)
{
    size_t pos = 0;
    int workspace = grf->inW, selected = -1;
    bool full_path = 0;
    char** temp = (char**)malloc(6*sizeof(char*)); 
    for (int i = 0; i < 6; i++)
    {
        temp[i] = (char*)malloc(PATH_MAX);
        memset(temp[i],0,PATH_MAX-1);
    }
    char* path = (char*)malloc(PATH_MAX);

    while (src[pos])
    {
        if (src[pos] && src[pos] == '-')
        {
            do {
                pos++;
                switch (src[pos])
                {
                    case 'f':
                        full_path = 1;
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
                        }
                        else
                        {
                            selected = 1<<atoi(src+pos);
                            while (isdigit(src[pos])) pos++;
                        }
                        break;
                    case 'S':
                        pos++;
                        pos += FindFirstCharacter(src+pos);
                        pos += StrToPath(temp[1],src+pos);
                        break;
                    case 'E':
                        pos++;
                        pos += FindFirstCharacter(src+pos);
                        pos += StrToPath(temp[2],src+pos);
                        break;
                    case 'b':
                        pos++;
                        pos += FindFirstCharacter(src+pos);
                        pos += StrToPath(temp[3],src+pos);
                        break;
                    case 'm':
                        pos++;
                        pos += FindFirstCharacter(src+pos);
                        pos += StrToPath(temp[4],src+pos);
                        break;
                    case 'e':
                        pos++;
                        pos += FindFirstCharacter(src+pos);
                        pos += StrToPath(temp[5],src+pos);
                        break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }
        else
            pos += StrToPath(path,src+pos);
        pos++;
    }

    if (!grf->Work[workspace].exists)
        return;
    
    realpath(path,temp[0]);
    free(path);
    bulk(grf,workspace,selected,full_path,temp);
    for (int i = 0; i < 6; i++)
        free(temp[i]);
    free(temp);
}

void ___CONSOLE(const char* src, Basic* grf)
{
    char buffer[16384];
    memset(buffer,0,16383);
    struct WinArgs args = {stdscr,
    {-1,1},{1,-1},{-1,-1},{-1,-1},
    {-1,-1},{-1,1},{-1,-1},{-1,-1},
    0};
    ConsoleGetLine(grf->win[5],grf,buffer,args,":",NULL);
    RunCommand(buffer,grf);
}








