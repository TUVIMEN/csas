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
#include "Functions.h"
#include "Usefull.h"
#include "Sort.h"
#include "Chars.h"
#include "Load.h"
#include "FastRun.h"
#include "Draw.h"
#include "Console.h"

#include "config.h"

#ifdef __LOAD_CONFIG_ENABLE__
#include "Loading.h"
#endif

void addKey(const Key src)
{
    if (keys_t == keys_a)
    {
        keys = (Key*)realloc(keys,(keys_a+=32)*sizeof(Key));
        for (size_t i = keys_t; i < keys_a; i++)
        {
            keys[i].keys = (char*)malloc(64);
            keys[i].value = NULL;
        }
    }

    long int found = -1;

    for (size_t i = 0; i < keys_t; i++)
    {
        if (strcmp(src.keys,keys[i].keys) == 0)
        {
            found = (long int)i;
            break;
        }
    }

    if (found == -1)
        found = (long int)keys_t++;

    strcpy(keys[found].keys,src.keys);

    if (keys[found].value != NULL)
    {
        free(keys[found].value);
        keys[found].value = NULL;
    }
    keys[found].value = (char*)malloc(PATH_MAX);
    strcpy(keys[found].value,src.value);
}

void KeyInit()
{
    addKey((Key){"q","quit"});
    addKey((Key){"Q","quit -f"});
    addKey((Key){"j","move -d"});
    addKey((Key){"J","move -dc 16"});
    addKey((Key){"k","move -u"});
    addKey((Key){"K","move -uc 16"});
    addKey((Key){"h","move -l"});
    addKey((Key){"l","move -r"});
    addKey((Key){"g/","/"});
    addKey((Key){"gd","cd /dev"});
    addKey((Key){"ge","cd /etc"});
    addKey((Key){"gM","cd /mnt"});
    addKey((Key){"go","cd /opt"});
    addKey((Key){"gs","cd /srv"});
    addKey((Key){"gp","cd /tmp"});
    addKey((Key){"gu","cd /usr"});
    addKey((Key){"gv","cd /var"});
    addKey((Key){"gg","gotop"});
    addKey((Key){"G","godown"});
    addKey((Key){"z1","ChangeWorkSpace 0"});
    addKey((Key){"z2","ChangeWorkSpace 1"});
    addKey((Key){"z3","ChangeWorkSpace 2"});
    addKey((Key){"z4","ChangeWorkSpace 3"});
    addKey((Key){"z5","ChangeWorkSpace 4"});
    addKey((Key){"z6","ChangeWorkSpace 5"});
    addKey((Key){"z7","ChangeWorkSpace 6"});
    addKey((Key){"z8","ChangeWorkSpace 7"});
    addKey((Key){"z9","ChangeWorkSpace 8"});
    addKey((Key){"z0","ChangeWorkSpace 9"});
    #ifdef __SORT_ELEMENTS_ENABLE__
    addKey((Key){"oe","set SortMethod SORT_NONE"});
    addKey((Key){"oE","set SortMethod SORT_NONE|SORT_REVERSE"});
    addKey((Key){"or","set SortMethod SORT_TYPE"});
    addKey((Key){"oR","set SortMethod SORT_TYPE|SORT_REVERSE'"});
    addKey((Key){"ob","set SortMethod SORT_CHIR"});
    addKey((Key){"oB","set SortMethod SORT_CHIR|SORT_REVERSE"});
    addKey((Key){"os","set SortMethod SORT_SIZE"});
    addKey((Key){"oS","set SortMethod SORT_SIZE|SORT_REVERSE"});
    addKey((Key){"otm","set SortMethod SORT_MTIME"});
    addKey((Key){"otM","set SortMethod SORT_MTIME|SORT_REVERSE"});
    addKey((Key){"otc","set SortMethod SORT_CTIME"});
    addKey((Key){"otC","set SortMethod SORT_CTIME|SORT_REVERSE"});
    addKey((Key){"ota","set SortMethod SORT_ATIME"});
    addKey((Key){"otA","set SortMethod SORT_ATIME|SORT_REVERSE"});
    addKey((Key){"og","set SortMethod SORT_GID"});
    addKey((Key){"oG","set SortMethod SORT_GID|SORT_REVERSE"});
    addKey((Key){"ou","set SortMethod SORT_UID"});
    addKey((Key){"oU","set SortMethod SORT_UID|SORT_REVERSE"});
    addKey((Key){"om","set SortMethod SORT_LNAME"});
    addKey((Key){"oM","set SortMethod SORT_LNAME|SORT_REVERSE"});
    addKey((Key){"on","set SortMethod SORT_NAME"});
    addKey((Key){"oN","set SortMethod SORT_NAME|SORT_REVERSE"});
    #endif
    #ifdef __GET_DIR_SIZE_ENABLE__
    addKey((Key){"dch","getsize -c"});
    addKey((Key){"dCh","getsize -cr"});
    addKey((Key){"dsh","getsize"});
    addKey((Key){"dSh","getsize -r"});
    addKey((Key){"dfh","getsize -f"});
    #ifdef __HUMAN_READABLE_SIZE_ENABLE__
    addKey((Key){"dcH","getsize -ch"});
    addKey((Key){"dCH","getsize -crh"});
    addKey((Key){"dsH","getsize -h"});
    addKey((Key){"dSH","getsize -rh"});
    addKey((Key){"dfH","getsize -fh"});
    #endif
    #endif
    addKey((Key){"x1","setgroup 0"});
    addKey((Key){"x2","setgroup 1"});
    addKey((Key){"x3","setgroup 2"});
    addKey((Key){"x4","setgroup 3"});
    addKey((Key){"x5","setgroup 4"});
    addKey((Key){"x6","setgroup 5"});
    addKey((Key){"x7","setgroup 6"});
    addKey((Key){"x8","setgroup 7"});
    addKey((Key){" ","fastselect"});
    addKey((Key){"V","togglevisual"});
    addKey((Key){"vta","select -ta"});
    addKey((Key){"vth","select -t ."});
    addKey((Key){"vda","select -da"});
    addKey((Key){"vdh","select -d ."});
    addKey((Key){"vea","select -ea"});
    addKey((Key){"veh","select -e ."});
    addKey((Key){"mm","f_move -c ."});
    addKey((Key){"mr","f_move -r ."});
    addKey((Key){"md","f_move -d ."});
    addKey((Key){"mM","f_move -cm ."});
    addKey((Key){"mR","f_move -rm ."});
    addKey((Key){"mD","f_move -dm ."});
    addKey((Key){"pp","f_copy -c ."});
    addKey((Key){"pr","f_copy -r ."});
    addKey((Key){"pd","f_copy -d ."});
    addKey((Key){"pP","f_copy -cm ."});
    addKey((Key){"pR","f_copy -rm ."});
    addKey((Key){"pD","f_copy -dm ."});
    addKey((Key){"dd","f_delete ."});
    addKey((Key){"dD","f_delete -a"});
    addKey((Key){"dh","f_delete -s"});
    addKey((Key){"R","load -tm 2"});
    addKey((Key){"s","console"});
    addKey((Key){"b","bulk -S sh -E nvim -b mv . -s 0"});
    addKey((Key){"/","console -a \"search -N \""});
    addKey((Key){"n","search -n 1"});
    addKey((Key){"N","search -b 1"});
}

Settings* SettingsInit()
{
    Settings* grf = (Settings*)malloc(sizeof(Settings));

    #ifdef __THREADS_FOR_DIR_ENABLE__
    grf->ThreadsForDir = 0;
    #endif
    #ifdef __THREADS_FOR_FILE_ENABLE__
    grf->ThreadsForFile = 0;
    #endif
    grf->FileOpener                    = (char*)malloc(PATH_MAX);
    strcpy(grf->FileOpener,"NULL");
    grf->shell                         = (char*)malloc(PATH_MAX);
    strcpy(grf->shell,"/bin/sh");
    grf->editor                        = (char*)malloc(PATH_MAX);
    strcpy(grf->editor,"/bin/nvim");
    grf->Values                        = (char*)malloc(PATH_MAX);
    strcpy(grf->Values,"BKMGTPEZY");
    grf->Bar1Settings                  = B_UHNAME | B_DIR | B_NAME | B_WORKSPACES | B_POSITION | B_FHBFREE | B_FGROUP | B_MODES | B_CSF;
    grf->Bar2Settings                  = DP_LSPERMS | DP_SMTIME | DP_PWNAME | DP_GRNAME;
    grf->UserHostPattern               = (char*)malloc(PATH_MAX);
    strcpy(grf->UserHostPattern,"%s@%s");
    grf->UserRHost                     = false;
    grf->CopyBufferSize                = 131072;
    #ifdef __INOTIFY_ENABLE__
    grf->INOTIFY_MASK                  = IN_DELETE | IN_DELETE_SELF | IN_CREATE | IN_MOVE | IN_MOVE_SELF;
    #endif
    grf->MoveOffSet                    = 0.1;
    grf->WrapScroll                    = false;
    grf->JumpScroll                    = false;
    grf->JumpScrollValue               = 16;
    grf->StatusBarOnTop                = false;
    grf->Win1Enable                    = true;
    grf->Win3Enable                    = true;
    grf->Bar1Enable                    = true;
    grf->Bar2Enable                    = true;
    grf->WinSizeMod                    = (double*)malloc(2*sizeof(double));
    grf->WinSizeMod[0]                 = 0.132f;
    grf->WinSizeMod[1]                 = 0.368f;
    grf->Borders                       = false;
    grf->FillBlankSpace                = true;
    grf->WindowBorder                  = (long int*)malloc(8*sizeof(long int));
    grf->WindowBorder[0]               = 0;
    grf->WindowBorder[1]               = 0;
    grf->WindowBorder[2]               = 0;
    grf->WindowBorder[3]               = 0;
    grf->WindowBorder[4]               = 0;
    grf->WindowBorder[5]                = 0;
    grf->WindowBorder[6]                = 0;
    grf->WindowBorder[7]                = 0;
    grf->EnableColor                    = true;
    grf->DelayBetweenFrames             = 8;
    grf->SDelayBetweenFrames            = 1;
    grf->NumberLines                    = false;
    grf->NumberLinesOff                 = false;
    grf->NumberLinesFromOne             = false;
    grf->DirLoadingMode                 = 1;
    grf->DisplayingC                    = DP_HSIZE;
    #ifdef __SHOW_HIDDEN_FILES_ENABLE__
    grf->ShowHiddenFiles                = true;
    #endif
    #ifdef __SORT_ELEMENTS_ENABLE__
    grf->SortMethod                     = SORT_NAME;
    grf->BetterFiles                    = (long int*)calloc(24,sizeof(long int));
    grf->BetterFiles[0]                 = T_DIR;
    grf->BetterFiles[1]                 = T_LDIR;
    #endif
    grf->DirSizeMethod                  = D_C;
    grf->C_Error                        = COLOR_PAIR(4) | A_BOLD | A_REVERSE;
    #ifdef __COLOR_FILES_BY_EXTENSION__
    grf->C_FType_A                      = COLOR_PAIR(4);
    grf->C_FType_I                      = COLOR_PAIR(2);
    grf->C_FType_V                      = COLOR_PAIR(6);
    #endif
    grf->C_Selected		                = A_REVERSE | A_BOLD;
    grf->C_Exec_set		                = A_BOLD;
    grf->C_Exec		                    = COLOR_PAIR(10);
    grf->C_BLink		                = COLOR_PAIR(1);
    grf->C_Dir		                    = COLOR_PAIR(1) | A_BOLD;
    grf->C_Reg		                    = A_NORMAL;
    grf->C_Fifo		                    = COLOR_PAIR(9) | A_ITALIC;
    grf->C_Sock		                    = COLOR_PAIR(9) | A_ITALIC;
    grf->C_Dev		                    = COLOR_PAIR(9);
    grf->C_BDev		                    = COLOR_PAIR(9);
    grf->C_LDir		                    = COLOR_PAIR(5) | A_BOLD;
    grf->C_LReg		                    = COLOR_PAIR(5);
    grf->C_LFifo		                = COLOR_PAIR(5);
    grf->C_LSock		                = COLOR_PAIR(5);
    grf->C_LDev		                    = COLOR_PAIR(5);
    grf->C_LBDev		                = COLOR_PAIR(5);
    grf->C_Other		                = COLOR_PAIR(0);
    grf->C_User_S_D		                = COLOR_PAIR(6) | A_BOLD;
    grf->C_Bar_Dir		                = COLOR_PAIR(1) | A_BOLD;
    grf->C_Bar_Name		                = A_NORMAL | A_BOLD;
    grf->C_Bar_WorkSpace		        = A_NORMAL | A_BOLD;
    grf->C_Bar_WorkSpace_Selected	    = COLOR_PAIR(6) | A_REVERSE | A_BOLD;
    grf->C_Group_0		                = COLOR_PAIR(2);
    grf->C_Group_1		                = COLOR_PAIR(1);
    grf->C_Group_2		                = COLOR_PAIR(7);
    grf->C_Group_3		                = COLOR_PAIR(4);
    grf->C_Group_4		                = COLOR_PAIR(5);
    grf->C_Group_5		                = COLOR_PAIR(6);
    grf->C_Group_6		                = COLOR_PAIR(9);
    grf->C_Group_7		                = COLOR_PAIR(10);
    grf->C_Bar_E                                = 0;
    grf->C_Bar_F                                = 0;
    grf->C_Borders                              = 0;

    return grf;
}

Basic* InitBasic()
{
    Basic* grf = (Basic*)malloc(sizeof(Basic));

    #ifdef __FILESYSTEM_INFORMATION_ENABLE__
    statfs(".",&grf->fs);
    #endif

    grf->ExitTime = false;
    grf->cSF_E = false;
    grf->cSF = (char*)malloc(64);

    grf->ConsoleHistory.allocated = 0;
    grf->ConsoleHistory.size = 0;
    grf->ConsoleHistory.History = NULL;
    grf->ConsoleHistory.max_size = 32;
    grf->ConsoleHistory.alloc_r = 8192;
    grf->ConsoleHistory.inc_r = 8;

    grf->SearchList.allocated = 0;
    grf->SearchList.inc_r = 16;
    grf->SearchList.pos = 0;
    grf->SearchList.size = 0;
    grf->SearchList.List = NULL;

    KeyInit();
    settings = SettingsInit();

    #ifdef __LOAD_CONFIG_ENABLE__
    settings->LoadConfig = 1;
    if (settings->LoadConfig)
    {
        LoadConfig("/etc/.csasrc",grf);
        char* HomeTemp = getenv("HOME");
        if (HomeTemp != NULL)
        {
            char* temp = (char*)malloc(PATH_MAX);
            sprintf(temp,"%s/.csasrc",HomeTemp);
            LoadConfig(temp,grf);
            sprintf(temp,"%s/.config/csas/.csasrc",HomeTemp);
            LoadConfig(temp,grf);
            free(temp);
        }
    }
    #endif

    settings->Win1Display = settings->Win1Enable;
    settings->Win3Display = settings->Win3Enable;

    grf->preview_fd = -1;

    initscr();

    if (has_colors() && settings->EnableColor)
    {
        start_color();
        use_default_colors();

        for (int i = 0; i < 16; i++)
            init_pair(i,i,-1);
    }

    for (int i = 0; i < 6; i++)
        grf->win[i] = newwin(0,0,0,0);

    grf->WinMiddle = 0;

    grf->ActualSize = 0;
    grf->AllocatedSize = 0;
    grf->Base = NULL;
    grf->FastRunSettings = 0;

    #ifdef __USER_NAME_ENABLE__
    grf->H_Host = (char*)malloc(64);
    grf->H_User = (char*)malloc(64);
    getlogin_r(grf->H_User,31);
    gethostname(grf->H_Host,31);
    #endif

    for (int i = 0; i < WORKSPACE_N; i++)
    {
        grf->Work[i].exists = 0;
        grf->Work[i].SelectedGroup = GROUP_0;
        grf->Work[i].Visual = 0;
        for (int j = 0; j < 3; j++)
            grf->Work[i].win[j] = -1;
    }

    grf->inW = 0;

    grf->Work[grf->inW].exists = 1;

    UpdateSizeBasic(grf);

    endwin();
    return grf;
}

void SetDelay(long int del)
{
    if (del > -1)
        halfdelay(del);
    else
        nodelay(stdscr,true); //!
}

void RunBasic(Basic* grf, const int argc, char** argv)
{
    refresh();
    noecho();

    curs_set(0);
    keypad(stdscr,true);
    notimeout(stdscr,true);
    SetDelay(settings->DelayBetweenFrames);

    time_t ActualTime, PastTime = 0;
    struct timespec MainTimer;

    int si;

    if (chdir(argv[1]) != 0)
    {
        endwin();
        fprintf(stderr,"Error while entering: %s\n",argv[1]);
        fflush(stderr);
        return;
    }

    CD(".",0,grf);

    do {
        clock_gettime(1,&MainTimer);
        ActualTime = MainTimer.tv_sec;

        #ifdef __THREADS_FOR_DIR_ENABLE__
        if (grf->Work[grf->inW].win[2] != -1 && grf->Work[grf->inW].win[0] != -1)
        {
            if (GET_DIR(grf->inW,0).enable || GET_DIR(grf->inW,2).enable)
                SetDelay(settings->SDelayBetweenFrames);
            else
                SetDelay(settings->DelayBetweenFrames);
        }
        else
            SetDelay(settings->SDelayBetweenFrames);
        #endif

        DrawBasic(grf,-1);

        si = UpdateEvent(grf);
        if (si != -1)
            RunCommand(keys[si].value,grf);

        if (ActualTime != PastTime)
        {
            PastTime = ActualTime;
            GetDir(".",grf,grf->inW,1,settings->DirLoadingMode
                #ifdef __THREADS_FOR_DIR_ENABLE__
                ,settings->ThreadsForDir
                #endif
                );
            if (settings->Win1Display)
                GetDir("..",grf,grf->inW,0,settings->DirLoadingMode
                #ifdef __THREADS_FOR_DIR_ENABLE__
                ,settings->ThreadsForDir
                #endif
                );
        }
    } while (!grf->ExitTime);

    endwin();

    freeBasic(grf);
}

void UpdateSizeBasic(Basic* grf)
{
    clear();
    getmaxyx(stdscr,grf->wy,grf->wx);

    if (settings->Bar1Enable)
    {
        wresize(grf->win[3],1,grf->wx);
        mvwin(grf->win[3],0,1);
        werase(grf->win[3]);
    }
    if (settings->Bar2Enable)
    {
        wresize(grf->win[4],1,grf->wx);
        mvwin(grf->win[4],(grf->wy-1)*!settings->StatusBarOnTop+settings->StatusBarOnTop-(!settings->Bar1Enable*settings->StatusBarOnTop),0);
        werase(grf->win[4]);
    }

    if (settings->Win1Enable)
    {
        wresize(grf->win[0],grf->wy-2+!settings->Bar1Enable+!settings->Bar2Enable,grf->wx*settings->WinSizeMod[0]);
        mvwin(grf->win[0],1+settings->StatusBarOnTop-!settings->Bar1Enable-(!settings->Bar2Enable*settings->StatusBarOnTop),0);
        grf->WinMiddle = grf->win[0]->_maxx;
        werase(grf->win[0]);
    }
    wresize(grf->win[1],grf->wy-2+!settings->Bar1Enable+!settings->Bar2Enable,(grf->wx*(settings->WinSizeMod[1]*settings->Win3Enable))+(!settings->Win3Enable*(grf->wx-grf->WinMiddle)));
    mvwin(grf->win[1],1+settings->StatusBarOnTop-!settings->Bar1Enable-(!settings->Bar2Enable*settings->StatusBarOnTop),grf->WinMiddle);
    werase(grf->win[1]);
    if (settings->Win3Enable)
    {
        wresize(grf->win[2],grf->wy-2+!settings->Bar1Enable+!settings->Bar2Enable,grf->wx-grf->win[1]->_maxx-grf->WinMiddle);
        mvwin(grf->win[2],1+settings->StatusBarOnTop-!settings->Bar1Enable-(!settings->Bar2Enable*settings->StatusBarOnTop),grf->win[1]->_maxx+grf->WinMiddle);
        werase(grf->win[2]);
    }

    refresh();
    if (settings->Borders)
        SetBorders(grf,-1);

    for (int i = 0; i < 3; i++)
    {
        if (grf->Work[grf->inW].win[i] == -1)
            continue;
        if (i == 0 && (!settings->Win1Enable || !settings->Win1Display))
            continue;
        if (i == 2 && (!settings->Win3Enable
        #ifdef __THREADS_FOR_DIR_ENABLE__
        || GET_DIR(grf->inW,1).enable
        #endif
        ))
            continue;

        #ifdef __THREADS_FOR_DIR_ENABLE__
        if (GET_DIR(grf->inW,i).enable)
            continue;
        #endif
        if ((long long int)GET_DIR(grf->inW,i).El_t == (long long int)-1)
            continue;
        if ((long long int)GET_DIR(grf->inW,i).El_t == (long long int)0)
            continue;

        if (GET_DIR(grf->inW,i).Ltop[grf->inW]+grf->win[i]->_maxy < GET_SELECTED(grf->inW,i))
            GET_DIR(grf->inW,i).Ltop[grf->inW] = GET_SELECTED(grf->inW,i)-grf->win[i]->_maxy;
        
        for (long long int j = GET_DIR(grf->inW,i).Ltop[grf->inW]; j-GET_DIR(grf->inW,i).Ltop[grf->inW] < (size_t)grf->win[i]->_maxy-(settings->Borders*2)+1; j++)
        {
            if (j == GET_DIR(grf->inW,i).El_t)
            {
                if (GET_DIR(grf->inW,i).Ltop[grf->inW] != 0)
                    GET_DIR(grf->inW,i).Ltop[grf->inW] = GET_DIR(grf->inW,i).El_t-1-grf->win[i]->_maxy;
                break;
            }
        }
    }
}

void freeBasic(Basic* grf)
{
    #ifdef __THREADS_FOR_DIR_ENABLE__
    for (size_t i = 0; i < grf->ActualSize; i++)
        if (grf->Base[i].enable)
            pthread_cancel(grf->Base[i].thread);
    #endif

    for (int i = 0; i < 6; i++)
        delwin(grf->win[i]);

    free(grf->cSF);
    #ifdef __USER_NAME_ENABLE__
    free(grf->H_Host);
    free(grf->H_User);
    #endif

    /*for (int i = 0; i < grf->ActualSize; i++)
    {
        if (grf->Base[i].El_t != -1)
        {
            inotify_rm_watch(grf->Base[i].fd,grf->Base[i].wd);
            close(grf->Base[i].fd);
            free(grf->Base[i].path);
            free(grf->Base[i].selected);
            free(grf->Base[i].Ltop);

            for (int j = 0; j < grf->Base[i].El_t; j++)
            {
                free(grf->Base[i].El[j].name);
                #ifdef __HUMAN_READABLE_SIZE_ENABLE__
                free(grf->Base[i].El[j].SizErrToDisplay);
                #endif
            }
        }
        free(grf->Base[i].El);
    }

    free(grf->Base);*/

    for (size_t i = 0; i < grf->ConsoleHistory.allocated; i++)
        free(grf->ConsoleHistory.History[i]);
    free(grf->ConsoleHistory.History);
    free(grf->SearchList.List);

    free(grf);
}
