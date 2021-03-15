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

void addkey(char *c, char *v)
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

    atok(c,temp);

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
    addkey("q","quit");
    addkey("Q","quit -f");
    addkey("j","move -d");
    addkey("J","move -dc 16");
    addkey("k","move -u");
    addkey("K","move -uc 16");
    addkey("h","move -l");
    addkey("l","move -r");
    addkey("g/","cd /");
    addkey("gh","cd \"${HOME}\"");
    addkey("gd","cd /dev");
    addkey("ge","cd /etc");
    addkey("gM","cd /mnt");
    addkey("go","cd /opt");
    addkey("gs","cd /srv");
    addkey("gp","cd /tmp");
    addkey("gu","cd /usr");
    addkey("gv","cd /var");
    addkey("gm","cd \"${MEDIA}\"");
    addkey("gg","gotop");
    addkey("G","godown");
    addkey("z1","change_workspace 0");
    addkey("z2","change_workspace 1");
    addkey("z3","change_workspace 2");
    addkey("z4","change_workspace 3");
    addkey("z5","change_workspace 4");
    addkey("z6","change_workspace 5");
    addkey("z7","change_workspace 6");
    addkey("z8","change_workspace 7");
    addkey("z9","change_workspace 8");
    addkey("z0","change_workspace 9");
    #ifdef __SORT_ELEMENTS_ENABLE__
    addkey("oe","set SortMethod SORT_NONE");
    addkey("oE","set SortMethod SORT_NONE|SORT_REVERSE");
    addkey("or","set SortMethod SORT_TYPE|SORT_BETTER_FILES");
    addkey("oR","set SortMethod SORT_TYPE|SORT_REVERSE|SORT_BETTER_FILES'");
    addkey("ob","set SortMethod SORT_NONE|SORT_BETTER_FILES");
    addkey("oB","set SortMethod SORT_NONE|SORT_REVERSE|SORT_BETTER_FILES");
    addkey("os","set SortMethod SORT_SIZE|SORT_BETTER_FILES");
    addkey("oS","set SortMethod SORT_SIZE|SORT_REVERSE|SORT_BETTER_FILES");
    addkey("otm","set SortMethod SORT_MTIME|SORT_BETTER_FILES");
    addkey("otM","set SortMethod SORT_MTIME|SORT_REVERSE|SORT_BETTER_FILES");
    addkey("otc","set SortMethod SORT_CTIME|SORT_BETTER_FILES");
    addkey("otC","set SortMethod SORT_CTIME|SORT_REVERSE|SORT_BETTER_FILES");
    addkey("ota","set SortMethod SORT_ATIME|SORT_BETTER_FILES");
    addkey("otA","set SortMethod SORT_ATIME|SORT_REVERSE|SORT_BETTER_FILES");
    addkey("og","set SortMethod SORT_GID|SORT_BETTER_FILES");
    addkey("oG","set SortMethod SORT_GID|SORT_REVERSE|SORT_BETTER_FILES");
    addkey("ou","set SortMethod SORT_UID|SORT_BETTER_FILES");
    addkey("oU","set SortMethod SORT_UID|SORT_REVERSE|SORT_BETTER_FILES");
    addkey("om","set SortMethod SORT_LNAME|SORT_BETTER_FILES");
    addkey("oM","set SortMethod SORT_LNAME|SORT_REVERSE|SORT_BETTER_FILES");
    addkey("on","set SortMethod SORT_NAME|SORT_BETTER_FILES");
    addkey("oN","set SortMethod SORT_NAME|SORT_REVERSE|SORT_BETTER_FILES");
    #endif
    addkey("dct","getsize -cs s");
    addkey("dCt","getsize -crs s");
    addkey("dst","getsize -s s");
    addkey("dSt","getsize -rs s");
    addkey("dft","getsize -fs s");
    addkey("dch","getsize -cs .");
    addkey("dCh","getsize -crs .");
    addkey("dsh","getsize -s .");
    addkey("dSh","getsize -rs .");
    addkey("dfh","getsize -fs .");
    addkey("x1","setgroup 0");
    addkey("x2","setgroup 1");
    addkey("x3","setgroup 2");
    addkey("x4","setgroup 3");
    addkey("x5","setgroup 4");
    addkey("x6","setgroup 5");
    addkey("x7","setgroup 6");
    addkey("x8","setgroup 7");
    addkey(" ","fastselect");
    addkey("V","togglevisual");
    addkey("vta","select -ts - -o .");
    addkey("vth","select -ts - -o . .");
    addkey("vda","select -ds - -o .");
    addkey("vdh","select -ds - -o . .");
    addkey("vea","select -es - -o .");
    addkey("veh","select -es - -o . .");
    addkey("mm","f_mod m -s . -c -o .");
    addkey("mr","f_mod m -s . -r -o .");
    addkey("md","f_mod m -s . -d -o .");
    addkey("mM","f_mod m -s . -cm -o .");
    addkey("mR","f_mod m -s . -rm -o .");
    addkey("mD","f_mod m -s . -dm -o .");
    addkey("pp","f_mod c -s . -c -o .");
    addkey("pr","f_mod c -s . -r -o .");
    addkey("pd","f_mod c -s . -d -o .");
    addkey("pP","f_mod c -s . -cm -o .");
    addkey("pR","f_mod c -s . -rm -o .");
    addkey("pD","f_mod c -s . -dm -o .");
    addkey("Dd","f_mod d -s . .");
    addkey("DD","f_mod d -s .");
    addkey("Dt","f_mod d -s s");
    addkey("R","load -tm 2");
    addkey(":","console");
    addkey("cd","console -a 'cd '");
    addkey("s","console -a 'shell '");
    addkey("S","exec bash");
    addkey("b","bulk -S sh -E vim -b mv -s 0 -R .");
    addkey("/","console -a 'search -N '");
    addkey("n","search -n 1");
    addkey("N","search -b 1");
}

Settings *settings_init()
{
    Settings *cfg = (Settings*)malloc(sizeof(Settings));

    #ifdef __THREADS_FOR_DIR_ENABLE__
    cfg->ThreadsForDir = 0;
    #endif
    #ifdef __THREADS_FOR_FILE_ENABLE__
    cfg->ThreadsForFile = 0;
    #endif
    cfg->FileOpener                    = strcpy(malloc(PATH_MAX),"NULL");
    cfg->shell                         = strcpy(malloc(PATH_MAX),"sh");
    cfg->editor                        = strcpy(malloc(PATH_MAX),"vim");
    cfg->BinaryPreview                 = strcpy(malloc(PATH_MAX),"file");
    cfg->Values                        = strcpy(malloc(16),"KMGTPEZY");
    cfg->Bar1Settings                  = B_DIR | B_WORKSPACES | B_POSITION | B_FGROUP | B_MODES | B_CSF;
    cfg->Bar2Settings                  = DP_LSPERMS | DP_SMTIME | DP_HSIZE;
    cfg->UserHostPattern               = strcpy(malloc(NAME_MAX),"%s@%s");
    cfg->UserRHost                     = false;
    cfg->CopyBufferSize                = 131072;
    cfg->MoveOffSet                    = 0.1;
    cfg->WrapScroll                    = true;
    cfg->JumpScroll                    = false;
    cfg->JumpScrollValue               = 0.5;
    cfg->StatusBarOnTop                = false;
    cfg->Win1Enable                    = false;
    cfg->Win3Enable                    = false;
    cfg->Bar1Enable                    = true;
    cfg->Bar2Enable                    = true;
    cfg->WinSizeMod                    = (double*)malloc(2*sizeof(double));
    cfg->WinSizeMod[0]                 = 0.132f;
    cfg->WinSizeMod[1]                 = 0.368f;
    cfg->Borders                       = false;
    cfg->FillBlankSpace                = false;
    cfg->WindowBorder                  = (li*)malloc(8*sizeof(li));
    cfg->WindowBorder[0]               = 0;
    cfg->WindowBorder[1]               = 0;
    cfg->WindowBorder[2]               = 0;
    cfg->WindowBorder[3]               = 0;
    cfg->WindowBorder[4]               = 0;
    cfg->WindowBorder[5]                = 0;
    cfg->WindowBorder[6]                = 0;
    cfg->WindowBorder[7]                = 0;
    cfg->EnableColor                    = true;
    cfg->DelayBetweenFrames             = 1024;
    cfg->SDelayBetweenFrames            = 1;
    cfg->NumberLines                    = false;
    cfg->NumberLinesOff                 = false;
    cfg->NumberLinesFromOne             = false;
    cfg->DirLoadingMode                 = 0;
    cfg->DisplayingC                    = 0;
    cfg->PreviewSettings                = PREV_DIR|PREV_FILE|PREV_ASCII;
    #ifdef __SORT_ELEMENTS_ENABLE__
    cfg->SortMethod                     = SORT_NAME|SORT_BETTER_FILES;
    cfg->BetterFiles                    = (li*)calloc(16,sizeof(li));
    cfg->BetterFiles[0]                 = T_DIR;
    cfg->BetterFiles[1]                 = T_DIR|T_SYMLINK;
    #endif
    cfg->DirSizeMethod                  = D_F;
    cfg->C_Error                        = COLOR_PAIR(1) | A_BOLD | A_REVERSE;
    #ifdef __COLOR_FILES_BY_EXTENSION__
    cfg->C_FType_A                      = COLOR_PAIR(1);
    cfg->C_FType_I                      = COLOR_PAIR(3);
    cfg->C_FType_V                      = COLOR_PAIR(5);
    #endif
    cfg->C_Selected		                = A_REVERSE | A_BOLD;
    cfg->C_Exec_set		                = A_BOLD;
    cfg->C_Exec_col	                    = COLOR_PAIR(2);
    cfg->C_Dir		                    = COLOR_PAIR(4) | A_BOLD;
    cfg->C_Reg		                    = A_NORMAL;
    cfg->C_Fifo		                    = COLOR_PAIR(3) | A_ITALIC;
    cfg->C_Sock		                    = COLOR_PAIR(3) | A_ITALIC;
    cfg->C_Dev		                    = COLOR_PAIR(3);
    cfg->C_BDev		                    = COLOR_PAIR(6);
    cfg->C_Other		                = COLOR_PAIR(0);
    cfg->C_FileMissing                  = COLOR_PAIR(5);
    cfg->C_SymLink                      = COLOR_PAIR(6);
    cfg->C_User_S_D		                = COLOR_PAIR(2) | A_BOLD;
    cfg->C_Bar_Dir		                = COLOR_PAIR(4) | A_BOLD;
    cfg->C_Bar_Name		                = A_NORMAL | A_BOLD;
    cfg->C_Bar_WorkSpace		        = A_NORMAL | A_BOLD;
    cfg->C_Bar_WorkSpace_Selected	    = COLOR_PAIR(2) | A_REVERSE | A_BOLD;
    cfg->C_Group                        = malloc(sizeof(ll)*8);
    cfg->C_Group[0]		                = COLOR_PAIR(3);
    cfg->C_Group[1]		                = COLOR_PAIR(2);
    cfg->C_Group[2]		                = COLOR_PAIR(1);
    cfg->C_Group[3]		                = COLOR_PAIR(4);
    cfg->C_Group[4]		                = COLOR_PAIR(5);
    cfg->C_Group[5]		                = COLOR_PAIR(6);
    cfg->C_Group[6]		                = COLOR_PAIR(7);
    cfg->C_Group[7]		                = COLOR_PAIR(8);
    cfg->C_Bar_E                        = COLOR_PAIR(0);
    cfg->C_Bar_F                        = COLOR_PAIR(0);
    cfg->C_Borders                      = 0;

    return cfg;
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

    if (has_colors() && cfg->EnableColor)
    {
        start_color();
        use_default_colors();

        for (int i = 0; i < 8; i++)
            init_pair(i,i,-1);
    }

    return 0;
}

Csas *csas_init()
{
    Csas *cs = (Csas*)malloc(sizeof(Csas));

    #ifdef __FILESYSTEM_INFO_ENABLE__
    statfs(".",&cs->fs);
    #endif

    cs->exit_time = false;
    cs->was_typed = false;
    cs->typed_keys = (char*)malloc(64);

    cs->consolehistory.allocated = 0;
    cs->consolehistory.size = 0;
    cs->consolehistory.history = NULL;
    cs->consolehistory.max_size = 32;
    cs->consolehistory.alloc_r = 8192;
    cs->consolehistory.inc_r = 8;

    cs->SearchList.allocated = 0;
    cs->SearchList.inc_r = 16;
    cs->SearchList.pos = 0;
    cs->SearchList.size = 0;
    cs->SearchList.list = NULL;

    KeyInit();
    cfg = settings_init();

    #ifdef __LOAD_CONFIG_ENABLE__
    cfg->config_load = 1;
    if (cfg->config_load)
    {
        config_load("/etc/csasrc",cs);
        char *HomeTemp = getenv("HOME");
        if (HomeTemp != NULL)
        {
            char temp[PATH_MAX];
            sprintf(temp,"%s/.csasrc",HomeTemp);
            config_load(temp,cs);
            sprintf(temp,"%s/.config/csas/.csasrc",HomeTemp);
            config_load(temp,cs);
        }
    }
    #endif

    cfg->Win1Display = cfg->Win1Enable;
    cfg->Win3Display = cfg->Win3Enable;

    #ifndef __SAVE_PREVIEW__
    cs->cpreview = (uchar*)malloc(PREVIEW_MAX);
    #endif

    initcurs();

    for (int i = 0; i < 6; i++)
        cs->win[i] = newwin(0,0,0,0);

    cs->win_middle = 0;

    cs->size = 0;
    cs->asize = 0;
    cs->base = NULL;

    #ifdef __USER_NAME_ENABLE__
    cs->hostn = (char*)malloc(64);
    cs->usern = (char*)malloc(64);
    getlogin_r(cs->usern,63);
    gethostname(cs->hostn,63);
    #endif

    for (int i = 0; i < WORKSPACE_N; i++)
    {
        cs->ws[i].path = NULL;
        cs->ws[i].exists = 0;
        cs->ws[i].sel_group = 0;
        cs->ws[i].visual = 0;
        for (int j = 0; j < 3; j++)
            cs->ws[i].win[j] = -1;
    }

    cs->current_ws = 0;
    cs->ws[cs->current_ws].exists = true;
    cs->ws[cs->current_ws].show_message = false;
    cs->ws[cs->current_ws].path = (char*)malloc(PATH_MAX);

    update_size(cs);

    return cs;
}

void csas_run(Csas *cs, const int argc, char* *argv)
{
    time_t ActualTime, PastTime = 0;
    struct timespec MainTimer;

    int si;

    chdir(getenv("PWD"));
    if (argc > 1)
        csas_cd(argv[1],0,cs);
    else
        csas_cd(".",0,cs);

    bool ccs = 0;

    do {
        clock_gettime(1,&MainTimer);
        ActualTime = MainTimer.tv_sec;

        #ifdef __THREADS_FOR_DIR_ENABLE__

        if (cs->ws[cs->current_ws].win[2] != -1 && G_D(cs->current_ws,2)->enable)
            ccs = 1;

        if (cs->ws[cs->current_ws].win[0] != -1 && G_D(cs->current_ws,0)->enable)
            ccs = 1;
        
        if (G_D(cs->current_ws,1)->enable)
            ccs = 1;
        
        if (ccs)
            timeout(cfg->SDelayBetweenFrames);
        else
            timeout(cfg->DelayBetweenFrames);
        
        ccs = 0;
        #endif

        csas_draw(cs,-1);

        if ((si = update_event(cs)) != -1)
            command_run(keys[si].value,cs);

        if (ActualTime != PastTime)
        {
            PastTime = ActualTime;
            getdir(".",cs,cs->current_ws,1,cfg->DirLoadingMode
                #ifdef __FOLLOW_PARENT_DIR__
                ,NULL
                #endif
                #ifdef __THREADS_FOR_DIR_ENABLE__
                ,cfg->ThreadsForDir
                #endif
                );
            if (cfg->Win1Display)
                getdir("..",cs,cs->current_ws,0,cfg->DirLoadingMode
                #ifdef __FOLLOW_PARENT_DIR__
                ,NULL
                #endif
                #ifdef __THREADS_FOR_DIR_ENABLE__
                ,cfg->ThreadsForDir
                #endif
                );
        }


    } while (!cs->exit_time);

    endwin();

    csas_free(cs);
}

void update_size(Csas *cs)
{
    clear();
    getmaxyx(stdscr,cs->wy,cs->wx);

    if (cfg->Bar1Enable)
    {
        wresize(cs->win[3],1,cs->wx);
        mvwin(cs->win[3],0,1);
        werase(cs->win[3]);
    }
    if (cfg->Bar2Enable)
    {
        wresize(cs->win[4],1,cs->wx);
        mvwin(cs->win[4],(cs->wy-1)*!cfg->StatusBarOnTop+cfg->StatusBarOnTop-(!cfg->Bar1Enable*cfg->StatusBarOnTop),0);
        werase(cs->win[4]);
    }

    if (cfg->Win1Enable)
    {
        wresize(cs->win[0],cs->wy-2+!cfg->Bar1Enable+!cfg->Bar2Enable,cs->wx*cfg->WinSizeMod[0]);
        mvwin(cs->win[0],1+cfg->StatusBarOnTop-!cfg->Bar1Enable-(!cfg->Bar2Enable*cfg->StatusBarOnTop),0);
        cs->win_middle = cs->win[0]->_maxx;
        werase(cs->win[0]);
    }
    wresize(cs->win[1],cs->wy-2+!cfg->Bar1Enable+!cfg->Bar2Enable,(cs->wx*(cfg->WinSizeMod[1]*cfg->Win3Enable))+(!cfg->Win3Enable*(cs->wx-cs->win_middle)));
    mvwin(cs->win[1],1+cfg->StatusBarOnTop-!cfg->Bar1Enable-(!cfg->Bar2Enable*cfg->StatusBarOnTop),cs->win_middle);
    werase(cs->win[1]);
    if (cfg->Win3Enable)
    {
        wresize(cs->win[2],cs->wy-2+!cfg->Bar1Enable+!cfg->Bar2Enable,cs->wx-cs->win[1]->_maxx-cs->win_middle);
        mvwin(cs->win[2],1+cfg->StatusBarOnTop-!cfg->Bar1Enable-(!cfg->Bar2Enable*cfg->StatusBarOnTop),cs->win[1]->_maxx+cs->win_middle);
        werase(cs->win[2]);
    }

    refresh();
    if (cfg->Borders)
        setborders(cs,-1);

    for (int i = 0; i < 3; i++)
    {
        if (cs->ws[cs->current_ws].win[i] == -1)
            continue;
        if (i == 0 && (!cfg->Win1Enable || !cfg->Win1Display))
            continue;
        if (i == 2 && (!cfg->Win3Enable
        #ifdef __THREADS_FOR_DIR_ENABLE__
        || G_D(cs->current_ws,1)->enable
        #endif
        ))
            continue;

        #ifdef __THREADS_FOR_DIR_ENABLE__
        if (G_D(cs->current_ws,i)->enable)
            continue;
        #endif
        if (G_D(cs->current_ws,i)->permission_denied)
            continue;
        if (G_D(cs->current_ws,i)->size == 0)
            continue;

        if (G_D(cs->current_ws,i)->ltop[cs->current_ws]+cs->win[i]->_maxy < G_S(cs->current_ws,i))
            G_D(cs->current_ws,i)->ltop[cs->current_ws] = G_S(cs->current_ws,i)-cs->win[i]->_maxy;

        for (size_t j = G_D(cs->current_ws,i)->ltop[cs->current_ws]; j-G_D(cs->current_ws,i)->ltop[cs->current_ws] < (size_t)cs->win[i]->_maxy-(cfg->Borders*2)+1; j++)
        {
            if (j == G_D(cs->current_ws,i)->size)
            {
                if (G_D(cs->current_ws,i)->ltop[cs->current_ws] != 0)
                    G_D(cs->current_ws,i)->ltop[cs->current_ws] = G_D(cs->current_ws,i)->size-1-cs->win[i]->_maxy;
                break;
            }
        }
    }
}


void free_el(struct Element* *el, size_t *size)
{
    for (size_t i = 0; i < *size; i++)
    {
        free((*el)[i].name);
        #ifdef __SAVE_PREVIEW__
        free((*el)[i].cpreview);
        #endif
    }
    free(*el);
    *el = NULL;
    *size = 0;
}

void csas_free(Csas *cs)
{
    #ifdef __THREADS_FOR_DIR_ENABLE__
    for (size_t i = 0; i < cs->size; i++)
        if (cs->base[i]->enable)
            pthread_cancel(cs->base[i]->thread);
    #endif

    for (int i = 0; i < 6; i++)
        delwin(cs->win[i]);

    free(cs->typed_keys);
    #ifdef __USER_NAME_ENABLE__
    free(cs->hostn);
    free(cs->usern);
    #endif

    for (int i = 0; i < WORKSPACE_N; i++)
        free(cs->ws[i].path);

    for (size_t i = 0; i < cs->size; i++)
    {
        free(cs->base[i]->path);
        free(cs->base[i]->selected);
        free(cs->base[i]->ltop);
        free(cs->base[i]->filter);

        if (cs->base[i]->oldsize > cs->base[i]->size)
            cs->base[i]->size = cs->base[i]->oldsize;

        free_el(&cs->base[i]->el,&cs->base[i]->size);
    }
    free(cs->base);

    for (size_t i = 0; i < cs->consolehistory.allocated; i++)
        free(cs->consolehistory.history[i]);
    free(cs->consolehistory.history);

    free(cs);
}
