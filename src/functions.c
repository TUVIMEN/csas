#include "main.h"
#include "useful.h"
#include "load.h"
#include "draw.h"
#include "csas.h"
#include "console.h"
#include "functions.h"

extern uchar Exit;

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
                return command_run(line,cs);
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

static int
tab_d(size_t t, int sel, csas *cs)
{
    ret_errno(t>=TABS,ERANGE,-1);
    if (cs->tabs[t].flags&T_EXISTS) {
        if (t != cs->ctab)
            if (chdir(TAB(t).path) != 0)
                return -1;
        goto END;
    }

    cs->tabs[t].t = cs->tabs[cs->ctab].t;
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
            char *r = get_path(path,src+pos,' ',strlen(src+pos),PATH_MAX,&TAB(tab1));
            if (r == NULL)
                continue;
            pos = r-src+1;
            plen = pos-t;
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

    li n = getdir(path,cs->dirs,D_MODE_ONCE|D_CHDIR);
    if (n == -1)
        return -1;
    cs->tabs[cs->ctab].t = (size_t)n;

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

    li n = getdir(path,cs->dirs,D_CHDIR|D_MODE_ONCE);
    if (n == -1)
        return -1;
    cs->tabs[cs->ctab].t = n;
    return 0;
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

int
cmd_map(char *src, csas *cs)
{
    size_t pos = 0;
    char keys[BINDING_KEY_MAX],line[PATH_MAX],*r;

    while (isspace(src[pos]))
        pos++;

    r = get_path(keys,src+pos,' ',strlen(src+pos),BINDING_KEY_MAX,&CTAB);
    ret_errno(r==NULL,EINVAL,-1);
    pos = r-src+1;

    while (isspace(src[pos]))
        pos++;

    r = get_path(line,src+pos,' ',strlen(src+pos),PATH_MAX,&CTAB);
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
    int selected=-1,ws=cs->ctab;
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
                                ws = atoi(src+pos);
                                if (!(cs->tabs[ws].flags&T_EXISTS))
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
            char *r = get_path(path,src+pos,' ',s-pos,PATH_MAX,&TAB(ws));
            if (r == NULL)
                continue;
            pos = r-src+1;
            plen = pos-t;
        }
    }

    if (selected == -2) {
        selected = cs->tabs[ws].sel;
    } else if (selected == -3) {
        xdir *dir = &TAB(ws);
        int dfd,fd;
        dfd = open(dir->path,O_DIRECTORY);
        if (dfd == -1)
            return -1;
        if (dir->size == 0)
            return 0;
        xfile *file = &dir->files[dir->sel];
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
            if (selected < 0 ? 1 : (d->files[j].sel[ws]&(1<<selected))) {
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
    int selected=-1,ws=cs->ctab;
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
                        char *r = get_path(target,src+pos,' ',s-pos,PATH_MAX,&TAB(ws));
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
                                ws = atoi(src+pos);
                                if (!(cs->tabs[ws].flags&T_EXISTS))
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
            char *r = get_path(path,src+pos,' ',s-pos,PATH_MAX,&TAB(ws));
            if (r == NULL)
                continue;
            pos = r-src+1;
            plen = pos-t;
        }
    }

    if (selected == -2)
        selected = cs->tabs[ws].sel;
    if ((fd1 = open(target,O_DIRECTORY)) == -1)
        return -1;

    char *buffer = (act == 0 ? NULL : (char*)malloc(1<<16));
    off_t count=0,size=0;
    int ev=-1;

    if (selected == -3) {
        xdir *dir = &TAB(ws);
        if (dir->size == 0) {
            close(fd1);
            return 0;
        }
        if ((fd2 = open(dir->path,O_DIRECTORY)) == -1)
            goto END;

        xfile *file = &dir->files[dir->sel];
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
            if (selected < 0 ? 1 : (d->files[j].sel[ws]&(1<<selected))) {
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
            if (selected < 0 ? 1 : (d->files[j].sel[ws]&(1<<selected))) {
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
            r = get_path(path,src+pos,' ',s-pos,PATH_MAX,&CTAB);
            if (r == NULL)
                continue;
            pos = r-src+1;
            if (getdir(path,cs->dirs,flags) == -1)
                return -1;
        }
        while (isspace(src[pos]))
            pos++;
    }
    return 0;
}

int
cmd_rename(char *src, csas *cs)
{
    char name[2][NAME_MAX],*n;
    size_t pos=0;

    while (isspace(src[pos]))
        pos++;
    if (get_path(name[0],src+pos,'\0',strlen(src)-pos,NAME_MAX,&CTAB) == NULL)
        return -1;

    n = name[1];
    console_getline(&n,1,"rename ",(char*)name[0],cs);
    return rename((char*)name[0],(char*)name[1]);
}

int
cmd_open_with(char *src, csas *cs)
{
    char file[PATH_MAX],path[PATH_MAX],*n;
    size_t pos=0;

    while (isspace(src[pos]))
        pos++;
    if (get_path(file,src+pos,'\0',strlen(src)-pos,PATH_MAX,&CTAB) == NULL)
        return -1;
    if (access(file,R_OK) != 0)
        return -1;

    n = path;
    console_getline(&n,1,"open_with ",NULL,cs);
    return spawn(n,file,NULL,F_NORMAL|F_WAIT);
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
                            r = get_path(args[n],src+pos,' ',s-pos,PATH_MAX,&CTAB);
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
            r = get_path(path,src+pos,' ',s-pos,PATH_MAX,&CTAB);
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
