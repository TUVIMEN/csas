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

uint
update_event(csas *cs)
{
    int event;
    size_t i,j,size = cs->bindings->size,n=0;
    xbind *b = BINDINGS;
    uint passedl=size,tmp_passedl,
         *passed = malloc(size*sizeof(uint));

    for (i = 0; i < size; i++)
        passed[i] = i;
    i = 0;
    while (true) {
        if (ShowKeyBindings && i > 0) {
            if (i > 1) {
                draw_borders();
                csas_draw(cs);
            }
            for (j = 0; j < passedl && j < (size_t)LINES-2; j++) {
                mvhline(LINES-2-j,0,' ',COLS);
                mvprintw(LINES-2-j,0," %c\t%s",b[passed[j]].keys[i],b[passed[j]].value);
            }
        }
        while ((event = getinput(cs)) == -1);
        cs->typed[n++] = (char)event;
        cs->typed[n] = 0;
        if (n >= NUM_MAX || event == ESC)
            goto EXIT;
        draw_bbar(LINES-1,cs);
        if (i == 0 && isdigit(event))
            continue;
        tmp_passedl = 0;
        for (j = 0; j < passedl; j++) {
            if (event == b[passed[j]].keys[i]) {
                passed[tmp_passedl] = passed[j];
                tmp_passedl++;
            }
        }
        passedl = tmp_passedl;
        if (passedl == 0)
            goto EXIT;
        if (passedl == 1)
            goto END;
        i++;
    }

    END: ;
    if (ShowKeyBindings && i > 0) {
        draw_borders();
        csas_draw(cs);
    }
    event = passed[0];
    free(passed);
    return (uint)event;

    EXIT: ;
    if (ShowKeyBindings && i > 0) {
        draw_borders();
        csas_draw(cs);
    }
    cs->typed[0] = 0;
    free(passed);
    return -1;
}

int
alias_run(char *src, csas *cs)
{
    size_t size=strlen(src);
    if (size == 0)
        return -1;

    char line[LLINE_MAX];
    for (size_t i = 0; i < size; i++) {
        get_line(line,src,&i,size);
        command_run(line,cs);
        i++;
    }
    return 0;
}

int
command_run(char *src, csas *cs)
{
    size_t size=strlen(src),pos=0,t,s;
    if (size == 0)
        return 0;

    while_is(isspace,src,pos,size);
    t = pos;
    while_isnt(isspace,src,pos,size);
    s = pos-t;
    while_is(isspace,src,pos,size);
    
    size = cs->functions->size;
    xfunc *functions = FUNCTIONS;


    for (size_t i = 0; i < size; i++) {
        if (s == strlen(functions[i].name) && memcmp(src+t,functions[i].name,s) == 0) {
            if (functions[i].type == 'f') {
                int r = ((int (*)(char*,csas*))functions[i].func)(src+pos,cs);
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
                return alias_run(line,cs);
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
cmd_move(char *src, csas *cs)
{
    uchar flags = MOVE_UP;
    size_t value=(size_t)atol(cs->typed),pos=0,tab=cs->ctab;
    size_t size = strlen(src);
    if (value == 0)
        value = 1;

    xdir *dir = &CTAB(1);

    if (dir->size == 0)
        return 0;

    while (pos < size) {
        if (src[pos] == '-') {
            do {
                pos++;
                switch (src[pos]) {
                    case 'u': flags = MOVE_UP; break; //up
                    case 'd': flags = MOVE_DOWN; break; //down
                    case 's': flags = MOVE_SET; value--; break; //set
                    case 'w': //tab
                        pos++;
                        while (isspace(src[pos]))
                            pos++;
                        tab = (size_t)atol(src+pos);
                        while (isdigit(src[pos]))
                            pos++;
                        if (tab < TABS)
                            dir = &TAB(tab,1);
                }
            } while (src[pos] && !isspace(src[pos]));
        } else if (src[pos] == '$') {
            value = dir->size-1;
        } else if (!isspace(src[pos])) {
            pos += calc(src+pos,(li*)&value,cs->vars);
        }
        if (src[pos])
            pos++;
        while (isspace(src[pos]))
            pos++;
    }

    move_d(dir,value,cs->ctab,flags);
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
cmd_tab(char *src, csas *cs)
{
    size_t pos=0,tabc=cs->ctab;
    int sel = -1;
    while (src[pos]) {
        while (isspace(src[pos]))
            pos++;
        if (src[pos] == '-' && src[pos+1] == 'g') {
            pos += 2;
            while (isspace(src[pos]))
                pos++;
            sel = atoi(src+pos);
            while (isdigit(src[pos]))
                pos++;
            continue;
        }
        if (isdigit(src[pos])) {
            tabc = (size_t)atol(src+pos);
            while (isdigit(src[pos]))
                pos++;
            ret_errno(tabc>=TABS,ERANGE,-1);
        }
    }

    return tab_d(tabc,sel,cs);
}

int
cmd_console(char *src, csas *cs)
{
    size_t pos = 0,s=strlen(src);
    char add[PATH_MAX],first[NAME_MAX] = ":",*r;
    li offset = -1;
    add[0] = '\0';

    while (src[pos]) {
        while (isspace(src[pos]))
            pos++;
        if (src[pos] == '-') {
            pos++;
            if (src[pos] == 'f') {
                pos++;
                while (isspace(src[pos]))
                    pos++;
                r = get_path(first,src+pos,' ',s-pos,NAME_MAX,cs);
                if (r == NULL)
                    continue;
                pos = r-src+1;
            } else if (src[pos] == 'n') {
                pos++;
                while (isspace(src[pos]))
                    pos++;
                pos += calc(src+pos,&offset,cs->vars);
            }
        } else {
            r = get_path(add,src+pos,' ',s-pos,PATH_MAX,cs);
            if (r == NULL)
                continue;
            pos = r-src+1;
        }
    }

    flexarr *history = cs->consoleh;
    if (history->size == HISTORY_MAX) {
        char *t = *(char**)history->v;
        for (size_t i = 0; i < history->size-1; i++)
            ((char**)history->v)[i] = ((char**)history->v)[i+1];
        ((char**)history->v)[history->size-1] = t;
    } else
        *((char**)flexarr_inc(history)) = malloc(LLINE_MAX);
    ((char**)history->v)[history->size-1][0] = 0;

    console_getline((char**)history->v,history->size,first,add,offset,cs,expand_commands);
    char *line = ((char**)history->v)[history->size-1];
    if (command_run(line,cs) != 0)
        printmsg(Error_C,"%s: %s",line,strerror(errno));
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
cmd_scout(char *src, csas *cs)
{
    size_t s=strlen(src),rpathl,sizex,sizey;
    static char *func_fmod_action_name[]={"delete","move","copy"};
    char path_tmp[PATH_MAX],rpath[PATH_MAX],pattern[NAME_MAX],func_target[PATH_MAX];
    uint flags = 0,func_flags=0;
    int func_target_fd=-1,ret = -1,func_bulk_fd=-1
    #ifdef REGEX
        ,regflags = 0
    #endif
    ;
    flexarr *dir_list = flexarr_init(sizeof(size_t),8);
    flexarr *dirs = cs->dirs;
    xdir *dir = (xdir*)dirs->v;
    mode_t mode = 0;
    off_t func_fmod_count=0,func_fmod_size=0;
    time_t mtimex,mtimey;
    FILE *func_bulk_file = NULL;
    char groupx,groupy=-2,tabx=cs->ctab,taby=-2,*func_buffer=NULL,func_bulk_shell[PATH_MAX]="",
        func_bulk_editor[PATH_MAX]="",func_bulk_begin[PATH_MAX]="",func_bulk_middle[PATH_MAX]="",
        func_bulk_end[PATH_MAX]="",func_groupx=cs->tabs[cs->ctab].sel,func_groupy=-2,
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

    for (size_t i = 0,j; i < s; i++) {
        if (src[i] == '-') {
            i++;
            if (src[i] == 'f') {
                flags |= fl_force;
                i++;
            } else if (memcmp(src+i,"perm",4) == 0) {
                i += 4;
                while_is(isspace,src,i,s);
                li t;
                i += get_oct(src+i,&t);
                mode &= ~0777;
                mode |= (mode_t)t&0777;
                flags |= fl_mode;
            } else if (memcmp(src+i,"type",4) == 0) {
                i += 4;
                while_is(isspace,src,i,s);
                switch (src[i]) {
                    case 'f': mode |= S_IFREG; break;
                    case 'd': mode |= S_IFDIR; break;
                    case 'l': mode |= S_IFLNK; break;
                    case 's': mode |= S_IFSOCK; break;
                    case 'p': mode |= S_IFIFO; break;
                    case 'b': mode |= S_IFBLK; break;
                    case 'c': mode |= S_IFCHR; break;
                }
                i++;
                flags |= fl_type;
            } else if (memcmp(src+i,"size",4) == 0) {
                i += 4;
                while_is(isspace,src,i,s);
                i += get_range(src+i,&sizex,&sizey,strtosize);
                flags |= fl_size;
            } else if (memcmp(src+i,"mtime",5) == 0) {
                i += 5;
                while_is(isspace,src,i,s);
                i += get_range(src+i,(ul*)&mtimex,(ul*)&mtimey,strtotime);
                flags |= fl_mtime;
            } else if (src[i] == 'a') {
                flags |= fl_all;
                i++;
            } else if (src[i] == 'r') { //load directories recursively
                i++;
                lflags |= D_RECURSIVE;
            } else if (src[i] == 'l') {
                flags |= fl_load;
                i++;
                while_is(isspace,src,i,s);
                li t;
                i += get_dec(src+i,&t);
                lflags &= ~(D_MODE_ALWAYS|D_MODE_CHANGE|D_MODE_ONCE);
                switch (t) {
                    case 0: lflags |= D_MODE_ONCE; break;
                    case 1: lflags |= D_MODE_CHANGE; break;
                    case 2: lflags |= D_MODE_ALWAYS; break;
                }
            } else if (src[i] == 'L') { //follow symlinks
                lflags |= D_FOLLOW;
                i++;
            } else if (src[i] == 'P') { //never follow symlinks
                lflags &= ~D_FOLLOW;
                i++;
            } else if (src[i] == 'g') {
                flags |= fl_group;
                i++;
                while_is(isspace,src,i,s);
                if (src[i] == '-') {
                    i++;
                    groupx = cs->tabs[cs->ctab].sel;
                } else {
                    ul t1,t2;
                    i += get_range(src+i,&t1,&t2,(size_t(*)(const char*,ul*))get_dec);
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
            } else if (src[i] == 't') { //specify tabs to be searched
                i++;
                while_is(isspace,src,i,s);
                ul t1,t2;
                i += get_range(src+i,&t1,&t2,(size_t(*)(const char*,ul*))get_dec);
                tabx = t1;
                if (tabx >= TABS)
                    tabx = TABS-1;
                if (t2 == 0)
                    taby = -2;
                else {
                    taby = t2;
                    if (taby >= TABS)
                        taby = TABS-1;
                }
            } else if (src[i] == 'v') {
                i++;
                flags |= fl_invert;
            } else if (src[i] == 'i') {
                i++;
                flags |= fl_insensitive;
                #ifdef REGEX
                regflags |= REG_ICASE;
                #endif
            } else if (src[i] == 'N' || src[i] == 'G' || src[i] == 'E') {
                switch (src[i]) {
                    case 'N': flags |= fl_name; break;
                    #ifdef REGEX
                    case 'E': regflags |= REG_EXTENDED; // fall through
                    case 'G': flags |= fl_regex; break;
                    #endif
                }
                i++;
                while_is(isspace,src,i,s);
                char *t = get_path(pattern,src+i,' ',s-i,PATH_MAX,cs);
                i = t-src+1;
            } else if (src[i] == '-') {
                i++;
                while_is(isspace,src,i,s);
                if (memcmp(src+i,"list",4) == 0) {
                    i += 4;
                    func = func_list;
                } else if (memcmp(src+i,"filter",6) == 0) {
                    i += 6;
                    func = func_filter;
                } else if (memcmp(src+i,"select",6) == 0) {
                    i += 6;
                    func = func_select;
                } else if (memcmp(src+i,"ds",2) == 0) {
                    i += 2;
                    func = func_ds;
                } else if (memcmp(src+i,"bulk",4) == 0) {
                    i += 4;
                    func = func_bulk;
                    strcpy(func_bulk_tfile,TTEMPLATE);
                } else if (memcmp(src+i,"fmod",4) == 0) {
                    i += 4;
                    func = func_fmod;
                }
                if (src[i] && !isspace(src[i]))
                    goto END1;
                for (; i < s; i++) {
                    while (i < s && isspace(src[i]))
                        i++;
                    if (src[i] == '-') {
                        i++;
                        if (src[i] == 'C') {
                            i++;
                            func_flags &= ~(fufl_copy|fufl_move|fufl_disable);
                            func_flags |= fufl_clear|fufl_copy;
                            func_ds_flags &= ~D_S;
                            func_ds_flags |= D_C;
                        } else if (src[i] == 'M') {
                            i++;
                            func_flags &= ~(fufl_copy|fufl_move|fufl_disable);
                            func_flags |= fufl_move;
                        } else if (src[i] == 'r') {
                            i++;
                            func_ds_flags |= D_R;
                            func_fmod_flags |= M_REPLACE;
                        } else if (src[i] == 'c') {
                            i++;
                            func_fmod_flags |= M_CHNAME;
                        } else if (src[i] == 'n') {
                            i++;
                            func_flags |= fufl_no_comments;
                        } else if (src[i] == 'N') {
                            i++;
                            func_flags &= ~fufl_no_comments;
                        } else if (src[i] == 'f') {
                            i++;
                            func_flags |= fufl_full;
                        } else if (src[i] == 'm') {
                            i++;
                            if (func == func_bulk) {
                                while_is(isspace,src,i,s);
                                char *t = get_path(func_bulk_middle,src+i,' ',s-i,PATH_MAX,cs);
                                i = t-src+1;
                            }
                            func_fmod_flags |= M_MERGE;
                        } else if (src[i] == 'b') {
                            i++;
                            while_is(isspace,src,i,s);
                            char *t = get_path(func_bulk_begin,src+i,' ',s-i,PATH_MAX,cs);
                            i = t-src+1;
                        } else if (src[i] == 'e') {
                            i++;
                            while_is(isspace,src,i,s);
                            char *t = get_path(func_bulk_end,src+i,' ',s-i,PATH_MAX,cs);
                            i = t-src+1;
                        } else if (src[i] == 'd') {
                            i++;
                            func_fmod_flags |= M_DCPY;
                        } else if (src[i] == 'F') {
                            i++;
                            func_ds_flags |= D_F;
                        } else if (src[i] == 'S') {
                            i++;
                            if (func == func_bulk) {
                                while_is(isspace,src,i,s);
                                char *t = get_path(func_bulk_shell,src+i,' ',s-i,PATH_MAX,cs);
                                i = t-src+1;
                            }
                            func_ds_flags |= D_S;
                        } else if (src[i] == 's') {
                            i++;
                            func_flags |= fufl_selected;
                        } else if (src[i] == 'o') {
                            func_flags |= fufl_target;
                            i++;
                            while_is(isspace,src,i,s);
                            char *t = get_path(func_target,src+i,' ',s-i,PATH_MAX,cs);
                            i = t-src+1;
                        } else if (src[i] == 'E') {
                            i++;
                            if (func == func_bulk) {
                                while_is(isspace,src,i,s);
                                char *t = get_path(func_bulk_editor,src+i,' ',s-i,PATH_MAX,cs);
                                i = t-src+1;
                            }
                            func_flags &= ~(fufl_enable|fufl_disable|fufl_toggle|fufl_set);
                            func_flags |= fufl_enable;
                        } else if (src[i] == 'D') {
                            i++;
                            func_flags &= ~(fufl_copy|fufl_move|fufl_disable);
                            func_flags &= ~(fufl_enable|fufl_disable|fufl_toggle|fufl_set);
                            func_flags |= fufl_disable|fufl_delete;
                        } else if (src[i] == 'T') {
                            i++;
                            func_flags &= ~(fufl_enable|fufl_disable|fufl_toggle|fufl_set);
                            func_flags |= fufl_toggle;
                        } else if (src[i] == 'S') {
                            i++;
                            func_flags &= ~(fufl_enable|fufl_disable|fufl_toggle|fufl_set);
                            func_flags |= fufl_set;
                        } else if (src[i] == 'g') {
                            i++;
                            while_is(isspace,src,i,s);
                            if (src[i] == '-') {
                                i++;
                                func_groupx = cs->tabs[cs->ctab].sel;
                            } else {
                                ul t1,t2;
                                i += get_range(src+i,&t1,&t2,(size_t(*)(const char*,ul*))get_dec);
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
                        } else if (src[i] == 't') {
                            i++;
                            while_is(isspace,src,i,s);
                            ul t1,t2;
                            i += get_range(src+i,&t1,&t2,(size_t(*)(const char*,ul*))get_dec);
                            func_tabx = t1;
                            if (func_tabx >= TABS)
                                func_tabx = TABS-1;
                            if (t2 == 0)
                                func_taby = -2;
                            else {
                                func_taby = t2;
                                if (func_taby >= TABS)
                                    func_taby = TABS-1;
                            }
                        }
                    }
                    while (i < s && !isspace(src[i]))
                        i++;
                }
                break;
            }
            continue;
        }
        if (isspace(src[i]))
            continue;


        char *r = get_path(path_tmp,src+i,' ',s-i,PATH_MAX,cs);
        i = r-src+1;
        if (r == NULL)
            continue;
        realpath(path_tmp,rpath);
        rpathl = strlen(rpath);
        uchar found = 0;
        for (j = 0; j < dirs->size; j++) {
            if (((lflags&D_RECURSIVE) ? (rpathl <= dir[j].plen && (dir[j].path[rpathl] == '/' || dir[j].path[rpathl] == 0)) : rpathl == dir[j].plen) && memcmp(dir[j].path,rpath,rpathl) == 0) {
                found = 1;
                *((size_t*)flexarr_inc(dir_list)) = j;
                if (flags&fl_load) {
                    li t = dirs->size;
                    if (getdir(rpath,dirs,lflags) == -1)
                        goto END1;
                    dir = (xdir*)dirs->v;
                    for (size_t n = (size_t)t; n < dirs->size; n++)
                        *((size_t*)flexarr_inc(dir_list)) = n;
                }
                if (!(lflags&D_RECURSIVE))
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
            } else
                goto END1;
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
                    goto END2;
                }
                if ((fd2 = open(d->path,O_DIRECTORY)) == -1)
                    goto END2;
                
                func_fmod_count = 1;
                if ((f->mode&S_IFMT) == S_IFDIR) {
                    if ((fd3 = openat(fd2,f->name,O_DIRECTORY)) == -1)
                        goto END2;
                    if (get_dirsize(fd3,&func_fmod_count,&func_fmod_size,D_R|D_C|D_S) == -1)
                        close(fd3);
                }
                int act = 0,ev = -1;
                switch (func_flags&(fufl_delete|fufl_move|fufl_copy)) {
                    case fufl_delete: act = 0; break;
                    case fufl_move: act = 1; break;
                    case fufl_copy: act = 2; break;
                    default: goto END2;
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
                        goto END2;
                    ev = getinput(cs);
                } while (1);
                
                switch (func_flags&(fufl_delete|fufl_move|fufl_copy)) {
                    case fufl_delete: file_rm(fd2,f->name); break;
                    case fufl_move: file_mv(func_target_fd,fd2,f->name,func_buffer,func_fmod_flags); break;
                    case fufl_copy: file_cp(func_target_fd,fd2,f->name,func_buffer,func_fmod_flags); break;
                }
                ret = 0;
                END2: ;
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
                    if (getdir(path,dirs,lflags) == -1)
                        return -1;
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
                func_bulk_comment_write = (func_flags&fufl_no_comments)>>6;
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
                    memcpy(t,&d->files[dir->size],sizeof(xfile));
                    memcpy(&d->files[dir->size],f,sizeof(xfile));
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
            if (spawn(func_bulk_editor,func_bulk_tfile,NULL,F_NORMAL|F_WAIT) != 0)
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
                        goto END3;
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
                            fprintf(func_bulk_file,"%s ",func_bulk_begin);
                            if (!(func_flags&fufl_full))
                                t = mkpath(d->path,t);
                            strtoshellpath(t);
                            fprintf(func_bulk_file,"%s %s ",t,func_bulk_middle);
                            t = mkpath(d->path,func_bulk_path);
                            strtoshellpath(t);
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
        END3: ;

        if (func == func_bulk) {
            fflush(func_bulk_file);
        
            if (writed == 0) {
                ret = 0;
                goto END1;
            }

        if (spawn(func_bulk_editor,func_bulk_tfile,NULL,F_NORMAL|F_WAIT) != 0 ||
            spawn(func_bulk_shell,func_bulk_tfile,NULL,F_NORMAL|F_WAIT|F_CONFIRM) != 0)
            goto END1;
        }
    }

    ret = 0;
    END1: ;
    flexarr_free(dir_list);
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
    
    return ret;
}

int
cmd_source(char *src, csas *cs)
{
    char path[PATH_MAX];
    size_t size=strlen(src),pos=0;
    while_is(isspace,src,pos,size);
    if (get_path(path,src+pos,' ',size-pos,PATH_MAX,cs) == NULL) {
        errno = ENOENT;
        return -1;
    }
    
    endwin();
    int r = config_load(path,cs);
    int e = errno;
    refresh();
    errno = e;
    return r;
}

int
cmd_cd(char *src, csas *cs)
{
    size_t size=strlen(src),pos = 0;
    char path[PATH_MAX];
    while_is(isspace,src,pos,size);
    if (get_path(path,src+pos,' ',size-pos,PATH_MAX,cs) == NULL) {
        errno = ENOENT;
        return -1;
    }

    return csas_cd(path,cs);
}

int
cmd_file_run(char *src, csas *cs)
{
    char path[PATH_MAX];
    size_t size=strlen(src),pos=0;
    while_is(isspace,src,pos,size);
    if (get_path(path,src+pos,' ',size-pos,PATH_MAX,cs) == NULL) {
        errno = ENOENT;
        return -1;
    }

    struct stat statbuf;
    if (stat(path,&statbuf) != 0)
        return -1;
    if ((statbuf.st_mode&S_IFMT) != S_IFDIR)
        return file_run(path,cs);

    return csas_cd(path,cs);
}

int
cmd_exec(char *src, csas *cs)
{
    size_t pos = 0;
    uchar flags = F_MULTI;
    char line[LLINE_MAX];

    while (src[pos]) {
        if (src[pos] == '-') {
            do {
                pos++;
                switch (src[pos]) {
                    case 's': flags |= F_SILENT; break;
                    case 'n': flags |= F_NORMAL; break;
                    case 'c': flags |= F_CONFIRM; break;
                    case 'w': flags |= F_WAIT; break;
                }
            } while (src[pos] && !isspace(src[pos]));
        } else if (!isspace(src[pos])) {
            char *r = get_path(line,src+pos,'\0',strlen(src+pos),LLINE_MAX,cs);
            if (r == NULL) {
                pos++;
                continue;
            }
            pos = r-src+1;
            break;
        }
        while (src[pos] && isspace(src[pos]))
            pos++;
    }
    return spawn(line,NULL,NULL,flags);
}

int
cmd_alias(char *src, csas *cs)
{
    size_t pos=0,s=strlen(src);
    char name[FUNCTIONS_NAME_MAX],line[LLINE_MAX],*r;

    while (isspace(src[pos]))
        pos++;

    r = get_path(name,src+pos,' ',s-pos,FUNCTIONS_NAME_MAX,cs);
    ret_errno(r==NULL,EINVAL,-1);
    pos = r-src+1;

    while (isspace(src[pos]))
        pos++;

    r = get_path(line,src+pos,' ',s-pos,LLINE_MAX,cs);
    ret_errno(r==NULL,EINVAL,-1);
    return xfunc_add(name,'a',strdup(line),NULL,cs->functions);
}

int
cmd_map(char *src, csas *cs)
{
    size_t pos = 0;
    char keys[BINDING_KEY_MAX],line[PATH_MAX],*r;

    while (isspace(src[pos]))
        pos++;

    r = get_path(keys,src+pos,' ',strlen(src+pos),BINDING_KEY_MAX,cs);
    ret_errno(r==NULL,EINVAL,-1);
    pos = r-src+1;

    while (isspace(src[pos]))
        pos++;

    r = get_path(line,src+pos,' ',strlen(src+pos),PATH_MAX,cs);
    ret_errno(r==NULL,EINVAL,-1);
    pos = r-src+1;

    return xbind_add(keys,line,cs->bindings);
}

int
cmd_quit(char *src, csas *cs)
{
    int n=0;
    size_t i,pos=0;
    while (isspace(src[pos]))
        pos++;
    if (src[0] == '-' && src[1] == 'f')
        goto END;

    for (i = 0; i < TABS; i++)
        n += cs->tabs[i].flags&T_EXISTS;

    cs->tabs[cs->ctab].flags &= ~T_EXISTS;

    if (n > 1) {
        for (i = cs->ctab+1; i < TABS; i++)
            if (cs->tabs[i].flags&T_EXISTS)
                return tab_d(i,-1,cs);
        for (i = 0; i < cs->ctab; i++)
            if (cs->tabs[i].flags&T_EXISTS)
                return tab_d(i,-1,cs);
    }

    END: ;
    Exit = 1;
    return 0;
}


int
cmd_set(char *src, csas *cs)
{
    size_t pos=0,s=strlen(src);
    char name[VARS_NAME_MAX],val[PATH_MAX],*r;
    name[0] = 0;
    val[0] = 0;
    uchar string=0;

    while (pos < s) {
        if (src[pos] == '-' && src[pos+1] == 's') {
            string = 1;
            pos += 2;
        } else {
            if (name[0] == 0)
                r = get_path(name,src+pos,' ',s-pos,VARS_NAME_MAX,cs);
            else
                r = get_path(val,src+pos,' ',s-pos,PATH_MAX,cs);
            if (r == NULL)
                continue;
            pos = r-src+1;
        }
        pos++;
        while (isspace(src[pos]))
            pos++;
    }

    ret_errno(!name[0]||!val[0],EINVAL,-1);
    return xvar_add(NULL,name,string == 0 ? 'i' : 's',val,cs->vars);
}

int
cmd_rename(char *src, csas *cs)
{
    char name[2][NAME_MAX],*n;
    size_t pos=0;

    while (isspace(src[pos]))
        pos++;
    if (get_path(name[0],src+pos,'\0',strlen(src)-pos,NAME_MAX,cs) == NULL)
        return -1;

    n = name[1];
    console_getline(&n,1,"rename ",(char*)name[0],-1,cs,NULL);
    return rename((char*)name[0],(char*)name[1]);
}

int
cmd_open_with(char *src, csas *cs)
{
    char file[PATH_MAX],path[PATH_MAX],*n;
    size_t pos=0;

    while (isspace(src[pos]))
        pos++;
    if (get_path(file,src+pos,'\0',strlen(src)-pos,PATH_MAX,cs) == NULL)
        return -1;
    if (access(file,R_OK) != 0)
        return -1;

    n = path;
    n[0] = 0;
    console_getline(&n,1,"open_with ",NULL,-1,cs,expand_shell_commands);
    pos = 0;
    while (n[pos] && !isspace(n[pos]))
        pos++;
    n[pos] = 0;
    return spawn(n,file,NULL,F_NORMAL|F_WAIT);
}

int
cmd_sort(char *src, csas *cs)
{
    xdir *dir = &CTAB(1);
    if (dir->size > 0) {
        xfile_sort(dir->files,dir->size,SortMethod);
        dir->sort = SortMethod;
    }
    return 0;
}

int
cmd_lmove(char *src, csas *cs)
{
    xdir *dir = &CTAB(1);
    if (dir->size == 0)
        return 0;
    xfile *files = dir->files;
    size_t pos=0,mul=(size_t)atol(cs->typed),ctab=cs->ctab;
    if (mul == 0)
        mul = 1;
    uchar flags = 0;
    char **searchl = (char**)dir->searchlist->v;

    while (src[pos]) {
        if (src[pos] == '-') {
            do {
                pos++;
                switch (src[pos]) {
                    case 'p': //previous
                    case 'n': //next
                        flags = (src[pos] == 'n' ? 0x2 : 0x1);
                        pos++;
                        while (isspace(src[pos]))
                            pos++;
                        pos += calc(src+pos,(li*)&mul,cs->vars);
                        break;
                }
            } while (src[pos] && !isspace(src[pos]));
        }
        pos++;
        while (isspace(src[pos]))
            pos++;
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
