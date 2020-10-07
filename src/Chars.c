#include "main.h"
#include "Draw.h"
#include "Load.h"
#include "FastRun.h"
#include "Usefull.h"
#include "Functions.h"

extern Key* keys;
extern size_t keys_t;
extern Settings* settings;

void ChangeWorkSpace(Basic* grf, const int num)
{
    register int temp = grf->inW;
    grf->inW = num;

    if (!grf->Work[num].exists)
    {
        grf->Work[num].exists = 1;
        CD(grf->Work[temp].win[1]->path,grf);
    }
    else
        CD(grf->Work[num].win[1]->path,grf);
}

int UpdateEvent(Basic* grf)
{
    memset(grf->cSF,0,63);
    int Event = getch();
    if (Event == -1) { return -1; }

    grf->cSF_E = true;

    while (Event == -1 || Event == 410 || (Event > 47 && Event < 58))
    {
        if (Event == 410) { UpdateSizeBasic(grf); DrawBasic(grf,-1); }
        if (Event > 47 && Event < 58) { grf->cSF[strlen(grf->cSF)] = Event; DrawBasic(grf,4); }
        Event = getch();
    }

    int* theyPass = NULL;
    size_t theyPass_t = 0;

    if ((int)Event == 27) { grf->cSF_E = false; return -1; }
    grf->cSF[strlen(grf->cSF)] = Event;

    for (size_t i = 0; i < keys_t; i++)
    {
        if (Event == keys[i].keys[0])
        {
            theyPass = (int*)realloc(theyPass,(theyPass_t+1)*sizeof(int));
            theyPass_t++;
            theyPass[theyPass_t-1] = i;
        }
    }


    bool StartsTheString = 0;

    for (int i = 1; theyPass_t > 1; i++)
    {
        DrawBasic(grf,-1);
        do {
            if (Event == 410) { UpdateSizeBasic(grf); DrawBasic(grf,-1); }
            if (Event > 47 && Event < 58) { grf->cSF[strlen(grf->cSF)] = Event; DrawBasic(grf,4); }
            Event = getch();
        } while (Event == -1 || Event == 410 || (StartsTheString && Event > 47 && Event < 58));
        if (Event == 27) { grf->cSF_E = false; return -1; }
        grf->cSF[strlen(grf->cSF)] = Event;
        StartsTheString = 1;

        int* abcs = NULL;
        size_t abcs_t = 0;

        for (size_t j = 0; j < theyPass_t; j++)
        {
            if (Event == keys[theyPass[j]].keys[i])
            {
                abcs = (int*)realloc(abcs,(abcs_t+1)*sizeof(int));
                abcs_t++;
                abcs[abcs_t-1] = theyPass[j];
            }
        }

        free(theyPass);
        theyPass = abcs;
        theyPass_t = abcs_t;
    }

    grf->cSF_E = false;
    return (theyPass_t != 0) ? theyPass[0] : -1;
}

void GoDown(Basic* grf)
{
    if (grf->Work[grf->inW].Visual)
    {
        for (long long int i = grf->Work[grf->inW].win[1]->selected[grf->inW]; i < grf->Work[grf->inW].win[1]->El_t; i++)
            grf->Work[grf->inW].win[1]->El[i].List[grf->inW] |= grf->Work[grf->inW].SelectedGroup;
    }
    if (grf->Work[grf->inW].win[1]->El_t > (long long int)grf->win[1]->_maxy-!settings->Borders+settings->Borders)
        grf->Work[grf->inW].win[1]->Ltop[grf->inW] = grf->Work[grf->inW].win[1]->El_t-grf->win[1]->_maxy-!settings->Borders+settings->Borders;
    else
        grf->Work[grf->inW].win[1]->Ltop[grf->inW] = 0;
    grf->Work[grf->inW].win[1]->selected[grf->inW] = grf->Work[grf->inW].win[1]->El_t-1;
}

static void GoTop(Basic* grf)
{
    if (grf->Work[grf->inW].Visual)
    {
        for (int i = grf->Work[grf->inW].win[1]->selected[grf->inW]; i > -1; i--)
            grf->Work[grf->inW].win[1]->El[i].List[grf->inW] |= grf->Work[grf->inW].SelectedGroup;
    }
    grf->Work[grf->inW].win[1]->selected[grf->inW] = 0;
    grf->Work[grf->inW].win[1]->Ltop[grf->inW] = 0;
}

static void MoveD(const int how, Basic* grf)
{
    if (how == 1) //down
    {
        if (settings->WrapScroll)
        {
            if ((long long int)grf->Work[grf->inW].win[1]->selected[grf->inW] == grf->Work[grf->inW].win[1]->El_t-1)
            {
                GoTop(grf);
                return;
            }
        }
        if (grf->Work[grf->inW].win[1]->El_t-1 > (long long int)grf->Work[grf->inW].win[1]->selected[grf->inW])
            grf->Work[grf->inW].win[1]->selected[grf->inW]++;
        if ((long long int)(grf->win[1]->_maxy+grf->Work[grf->inW].win[1]->Ltop[grf->inW]-(settings->Borders*2)) != grf->Work[grf->inW].win[1]->El_t-1 && grf->win[1]->_maxy+grf->Work[grf->inW].win[1]->Ltop[grf->inW]-(settings->Borders*2) < grf->Work[grf->inW].win[1]->selected[grf->inW]+(int)(grf->win[1]->_maxy*settings->MoveOffSet))
        {
            if (settings->JumpScroll)
            {
                if ((long long int)(grf->win[1]->_maxy+grf->Work[grf->inW].win[1]->Ltop[grf->inW]-(settings->Borders*2)+(int)(grf->win[1]->_maxy*settings->JumpScrollValue)) > grf->Work[grf->inW].win[1]->El_t-1)
                    grf->Work[grf->inW].win[1]->Ltop[grf->inW] = grf->Work[grf->inW].win[1]->El_t-grf->win[1]->_maxy-!settings->Borders+settings->Borders;
                else
                    grf->Work[grf->inW].win[1]->Ltop[grf->inW] += (int)(grf->win[1]->_maxy*settings->JumpScrollValue);
            }
            else
                grf->Work[grf->inW].win[1]->Ltop[grf->inW]++;
        }
    }
    if (how == 2) //up
    {
        if (settings->WrapScroll)
        {
            if (grf->Work[grf->inW].win[1]->selected[grf->inW] == 0)
            {
                GoDown(grf);
                return;
            }
        }
        if (0 < grf->Work[grf->inW].win[1]->selected[grf->inW])
            grf->Work[grf->inW].win[1]->selected[grf->inW]--;
        if (grf->win[1]->_maxy+grf->Work[grf->inW].win[1]->Ltop[grf->inW]-(settings->Borders*2) != 0 && grf->Work[grf->inW].win[1]->Ltop[grf->inW] > grf->Work[grf->inW].win[1]->selected[grf->inW]-(int)(grf->win[1]->_maxy*settings->MoveOffSet))
        {
            if (settings->JumpScroll)
            {
                if (grf->win[1]->_maxy+grf->Work[grf->inW].win[1]->Ltop[grf->inW]-(settings->Borders*2)-(int)(grf->win[1]->_maxy*settings->JumpScrollValue) < (size_t)(grf->win[1]->_maxy*settings->JumpScrollValue)*2)
                    grf->Work[grf->inW].win[1]->Ltop[grf->inW] = 0;
                else
                    grf->Work[grf->inW].win[1]->Ltop[grf->inW] -= (int)(grf->win[1]->_maxy*settings->JumpScrollValue);
            }
            else
                grf->Work[grf->inW].win[1]->Ltop[grf->inW]--;
        }
    }
    if (grf->Work[grf->inW].Visual)
        grf->Work[grf->inW].win[1]->El[grf->Work[grf->inW].win[1]->selected[grf->inW]].List[grf->inW] |= grf->Work[grf->inW].SelectedGroup;
}

void ExitBasic(Basic* grf)
{
    int count = 0;
    for (int i = 0; i < WORKSPACE_N; i++)
        count += grf->Work[i].exists;

    grf->Work[grf->inW].exists = 0;

    if (count > 1)
    {
        for (int i = grf->inW+1; i < WORKSPACE_N; i++)
            if (grf->Work[i].exists)
            {
                ChangeWorkSpace(grf,i);
                return;
            }
        for (int i = 0; i < grf->inW; i++)
            if (grf->Work[i].exists)
            {
                ChangeWorkSpace(grf,i);
                return;
            }
    }
    else
        grf->ExitTime = true;
}

void RunEvent(const int si, Basic* grf)
{
    int temp;
    switch (keys[si].act)
    {
        case 0:
            ExitBasic(grf);
            break;
        case 1:
            if (
                #ifdef __THREADS_FOR_DIR_ENABLE__
                !grf->Work[grf->inW].win[1]->enable &&
                #endif
                grf->Work[grf->inW].win[1]->El_t > 0)
            {
                temp = atoi(grf->cSF);
                for (int i = 0; i < (temp+(temp == 0))*(keys[si].slc[0].ll+(keys[si].slc[0].ll == 0)); i++)
                {
                    MoveD(1,grf);
                }
                if (settings->Win3Enable)
                    FastRun(grf);
            }
            break;
        case 2:
            if (
                #ifdef __THREADS_FOR_DIR_ENABLE__
                !grf->Work[grf->inW].win[1]->enable &&
                #endif
                grf->Work[grf->inW].win[1]->El_t > 0)
                {
                    temp = atoi(grf->cSF);
                    for (int i = 0; i < (temp+(temp == 0))*(keys[si].slc[0].ll+(keys[si].slc[0].ll == 0)); i++)
                    {
                        MoveD(2,grf);
                    }
                    if (settings->Win3Enable)
                        FastRun(grf);
                }
            break;
        case 3:
            CD("..",grf);
            grf->Work[grf->inW].Visual = 0;
            break;
        case 4:
            if (grf->Work[grf->inW].win[1]->El_t > 0)
            {
                switch (grf->Work[grf->inW].win[1]->El[grf->Work[grf->inW].win[1]->selected[grf->inW]].Type)
                {
                    case T_DIR:
                    case T_LDIR:
                        CD(grf->Work[grf->inW].win[1]->El[grf->Work[grf->inW].win[1]->selected[grf->inW]].name,grf);
                        grf->Work[grf->inW].Visual = 0;
                        break;
                    case T_REG:
                    case T_LREG:
                        RunFile(grf->Work[grf->inW].win[1]->El[grf->Work[grf->inW].win[1]->selected[grf->inW]].name);
                        break;
                }
            }
            break;
        case 5:
            if (
                #ifdef __THREADS_FOR_DIR_ENABLE__
                !grf->Work[grf->inW].win[1]->enable &&
                #endif
                grf->Work[grf->inW].win[1]->El_t > 0)
            {
                temp = atoi(grf->cSF);
                if (temp == 0)
                    GoTop(grf);
                else if ((size_t)temp > grf->Work[grf->inW].win[1]->selected[grf->inW])
                {
                    for (int i = grf->Work[grf->inW].win[1]->selected[grf->inW]; i < temp-1; i++)
                        MoveD(1,grf);
                }
                else if ((size_t)temp < grf->Work[grf->inW].win[1]->selected[grf->inW])
                {
                    for (int i = grf->Work[grf->inW].win[1]->selected[grf->inW]; i > temp-1; i--)
                        MoveD(2,grf);
                }
                if (settings->Win3Enable)
                    FastRun(grf);
            }
            break;
        case 6:
            if (
                #ifdef __THREADS_FOR_DIR_ENABLE__
                !grf->Work[grf->inW].win[1]->enable &&
                #endif
                grf->Work[grf->inW].win[1]->El_t > 0)
            {
                temp = atoi(grf->cSF);
                if (temp == 0)
                    GoDown(grf);
                else if ((size_t)temp > grf->Work[grf->inW].win[1]->selected[grf->inW])
                {
                    for (int i = grf->Work[grf->inW].win[1]->selected[grf->inW]; i < temp-1; i++)
                        MoveD(1,grf);
                }
                else if ((size_t)temp < grf->Work[grf->inW].win[1]->selected[grf->inW])
                {
                    for (int i = grf->Work[grf->inW].win[1]->selected[grf->inW]; i > temp-1; i--)
                        MoveD(2,grf);
                }
                if (settings->Win3Enable)
                    FastRun(grf);
            }
            break;
        case 8:
            ChangeWorkSpace(grf,keys[si].slc[0].ll);
            break;
        #ifdef __SORT_ELEMENTS_ENABLE__
        case 9:
            settings->SortMethod = keys[si].slc[0].ll;
            break;
        #endif
        #ifdef __GET_DIR_SIZE_ENABLE__
        case 10:
            if (
                #ifdef __THREADS_FOR_DIR_ENABLE__
                !grf->Work[grf->inW].win[1]->enable &&
                #endif
                grf->Work[grf->inW].win[1]->El_t > 0 && (temp = open(grf->Work[grf->inW].win[1]->path,O_DIRECTORY)) != -1)
            {
                int tfd;
                int counter = 0;

                for (long long int i = 0; i < grf->Work[grf->inW].win[1]->El_t; i++)
                {
                    if ((grf->Work[grf->inW].win[1]->El[i].List[grf->inW]&grf->Work[grf->inW].SelectedGroup) == grf->Work[grf->inW].SelectedGroup)
                    {
                        counter++;
                        if (grf->Work[grf->inW].win[1]->El[i].Type == T_DIR ||
                            grf->Work[grf->inW].win[1]->El[i].Type == T_LDIR)
                        {
                            if (faccessat(temp,grf->Work[grf->inW].win[1]->El[i].name,R_OK,0) == 0)
                            {
                                if ((tfd = openat(temp,grf->Work[grf->inW].win[1]->El[i].name,O_DIRECTORY)) != -1)
                                {
                                    #ifdef __GET_DIR_SIZE_ENABLE__
                                    if ((keys[si].slc[0].ll&D_F) != D_F)
                                        grf->Work[grf->inW].win[1]->El[i].size = GetDirSize(tfd,(keys[si].slc[0].ll&D_R) == D_R,(keys[si].slc[0].ll&D_C) == D_C);
                                    else
                                    #endif
                                    {
                                        struct stat sFile;
                                        stat(grf->Work[grf->inW].win[1]->El[i].name,&sFile);
                                        grf->Work[grf->inW].win[1]->El[i].size = sFile.st_size;
                                    }
                                    #ifdef __HUMAN_READABLE_SIZE_ENABLE__
                                    if (grf->Work[grf->inW].win[1]->El[i].SizErrToDisplay == NULL)
                                        grf->Work[grf->inW].win[1]->El[i].SizErrToDisplay = (char*)malloc(16);
                                    MakeHumanReadAble(grf->Work[grf->inW].win[1]->El[i].SizErrToDisplay
                                        ,grf->Work[grf->inW].win[1]->El[i].size,(keys[si].slc[0].ll&D_H) != D_H);
                                    #endif
                                    close(tfd);
                                }
                            }
                        }
                    }
                }

                if (counter == 0 && (grf->Work[grf->inW].win[1]->El[grf->Work[grf->inW].win[1]->selected[grf->inW]].Type == T_DIR ||
                    grf->Work[grf->inW].win[1]->El[grf->Work[grf->inW].win[1]->selected[grf->inW]].Type == T_LDIR))
                {
                    if (faccessat(temp,grf->Work[grf->inW].win[1]->El[grf->Work[grf->inW].win[1]->selected[grf->inW]].name,R_OK,0) == 0)
                    {
                        if ((tfd = openat(temp,grf->Work[grf->inW].win[1]->El[grf->Work[grf->inW].win[1]->selected[grf->inW]].name,O_DIRECTORY)) != -1)
                        {
                            #ifdef __GET_DIR_SIZE_ENABLE__
                            if ((keys[si].slc[0].ll&D_F) != D_F)
                                grf->Work[grf->inW].win[1]->El[grf->Work[grf->inW].win[1]->selected[grf->inW]].size = GetDirSize(tfd,(keys[si].slc[0].ll&D_R) == D_R,(keys[si].slc[0].ll&D_C) == D_C);
                            else
                            #endif
                            {
                                struct stat sFile;
                                stat(grf->Work[grf->inW].win[1]->El[grf->Work[grf->inW].win[1]->selected[grf->inW]].name,&sFile);
                                grf->Work[grf->inW].win[1]->El[grf->Work[grf->inW].win[1]->selected[grf->inW]].size = sFile.st_size;
                            }
                            #ifdef __HUMAN_READABLE_SIZE_ENABLE__
                            if (grf->Work[grf->inW].win[1]->El[grf->Work[grf->inW].win[1]->selected[grf->inW]].SizErrToDisplay == NULL)
                                grf->Work[grf->inW].win[1]->El[grf->Work[grf->inW].win[1]->selected[grf->inW]].SizErrToDisplay = (char*)malloc(16);
                            MakeHumanReadAble(grf->Work[grf->inW].win[1]->El[grf->Work[grf->inW].win[1]->selected[grf->inW]].SizErrToDisplay
                                ,grf->Work[grf->inW].win[1]->El[grf->Work[grf->inW].win[1]->selected[grf->inW]].size,(keys[si].slc[0].ll&D_H) != D_H);
                            #endif
                            close(tfd);
                        }
                    }
                }
                close(temp);
            }
            break;
        #endif
        case 11:
            grf->Work[grf->inW].SelectedGroup = keys[si].slc[0].ll;
            if (grf->Work[grf->inW].Visual && grf->Work[grf->inW].win[1]->El_t > 0)
                grf->Work[grf->inW].win[1]->El[grf->Work[grf->inW].win[1]->selected[grf->inW]].List[grf->inW] |= grf->Work[grf->inW].SelectedGroup;
            break;
        case 12:
            if (
                #ifdef __THREADS_FOR_DIR_ENABLE__
                !grf->Work[grf->inW].win[1]->enable &&
                #endif
                grf->Work[grf->inW].win[1]->El_t > 0)
            {
                grf->Work[grf->inW].win[1]->El[grf->Work[grf->inW].win[1]->selected[grf->inW]].List[grf->inW] ^= grf->Work[grf->inW].SelectedGroup;
                MoveD(1,grf);
                if (settings->Win3Enable)
                        FastRun(grf);
            }
            break;
        case 13:
            grf->Work[grf->inW].Visual = !grf->Work[grf->inW].Visual;
            if (grf->Work[grf->inW].Visual && grf->Work[grf->inW].win[1]->El_t > 0)
                grf->Work[grf->inW].win[1]->El[grf->Work[grf->inW].win[1]->selected[grf->inW]].List[grf->inW] |= grf->Work[grf->inW].SelectedGroup;
            break;
        case 14:
            if (keys[si].slc[1].ll == 0)
            {
                if (keys[si].slc[0].ll == -1)
                {
                    for (size_t i = 0; i < grf->ActualSize; i++)
                        for (long long int j = 0; j < grf->Base[i].El_t; j++)
                            grf->Base[i].El[j].List[grf->inW] ^= grf->Work[grf->inW].SelectedGroup;
                }
                else if (keys[si].slc[0].ll == 0)
                {
                    for (size_t i = 0; i < grf->ActualSize; i++)
                        for (long long int j = 0; j < grf->Base[i].El_t; j++)
                            grf->Base[i].El[j].List[grf->inW] ^= grf->Work[grf->inW].SelectedGroup*((grf->Base[i].El[j].List[grf->inW]&grf->Work[grf->inW].SelectedGroup) == grf->Work[grf->inW].SelectedGroup);
                }
                else
                {
                    for (size_t i = 0; i < grf->ActualSize; i++)
                        for (long long int j = 0; j < grf->Base[i].El_t; j++)
                            grf->Base[i].El[j].List[grf->inW] |= grf->Work[grf->inW].SelectedGroup;
                }
            }
            else
            {
                if (keys[si].slc[0].ll == -1)
                {
                    for (long long int i = 0; i < grf->Work[grf->inW].win[1]->El_t; i++)
                        grf->Work[grf->inW].win[1]->El[i].List[grf->inW] ^= grf->Work[grf->inW].SelectedGroup;
                }
                else if (keys[si].slc[0].ll == 0)
                {
                    for (long long int i = 0; i < grf->Work[grf->inW].win[1]->El_t; i++)
                        grf->Work[grf->inW].win[1]->El[i].List[grf->inW] ^= grf->Work[grf->inW].SelectedGroup*((grf->Work[grf->inW].win[1]->El[i].List[grf->inW]&grf->Work[grf->inW].SelectedGroup) == grf->Work[grf->inW].SelectedGroup);
                }
                else
                {
                    for (long long int i = 0; i < grf->Work[grf->inW].win[1]->El_t; i++)
                        grf->Work[grf->inW].win[1]->El[i].List[grf->inW] |= grf->Work[grf->inW].SelectedGroup;
                }
            }
            break;
        case 15:
            MoveGroup(grf,".",(mode_t)keys[si].slc[0].ll);
            UpdateSizeBasic(grf);
            CD(".",grf);
            break;
        case 16:
            CopyGroup(grf,".",(mode_t)keys[si].slc[0].ll);
            UpdateSizeBasic(grf);
            CD(".",grf);
            break;
        case 17:
            DeleteGroup(grf,(mode_t)keys[si].slc[0].ll);
            UpdateSizeBasic(grf);
            CD(".",grf);
            break;
        case 27:
            if (keys[si].slc[0].ll == 0)
                CD(keys[si].slc[1].v,grf);
            else
                CD(getenv(keys[si].slc[1].v),grf);
            break;
    }
}
