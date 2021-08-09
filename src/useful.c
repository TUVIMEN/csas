#include "main.h"
#include "useful.h"
#include "functions.h"
#include "csas.h"

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
addenv(char *dest, char *src, size_t *x, size_t *y, size_t size)
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
    memcpy(dest+*x,t1,s);
    *x += s-1;
    return 0;

    END: ;
    *y -= 2;
    return -1;
}

int
handle_percent(char *dest, char *src, size_t *x, size_t *y, xdir *dir)
{
    if (src[*y] != '%')
        return -1;
    
    switch (src[++(*y)]) {
        case 'c':
           (*y)++;
           if (*x+dir->plen > PATH_MAX)
               return 0;
           memcpy(dest+*x,dir->path,dir->plen);
           *x += dir->plen-1;
           break;
        case 's':
           (*y)++;
           if (dir->size == 0 || *x+dir->files[dir->sel].nlen > PATH_MAX)
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
get_path(char *dest, char *src, size_t size, xdir *dir)
{
    size_t pos=0,x=0;
    if (*src == '\'') {
        pos++;
        char *n = memchr(src+pos,'\'',size);
        if (n == NULL)
            return NULL;
        size_t s = n-(src+pos);
        if (x+s > PATH_MAX || pos+s+1 > size)
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

        for (; x < PATH_MAX && pos < size && (*src == '"' ? src[pos] != '"' : !isspace(src[pos])); x++, pos++) {
            if (src[pos] == '\\') {
                dest[x] = special_character(src[++pos]);
                continue;
            }
            if (addenv(dest,src,&x,&pos,size) == 0)
                continue;
            if (handle_percent(dest,src,&x,&pos,dir) == 0)
                continue;
            dest[x] = src[pos];
        }
        dest[x] = 0;
        pos--;
        if (src[pos] == '"')
            pos++;
        if (x > PATH_MAX || pos > size)
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

        if (src[*pos] == '\n' || src[*pos] == ';') {
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
            *pos += s+1;
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
            fprintf(stderr,"%s: %s\n%lu:\t%s\n",path,strerror(errno),i,line);
        pos++;
    }

    return munmap(file,statbuf.st_size);
}
