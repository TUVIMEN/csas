#include "main.h"
#include "useful.h"
#include "functions.h"
#include "csas.h"

static struct sigaction oldsighup;
static struct sigaction oldsigtstp;
extern fsig signatures[];
extern char *FileOpener;
extern char *Editor;

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
    while ((ret = getch()) == KEY_RESIZE)
        csas_resize(cs);
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

void
change_keys(wchar_t *dest, const char *src)
{
    size_t i, h;
    for (i = 0; src[i] && i < BINDING_KEY_MAX; i++)
        dest[i] = btowc(src[i]);
    dest[i] = 0;
    return;
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
            dest[h++] = wctob(src[i]);
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
    const char suffixes[] = "BKMGTPEZY";

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

    if (t == 0 || size >= 100) {
        snprintf(ret,23,"%lu %c",size,suffixes[t]);
        return ret;
    }

    size_t c = size <= 9 ? 100 : 10;
    while (r >= c)
        r /= 10;

    if (r != 0)
        snprintf(ret,23,"%lu.%lu %c",size,r,suffixes[t]);
    else
        snprintf(ret,23,"%lu %c",size,suffixes[t]);
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

int
handle_percent(char *dest, char *src, size_t *x, size_t *y, const size_t max, xdir *dir)
{
    if (src[*y] != '%')
        return -1;
    
    switch (src[++(*y)]) {
        case 'c':
           (*y)++;
           if (*x+dir->plen > max)
               return 0;
           memcpy(dest+*x,dir->path,dir->plen);
           *x += dir->plen-1;
           break;
        case 's':
           (*y)++;
           if (dir->size == 0 || *x+dir->files[dir->sel].nlen > max)
               return 0;
           memcpy(dest+*x,dir->files[dir->sel].name,dir->files[dir->sel].nlen);
           *x += dir->files[dir->sel].nlen-1;
           break;
       default:
           dest[*x] = src[*y];
    }
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
get_path(char *dest, char *src, const char delim, size_t size, const size_t max, xdir *dir)
{
    size_t pos=0,x=0;
    if (*src == '\'') {
        pos++;
        char *n = memchr(src+pos,'\'',size);
        if (n == NULL)
            return NULL;
        size_t s = n-(src+pos);
        if (x+s > max || pos+s+1 > size)
            return NULL;
        memcpy(dest+x,src+pos,s);
        x += s;
        dest[x] = 0;
        pos += s+1;
    } else {
        if (*src == '"') {
            pos++;
        } else if (*src == '~' && !isalnum(*(src+1))) {
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
            if (addenv(dest,src,&x,&pos,max,size) == 0)
                continue;
            if (handle_percent(dest,src,&x,&pos,max,dir) == 0)
                continue;
            dest[x] = src[pos];
        }
        dest[x] = 0;
        if (src[pos] == '"')
            pos++;
        pos--;
        if (x > max || pos > size)
            return NULL;
    }
    return src+pos;
}

void
get_line(char *dest, char *src, size_t *pos, size_t size)
{
    while (*pos < size && isspace(src[*pos]))
        (*pos)++;
    size_t x = 0;

    while (*pos < size && x < LLINE_MAX) {
        if (src[*pos] == '\\')
            *pos += 2;

        if (src[*pos] == '\n' || src[*pos] == '\r' || src[*pos] == ';') {
            dest[x] = '\0';
            break;
        }

        if (src[*pos] == '\'' || src[*pos] == '"') {
            dest[x++] = src[(*pos)++];
            char *t = (src[*pos-1] == '\'' ? seek_end_of_squote(src+*pos,size-*pos) :
                seek_end_of_dquote(src+*pos,size-*pos));
            if (t == NULL)
                goto END;
            size_t s = t-(src+*pos);
            memcpy(dest+x,src+*pos,s);
            x += s;
            *pos += s;
            continue;
        }

        if (src[*pos] == '/') {
            if (src[*pos+1] == '/') {
                (*pos) += 2;
                while (*pos < size && x < LLINE_MAX) {
                    if (src[*pos] == '\\') {
                        if (++(*pos) >= size)
                            break;
                        if (++(*pos) >= size)
                            break;
                        continue;
                    }
                    if (src[*pos] == '\n')
                        break;
                    (*pos)++;
                }
                (*pos)++;
            } else if (src[*pos+1] == '*') {
                *pos += 2;
                while (*pos < size && (src[*pos] != '*' || src[*pos+1] != '/'))
                    (*pos)++;
                *pos += 3;
            }
            dest[x++] = src[(*pos)++];
            continue;
        }

        dest[x++] = src[(*pos)++];
    }
    
    END: ;
    if (x >= LLINE_MAX)
        x = LLINE_MAX-1;
    dest[x] = 0;
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

    for (size_t pos = 0, i = 0; file[pos]; i++)
    {
        get_line(line,file,&pos,statbuf.st_size);
        r = command_run(line,cs);
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
        if (s == '\\') {
            delchar(src,0,s);
            src++;
            s -= 2;
            continue;
        }
        if (s == '\'') {
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
spawn(char *file, char *arg1, char *arg2, const uchar flags)
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
        execvp(file,argv);
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

static uchar
isbinfile(char *src, size_t size)
{
    for (register size_t i = 0; i < size; i++)
        if ((src[i] < 0x07 || src[i] > 0xd) && (src[i] < 0x20 || src[i] > 0x7e))
            return 1;
    return 0;
}

int
file_run(char *path)
{
    if (*FileOpener)
        return spawn(FileOpener,path,NULL,F_NORMAL|F_WAIT);
    struct stat sfile;
    if (stat(path,&sfile) == -1)
        return -1;
    ret_errno(!(sfile.st_mode&S_IRUSR),EACCES,-1);
    if (sfile.st_size == 0)
        return spawn(Editor,path,NULL,F_NORMAL|F_WAIT);

    int fd;
    if ((fd = open(path,O_RDONLY)) == -1)
        return -1;
    size_t sigl;
    char sig[SIG_MAX];
    sigl = read(fd,sig,SIG_MAX);
    uchar bin = isbinfile(sig,sigl);

    for (register int i = 0; signatures[i].sig != NULL; i++) {
        if ((signatures[i].flags&F_BIN) ? bin : !bin) {
            lseek(fd,signatures[i].offset,signatures[i].whence);
            if (signatures[i].len > SIG_MAX)
                continue;
            sigl = read(fd,sig,signatures[i].len);
            if (sigl == signatures[i].len && memcmp(sig,signatures[i].sig,sigl) == 0) {
                close(fd);
                return spawn(signatures[i].path,path,NULL,signatures[i].flags);
            }
        }
    }

    close(fd);
    if (!bin)
        return spawn(Editor,path,NULL,F_NORMAL|F_WAIT);
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
            if (faccessat(fd,dir->d_name,R_OK,0) != 0)
                return -1;
            tfd = openat(fd,dir->d_name,O_DIRECTORY);
            if (tfd == -1)
                return -1;
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

