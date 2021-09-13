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
    ret_errno((size=strlen(keys))>BINDING_KEY_MAX||strlen(value)>PATH_MAX,EOVERFLOW,-1);

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
        bind->keys = malloc(BINDING_KEY_MAX*sizeof(wchar_t));
        bind->value = malloc(PATH_MAX);
        wcscpy(bind->keys,k);
    } else {
        bind = &bind[i];
    }

    strcpy(bind->value,value);

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
    xbind_add("l","file_run %s",b);
    xbind_add("g/","cd /",b);
    xbind_add("gu","cd /usr",b);
    xbind_add("gv","cd /var",b);
    xbind_add("gp","cd /tmp",b);
    xbind_add("gs","cd /srv",b);
    xbind_add("go","cd /opt",b);
    xbind_add("gM","cd /mnt",b);
    xbind_add("ge","cd /etc",b);
    xbind_add("gd","cd /dev",b);
    xbind_add("gh","cd ~",b);
    xbind_add(" ","fastselect",b);
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
    xbind_add("vta","select -ts - -o .",b);
    xbind_add("vth","select -ts - -o . .",b);
    xbind_add("vda","select -ds - -o .",b);
    xbind_add("vdh","select -ds - -o . .",b);
    xbind_add("vea","select -es - -o .",b);
    xbind_add("veh","select -es - -o . .",b);
    xbind_add("dct","ds -cs s",b);
    xbind_add("dCt","ds -crs s",b);
    xbind_add("dst","ds -s s",b);
    xbind_add("dSt","ds -rs s",b);
    xbind_add("dft","ds -fs s",b);
    xbind_add("dch","ds -cs .",b);
    xbind_add("dCh","ds -crs .",b);
    xbind_add("dsh","ds -s .",b);
    xbind_add("dSh","ds -rs .",b);
    xbind_add("dfh","ds -fs .",b);
    xbind_add("mm","fmod -Ms . -c -o .",b);
    xbind_add("mr","fmod -Ms . -r -o .",b);
    xbind_add("md","fmod -Ms . -d -o .",b);
    xbind_add("mM","fmod -Ms . -cm -o .",b);
    xbind_add("mR","fmod -Ms . -rm -o .",b);
    xbind_add("mD","fmod -Ms . -dm -o .",b);
    xbind_add("pp","fmod -Cs . -c -o .",b);
    xbind_add("pr","fmod -Cs . -r -o .",b);
    xbind_add("pd","fmod -Cs . -d -o .",b);
    xbind_add("pP","fmod -Cs . -cm -o .",b);
    xbind_add("pR","fmod -Cs . -rm -o .",b);
    xbind_add("pD","fmod -Cs . -dm -o .",b);
    xbind_add("Dd","fmod -Ds . .",b);
    xbind_add("DD","fmod -Ds .",b);
    xbind_add("Dt","fmod -Ds s",b);
    xbind_add("R","load -m1 .",b);
    xbind_add("a","rename %s",b);
    xbind_add("r","open_with %s",b);
    xbind_add("/","console \"search -N \"",b);
    xbind_add("n","search -n 1",b);
    xbind_add("N","search -p 1",b);
    xbind_add("f","console \"filter -N \"",b);
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
        funcs->name = malloc(FUNCTIONS_NAME_MAX*sizeof(wchar_t));
        strcpy(funcs->name,name);
    } else {
        funcs = &funcs[i];
    }

    if (funcs->type == 'a')
        free(funcs->func);

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
    xfunc_add("file_run",'f',cmd_file_run,expand_file,f);
    xfunc_add("source",'f',cmd_source,expand_file,f);
    xfunc_add("load",'f',cmd_load,expand_dir,f);
    xfunc_add("fastselect",'f',cmd_fastselect,NULL,f);
    xfunc_add("console",'f',cmd_console,NULL,f);
    xfunc_add("tab",'f',cmd_tab,NULL,f);
    xfunc_add("select",'f',cmd_select,NULL,f);
    xfunc_add("exec",'f',cmd_exec,expand_shell,f);
    xfunc_add("open_with",'f',cmd_open_with,NULL,f);
    xfunc_add("ds",'f',cmd_ds,NULL,f);
    xfunc_add("bulk",'f',cmd_bulk,NULL,f);
    xfunc_add("sort",'f',cmd_sort,NULL,f);
    xfunc_add("fmod",'f',cmd_fmod,NULL,f);
    xfunc_add("rename",'f',cmd_rename,expand_file,f);
    xfunc_add("search",'f',cmd_search,NULL,f);
    xfunc_add("filter",'f',cmd_filter,NULL,f);
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
        vars->name = malloc(VARS_NAME_MAX);
        strcpy(vars->name,name);
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
            addr = vars->v;
            if (!vars->v)
                addr = malloc(PATH_MAX);
            // fall through
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
    xvar_add(&LeftWindowSize,"LefetWindowSize",'I',NULL,v);
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
    initcurses();

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

int
csas_run(csas *cs, int argc, char **argv)
{
    char *path = ".";
    if (argc > 1)
        path = argv[1];
    config_load("/etc/csasrc",cs);
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
            if (command_run(BINDINGS[e].value,cs) == -1) {
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
                if (DirLoadingMode == D_MODE_ONCE)
                    d->flags |= S_CHANGED;
                else
                    getdir(d->path,cs->dirs,DirLoadingMode);
            }
        }
    }
    return 0;
}

void
xdir_free(xdir *dir)
{
    flexarr_free(dir->searchlist);
    dir->searchlist = NULL;
    free(dir->path);
    dir->path = NULL;
    dir->plen = 0;
    free(dir->names);
    dir->names = NULL;
    free(dir->files);
    dir->files = NULL;
    dir->size = 0;
}
