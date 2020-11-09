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

#include "Sort.h"

extern Settings* settings;

static bool CheckIfMatchesBetterFiles(const uchar grf)
{
    for (register int i = 0; settings->BetterFiles[i] != 0; i++)
        if (grf == settings->BetterFiles[i])
            return 1;
    return 0;
}

static int comp(const void* El1, const void* El2, void* flag)
{
    if (*(ull*)flag & SORT_BETTER_FILES)
    {
        register bool g1 = 0, g2 = 0;

        g1 = CheckIfMatchesBetterFiles(((struct Element*)El1)->Type);
        g2 = CheckIfMatchesBetterFiles(((struct Element*)El2)->Type);

        if (!g1 && !g2) goto Result;
        if (!g1 && g2) return 1;
        if (g1 && !g2) return 0;
    }

    Result: ;
    register int ret = 0;

    switch (*(ull*)flag & SORT_IF)
    {
        case SORT_TYPE: ret = ((struct Element*)El1)->Type > ((struct Element*)El2)->Type; break;
        #ifdef __FILE_SIZE_ENABLE__
        case SORT_SIZE: ret = (((struct Element*)El1)->size) < (((struct Element*)El2)->size); break;
        #endif
        case SORT_NAME: ret = strcasecmp((((struct Element*)El1)->name),((struct Element*)El2)->name);  break;
        case SORT_LNAME: ret = (strcmp(((struct Element*)El1)->name,((struct Element*)El2)->name)); break;
        #ifdef __MTIME_ENABLE__
        case SORT_MTIME: ret = (((struct Element*)El1)->mtim.tv_sec) > (((struct Element*)El2)->mtim.tv_sec); break;
        #endif
        #ifdef __ATIME_ENABLE__
        case SORT_ATIME: ret = (((struct Element*)El1)->atim.tv_sec) > (((struct Element*)El2)->atim.tv_sec); break;
        #endif
        #ifdef __CTIME_ENABLE__
        case SORT_CTIME: ret = (((struct Element*)El1)->ctim.tv_sec) > (((struct Element*)El2)->ctim.tv_sec); break;
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

static int FindBorder(struct Element* El, const size_t begin, const size_t end)
{
    if (end == 0)
        return 0;
    register bool g1 = 0, g2 = 0;
    register int ret = 1;

    g1 = CheckIfMatchesBetterFiles(El[((begin+end)/2)-1].Type);
    g2 = CheckIfMatchesBetterFiles(El[((begin+end)/2)].Type);

    if (g1 == 1 && g2 == 0)
        ret = (begin+end)/2;
    if (g1 == 1 && g2 == 1)
        ret = FindBorder(El,begin+end/2,end);
    if (g1 == 0 && g2 == 0)
        ret = FindBorder(El,begin,end/2);

    return ret;
}

void SortEl(struct Element* El, const size_t El_t, ull flag)
{
    qsort_r(El,El_t,sizeof(struct Element),comp,&flag);

    if (!(flag & SORT_REVERSE))
        return;
    
    register size_t border = 0, i, j;
    struct Element temp;

    if (flag & SORT_BETTER_FILES)
    {
        if (El_t > 2) border = FindBorder(El,0,El_t-1);

        if (border)
        {
            for (i = 0, j = border-1; i < j; i++, j--)
            {
                temp = El[i];
                El[i] = El[j];
                El[j] = temp;
            }
        }
    }

    if (border != El_t)
    {
        for (i = border, j = El_t-1; i < j; i++, j--)
        {
            temp = El[i];
            El[i] = El[j];
            El[j] = temp;
        }
    }
}

#endif
