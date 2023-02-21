/*
    csas - console file manager
    Copyright (C) 2020-2023 Dominik Stanisław Suchora <suchora.dominik7@gmail.com>

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
#include "useful.h"
#include "load.h"
#include "draw.h"
#include "csas.h"
#include "calc.h"
#include "sort.h"
#include "console.h"
#include "preview.h"
#include "expand.h"
#include "functions.h"

extern li Exit;
extern size_t BufferSize;
extern li DirLoadingMode;
extern li Visual;
extern li WrapScroll;
extern const char *TTEMPLATE;
extern li Error_C;
extern li Borders;
extern li SortMethod;
extern li ShowKeyBindings;
extern li MultipaneView;
extern flexarr *trap_exit;
extern flexarr *trap_preview;
extern flexarr *trap_newdir;
extern flexarr *trap_chdir;

uint
update_event(csas *cs)
{
    #define add_to_typed(x) cs->typed[n++] = (char)x; \
        cs->typed[n] = 0; \
        if (n >= NUM_MAX || x == ESC) \
            goto EXIT;
    #define finalize(x,y) x = y; \
        if (x == 0) \
            goto EXIT; \
        if (x == 1) \
            goto END; \
        i++;
    int event=0;
    size_t i=0,j,n=0;
    const size_t size = cs->bindings->size%BINDINGS_QUANTITY;
    xbind *b = BINDINGS;
    ushort passedl=0,tmp_passedl=0,
         passed[BINDINGS_QUANTITY*sizeof(uint)];

    while (true) { //handle first character
        event = getinput(cs);
        add_to_typed(event);
        if (isdigit(event)) {
            draw_bbar(LINES-1,cs);
            continue;
        }
        break;
    }
    for (j = 0; j < size; j++) {
        if (event == b[j].keys[i]) {
            passed[tmp_passedl] = j;
            tmp_passedl++;
        }
    }
    finalize(passedl,tmp_passedl);

    while (true) {
        if (ShowKeyBindings) {
            if (i > 1) {
                draw_borders();
                csas_draw(cs);
            }
            for (j = 0; j < passedl && j < (size_t)LINES-2; j++) {
                mvhline(LINES-2-j,0,' ',COLS);
                mvprintw(LINES-2-j,0," %c\t%s",b[passed[j]].keys[i],b[passed[j]].value);
            }
        }
        draw_bbar(LINES-1,cs);
        event = getinput(cs);
        add_to_typed(event);
        for (j=0,tmp_passedl=0; j < passedl; j++) {
            if (event == b[passed[j]].keys[i]) {
                passed[tmp_passedl] = passed[j];
                tmp_passedl++;
            }
        }
        finalize(passedl,tmp_passedl);
    }

    END: ;
    if (ShowKeyBindings && i > 0) {
        draw_borders();
        csas_draw(cs);
    }
    event = passed[0];
    return (uint)event;

    EXIT: ;
    if (ShowKeyBindings && i > 0) {
        draw_borders();
        csas_draw(cs);
    }
    cs->typed[0] = 0;
    return -1;
}

int
command_run(char *src, size_t size, csas *cs)
{
    size_t pos=0,t,s;

    while_is(isspace,src,pos,size);
    t = pos;
    while_isnt(isspace,src,pos,size);
    s = pos-t;
    while_is(isspace,src,pos,size);
    if (!s)
        return 0;

    xfunc *functions = FUNCTIONS;

    for (size_t i = 0; i < cs->functions->size; i++) {
        if (s == strlen(functions[i].name) && memcmp(src+t,functions[i].name,s) == 0) {
            if (functions[i].type == 'f') {
                int argc = splitargs(src+pos,size-pos,cs);
                char **argv = (char**)cs->args->v;
                int r = ((int (*)(int,char**,csas*))functions[i].func)(argc,argv,cs);
                cs->typed[0] = 0;
                return r;
            } else if (functions[i].type == 'a') {
                s = strlen(functions[i].func);
                if (s > LLINE_MAX)
                    return 0;
                char line[LLINE_MAX];
                memcpy(line,functions[i].func,s);
                line[s++] = ' ';
                size_t c = strlen(src+pos);
                if (s+c > LLINE_MAX)
                    return 0;
                memcpy(line+s,src+pos,c);
                s += c;
                line[s] = 0;
                return alias_run(line,s,cs);
            }
        }
    }

    cs->typed[0] = 0;
    errno = EINVAL;
    return -1;
}

static void
move_d(xdir *dir, size_t value, const size_t tab, const uchar flags)
{
    size_t sel=dir->sel[tab],t=sel,b1=0,e1=0,b2=0,e2=0;
    switch (flags&3) {
        case MOVE_SET:
            t = value;
            if (t >= dir->size)
                t = dir->size-1;
            if (t > sel) {
                b1 = sel;
                e1 = t;
            } else {
                b1 = t;
                e1 = sel;
            }
            break;
        case MOVE_UP:
            t += value;
            b1 = sel;
            e1 = t;
            if (t >= dir->size) {
                if (WrapScroll) {
                    b1 = sel;
                    e1 = dir->size-1;
                    t %= dir->size;
                    if (b1 != 0 || e1 != dir->size-1) {
                        b2 = 0;
                        e2 = t;
                    }
                } else {
                    b1 = t;
                    t = dir->size-1;
                    e1 = t;
                }
                if (value >= dir->size) {
                    b1 = 0;
                    e1 = dir->size-1;
                    b2 = 0;
                    e2 = 0;
                }
            }
            break;
        case MOVE_DOWN:
            if (t < value) {
                if (WrapScroll) {
                    b1 = 0;
                    e1 = sel;
                    t = (dir->size+sel-value)%dir->size;
                    if (t > e1) {
                        b2 = t;
                        e2 = dir->size-1;
                    }
                } else {
                    t = 0;
                    b1 = t;
                    e1 = sel;
                }
            } else {
                t -= value;
                b1 = t;
                e1 = sel;
            }
            if (value >= dir->size) {
                b1 = 0;
                e1 = dir->size-1;
                b2 = 0;
                e2 = 0;
            }
            break;
    }

    if (Visual) {
        for (; b1 < e1; b1++)
            dir->files[b1].sel[tab] |= 1<<0;
        for (; b2 < e2; b2++)
            dir->files[b2].sel[tab] |= 1<<0;
    }

    dir->sel[tab] = t;
}

int
cmd_move(int argc, char **argv, csas *cs)
{
    if (argc < 1) {
        printerr("move [-udsw] VALUE\n");
        return -1;
    }
    uchar mode = MOVE_UP;
    size_t value=(size_t)atol(cs->typed),tab=cs->ctab;
    if (value == 0)
        value = 1;

    xdir *dir = &CTAB(1);

    if (dir->size == 0)
        return 0;

    int opt;
    optind = 0;
    argc++;
    argv--;
    while ((opt = getopt(argc,argv,"udsw:")) != -1) {
        switch (opt) {
            case 'u': mode = MOVE_UP; break; //up
            case 'd': mode = MOVE_DOWN; break; //down
            case 's': mode = MOVE_SET; value--; break; //set
            case 'w': //tab
                tab = (size_t)atol(optarg);
                if (tab < TABS)
                    dir = &TAB(tab,1);
                break;
        }
    }
    if (optind < argc) {
        if (*argv[optind] == '$') {
            value = dir->size-1;
        } else {
            calc(argv[optind],(li*)&value,cs->vars);
        }
    }

    move_d(dir,value,cs->ctab,mode);
    if (MultipaneView && dir->size)
        preview_get(&dir->files[dir->sel[cs->ctab]],cs);
    return 0;
}

static int
tab_d(size_t t, int sel, csas *cs)
{
    ret_errno(t>=TABS,ERANGE,-1);
    if (cs->tabs[t].flags&T_EXISTS) {
        if (t != cs->ctab)
            if (chdir(TAB(t,1).path) != 0)
                return -1;
        goto END;
    }

    memcpy(cs->tabs[t].wins,cs->tabs[cs->ctab].wins,sizeof(cs->tabs[t].wins));
    cs->tabs[t].flags |= T_EXISTS;
    END: ;
    cs->ctab = t;
    if (sel >= 0 && sel < 8)
        cs->tabs[t].sel = sel;
    return 0;
}

int
cmd_tab(int argc, char **argv, csas *cs)
{
    if (argc < 1) {
        printerr("tab [-g] \n");
        return -1;
    }
    size_t tabc=cs->ctab;
    int sel = -1,opt;
    optind = 0;
    argc++;
    argv--;
    while ((opt = getopt(argc,argv,"g:")) != -1) {
        if (opt == 'g')
            sel = atoi(optarg);
    }
    if (optind < argc) {
        tabc = (size_t)atol(argv[optind]);
        if (tabc >= TABS) {
            printerr("tab: %s\n",strerror(ERANGE));
            return -1;
        }
    }

    int r = tab_d(tabc,sel,cs);
    if (r == -1)
        printerr("tab: %lu: %s\n",tabc,strerror(errno));
    return r;
}

int
cmd_console(int argc, char **argv, csas *cs)
{
    size_t s;
    char *add="",*first = ":";
    int opt;
    li offset = -1;

    optind = 0;
    argc++;
    argv--;
    while ((opt = getopt(argc,argv,"f:n:")) != -1) {
        switch (opt) {
            case 'f': first = optarg; break;
            case 'n': calc(optarg,&offset,cs->vars); break;
        }
    }
    if (optind < argc)
        add = argv[optind];

    flexarr *history = cs->consoleh;
    if (history->size == HISTORY_MAX) {
        char *t = *(char**)history->v;
        for (size_t i = 0; i < history->size-1; i++)
            ((char**)history->v)[i] = ((char**)history->v)[i+1];
        ((char**)history->v)[history->size-1] = t;
    } else
        *((char**)flexarr_inc(history)) = malloc(LLINE_MAX);
    ((char**)history->v)[history->size-1][0] = 0;

    s = console_getline((char**)history->v,history->size,first,add,offset,cs,expand_commands);
    char *line = ((char**)history->v)[history->size-1];
    command_run(line,s,cs);
    size_t empty = 1;
    for (size_t i = 0; line[i]; i++) {
        if (!isspace(line[i])) {
            empty = 0;
            break;
        }
    }
    if (empty) {
        free(line);
        history->size--;
    }
    return 0;
}

size_t
strtosize(const char *src, ul *num)
{
    size_t pos = 0;
    pos += get_dec(src+pos,(li*)num);
    switch (src[pos]) {
        case 'b': *num<<=9; pos++; break;
        case 'c': pos++; break;
        case 'w': *num<<=1; pos++; break;
        case 'k': *num<<=10; pos++; break;
        case 'M': *num<<=20; pos++; break;
        case 'G': *num<<=30; pos++; break;
        case 'T': *num<<=40; pos++; break;
        case 'P': *num<<=50; pos++; break;
    }

    return pos;
}

size_t
strtotime(const char *src, ul *num)
{
    size_t pos = 0;
    ul r = 0;
    while (src[pos] && src[pos] != '-' && !isspace(src[pos])) {
        *num = r;
        pos += get_dec(src+pos,(li*)&r);
        switch (src[pos]) {
            case 'y': case 'Y': r *= 365*24*60*60; break;
            case 'M': r *= 31*24*60*60; break;
            case 'w': case 'W': r *= 7*24*60*60; break;
            case 'd': case 'D': r *= 24*60*60; break;
            case 'h': case 'H': r *= 60*60; break;
            case 'm': r *= 60; break;
            case '+': break;
            default: goto END;
        }
        pos++;
    }
    
    END: ;
    *num += r;
    return pos;
}

int
cmd_scout(int argc, char **argv, csas *cs)
{
    size_t rpathl,sizex,sizey;
    static char *func_fmod_action_name[]={"delete","move","copy"};
    char *path_tmp,rpath[PATH_MAX],*pattern=NULL,*func_target=NULL;
    uint flags = 0,func_flags=0;
    int func_target_fd=-1,ret=-1,func_bulk_fd=-1
    #ifdef REGEX
        ,regflags = 0
    #endif
    ;
    flexarr *dir_list = flexarr_init(sizeof(size_t),16);
    flexarr *dirs = cs->dirs;
    xdir *dir = (xdir*)dirs->v;
    mode_t mode = 0;
    off_t func_fmod_count=0,func_fmod_size=0;
    time_t mtimex,mtimey;
    FILE *func_bulk_file = NULL;
    char groupx,groupy=-2,tabx=cs->ctab,taby=-2,*func_buffer=NULL,*func_bulk_shell="/bin/sh",
        *func_bulk_editor="vim",*func_bulk_begin=NULL,*func_bulk_middle=NULL,
        *func_bulk_end=NULL,func_groupx=cs->tabs[cs->ctab].sel,func_groupy=-2,
        func_tabx=cs->ctab,func_taby=-2,func_bulk_tfile[PATH_MAX],*func_bulk_filecopy=NULL;
    uchar lflags = DirLoadingMode,func=0,groups=0,func_groups=1<<func_groupx,func_ds_flags=D_S,
          func_fmod_flags = 0,func_bulk_comment_write=0;

    enum Flags {
        fl_force = 0x1,
        fl_mode = 0x2, //check for permissions
        fl_type = 0x4, //check for type
        fl_size = 0x8, //check for size
        fl_mtime = 0x10, //check for mtime
        fl_all = 0x20, //all
        fl_group = 0x40, //check for group
        fl_load = 0x80, //load directories
        fl_insensitive = 0x100, //case insensitive
        fl_name = 0x200, //check for name
        fl_regex = 0x400, //check for regex
        fl_invert = 0x800 //select non-matching
    };

    enum Functions {
        func_list = 1,
        func_filter = 2,
        func_select = 3,
        func_ds = 4,
        func_bulk = 5,
        func_fmod = 6
    };

    enum FuncFlags {
        fufl_clear = 0x1,
        fufl_selected = 0x2,
        fufl_enable = 0x4,
        fufl_disable = 0x8,
        fufl_toggle = 0x10,
        fufl_set = 0x20,
        fufl_target = 0x40,
        fufl_delete = 0x80,
        fufl_move = 0x100,
        fufl_copy = 0x200,
        fufl_no_comments = 0x400,
        fufl_full = 0x800
    };

    int opt;
    optind = 0;
    argv--;
    argc++;
    REPEAT: ;
    while ((opt = getopt(argc,argv,"+t:farLPvip:s:m:l:N:G:E:g:T:")) != -1) {
        switch (opt) {
            case 'f':
                flags |= fl_force;
                break;
            case 'p':
                li t;
                get_oct(optarg,&t);
                mode &= ~0777;
                mode |= (mode_t)t&0777;
                flags |= fl_mode;
                break;
            case 't':
                switch (*optarg) {
                    case 'f': mode |= S_IFREG; break;
                    case 'd': mode |= S_IFDIR; break;
                    case 'l': mode |= S_IFLNK; break;
                    case 's': mode |= S_IFSOCK; break;
                    case 'p': mode |= S_IFIFO; break;
                    case 'b': mode |= S_IFBLK; break;
                    case 'c': mode |= S_IFCHR; break;
                }
                flags |= fl_type;
                break;
            case 's':
                get_range(optarg,&sizex,&sizey,strtosize);
                flags |= fl_size;
                break;
            case 'm':
                get_range(optarg,(ul*)&mtimex,(ul*)&mtimey,strtotime);
                flags |= fl_mtime;
                break;
            case 'a':
                flags |= fl_all;
                break;
            case 'r': //load directories recursively
                lflags |= D_RECURSIVE;
                break;
            case 'l': {
                flags |= fl_load;
                li t;
                get_dec(optarg,&t);
                lflags &= ~(D_MODE_ALWAYS|D_MODE_CHANGE|D_MODE_ONCE);
                switch (t) {
                    case 0: lflags |= D_MODE_ONCE; break;
                    case 1: lflags |= D_MODE_CHANGE; break;
                    case 2: lflags |= D_MODE_ALWAYS; break;
                }}
                break;
            case 'L': //follow symlinks
                lflags |= D_FOLLOW;
                break;
            case 'P': //never follow symlinks
                lflags &= ~D_FOLLOW;
                break;
            case 'g':
                flags |= fl_group;
                if (*optarg == '-') {
                    groupx = cs->tabs[cs->ctab].sel;
                } else {
                    ul t1,t2;
                    get_range(optarg,&t1,&t2,(size_t(*)(const char*,ul*))get_dec);
                    groupx = t1&7;
                    if (t2 == 0)
                        groupy = -2;
                    else
                        groupy = t2&7;
                }
                groups = 0;
                for (uchar h = groupx; h != groupy+1; h++) {
                    groups |= 1<<h;
                    if (h > groupy)
                        break;
                }
                break;
            case 'T': { //specify tabs to be searched
                ul t1,t2;
                get_range(optarg,&t1,&t2,(size_t(*)(const char*,ul*))get_dec);
                tabx = t1;
                if (tabx >= TABS)
                    tabx = TABS-1;
                if (t2 == 0) {
                    taby = -2;
                } else {
                    taby = t2;
                    if (taby >= TABS)
                        taby = TABS-1;
                }}
                break;
            case 'v':
                flags |= fl_invert;
                break;
            case 'i':
                flags |= fl_insensitive;
                #ifdef REGEX
                regflags |= REG_ICASE;
                #endif
                break;
            case 'N':
            case 'G':
            case 'E':
                switch (opt) {
                    case 'N': flags |= fl_name; break;
                    #ifdef REGEX
                    case 'E': regflags |= REG_EXTENDED; // fall through
                    case 'G': flags |= fl_regex; break;
                    #endif
                }
                pattern = optarg;
                break;
        }
    }

    if (optind < argc && (optind == 1 || (argv[optind-1][0] != '-' || argv[optind-1][1] != '-' || argv[optind-1][2] != 0))) {
        path_tmp = argv[optind];
        if (realpath(path_tmp,rpath) == NULL)
            goto END_premature;
        rpathl = strlen(rpath);
        uchar found = 0;
        for (size_t j = 0; j < dirs->size; j++) {
            if (((lflags&D_RECURSIVE) ? (rpathl <= dir[j].plen && (dir[j].path[rpathl] == '/' || dir[j].path[rpathl] == 0)) : rpathl == dir[j].plen) && memcmp(dir[j].path,rpath,rpathl) == 0) {
                found = 1;
                *((size_t*)flexarr_inc(dir_list)) = j;
                if (flags&fl_load) {
                    li t = dirs->size;
                    if (getdir(rpath,dirs,lflags) == -1)
                        goto END_premature;
                    dir = (xdir*)dirs->v;
                    for (size_t n = (size_t)t; n < dirs->size; n++)
                        *((size_t*)flexarr_inc(dir_list)) = n;
                }
                if ((flags&fl_load) ? (lflags&D_RECURSIVE) : !(lflags&D_RECURSIVE))
                    break;
            }
        }
        if (!found) {
            if (flags&fl_force) {
                li t = getdir(rpath,dirs,lflags);
                if (t == -1)
                    goto END1;
                dir = (xdir*)dirs->v;
                for (size_t n = (size_t)t; n < dirs->size; n++)
                    *((size_t*)flexarr_inc(dir_list)) = n;
            } else {
                goto END_premature;
            }
        }
        optind++;
        if (optind < argc)
            goto REPEAT;
    }

    if (optind < argc) {
        const struct { char *name; uint func; uchar size; } func_names[] = {
            {"list",func_list,4},{"filter",func_filter,6},
            {"select",func_select,6},{"ds",func_ds,2},
            {"bulk",func_bulk,4},{"fmod",func_fmod,4}
        };
        size_t s = strlen(argv[optind]);
        for (size_t j = 0; j < LENGHT(func_names); j++) {
            if (s == (size_t)func_names[j].size && memcmp(func_names[j].name,argv[optind],s) == 0) {
                func = func_names[j].func;
                break;
            }
        }
        if (func == func_bulk)
            strcpy(func_bulk_tfile,TTEMPLATE);
        optind++;
        if (func == func_bulk) {
            while ((opt = getopt(argc,argv,"nNfS:E:b:m:e:")) != -1) {
                switch (opt) {
                    case 'n':
                        func_flags |= fufl_no_comments;
                        break;
                    case 'N':
                        func_flags &= ~fufl_no_comments;
                        break;
                    case 'f':
                        func_flags |= fufl_full;
                        break;
                    case 'm':
                        func_bulk_middle = optarg;
                        break;
                    case 'S':
                        func_bulk_shell = optarg;
                        break;
                    case 'E':
                        func_bulk_editor = optarg;
                        break;
                    case 'b':
                        func_bulk_begin = optarg;
                        break;
                    case 'e':
                        func_bulk_end = optarg;
                        break;
                }
            }
        } else {
            while ((opt = getopt(argc,argv,"CMrcmdFSst:o:EDTsg:T")) != -1) {
                switch (opt) {
                    case 'C':
                        func_flags &= ~(fufl_copy|fufl_move|fufl_disable);
                        func_flags |= fufl_clear|fufl_copy;
                        func_ds_flags &= ~D_S;
                        func_ds_flags |= D_C;
                        break;
                    case 'M':
                        func_flags &= ~(fufl_copy|fufl_move|fufl_disable);
                        func_flags |= fufl_move;
                        break;
                    case 'r':
                        func_ds_flags |= D_R;
                        func_fmod_flags |= M_REPLACE;
                        break;
                    case 'c':
                        func_fmod_flags |= M_CHNAME;
                        break;
                    case 'm':
                        func_fmod_flags |= M_MERGE;
                        break;
                    case 'd':
                        func_fmod_flags |= M_DCPY;
                        break;
                    case 'F':
                        func_ds_flags |= D_F;
                        break;
                    case 'S':
                        func_flags &= ~(fufl_enable|fufl_disable|fufl_toggle|fufl_set);
                        func_flags |= fufl_set;
                        func_ds_flags |= D_S;
                        break;
                    case 's':
                        func_flags |= fufl_selected;
                        break;
                    case 'o':
                        func_flags |= fufl_target;
                        func_target = optarg;
                        break;
                    case 'E':
                        func_flags &= ~(fufl_enable|fufl_disable|fufl_toggle|fufl_set);
                        func_flags |= fufl_enable;
                        break;
                    case 'D':
                        func_flags &= ~(fufl_copy|fufl_move|fufl_disable);
                        func_flags &= ~(fufl_enable|fufl_disable|fufl_toggle|fufl_set);
                        func_flags |= fufl_disable|fufl_delete;
                        break;
                    case 'T':
                        func_flags &= ~(fufl_enable|fufl_disable|fufl_toggle|fufl_set);
                        func_flags |= fufl_toggle;
                        break;
                    case 'g':
                        if (*optarg == '-') {
                            func_groupx = cs->tabs[cs->ctab].sel;
                        } else {
                            ul t1,t2;
                            get_range(optarg,&t1,&t2,(size_t(*)(const char*,ul*))get_dec);
                            func_groupx = t1&7;
                            if (t2 == 0)
                                func_groupy = -2;
                            else
                                func_groupy = t2&7;
                        }
                        func_groups = 0;
                        for (uchar h = func_groupx; h != func_groupy+1; h++) {
                            func_groups |= 1<<h;
                            if (h > func_groupy)
                                break;
                        }
                        break;
                    case 't':
                        ul t1,t2;
                        get_range(optarg,&t1,&t2,(size_t(*)(const char*,ul*))get_dec);
                        func_tabx = t1;
                        if (func_tabx >= TABS)
                            func_tabx = TABS-1;
                        if (t2 == 0) {
                            func_taby = -2;
                        } else {
                            func_taby = t2;
                            if (func_taby >= TABS)
                                func_taby = TABS-1;
                        }
                        break;
                }
            }
        }
    }

    if (func == func_fmod && (func_flags&(fufl_move|fufl_copy))) {
        func_target_fd = open(((func_flags&fufl_target) ? func_target : CTAB(1).path),O_DIRECTORY);
        if (func_target_fd == -1)
            goto END1;
        func_buffer = (char*)malloc(BufferSize);
    }

    if (func == func_bulk) {
        func_bulk_fd = mkstemp(func_bulk_tfile);
        func_bulk_file = fdopen(func_bulk_fd,"w+");
        if (!func_bulk_file)
            goto END1;
    }

    if (func_flags&fufl_selected) {
        flexarr_free(dir_list);
        xfile *f = &CTAB(1).files[CTAB(1).sel[cs->ctab]];
        switch (func) {
            case func_select:
                for (uchar n = func_tabx; n != func_taby+1; n++) {
                    switch (func_flags&(fufl_enable|fufl_disable|fufl_toggle|fufl_set)) {
                        case fufl_enable: f->sel[n] |= func_groups; break;
                        case fufl_disable: f->sel[n] &= ~func_groups; break;
                        case fufl_toggle: f->sel[n] ^= func_groups; break;
                        case fufl_set: f->sel[n] = func_groups; break;
                    }
                    if (n > func_taby)
                        break;
                }
                return 0;
                break;
            case func_ds: {
                xdir *d = &CTAB(1);
                xfile *file = &d->files[d->sel[cs->ctab]];
                if (func_ds_flags&D_F) {
                    struct stat statbuf;
                    if (stat(file->name,&statbuf) == -1)
                        return -1;
                    file->size = statbuf.st_size;
                    return 0;
                } else {
                    int fd = open(file->name,O_DIRECTORY);
                    if (fd == -1)
                        return -1;
                    int e,t;
                    file->size = 0;
                    t = get_dirsize(fd,&file->size,&file->size,func_ds_flags);
                    e = errno;
                    close(fd);
                    errno = e;
                    return t;
                }
                }
                break;
            case func_fmod: {
                xdir *d = &CTAB(1);
                xfile *f = &d->files[d->sel[cs->ctab]];
                int fd3,fd2=-1;
                if (d->size == 0) {
                    ret = 0;
                    goto END_fmod;
                }
                if ((fd2 = open(d->path,O_DIRECTORY)) == -1)
                    goto END_fmod;
                
                func_fmod_count = 1;
                if ((f->mode&S_IFMT) == S_IFDIR) {
                    if ((fd3 = openat(fd2,f->name,O_DIRECTORY)) == -1)
                        goto END_fmod;
                    if (get_dirsize(fd3,&func_fmod_count,&func_fmod_size,D_R|D_C|D_S) == -1)
                        close(fd3);
                }
                int act = 0,ev = -1;
                switch (func_flags&(fufl_delete|fufl_move|fufl_copy)) {
                    case fufl_delete: act = 0; break;
                    case fufl_move: act = 1; break;
                    case fufl_copy: act = 2; break;
                    default: goto END_fmod;
                }
                do {
                    if ((f->mode&S_IFMT) == S_IFDIR)
                        printmsg(0,"Do you want to %s %ld files(%s)? (Y/n)",func_fmod_action_name[act],func_fmod_count,size_shrink(func_fmod_size));
                    else
                        printmsg(0,"Do you want to %s \"%s\" (%s)? (Y/n)",func_fmod_action_name[act],f->name,size_shrink(f->size));
                    refresh();
                    if (ev == 'y' || ev == 'Y')
                        break;
                    if (ev != -1)
                        goto END_fmod;
                    ev = getinput(cs);
                } while (1);
                
                switch (func_flags&(fufl_delete|fufl_move|fufl_copy)) {
                    case fufl_delete: file_rm(fd2,f->name); break;
                    case fufl_move: file_mv(func_target_fd,fd2,f->name,func_buffer,func_fmod_flags); break;
                    case fufl_copy: file_cp(func_target_fd,fd2,f->name,func_buffer,func_fmod_flags); break;
                }
                ret = 0;
                END_fmod: ;
                if (func_flags&(fufl_copy|fufl_move)) {
                    close(func_target_fd);
                    free(func_buffer);
                }
                close(fd2);
                return ret;
                }
                break;
        }
        return -1;
    }

    if (!(flags&fl_all) && dir_list->size == 0) {
        char *path = CTAB(1).path;
        size_t pathl = CTAB(1).plen;
        for (size_t j = 0; j < dirs->size; j++) {
            if (((lflags&D_RECURSIVE) ? (pathl <= dir[j].plen && (dir[j].path[pathl] == '/' || dir[j].path[pathl] == 0)) : pathl == dir[j].plen) && memcmp(dir[j].path,path,pathl) == 0) {
                *((size_t*)flexarr_inc(dir_list)) = j;
                if (flags&fl_load) {
                    li t = dirs->size;
                    if (getdir(path,dirs,lflags) == -1) {
                        flexarr_free(dir_list);
                        return -1;
                    }
                    dir = (xdir*)dirs->v;
                    for (size_t n = (size_t)t; n < dirs->size; n++)
                        *((size_t*)flexarr_inc(dir_list)) = n;
                }
                if (!(lflags&D_RECURSIVE))
                    break;
            }
        }
    }
    
    xdir *d;
    xfile *f;
    time_t date = time(NULL);
    size_t size;
    register uchar pass;
    int dfd=0,fd;
    
    char *(*cmp)(const char*,const char*)=(flags&fl_insensitive ? strcasestr : strstr);
    #ifdef REGEX
    regex_t regex;
    if (flags&fl_regex && regcomp(&regex,pattern,regflags) != 0) {
        errno = EFAULT;
        return -1;
    }
    #endif

    for (size_t i = 0; ; i++) {
        if (flags&fl_all) {
            if (i >= dirs->size)
                break;
            d = &((xdir*)dirs->v)[i];
            if (flags&fl_load) {
                if (getdir(d->path,dirs,lflags) == -1)
                    continue;
                d = &((xdir*)dirs->v)[i];
            }
        } else {
            if (i >= dir_list->size)
                break;
            d = &((xdir*)dirs->v)[((size_t*)dir_list->v)[i]];
        }
        size = d->size;
        switch (func) {
            case func_list:
                flexarr_free(d->searchlist);
                d->searchlist = flexarr_init(sizeof(char*),SEARCHLIST_INCR);
                d->searchlist_pos = 0;
                break;
            case func_filter:
                if (func_flags&fufl_clear) {
                    d->size = d->asize;
                    goto SKIP;
                }
                d->size = 0;
                size = d->asize;
                break;
            case func_fmod: case func_ds:
                if ((dfd = open(d->path,O_DIRECTORY)) == -1)
                    continue;
                break;
            case func_bulk:
                func_bulk_comment_write = (func_flags&fufl_no_comments) ? 1 : 0;
                break;
        }
        for (size_t j = 0; j < size; j++) {
            f = &d->files[j];
            pass = 1;
            if (flags&fl_mode && (f->mode&0777) != (mode&0777))
                pass = 0;
            if (flags&fl_type && (f->mode&S_IFMT) != (mode&S_IFMT))
                pass = 0;
            if (flags&fl_size && (sizex > sizey ? (size_t)f->size != sizex : ((size_t)f->size < sizex || (size_t)f->size > sizey)))
                pass = 0;
            if (flags&fl_mtime && (f->mtime < date-mtimex || f->mtime > date-mtimey))
                pass = 0;
            if (flags&fl_group) {
                uchar found = 1;
                for (uchar n = tabx; n != taby+1; n++) {
                    if ((f->sel[n]&groups) != groups) {
                        found = 0;
                        break;
                    }
                    if (n > taby)
                        break;
                }
                if (!found)
                    pass = 0;
            }
            if (flags&fl_name && cmp(f->name,pattern) == NULL)
                pass = 0;
            #ifdef REGEX
            if (flags&fl_regex && regexec(&regex,f->name,0,NULL,0) != 0)
                pass = 0;
            #endif

            if ((flags&fl_invert) ? pass : !pass)
                continue;

            switch (func) {
                case func_list:
                    *((char**)flexarr_inc(d->searchlist)) = f->name;
                    break;
                case func_filter: {
                    if (j == d->size) {
                        d->size++;
                        continue;
                    }
                    char t[sizeof(xfile)];
                    memcpy(t,&d->files[d->size],sizeof(xfile));
                    memcpy(&d->files[d->size],f,sizeof(xfile));
                    memcpy(f,t,sizeof(xfile));
                    d->size++;
                    }
                    break;
                case func_select:
                    for (uchar n = func_tabx; n != func_taby+1; n++) {
                        switch (func_flags&(fufl_enable|fufl_disable|fufl_toggle|fufl_set)) {
                            case fufl_enable: f->sel[n] |= func_groups; break;
                            case fufl_disable: f->sel[n] &= ~func_groups; break;
                            case fufl_toggle: f->sel[n] ^= func_groups; break;
                            case fufl_set: f->sel[n] = func_groups; break;
                        }
                        if (n > func_taby)
                            break;
                    }
                    break;
                case func_ds:
                    if (func_ds_flags&D_F) {
                        struct stat statbuf;
                        if (fstatat(dfd,d->files[i].name,&statbuf,0) == -1)
                            continue;
                        d->files[j].size = statbuf.st_size;
                    } else {
                        if ((fd = openat(dfd,d->files[j].name,O_DIRECTORY)) == -1)
                            continue;
                        d->files[j].size = 0;
                        get_dirsize(fd,&d->files[j].size,&d->files[j].size,func_ds_flags);
                    }
                    break;
                case func_fmod:
                    func_fmod_count++;
                    if ((fd = openat(dfd,f->name,O_DIRECTORY)) == -1) {
                        struct stat statbuf;
                        fstatat(dfd,f->name,&statbuf,AT_SYMLINK_NOFOLLOW);
                        func_fmod_size += statbuf.st_size;
                    } else if (get_dirsize(fd,&func_fmod_count,&func_fmod_size,D_R|D_C|D_S) == -1)
                        close(fd);
                    break;
                case func_bulk:
                    if (!func_bulk_comment_write) {
                        fprintf(func_bulk_file,"//\t%s\n",d->path);
                        func_bulk_comment_write = 1;
                    }
                    char *t = (func_flags&fufl_full) ? mkpath(d->path,f->name) : f->name;
                    fprintf(func_bulk_file,"%s\n",t);
                   break;
            }
        }
        if (func == func_ds || func == func_fmod)
            close(dfd);
        SKIP: ;
    }

    if (func == func_fmod || func == func_bulk) {
        int act = 0,ev = -1;
        size_t pos=0,x;
        struct stat statbuf;
        if (func == func_fmod) {
            switch (func_flags&(fufl_delete|fufl_move|fufl_copy)) {
                case fufl_delete: act = 0; break;
                case fufl_move: act = 1; break;
                case fufl_copy: act = 2; break;
                default: goto END1;
            }
            do {
                printmsg(0,"Do you want to %s %ld files(%s)? (Y/n)",func_fmod_action_name[act],func_fmod_count,size_shrink(func_fmod_size));
                refresh();
                if (ev == 'y' || ev == 'Y')
                    break;
                if (ev != -1)
                    goto END1;
                ev = getinput(cs);
            } while (1);
        }

        char func_bulk_path[PATH_MAX];
        uchar writed = 0;

        if (func == func_bulk) {
            fflush(func_bulk_file);
            rewind(func_bulk_file);
            if (fstat(func_bulk_fd,&statbuf) != 0)
                goto END1;
            if (statbuf.st_size == 0) {
                ret = 0;
                goto END1;
            }
            if (spawnp(func_bulk_editor,func_bulk_tfile,NULL,F_NORMAL|F_WAIT) != 0)
                goto END1;
            if (fstat(func_bulk_fd,&statbuf) != 0)
                goto END1;
            if (statbuf.st_size == 0) {
                ret = 0;
                goto END1;
            }
            func_bulk_filecopy = (char*)malloc(statbuf.st_size+1);
            if (read(func_bulk_fd,func_bulk_filecopy,statbuf.st_size) == -1)
                goto END1;
            if (freopen(func_bulk_tfile,"w+",func_bulk_file) == NULL)
                goto END1;
            fprintf(func_bulk_file,"#!%s\n\n",func_bulk_shell);
        }


        for (size_t i = 0; ; i++) {
            if (flags&fl_all) {
                if (i >= dirs->size)
                    break;
                d = &((xdir*)dirs->v)[i];
            } else {
                if (i >= dir_list->size)
                    break;
                d = &((xdir*)dirs->v)[((size_t*)dir_list->v)[i]];
            }
        
            if (func == func_fmod && (dfd = open(d->path,O_DIRECTORY)) == -1)
                continue;
            size = d->size;
            for (size_t j = 0; j < size; j++) {
                f = &d->files[j];
                pass = 1;
                if (flags&fl_mode && (f->mode&0777) != (mode&0777))
                    pass = 0;
                if (flags&fl_type && (f->mode&S_IFMT) != (mode&S_IFMT))
                    pass = 0;
                if (flags&fl_size && (sizex > sizey ? (size_t)f->size != sizex : ((size_t)f->size < sizex || (size_t)f->size > sizey)))
                    pass = 0;
                if (flags&fl_mtime && (f->mtime < date-mtimex || f->mtime > date-mtimey))
                    pass = 0;
                if (flags&fl_group) {
                    uchar found = 1;
                    for (uchar n = tabx; n != taby+1; n++) {
                        if ((f->sel[n]&groups) != groups) {
                            found = 0;
                            break;
                        }
                        if (n > taby)
                            break;
                    }
                    if (!found)
                        pass = 0;
                }
                if (flags&fl_name && cmp(f->name,pattern) == NULL)
                    pass = 0;
                #ifdef REGEX
                if (flags&fl_regex && regexec(&regex,f->name,0,NULL,0) != 0)
                    pass = 0;
                #endif
        
                if ((flags&fl_invert) ? pass : !pass)
                    continue;
        
                if (func == func_fmod) {
                    switch (func_flags&(fufl_delete|fufl_move|fufl_copy)) {
                        case fufl_delete: file_rm(dfd,f->name); break;
                        case fufl_move: file_mv(func_target_fd,dfd,f->name,func_buffer,func_fmod_flags); break;
                        case fufl_copy: file_cp(func_target_fd,dfd,f->name,func_buffer,func_fmod_flags); break;
                    }
                }
                if (func == func_bulk) {
                    if (!func_bulk_filecopy[pos])
                        goto END_last_loop;
                    while (func_bulk_filecopy[pos] == '\n') {
                        pos++;
                        j--;
                    }
                    if (!(func_flags&fufl_no_comments) && func_bulk_filecopy[pos] == '/' && func_bulk_filecopy[pos+1] == '/') {
                        while (func_bulk_filecopy[pos] && func_bulk_filecopy[pos] != '\n')
                            pos++;
                        j--;
                    } else {
                        x = 0;
                        while (x < PATH_MAX && func_bulk_filecopy[pos] && func_bulk_filecopy[pos] != '\n')
                            func_bulk_path[x++] = func_bulk_filecopy[pos++];
                        func_bulk_path[x] = '\0';

                        char *t = (func_flags&fufl_full) ? mkpath(d->path,f->name) : f->name;
                        if (x > 0 && strcmp(t,func_bulk_path) != 0) {
                            if (func_bulk_begin)
                                fprintf(func_bulk_file,"%s ",func_bulk_begin);
                            if (!(func_flags&fufl_full))
                                t = mkpath(d->path,t);
                            strtoshellpath(t);
                            fprintf(func_bulk_file,"%s ",t);
                            if (func_bulk_middle)
                                fprintf(func_bulk_file,"%s ",t);
                            t = (func_flags&fufl_full) ? func_bulk_path :
                                mkpath(d->path,func_bulk_path);
                            strtoshellpath(t);
                            fprintf(func_bulk_file,"%s\n",t);
                            if (func_bulk_end)
                                fprintf(func_bulk_file,"%s %s\n",t,func_bulk_end);
                            writed = 1;
                        }
                    }
                    pos++;
                }
            }
            if (func == func_fmod)
                close(dfd);
        }
        END_last_loop: ;

        if (func == func_bulk) {
            fflush(func_bulk_file);
        
            if (writed == 0) {
                ret = 0;
                goto END1;
            }

        if (spawnp(func_bulk_editor,func_bulk_tfile,NULL,F_NORMAL|F_WAIT) != 0 ||
            spawnp(func_bulk_shell,func_bulk_tfile,NULL,F_NORMAL|F_WAIT|F_CONFIRM) != 0)
            goto END1;
        }
    }

    ret = 0;
    END1: ;
    #ifdef REGEX
    if (flags&fl_regex)
        regfree(&regex);
    #endif
    if (func == func_fmod && (func_flags&(fufl_move|fufl_copy))) {
        close(func_target_fd);
        free(func_buffer);
    }
    if (func_bulk_filecopy)
        free(func_bulk_filecopy);
    if (func == func_bulk) {
        unlink(func_bulk_tfile);
        fclose(func_bulk_file);
    }

    END_premature: ;
    flexarr_free(dir_list);
    return ret;
}

int
cmd_source(int argc, char **argv, csas *cs)
{
    if (argc < 1) {
        printerr("source FILE\n");
        return -1;
    }
    int r = config_load(argv[0],cs);
    if (r == -1)
        printerr("source: %s: %s\n",argv[0],strerror(errno));
    return r;
}

int
cmd_cd(int argc, char **argv, csas *cs)
{
    if (argc < 1) {
        printerr("cd DIR\n");
        return -1;
    }
    int r = csas_cd(argv[0],cs);
    if (r == -1)
        printerr("cd: %s: %s\n",argv[0],strerror(errno));
    return r;
}

int
cmd_file_run(int argc, char **argv, csas *cs)
{
    if (argc < 1) {
        printerr("file_run FILE\n");
        return -1;
    }
    struct stat statbuf;
    if (stat(argv[0],&statbuf) != 0)
        return -1;
    int r = ((statbuf.st_mode&S_IFMT) == S_IFDIR) ? csas_cd(argv[0],cs)
        : file_run(argv[0],cs);
    if (r == -1)
        printerr("file_run: %s: %s\n",argv[0],strerror(errno));
    return r;
}

int
cmd_exec(int argc, char **argv, csas *cs)
{
    if (argc < 1) {
        printerr("exec -[sncw] program arg1 arg2\n");
        return -1;
    }
    int opt;
    uchar flags = F_MULTI;

    optind = 0;
    argc++;
    argv--;
    while ((opt = getopt(argc,argv,"+sncw")) != -1) {
        switch (opt) {
            case 's': flags |= F_SILENT; break;
            case 'n': flags |= F_NORMAL; break;
            case 'c': flags |= F_CONFIRM; break;
            case 'w': flags |= F_WAIT; break;
        }
    }
    if (optind >= argc)
        return 0;
    for (int i = optind; i < argc; i++) {
        if (argv[i][0] == '$' && argv[i][1] == '$' && argv[i][2] == 0) {
            char *arg = argv[i];
            size_t s,pos=0,end=i+1;
            i++;
            for (; i < argc; i++) {
                s = strlen(argv[i]);
                memcpy(arg+pos,argv[i],s);
                arg[pos+++s] = ' ';
                pos += s;
            }
            arg[pos] = 0;
            argc = end;
            break;
        }
    }
    char *t = argv[argc];
    argv[argc] = NULL;
    argv += optind;
    opt = spawn(argv,flags);
    argv[argc-optind] = t;
    if (opt == -1) {
        printerr("exec: ");
        for (int i = 0; i < argc; i++)
            printerr("%s",argv[i]);
        printerr(": %s\n",strerror(errno));
    }
    return opt;
}

int
cmd_alias(int argc, char **argv, csas *cs)
{
    if (argc < 2) {
        printerr("alias NAME VALUE\n");
        return -1;
    }
    int r = xfunc_add(argv[0],'a',strdup(argv[1]),NULL,cs->functions);
    if (r == -1)
        printerr("alias: %s\n",strerror(errno));
    return r;
}

int
cmd_map(int argc, char **argv, csas *cs)
{
    if (argc < 2) {
        printerr("map KEYS VALUE\n");
        return -1;
    }
    int r = xbind_add(argv[0],argv[1],cs->bindings);
    if (r == -1)
        printerr("map: %s\n",strerror(errno));
    return r;
}

int
cmd_unmap(int argc, char **argv, csas *cs)
{
    if (argc < 1) {
        printerr("unmap [BINDING...]\n");
        return -1;
    }

    xbind *binds = (xbind*)cs->bindings->v;
    wchar_t k[BINDING_KEY_MAX];
    for (int i = 0; i < argc; i++) {
        for (size_t j = 0; j < cs->bindings->size; j++) {
            change_keys(k,argv[i]);
            if (wcscmp(k,binds[j].keys) == 0) {
                free(binds[j].keys);
                free(binds[j].value);
                if (j != cs->bindings->size-1)
                    memcpy(binds+j,binds+(cs->bindings->size-1),sizeof(xbind));
                flexarr_dec(cs->bindings);
            }
        }
    }

    return 0;
}

int
cmd_quit(int argc, char **argv, csas *cs)
{
    size_t i;
    int n=0,opt,r=0;
    optind = 0;
    argc++;
    argv--;
    while ((opt = getopt(argc,argv,"f")) != -1) {
        if (opt == 'f')
            goto END;
    }

    for (i = 0; i < TABS; i++)
         n += cs->tabs[i].flags&T_EXISTS;
 
     cs->tabs[cs->ctab].flags &= ~T_EXISTS;
 
     if (n > 1) {
         for (i = cs->ctab+1; i < TABS; i++) {
             if (cs->tabs[i].flags&T_EXISTS) {
                 r = tab_d(i,-1,cs);
                 break;
             }
         }
         for (i = 0; i < cs->ctab; i++) {
             if (cs->tabs[i].flags&T_EXISTS) {
                 r = tab_d(i,-1,cs);
                 break;
             }
         }
     }

    if (r == -1)
        printerr("quit: %s\n",strerror(errno));

    END: ;
    Exit = 1;
    return r;
}

int
cmd_set(int argc, char **argv, csas *cs)
{
    if (argc < 2) {
        printerr("set VAR VALUE\n");
        return -1;
    }
    uchar type = XVAR_INT;
    char *name=NULL,*val=NULL;
    int n=0;
    if (argv[n][0] == '-' && argv[n][1] == 's')
        type = XVAR_STRING;
    name = argv[n++];
    if (n < argc)
        val = argv[n];

    int r = xvar_add(NULL,name,type,val,cs->vars);
    if (r == -1)
        printerr("set: %s\n",strerror(errno));
    return r;
}

int
cmd_unset(int argc, char **argv, csas *cs)
{
    if (argc < 1) {
        printerr("unset [VAR...]\n");
        return -1;
    }

    xvar *vars = (xvar*)cs->vars->v;
    for (int i = 0; i < argc; i++) {
        for (volatile size_t j = 0; j < cs->vars->size; j++) {
            if (strcmp(argv[i],vars[j].name) == 0) {
                if (!(vars[j].type&XVAR_POINTER))
                    free(vars[j].v);
                if (j != cs->vars->size-1)
                    memcpy(vars+j,vars+(cs->vars->size-1),sizeof(xvar));
                flexarr_dec(cs->vars);
            }
        }
    }

    return 0;
}

int
cmd_trap(int argc, char **argv, csas *cs)
{
    if (argc < 2) {
        printerr("trap COMMAND EVENT\n");
        return -1;
    }
    flexarr *dest = NULL;

    static struct {
        char *b;
        uchar s;
        flexarr **v;
    } traps[] = {
        {"EXIT",4,&trap_exit},
        {"PREVIEW",7,&trap_preview},
        {"CHDIR",5,&trap_chdir},
        {"NEWDIR",6,&trap_newdir}
    };

    char found=0;
    size_t s = strlen(argv[1]);
    for (size_t i = 0; i < LENGHT(traps); i++) {
        if(s == traps[i].s && memcmp(traps[i].b,argv[1],s) == 0) {
            dest = *traps[i].v;
            found = 1;
            break;
        }
    }
    if (!found) {
        printerr("trap: EVENT not found\n");
        return -1;
    }

    if (dest->size >= TRAP_MAX) {
        printerr("trap: couldn't add as limit of traps has been already reached\n");
        return -1;
    }
    *(char **)flexarr_inc(dest) = strdup(argv[0]);
    return 0;
}

int
cmd_rename(int argc, char **argv, csas *cs)
{
    if (argc < 1) {
        printerr("rename FILE\n");
        return -1;
    }
    char name[NAME_MAX],*n=name;
    name[0] = 0;

    console_getline(&n,1,"rename ",argv[0],-1,cs,NULL);
    int r = rename(argv[0],n);
    if (r == -1)
        printerr("rename: %s: %s\n",argv[0],strerror(errno));
    return r;
}

int
cmd_open_with(int argc, char **argv, csas *cs)
{
    if (argc < 1) {
        printerr("open_with FILE\n");
        return -1;
    }
    char path[PATH_MAX],*n=path;
    if (access(argv[0],W_OK) != 0) {
        printerr("open_with: %s: %s\n",argv[0],strerror(errno));
        return -1;
    }

    path[0] = 0;
    size_t s = console_getline(&n,1,"open_with ",NULL,-1,cs,expand_shell_commands);
    n[s] = 0;
    int r = spawnp(n,argv[0],NULL,F_NORMAL|F_WAIT);
    if (r == -1)
        printerr("open_with: %s: %s\n",argv[0],strerror(errno));
    return r;
}

int
cmd_sort(int argc, char **argv, csas *cs)
{
    xdir *dir = &CTAB(1);
    if (dir->size > 0) {
        xfile_sort(dir->files,dir->size,SortMethod);
        dir->sort = SortMethod;
    }
    return 0;
}

int
cmd_lmove(int argc, char **argv, csas *cs)
{
    if (argc < 1) {
        printerr("lmove VALUE\n");
        return -1;
    }
    xdir *dir = &CTAB(1);
    if (dir->size == 0)
        return 0;
    xfile *files = dir->files;
    size_t mul=(size_t)atol(cs->typed),ctab=cs->ctab;
    if (mul == 0)
        mul = 1;
    uchar flags = 0;
    char **searchl = (char**)dir->searchlist->v;
    int opt;

    optind = 0;
    argc++;
    argv--;
    while ((opt = getopt(argc,argv,"n:p:")) != -1) {
        switch (opt) {
            case 'n':
                flags = 0x2;
                calc(optarg,(li*)&mul,cs->vars);
                break;
            case 'p':
                flags = 0x1;
                calc(optarg,(li*)&mul,cs->vars);
                break;
        }
    }
    if (flags == 0)
        return 0;

    if (!dir->searchlist->size)
        return 0;
    if (files[dir->sel[ctab]].name == searchl[dir->searchlist_pos]) {
        if (flags&0x2) {
            if (mul+dir->searchlist_pos == dir->searchlist->size)
                dir->searchlist_pos = 0;
            else if (mul+dir->searchlist_pos > dir->searchlist->size)
                dir->searchlist_pos = dir->searchlist->size-1;
            else
                dir->searchlist_pos += mul;
        }
        if (flags&0x1) {
            if (dir->searchlist_pos == 0)
                dir->searchlist_pos = dir->searchlist->size-1;
            else if (mul > dir->searchlist_pos)
                dir->searchlist_pos = 0;
            else
                dir->searchlist_pos -= mul;
        }
    }
    size_t i;
    uchar found = 0;
    char *name = searchl[dir->searchlist_pos];
    for (i = 0; i < dir->size; i++) {
        if (name == files[i].name) {
            found = 1;
            break;
        }
    }
    if (found) {
        dir->sel[ctab] = i;
        if (MultipaneView)
            preview_get(&dir->files[i],cs);
    }
    return 0;
}
