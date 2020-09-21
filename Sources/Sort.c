#include "main.h"

#ifdef __SORT_ELEMENTS_ENABLE__

#include "Sort.h"

extern int BetterFiles[];

static int compBet(const void* El1, const void* El2)
{
    bool g1 = 0, g2 = 0;
    int j;

    for (j = 0; BetterFiles[j] != 0; j++)
    {
        if (((struct Element*)El1)->Type == BetterFiles[j])
        {
            g1 = 1;
            break;
        }
    }
    for (j = 0; BetterFiles[j] != 0; j++)
    {
        if (((struct Element*)El2)->Type == BetterFiles[j])
        {
            g2 = 1;
            break;
        }
    }

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
static int compSize(const void *El1, const void *El2)
{
    bool g1 = 0, g2 = 0;
    int j;

    for (j = 0; BetterFiles[j] != 0; j++)
    {
        if (((struct Element*)El1)->Type == BetterFiles[j])
        {
            g1 = 1;
            break;
        }
    }
    for (j = 0; BetterFiles[j] != 0; j++)
    {
        if (((struct Element*)El2)->Type == BetterFiles[j])
        {
            g2 = 1;
            break;
        }
    }

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
    bool g1 = 0, g2 = 0;
    register int j;

    for (j = 0; BetterFiles[j] != 0; j++)
    {
        if (((struct Element*)El1)->Type == BetterFiles[j])
        {
            g1 = 1;
            break;
        }
    }
    for (j = 0; BetterFiles[j] != 0; j++)
    {
        if (((struct Element*)El2)->Type == BetterFiles[j])
        {
            g2 = 1;
            break;
        }
    }

    if (g1 == false && g2 == false)
        goto Result;
    if (g1 == false && g2 == true)
        return 1;
    if (g1 == true && g2 == false)
        return 0;
    //if (g1 == true && g1 == true)
        Result: ;
            static char tesm[2][NAME_MAX];
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
    bool g1 = 0, g2 = 0;
    register int j;

    for (j = 0; BetterFiles[j] != 0; j++)
    {
        if (((struct Element*)El1)->Type == BetterFiles[j])
        {
            g1 = 1;
            break;
        }
    }
    for (j = 0; BetterFiles[j] != 0; j++)
    {
        if (((struct Element*)El2)->Type == BetterFiles[j])
        {
            g2 = 1;
            break;
        }
    }

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
    bool g1 = 0, g2 = 0;
    int j;

    for (j = 0; BetterFiles[j] != 0; j++)
    {
        if (((struct Element*)El1)->Type == BetterFiles[j])
        {
            g1 = 1;
            break;
        }
    }
    for (j = 0; BetterFiles[j] != 0; j++)
    {
        if (((struct Element*)El2)->Type == BetterFiles[j])
        {
            g2 = 1;
            break;
        }
    }

    if (g1 == false && g2 == false)
        goto Result;
    if (g1 == false && g2 == true)
        return 1;
    if (g1 == true && g2 == false)
        return 0;
    //if (g1 == true && g1 == true)
        Result: ;
            return (((struct Element*)El1)->mtime) > (((struct Element*)El2)->mtime);
}
#endif
#ifdef __ATIME_ENABLE__
static int compATime(const void *El1, const void *El2)
{
    bool g1 = 0, g2 = 0;
    int j;

    for (j = 0; BetterFiles[j] != 0; j++)
    {
        if (((struct Element*)El1)->Type == BetterFiles[j])
        {
            g1 = 1;
            break;
        }
    }
    for (j = 0; BetterFiles[j] != 0; j++)
    {
        if (((struct Element*)El2)->Type == BetterFiles[j])
        {
            g2 = 1;
            break;
        }
    }

    if (g1 == false && g2 == false)
        goto Result;
    if (g1 == false && g2 == true)
        return 1;
    if (g1 == true && g2 == false)
        return 0;
    //if (g1 == true && g1 == true)
        Result: ;
            return (((struct Element*)El1)->atime) > (((struct Element*)El2)->atime);
}
#endif
#ifdef __CTIME_ENABLE__
static int compCTime(const void *El1, const void *El2)
{
    bool g1 = 0, g2 = 0;
    int j;

    for (j = 0; BetterFiles[j] != 0; j++)
    {
        if (((struct Element*)El1)->Type == BetterFiles[j])
        {
            g1 = 1;
            break;
        }
    }
    for (j = 0; BetterFiles[j] != 0; j++)
    {
        if (((struct Element*)El2)->Type == BetterFiles[j])
        {
            g2 = 1;
            break;
        }
    }

    if (g1 == false && g2 == false)
        goto Result;
    if (g1 == false && g2 == true)
        return 1;
    if (g1 == true && g2 == false)
        return 0;
    //if (g1 == true && g1 == true)
        Result: ;
            return (((struct Element*)El1)->ctime) > (((struct Element*)El2)->ctime);
}
#endif

#ifdef __FILE_GROUPS_ENABLE__
static int compGid(const void *El1, const void *El2)
{
    bool g1 = 0, g2 = 0;
    int j;

    for (j = 0; BetterFiles[j] != 0; j++)
    {
        if (((struct Element*)El1)->Type == BetterFiles[j])
        {
            g1 = 1;
            break;
        }
    }
    for (j = 0; BetterFiles[j] != 0; j++)
    {
        if (((struct Element*)El2)->Type == BetterFiles[j])
        {
            g2 = 1;
            break;
        }
    }

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
    bool g1 = 0, g2 = 0;
    int j;

    for (j = 0; BetterFiles[j] != 0; j++)
    {
        if (((struct Element*)El1)->Type == BetterFiles[j])
        {
            g1 = 1;
            break;
        }
    }
    for (j = 0; BetterFiles[j] != 0; j++)
    {
        if (((struct Element*)El2)->Type == BetterFiles[j])
        {
            g2 = 1;
            break;
        }
    }

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
    for (j = 0; BetterFiles[j] != 0; j++)
    {
        if (El[((begin+end)/2)-1].Type == BetterFiles[j])
        {
            g1 = 1;
            break;
        }
    }
    for (j = 0; BetterFiles[j] != 0; j++)
    {
        if (El[((begin+end)/2)].Type == BetterFiles[j])
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

static int FindBorder(struct Element* El, size_t end)
{
    int j;
    bool g = 0;
    
    for (int i = 0; i < end; i++)
    {
        for (j = 0; BetterFiles[j] != 0; j++)
        {
            if (El[i].Type == BetterFiles[j])
            {
                g = 1;
                break;
            }
        }

        if (g == 0)
            return i;
        g = 0;
    }

    return 1;
}

void SortEl(struct Element* El, size_t El_t, unsigned char Method)
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

    if (Method&128)
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
            //int border = FindBorder(El,0,El_t-1);
            int border = FindBorder(El,El_t);

            if (border > 0)
            {
                for (int i = 0, j = border-1; i < j; i++, j--)
                {
                    Temp = El[i];
                    El[i] = El[j];
                    El[j] = Temp;
                }

                if (border != El_t)
                {
                    for (int i = border, j = El_t-1; i < j; i++, j--)
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
