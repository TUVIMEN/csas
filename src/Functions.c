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

    static char temp[128];
    strcpy(temp,src.keys);
    StrToKeys(temp);

    li found = -1;

    for (size_t i = 0; i < keys_t; i++)
    {
        if (strcmp(temp,keys[i].keys) == 0)
        {
            found = (li)i;
            break;
        }
    }

    if (found == -1)
        found = (li)keys_t++;

    strcpy(keys[found].keys,temp);

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
    addKey((Key){"gh","cd \"${HOME}\""});
    addKey((Key){"gd","cd /dev"});
    addKey((Key){"ge","cd /etc"});
    addKey((Key){"gM","cd /mnt"});
    addKey((Key){"go","cd /opt"});
    addKey((Key){"gs","cd /srv"});
    addKey((Key){"gp","cd /tmp"});
    addKey((Key){"gu","cd /usr"});
    addKey((Key){"gv","cd /var"});
	addKey((Key){"gm","cd \"${MEDIA}\""});
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
    addKey((Key){"or","set SortMethod SORT_TYPE|SORT_BETTER_FILES"});
    addKey((Key){"oR","set SortMethod SORT_TYPE|SORT_REVERSE|SORT_BETTER_FILES'"});
    addKey((Key){"ob","set SortMethod SORT_NONE|SORT_BETTER_FILES"});
    addKey((Key){"oB","set SortMethod SORT_NONE|SORT_REVERSE|SORT_BETTER_FILES"});
    addKey((Key){"os","set SortMethod SORT_SIZE|SORT_BETTER_FILES"});
    addKey((Key){"oS","set SortMethod SORT_SIZE|SORT_REVERSE|SORT_BETTER_FILES"});
    addKey((Key){"otm","set SortMethod SORT_MTIME|SORT_BETTER_FILES"});
    addKey((Key){"otM","set SortMethod SORT_MTIME|SORT_REVERSE|SORT_BETTER_FILES"});
    addKey((Key){"otc","set SortMethod SORT_CTIME|SORT_BETTER_FILES"});
    addKey((Key){"otC","set SortMethod SORT_CTIME|SORT_REVERSE|SORT_BETTER_FILES"});
    addKey((Key){"ota","set SortMethod SORT_ATIME|SORT_BETTER_FILES"});
    addKey((Key){"otA","set SortMethod SORT_ATIME|SORT_REVERSE|SORT_BETTER_FILES"});
    addKey((Key){"og","set SortMethod SORT_GID|SORT_BETTER_FILES"});
    addKey((Key){"oG","set SortMethod SORT_GID|SORT_REVERSE|SORT_BETTER_FILES"});
    addKey((Key){"ou","set SortMethod SORT_UID|SORT_BETTER_FILES"});
    addKey((Key){"oU","set SortMethod SORT_UID|SORT_REVERSE|SORT_BETTER_FILES"});
    addKey((Key){"om","set SortMethod SORT_LNAME|SORT_BETTER_FILES"});
    addKey((Key){"oM","set SortMethod SORT_LNAME|SORT_REVERSE|SORT_BETTER_FILES"});
    addKey((Key){"on","set SortMethod SORT_NAME|SORT_BETTER_FILES"});
    addKey((Key){"oN","set SortMethod SORT_NAME|SORT_REVERSE|SORT_BETTER_FILES"});
    #endif
    addKey((Key){"dct","getsize -cs s"});
    addKey((Key){"dCt","getsize -crs s"});
    addKey((Key){"dst","getsize -s s"});
    addKey((Key){"dSt","getsize -rs s"});
    addKey((Key){"dft","getsize -fs s"});
    addKey((Key){"dch","getsize -cs ."});
    addKey((Key){"dCh","getsize -crs ."});
    addKey((Key){"dsh","getsize -s ."});
    addKey((Key){"dSh","getsize -rs ."});
    addKey((Key){"dfh","getsize -fs ."});
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
    addKey((Key){"vta","select -ts - -o ."});
    addKey((Key){"vth","select -ts - -o . ."});
    addKey((Key){"vda","select -ds - -o ."});
    addKey((Key){"vdh","select -ds - -o . ."});
    addKey((Key){"vea","select -es - -o ."});
    addKey((Key){"veh","select -es - -o . ."});
    addKey((Key){"mm","f_mod m -s . -c -o ."});
    addKey((Key){"mr","f_mod m -s . -r -o ."});
    addKey((Key){"md","f_mod m -s . -d -o ."});
    addKey((Key){"mM","f_mod m -s . -cm -o ."});
    addKey((Key){"mR","f_mod m -s . -rm -o ."});
    addKey((Key){"mD","f_mod m -s . -dm -o ."});
    addKey((Key){"pp","f_mod c -s . -c -o ."});
    addKey((Key){"pr","f_mod c -s . -r -o ."});
    addKey((Key){"pd","f_mod c -s . -d -o ."});
    addKey((Key){"pP","f_mod c -s . -cm -o ."});
    addKey((Key){"pR","f_mod c -s . -rm -o ."});
    addKey((Key){"pD","f_mod c -s . -dm -o ."});
    addKey((Key){"Dd","f_mod d -s . ."});
    addKey((Key){"DD","f_mod d -s ."});
    addKey((Key){"Dt","f_mod d -s s"});
    addKey((Key){"R","load -tm 2"});
    addKey((Key){":","console"});
    addKey((Key){"cd","console -a \"cd \""});
    addKey((Key){"s","console -a \"shell \""});
    addKey((Key){"S","exec bash"});
    addKey((Key){"b","bulk -S sh -E nvim -b mv -s 0 -R ."});
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
    grf->FileOpener                    = strcpy(malloc(PATH_MAX),"NULL");
    grf->shell                         = strcpy(malloc(PATH_MAX),"sh");
    grf->editor                        = strcpy(malloc(PATH_MAX),"vim");
    grf->Values                        = strcpy(malloc(16),"KMGTPEZY");
    grf->Bar1Settings                  = B_DIR | B_WORKSPACES | B_POSITION | B_FGROUP | B_MODES | B_CSF;
    grf->Bar2Settings                  = DP_LSPERMS | DP_SMTIME | DP_HSIZE;
    grf->UserHostPattern               = strcpy(malloc(NAME_MAX),"%s@%s");
    grf->UserRHost                     = false;
    grf->CopyBufferSize                = 131072;
    grf->MoveOffSet                    = 0.1;
    grf->WrapScroll                    = true;
    grf->JumpScroll                    = false;
    grf->JumpScrollValue               = 16;
    grf->StatusBarOnTop                = false;
    grf->Win1Enable                    = false;
    grf->Win3Enable                    = false;
    grf->Bar1Enable                    = true;
    grf->Bar2Enable                    = true;
    grf->WinSizeMod                    = (double*)malloc(2*sizeof(double));
    grf->WinSizeMod[0]                 = 0.132f;
    grf->WinSizeMod[1]                 = 0.368f;
    grf->Borders                       = false;
    grf->FillBlankSpace                = false;
    grf->WindowBorder                  = (li*)malloc(8*sizeof(li));
    grf->WindowBorder[0]               = 0;
    grf->WindowBorder[1]               = 0;
    grf->WindowBorder[2]               = 0;
    grf->WindowBorder[3]               = 0;
    grf->WindowBorder[4]               = 0;
    grf->WindowBorder[5]                = 0;
    grf->WindowBorder[6]                = 0;
    grf->WindowBorder[7]                = 0;
    grf->EnableColor                    = true;
    grf->DelayBetweenFrames             = 1024;
    grf->SDelayBetweenFrames            = 1024;
    grf->NumberLines                    = false;
    grf->NumberLinesOff                 = false;
    grf->NumberLinesFromOne             = false;
    grf->DirLoadingMode                 = 1;
    grf->DisplayingC                    = 0;
    #ifdef __SHOW_HIDDEN_FILES_ENABLE__
    grf->ShowHiddenFiles                = true;
    #endif
    #ifdef __SORT_ELEMENTS_ENABLE__
    grf->SortMethod                     = SORT_NAME|SORT_BETTER_FILES;
    grf->BetterFiles                    = (li*)calloc(16,sizeof(li));
    grf->BetterFiles[0]                 = T_DIR;
    grf->BetterFiles[1]                 = T_LDIR;
    #endif
    grf->DirSizeMethod                  = D_F;
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
    grf->C_Bar_Dir		                = COLOR_PAIR(1) | A_UNDERLINE | A_BOLD;
    grf->C_Bar_Name		                = A_NORMAL | A_BOLD;
    grf->C_Bar_WorkSpace		        = A_NORMAL | A_BOLD;
    grf->C_Bar_WorkSpace_Selected	    = COLOR_PAIR(6) | A_REVERSE | A_BOLD;
    grf->C_Group                        = malloc(sizeof(ll)*8);
    grf->C_Group[0]		                = COLOR_PAIR(2);
    grf->C_Group[1]		                = COLOR_PAIR(1);
    grf->C_Group[2]		                = COLOR_PAIR(7);
    grf->C_Group[3]		                = COLOR_PAIR(4);
    grf->C_Group[4]		                = COLOR_PAIR(5);
    grf->C_Group[5]		                = COLOR_PAIR(6);
    grf->C_Group[6]		                = COLOR_PAIR(9);
    grf->C_Group[7]		                = COLOR_PAIR(10);
    grf->C_Bar_E                        = COLOR_PAIR(1);
    grf->C_Bar_F                        = COLOR_PAIR(1);
    grf->C_Borders                      = 0;

    return grf;
}

int initcurs()
{
    newterm(NULL,stderr,stdin);

    noecho();
	cbreak();
	nonl();

    curs_set(0);
    keypad(stdscr,true);
    notimeout(stdscr,true);
	set_escdelay(25);

    if (has_colors() && settings->EnableColor)
    {
        start_color();
        use_default_colors();

        for (int i = 0; i < 16; i++)
            init_pair(i,i,-1);
    }

    return 0;
}

Basic* InitBasic()
{
    Basic* grf = (Basic*)malloc(sizeof(Basic));

    #ifdef __FILESYSTEM_INFO_ENABLE__
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

    initcurs();

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
    getlogin_r(grf->H_User,63);
    gethostname(grf->H_Host,63);
    #endif

    for (int i = 0; i < WORKSPACE_N; i++)
    {
        grf->Work[i].path = NULL;
        grf->Work[i].exists = 0;
        grf->Work[i].SelectedGroup = 0;
        grf->Work[i].Visual = 0;
        for (int j = 0; j < 3; j++)
            grf->Work[i].win[j] = -1;
    }

    grf->inW = 0;
    grf->Work[grf->inW].exists = true;
    grf->Work[grf->inW].ShowMessage = false;
    grf->Work[grf->inW].path = (char*)malloc(PATH_MAX);

    UpdateSizeBasic(grf);

    return grf;
}

void RunBasic(Basic* grf, const int argc, char** argv)
{
    time_t ActualTime, PastTime = 0;
    struct timespec MainTimer;

    int si;

    char* path;
    if (argv[1])
        path = argv[1];
    else
        path = getenv("PWD");

    if (chdir(path) != 0)
    {
        endwin();
        fprintf(stderr,"Error while entering: %s\n",path);
        fflush(stderr);
        return;
    }

    if (strcmp(path,".") == 0)
        strcpy(grf->Work[grf->inW].path,getenv("PWD"));
    else if (strcmp(path,"..") == 0)
    {
        strcpy(grf->Work[grf->inW].path,getenv("PWD"));
        size_t end = strlen(grf->Work[grf->inW].path)-1;
        for (; end != 1 && grf->Work[grf->inW].path[end] != '/'; end--) grf->Work[grf->inW].path[end] = 0;
        if (end != 1) grf->Work[grf->inW].path[end] = 0;
    }
    else if (path[0] == '/')
        strcpy(grf->Work[grf->inW].path,path);
    else
    {
        strcpy(grf->Work[grf->inW].path,getenv("PWD"));
        size_t end = strlen(grf->Work[grf->inW].path);
        if (grf->Work[grf->inW].path[end-1] != '/')
        {
            grf->Work[grf->inW].path[end] = '/';
            strcpy(grf->Work[grf->inW].path+end+1,path);
        }
        else
            strcpy(grf->Work[grf->inW].path+end,path);
    }

    size_t end = strlen(grf->Work[grf->inW].path);
    while (end-1 && grf->Work[grf->inW].path[end-1] == '/')
    {
        grf->Work[grf->inW].path[end-1] = 0;
        end = strlen(grf->Work[grf->inW].path);
    }

    CD(".",0,grf);

    do {
        clock_gettime(1,&MainTimer);
        ActualTime = MainTimer.tv_sec;

        #ifdef __THREADS_FOR_DIR_ENABLE__
        if (grf->Work[grf->inW].win[2] != -1 && grf->Work[grf->inW].win[0] != -1)
        {
            if (GET_DIR(grf->inW,0)->enable || GET_DIR(grf->inW,2)->enable)
                timeout(settings->SDelayBetweenFrames);
            else
                timeout(settings->DelayBetweenFrames);
        }
        else
            timeout(settings->SDelayBetweenFrames);
        #endif

        DrawBasic(grf,-1);

        if ((si = UpdateEvent(grf)) != -1)
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
        || GET_DIR(grf->inW,1)->enable
        #endif
        ))
            continue;

        #ifdef __THREADS_FOR_DIR_ENABLE__
        if (GET_DIR(grf->inW,i)->enable)
            continue;
        #endif
        if ((ll)GET_DIR(grf->inW,i)->El_t == (ll)-1)
            continue;
        if ((ll)GET_DIR(grf->inW,i)->El_t == (ll)0)
            continue;

        if (GET_DIR(grf->inW,i)->Ltop[grf->inW]+grf->win[i]->_maxy < GET_SELECTED(grf->inW,i))
            GET_DIR(grf->inW,i)->Ltop[grf->inW] = GET_SELECTED(grf->inW,i)-grf->win[i]->_maxy;

        for (ll j = GET_DIR(grf->inW,i)->Ltop[grf->inW]; j-GET_DIR(grf->inW,i)->Ltop[grf->inW] < (size_t)grf->win[i]->_maxy-(settings->Borders*2)+1; j++)
        {
            if (j == GET_DIR(grf->inW,i)->El_t)
            {
                if (GET_DIR(grf->inW,i)->Ltop[grf->inW] != 0)
                    GET_DIR(grf->inW,i)->Ltop[grf->inW] = GET_DIR(grf->inW,i)->El_t-1-grf->win[i]->_maxy;
                break;
            }
        }
    }
}


void freeEl(struct Element** El, ll* El_t)
{
    for (ll i = 0; i < *El_t; i++)
    {
        free((*El)[i].List);
        free((*El)[i].name);
    }
    free(*El);
    *El = NULL;
    *El_t = 0;
}

void freeBasic(Basic* grf)
{
    #ifdef __THREADS_FOR_DIR_ENABLE__
    for (size_t i = 0; i < grf->ActualSize; i++)
        if (grf->Base[i]->enable)
            pthread_cancel(grf->Base[i]->thread);
    #endif

    for (int i = 0; i < 6; i++)
        delwin(grf->win[i]);

    free(grf->cSF);
    #ifdef __USER_NAME_ENABLE__
    free(grf->H_Host);
    free(grf->H_User);
    #endif

    for (int i = 0; i < WORKSPACE_N; i++)
        free(grf->Work[i].path);

    for (size_t i = 0; i < grf->ActualSize; i++)
    {
        free(grf->Base[i]->path);
        free(grf->Base[i]->selected);
        free(grf->Base[i]->Ltop);
        free(grf->Base[i]->filter);

        if (grf->Base[i]->oldEl_t > grf->Base[i]->El_t)
            grf->Base[i]->El_t = grf->Base[i]->oldEl_t;

        freeEl(&grf->Base[i]->El,&grf->Base[i]->El_t);
    }
    free(grf->Base);

    for (size_t i = 0; i < grf->ConsoleHistory.allocated; i++)
        free(grf->ConsoleHistory.History[i]);
    free(grf->ConsoleHistory.History);

    free(grf->SearchList.List);

    free(grf);
}
