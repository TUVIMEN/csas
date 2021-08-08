#include "main.h"
#include "useful.h"
#include "load.h"
#include "draw.h"
#include "console.h"
#include "functions.h"

uint
update_event(csas *cs)
{
    int event;
    size_t i,j,size = cs->bindings->size;
    xbind *b = BINDINGS;
    uint passedl=size,tmp_passedl,
         *passed = malloc(size*sizeof(uint));

    for (i = 0; i < size; i++)
        passed[i] = i;
    i = 0;
    while (true) {
        while ((event = getinput(cs)) == -1);
        if (event == 27)
            goto EXIT;
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
            return ((int (*)(char*,csas*))functions[i].func)(src+pos,cs);
        }
    }

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
        case MOVE_DOWN: t -= value; break;
    }

    if (t > dir->size-1)
        return;
    dir->sel = t;
}

int
cmd_move(char *src, csas *cs)
{
    uchar flags = MOVE_UP;
    size_t value=1,pos=0;

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
                    case 's': flags = MOVE_SET; break;
                }
            } while (src[pos] && !isspace(src[pos]));
        } else if (src[pos] == '$') {
            value = dir->size-1;
        } else if (isdigit(src[pos])) {
            value = (size_t)atoll(src+pos);
            while (src[pos] && isdigit(src[pos]))
                pos++;
        }
        pos++;
    }

    move_d(dir,value,flags);
    return 0;
}

int
cmd_console(char *src, csas *cs)
{
    size_t pos = 0,s=strlen(src);
    char add[PATH_MAX],first[PATH_MAX] = ":",*r;
    add[0] = '\0';

    while (src[pos]) {
        while (src[pos] && isspace(src[pos]))
            pos++;
        if (src[pos] == '-' && src[pos+1] == 'f') {
            pos += 2;
            while (src[pos] && isspace(src[pos]))
                pos++;
            r = get_path(first,src+pos,s-pos,&CTAB);
            if (r == NULL)
                continue;
            pos = r-src+1;
            endwin();
            printf("%s\n",src+pos);
            refresh();
        } else {
            r = get_path(add,src+pos,s-pos,&CTAB);
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
    if (get_path(path,src+pos,size-pos,dir) == NULL) {
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
    if (get_path(path,src+pos,size-pos,dir) == NULL) {
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
    if (get_path(path,src+pos,size-pos,dir) == NULL) {
        errno = ENOENT;
        return -1;
    }

    struct stat statbuf;
    if (stat(path,&statbuf) != 0)
        return -1;
    if ((statbuf.st_mode&S_IFMT) != S_IFDIR) {
        errno = ENOTDIR;
        return -1;
    }
    return get_dir(path,cs->dirs,&cs->tabs[cs->ctab].t,D_CHDIR|D_MODE_ONCE);
}
