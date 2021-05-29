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
#include "expand.h"
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
    if (keys_t == keys_a) {
        keys = (xkey*)realloc(keys,(keys_a+=32)*sizeof(xkey));
        for (size_t i = keys_t; i < keys_a; i++) {
            keys[i].keys = (wchar_t*)malloc(64*sizeof(wchar_t));
            keys[i].value = NULL;
        }
    }

    wchar_t temp[64];
    atok(c,temp);

    li found = -1;
    for (size_t i = 0; i < keys_t; i++) {
        if (wcscmp(temp,keys[i].keys) == 0) {
            found = (li)i;
            break;
        }
    }

    if (found == -1) found = (li)keys_t++;

    wcpcpy(keys[found].keys,temp);

    if (keys[found].value == NULL)
        keys[found].value = (char*)malloc(PATH_MAX);
    strcpy(keys[found].value,v);
}

void addcommand(const char *name, const uchar type, void *func, const size_t s, void *expand)
{
    if (commandsl == commandsa)
        commands = realloc(commands,(commandsa+=32)*sizeof(struct command));

    li found = -1;
    for (size_t i = 0; i < commandsl; i++) {
        if (strcmp(name,commands[i].name) == 0) {
            found = (li)i;
            break;
        }
    }

    if (found == -1) {
        found = (li)commandsl++;
        commands[found].name = strdup(name);
    }

    commands[found].expand = expand;
    commands[found].type = type;
    free(commands[found].func);
    
    commands[found].func = (type == 'f') ? func : memcpy(malloc(s),func,s);
}

static void commands_init()
{
    addcommand("move",'f',cmd_move,8,NULL);
    addcommand("fastselect",'f',cmd_fastselect,8,NULL);
    addcommand("cd",'f',cmd_cd,8,expand_dir);
    addcommand("rename",'f',cmd_rename,8,NULL);
    addcommand("change_workspace",'f',cmd_change_workspace,8,NULL);
    addcommand("gotop",'f',cmd_gotop,8,NULL); //!
    addcommand("godown",'f',cmd_godown,8,NULL); //!
    #ifdef __FILE_SIZE_ENABLE__
    addcommand("getsize",'f',cmd_getsize,8,NULL);
    #endif
    addcommand("open_with",'f',cmd_open_with,8,expand_shell_commands);
    addcommand("setgroup",'f',___SETGROUP,8,NULL);
    addcommand("select",'f',cmd_select,8,expand_dir);
    addcommand("togglevisual",'f',cmd_togglevisual,8,NULL);
    addcommand("f_mod",'f',cmd_f_mod,8,expand_file);
    addcommand("set",'f',cmd_set,8,expand_options);
    addcommand("map",'f',cmd_map,8,NULL);
    addcommand("search",'f',cmd_search,8,NULL);
    addcommand("load",'f',cmd_load,8,expand_dir);
    addcommand("exec",'f',cmd_exec,8,expand_shell);
    addcommand("quit",'f',cmd_quit,8,NULL);
    addcommand("console",'f',cmd_console,8,NULL);
    addcommand("bulk",'f',cmd_bulk,8,NULL);
    addcommand("alias",'f',cmd_alias,8,NULL);
    #ifdef __LOAD_CONFIG_ENABLE__
    addcommand("source",'f',cmd_source,8,expand_file);
    #endif
    addcommand("filter",'f',cmd_filter,8,NULL);
}

static void keys_init()
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
    addkey("gh","cd ~");
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
    addkey("oz","set SortMethod SORT_ZNAME|SORT_BETTER_FILES");
    addkey("oZ","set SortMethod SORT_ZNAME|SORT_REVERSE|SORT_BETTER_FILES");
    addkey("ox","set SortMethod SORT_LZNAME|SORT_BETTER_FILES");
    addkey("oX","set SortMethod SORT_LZNAME|SORT_REVERSE|SORT_BETTER_FILES");
    #endif
    addkey("f","console -a 'filter '");
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
    addkey("r","console -a 'open_with '");
    addkey("R","load -tm 2");
    addkey(":","console");
    addkey("cd","console -a 'cd '");
    addkey("S","exec -nw ${SHELL}");
    addkey("/","console -a 'search -N '");
    addkey("n","search -n");
    addkey("N","search -p");
}

static void settings_init()
{
    s_FileOpener                    = strcpy(malloc(PATH_MAX),"NULL");
    s_shell                         = strcpy(malloc(PATH_MAX),"sh");
    s_editor                        = strcpy(malloc(PATH_MAX),"vim");
    s_BinaryPreview                 = strcpy(malloc(PATH_MAX),"file");
    s_Values                        = strcpy(malloc(16),"KMGTPEZY");
    s_UserHostPattern               = strcpy(malloc(NAME_MAX),"%s@%s");
    s_WinSizeMod                    = (double*)malloc(sizeof(double)<<1);
    s_WinSizeMod[0]                 = 0.132f;
    s_WinSizeMod[1]                 = 0.368f;
    s_WindowBorder                  = (li*)malloc(8*sizeof(li));
    s_WindowBorder[0]               = 0;
    s_WindowBorder[1]               = 0;
    s_WindowBorder[2]               = 0;
    s_WindowBorder[3]               = 0;
    s_WindowBorder[4]               = 0;
    s_WindowBorder[5]               = 0;
    s_WindowBorder[6]               = 0;
    s_WindowBorder[7]               = 0;
    #ifdef __SORT_ELEMENTS_ENABLE__
    s_BetterFiles                   = (li*)calloc(16,sizeof(li));
    s_BetterFiles[0]                = T_DIR;
    s_BetterFiles[1]                = T_DIR|T_SYMLINK;
    #endif
    s_C_Group                       = malloc(sizeof(ll)*8);
    s_C_Group[0]		            = COLOR_PAIR(3);
    s_C_Group[1]		            = COLOR_PAIR(2);
    s_C_Group[2]		            = COLOR_PAIR(1);
    s_C_Group[3]		            = COLOR_PAIR(4);
    s_C_Group[4]		            = COLOR_PAIR(5);
    s_C_Group[5]		            = COLOR_PAIR(6);
    s_C_Group[6]		            = COLOR_PAIR(7);
    s_C_Group[7]		            = COLOR_PAIR(8);
}

int initcurses()
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

    if (has_colors() && s_EnableColor) {
        start_color();
        use_default_colors();

        for (int i = 0; i < 16; i++)
            init_pair(i,i,-1);
    }

	set_escdelay(25);

    return 0;
}

csas *csas_init()
{
    csas *cs = (csas*)malloc(sizeof(csas));

    #ifdef __FILESYSTEM_INFO_ENABLE__
    statfs(".",&cs->fs);
    #endif

    cs->exit_time = false;
    cs->was_typed = false;
    cs->typed_keys = (char*)malloc(64);

    cs->consoleh.allocated = 0;
    cs->consoleh.size = 0;
    cs->consoleh.history = NULL;
    cs->consoleh.max_size = 32;
    cs->consoleh.alloc_r = 8192;
    cs->consoleh.inc_r = 8;

    cs->searchlist.allocated = 0;
    cs->searchlist.inc_r = 16;
    cs->searchlist.pos = 0;
    cs->searchlist.size = 0;
    cs->searchlist.list = NULL;

    keys_init();
    settings_init();
    commands_init();

    #ifdef __LOAD_CONFIG_ENABLE__
    if (s_config_load) {
        config_load("/etc/csasrc",cs);
        char *HomeTemp = getenv("HOME");
        if (HomeTemp != NULL) {
            char temp[PATH_MAX];
            sprintf(temp,"%s/.csasrc",HomeTemp);
            config_load(temp,cs);
            sprintf(temp,"%s/.config/csas/.csasrc",HomeTemp);
            config_load(temp,cs);
        }
    }
    #endif

    s_Win1Display = s_Win1Enable;
    s_Win3Display = s_Win3Enable;

    #ifndef __SAVE_PREVIEW__
    cs->cpreview = (uchar*)malloc(PREVIEW_MAX);
    #endif

    initcurses();

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

    for (int i = 0; i < WORKSPACE_N; i++) {
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

void csas_run(csas *cs, const int argc, char **argv)
{
    time_t ActualTime, PastTime = 0;
    struct timespec MainTimer;

    int si;
    char *path = NULL;

    chdir(getenv("PWD"));

    if (argc > 1)
        path = argv[1];
    else
        path = ".";
    
    if (csas_cd(path,cs->current_ws,cs) == -1) {
        endwin();
        err(-1,"%s",path);
    }

    do {
        clock_gettime(1,&MainTimer);
        ActualTime = MainTimer.tv_sec;

        #ifdef __THREADS_FOR_DIR_ENABLE__
        bool ccs = 0;
        if (cs->ws[cs->current_ws].win[2] != -1 && G_D(cs->current_ws,2)->enable)
            ccs = 1;

        if (cs->ws[cs->current_ws].win[0] != -1 && G_D(cs->current_ws,0)->enable)
            ccs = 1;
        
        if (G_D(cs->current_ws,1)->enable)
            ccs = 1;
        
        if (ccs)
            timeout(s_SDelayBetweenFrames);
        else
            timeout(s_DelayBetweenFrames);
        #endif

        csas_draw(cs,-1);

        if ((si = update_event(cs)) != -1)
        {
            int r = command_run(keys[si].value,cs);
            if (r != 0)
            {
                char *fc = strchr(keys[si].value,' ');
                if (fc)
                    set_message(cs,s_C_Error,"%s: %s",fc,csas_strerror());
                else
                    set_message(cs,s_C_Error,"%s",csas_strerror());
            }
        }

        if (ActualTime != PastTime)
        {
            PastTime = ActualTime;
            getdir(".",cs,cs->current_ws,1,s_DirLoadingMode
                #ifdef __FOLLOW_PARENT_DIR__
                ,NULL
                #endif
                #ifdef __THREADS_FOR_DIR_ENABLE__
                ,s_ThreadsForDir
                #endif
                );
            if (s_Win1Display)
                getdir("..",cs,cs->current_ws,0,s_DirLoadingMode
                #ifdef __FOLLOW_PARENT_DIR__
                ,NULL
                #endif
                #ifdef __THREADS_FOR_DIR_ENABLE__
                ,s_ThreadsForDir
                #endif
                );
        }


    } while (!cs->exit_time);

    endwin();

    csas_free(cs);
}

void update_size(csas *cs)
{
    clear();
    getmaxyx(stdscr,cs->wy,cs->wx);

    if (s_Bar1Enable) {
        wresize(cs->win[3],1,cs->wx);
        mvwin(cs->win[3],0,1);
        werase(cs->win[3]);
    }
    if (s_Bar2Enable) {
        wresize(cs->win[4],1,cs->wx);
        mvwin(cs->win[4],(cs->wy-1)*!s_StatusBarOnTop+s_StatusBarOnTop-(!s_Bar1Enable*s_StatusBarOnTop),0);
        werase(cs->win[4]);
    }

    if (s_Win1Enable) {
        wresize(cs->win[0],cs->wy-2+!s_Bar1Enable+!s_Bar2Enable,cs->wx*s_WinSizeMod[0]);
        mvwin(cs->win[0],1+s_StatusBarOnTop-!s_Bar1Enable-(!s_Bar2Enable*s_StatusBarOnTop),0);
        cs->win_middle = cs->win[0]->_maxx;
        werase(cs->win[0]);
    }
    wresize(cs->win[1],cs->wy-2+!s_Bar1Enable+!s_Bar2Enable,(cs->wx*(s_WinSizeMod[1]*s_Win3Enable))+(!s_Win3Enable*(cs->wx-cs->win_middle)));
    mvwin(cs->win[1],1+s_StatusBarOnTop-!s_Bar1Enable-(!s_Bar2Enable*s_StatusBarOnTop),cs->win_middle);
    werase(cs->win[1]);
    if (s_Win3Enable) {
        wresize(cs->win[2],cs->wy-2+!s_Bar1Enable+!s_Bar2Enable,cs->wx-cs->win[1]->_maxx-cs->win_middle);
        mvwin(cs->win[2],1+s_StatusBarOnTop-!s_Bar1Enable-(!s_Bar2Enable*s_StatusBarOnTop),cs->win[1]->_maxx+cs->win_middle);
        werase(cs->win[2]);
    }

    refresh();
    if (s_Borders) setborders(cs,-1);

    for (int i = 0; i < 3; i++) {
        if (cs->ws[cs->current_ws].win[i] == -1)
            continue;
        if (i == 0 && (!s_Win1Enable || !s_Win1Display))
            continue;
        if (i == 2 && (!s_Win3Enable
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

        for (size_t j = G_D(cs->current_ws,i)->ltop[cs->current_ws]; j-G_D(cs->current_ws,i)->ltop[cs->current_ws] < (size_t)cs->win[i]->_maxy-(s_Borders*2)+1; j++) {
            if (j == G_D(cs->current_ws,i)->size) {
                if (G_D(cs->current_ws,i)->ltop[cs->current_ws] != 0)
                    G_D(cs->current_ws,i)->ltop[cs->current_ws] = G_D(cs->current_ws,i)->size-1-cs->win[i]->_maxy;
                break;
            }
        }
    }
}


void free_xfile(struct xfile **xf, size_t *size)
{
    for (size_t i = 0; i < *size; i++) {
        free((*xf)[i].name);
        #ifdef __SAVE_PREVIEW__
        free((*xf)[i].cpreview);
        #endif
    }
    free(*xf);
    *xf = NULL;
    *size = 0;
}

void csas_free(csas *cs)
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

    for (size_t i = 0; i < cs->size; i++) {
        free(cs->base[i]->path);
        free(cs->base[i]->selected);
        free(cs->base[i]->ltop);
        free(cs->base[i]->filter);

        if (cs->base[i]->oldsize > cs->base[i]->size)
            cs->base[i]->size = cs->base[i]->oldsize;

        free_xfile(&cs->base[i]->xf,&cs->base[i]->size);
    }
    free(cs->base);

    for (size_t i = 0; i < cs->consoleh.allocated; i++)
        free(cs->consoleh.history[i]);
    free(cs->consoleh.history);

    free(cs);
}
