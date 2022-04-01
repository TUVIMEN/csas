/*
    csas - console file manager
    Copyright (C) 2020-2022 TUVIMEN <suchora.dominik7@gmail.com>

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
#include "csas.h"
#include "load.h"
#include "functions.h"
#include "useful.h"
#include "preview.h"
#include "calc.h"
#include "draw.h"
#include "expand.h"
#include "config.h"

static void
initcurses()
{
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    timeout(IdleDelay);
    noecho();
    nonl();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    curs_set(FALSE);
    if (Color && has_colors()) {
        short bg;
        start_color();
        use_default_colors();
        bg = -1;
        init_pair(RED,COLOR_RED,bg);
        init_pair(GREEN,COLOR_GREEN,bg);
        init_pair(YELLOW,COLOR_YELLOW,bg);
        init_pair(BLUE,COLOR_BLUE,bg);
        init_pair(CYAN,COLOR_CYAN,bg);
        init_pair(MAGENTA,COLOR_MAGENTA,bg);
        init_pair(WHITE,COLOR_WHITE,bg);
        init_pair(BLACK,COLOR_BLACK,bg);
    }
    atexit((void (*)(void)) endwin);
}

int
xbind_add(const char *keys, const char *value, flexarr *b)
{
    size_t i,size;
    ret_errno(keys==NULL||value==NULL||b==NULL,EINVAL,-1);
    ret_errno((size=strlen(keys))>BINDING_KEY_MAX||strlen(value)>LLINE_MAX,EOVERFLOW,-1);

    wchar_t k[BINDING_KEY_MAX];
    change_keys(k,keys);
    
    xbind *bind = (xbind*)b->v;
    uchar found = 0;
    for (i = 0; i < b->size; i++) {
        if (wcscmp(k,bind[i].keys) == 0) {
            found = 1;
            break;
        }
    }
    
    if (found == 0) {
        bind = (xbind*)flexarr_inc(b);
        bind->keys = wcsdup(k);
    } else {
        free(bind[i].value);
        bind = &bind[i];
    }

    bind->value = strdup(value);
    return 0;
}

static void
add_bindings(flexarr *b)
{
    xbind_add("q","quit",b);
    xbind_add("Q","quit -f",b);
    xbind_add("k","move -d",b);
    xbind_add("j","move -u",b);
    xbind_add("gg","move -s",b);
    xbind_add("G","move -s $",b);
    xbind_add("h","cd ..",b);
    xbind_add("l","file_run %f",b);
    xbind_add("g/","cd /",b);
    xbind_add("gh","cd ~",b);
    xbind_add(":","console",b);
    xbind_add("z1","tab 0",b);
    xbind_add("z2","tab 1",b);
    xbind_add("z3","tab 2",b);
    xbind_add("z4","tab 3",b);
    xbind_add("z5","tab 4",b);
    xbind_add("z6","tab 5",b);
    xbind_add("z7","tab 6",b);
    xbind_add("z8","tab 7",b);
    xbind_add("z9","tab 8",b);
    xbind_add("z0","tab 9",b);
    xbind_add("x1","tab -g0",b);
    xbind_add("x2","tab -g1",b);
    xbind_add("x3","tab -g2",b);
    xbind_add("x4","tab -g3",b);
    xbind_add("x5","tab -g4",b);
    xbind_add("x6","tab -g5",b);
    xbind_add("x7","tab -g6",b);
    xbind_add("x8","tab -g7",b);
    xbind_add("R","scout -l 1",b);
    xbind_add("a","rename %f",b);
    xbind_add("r","open_with %f",b);
    xbind_add("cd","console \"cd \"",b);
    xbind_add("O","sort",b);
}

int
xfunc_add(const char *name, const uchar type, void *func, void *expand, flexarr *f)
{
    ret_errno(name==NULL||f==NULL,EINVAL,-1);
    ret_errno(strlen(name)>FUNCTIONS_NAME_MAX,EOVERFLOW,-1);

    xfunc *funcs = (xfunc*)f->v;

    size_t i;
    uchar found = 0;
    for (i = 0; i < f->size; i++) {
        if (strcmp(name,funcs[i].name) == 0) {
            found = 1;
            break;
        }
    }

    if (found == 0) {
        funcs = (xfunc*)flexarr_inc(f);
        funcs->name = strdup(name);
    } else {
        funcs = &funcs[i];
        if (funcs->type == 'a')
            free(funcs->func);
    }

    funcs->type = type;
    funcs->func = func;
    funcs->expand = expand;

    return 0;
}


static void
add_functions(flexarr *f)
{
    xfunc_add("move",'f',cmd_move,NULL,f);
    xfunc_add("cd",'f',cmd_cd,expand_dir,f);
    xfunc_add("scout",'f',cmd_scout,NULL,f);
    xfunc_add("file_run",'f',cmd_file_run,expand_file,f);
    xfunc_add("source",'f',cmd_source,expand_file,f);
    xfunc_add("console",'f',cmd_console,NULL,f);
    xfunc_add("tab",'f',cmd_tab,NULL,f);
    xfunc_add("exec",'f',cmd_exec,expand_shell,f);
    xfunc_add("open_with",'f',cmd_open_with,NULL,f);
    xfunc_add("sort",'f',cmd_sort,NULL,f);
    xfunc_add("rename",'f',cmd_rename,expand_file,f);
    xfunc_add("lmove",'f',cmd_lmove,NULL,f);
    xfunc_add("map",'f',cmd_map,NULL,f);
    xfunc_add("set",'f',cmd_set,expand_vars,f);
    xfunc_add("alias",'f',cmd_alias,NULL,f);
    xfunc_add("quit",'f',cmd_quit,NULL,f);
}

int
xvar_add(void *addr, const char *name, const uchar type, void *val, flexarr *v)
{
    ret_errno(name==NULL||v==NULL,EINVAL,-1);
    ret_errno(strlen(name)>FUNCTIONS_NAME_MAX,EOVERFLOW,-1);

    xvar *vars = (xvar*)v->v;
    size_t i;
    uchar found = 0;
    for (i = 0; i < v->size; i++) {
        if (strcmp(name,vars[i].name) == 0) {
            found = 1;
            break;
        }
    }

    if (found == 0) {
        vars = (xvar*)flexarr_inc(v);
        vars->name = strdup(name);
        vars->v = NULL;
    } else {
        vars = &vars[i];
        if ((vars->type == 's' || vars->type == 'i') && vars->type != type) {
            free(vars->v);
            vars->v = NULL;
        }
        if (vars->type == 'S') {
            strcpy(vars->v,val);
            return 0;
        }
        if (vars->type == 'I') {
            if (type&0x80)
                *(li*)vars->v = (li)val;
            else
                calc(val,vars->v,v);
            return 0;
        }
    }

    switch (type&~0x80) {
        case 's':
            if (!vars->v && val)
                vars->v = strndup(val,LLINE_MAX);
            break;
        case 'S':
            vars->v = addr;
            if (val)
                strcpy(vars->v,val);
            break;
        case 'i':
            if (!vars->v)
                addr = malloc(sizeof(li));
            else
                addr = vars->v;
            // fall through
        case 'I':
            vars->v = addr;
            if (type&0x80)
                *(li*)vars->v = (li)val;
            else if (val)
                calc(val,vars->v,v);
            break;
    }

    vars->type = type&~0x80;
    return 0;
}

static void
add_vars(flexarr *v)
{
    xvar_add(&BufferSize,"BufferSize",'I',NULL,v);
    xvar_add(&COLS,"COLS",'I',NULL,v);
    xvar_add(&LINES,"LINES",'I',NULL,v);
    xvar_add(&SortMethod,"SortMethod",'I',NULL,v);
    xvar_add(NULL,"s_none",'i'|0x80,(void*)SORT_NONE,v);
    xvar_add(NULL,"s_name",'i'|0x80,(void*)SORT_NAME,v);
    xvar_add(NULL,"s_cname",'i'|0x80,(void*)SORT_CNAME,v);
    xvar_add(NULL,"s_vcname",'i'|0x80,(void*)SORT_VCNAME,v);
    xvar_add(NULL,"s_size",'i'|0x80,(void*)SORT_SIZE,v);
    xvar_add(NULL,"s_mtime",'i'|0x80,(void*)SORT_MTIME,v);
    xvar_add(NULL,"s_type",'i'|0x80,(void*)SORT_TYPE,v);
    xvar_add(NULL,"s_reverse",'i'|0x80,(void*)SORT_REVERSE,v);
    xvar_add(NULL,"s_ddist",'i'|0x80,(void*)SORT_DIR_DISTINCTION,v);
    xvar_add(NULL,"s_lddist",'i'|0x80,(void*)SORT_LDIR_DISTINCTION,v);
    xvar_add(NULL,"s_rddist",'i'|0x80,(void*)SORT_REVERSE_DIR_DISTINCTIONS,v);
    xvar_add(&Visual,"Visual",'I',NULL,v);
    xvar_add(&MoveOffset,"MoveOffset",'I',NULL,v);
    xvar_add(&WrapScroll,"WrapScroll",'I',NULL,v);
    xvar_add(&JumpScroll,"JumpScroll",'I',NULL,v);
    xvar_add(&JumpScrollValue,"JumpScrollValue",'I',NULL,v);
    xvar_add(FileOpener,"FileOpener",'S',NULL,v);
    xvar_add(Editor,"Editor",'S',NULL,v);
    xvar_add(&DirLoadingMode,"DirLoadingMode",'I',NULL,v);
    xvar_add(NULL,"dm_always",'i'|0x80,(void*)D_MODE_ALWAYS,v);
    xvar_add(NULL,"dm_once",'i'|0x80,(void*)D_MODE_ONCE,v);
    xvar_add(NULL,"dm_change",'i'|0x80,(void*)D_MODE_CHANGE,v);
    xvar_add(NULL,"d_recursive",'i'|0x80,(void*)D_RECURSIVE,v);
    xvar_add(NULL,"d_follow",'i'|0x80,(void*)D_FOLLOW,v);
    xvar_add(NULL,"d_chdir",'i'|0x80,(void*)D_CHDIR,v);
    xvar_add(&Color,"Color",'I',NULL,v);
    xvar_add(&HostnameInTitlebar,"HostnameInTitlebar",'I',NULL,v);
    xvar_add(&NumberLines,"NumberLines",'I',NULL,v);
    xvar_add(&NumberLinesOffset,"NumberLinesOffset",'I',NULL,v);
    xvar_add(&NumberLinesStartFrom,"NumberLinesStartFrom",'I',NULL,v);
    xvar_add(&IdleDelay,"IdleDelay",'I',NULL,v);
    xvar_add(&DirSizeMethod,"DirSizeMethod",'I',NULL,v);
    xvar_add(NULL,"d_f",'i'|0x80,(void*)D_F,v);
    xvar_add(NULL,"d_r",'i'|0x80,(void*)D_R,v);
    xvar_add(NULL,"d_c",'i'|0x80,(void*)D_C,v);
    xvar_add(NULL,"d_s",'i'|0x80,(void*)D_S,v);
    xvar_add(&Sel_C,"Sel_C",'I',NULL,v);
    xvar_add(&Reg_C,"Reg_C",'I',NULL,v);
    xvar_add(&Exec_C,"Exec_C",'I',NULL,v);
    xvar_add(&Dir_C,"Dir_C",'I',NULL,v);
    xvar_add(&Link_C,"Link_C",'I',NULL,v);
    xvar_add(&Chr_C,"Chr_C",'I',NULL,v);
    xvar_add(&Blk_C,"Blk_C",'I',NULL,v);
    xvar_add(&Fifo_C,"Fifo_C",'I',NULL,v);
    xvar_add(&Sock_C,"Sock_C",'I',NULL,v);
    xvar_add(&Missing_C,"Missing_C",'I',NULL,v);
    xvar_add(&Other_C,"Other_C",'I',NULL,v);
    xvar_add(&Error_C,"Error_C",'I',NULL,v);
    xvar_add(&Bar_C,"Bar_C",'I',NULL,v);
    xvar_add(&Host_C,"Host_C",'I',NULL,v);
    xvar_add(NULL,"DEFAULT",'i'|0x80,(void*)COLOR_PAIR(DEFAULT),v);
    xvar_add(NULL,"RED",'i'|0x80,(void*)COLOR_PAIR(RED),v);
    xvar_add(NULL,"GREEN",'i'|0x80,(void*)COLOR_PAIR(GREEN),v);
    xvar_add(NULL,"YELLOW",'i'|0x80,(void*)COLOR_PAIR(YELLOW),v);
    xvar_add(NULL,"BLUE",'i'|0x80,(void*)COLOR_PAIR(BLUE),v);
    xvar_add(NULL,"CYAN",'i'|0x80,(void*)COLOR_PAIR(CYAN),v);
    xvar_add(NULL,"MAGENTA",'i'|0x80,(void*)COLOR_PAIR(MAGENTA),v);
    xvar_add(NULL,"WHITE",'i'|0x80,(void*)COLOR_PAIR(WHITE),v);
    xvar_add(NULL,"BLACK",'i'|0x80,(void*)COLOR_PAIR(BLACK),v);
    xvar_add(NULL,"A_NORMAL",'i'|0x80,(void*)A_NORMAL,v);
    xvar_add(NULL,"A_STANDOUT",'i'|0x80,(void*)A_STANDOUT,v);
    xvar_add(NULL,"A_UNDERLINE",'i'|0x80,(void*)A_UNDERLINE,v);
    xvar_add(NULL,"A_REVERSE",'i'|0x80,(void*)A_REVERSE,v);
    xvar_add(NULL,"A_BLINK",'i'|0x80,(void*)A_BLINK,v);
    xvar_add(NULL,"A_DIM",'i'|0x80,(void*)A_DIM,v);
    xvar_add(NULL,"A_BOLD",'i'|0x80,(void*)A_BOLD,v);
    xvar_add(NULL,"A_PROTECT",'i'|0x80,(void*)A_PROTECT,v);
    xvar_add(NULL,"A_INVIS",'i'|0x80,(void*)A_INVIS,v);
    xvar_add(NULL,"A_ALTCHARSET",'i'|0x80,(void*)A_ALTCHARSET,v);
    xvar_add(NULL,"A_ITALIC",'i'|0x80,(void*)A_ITALIC,v);
    xvar_add(NULL,"A_CHARTEXT",'i'|0x80,(void*)A_CHARTEXT,v);
    xvar_add(NULL,"A_COLOR",'i'|0x80,(void*)A_COLOR,v);
    xvar_add(NULL,"WA_HORIZONTAL",'i'|0x80,(void*)WA_HORIZONTAL,v);
    xvar_add(NULL,"WA_LEFT",'i'|0x80,(void*)WA_LEFT,v);
    xvar_add(NULL,"WA_LOW",'i'|0x80,(void*)WA_LOW,v);
    xvar_add(NULL,"WA_RIGHT",'i'|0x80,(void*)WA_RIGHT,v);
    xvar_add(NULL,"WA_TOP",'i'|0x80,(void*)WA_TOP,v);
    xvar_add(NULL,"WA_VERTICAL",'i'|0x80,(void*)WA_VERTICAL,v);
    xvar_add(&Linemode,"Linemode",'I',NULL,v);
    xvar_add(NULL,"l_size",'i'|0x80,(void*)L_SIZE,v);
    xvar_add(NULL,"l_mtime",'i'|0x80,(void*)L_MTIME,v);
    xvar_add(NULL,"l_perms",'i'|0x80,(void*)L_PERMS,v);
    xvar_add(&ColorByExtension,"ColorByExtension",'I',NULL,v);
    xvar_add(&Archive_C,"Archive_C",'I',NULL,v);
    xvar_add(&Image_C,"Image_C",'I',NULL,v);
    xvar_add(&Video_C,"Video_C",'I',NULL,v);
    xvar_add(&UpdateFile,"UpdateFile",'I',NULL,v);
    xvar_add(&SizeInBytes,"SizeInBytes",'I',NULL,v);
    xvar_add(&FileSystemInfo,"FileSystemInfo",'I',NULL,v);
    xvar_add(NULL,"fs_free",'i'|0x80,(void*)FS_FREE,v);
    xvar_add(NULL,"fs_avail",'i'|0x80,(void*)FS_AVAIL,v);
    xvar_add(NULL,"fs_all",'i'|0x80,(void*)FS_ALL,v);
    xvar_add(NULL,"fs_files",'i'|0x80,(void*)FS_FILES,v);
    xvar_add(&MultipaneView,"MultipaneView",'I',NULL,v);
    xvar_add(&FollowParentDir,"FollowParentDir",'I',NULL,v);
    xvar_add(&LeftWindowSize,"LeftWindowSize",'I',NULL,v);
    xvar_add(&CenterWindowSize,"CenterWindowSize",'I',NULL,v);
    xvar_add(&RightWindowSize,"RightWindowSize",'I',NULL,v);
    xvar_add(&Border_C,"Border_C",'I',NULL,v);
    xvar_add(&Borders,"Borders",'I',NULL,v);
    xvar_add(NULL,"b_none",'i'|0x80,(void*)B_NONE,v);
    xvar_add(NULL,"b_separators",'i'|0x80,(void*)B_SEPARATORS,v);
    xvar_add(NULL,"b_outline",'i'|0x80,(void*)B_OUTLINE,v);
    xvar_add(NULL,"b_all",'i'|0x80,(void*)B_ALL,v);
    xvar_add(&ShowKeyBindings,"ShowKeyBindings",'I',NULL,v);
    xvar_add(&PreviewSettings,"PreviewSettings",'I',NULL,v);
    xvar_add(NULL,"p_dir",'i'|0x80,(void*)P_DIR,v);
    xvar_add(NULL,"p_file",'i'|0x80,(void*)P_FILE,v);
    xvar_add(NULL,"p_bfile",'i'|0x80,(void*)P_BFILE,v);
    xvar_add(NULL,"p_wrap",'i'|0x80,(void*)P_WRAP,v);
    xvar_add(BinaryPreview,"BinaryPreview",'S',NULL,v);
    xvar_add(&OpenAllImages,"OpenAllImages",'I',NULL,v);
}

static void
wins_resize(WINDOW **wins)
{
    draw_borders();
    int outline=0;
    if (Borders&B_OUTLINE)
        outline = 1;
    if (!MultipaneView) {
        wins[1] = subwin(stdscr,LINES-2-(outline<<1),COLS-(outline<<1),1+outline,outline);
        delwin(wins[0]);
        wins[0] = 0;
        delwin(wins[2]);
        wins[2] = 0;
        return;
    }

    li sum = CenterWindowSize+LeftWindowSize+RightWindowSize,t1,t2;
    t1 = (COLS/sum)*LeftWindowSize;
    t2 = t1+1;
    wins[0] = subwin(stdscr,LINES-2-(outline<<1),t1-outline,1+outline,outline);
    t1 = (COLS/sum)*CenterWindowSize;
    wins[1] = subwin(stdscr,LINES-2-(outline<<1),t1,1+outline,t2);
    t2 += t1+1;
    wins[2] = subwin(stdscr,LINES-2-(outline<<1),COLS-t2-outline,1+outline,t2);
}

csas *
csas_init()
{
    csas *ret = malloc(sizeof(csas));
    memset(ret->tabs,0,sizeof(xtab)*TABS);
    ret->ctab = 0;
    ret->dirs = flexarr_init(sizeof(xdir),DIR_INCR);
    ret->consoleh = flexarr_init(sizeof(char**),HISTORY_INCR);
    ret->vars = flexarr_init(sizeof(xvar),VARS_INCR);
    ret->functions = flexarr_init(sizeof(xfunc),FUNCTIONS_INCR);
    ret->bindings = flexarr_init(sizeof(xbind),BINDINGS_INCR);
    ret->args = flexarr_init(sizeof(char**),ARGS_INCR);

    add_functions(ret->functions);
    add_bindings(ret->bindings);
    add_vars(ret->vars);

    gethostname(hostname,NAME_MAX);
    username = getenv("USER");

    return ret;
}

void
csas_draw(csas *cs)
{
    if (MultipaneView && (!cs->wins[0] || !cs->wins[2])) {
        csas_resize(cs);
        csas_cd(".",cs);
    }
    if (!MultipaneView && (cs->wins[0] || cs->wins[2]))
        csas_resize(cs);
    draw_tbar(0,cs);
    draw_bbar(LINES-1,cs);
    xdir *d = &CTAB(1);
    if (Visual && d->size)
        d->files[d->sel[cs->ctab]].sel[cs->ctab] |= 1<<cs->tabs[cs->ctab].sel;
    draw_dir(cs->wins[1],d,cs);
    if (MultipaneView) {
        if (d->path[0] == '/' && d->path[1] == 0) {
            werase(cs->wins[0]);
            wrefresh(cs->wins[0]);
        } else {
            draw_dir(cs->wins[0],&CTAB(0),cs);
        }
        if (CTAB(1).size == 0) {
            werase(cs->wins[2]);
            wrefresh(cs->wins[2]);
        } else {
            preview_draw(cs->wins[2],cs);
        }
    }
}

void
csas_resize(csas *cs)
{
    delwin(cs->wins[1]);
    if (MultipaneView) {
        delwin(cs->wins[0]);
        delwin(cs->wins[2]);
    }
    endwin();
    refresh();
    clear();
    wins_resize(cs->wins);
    csas_draw(cs);
}

static void
searchfor(const char *name, const size_t tab, xdir *d)
{
    size_t nlen = strlen(name),i;
    if (nlen == 0)
        return;
    xfile *files = d->files;
    i = d->sel[tab];
    if (i != 0 && nlen == files[i].nlen
        && memcmp(name,files[i].name,nlen) == 0)
        return;
    for (i = 0; i < d->size; i++) {
        if (nlen == files[i].nlen
            && memcmp(name,files[i].name,nlen) == 0) {
            d->sel[tab] = i;
            break;
        }
    }
}

int
csas_cd(const char *path, csas* cs)
{
    char *search_name = NULL;
    xdir *dir = &CTAB(1);
    if (FollowParentDir && cs->dirs->size && path[0] == '.' && path[1] == '.' && path[2] == 0)
        search_name = memrchr(dir->path,'/',dir->plen);

    li n = getdir(path,cs->dirs,DirLoadingMode|D_CHDIR);
    if (n == -1)
        return -1;
    cs->tabs[cs->ctab].wins[1] = (size_t)n;
    dir = &CTAB(1);
    if (search_name)
        searchfor(++search_name,cs->ctab,dir);
    if (MultipaneView && dir->size)
        preview_get(&dir->files[dir->sel[cs->ctab]],cs);
    if (MultipaneView && (dir->path[0] != '/' || dir->path[1] != 0)) {
        n = getdir("..",cs->dirs,DirLoadingMode);
        if (n == -1)
            return -1;
        cs->tabs[cs->ctab].wins[0] = (size_t)n;
        if (FollowParentDir) {
            search_name = memrchr(dir->path,'/',dir->plen);
            dir = &CTAB(0);
            if (search_name)
                searchfor(++search_name,cs->ctab,dir);
        }
    }
    return 0;
}

static void
usage(const char *argv0)
{
    fprintf(stderr,"Usage: %s [OPTION]... [PATH]\n\n"\
      "Options:\n"\
      "  -f FILE\tuse FILE as configuration file\n" \
      "  -c\t\tdo not load configuration file\n" \
      "  -h\t\tshow help\n" \
      "  -v\t\tshow version\n",argv0);
    exit(1);
}

int
csas_run(csas *cs, int argc, char **argv)
{
    char *path = ".", cf[PATH_MAX];
    char *conf = getenv("CSAS_HOME");
    opterr = 0;
    if (!conf) {
        size_t s;
        conf = getenv("XDG_CONFIG_HOME");
        if (conf) {
            s = strlen(conf);
            memcpy(cf,conf,s);
            strcpy(cf+s,"/csasrc");
            conf = cf;
        } else {
            conf = getenv("HOME");
            if (conf) {
                s = strlen(conf);
                memcpy(cf,conf,s);
                strcpy(cf+s,"/.csasrc");
                conf = cf;
            } else {
                conf = "/etc/csasrc";
            }
        }
    }

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            for (int j=1,brk=0; argv[i][j] && !brk; j++) {
                switch (argv[i][j]) {
                    case 'v':
                        fprintf(stderr,"%s\n",VERSION);
                        exit(1);
                        break;
                    case 'h': usage(argv[0]); break;
                    case 'c': conf = NULL; break;
                    case 'f':
                        if (argv[i][++j]) {
                            conf = argv[i]+j;
                        } else if (i < argc) {
                            conf = argv[++i];
                            if (i == argc) {
                                fprintf(stderr,"%s: option requires an argument -- 'f'\n",argv[0]);
                                exit(1);
                            }
                        }
                        brk = 1;
                        break;
                    default:
                        fprintf(stderr,"%s: invalid option -- '%c'\n",argv[0],argv[i][j]);
                        exit(1);
                }
            }
        } else {
            path = argv[i];
        }
    }

    config_load(conf,cs);
    initcurses();
    wins_resize(cs->wins);
    if (csas_cd(path,cs) == -1)
        return -1;
    cs->tabs[cs->ctab].flags |= T_EXISTS;

    struct timespec timer;
    int e;
    time_t t1,t2=0;
    struct stat statbuf;

    if (FileSystemInfo)
        statfs(".",&cs->fs);

    while (!Exit) {
        clock_gettime(1,&timer);
        t1 = timer.tv_sec;

        csas_draw(cs);

        REPEAT: ;
        if ((e = update_event(cs)) != -1) {
            if (alias_run(BINDINGS[e].value,strlen(BINDINGS[e].value),cs) == -1) {
                printmsg(Error_C,"%s: %s",BINDINGS[e].value,strerror(errno));
                refresh();
                goto REPEAT;
            }
            if (FileSystemInfo)
                statfs(".",&cs->fs);
        }

        if (UpdateFile) {
            register xdir *d = &CTAB(1);
            if (d->files)
                xfile_update(&d->files[d->sel[cs->ctab]]);
        }

        if (t1 != t2) {
            register xdir *d = &CTAB(1);
            t2 = t1;
            if (lstat(d->path,&statbuf) != 0)
                continue;
            if (memcmp(&statbuf.st_ctim,&d->ctime,sizeof(struct timespec)) != 0) {
                if (DirLoadingMode&D_MODE_ONCE)
                    d->flags |= S_CHANGED;
                else
                    getdir(d->path,cs->dirs,DirLoadingMode);
            }
        }
    }
    endwin();
    return 0;
}

static void
xbind_free(xbind *b)
{
    free(b->keys);
    free(b->value);
}

static void
xfunc_free(xfunc *f)
{
    free(f->name);
    if (f->type == 'a')
        free(f->func);
}

static void
xvar_free(xvar *v)
{
    free(v->name);
    if (v->type == 's' || v->type == 'i')
        free(v->v);
}

static void
xdir_free(xdir *dir)
{
    flexarr_free(dir->searchlist);
    free(dir->path);
    if (dir->asize)
        free(dir->names);
    if (dir->files)
        free(dir->files);
}

void
csas_free(csas *cs)
{
    size_t i;
    delwin(cs->wins[0]);
    delwin(cs->wins[1]);
    delwin(cs->wins[2]);
    for (i = 0; i < cs->vars->size; i++)
        xvar_free(&(((xvar*)cs->vars->v)[i]));
    flexarr_free(cs->vars);
    for (i = 0; i < cs->functions->size; i++)
        xfunc_free(&(((xfunc*)cs->functions->v)[i]));
    flexarr_free(cs->functions);
    for (i = 0; i < cs->bindings->size; i++)
        xbind_free(&(((xbind*)cs->bindings->v)[i]));
    flexarr_free(cs->bindings);
    for (i = 0; i < cs->args->size; i++)
        free((((char**)cs->args->v)[i]));
    flexarr_free(cs->args);
    for (i = 0; i < cs->consoleh->size; i++)
        free(((char**)cs->consoleh->v)[i]);
    flexarr_free(cs->consoleh);
    for (i = 0; i < cs->dirs->size; i++)
        xdir_free(&(((xdir*)cs->dirs->v)[i]));
    flexarr_free(cs->dirs);
    free(cs);
}
