/*
    csas - console file manager
    Copyright (C) 2020 TUVIMEN <suchora.dominik7@gmail.com>

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

extern Settings* settings;

static bool ismatching(const uchar src)
{
    register bool ret = 0;
    for (register int i = 0; settings->BetterFiles[i] != 0 && ret == 0; i++)
        ret = 1*(src == settings->BetterFiles[i]);
    return ret;
}

static int comp(const void* El1, const void* El2, void* flag)
{
    if (*(ull*)flag & SORT_BETTER_FILES)
    {
        register bool g1 = 0, g2 = 0;

        g1 = ismatching(((struct Element*)El1)->type);
        g2 = ismatching(((struct Element*)El2)->type);

        if (!g1 && !g2) goto Result;
        if (!g1 && g2) return 1;
        if (g1 && !g2) return 0;
    }

    Result: ;
    register int ret = 0;

    switch (*(ull*)flag & SORT_IF)
    {
        case SORT_TYPE: ret = ((struct Element*)El1)->type > ((struct Element*)El2)->type; break;
        #ifdef __FILE_SIZE_ENABLE__
        case SORT_SIZE: ret = (((struct Element*)El1)->size) < (((struct Element*)El2)->size); break;
        #endif
        case SORT_NAME: ret = strcasecmp((((struct Element*)El1)->name),((struct Element*)El2)->name);  break;
        case SORT_LNAME: ret = (strcmp(((struct Element*)El1)->name,((struct Element*)El2)->name)); break;
        #ifdef __MTIME_ENABLE__
        case SORT_MTIME: ret = (((struct Element*)El1)->mtim) > (((struct Element*)El2)->mtim); break;
        #endif
        #ifdef __ATIME_ENABLE__
        case SORT_ATIME: ret = (((struct Element*)El1)->atim) > (((struct Element*)El2)->atim); break;
        #endif
        #ifdef __CTIME_ENABLE__
        case SORT_CTIME: ret = (((struct Element*)El1)->ctim) > (((struct Element*)El2)->ctim); break;
        #endif
        #ifdef __FILE_GROUPS_ENABLE__
        case SORT_GID: ret = (((struct Element*)El1)->gr) > (((struct Element*)El2)->gr); break;
        #endif
        #ifdef __FILE_OWNERS_ENABLE__
        case SORT_UID: ret = (((struct Element*)El1)->pw) > (((struct Element*)El2)->pw); break;
        #endif
    }

    return ret;
}

static size_t FindBorder(const struct Element* el, size_t size)
{
    register size_t ret = 0;
    for (register size_t i = 0; i < size && ret == 0; i++)
        ret = i*!ismatching(el[i].type);
    return ret;
}

void SortEl(struct Element* el, const size_t size, ull flag)
{
    qsort_r(el,size,sizeof(struct Element),comp,&flag);

    if (!(flag & SORT_REVERSE))
        return;
    
    register size_t border = 0, i, j;
    struct Element temp;

    if (flag & SORT_BETTER_FILES)
    {
        border = FindBorder(el,size);

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
