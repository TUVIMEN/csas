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
#include "useful.h"
#include "functions.h"
#include "calc.h"
#include "csas.h"

static struct sigaction oldsighup;
static struct sigaction oldsigtstp;
extern fsig signatures[];
extern fext extensions[];
extern char FileOpener[];
extern char Editor[];
extern li IdleDelay;
extern const char *TTEMPLATE;
extern li OpenAllImages;

void
exiterr()
{
    endwin();
    perror(NULL);
    exit(1);
}

int
getinput(csas *cs)
{
    int ret;
    timeout(-1);
    while ((ret = getch()) == KEY_RESIZE && ret != OK)
        csas_resize(cs);
    if (ret == ERR)
        exit(1);
    timeout(IdleDelay);
    return ret;
}

int
getinput_wch(wint_t *wch, csas *cs)
{
    int ret;
    timeout(-1);
    while ((ret = get_wch(wch)) == KEY_RESIZE && ret != OK)
        csas_resize(cs);
    if (ret == ERR)
        exit(1);
    timeout(IdleDelay);
    return ret;
}

char *
delchar(char *src, const size_t pos, const size_t size)
{
  size_t s = size-1;
  for (size_t i = pos; i < s; i++)
    src[i] = src[i+1];
  src[s] = 0;
  return src;
}

wchar_t *
delwc(wchar_t *src, const size_t pos, const size_t size)
{
  size_t s = size-1;
  for (size_t i = pos; i < s; i++)
    src[i] = src[i+1];
  src[s] = 0;
  return src;
}

void
change_keys(wchar_t *dest, const char *src)
{
    size_t h,i;
    for (i = 0, h = 0; h < BINDING_KEY_MAX && src[i]; i++, h++) {
        if (src[i] == '<' && src[i+1] == 'C' && src[i+2] == '-' && src[i+3] && src[i+4] == '>') {
            i += 3;
            dest[h] = src[i]&0x1f;
            i += 1;
        } else if (memcmp(src+i,"<space>",7) == 0) {
            i += 6;
            dest[h] = btowc(' ');
        } else if (strncasecmp(src+i,"<esc>",5) == 0) {
            i += 4;
            dest[h] = KEY_EXIT;
        } else if (strncasecmp(src+i,"<left>",6) == 0) {
            i += 5;
            dest[h] = KEY_LEFT;
        } else if (strncasecmp(src+i,"<right>",7) == 0) {
            i += 6;
            dest[h] = KEY_RIGHT;
        } else if (strncasecmp(src+i,"<up>",4) == 0) {
            i += 3;
            dest[h] = KEY_UP;
        } else if (strncasecmp(src+i,"<down>",6) == 0) {
            i += 5;
            dest[h] = KEY_DOWN;
        } else if (strncasecmp(src+i,"<br>",4) == 0) {
            i += 3;
            dest[h] = KEY_ENTER;
        } else if (src[i] == '\\' && src[i+1]) {
            dest[h] = special_character(src[i++]);
        } else
            dest[h] = btowc(src[i]);
    }
    dest[h] = 0;
}

const char *
path_shrink(const char *path, size_t size, const size_t max_size)
{
    if (size <= 2)
        return path;
    
    static char ret[PATH_MAX];
    size_t x=0,pos,s=size;

    ret[x++] = path[0];
    ret[x++] = path[1];
    pos = 2;

    while (size > max_size) {
        while (path[pos] != '/' && path[pos]) {
            pos++;
            size--;
        }
        if (!path[pos])
            goto END;
        ret[x++] = path[pos];
        ret[x++] = path[pos+1];
        pos += 2;
        if (pos > s-1)
            goto END;
    }
    END: ;
    strcpy(ret+x,path+pos);
    return ret;
}

char *
size_shrink(size_t size)
{
    static char ret[23];
    const char suffixes[] = "\0KMGTPEZY";

    if (size == 0) {
        ret[0] = '0';
        ret[1] = 0;
        return ret;
    }

    uchar t = 0;
    size_t r = 0;
    while (t < 8 && size >= 1024) {
        r = size&0x3ff;
        size >>= 10;
        t++;
    }
    
    r *= 1000;
    r >>= 10;

    if (t == 0 || size >= 100) {
        snprintf(ret,23,"%lu%c",size,suffixes[t]);
        return ret;
    }

    if (size < 10) {
        snprintf(ret,23,"%lu.%02lu%c",size,r/10,suffixes[t]);
    } else {
        snprintf(ret,23,"%lu.%lu%c",size,r/100,suffixes[t]);
    }
    return ret;
}

char *
seek_end_of_dquote(char *src, size_t size)
{
    char *t1=src,*t2;
    size_t s,t;
    while ((t2 = memchr(t1,'"',size))) {
        s = t2-t1;
        t1 = t2;
        while (*(t1-1) == '\\')
            t1--;
        t = t2-t1;
        if (t%2 == 0)
            return t2+1;
        size -= s;
        t2++;
        t1 = t2;
    }

    return NULL;
}

int
addenv(char *dest, char *src, size_t *x, size_t *y, const size_t max, size_t size)
{
    if (src[*y] != '$' || src[*y+1] != '{')
        return -1;

    *y += 2;
    char *t = memchr(src+*y,'}',size-*y);
    size_t s = t-(src+*y);
    if (t == NULL || s > NAME_MAX)
        goto END;

    char name[NAME_MAX];
    memcpy(name,src+*y,s);
    name[s] = 0;
    char *t1 = getenv(name);
    if (t1 == NULL)
        goto END;

    *y += s;
    s = strlen(t1);
    if (s+*x > max)
        return 0;
    memcpy(dest+*x,t1,s);
    *x += s-1;
    return 0;

    END: ;
    *y -= 2;
    return -1;
}

static void
strrev(char *str, size_t s)
{
    register uchar c;
    for (size_t i=0,j=s-1; i < j; i++, j--) {
        c = str[i];
        str[i] = str[j];
        str[j] = c;
    }
}

void
ltoa(li num, char *result)
{
    size_t n = 0;
    if (num < 0) {
        result[n++] = '-';
        num *= -1;
    }

    do {
        result[n++] = (num%10)+48;
        num /= 10;
    } while (num != 0);

    if (n > 1) {
        if (*result == '-')
            strrev(result+1,n-1);
        else
            strrev(result,n);
    }
    result[n] = 0;
}

size_t
get_range(const char *src, ul *x, ul *y, size_t(handle_number)(const char*,ul*))
{
    *x = 0;
    *y = 0;
    size_t pos = 0;

    if (src[pos] == '-')
        goto NEXT_VALUE;
    if (src[pos] == '$') {
        *x = -1;
        pos++;
        goto END;
    }

    pos += handle_number(src+pos,x);
    if (src[pos] != '-')
        goto END;

    NEXT_VALUE: ;
    pos++;
    if (src[pos] == ' ') {
        *y = -1;
        goto END;
    }
    pos += handle_number(src+pos,y);

    END: ;
    return pos;
}

static int
addvar(char *dest, char *src, size_t *x, size_t *y, const size_t max, size_t size, flexarr *vars)
{
    if (src[*y] != '$' || src[*y+1] != '(')
        return -1;

    *y += 2;
    char *t=memchr(src+*y,')',size-*y);
    size_t s=t-(src+*y),i;
    if (t == NULL || s > VARS_NAME_MAX)
        goto END;

    char name[VARS_NAME_MAX];
    memcpy(name,src+*y,s);
    name[s] = 0;

    xvar *v = (xvar*)vars->v;
    uchar found=0;
    for (i = 0; i < vars->size; i++) {
        if (s == strlen(v[i].name) && memcmp(name,v[i].name,s) == 0) {
            found = 1;
            break;
        }
    }
    if (!found) {
        errno = EFAULT;
        goto END;
    }

    v = &v[i];
    *y += s;

    if (v->type == 'i' || v->type == 'I') {
        ltoa(*(li*)v->v,name);
        t = name;
    } else {
        t = v->v;
    }

    s = strlen(t);
    if (s+*x > max)
        return 0;
    memcpy(dest+*x,t,s);
    *x += s-1;
    return 0;

    END: ;
    *y -= 2;
    return -1;
}

static int
addcalc(char *dest, char *src, size_t *x, size_t *y, const size_t max, size_t size, flexarr *vars)
{
    if (src[*y] != '$' || src[*y+1] != '[')
        return -1;

    *y += 2;
    char *t=memchr(src+*y,']',size-*y);
    size_t s=t-(src+*y);
    if (t == NULL || s > NAME_MAX)
        goto END;

    char name[NAME_MAX];
    memcpy(name,src+*y,s);
    name[s] = 0;
    *y += s;

    li num;
    calc(name,&num,vars);
    ltoa(num,name);

    s = strlen(name);
    if (s+*x > max)
        return 0;
    memcpy(dest+*x,name,s);
    *x += s-1;
    return 0;

    END: ;
    *y -= 2;
    return -1;
}

int
handle_percent(char *dest, char *src, size_t *x, size_t *y, const size_t max, xdir *dir, const int tab, const uchar sel)
{
    if (src[*y] != '%')
        return -1;

    size_t posx=*x,posy=*y+1;
    int num=sel;
    xfile *files = dir->files;
    if (isdigit(src[posy])) {
        int n=atoi(src+posy++);
        while (isdigit(src[posy])) posy++;
        if (n < TABS)
            num = n;
    }
    
    switch (src[posy]) {
        case 'd':
           if (posx+dir->plen > max)
               return 0;
           memcpy(dest+posx,dir->path,dir->plen);
           posx += dir->plen-1;
           break;
        case 'f':
           if (dir->size == 0 || posx+files[dir->sel[tab]].nlen > max)
               return 0;
           memcpy(dest+posx,files[dir->sel[tab]].name,files[dir->sel[tab]].nlen);
           posx += files[dir->sel[tab]].nlen-1;
           break;
        case 's':
           if (dir->size == 0 || posx+dir->files[dir->sel[tab]].nlen > max)
               return 0;
           for (size_t i = 0; i < dir->size; i++) {
               if (num < 0 ? 1 : (files[i].sel[tab]&(1<<num))) {
                   if (files[i].nlen+posx >= LLINE_MAX)
                       break;
                   memcpy(dest+posx,files[i].name,files[i].nlen+1);
                   posx += files[i].nlen;
                   dest[posx++] = ' ';
               }
           }
           break;
        case '%':
            dest[posx] = src[posy];
            (*y)++;
           return 0;
       default:
           dest[posx] = src[--posy];
           return 0;
    }
    *x = posx;
    *y = posy;
    return 0;
}

char
special_character(const char c)
{
    char r;
    switch (c) {
        case '0': r = '\0'; break;
        case 'a': r = '\a'; break;
        case 'b': r = '\b'; break;
        case 't': r = '\t'; break;
        case 'n': r = '\n'; break;
        case 'v': r = '\v'; break;
        case 'f': r = '\f'; break;
        case 'r': r = '\r'; break;
        default: r = c;
    }
    return r;
}

char *
get_arg(char *dest, char *src, const char delim, size_t size, size_t *count, const size_t max, csas *cs)
{
    size_t pos=0,x=0;
    if (*src == '\'') {
        pos++;
        char *n = memchr(src+pos,'\'',size);
        if (n == NULL)
            return NULL;
        size_t s = n-(src+pos);
        if (s > max || pos+s+1 > size)
            return NULL;
        memcpy(dest+x,src+pos,s);
        x += s;
        dest[x] = 0;
        pos += s+1;
    } else {
        if (*src == '"')
            pos++;
        if (src[pos] == '~' && (src[pos+1] == '/' || src[pos+1] == 0)) {
            pos++;
            char *r = getenv("HOME");
            size_t s = strlen(r);
            memcpy(dest,r,s);
            x += s;
            dest[x++] = src[pos++];
        }

        for (; x < max && pos < size && (*src == '"' ? src[pos] != '"' : src[pos] != delim); x++, pos++) {
            if (src[pos] == '\\') {
                dest[x] = special_character(src[++pos]);
                continue;
            }
            if (src[pos] == '$') {
                if (addenv(dest,src,&x,&pos,max,size) == 0)
                    continue;
                if (addvar(dest,src,&x,&pos,max,size,cs->vars) == 0)
                    continue;
                if (addcalc(dest,src,&x,&pos,max,size,cs->vars) == 0)
                    continue;
            } else if (handle_percent(dest,src,&x,&pos,max,&CTAB(1),cs->ctab,cs->tabs[cs->ctab].sel) == 0)
                continue;
            dest[x] = src[pos];
        }
        dest[x] = 0;
        if (src[pos] != '"')
            pos--;
        if (x > max || pos > size)
            return NULL;
    }
    if (count)
        *count = x;
    return src+pos;
}

size_t
get_line(char *dest, char *src, size_t *count, size_t size)
{
    size_t pos = 0;
    while (pos < size && isspace(src[pos]))
        pos++;
    size_t x = 0;

    while (pos < size && x < LLINE_MAX) {
        if (src[pos] == '\\')
            pos += 2;

        if (src[pos] == '\n' || src[pos] == '\r' || src[pos] == ';') {
            dest[x] = '\0';
            break;
        }

        if (src[pos] == '\'' || src[pos] == '"') {
            dest[x++] = src[pos++];
            char *t = (src[pos-1] == '\'' ? seek_end_of_squote(src+pos,size-pos) :
                seek_end_of_dquote(src+pos,size-pos));
            if (t == NULL)
                goto END;
            size_t s = t-(src+pos);
            if (s > LLINE_MAX) {
                if (count)
                    *count = x;
                return pos;
            }
            memcpy(dest+x,src+pos,s);
            x += s;
            pos += s;
            continue;
        }

        if (src[pos] == '/') {
            if (src[pos+1] == '/') {
                pos += 2;
                while (pos < size && x < LLINE_MAX) {
                    if (src[pos] == '\\') {
                        if (++pos >= size)
                            break;
                        if (++pos >= size)
                            break;
                        continue;
                    }
                    if (src[pos] == '\n')
                        break;
                    pos++;
                }
                pos++;
            } else if (src[pos+1] == '*') {
                pos += 2;
                while (pos < size && (src[pos] != '*' || src[pos+1] != '/'))
                    pos++;
                pos += 3;
            }
            dest[x++] = src[pos++];
            continue;
        }

        dest[x++] = src[pos++];
    }
    
    END: ;
    if (x >= LLINE_MAX)
        x = LLINE_MAX-1;
    dest[x] = 0;
    if (count)
        *count = x;
    return pos;
}

int
config_load(const char *path, csas *cs)
{
    int fd;
    if ((fd = open(path,O_RDONLY)) == -1)
        return -1;
    
    struct stat statbuf;
    if (fstat(fd,&statbuf) == -1) {
        close(fd);
        return -1;
    }

    char *file = mmap(NULL,statbuf.st_size,PROT_READ,MAP_PRIVATE,fd,0);
    if (file == MAP_FAILED) {
        close(fd);
        return -1;
    }
    close(fd);

    char line[LLINE_MAX];
    int r;
    size_t s,count;

    for (size_t pos = 0, i = 0; pos < (size_t)statbuf.st_size; i++)
    {
        s = get_line(line,file+pos,&count,statbuf.st_size-pos);
        pos += s;
        r = command_run(line,count,cs);
        if (r != 0)
            fprintf(stderr,"%s: %s\n%lu:\t%s\n",path,strerror(errno),i+1,line);
        pos++;
    }

    return munmap(file,statbuf.st_size);
}

static size_t
parseargs(char *src, char **dest)
{
    if (src == NULL)
        return 0;
    size_t x = 0,s=strlen(src);
    dest[x++] = src;

    while (s) {
        if (*src == '\\') {
            delchar(src,0,s);
            src++;
            s -= 2;
            continue;
        }
        if (*src == '\'') {
            delchar(src,0,s);
            src++;
            s -= 2;
            while (s && *src != '\'') {
                src++;
                s--;
            }
            if (!s)
                return x;
            delchar(src,0,s);
            src++;
            s -= 2;
            continue;
        }
        if (*src == '"') {
            delchar(src,0,s);
            src++;
            s -= 2;
            while (s && *src != '"') {
                if (*src == '\\') {
                    delchar(src,0,s);
                    src++;
                    s -= 2;
                    continue;
                }
                src++;
                s--;
            }
            if (!s)
                return x;
            delchar(src,0,s);
            src++;
            s -= 2;
        }
        if (isspace(*src)) {
            while (s && !isspace(*src)) {
                src++;
                s--;
            }
            while (s && !isspace(*src)) {
                src++;
                s--;
            }
            *src++ = '\0';
            s--;
            if (!s)
                return x;
            uchar last = 0;
            if (*src == '$' && *(src+1) == '$') {
                *src++ = '\0';
                *src++ = '\0';
                s -= 2;
                last = 1;
            }
            dest[x++] = src;
            if (x == EXEC_ARGS_MAX)
                return 0;
            if (last)
                return x;
            continue;
        }
        src++;
        s--;
    }
    return x;
}

pid_t
xfork(uchar flags)
{
    int status;
    pid_t p = fork();
    struct sigaction act = {.sa_handler=SIG_DFL};

    if (p > 0) {
        sigaction(SIGHUP,&(struct sigaction){.sa_handler=SIG_IGN},&oldsighup);
	sigaction(SIGTSTP,&act,&oldsigtstp);
    }
    else if (p == 0) {
        if (flags&F_WAIT) {
            sigaction(SIGHUP,&act,NULL);
            sigaction(SIGINT,&act,NULL);
            sigaction(SIGQUIT,&act,NULL);
            sigaction(SIGTSTP,&act,NULL);
	} else {
            p = fork();
	    if (p > 0) {
		_exit(EXIT_SUCCESS);
            } else if (p == 0) {
                sigaction(SIGHUP,&act,NULL);
                sigaction(SIGINT,&act,NULL);
                sigaction(SIGQUIT,&act,NULL);
                sigaction(SIGTSTP,&act,NULL);
                setsid();
                return p;
	    }
	    _exit(EXIT_FAILURE);
        }
    }

    if (!(flags&F_WAIT))
	    waitpid(p,&status,0);
    return p;
}

int
spawnp(char *file, char *arg1, char *arg2, const uchar flags)
{
    if (!file || !*file)
        return -1;

    char *argv[EXEC_ARGS_MAX] = {0};

    if (!arg1 && arg2) {
        arg1 = arg2;
        arg2 = NULL;
    }

    size_t x = 0;
    if (flags&F_MULTI) {
        x = parseargs(file,argv);
        if (x == 0)
            return -1;
    }
    else
        argv[x++] = file;
    argv[x++] = arg1;
    argv[x++] = arg2;

    return spawn(argv,flags);
}

int
spawn(char **argv, const uchar flags)
{
    if (flags&F_NORMAL)
        endwin();

    pid_t pid = xfork(flags);

    if (pid == 0) {
        if (flags&F_SILENT) {
            int fd = open("/dev/null",O_WRONLY);
            dup2(fd,1);
            dup2(fd,2);
            close(fd);
        }
        execvp(argv[0],argv);
        _exit(1);
    } else {
        if (flags&F_WAIT)
            while (waitpid(pid,NULL,0) == -1);

    	sigaction(SIGHUP, &oldsighup, NULL);
    	sigaction(SIGTSTP, &oldsigtstp, NULL);

        if (flags&F_NORMAL) {
            if (flags&F_CONFIRM) {
                printf("\nPress ENTER to continue");
                fflush(stdout);
                while (getch() != '\n');
            }
            refresh();
        }
    }

    return 0;
}

static int
openimages(char *name, csas *cs, const uchar flags)
{
    xdir *d = &CTAB(1);
    xfile *files = d->files;
    flexarr *matched = flexarr_init(sizeof(char*),8*((d->size>>8)+1));
    *(char**)flexarr_inc(matched) = OP_IMAGE;
    size_t namel=strlen(name),spos=0;
    char *ext;
    static char *img_ext[] = {
        "gif","jpeg","jpg","png","raw",
        "tiff","bmp","tga","gvs","webp",
        NULL
    };

    for (size_t i = 0; i < d->size; i++) {
        if ((files[i].mode&S_IFMT) != S_IFREG)
            continue;
        if (namel == files[i].nlen && memcmp(name,files[i].name,namel) == 0) {
            spos = matched->size;
            *(char**)flexarr_inc(matched) = files[i].name;
            continue;
        }
        ext = memrchr(files[i].name,'.',files[i].nlen);
        if (ext == NULL || !*(++ext))
            continue;
        for (size_t j = 0; img_ext[j]; j++) {
            if (strcasecmp(ext,img_ext[j]) == 0) {
                *(char**)flexarr_inc(matched) = files[i].name;
                break;
            }
        }
    }

    char tm1[3]="-n",tm2[32];
    ltoa(spos,tm2);
    *(char**)flexarr_inc(matched) = tm1;
    *(char**)flexarr_inc(matched) = tm2;
    *(char**)flexarr_inc(matched) = NULL;

    int r = spawn((char**)matched->v,flags);
    flexarr_free(matched);
    return r;
}

uchar
isbinfile(char *src, size_t size)
{
    for (register size_t i = 0; i < size; i++)
        if (src[i]&0x80)
            return 1;
    return 0;
}

int
file_run(char *path, csas* cs)
{
    if (*FileOpener)
        return spawnp(FileOpener,path,NULL,F_NORMAL|F_WAIT);
    struct stat sfile;
    if (stat(path,&sfile) == -1)
        return -1;
    ret_errno(!(sfile.st_mode&S_IRUSR),EACCES,-1);
    if (sfile.st_size == 0)
        return spawnp(Editor,path,NULL,F_NORMAL|F_WAIT);

    int fd;
    if ((fd = open(path,O_RDONLY)) == -1)
        return -1;
    size_t sigl;
    char sig[SIG_MAX];
    sigl = read(fd,sig,SIG_MAX);
    uchar bin = isbinfile(sig,sigl);

    for (register int i = 0; signatures[i].sig != NULL; i++) {
        if (!(((signatures[i].flags&F_BIN)==F_BIN)^bin)) {
            lseek(fd,signatures[i].offset,signatures[i].whence);
            if (signatures[i].len > SIG_MAX)
                continue;
            sigl = read(fd,sig,signatures[i].len);
            if (sigl == signatures[i].len && memcmp(sig,signatures[i].sig,sigl) == 0) {
                close(fd);
                if (OpenAllImages && strcmp(signatures[i].path,OP_IMAGE) == 0 && strrchr(path,'/') == NULL)
                    return openimages(path,cs,signatures[i].flags);
                else
                    return spawnp(signatures[i].path,path,NULL,signatures[i].flags);
            }
        }
    }

    close(fd);
    if (!bin)
        return spawnp(Editor,path,NULL,F_NORMAL|F_WAIT);
    return 0;
}

int
file_rm(const int fd, const char *name)
{
    struct stat statbuf;
    fstatat(fd,name,&statbuf,0);

    if ((statbuf.st_mode&S_IFMT) != S_IFDIR)
        return unlinkat(fd,name,0);

    int t;
    if ((t = openat(fd,name,O_DIRECTORY)) == -1)
        goto END;

    DIR *d = fdopendir(t);
    struct dirent *dir;
    if (!d)
        goto END;

    while ((dir = readdir(d)))
        if (!(dir->d_name[0] == '.' && (dir->d_name[1] == '\0' || (dir->d_name[1] == '.' && dir->d_name[2] == '\0'))))
            file_rm(t,dir->d_name);
    closedir(d);

    END: ;
    close(t);
    return unlinkat(fd,name,AT_REMOVEDIR);
}

int
file_cp(const int fd1, const int fd2, const char *name, char *buffer, const mode_t flags)
{
    struct stat statbuf;
    int fd3, fd4;
    ssize_t bytesread;

    char t[NAME_MAX];
    if (fstatat(fd2,name,&statbuf,0) != 0)
        return -1;
    strcpy(t,name);
    size_t num = 0;

    if (!(flags&M_MERGE) || (statbuf.st_mode&S_IFMT) != S_IFDIR) {
        if (flags&M_CHNAME) {
            while (faccessat(fd1,t,F_OK,0) == 0) {
                if (snprintf(t,NAME_MAX-1,"%s_%lu",name,num) == NAME_MAX)
                    return 0;
                num++;
            }
        } else if (flags&M_DCPY) {
            if (faccessat(fd1,t,F_OK,0) != 0)
                return -1;
        } else if (flags&M_REPLACE)
            file_rm(fd1,t);
    }

    if ((statbuf.st_mode& S_IFMT) == S_IFDIR) {
        if ((fd3 = openat(fd2,name,O_DIRECTORY)) == -1)
            return -1;

        DIR *d = fdopendir(fd3);
        struct dirent *dir;

        if (!d) {
            int e = errno;
            close(fd3);
            errno = e;
            return -1;
        }
            
        if (mkdirat(fd1,t,statbuf.st_mode) != 0
            || (fd4 = openat(fd1,t,O_DIRECTORY)) == -1) {
            int e = errno;
            closedir(d);
            close(fd3);
            errno = e;
            return -1;
        }

        while ((dir = readdir(d)))
            if (!(dir->d_name[0] == '.' && (dir->d_name[1] == '\0' || (dir->d_name[1] == '.' && dir->d_name[2] == '\0'))))
                file_cp(fd4,fd3,dir->d_name,buffer,flags);
        close(fd4);
        closedir(d);
        close(fd3);
        return 0;
    }

    if ((fd3 = openat(fd2,name,O_RDONLY)) == -1)
        return -1;

    if ((fd4 = openat(fd1,t,O_WRONLY|O_CREAT,statbuf.st_mode)) == -1) {
        int e = errno;
        close(fd3);
        errno = e;
        return -1;
    }
    while ((bytesread = read(fd3,buffer,BUFFER_MAX)) > 0)
        if (write(fd4,buffer,bytesread) == -1) {
            int e = errno;
            close(fd4);
            close(fd3);
            errno = e;
            return -1;
        }

    close(fd4);
    close(fd3);
    return 0;
}

int
file_mv(const int fd1, const int fd2, const char *name, char *buffer, const mode_t flags)
{
    struct stat statbuf,dir;
    int fd3, fd4;
    ssize_t bytesread;
    if (fstatat(fd2,name,&statbuf,0) != 0)
        return -1;
    if (fstat(fd1,&dir) != 0)
        return -1;
    char t[NAME_MAX];
    strcpy(t,name);
    size_t num = 0;

    if (!(flags&M_MERGE) || (statbuf.st_mode&S_IFMT) != S_IFDIR) {
        if (flags&M_CHNAME) {
            while (faccessat(fd1,t,F_OK,0) == 0) {
                if (snprintf(t,NAME_MAX-1,"%s_%lu",name,num) == NAME_MAX)
                    return 0;
                num++;
            }
        } else if (flags&M_DCPY) {
            if (faccessat(fd1,t,F_OK,0) != 0)
                return -1;
        } else if (flags&M_REPLACE)
            file_rm(fd1,t);
    }

    if (dir.st_dev == statbuf.st_dev)
        return renameat(fd2,name,fd1,t);

    if ((statbuf.st_mode& S_IFMT) == S_IFDIR) {
        if ((fd3 = openat(fd2,name,O_DIRECTORY)) == -1)
            return -1;

        DIR *d = fdopendir(fd3);
        struct dirent *dir;

        if (!d) {
            int e = errno;
            close(fd3);
            errno = e;
            return -1;
        }
            
        if (mkdirat(fd1,t,statbuf.st_mode) != 0
            || (fd4 = openat(fd1,t,O_DIRECTORY)) == -1) {
            int e = errno;
            closedir(d);
            close(fd3);
            errno = e;
            return -1;
        }

        while ((dir = readdir(d))) {
            if (dir->d_name[0] == '.' && (dir->d_name[1] == '\0' || (dir->d_name[1] == '.' && dir->d_name[2] == '\0')))
                continue;
            file_mv(fd4,fd3,dir->d_name,buffer,flags);
            //unlinkat(fd4,name,AT_REMOVEDIR);
        }
        close(fd4);
        closedir(d);
        close(fd3);
        return unlinkat(fd2,name,AT_REMOVEDIR);
    }

    if ((fd3 = openat(fd2,name,O_RDONLY)) == -1)
        return -1;

    if ((fd4 = openat(fd1,t,O_WRONLY|O_CREAT,statbuf.st_mode)) == -1) {
        int e = errno;
        close(fd3);
        errno = e;
        return -1;
    }
    while ((bytesread = read(fd3,buffer,BUFFER_MAX)) > 0)
        if (write(fd4,buffer,bytesread) == -1) {
            int e = errno;
            close(fd4);
            close(fd3);
            errno = e;
            return -1;
        }

    close(fd4);
    close(fd3);
    return unlinkat(fd2,name,0);
}

int
get_dirsize(const int fd, off_t *count, off_t *size, const uchar flags)
{
    ret_errno((count==NULL&&size==NULL)||flags==0,EINVAL,-1);
    DIR *d = fdopendir(fd);
    if (d == NULL)
        return -1;

    struct dirent *dir;
    int tfd;
    struct stat st;
    size_t c=0,s=0;

    while ((dir = readdir(d))) {
        if (dir->d_name[0] == '.' && (dir->d_name[1] == '\0' || (dir->d_name[1] == '.' && dir->d_name[2] == '\0')))
            continue;

        if (flags&D_R && dir->d_type == DT_DIR) {
            if (faccessat(fd,dir->d_name,R_OK,0) != 0) {
                closedir(d);
                return -1;
            }
            tfd = openat(fd,dir->d_name,O_DIRECTORY);
            if (tfd == -1) {
                closedir(d);
                return -1;
            }
            get_dirsize(tfd,count,size,flags);
            close(tfd);
        }
        if (flags&D_C)
            c++;
        if (flags&D_S && dir->d_type == DT_REG) {
            if (fstatat(fd,dir->d_name,&st,AT_SYMLINK_NOFOLLOW) == 0)
                s += st.st_size;
        }
    }

    if (flags&D_C)
        *count += c;
    if (flags&D_S)
        *size += s;
    return closedir(d);
}

char *
mkpath(const char *dir, const char *name)
{
    static char path[PATH_MAX];
    size_t dlen = strlen(dir);
    memcpy(path,dir,dlen+1);
    if (path[0] == '/' && path[1] != '\0')
        path[dlen++] = '/';
    strcpy(path+dlen,name);
    return path;
}

char *
strtoshellpath(char *src)
{
    size_t i,size=strlen(src);
    for (i = 0; i < size && size < PATH_MAX; i++) {
        if(src[i] == '\\' || src[i] == '\"' || src[i] == '\'' || src[i] == ' ' || src[i] == '(' || src[i] == ')' || src[i] == '[' || src[i] == ']' || src[i] == '{' || src[i] == '}' || src[i] == '|' || src[i] == '&' || src[i] == ';' || src[i] == '?' || src[i] == '~' || src[i] == '*' || src[i] == '!' || src[i] == '$' || src[i] == '#') {
            for (size_t j = size++; j > i; j--)
                src[j] = src[j-1];
            src[i] = '\\';
            i++;
        }
    }
    src[size] = 0;
    return src;
}

int
ttoa(const time_t *time, char *result)
{
    struct tm t;
    localtime_r(time, &t);
    return sprintf(result,"%d-%02d-%02d %02d:%02d",t.tm_year+1900,t.tm_mon+1,t.tm_mday,t.tm_hour,t.tm_min);
}

char *
lsperms(const mode_t mode)
{
    const char *const rwx[] = {"---", "--x", "-w-", "-wx", "r--", "r-x", "rw-", "rwx"};
    static char bits[11];

    switch (mode & S_IFMT) {
        case S_IFREG:
            bits[0] = '-';
            break;
        case S_IFDIR:
            bits[0] = 'd';
            break;
        case S_IFLNK:
            bits[0] = 'l';
            break;
        case S_IFSOCK:
            bits[0] = 's';
            break;
        case S_IFIFO:
            bits[0] = 'p';
            break;
        case S_IFBLK:
            bits[0] = 'b';
            break;
        case S_IFCHR:
            bits[0] = 'c';
            break;
        default:
            bits[0] = '?';
            break;
    }

    memcpy(bits+1,rwx[(mode>>6)&7],4);
    memcpy(bits+4,rwx[(mode>>3)&7],4);
    memcpy(bits+7,rwx[mode&7],4);

    return bits;
}

uchar
get_extension_group(const char *name)
{
    register char *ret = memrchr(name,'.',strlen(name)-1);

    if (ret == NULL)
        return 0;
    ret++;

    for (register size_t j = 0; extensions[j].group != 0; j++)
        if (strcasecmp(ret,extensions[j].name) == 0)
            return extensions[j].group;

    return 0;
}

int
alias_run(char *src, size_t size, csas *cs)
{
    if (size == 0)
        return -1;

    char line[LLINE_MAX];
    size_t s,count;
    for (size_t i = 0; i < size; i++) {
        s = get_line(line,src+i,&count,size-i);
        i += s;
        command_run(line,count,cs);
        i++;
    }
    return 0;
}

int
splitargs(char *src, size_t size, csas *cs)
{
    char *r;
    int argc = 0;
    size_t count;
    for (size_t i = 0; i < size; i++) {
        while (isspace(src[i]) && i < size)
            i++;
        if ((int)cs->args->size == argc)
            *((char**)flexarr_inc(cs->args)) = malloc(ARG_MAX);
        r = get_arg(((char**)cs->args->v)[argc],src+i,' ',size-i,&count,ARG_MAX-1,cs);
        if (r == NULL)
            return argc;
        ((char**)cs->args->v)[argc][count] = 0;
        i = r-src;
        argc++;
    }
    return argc;
}
