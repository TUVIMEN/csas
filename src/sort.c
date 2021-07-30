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

#ifdef __SORT_ELEMENTS_ENABLE__

#include "sort.h"
#include "useful.h"

extern li *s_BetterFiles;

static bool
ismatching(const uchar src)
{
    for (register int i = 0; s_BetterFiles[i] != 0; i++)
        if (src == s_BetterFiles[i])
            return 1;
    return 0;
}

static int
cmp_type(struct xfile *el1, struct xfile *el2)
    {return el1->type > el2->type;}
#ifdef __FILE_SIZE_ENABLE__
static int
cmp_size(struct xfile *el1, struct xfile *el2)
    {return el1->size < el2->size;}
#endif
static int
cmp_name(struct xfile *el1, struct xfile *el2)
    {return strcasecmp(el1->name,el2->name);}
static int
cmp_lname(struct xfile *el1, struct xfile *el2)
    {return strcmp(el1->name,el2->name);}
#ifdef __MTIME_ENABLE__
static int
cmp_mtime(struct xfile *el1, struct xfile *el2)
    {return el1->mtim > el2->mtim;}
#endif
#ifdef __ATIME_ENABLE__
static int
cmp_atime(struct xfile *el1, struct xfile *el2)
    {return el1->atim > el2->atim;}
#endif
#ifdef __CTIME_ENABLE__
static int
cmp_ctime(struct xfile *el1, struct xfile *el2)
    {return el1->ctim > el2->ctim;}
#endif
#ifdef __FILE_GROUPS_ENABLE__
static int
cmp_gid(struct xfile *el1, struct xfile *el2)
    {return el1->gr > el2->gr;}
#endif
#ifdef __FILE_GROUPS_ENABLE__
static int
cmp_uid(struct xfile *el1, struct xfile *el2)
    {return el1->pw > el2->pw;}
#endif
static int
cmp_zname(struct xfile *el1, struct xfile *el2)
{
    char *n1 = el1->name, *n2 = el2->name;
    while (*n1 == '0') n1++;
    while (*n2 == '0') n2++;
    return strverscasecmp(n1,n2);
}
static int
cmp_lzname(struct xfile *el1, struct xfile *el2)
    { return strverscmp(el1->name,el2->name); }

int
(*mas[])(struct xfile*, struct xfile*) = {
    [0]=cmp_type,
    #ifdef __FILE_SIZE_ENABLE__
    [1]=cmp_size,
    #endif
    [2]=cmp_name,[3]=cmp_lname,[9]=cmp_zname,[10]=cmp_lzname,
    #ifdef __MTIME_ENABLE__
    [4]=cmp_mtime,
    #endif
    #ifdef __ATIME_ENABLE__
    [5]=cmp_atime,
    #endif
    #ifdef __CTIME_ENABLE__
    [6]=cmp_ctime,
    #endif
    #ifdef __FILE_GROUPS_ENABLE__
    [7]=cmp_gid,
    #endif
    #ifdef __FILE_GROUPS_ENABLE__
    [8]=cmp_uid,
    #endif
};

static int
comp(const void *el1, const void *el2, void *flag)
{
    if (*(ull*)flag & SORT_BETTER_FILES)
    {
        register bool g1 = 0, g2 = 0;

        g1 = ismatching(((struct xfile*)el1)->type);
        g2 = ismatching(((struct xfile*)el2)->type);

        if (!g1 && !g2) goto RESULT;
        if (!g1 && g2) return 1;
        if (g1 && !g2) return 0;
    }

    RESULT: ;
    return (*mas[(*(ull*)flag&SORT_IF)-1])((struct xfile*)el1,(struct xfile*)el2);
}

static size_t
find_border(const struct xfile *xf, size_t size)
{
    register size_t ret = 0;
    for (register size_t i = 0; i < size && ret == 0; i++)
        ret = i*!ismatching(xf[i].type);
    return ret;
}

void
sort_xfile(struct xfile *xf, const size_t size, ull flag)
{
    if ((flag&SORT_IF) != SORT_NONE)
        qsort_r(xf,size,sizeof(struct xfile),comp,&flag);

    if (!(flag & SORT_REVERSE))
        return;
    
    register size_t border = 0, i, j;
    struct xfile temp;

    if (flag & SORT_BETTER_FILES) {
        border = find_border(xf,size);
        if (border) {
            for (i = 0, j = border-1; i < j; i++, j--) {
                temp = xf[i];
                xf[i] = xf[j];
                xf[j] = temp;
            }
        }
    }

    if (border != size) {
        for (i = border, j = size-1; i < j; i++, j--) {
            temp = xf[i];
            xf[i] = xf[j];
            xf[j] = temp;
        }
    }
}

#endif
