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

extern Settings *cfg;

static bool ismatching(const uchar src)
{
    for (register int i = 0; cfg->BetterFiles[i] != 0; i++)
        if (src == cfg->BetterFiles[i])
            return 1;
    return 0;
}

static int cmp_type(struct Element *el1, struct Element *el2)
    {return el1->type > el2->type;}
#ifdef __FILE_SIZE_ENABLE__
static int cmp_size(struct Element *el1, struct Element *el2)
    {return el1->size < el2->size;}
#endif
static int cmp_name(struct Element *el1, struct Element *el2)
    {return strcasecmp(el1->name,el2->name);}
static int cmp_lname(struct Element *el1, struct Element *el2)
    {return strcmp(el1->name,el2->name);}
#ifdef __MTIME_ENABLE__
static int cmp_mtime(struct Element *el1, struct Element *el2)
    {return el1->mtim > el2->mtim;}
#endif
#ifdef __ATIME_ENABLE__
static int cmp_atime(struct Element *el1, struct Element *el2)
    {return el1->atim > el2->atim;}
#endif
#ifdef __CTIME_ENABLE__
static int cmp_ctime(struct Element *el1, struct Element *el2)
    {return el1->ctim > el2->ctim;}
#endif
#ifdef __FILE_GROUPS_ENABLE__
static int cmp_gid(struct Element *el1, struct Element *el2)
    {return el1->gr > el2->gr;}
#endif
#ifdef __FILE_GROUPS_ENABLE__
static int cmp_uid(struct Element *el1, struct Element *el2)
    {return el1->pw > el2->pw;}
#endif

int (*mas[])(struct Element*, struct Element*) = {
    [0]=cmp_type,
    #ifdef __FILE_SIZE_ENABLE__
    [1]=cmp_size,
    #endif
    [2]=cmp_name,[3]=cmp_lname,
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
    [8]=cmp_uid
    #endif
};

static int comp(const void *el1, const void *el2, void *flag)
{
    if (*(ull*)flag & SORT_BETTER_FILES)
    {
        register bool g1 = 0, g2 = 0;

        g1 = ismatching(((struct Element*)el1)->type);
        g2 = ismatching(((struct Element*)el2)->type);

        if (!g1 && !g2) goto RESULT;
        if (!g1 && g2) return 1;
        if (g1 && !g2) return 0;
    }

    RESULT: ;
    register int ret;

    ret = (*mas[(*(ull*)flag&SORT_IF)-1])((struct Element*)el1,(struct Element*)el2);

    return ret;
}

static size_t find_border(const struct Element *el, size_t size)
{
    register size_t ret = 0;
    for (register size_t i = 0; i < size && ret == 0; i++)
        ret = i*!ismatching(el[i].type);
    return ret;
}

void sort_el(struct Element *el, const size_t size, ull flag)
{
    if ((flag&SORT_IF) != SORT_NONE)
        qsort_r(el,size,sizeof(struct Element),comp,&flag);

    if (!(flag & SORT_REVERSE))
        return;
    
    register size_t border = 0, i, j;
    struct Element temp;

    if (flag & SORT_BETTER_FILES)
    {
        border = find_border(el,size);

        if (border)
        {
            for (i = 0, j = border-1; i < j; i++, j--)
            {
                temp = el[i];
                el[i] = el[j];
                el[j] = temp;
            }
        }
    }

    if (border != size)
    {
        for (i = border, j = size-1; i < j; i++, j--)
        {
            temp = el[i];
            el[i] = el[j];
            el[j] = temp;
        }
    }
}

#endif
