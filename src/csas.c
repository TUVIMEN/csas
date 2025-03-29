/*
    csas - console file manager
    Copyright (C) 2020-2025 Dominik Stanisław Suchora <suchora.dominik7@gmail.com>

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

int
xbind_add(char *keys, const char *value, flexarr *b)
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
    xfunc_add("trap",'f',cmd_trap,NULL,f);
    xfunc_add("sort",'f',cmd_sort,NULL,f);
    xfunc_add("rename",'f',cmd_rename,expand_file,f);
    xfunc_add("lmove",'f',cmd_lmove,NULL,f);
    xfunc_add("map",'f',cmd_map,NULL,f);
    xfunc_add("unmap",'f',cmd_unmap,NULL,f);
    xfunc_add("set",'f',cmd_set,expand_vars,f);
    xfunc_add("unset",'f',cmd_unset,expand_vars,f);
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

    if (!found) {
        vars = (xvar*)flexarr_inc(v);
        vars->name = strdup(name);
        vars->v = NULL;
    } else {
        vars = &vars[i];

        if ((vars->type == XVAR_STRING || vars->type == XVAR_INT) && vars->type != type) {
            free(vars->v);
            vars->v = NULL;
        }
        if (vars->type == (XVAR_STRING|XVAR_POINTER)) {
            strcpy(vars->v,val);
            return 0;
        }
        if (vars->type == (XVAR_INT|XVAR_POINTER)) {
            if (type&XVAR_CONST)
                *(li*)vars->v = (li)val;
            else
                calc(val,vars->v,v);
            return 0;
        }
    }

    switch (type&~XVAR_CONST) {
        case XVAR_STRING:
            if (!vars->v && val)
                vars->v = strndup(val,LLINE_MAX);
            break;
        case XVAR_STRING|XVAR_POINTER:
            vars->v = addr;
            if (val)
                strcpy(vars->v,val);
            break;
        case XVAR_INT:
            addr = (vars->v) ? vars->v : xmalloc(sizeof(li));
            // fall through
        case XVAR_INT|XVAR_POINTER:
            vars->v = addr;
            if (type&XVAR_CONST)
                *(li*)vars->v = (li)val;
            else if (val)
                calc(val,vars->v,v);
            break;
    }

    vars->type = type&~XVAR_CONST;
    return 0;
}

static void
add_vars(flexarr *v)
{
    xvar_add(&BufferSize,"BufferSize",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&COLS,"COLS",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&LINES,"LINES",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&SortMethod,"SortMethod",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(NULL,"s_none",XVAR_INT|XVAR_CONST,(void*)SORT_NONE,v);
    xvar_add(NULL,"s_name",XVAR_INT|XVAR_CONST,(void*)SORT_NAME,v);
    xvar_add(NULL,"s_cname",XVAR_INT|XVAR_CONST,(void*)SORT_CNAME,v);
    xvar_add(NULL,"s_vcname",XVAR_INT|XVAR_CONST,(void*)SORT_VCNAME,v);
    xvar_add(NULL,"s_size",XVAR_INT|XVAR_CONST,(void*)SORT_SIZE,v);
    xvar_add(NULL,"s_mtime",XVAR_INT|XVAR_CONST,(void*)SORT_MTIME,v);
    xvar_add(NULL,"s_type",XVAR_INT|XVAR_CONST,(void*)SORT_TYPE,v);
    xvar_add(NULL,"s_reverse",XVAR_INT|XVAR_CONST,(void*)SORT_REVERSE,v);
    xvar_add(NULL,"s_ddist",XVAR_INT|XVAR_CONST,(void*)SORT_DIR_DISTINCTION,v);
    xvar_add(NULL,"s_lddist",XVAR_INT|XVAR_CONST,(void*)SORT_LDIR_DISTINCTION,v);
    xvar_add(NULL,"s_rddist",XVAR_INT|XVAR_CONST,(void*)SORT_REVERSE_DIR_DISTINCTIONS,v);
    xvar_add(&Visual,"Visual",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&MoveOffset,"MoveOffset",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&WrapScroll,"WrapScroll",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&JumpScroll,"JumpScroll",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&JumpScrollValue,"JumpScrollValue",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(FileOpener,"FileOpener",XVAR_STRING|XVAR_POINTER,NULL,v);
    xvar_add(Editor,"Editor",XVAR_STRING|XVAR_POINTER,NULL,v);
    xvar_add(Shell,"Shell",XVAR_STRING|XVAR_POINTER,NULL,v);
    xvar_add(&DirLoadingMode,"DirLoadingMode",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(NULL,"dm_always",XVAR_INT|XVAR_CONST,(void*)D_MODE_ALWAYS,v);
    xvar_add(NULL,"dm_once",XVAR_INT|XVAR_CONST,(void*)D_MODE_ONCE,v);
    xvar_add(NULL,"dm_change",XVAR_INT|XVAR_CONST,(void*)D_MODE_CHANGE,v);
    xvar_add(NULL,"d_recursive",XVAR_INT|XVAR_CONST,(void*)D_RECURSIVE,v);
    xvar_add(NULL,"d_follow",XVAR_INT|XVAR_CONST,(void*)D_FOLLOW,v);
    xvar_add(NULL,"d_chdir",XVAR_INT|XVAR_CONST,(void*)D_CHDIR,v);
    xvar_add(&Color,"Color",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&HostnameInTitlebar,"HostnameInTitlebar",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&NumberLines,"NumberLines",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&NumberLinesOffset,"NumberLinesOffset",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&NumberLinesStartFrom,"NumberLinesStartFrom",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&IdleDelay,"IdleDelay",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&DirSizeMethod,"DirSizeMethod",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(NULL,"d_f",XVAR_INT|XVAR_CONST,(void*)D_F,v);
    xvar_add(NULL,"d_r",XVAR_INT|XVAR_CONST,(void*)D_R,v);
    xvar_add(NULL,"d_c",XVAR_INT|XVAR_CONST,(void*)D_C,v);
    xvar_add(NULL,"d_s",XVAR_INT|XVAR_CONST,(void*)D_S,v);
    xvar_add(&Sel_C,"Sel_C",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&Reg_C,"Reg_C",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&Exec_C,"Exec_C",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&Dir_C,"Dir_C",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&Link_C,"Link_C",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&Chr_C,"Chr_C",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&Blk_C,"Blk_C",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&Fifo_C,"Fifo_C",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&Sock_C,"Sock_C",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&Missing_C,"Missing_C",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&Other_C,"Other_C",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&Error_C,"Error_C",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&Bar_C,"Bar_C",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&Path_C,"Path_C",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&Host_C,"Host_C",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(NULL,"DEFAULT",XVAR_INT|XVAR_CONST,(void*)COLOR_PAIR(DEFAULT),v);
    xvar_add(NULL,"RED",XVAR_INT|XVAR_CONST,(void*)COLOR_PAIR(RED),v);
    xvar_add(NULL,"GREEN",XVAR_INT|XVAR_CONST,(void*)COLOR_PAIR(GREEN),v);
    xvar_add(NULL,"YELLOW",XVAR_INT|XVAR_CONST,(void*)COLOR_PAIR(YELLOW),v);
    xvar_add(NULL,"BLUE",XVAR_INT|XVAR_CONST,(void*)COLOR_PAIR(BLUE),v);
    xvar_add(NULL,"CYAN",XVAR_INT|XVAR_CONST,(void*)COLOR_PAIR(CYAN),v);
    xvar_add(NULL,"MAGENTA",XVAR_INT|XVAR_CONST,(void*)COLOR_PAIR(MAGENTA),v);
    xvar_add(NULL,"WHITE",XVAR_INT|XVAR_CONST,(void*)COLOR_PAIR(WHITE),v);
    xvar_add(NULL,"BLACK",XVAR_INT|XVAR_CONST,(void*)COLOR_PAIR(BLACK),v);
    xvar_add(NULL,"A_NORMAL",XVAR_INT|XVAR_CONST,(void*)A_NORMAL,v);
    xvar_add(NULL,"A_STANDOUT",XVAR_INT|XVAR_CONST,(void*)A_STANDOUT,v);
    xvar_add(NULL,"A_UNDERLINE",XVAR_INT|XVAR_CONST,(void*)A_UNDERLINE,v);
    xvar_add(NULL,"A_REVERSE",XVAR_INT|XVAR_CONST,(void*)A_REVERSE,v);
    xvar_add(NULL,"A_BLINK",XVAR_INT|XVAR_CONST,(void*)A_BLINK,v);
    xvar_add(NULL,"A_DIM",XVAR_INT|XVAR_CONST,(void*)A_DIM,v);
    xvar_add(NULL,"A_BOLD",XVAR_INT|XVAR_CONST,(void*)A_BOLD,v);
    xvar_add(NULL,"A_PROTECT",XVAR_INT|XVAR_CONST,(void*)A_PROTECT,v);
    xvar_add(NULL,"A_INVIS",XVAR_INT|XVAR_CONST,(void*)A_INVIS,v);
    xvar_add(NULL,"A_ALTCHARSET",XVAR_INT|XVAR_CONST,(void*)A_ALTCHARSET,v);
    xvar_add(NULL,"A_ITALIC",XVAR_INT|XVAR_CONST,(void*)A_ITALIC,v);
    xvar_add(NULL,"A_CHARTEXT",XVAR_INT|XVAR_CONST,(void*)A_CHARTEXT,v);
    xvar_add(NULL,"A_COLOR",XVAR_INT|XVAR_CONST,(void*)A_COLOR,v);
    xvar_add(NULL,"WA_HORIZONTAL",XVAR_INT|XVAR_CONST,(void*)WA_HORIZONTAL,v);
    xvar_add(NULL,"WA_LEFT",XVAR_INT|XVAR_CONST,(void*)WA_LEFT,v);
    xvar_add(NULL,"WA_LOW",XVAR_INT|XVAR_CONST,(void*)WA_LOW,v);
    xvar_add(NULL,"WA_RIGHT",XVAR_INT|XVAR_CONST,(void*)WA_RIGHT,v);
    xvar_add(NULL,"WA_TOP",XVAR_INT|XVAR_CONST,(void*)WA_TOP,v);
    xvar_add(NULL,"WA_VERTICAL",XVAR_INT|XVAR_CONST,(void*)WA_VERTICAL,v);
    xvar_add(&Linemode,"Linemode",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(NULL,"l_size",XVAR_INT|XVAR_CONST,(void*)L_SIZE,v);
    xvar_add(NULL,"l_mtime",XVAR_INT|XVAR_CONST,(void*)L_MTIME,v);
    xvar_add(NULL,"l_perms",XVAR_INT|XVAR_CONST,(void*)L_PERMS,v);
    xvar_add(&ColorByExtension,"ColorByExtension",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&Archive_C,"Archive_C",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&Image_C,"Image_C",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&Video_C,"Video_C",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&UpdateFile,"UpdateFile",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&SizeInBytes,"SizeInBytes",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&FileSystemInfo,"FileSystemInfo",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(NULL,"fs_free",XVAR_INT|XVAR_CONST,(void*)FS_FREE,v);
    xvar_add(NULL,"fs_avail",XVAR_INT|XVAR_CONST,(void*)FS_AVAIL,v);
    xvar_add(NULL,"fs_all",XVAR_INT|XVAR_CONST,(void*)FS_ALL,v);
    xvar_add(NULL,"fs_files",XVAR_INT|XVAR_CONST,(void*)FS_FILES,v);
    xvar_add(&MultipaneView,"MultipaneView",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&FollowParentDir,"FollowParentDir",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&LeftWindowSize,"LeftWindowSize",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&CenterWindowSize,"CenterWindowSize",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&RightWindowSize,"RightWindowSize",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&Border_C,"Border_C",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&Borders,"Borders",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(NULL,"b_none",XVAR_INT|XVAR_CONST,(void*)B_NONE,v);
    xvar_add(NULL,"b_separators",XVAR_INT|XVAR_CONST,(void*)B_SEPARATORS,v);
    xvar_add(NULL,"b_outline",XVAR_INT|XVAR_CONST,(void*)B_OUTLINE,v);
    xvar_add(NULL,"b_all",XVAR_INT|XVAR_CONST,(void*)B_ALL,v);
    xvar_add(&ShowKeyBindings,"ShowKeyBindings",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&PreviewSettings,"PreviewSettings",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(NULL,"p_dir",XVAR_INT|XVAR_CONST,(void*)P_DIR,v);
    xvar_add(NULL,"p_file",XVAR_INT|XVAR_CONST,(void*)P_FILE,v);
    xvar_add(NULL,"p_bfile",XVAR_INT|XVAR_CONST,(void*)P_BFILE,v);
    xvar_add(NULL,"p_wrap",XVAR_INT|XVAR_CONST,(void*)P_WRAP,v);
    xvar_add(NULL,"p_trap",XVAR_INT|XVAR_CONST,(void*)P_TRAP,v);
    xvar_add(BinaryPreview,"BinaryPreview",XVAR_STRING|XVAR_POINTER,NULL,v);
    xvar_add(&OpenAllImages,"OpenAllImages",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&PreviewWidth,"PreviewWidth",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&PreviewHeight,"PreviewHeight",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&PreviewPosx,"PreviewPosx",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&PreviewPosy,"PreviewPosy",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&Verbose,"Verbose",XVAR_INT|XVAR_POINTER,NULL,v);
    xvar_add(&PID,"PID",XVAR_INT|XVAR_CONST,(void*)getpid(),v);

    struct timespec timer;
    clock_gettime(CLOCK_REALTIME,&timer);
    xvar_add(&PID,"StartTime",XVAR_INT|XVAR_CONST,(void*)timer.tv_sec,v);
}

void
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
    PreviewHeight = LINES-2-(outline<<1);
    PreviewWidth = COLS-t2-outline;
    PreviewPosy = 1+outline;
    PreviewPosx = t2;
    wins[2] = subwin(stdscr,PreviewHeight,PreviewWidth,PreviewPosy,PreviewPosx);
}

csas *
csas_init()
{
    csas *ret = xmalloc(sizeof(csas));
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

    trap_exit = flexarr_init(sizeof(char**),TRAP_INCR);
    trap_preview = flexarr_init(sizeof(char**),TRAP_INCR);
    trap_chdir = flexarr_init(sizeof(char**),TRAP_INCR);
    trap_newdir = flexarr_init(sizeof(char**),TRAP_INCR);
    trap_filerun = flexarr_init(sizeof(char**),TRAP_INCR);

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
    if (Visual && d->files->size)
        ((xfile*)d->files->v)[d->sel[cs->ctab]].sel[cs->ctab] |= 1<<cs->tabs[cs->ctab].sel;
    draw_dir(cs->wins[1],d,cs);
    if (MultipaneView) {
        if (d->path[0] == '/' && d->path[1] == 0) {
            werase(cs->wins[0]);
            xwrefresh(cs->wins[0]);
        } else {
            draw_dir(cs->wins[0],&CTAB(0),cs);
        }
        if (CTAB(1).files->size == 0) {
            werase(cs->wins[2]);
            xwrefresh(cs->wins[2]);
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
    xfile *files = (xfile*)d->files->v;
    size_t filesl = d->files->size;
    i = d->sel[tab];
    if (i != 0 && nlen == files[i].nlen
        && memcmp(name,files[i].name,nlen) == 0)
        return;
    for (i = 0; i < filesl; i++) {
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
    li n = getdir(path,cs->dirs,DirLoadingMode|D_CHDIR);
    if (n == -1)
        return -1;

    xdir *dir = cs->dirs->size ? &CTAB(1) : NULL;
    size_t size = cs->dirs->size;

    if (FollowParentDir && size && path[0] == '.' && path[1] == '.' && path[2] == 0)
        search_name = memrchr(dir->path,'/',dir->plen);

    cs->tabs[cs->ctab].wins[1] = (size_t)n;
    dir = &CTAB(1);
    xfile *files = (xfile*)dir->files->v;
    size_t filesl = dir->files->size;

    if ((size_t)n == size)
        trap_run(trap_newdir,cs);
    trap_run(trap_chdir,cs);

    if (search_name)
        searchfor(++search_name,cs->ctab,dir);
    if (MultipaneView && filesl) {
        size = cs->dirs->size;
        preview_get(&files[dir->sel[cs->ctab]],cs);
        if (cs->dirs->size > size)
            trap_run(trap_newdir,cs);
    }
    if (MultipaneView && (dir->path[0] != '/' || dir->path[1] != 0)) {
        size = cs->dirs->size;
        n = getdir("..",cs->dirs,DirLoadingMode);
        if (n == -1)
            return -1;
        if ((size_t)n == size)
            trap_run(trap_newdir,cs);
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
    flexarr_free(dir->files);
    if (dir->names) {
        flexarr *names = dir->names;
        flexarr **namesv = (flexarr**)names->v;
        for (size_t i = 0; i < names->size; i++)
            flexarr_free(namesv[i]);
        flexarr_free(names);
    }
}

void
csas_free(csas *cs)
{
    trap_run(trap_exit,cs);

    size_t i;
    delwin(cs->wins[0]);
    delwin(cs->wins[1]);
    delwin(cs->wins[2]);

    #define xfree(x,y,z)  for (i = 0; i < (x)->size; i++) \
            (y)(&(((z)(x)->v)[i])); \
        flexarr_free(x);
    #define xfree_(x,y)  for (i = 0; i < (x)->size; i++) \
            (y)((((char**)(x)->v)[i])); \
        flexarr_free(x);

    xfree_(trap_exit,free);
    xfree_(trap_preview,free);
    xfree_(trap_chdir,free);
    xfree_(trap_newdir,free);
    xfree_(trap_filerun,free);

    xfree(cs->vars,xvar_free,xvar*);
    xfree(cs->functions,xfunc_free,xfunc*);
    xfree(cs->bindings,xbind_free,xbind*);
    xfree_(cs->args,free);
    xfree_(cs->consoleh,free);
    xfree(cs->dirs,xdir_free,xdir*);
    free(cs);
}
