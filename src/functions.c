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
extern li Error_C;
extern li Borders;
extern li SortMethod;
extern li ShowKeyBindings;

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
        if (n >= NUM_MAX || event == 27)
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
command_run(char *src, csas *cs)
{
    size_t size=strlen(src),pos=0,t,s;
    if (size == 0)
        return 0;

    while_is(isspace,src,pos,size);
    t = pos;
    while_isnt(isspace,src,pos,size);
    s = pos-t;
    ret_errno(size==0,EINVAL,-1);
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
                char line[LLINE_MAX];
                s = strlen(functions[i].func);
                if (s > LLINE_MAX)
                    return 0;
                memcpy(line,functions[i].func,s);
                line[s++] = ' ';
                size_t c = strlen(src+pos);
                if (s+c > LLINE_MAX)
                    return 0;
                memcpy(line+s,src+pos,c);
                s += c;
                line[s] = 0;
                return command_run(line,cs);
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
    if (value == 0)
        value = 1;

    xdir *dir = &CTAB(1);

    if (dir->size == 0)
        return 0;

    while (src[pos]) {
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
        pos++;
        while (isspace(src[pos]))
            pos++;
    }

    move_d(dir,value,cs->ctab,flags);
    if (dir->size)
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
    add[0] = '\0';

    while (src[pos]) {
        while (isspace(src[pos]))
            pos++;
        if (src[pos] == '-' && src[pos+1] == 'f') {
            pos += 2;
            while (isspace(src[pos]))
                pos++;
            r = get_path(first,src+pos,' ',s-pos,NAME_MAX,cs);
            if (r == NULL)
                continue;
            pos = r-src+1;
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

    console_getline((char**)history->v,history->size,first,add,cs,expand_commands);
    char *line = ((char**)history->v)[history->size-1];
    if (command_run(line,cs) != 0)
        printmsg(Error_C,"%s: %s",line,strerror(errno));
    return 0;
}

int
cmd_select(char *src, csas *cs)
{
    size_t pos=0,plen=0,i,j;
    uchar flags=1;
    int selected=-1,tab1=cs->ctab,tab2=cs->ctab,toselected=-1;
    char path[PATH_MAX];

    while (src[pos]) {
        while (isspace(src[pos]))
            pos++;

        if (src[pos] == '-') {
            do {
                pos++;
                switch (src[pos]) {
                    case 'r': flags |= 0x4; break; //recursive
                    case 'o': //dest sel
                        pos++;
                        while (isspace(src[pos]))
                            pos++;
                        switch(src[pos]) {
                            case '.':
                                toselected = -1;
                                pos++;
                                break;
                            default:
                                toselected = atoi(src+pos);
                                while (isdigit(src[pos]))
                                    pos++;
                                break;
                        }
                        break;
                    case 'w': //src tab
                    case 'W': { //dest tab
                            char c = src[pos];
                            pos++;
                            while (src[pos] && !isspace(src[pos]))
                                pos++;
                            if (c == 'w')
                                tab1 = atoi(src+pos);
                            else
                                tab2 = atoi(src+pos);
                            while (isdigit(src[pos]))
                                pos++;
                        }
                        break;
                    case 's': //src sel
                        pos++;
                        while (isspace(src[pos]))
                            pos++;
                        switch (src[pos]) {
                            case '-':
                                selected = -1;
                                pos++;
                                break;
                            case '.':
                                selected = -2;
                                pos++;
                                break;
                            case 's':
                                selected = -3;
                                pos++;
                                break;
                            default:
                                selected = atoi(src+pos);
                                while (isdigit(src[pos]))
                                    pos++;
                                break;
                        }
                        break;
                    case 'e': flags |= 1; break; //enable
                    case 'd': flags &= ~3; break; //disable
                    case 't': flags |= 2; break; //toggle
                }

            } while (src[pos] && !isspace(src[pos]));
        } else {
            size_t t = pos;
            char *r = get_path(path,src+pos,' ',strlen(src+pos),PATH_MAX,cs);
            if (r == NULL)
                continue;
            pos = r-src+1;
            plen = pos-t;
        }
        if (!src[pos+1])
            break;
    }

    ret_errno((!(cs->tabs[tab1].flags&T_EXISTS))||(!(cs->tabs[tab2].flags&T_EXISTS)),EINVAL,-1);

    xtab *tabs = cs->tabs;
    flexarr *dirs = cs->dirs;
    xdir *dir = (xdir*)dirs->v;

    if (selected == -2)
        selected = tabs[tab1].sel;
    if (toselected == -1)
        toselected = tabs[tab2].sel;
    if (selected == -3) {
        if (!(flags&0x4))
            return 0;
        uchar *s = dir[tabs[tab1].wins[1]].files[dir[tabs[tab1].wins[1]].sel[tab1]].sel;
        switch (flags&0x3) {
            case 0: s[tab2] &= ~(1<<toselected); break;
            case 1: s[tab2] |= 1<<toselected; break;
            default: s[tab2] ^= 1<<toselected;
        }
    }

    if (plen) {
        char *t = path;
        if (realpath(path,t) == NULL)
            return -1;
        plen = strlen(path);
    }

    for (i = 0; i < dirs->size; i++) {
        xdir *d = &dir[i];
        if (d->size == 0 || (plen && (flags&0x4 ? plen > d->plen : plen != d->plen || memcmp(path,d->path,plen) != 0)))
            continue;

        for (j = 0; j < d->size; j++) {
            if (selected < 0 ? 1 : (d->files[j].sel[tab1]&(1<<selected))) {
                switch (flags&0x3) {
                    case 0: d->files[j].sel[tab2] &= ~(1<<toselected); break;
                    case 1: d->files[j].sel[tab2] |= 1<<toselected; break;
                    default: d->files[j].sel[tab2] ^= 1<<toselected;
                }
            }
        }
    }

    return 0;
}

int
cmd_fastselect(char *src, csas *cs)
{
    xdir *dir = &CTAB(1);
    ret_errno(dir->size==0,EINVAL,-1);
    dir->files[dir->sel[cs->ctab]].sel[cs->ctab] ^= (1<<cs->tabs[cs->ctab].sel);
    move_d(dir,1,cs->ctab,MOVE_UP);
    return 0;
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
    char name[NAME_MAX],*line,*r;

    while (isspace(src[pos]))
        pos++;

    r = get_path(name,src+pos,' ',s-pos,FUNCTIONS_NAME_MAX,cs);
    ret_errno(r==NULL,EINVAL,-1);
    pos = r-src+1;

    while (isspace(src[pos]))
        pos++;

    line = malloc(LLINE_MAX);
    r = get_path(line,src+pos,' ',s-pos,LLINE_MAX,cs);
    errno = EINVAL;
    if (r == NULL) {
        free(line);
        return -1;
    }
    return xfunc_add(name,'a',line,NULL,cs->functions);
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
cmd_ds(char *src, csas *cs)
{
    size_t pos=0,plen=0,s=strlen(src);
    uchar flags=D_S;
    int selected=-1,tab=cs->ctab;
    char path[PATH_MAX];

    while (src[pos] && src[pos+1]) {
        while (isspace(src[pos]))
            pos++;
        if (src[pos] == '-') {
            do {
                pos++;
                switch (src[pos]) {
                    case 'w': //tab
                    case 's': { //sel
                            char c = src[pos];
                            pos++;
                            while (isspace(src[pos]))
                                pos++;
                            if (c == 'w') {
                                tab = atoi(src+pos);
                                if (!(cs->tabs[tab].flags&T_EXISTS))
                                    return 0;
                            } else {
                                switch (src[pos]) {
                                    case '-': selected = -1; pos++; break;
                                    case '.': selected = -2; pos++; break;
                                    case 's': selected = -3; pos++; break;
                                    default: selected = atoi(src+pos);
                                }
                            }
                            while (isdigit(src[pos]))
                                pos++;
                        }
                        break;
                    case 'R': flags |= 0x80; break;
                    case 'r': flags |= D_R; break;
                    case 'c': flags |= D_C; flags &= ~D_S; break;
                    case 'f': flags |= D_F; break;
                }
            } while (src[pos] && !isspace(src[pos]));
        } else {
            size_t t = pos;
            char *r = get_path(path,src+pos,' ',s-pos,PATH_MAX,cs);
            if (r == NULL)
                continue;
            pos = r-src+1;
            plen = pos-t;
        }
    }

    if (selected == -2) {
        selected = cs->tabs[tab].sel;
    } else if (selected == -3) {
        xdir *dir = &TAB(tab,1);
        int dfd,fd;
        dfd = open(dir->path,O_DIRECTORY);
        if (dfd == -1)
            return -1;
        if (dir->size == 0)
            return 0;
        xfile *file = &dir->files[dir->sel[tab]];
        fd = openat(dfd,file->name,O_DIRECTORY);
        if (fd == -1)
            return -1;
        close(dfd);
        int e,t;
        file->size = 0;
        t = get_dirsize(fd,&file->size,&file->size,flags);
        e = errno;
        close(fd);
        errno = e;
        return t;
    }
    if (plen) {
        char *t = path;
        if (realpath(path,t) == NULL)
            return -1;
        plen = strlen(path);
    }

    int dfd,fd;
    flexarr *dirs = cs->dirs;
    xdir *dir = (xdir*)dirs->v;

    for (size_t i = 0; i < dirs->size; i++) {
        xdir *d = &dir[i];
        if (d->size == 0 || (plen && (flags&0x80 ? plen > d->plen : plen != d->plen || memcmp(path,d->path,plen) != 0)))
            continue;

        if ((dfd = open(d->path,O_DIRECTORY)) == -1)
            continue;

        for (size_t j = 0; j < d->size; j++) {
            if (selected < 0 ? 1 : (d->files[j].sel[tab]&(1<<selected))) {
                if ((fd = openat(dfd,d->files[j].name,O_DIRECTORY)) == -1)
                    continue;
                d->files[j].size = 0;
                get_dirsize(fd,&d->files[j].size,&d->files[j].size,flags);
            }
        }
        close(dfd);
    }

    return 0;
}

int
cmd_fmod(char *src, csas *cs)
{
    uchar act = 2;
    mode_t flags = 0;
    size_t pos = 0,s=strlen(src),plen=0;
    int selected=-1,tab=cs->ctab;
    char path[PATH_MAX],target[PATH_MAX]=".";
    static char *name[]={"delete","move","copy"};
    int fd1=-1,fd2=-1,fd3=-1;
    struct stat statbuf;

    while (src[pos] && src[pos+1]) {
        while (isspace(src[pos]))
            pos++;

        if (src[pos] == '-') {
            do {
                pos++;
                switch (src[pos]) {
                    case 'o': //dest
                        pos++;
                        while (isspace(src[pos]))
                            pos++;
                        char *r = get_path(target,src+pos,' ',s-pos,PATH_MAX,cs);
                        if (r == NULL)
                            continue;
                        pos = r-src+1;
                        break;
                    case 'w': //tab
                    case 's': { //sel
                            char c = src[pos];
                            pos++;
                            while (isspace(src[pos]))
                                pos++;
                            if (c == 'w') {
                                tab = atoi(src+pos);
                                if (!(cs->tabs[tab].flags&T_EXISTS))
                                    return 0;
                            } else {
                                switch (src[pos]) {
                                    case '-': selected = -1; pos++; break;
                                    case '.': selected = -2; pos++; break;
                                    case 's': selected = -3; pos++; break;
                                    default: selected = atoi(src+pos);
                                }
                            }
                            while (isdigit(src[pos]))
                                pos++;
                        }
                        break;
                    case 'c': flags |= M_CHNAME; break;
                    case 'r': flags |= M_REPLACE; break;
                    case 'd': flags |= M_DCPY; break;
                    case 'm': flags |= M_MERGE; break;
                    case 'D': act = 0; break; //delete
                    case 'M': act = 1; break; //move
                    case 'C': act = 2; break; //copy
                }
            } while (src[pos] && !isspace(src[pos]));
        } else {
            size_t t = pos;
            char *r = get_path(path,src+pos,' ',s-pos,PATH_MAX,cs);
            if (r == NULL)
                continue;
            pos = r-src+1;
            plen = pos-t;
        }
    }

    if (selected == -2)
        selected = cs->tabs[tab].sel;
    if ((fd1 = open(target,O_DIRECTORY)) == -1)
        return -1;

    char *buffer = (act == 0 ? NULL : (char*)malloc(BufferSize));
    off_t count=0,size=0;
    int ev=-1;

    if (selected == -3) {
        xdir *dir = &TAB(tab,1);
        if (dir->size == 0) {
            close(fd1);
            return 0;
        }
        if ((fd2 = open(dir->path,O_DIRECTORY)) == -1)
            goto END;

        xfile *file = &dir->files[dir->sel[tab]];
        count = 1;
        if ((file->mode&S_IFMT) == S_IFDIR) {
	    if ((fd3 = openat(fd2,file->name,O_RDONLY)) == -1)
	        goto END;
            get_dirsize(fd3,&count,&size,D_R|D_C|D_S);
	    close(fd3);
        }

        do {
            if ((file->mode&S_IFMT) == S_IFDIR)
                printmsg(0,"Do you want to %s %ld files(%s)? (Y/n)",name[act],count,size_shrink(size));
            else
                printmsg(0,"Do you want to %s \"%s\" (%s)? (Y/n)",name[act],file->name,size_shrink(file->size));
            refresh();
            if (ev == 'y' || ev == 'Y')
                break;
            if (ev != -1)
                goto END;
            ev = getinput(cs);
        } while (1);

        switch (act) {
            case 0: file_rm(fd2,file->name); break;
            case 1: file_mv(fd1,fd2,file->name,buffer,flags); break;
            case 2: file_cp(fd1,fd2,file->name,buffer,flags); break;
        }
        goto END;
    }

    if (plen) {
        char *t = path;
        if (realpath(path,t) == NULL)
            goto END;
        plen = strlen(path);
    }
    
    flexarr *dirs = cs->dirs;
    xdir *dir = (xdir*)dirs->v;
    for (size_t i = 0; i < dirs->size; i++) {
        xdir *d = &dir[i];
        if (d->size == 0 || (plen && (plen != d->plen || memcmp(path,d->path,plen) != 0)))
            continue;
        if ((fd2 = open(d->path,O_DIRECTORY)) == -1)
            continue;
        for (size_t j = 0; j < d->size; j++) {
            if (selected < 0 ? 1 : (d->files[j].sel[tab]&(1<<selected))) {
                if ((fd3 = openat(fd2,d->files[j].name,O_DIRECTORY)) == -1) {
                    count++;
                    fstatat(fd2,d->files[j].name,&statbuf,AT_SYMLINK_NOFOLLOW);
                    size += statbuf.st_size;
                } else {
                    get_dirsize(fd3,&count,&size,D_R|D_C|D_S);
                    close(fd3);
                }
            }
        }
        close(fd2);
    }
    fd2 = -1;
    ev = -1;
    do {
        printmsg(0,"Do you want to %s %ld files(%s)? (Y/n)",name[act],count,size_shrink(size));
        refresh();
        if (ev == 'y' || ev == 'Y')
            break;
        if (ev != -1)
            goto END;
        ev = getinput(cs);
    } while (1);

    for (size_t i = 0; i < dirs->size; i++) {
        xdir *d = &dir[i];
        if (d->size == 0 || (plen && (plen != d->plen || memcmp(path,d->path,plen) != 0)))
            continue;
        if ((fd2 = open(d->path,O_DIRECTORY)) == -1)
            continue;
        for (size_t j = 0; j < d->size; j++) {
            if (selected < 0 ? 1 : (d->files[j].sel[tab]&(1<<selected))) {
                switch (act) {
                    case 0: file_rm(fd2,d->files[j].name); break;
                    case 1: file_mv(fd1,fd2,d->files[j].name,buffer,flags); break;
                    case 2: file_cp(fd1,fd2,d->files[j].name,buffer,flags); break;
                }
            }
        }
        close(fd2);
    }
    close(fd1);
    fd1 = -1;
    fd2 = -1;

    END:
    close(fd1);
    close(fd2);
    free(buffer);
    return 0;
}

int
cmd_load(char *src, csas *cs)
{
    size_t pos=0,s=strlen(src);
    char path[PATH_MAX],*r;
    uchar flags=0;

    while (src[pos]) {
        if (src[pos] == '-') {
            do {
                pos++;
                switch (src[pos]) {
                    case 'm': //mode
                        pos++;
                        while (isspace(src[pos]))
                            pos++;
                        switch (atoi(src+pos)) {
                            case 0: flags |= D_MODE_ONCE; break;
                            case 1: flags |= D_MODE_CHANGE; break;
                            case 2: flags &= ~(D_MODE_ONCE|D_MODE_CHANGE); break;
                        }
                        while (isdigit(src[pos]))
                            pos++;
                        break;
                    case 'R': flags |= D_RECURSIVE; break;
                }
            } while (src[pos] && !isspace(src[pos]));
        } else {
            r = get_path(path,src+pos,' ',s-pos,PATH_MAX,cs);
            if (r == NULL)
                continue;
            pos = r-src+1;
            if (getdir(path,cs->dirs,flags) == -1)
                return -1;
        }
        while (isspace(src[pos]))
            pos++;
    }

    xdir *dir = &CTAB(1);
    if (dir->size)
        preview_get(&dir->files[dir->sel[cs->ctab]],cs);

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
    console_getline(&n,1,"rename ",(char*)name[0],cs,NULL);
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
    console_getline(&n,1,"open_with ",NULL,cs,expand_shell_commands);
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
    if (dir->size > 0)
        xfile_sort(dir->files,dir->size,SortMethod);
    return 0;
}

int
cmd_bulk(char *src, csas *cs)
{
    size_t pos=0,s=strlen(src);
    int tab=cs->ctab,selected=-1;
    uchar flags=0;
    char *args[6],*r,a[6][PATH_MAX],path[PATH_MAX]=".";
    for (int i = 0; i < 6; i++)
        args[i] = a[i];

    while (src[pos]) {
        if (src[pos] == '-') {
            do {
                pos++;
                switch (src[pos]) {
                    case 'n': flags |= 0x4; break; //no path in comment
                    case 'N': flags &= ~0x4; break;
                    case 'f': flags |= 0x1; break; //full path
                    case 'w': //tab
                    case 's': { //sel
                            char c = src[pos];
                            pos++;
                            while (isspace(src[pos]))
                                pos++;
                            if (c == 'w') {
                                tab = atoi(src+pos);
                                if (tab >= TABS || !(cs->tabs[tab].flags&T_EXISTS))
                                    return 0;
                            } else switch (src[pos]) {
                                case '-': selected = -1; pos++; break;
                                case '.': selected = -2; pos++; break;
                                default: selected = atoi(src+pos);
                            }
                            while (isdigit(src[pos]))
                                pos++;
                        }
                        break;
                    case 'R': flags |= 0x2; break; //recursive
                    case 'S': //shell
                    case 'E': //editor
                    case 'b': //first
                    case 'm': //middle
                    case 'e': { //end
                            while (isspace(src[pos]))
                                pos++;
                            int n;
                            switch (src[pos]) {
                                case 'S': n = 1; break;
                                case 'E': n = 2; break;
                                case 'b': n = 3; break;
                                case 'm': n = 4; break;
                                default: n = 5;
                            }
                            pos++;
                            while (isspace(src[pos]))
                                pos++;
                            r = get_path(args[n],src+pos,' ',s-pos,PATH_MAX,cs);
                            if (r == NULL)
                                continue;
                            pos = r-src+1;
                        }
                        break;
                }

            } while (src[pos] && !isspace(src[pos]));
        } else {
            while (isspace(src[pos]))
                pos++;
            r = get_path(path,src+pos,' ',s-pos,PATH_MAX,cs);
            if (r == NULL)
                continue;
            pos = r-src+1;
        }
        pos++;
        while (isspace(src[pos]))
            pos++;
    }
    

    if (selected == -2)
        selected = cs->tabs[tab].sel;
    if (realpath(path,args[0]) == NULL)
        return -1;

    return bulk(cs,tab,selected,(char**)args,flags);
}

int
cmd_search(char *src, csas *cs)
{
    xdir *dir = &CTAB(1);
    if (dir->size == 0)
        return 0;
    xfile *files = dir->files;
    size_t pos=0,mul=(size_t)atol(cs->typed),s=strlen(src),ctab=cs->ctab;
    if (mul == 0)
        mul = 1;
    int sel = -1;
    char pattern[NAME_MAX],*r;
    uchar flags = 0;
    char **searchl = (char**)dir->searchlist->v;

    while (src[pos]) {
        if (src[pos] == '-') {
            do {
                pos++;
                switch (src[pos]) {
                    case 's': //sel
                        pos++;
                        while (isspace(src[pos]))
                            pos++;
                        switch (src[pos]) {
                            case '-':
                                sel = -1;
                                pos++;
                                break;
                            case '.':
                                sel = cs->tabs[ctab].sel;
                                pos++;
                                break;
                            default:
                                sel = atoi(src+pos);
                                while (isdigit(src[pos]))
                                    pos++;
                                break;
                        }
                        break;
                    case 'p': //previous
                    case 'n': //next
                        flags = (src[pos] == 'n' ? 0x2 : 0x1);
                        pos++;
                        while (isspace(src[pos]))
                            pos++;
                        pos += calc(src+pos,(li*)&mul,cs->vars);
                        break;
                    case 'i': //case insensitive
                        flags |= 0x80;
                        break;
                    case 'N': //name
                    #ifdef REGEX
                    case 'G': //regex
                    case 'E': //extended regex
                    #endif
                        switch (src[pos]) {
                            case 'N': flags |= 0x4; break;
                            #ifdef REGEX
                            case 'G': flags |= 0x8; break;
                            case 'E': flags |= 0x10; break;
                            #endif
                        }
                        pos++;
                        while (isspace(src[pos]))
                            pos++;
                        r = get_path(pattern,src+pos,' ',s-pos,NAME_MAX,cs);
                        if (r == NULL)
                            continue;
                        pos = r-src+1;
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

    if (flags&0x3) {
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
        if (found)
            dir->sel[ctab] = i;
        return 0;
    }

    flexarr_free(dir->searchlist);
    dir->searchlist = flexarr_init(sizeof(char*),SEARCHLIST_INCR);
    dir->searchlist_pos = 0;
    #ifdef REGEX
    int cflags=(flags&0x80 ? REG_ICASE : 0)|(flags&0x10 ? REG_EXTENDED : 0);
    #endif
    char *(*cmp)(const char*,const char*)=(flags&0x80 ? strcasestr : strstr);
    #ifdef REGEX
    regex_t regex;
    if (flags&0x18) {
        if (regcomp(&regex,pattern,cflags) != 0) {
            errno = EFAULT;
            return -1;
        }
    }
    #endif
    
    for (size_t i = 0; i < dir->size; i++) {
        if (sel == -1 ? 0 : !(files[i].sel[ctab]&sel))
            continue;
        if (flags&0x4) {
            if (cmp(files[i].name,pattern) == NULL)
                continue;
        }
        #ifdef REGEX
        else if (regexec(&regex,files[i].name,0,NULL,0) != 0)
            continue;
        #endif
    
        *((char**)flexarr_inc(dir->searchlist)) = files[i].name;
    }

    #ifdef REGEX
    if (flags&0x18)
        regfree(&regex);
    #endif

    return 0;
}

int
cmd_filter(char *src, csas *cs)
{
    xdir *dir = &CTAB(1);
    if (dir->asize == 0)
        return 0;
    xfile *files = dir->files;
    size_t pos=0,ctab=cs->ctab,s=strlen(src);
    int sel = -1;
    char pattern[NAME_MAX],*r;
    uchar flags = 0;

    if (dir->asize == 0)
        dir->asize = dir->size;
    else
        dir->size = dir->asize;

    while (src[pos]) {
        if (src[pos] == '-') {
            do {
                pos++;
                switch (src[pos]) {
                    case 's': //sel
                        pos++;
                        while (isspace(src[pos]))
                            pos++;
                        switch (src[pos]) {
                            case '-':
                                sel = -1;
                                pos++;
                                break;
                            case '.':
                                sel = cs->tabs[ctab].sel;
                                pos++;
                                break;
                            default:
                                sel = atoi(src+pos);
                                while (isdigit(src[pos]))
                                    pos++;
                                break;
                        }
                        break;
                    case 'i': //case insensitive
                        flags |= 0x80;
                        break;
                    case 'v': //invert match
                        flags |= 0x20;
                        break;
                    case 'N': //name
                    #ifdef REGEX
                    case 'G': //regex
                    case 'E': //extended regex
                    #endif
                        switch (src[pos]) {
                            case 'N': flags |= 0x4; break;
                            #ifdef REGEX
                            case 'G': flags |= 0x8; break;
                            case 'E': flags |= 0x10; break;
                            #endif
                        }
                        pos++;
                        while (isspace(src[pos]))
                            pos++;
                        r = get_path(pattern,src+pos,' ',s-pos,NAME_MAX,cs);
                        if (r == NULL)
                            continue;
                        pos = r-src+1;
                        break;
                }
            } while (src[pos] && !isspace(src[pos]));
        }
        pos++;
        while (isspace(src[pos]))
            pos++;
    }

    if (flags == 0)
        goto END;

    #ifdef REGEX
    int cflags=(flags&0x80 ? REG_ICASE : 0)|(flags&0x10 ? REG_EXTENDED : 0);
    #endif
    char *(*cmp)(const char*,const char*)=(flags&0x80 ? strcasestr : strstr);
    #ifdef REGEX
    regex_t regex;
    if (flags&0x18) {
        if (regcomp(&regex,pattern,cflags) != 0) {
            errno = EFAULT;
            return -1;
        }
    }
    #endif

    dir->size = 0;
    char t[sizeof(xfile)];
    int n;
    for (size_t i = 0; i < dir->asize; i++) {
        if (sel == -1 ? 0 : !(files[i].sel[ctab]&sel))
            continue;

        n = 0;
        if (flags&0x4) {
            if (cmp(files[i].name,pattern) != NULL)
                n = 1;
        }
        #ifdef REGEX
        else if (regexec(&regex,files[i].name,0,NULL,0) == 0)
            n = 1;
        #endif
        if ((flags&0x20) ? n : !n)
            continue;

        if (i == dir->size) {
            dir->size++;
            continue;
        }

        memcpy(t,&files[dir->size],sizeof(xfile));
        memcpy(&files[dir->size],&files[i],sizeof(xfile));
        memcpy(&files[i],t,sizeof(xfile));
        dir->size++;
    }

    #ifdef REGEX
    if (flags&0x18)
        regfree(&regex);
    #endif

    END: ;
    xfile_sort(dir->files,dir->size,SORT_CNAME|SORT_DIR_DISTINCTION|SORT_LDIR_DISTINCTION);
    return 0;
}
