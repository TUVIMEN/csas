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

static int compBet(const void* El1, const void* El2)
{
    register bool g1 = 0, g2 = 0;

    g1 = CheckIfMatchesBetterFiles(((struct Element*)El1)->Type);
    g2 = CheckIfMatchesBetterFiles(((struct Element*)El2)->Type);

    if (!g1 && !g2) goto Result;
    if (!g1 && g2) return 1;
    if (g1 && !g2) return 0;

    Result: ;
    return 0;
}

static int compType(const void* El1, const void* El2)
{
    return ((struct Element*)El1)->Type > ((struct Element*)El2)->Type;
}

#ifdef __FILE_SIZE_ENABLE__
static int compSize(const void  *El1, const void *El2)
{
    register bool g1 = 0, g2 = 0;

    g1 = CheckIfMatchesBetterFiles(((struct Element*)El1)->Type);
    g2 = CheckIfMatchesBetterFiles(((struct Element*)El2)->Type);

    if (!g1 && !g2) goto Result;
    if (!g1 && g2) return 1;
    if (g1 && !g2) return 0;

    Result: ;
    return (((struct Element*)El1)->size) < (((struct Element*)El2)->size);
}
#endif

static int compSName(const void *El1, const void *El2)
{
    register bool g1 = 0, g2 = 0;
    
    g1 = CheckIfMatchesBetterFiles(((struct Element*)El1)->Type);
    g2 = CheckIfMatchesBetterFiles(((struct Element*)El2)->Type);

    if (!g1 && !g2) goto Result;
    if (!g1 && g2) return 1;
    if (g1 && !g2) return 0;

    Result: ;
    register int j;
    char tesm[2][NAME_MAX];
    strcpy(tesm[0],((struct Element*)El1)->name);
    strcpy(tesm[1],((struct Element*)El2)->name);

    for (j = 0; tesm[0][j] != 0; j++)
        tesm[0][j] ^= 32*(tesm[0][j] > 96 && tesm[0][j] < 123);
    for (j = 0; tesm[1][j] != 0; j++)
            tesm[1][j] ^= 32*(tesm[1][j] > 96 && tesm[1][j] < 123);

    j = strcmp(tesm[0],tesm[1]);
    return j;
}

static int compLName(const void *El1, const void *El2)
{
    register bool g1 = 0, g2 = 0;
    
    g1 = CheckIfMatchesBetterFiles(((struct Element*)El1)->Type);
    g2 = CheckIfMatchesBetterFiles(((struct Element*)El2)->Type);

    if (!g1 && !g2) goto Result;
    if (!g1 && g2) return 1;
    if (g1 && !g2) return 0;

    Result: ;
    return (strcmp(((struct Element*)El1)->name,((struct Element*)El2)->name));

}

#ifdef __MTIME_ENABLE__
static int compMTime(const void *El1, const void *El2)
{
    register bool g1 = 0, g2 = 0;

    g1 = CheckIfMatchesBetterFiles(((struct Element*)El1)->Type);
    g2 = CheckIfMatchesBetterFiles(((struct Element*)El2)->Type);

    if (!g1 && !g2) goto Result;
    if (!g1 && g2) return 1;
    if (g1 && !g2) return 0;

    Result: ;
    return (((struct Element*)El1)->mtim.tv_sec) > (((struct Element*)El2)->mtim.tv_sec);
}
#endif
#ifdef __ATIME_ENABLE__
static int compATime(const void *El1, const void *El2)
{
    register bool g1 = 0, g2 = 0;
    
    g1 = CheckIfMatchesBetterFiles(((struct Element*)El1)->Type);
    g2 = CheckIfMatchesBetterFiles(((struct Element*)El2)->Type);

    if (!g1 && !g2) goto Result;
    if (!g1 && g2) return 1;
    if (g1 && !g2) return 0;

    Result: ;
    return (((struct Element*)El1)->atim.tv_sec) > (((struct Element*)El2)->atim.tv_sec);
}
#endif
#ifdef __CTIME_ENABLE__
static int compCTime(const void *El1, const void *El2)
{
    register bool g1 = 0, g2 = 0;
    
    g1 = CheckIfMatchesBetterFiles(((struct Element*)El1)->Type);
    g2 = CheckIfMatchesBetterFiles(((struct Element*)El2)->Type);

    if (!g1 && !g2) goto Result;
    if (!g1 && g2) return 1;
    if (g1 && !g2) return 0;

    Result: ;
    return (((struct Element*)El1)->ctim.tv_sec) > (((struct Element*)El2)->ctim.tv_sec);
}
#endif

#ifdef __FILE_GROUPS_ENABLE__
static int compGid(const void *El1, const void *El2)
{
    register bool g1 = 0, g2 = 0;
    
    g1 = CheckIfMatchesBetterFiles(((struct Element*)El1)->Type);
    g2 = CheckIfMatchesBetterFiles(((struct Element*)El2)->Type);

    if (!g1 && !g2) goto Result;
    if (!g1 && g2) return 1;
    if (g1 && !g2) return 0;

    Result: ;
    return (((struct Element*)El1)->gr) > (((struct Element*)El2)->gr);
}
#endif

#ifdef __FILE_OWNERS_ENABLE__
static int compUid(const void *El1, const void *El2)
{
    register bool g1 = 0, g2 = 0;
    
    g1 = CheckIfMatchesBetterFiles(((struct Element*)El1)->Type);
    g2 = CheckIfMatchesBetterFiles(((struct Element*)El2)->Type);

    if (!g1 && !g2) goto Result;
    if (!g1 && g2) return 1;
    if (g1 && !g2) return 0;
        
    Result: ;
    return (((struct Element*)El1)->pw) > (((struct Element*)El2)->pw);
}
#endif

static int FindBorder(struct Element* El, size_t begin, size_t end)
{
    if (end == 0)
        return 0;
    bool g1 = 0, g2 = 0;
    int ret = 1;

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

void SortEl(struct Element* El, const size_t El_t, const uchar Method)
{
    int (*Function)(const void*,const void*);
    switch((Method > 128) ? Method-128 : Method)
    {
        case SORT_TYPE: Function = compType; break;
        case SORT_CHIR: Function = compBet; break;
        #ifdef __FILE_SIZE_ENABLE__
        case SORT_SIZE: Function = compSize; break;
        #endif
        case SORT_NAME: Function = compSName; break;
        case SORT_LNAME: Function = compLName; break;
        #ifdef __MTIME_ENABLE__
        case SORT_MTIME: Function = compMTime; break;
        #endif
        #ifdef __ATIME_ENABLE__
        case SORT_ATIME: Function = compATime; break;
        #endif
        #ifdef __CTIME_ENABLE__
        case SORT_CTIME: Function = compCTime; break;
        #endif
        #ifdef __FILE_GROUPS_ENABLE__
        case SORT_GID: Function = compGid; break;
        #endif
        #ifdef __FILE_OWNERS_ENABLE__
        case SORT_UID: Function = compUid; break;
        #endif
    }

    qsort(El,El_t,sizeof(struct Element),Function);

    if (Method&128) //reverse
    {
        struct Element Temp;
        if ((Method-128) < 32)
        {
            for (int i = 0, j = El_t-1; i < j; i++, j--)
            {
                Temp = El[i];
                El[i] = El[j];
                El[j] = Temp;
            }
        }
        else
        {
            size_t border = 0;
            if (El_t > 2)
                border = FindBorder(El,0,El_t-1);

            if (border)
            {
                for (size_t i = 0, j = border-1; i < j; i++, j--)
                {
                    Temp = El[i];
                    El[i] = El[j];
                    El[j] = Temp;
                }
            }

            if (border != El_t)
            {
                for (size_t i = border, j = El_t-1; i < j; i++, j--)
                {
                    Temp = El[i];
                    El[i] = El[j];
                    El[j] = Temp;
                }
            }
        }
    }
}

#endif
