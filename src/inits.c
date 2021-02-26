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
#include "inits.h"
#include "useful.h"
#include "sort.h"
#include "functions.h"
#include "load.h"
#include "preview.h"
#include "draw.h"
#include "console.h"

#include "config.h"

#ifdef __LOAD_CONFIG_ENABLE__
#include "loading.h"
#endif

void addKey(char *c, char *v)
{
    if (keys_t == keys_a)
    {
        keys = (Key*)realloc(keys,(keys_a+=32)*sizeof(Key));
        for (size_t i = keys_t; i < keys_a; i++)
        {
            keys[i].keys = (wchar_t*)malloc(64*sizeof(wchar_t));
            keys[i].value = NULL;
        }
    }

    wchar_t temp[64];

    StrToKeys(c,temp);

    li found = -1;

    for (size_t i = 0; i < keys_t; i++)
    {
        if (wcscmp(temp,keys[i].keys) == 0)
        {
            found = (li)i;
            break;
        }
    }

    if (found == -1)
        found = (li)keys_t++;

    wcpcpy(keys[found].keys,temp);

    if (keys[found].value == NULL)
        keys[found].value = (char*)malloc(PATH_MAX);
    strcpy(keys[found].value,v);
}

void KeyInit()
{
    addKey("q","quit");
    addKey("Q","quit -f");
    addKey("j","move -d");
    addKey("J","move -dc 16");
    addKey("k","move -u");
    addKey("K","move -uc 16");
    addKey("h","move -l");
    addKey("l","move -r");
    addKey("g/","/");
    addKey("gh","cd \"${HOME}\"");
    addKey("gd","cd /dev");
    addKey("ge","cd /etc");
    addKey("gM","cd /mnt");
    addKey("go","cd /opt");
    addKey("gs","cd /srv");
    addKey("gp","cd /tmp");
    addKey("gu","cd /usr");
    addKey("gv","cd /var");
    addKey("gm","cd \"${MEDIA\"");
    addKey("gg","gotop");
    addKey("G","godown");
    addKey("z1","ChangeWorkSpace 0");
    addKey("z2","ChangeWorkSpace 1");
    addKey("z3","ChangeWorkSpace 2");
    addKey("z4","ChangeWorkSpace 3");
    addKey("z5","ChangeWorkSpace 4");
    addKey("z6","ChangeWorkSpace 5");
    addKey("z7","ChangeWorkSpace 6");
    addKey("z8","ChangeWorkSpace 7");
    addKey("z9","ChangeWorkSpace 8");
    addKey("z0","ChangeWorkSpace 9");
    #ifdef __SORT_ELEMENTS_ENABLE__
    addKey("oe","set SortMethod SORT_NONE");
    addKey("oE","set SortMethod SORT_NONE|SORT_REVERSE");
    addKey("or","set SortMethod SORT_TYPE|SORT_BETTER_FILES");
    addKey("oR","set SortMethod SORT_TYPE|SORT_REVERSE|SORT_BETTER_FILES'");
    addKey("ob","set SortMethod SORT_NONE|SORT_BETTER_FILES");
    addKey("oB","set SortMethod SORT_NONE|SORT_REVERSE|SORT_BETTER_FILES");
    addKey("os","set SortMethod SORT_SIZE|SORT_BETTER_FILES");
    addKey("oS","set SortMethod SORT_SIZE|SORT_REVERSE|SORT_BETTER_FILES");
    addKey("otm","set SortMethod SORT_MTIME|SORT_BETTER_FILES");
    addKey("otM","set SortMethod SORT_MTIME|SORT_REVERSE|SORT_BETTER_FILES");
    addKey("otc","set SortMethod SORT_CTIME|SORT_BETTER_FILES");
    addKey("otC","set SortMethod SORT_CTIME|SORT_REVERSE|SORT_BETTER_FILES");
    addKey("ota","set SortMethod SORT_ATIME|SORT_BETTER_FILES");
    addKey("otA","set SortMethod SORT_ATIME|SORT_REVERSE|SORT_BETTER_FILES");
    addKey("og","set SortMethod SORT_GID|SORT_BETTER_FILES");
    addKey("oG","set SortMethod SORT_GID|SORT_REVERSE|SORT_BETTER_FILES");
    addKey("ou","set SortMethod SORT_UID|SORT_BETTER_FILES");
    addKey("oU","set SortMethod SORT_UID|SORT_REVERSE|SORT_BETTER_FILES");
    addKey("om","set SortMethod SORT_LNAME|SORT_BETTER_FILES");
    addKey("oM","set SortMethod SORT_LNAME|SORT_REVERSE|SORT_BETTER_FILES");
    addKey("on","set SortMethod SORT_NAME|SORT_BETTER_FILES");
    addKey("oN","set SortMethod SORT_NAME|SORT_REVERSE|SORT_BETTER_FILES");
    #endif
    addKey("dct","getsize -cs s");
    addKey("dCt","getsize -crs s");
    addKey("dst","getsize -s s");
    addKey("dSt","getsize -rs s");
    addKey("dft","getsize -fs s");
    addKey("dch","getsize -cs .");
    addKey("dCh","getsize -crs .");
    addKey("dsh","getsize -s .");
    addKey("dSh","getsize -rs .");
    addKey("dfh","getsize -fs .");
    addKey("x1","setgroup 0");
    addKey("x2","setgroup 1");
    addKey("x3","setgroup 2");
    addKey("x4","setgroup 3");
    addKey("x5","setgroup 4");
    addKey("x6","setgroup 5");
    addKey("x7","setgroup 6");
    addKey("x8","setgroup 7");
    addKey(" ","fastselect");
    addKey("V","togglevisual");
    addKey("vta","select -ts - -o .");
    addKey("vth","select -ts - -o . .");
    addKey("vda","select -ds - -o .");
    addKey("vdh","select -ds - -o . .");
    addKey("vea","select -es - -o .");
    addKey("veh","select -es - -o . .");
    addKey("mm","f_mod m -s . -c -o .");
    addKey("mr","f_mod m -s . -r -o .");
    addKey("md","f_mod m -s . -d -o .");
    addKey("mM","f_mod m -s . -cm -o .");
    addKey("mR","f_mod m -s . -rm -o .");
    addKey("mD","f_mod m -s . -dm -o .");
    addKey("pp","f_mod c -s . -c -o .");
    addKey("pr","f_mod c -s . -r -o .");
    addKey("pd","f_mod c -s . -d -o .");
    addKey("pP","f_mod c -s . -cm -o .");
    addKey("pR","f_mod c -s . -rm -o .");
    addKey("pD","f_mod c -s . -dm -o .");
    addKey("Dd","f_mod d -s . .");
    addKey("DD","f_mod d -s .");
    addKey("Dt","f_mod d -s s");
    addKey("R","load -tm 2");
    addKey(":","console");
    addKey("cd","console -a 'cd '");
    addKey("s","console -a 'shell '");
    addKey("S","exec bash");
    addKey("b","bulk -S sh -E nvim -b mv -s 0 -R .");
    addKey("/","console -a 'search -N '");
    addKey("n","search -n 1");
    addKey("N","search -b 1");
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
    grf->JumpScrollValue               = 0.5;
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
    grf->SDelayBetweenFrames            = 1;
    grf->NumberLines                    = false;
    grf->NumberLinesOff                 = false;
    grf->NumberLinesFromOne             = false;
    grf->DirLoadingMode                 = 0;
    grf->DisplayingC                    = 0;
    grf->PreviewSettings                = PREV_ASCII|PREV_BINARY;
    #ifdef __SORT_ELEMENTS_ENABLE__
    grf->SortMethod                     = SORT_NAME|SORT_BETTER_FILES;
    grf->BetterFiles                    = (li*)calloc(16,sizeof(li));
    grf->BetterFiles[0]                 = T_DIR;
    grf->BetterFiles[1]                 = T_DIR|T_SYMLINK;
    #endif
    grf->DirSizeMethod                  = D_F;
    grf->C_Error                        = COLOR_PAIR(1) | A_BOLD | A_REVERSE;
    #ifdef __COLOR_FILES_BY_EXTENSION__
    grf->C_FType_A                      = COLOR_PAIR(1);
    grf->C_FType_I                      = COLOR_PAIR(3);
    grf->C_FType_V                      = COLOR_PAIR(5);
    #endif
    grf->C_Selected		                = A_REVERSE | A_BOLD;
    grf->C_Exec_set		                = A_BOLD;
    grf->C_Exec_col	                    = COLOR_PAIR(2);
    grf->C_Dir		                    = COLOR_PAIR(4) | A_BOLD;
    grf->C_Reg		                    = A_NORMAL;
    grf->C_Fifo		                    = COLOR_PAIR(3) | A_ITALIC;
    grf->C_Sock		                    = COLOR_PAIR(3) | A_ITALIC;
    grf->C_Dev		                    = COLOR_PAIR(3);
    grf->C_BDev		                    = COLOR_PAIR(6);
    grf->C_Other		                = COLOR_PAIR(0);
    grf->C_FileMissing                  = COLOR_PAIR(5);
    grf->C_SymLink                      = COLOR_PAIR(6);
    grf->C_User_S_D		                = COLOR_PAIR(2) | A_BOLD;
    grf->C_Bar_Dir		                = COLOR_PAIR(4) | A_BOLD;
    grf->C_Bar_Name		                = A_NORMAL | A_BOLD;
    grf->C_Bar_WorkSpace		        = A_NORMAL | A_BOLD;
    grf->C_Bar_WorkSpace_Selected	    = COLOR_PAIR(2) | A_REVERSE | A_BOLD;
    grf->C_Group                        = malloc(sizeof(ll)*8);
    grf->C_Group[0]		                = COLOR_PAIR(3);
    grf->C_Group[1]		                = COLOR_PAIR(2);
    grf->C_Group[2]		                = COLOR_PAIR(1);
    grf->C_Group[3]		                = COLOR_PAIR(4);
    grf->C_Group[4]		                = COLOR_PAIR(5);
    grf->C_Group[5]		                = COLOR_PAIR(6);
    grf->C_Group[6]		                = COLOR_PAIR(7);
    grf->C_Group[7]		                = COLOR_PAIR(8);
    grf->C_Bar_E                        = COLOR_PAIR(0);
    grf->C_Bar_F                        = COLOR_PAIR(0);
    grf->C_Borders                      = 0;

    return grf;
}

int initcurs()
{
    //newterm(NULL,stderr,stdin);
	initscr();

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

        for (int i = 0; i < 8; i++)
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

    grf->exit_time = false;
    grf->was_typed = false;
    grf->typed_keys = (char*)malloc(64);

    grf->ConsoleHistory.allocated = 0;
    grf->ConsoleHistory.size = 0;
    grf->ConsoleHistory.history = NULL;
    grf->ConsoleHistory.max_size = 32;
    grf->ConsoleHistory.alloc_r = 8192;
    grf->ConsoleHistory.inc_r = 8;

    grf->SearchList.allocated = 0;
    grf->SearchList.inc_r = 16;
    grf->SearchList.pos = 0;
    grf->SearchList.size = 0;
    grf->SearchList.list = NULL;

    KeyInit();
    settings = SettingsInit();

    #ifdef __LOAD_CONFIG_ENABLE__
    settings->LoadConfig = 1;
    if (settings->LoadConfig)
    {
        LoadConfig("/etc/csasrc",grf);
        char *HomeTemp = getenv("HOME");
        if (HomeTemp != NULL)
        {
            char temp[PATH_MAX];
            sprintf(temp,"%s/.csasrc",HomeTemp);
            LoadConfig(temp,grf);
            sprintf(temp,"%s/.config/csas/.csasrc",HomeTemp);
            LoadConfig(temp,grf);
        }
    }
    #endif

    settings->Win1Display = settings->Win1Enable;
    settings->Win3Display = settings->Win3Enable;

    grf->preview_fd = -1;

    initcurs();

    for (int i = 0; i < 6; i++)
        grf->win[i] = newwin(0,0,0,0);

    grf->win_middle = 0;

    grf->size = 0;
    grf->asize = 0;
    grf->base = NULL;

    grf->preview_settings = 0;

    #ifdef __USER_NAME_ENABLE__
    grf->hostn = (char*)malloc(64);
    grf->usern = (char*)malloc(64);
    getlogin_r(grf->usern,63);
    gethostname(grf->hostn,63);
    #endif

    for (int i = 0; i < WORKSPACE_N; i++)
    {
        grf->workspaces[i].path = NULL;
        grf->workspaces[i].exists = 0;
        grf->workspaces[i].sel_group = 0;
        grf->workspaces[i].visual = 0;
        for (int j = 0; j < 3; j++)
            grf->workspaces[i].win[j] = -1;
    }

    grf->current_workspace = 0;
    grf->workspaces[grf->current_workspace].exists = true;
    grf->workspaces[grf->current_workspace].show_message = false;
    grf->workspaces[grf->current_workspace].path = (char*)malloc(PATH_MAX);

    UpdateSizeBasic(grf);

    return grf;
}

void RunBasic(Basic* grf, const int argc, char** argv)
{
    time_t ActualTime, PastTime = 0;
    struct timespec MainTimer;

    int si;

    chdir(getenv("PWD"));
    if (argc > 1)
        CD(argv[1],0,grf);
    else
        CD(".",0,grf);

    bool ccs = 0;

    do {
        clock_gettime(1,&MainTimer);
        ActualTime = MainTimer.tv_sec;

        #ifdef __THREADS_FOR_DIR_ENABLE__

        if (grf->workspaces[grf->current_workspace].win[2] != -1 && GET_DIR(grf->current_workspace,2)->enable)
            ccs = 1;

        if (grf->workspaces[grf->current_workspace].win[0] != -1 && GET_DIR(grf->current_workspace,0)->enable)
            ccs = 1;
        
        if (GET_DIR(grf->current_workspace,1)->enable)
            ccs = 1;
        
        if (ccs)
            timeout(settings->SDelayBetweenFrames);
        else
            timeout(settings->DelayBetweenFrames);
        
        ccs = 0;
        #endif

        DrawBasic(grf,-1);

        if ((si = UpdateEvent(grf)) != -1)
            RunCommand(keys[si].value,grf);

        if (ActualTime != PastTime)
        {
            PastTime = ActualTime;
            GetDir(".",grf,grf->current_workspace,1,settings->DirLoadingMode
                #ifdef __FOLLOW_PARENT_DIR__
                ,NULL
                #endif
                #ifdef __THREADS_FOR_DIR_ENABLE__
                ,settings->ThreadsForDir
                #endif
                );
            if (settings->Win1Display)
                GetDir("..",grf,grf->current_workspace,0,settings->DirLoadingMode
                #ifdef __FOLLOW_PARENT_DIR__
                ,NULL
                #endif
                #ifdef __THREADS_FOR_DIR_ENABLE__
                ,settings->ThreadsForDir
                #endif
                );
        }


    } while (!grf->exit_time);

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
        grf->win_middle = grf->win[0]->_maxx;
        werase(grf->win[0]);
    }
    wresize(grf->win[1],grf->wy-2+!settings->Bar1Enable+!settings->Bar2Enable,(grf->wx*(settings->WinSizeMod[1]*settings->Win3Enable))+(!settings->Win3Enable*(grf->wx-grf->win_middle)));
    mvwin(grf->win[1],1+settings->StatusBarOnTop-!settings->Bar1Enable-(!settings->Bar2Enable*settings->StatusBarOnTop),grf->win_middle);
    werase(grf->win[1]);
    if (settings->Win3Enable)
    {
        wresize(grf->win[2],grf->wy-2+!settings->Bar1Enable+!settings->Bar2Enable,grf->wx-grf->win[1]->_maxx-grf->win_middle);
        mvwin(grf->win[2],1+settings->StatusBarOnTop-!settings->Bar1Enable-(!settings->Bar2Enable*settings->StatusBarOnTop),grf->win[1]->_maxx+grf->win_middle);
        werase(grf->win[2]);
    }

    refresh();
    if (settings->Borders)
        SetBorders(grf,-1);

    for (int i = 0; i < 3; i++)
    {
        if (grf->workspaces[grf->current_workspace].win[i] == -1)
            continue;
        if (i == 0 && (!settings->Win1Enable || !settings->Win1Display))
            continue;
        if (i == 2 && (!settings->Win3Enable
        #ifdef __THREADS_FOR_DIR_ENABLE__
        || GET_DIR(grf->current_workspace,1)->enable
        #endif
        ))
            continue;

        #ifdef __THREADS_FOR_DIR_ENABLE__
        if (GET_DIR(grf->current_workspace,i)->enable)
            continue;
        #endif
        if (GET_DIR(grf->current_workspace,i)->permission_denied)
            continue;
        if (GET_DIR(grf->current_workspace,i)->size == 0)
            continue;

        if (GET_DIR(grf->current_workspace,i)->ltop[grf->current_workspace]+grf->win[i]->_maxy < GET_SELECTED(grf->current_workspace,i))
            GET_DIR(grf->current_workspace,i)->ltop[grf->current_workspace] = GET_SELECTED(grf->current_workspace,i)-grf->win[i]->_maxy;

        for (size_t j = GET_DIR(grf->current_workspace,i)->ltop[grf->current_workspace]; j-GET_DIR(grf->current_workspace,i)->ltop[grf->current_workspace] < (size_t)grf->win[i]->_maxy-(settings->Borders*2)+1; j++)
        {
            if (j == GET_DIR(grf->current_workspace,i)->size)
            {
                if (GET_DIR(grf->current_workspace,i)->ltop[grf->current_workspace] != 0)
                    GET_DIR(grf->current_workspace,i)->ltop[grf->current_workspace] = GET_DIR(grf->current_workspace,i)->size-1-grf->win[i]->_maxy;
                break;
            }
        }
    }
}


void freeEl(struct Element** el, size_t* size)
{
    for (size_t i = 0; i < *size; i++)
        free((*el)[i].name);
    free(*el);
    *el = NULL;
    *size = 0;
}

void freeBasic(Basic* grf)
{
    #ifdef __THREADS_FOR_DIR_ENABLE__
    for (size_t i = 0; i < grf->size; i++)
        if (grf->base[i]->enable)
            pthread_cancel(grf->base[i]->thread);
    #endif

    for (int i = 0; i < 6; i++)
        delwin(grf->win[i]);

    free(grf->typed_keys);
    #ifdef __USER_NAME_ENABLE__
    free(grf->hostn);
    free(grf->usern);
    #endif

    for (int i = 0; i < WORKSPACE_N; i++)
        free(grf->workspaces[i].path);

    for (size_t i = 0; i < grf->size; i++)
    {
        free(grf->base[i]->path);
        free(grf->base[i]->selected);
        free(grf->base[i]->ltop);
        free(grf->base[i]->filter);

        if (grf->base[i]->oldsize > grf->base[i]->size)
            grf->base[i]->size = grf->base[i]->oldsize;

        freeEl(&grf->base[i]->el,&grf->base[i]->size);
    }
    free(grf->base);

    for (size_t i = 0; i < grf->ConsoleHistory.allocated; i++)
        free(grf->ConsoleHistory.history[i]);
    free(grf->ConsoleHistory.history);

    free(grf);
}
