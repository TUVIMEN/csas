#include "main.h"
#include "useful.h"
#include "load.h"
#include "draw.h"
#include "csas.h"
#include "console.h"
#include "functions.h"

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
    event = passed[0];
    free(passed);
    return (uint)event;

    EXIT: ;
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
                command_run(line,cs);
            }
        }
    }

    cs->typed[0] = 0;
    errno = EINVAL;
    return -1;
}

static void
move_d(xdir *dir, const size_t value, const uchar flags)
{
    size_t t = dir->sel;
    switch (flags&3) {
        case MOVE_SET: t = value; break;
        case MOVE_UP: t += value; break;
        case MOVE_DOWN:
            if (t < value)
                t = 0;
            else
                t -= value;
            break;
    }

    if (t > dir->size-1)
        t = dir->size-1;
    dir->sel = t;
}

int
cmd_move(char *src, csas *cs)
{
    uchar flags = MOVE_UP;
    size_t value=(size_t)atol(cs->typed),pos=0,tab=cs->ctab;
    if (value == 0)
        value = 1;

    xdir *dir = &CTAB;

    if (dir->size == 0)
        return 0;

    while (src[pos]) {
        if (src[pos] == '-') {
            do {
                pos++;
                switch (src[pos]) {
                    case 'u': flags = MOVE_UP; break;
                    case 'd': flags = MOVE_DOWN; break;
                    case 's': flags = MOVE_SET; value--; break;
                    case 'w':
                        pos++;
                        while (isspace(src[pos]))
                            pos++;
                        tab = (size_t)atol(src+pos);
                        while (isdigit(src[pos]))
                            pos++;
                        if (tab < TABS)
                            dir = &TAB(tab);
                }
            } while (src[pos] && !isspace(src[pos]));
        } else if (src[pos] == '$') {
            value = dir->size-1;
        } else if (isdigit(src[pos])) {
            value = (size_t)atoll(src+pos);
            while (isdigit(src[pos]))
                pos++;
        }
        pos++;
    }

    move_d(dir,value,flags);
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

    if (cs->tabs[tabc].flags&T_EXISTS) {
        if (tabc != cs->ctab)
            if (chdir(TAB(tabc).path) != 0)
                return -1;
        goto END;
    }

    cs->tabs[tabc].t = cs->tabs[cs->ctab].t;
    cs->tabs[tabc].flags |= T_EXISTS;
    END: ;
    cs->ctab = tabc;
    if (sel >= 0 && sel < 8)
        cs->tabs[tabc].sel = sel;
    return 0;
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
            r = get_path(first,src+pos,' ',s-pos,NAME_MAX,&CTAB);
            if (r == NULL)
                continue;
            pos = r-src+1;
        } else {
            r = get_path(add,src+pos,' ',s-pos,PATH_MAX,&CTAB);
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

    console_getline((char**)history->v,history->size,first,add,cs);
    char *line = ((char**)history->v)[history->size-1];
    if (command_run(line,cs) != 0)
        printmsg(ERROR_C,"%s: %s",line,strerror(errno));
    return 0;
}

int
cmd_select(char *src, csas *cs)
{
    size_t pos=0,plen,i,j;
    uchar flags=1;
    int selected=-1,tab1=cs->ctab,tab2=cs->ctab,toselected=-1;
    char path[PATH_MAX];
    path[0] = 0;

    while (src[pos]) {
        while (isspace(src[pos]))
            pos++;

        if (src[pos] == '-') {
            do {
                pos++;
                switch (src[pos]) {
                    case 'r': flags |= 0x4; break;
                    case 'o':
                        pos++;
                        while (src[pos] && !isspace(src[pos]))
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
                    case 'w':
                    case 'W': {
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
                    case 's':
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
                                selected = 1<<atoi(src+pos);
                                while (isdigit(src[pos]))
                                    pos++;
                                break;
                        }
                        break;
                    case 'e': flags |= 1; break;
                    case 'd': flags &= ~3; break;
                    case 't': flags |= 2; break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }
        else {
            char *r = get_path(path,src+pos,' ',strlen(src+pos),PATH_MAX,&TAB(tab1));
            if (r == NULL)
                continue;
            pos = r-src+1;
        }
        if (!src[pos+1])
            break;
    }

    ret_errno((!(cs->tabs[tab1].flags&T_EXISTS))||(!(cs->tabs[tab2].flags&T_EXISTS)),EINVAL,-1);

    tab *tabs = cs->tabs;
    flexarr *dirs = cs->dirs;
    xdir *dir = (xdir*)dirs->v;

    if (selected == -2)
        selected = tabs[tab1].sel;
    if (toselected == -1)
        toselected = tabs[tab2].sel;
    if (selected == -3) {
        if (!(flags&0x4))
            return 0;
        switch (flags&0x3) {
            case 0: dir[tabs[tab1].t].files[dir[tabs[tab1].t].sel].sel[tab2] &= ~(1<<toselected); break;
            case 1: dir[tabs[tab1].t].files[dir[tabs[tab1].t].sel].sel[tab2] |= 1<<toselected; break;
            default: dir[tabs[tab1].t].files[dir[tabs[tab1].t].sel].sel[tab2] ^= 1<<toselected;
        }
    }

    if (path[0]) {
        char t[PATH_MAX];
        if (realpath(path,t) == NULL)
            return -1;
        strcpy(path,t);
    }
    plen = strlen(path);

    for (i = 0; i < dirs->size; i++) {
        xdir *d = &dir[i];
        if (d->size == 0)
            continue;
        if (path[0]) {
            if (flags&0x4) {
                if (plen > d->plen || memcmp(d->path,path,plen) != 0)
                    continue;
            }
            else if (plen != d->plen || memcmp(path,d->path,plen) != 0)
                continue;
        }

        for (j = 0; j < d->size; j++) {
            if (selected < 0 ? 1 : (d->files[j].sel[tab1]&(1<<selected))) {
                switch (flags&0x3)
                {
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
    xdir *dir = &CTAB;
    ret_errno(dir->size==0,EINVAL,-1);
    dir->files[dir->sel].sel[cs->ctab] ^= (1<<cs->tabs[cs->ctab].sel);
    move_d(dir,1,MOVE_UP);
    return 0;
}

int
cmd_source(char *src, csas *cs)
{
    char path[PATH_MAX];
    xdir *dir = &CTAB;
    size_t size=strlen(src),pos=0;
    while_is(isspace,src,pos,size);
    if (get_path(path,src+pos,' ',size-pos,PATH_MAX,dir) == NULL) {
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
    char path[PATH_MAX], *search_name = NULL;
    xdir *dir = &CTAB;
    while_is(isspace,src,pos,size);
    if (get_path(path,src+pos,' ',size-pos,PATH_MAX,dir) == NULL) {
        errno = ENOENT;
        return -1;
    }
    if (path[0] == '.' && path[1] == '.' && path[2] == 0)
        search_name = memrchr(dir->path,'/',dir->plen);
    if (get_dir(path,cs->dirs,&cs->tabs[cs->ctab].t,D_CHDIR|D_MODE_ONCE) != 0)
        return -1;

    if (!search_name)
        return 0;
    search_name++;
    size_t nlen = strlen(search_name);
    if (nlen == 0)
        return 0;
    dir = &CTAB;
    xfile *files = dir->files;
    for (size_t i = 0; i < dir->size; i++) {
        if (nlen == files[i].nlen 
                && memcmp(search_name,files[i].name,nlen) == 0) {
            dir->sel = i;
            break;
        }
    }

    return 0;
}

int
cmd_file_run(char *src, csas *cs)
{
    char path[PATH_MAX];
    xdir *dir = &CTAB;
    size_t size=strlen(src),pos=0;
    while_is(isspace,src,pos,size);
    if (get_path(path,src+pos,' ',size-pos,PATH_MAX,dir) == NULL) {
        errno = ENOENT;
        return -1;
    }

    struct stat statbuf;
    if (stat(path,&statbuf) != 0)
        return -1;
    if ((statbuf.st_mode&S_IFMT) != S_IFDIR)
        return file_run(path);
    return get_dir(path,cs->dirs,&cs->tabs[cs->ctab].t,D_CHDIR|D_MODE_ONCE);
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
            char *r = get_path(line,src+pos,'\0',strlen(src+pos),LLINE_MAX,&CTAB);
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
    size_t pos = 0,s;
    char name[NAME_MAX],*line,*r;

    while (src[pos] && isspace(src[pos]))
        pos++;
    r = strchr(src+pos,'=');
    s = r-(src+pos);
    errno = EINVAL;
    if (r == NULL || s > NAME_MAX)
        return -1;
    memcpy(name,src+pos,s);
    name[s] = 0;
    pos += s+1;

    line = malloc(LLINE_MAX);
    r = get_path(line,src+pos,' ',strlen(src+pos),LLINE_MAX,&CTAB);
    errno = EINVAL;
    if (r == NULL) {
        free(line);
        return -1;
    }
    pos = r-src+1;
    xfunc_add(name,'a',line,cs->functions);
    return 0;
}
