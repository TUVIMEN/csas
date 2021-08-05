#include "main.h"
#include "useful.h"
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
    static char ret[16];
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
        snprintf(ret,16,"%lu %c",size,suffixes[t]);
        return ret;
    }

    if (size <= 9) {
        while (r >= 100)
            r /= 10;
    } else  {
        while (r >= 10)
           r /= 10;
    }

    if (r != 0)
        snprintf(ret,16,"%lu.%lu %c",size,r,suffixes[t]);
    else
        snprintf(ret,16,"%lu %c",size,suffixes[t]);
    return ret;
}



















