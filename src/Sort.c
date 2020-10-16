/*
    csas - terminal file manager
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

static bool CheckIfMatchesBetterFiles(const unsigned char grf)
{
    for (register int i = 0; settings->BetterFiles[i] != 0; i++)
    {
        if (grf == settings->BetterFiles[i])
        {
            return 1;
        }
    }
    return 0;
}

static int compBet(const void* El1, const void* El2)
{
    register bool g1 = 0, g2 = 0;

    g1 = CheckIfMatchesBetterFiles(((struct Element*)El1)->Type);
    g2 = CheckIfMatchesBetterFiles(((struct Element*)El2)->Type);

    if (g1 == false && g2 == false)
        goto Result;
    if (g1 == false && g2 == true)
        return 1;
    if (g1 == true && g2 == false)
        return 0;
    //if (g1 == true && g1 == true)
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

    if (g1 == false && g2 == false)
        goto Result;
    if (g1 == false && g2 == true)
        return 1;
    if (g1 == true && g2 == false)
        return 0;
    //if (g1 == true && g1 == true)
        Result: ;
            return (((struct Element*)El1)->size) < (((struct Element*)El2)->size);
}
#endif

static int compSName(const void *El1, const void *El2)
{
    register bool g1 = 0, g2 = 0;
    
    g1 = CheckIfMatchesBetterFiles(((struct Element*)El1)->Type);
    g2 = CheckIfMatchesBetterFiles(((struct Element*)El2)->Type);

    if (g1 == false && g2 == false)
        goto Result;
    if (g1 == false && g2 == true)
        return 1;
    if (g1 == true && g2 == false)
        return 0;
    //if (g1 == true && g1 == true)
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
    //}

}

static int compLName(const void *El1, const void *El2)
{
    register bool g1 = 0, g2 = 0;
    
    g1 = CheckIfMatchesBetterFiles(((struct Element*)El1)->Type);
    g2 = CheckIfMatchesBetterFiles(((struct Element*)El2)->Type);

    if (g1 == false && g2 == false)
        goto Result;
    if (g1 == false && g2 == true)
        return 1;
    if (g1 == true && g2 == false)
        return 0;
    //if (g1 == true && g1 == true)
        Result: ;

             return (strcmp(((struct Element*)El1)->name,((struct Element*)El2)->name));
    //}

}

#ifdef __MTIME_ENABLE__
static int compMTime(const void *El1, const void *El2)
{
    register bool g1 = 0, g2 = 0;

    g1 = CheckIfMatchesBetterFiles(((struct Element*)El1)->Type);
    g2 = CheckIfMatchesBetterFiles(((struct Element*)El2)->Type);

    if (g1 == false && g2 == false)
        goto Result;
    if (g1 == false && g2 == true)
        return 1;
    if (g1 == true && g2 == false)
        return 0;
    //if (g1 == true && g1 == true)
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

    if (g1 == false && g2 == false)
        goto Result;
    if (g1 == false && g2 == true)
        return 1;
    if (g1 == true && g2 == false)
        return 0;
    //if (g1 == true && g1 == true)
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

    if (g1 == false && g2 == false)
        goto Result;
    if (g1 == false && g2 == true)
        return 1;
    if (g1 == true && g2 == false)
        return 0;
    //if (g1 == true && g1 == true)
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

    if (g1 == false && g2 == false)
        goto Result;
    if (g1 == false && g2 == true)
        return 1;
    if (g1 == true && g2 == false)
        return 0;
    //if (g1 == true && g1 == true)
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

    if (g1 == false && g2 == false)
        goto Result;
    if (g1 == false && g2 == true)
        return 1;
    if (g1 == true && g2 == false)
        return 0;
    //if (g1 == true && g1 == true)
        Result: ;
            return (((struct Element*)El1)->pw) > (((struct Element*)El2)->pw);
}
#endif

/*static int FindBorder(struct Element* El, size_t begin, size_t end)
{
    int j;
    bool g1 = 0, g2 = 0;
    int ret = 1;
    for (j = 0; settings->BetterFiles[j] != 0; j++)
    {
        if (El[((begin+end)/2)-1].Type == settings->BetterFiles[j])
        {
            g1 = 1;
            break;
        }
    }
    for (j = 0; settings->BetterFiles[j] != 0; j++)
    {
        if (El[((begin+end)/2)].Type == settings->BetterFiles[j])
        {
            g2 = 1;
            break;
        }
    }

    if (g1 == 1 && g2 == 0)
        ret = (begin+end)/2;
    if (g1 == 1 && g2 == 1)
        ret = FindBorder(El,begin+end/2,end);
    if (g1 == 0 && g2 == 0)
        ret = FindBorder(El,begin,end/2);

    return ret;
}*/

static size_t FindBorder(const struct Element* El, const size_t end)
{
    register bool g = 0;

    for (size_t i = 0; i < end; i++)
    {
        g = CheckIfMatchesBetterFiles(El[i].Type);

        if (g == 0)
            return i;
        g = 0;
    }

    return 1;
}

void SortEl(struct Element* El, const size_t El_t, const unsigned char Method)
{
    switch((Method > 128) ? Method-128 : Method)
    {
        case SORT_TYPE: qsort(El,El_t,sizeof(struct Element),compType); break;
        case SORT_CHIR: qsort(El,El_t,sizeof(struct Element),compBet); break;
        #ifdef __FILE_SIZE_ENABLE__
        case SORT_SIZE: qsort(El,El_t,sizeof(struct Element),compSize); break;
        #endif
        case SORT_NAME: qsort(El,El_t,sizeof(struct Element),compSName); break;
        case SORT_LNAME: qsort(El,El_t,sizeof(struct Element),compLName); break;
        #ifdef __MTIME_ENABLE__
        case SORT_MTIME: qsort(El,El_t,sizeof(struct Element),compMTime); break;
        #endif
        #ifdef __ATIME_ENABLE__
        case SORT_ATIME: qsort(El,El_t,sizeof(struct Element),compATime); break;
        #endif
        #ifdef __CTIME_ENABLE__
        case SORT_CTIME: qsort(El,El_t,sizeof(struct Element),compCTime); break;
        #endif
        #ifdef __FILE_GROUPS_ENABLE__
        case SORT_GID: qsort(El,El_t,sizeof(struct Element),compGid); break;
        #endif
        #ifdef __FILE_OWNERS_ENABLE__
        case SORT_UID: qsort(El,El_t,sizeof(struct Element),compUid); break;
        #endif
    }

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
            //size_t border = FindBorder(El,0,El_t-1);
            size_t border = FindBorder(El,El_t);

            if (border > 0)
            {
                for (size_t i = 0, j = border-1; i < j; i++, j--)
                {
                    Temp = El[i];
                    El[i] = El[j];
                    El[j] = Temp;
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
}

#endif
