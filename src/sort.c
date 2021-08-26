#include "main.h"
#include "sort.h"

static int
cmp_type(const xfile *f1, const xfile *f2)
    {return (f1->mode&S_IFMT) < (f2->mode&S_IFMT);}
static int
cmp_size(const xfile *f1, const xfile *f2)
    {return f1->size < f2->size;}
static int
cmp_name(const xfile *f1, const xfile *f2)
    {return strcmp(f1->name,f2->name);}
static int
cmp_cname(const xfile *f1, const xfile *f2)
    {return strcasecmp(f1->name,f2->name);}
static int
cmp_mtime(const xfile *f1, const xfile *f2)
    {return f1->mtime < f2->mtime;}

static int
cmp_rtype(const xfile *f1, const xfile *f2)
    {return (f1->mode&S_IFMT) > (f2->mode^S_IFMT);}
static int
cmp_rsize(const xfile *f1, const xfile *f2)
    {return f1->size > f2->size;}
static int
cmp_rname(const xfile *f1, const xfile *f2)
    {return strcmp(f2->name,f1->name);}
static int
cmp_rcname(const xfile *f1, const xfile *f2)
    {return !strcasecmp(f2->name,f1->name);}
static int
cmp_rmtime(const xfile *f1, const xfile *f2)
    {return f1->mtime < f2->mtime;}

int (*sort_arr[])(const xfile*,const xfile*) = {
    [SORT_TYPE]=cmp_type,
    [SORT_SIZE]=cmp_size,
    [SORT_NAME]=cmp_name,
    [SORT_CNAME]=cmp_cname,
    [SORT_MTIME]=cmp_mtime,

    [SORT_TYPE|SORT_REVERSE]=cmp_rtype,
    [SORT_SIZE|SORT_REVERSE]=cmp_rsize,
    [SORT_NAME|SORT_REVERSE]=cmp_rname,
    [SORT_CNAME|SORT_REVERSE]=cmp_rcname,
    [SORT_MTIME|SORT_REVERSE]=cmp_rmtime,
};

static int
cmp_dir_distinction(const xfile *f1, const xfile *f2, void *flags)
{
    uchar t1=0,t2=0,fl=(uchar)(size_t)flags;
    if (fl&SORT_DIR_DISTINCTION) {
        t1 |= (f1->mode&S_IFMT) == S_IFDIR;
        t2 |= (f2->mode&S_IFMT) == S_IFDIR;
    }
    if (fl&SORT_LDIR_DISTINCTION) {
        t1 |= (f1->flags&SLINK_TO_DIR) == SLINK_TO_DIR;
        t2 |= (f2->flags&SLINK_TO_DIR) == SLINK_TO_DIR;
    }

    if (!(t1^t2))
        return sort_arr[fl&SORT_MTR](f1,f2);
    t2 = (fl&SORT_REVERSE_DIR_DISTINCTIONS)==SORT_REVERSE_DIR_DISTINCTIONS;
    if (t1^t2)
        return 0;
    return 1;
}

void
xfile_sort(xfile *f, const size_t size, uchar flags)
{
    if (flags == SORT_NONE)
        return;
    if (flags == SORT_REVERSE) {
        xfile t;
        size_t i,j;
        for (i = 0, j = size-1; i < j; i++, j--) {
            t = f[i];
            f[i] = f[j];
            f[j] = t;
        }
        return;
    }
    if (flags&(SORT_DIR_DISTINCTION|SORT_LDIR_DISTINCTION))
        qsort_r(f,size,sizeof(xfile),(int(*)(const void*,const void*,void*))cmp_dir_distinction,(void*)(size_t)flags);
    else
        qsort(f,size,sizeof(xfile),(int(*)(const void*,const void*))sort_arr[flags&SORT_MTR]);
}

