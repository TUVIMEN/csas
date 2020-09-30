#include "main.h"
#include "Functions.h"
#include "Load.h"
#include "FastRun.h"
#include "Usefull.h"

extern Key* keys;
extern size_t keys_t;
extern Settings* settings;

void CD(const char* path, Basic* this)
{
    if (chdir(path) != 0)
        return;

    #ifdef __FILESYSTEM_INFORMATION_ENABLE__
    statfs(".",&this->fs);
    #endif

    ClearWindow(this->win[1]);
    wrefresh(this->win[1]);
    GetDir(".",this,1
    #ifdef __THREADS_ENABLE__
    ,settings->Threads
    #endif
    );

    if (settings->Win1Enable)
    {
        ClearWindow(this->win[0]);
        wrefresh(this->win[0]);

        if (this->Work[this->inW].win[1]->path[0] == '/' && this->Work[this->inW].win[1]->path[1] == '\0')
            settings->Win1Display = false;
        else
        {
            GetDir("..",this,0
            #ifdef __THREADS_ENABLE__
            ,settings->Threads
            #endif
            );
            settings->Win1Display = true;
        }
    }

    if (settings->Win3Enable)
    {
        if (this->Work[this->inW].win[1]->El_t == 0)
        {
            ClearWindow(this->win[2]);
            wrefresh(this->win[2]);
        }
        if (
            #ifdef __THREADS_ENABLE__
            !this->Work[this->inW].win[1]->enable && 
            #endif
            this->Work[this->inW].win[1]->El_t > 0)
            FastRun(this);
    }
}

void ChangeWorkSpace(Basic* this, const int num)
{
    register int temp = this->inW;
    this->inW = num;

    if (!this->Work[num].exists)
    {
        this->Work[num].exists = 1;
        CD(this->Work[temp].win[1]->path,this);
    }
    else
        CD(this->Work[num].win[1]->path,this);
}

int UpdateEvent(Basic* this)
{
    memset(this->cSF,0,63);
    short int Event = getch();
    if (Event == -1) { return -1; }

    this->cSF_E = true;

    while (Event == -1 || Event == 410 || (Event > 47 && Event < 58))
    {
        if (Event == 410) { UpdateSizeBasic(this); DrawBasic(this,-1); }
        if (Event > 47 && Event < 58) { this->cSF[strlen(this->cSF)] = Event; DrawBasic(this,4); }
        Event = getch();
    }

    int* theyPass = NULL;
    size_t theyPass_t = 0;

    if ((int)Event == 27) { this->cSF_E = false; return -1; }
    this->cSF[strlen(this->cSF)] = Event;

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
        DrawBasic(this,-1);
        do {
            if (Event == 410) { UpdateSizeBasic(this); DrawBasic(this,-1); }
            if (Event > 47 && Event < 58) { this->cSF[strlen(this->cSF)] = Event; DrawBasic(this,4); }
            Event = getch();
        } while (Event == -1 || Event == 410 || (StartsTheString && Event > 47 && Event < 58));
        if ((int)Event == 27) { this->cSF_E = false; return -1; }
        this->cSF[strlen(this->cSF)] = Event;
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

    this->cSF_E = false;
    return (theyPass_t != 0) ? theyPass[0] : -1;
}

void GoDown(Basic* this)
{
    if (this->Work[this->inW].Visual)
    {
        for (long long int i = this->Work[this->inW].win[1]->selected[this->inW]; i < this->Work[this->inW].win[1]->El_t; i++)
            this->Work[this->inW].win[1]->El[i].List[this->inW] |= this->Work[this->inW].SelectedGroup;
    }
    if (this->Work[this->inW].win[1]->El_t > (long long int)this->win[1]->_maxy-!settings->Borders+settings->Borders)
        this->Work[this->inW].win[1]->Ltop[this->inW] = this->Work[this->inW].win[1]->El_t-this->win[1]->_maxy-!settings->Borders+settings->Borders;
    else
        this->Work[this->inW].win[1]->Ltop[this->inW] = 0;
    this->Work[this->inW].win[1]->selected[this->inW] = this->Work[this->inW].win[1]->El_t-1;
}

static void GoTop(Basic* this)
{
    if (this->Work[this->inW].Visual)
    {
        for (int i = this->Work[this->inW].win[1]->selected[this->inW]; i > -1; i--)
            this->Work[this->inW].win[1]->El[i].List[this->inW] |= this->Work[this->inW].SelectedGroup;
    }
    this->Work[this->inW].win[1]->selected[this->inW] = 0;
    this->Work[this->inW].win[1]->Ltop[this->inW] = 0;
}

static void MoveD(const int how, Basic* this)
{
    if (how == 1) //down
    {
        if (settings->WrapScroll)
        {
            if ((long long int)this->Work[this->inW].win[1]->selected[this->inW] == this->Work[this->inW].win[1]->El_t-1)
            {
                GoTop(this);
                return;
            }
        }
        if (this->Work[this->inW].win[1]->El_t-1 > (long long int)this->Work[this->inW].win[1]->selected[this->inW])
            this->Work[this->inW].win[1]->selected[this->inW]++;
        if ((long long int)(this->win[1]->_maxy+this->Work[this->inW].win[1]->Ltop[this->inW]-(settings->Borders*2)) != this->Work[this->inW].win[1]->El_t-1 && this->win[1]->_maxy+this->Work[this->inW].win[1]->Ltop[this->inW]-(settings->Borders*2) < this->Work[this->inW].win[1]->selected[this->inW]+(int)(this->win[1]->_maxy*settings->MoveOffSet))
        {
            if (settings->JumpScroll)
            {
                if ((long long int)(this->win[1]->_maxy+this->Work[this->inW].win[1]->Ltop[this->inW]-(settings->Borders*2)+(int)(this->win[1]->_maxy*settings->JumpScrollValue)) > this->Work[this->inW].win[1]->El_t-1)
                    this->Work[this->inW].win[1]->Ltop[this->inW] = this->Work[this->inW].win[1]->El_t-this->win[1]->_maxy-!settings->Borders+settings->Borders;
                else
                    this->Work[this->inW].win[1]->Ltop[this->inW] += (int)(this->win[1]->_maxy*settings->JumpScrollValue);
            }
            else
                this->Work[this->inW].win[1]->Ltop[this->inW]++;
        }
    }
    if (how == 2) //up
    {
        if (settings->WrapScroll)
        {
            if (this->Work[this->inW].win[1]->selected[this->inW] == 0)
            {
                GoDown(this);
                return;
            }
        }
        if (0 < this->Work[this->inW].win[1]->selected[this->inW])
            this->Work[this->inW].win[1]->selected[this->inW]--;
        if (this->win[1]->_maxy+this->Work[this->inW].win[1]->Ltop[this->inW]-(settings->Borders*2) != 0 && this->Work[this->inW].win[1]->Ltop[this->inW] > this->Work[this->inW].win[1]->selected[this->inW]-(int)(this->win[1]->_maxy*settings->MoveOffSet))
        {
            if (settings->JumpScroll)
            {
                if (this->win[1]->_maxy+this->Work[this->inW].win[1]->Ltop[this->inW]-(settings->Borders*2)-(int)(this->win[1]->_maxy*settings->JumpScrollValue) < (size_t)(this->win[1]->_maxy*settings->JumpScrollValue)*2)
                    this->Work[this->inW].win[1]->Ltop[this->inW] = 0;
                else
                    this->Work[this->inW].win[1]->Ltop[this->inW] -= (int)(this->win[1]->_maxy*settings->JumpScrollValue);
            }
            else
                this->Work[this->inW].win[1]->Ltop[this->inW]--;
        }
    }
    if (this->Work[this->inW].Visual)
        this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].List[this->inW] |= this->Work[this->inW].SelectedGroup;
}

void ExitBasic(Basic* this)
{
    int count = 0;
    for (int i = 0; i < WORKSPACE_N; i++)
        count += this->Work[i].exists;

    this->Work[this->inW].exists = 0;

    if (count > 1)
    {
        for (int i = this->inW+1; i < WORKSPACE_N; i++)
            if (this->Work[i].exists)
            {
                ChangeWorkSpace(this,i);
                return;
            }
        for (int i = 0; i < this->inW; i++)
            if (this->Work[i].exists)
            {
                ChangeWorkSpace(this,i);
                return;
            }
    }
    else
        this->ExitTime = true;
}

void RunEvent(const int si, Basic* this)
{
    int temp;
    switch (keys[si].act)
    {
        case 0:
            ExitBasic(this);
            break;
        case 1:
            if (
                #ifdef __THREADS_ENABLE__
                !this->Work[this->inW].win[1]->enable &&
                #endif
                this->Work[this->inW].win[1]->El_t > 0)
            {
                temp = atoi(this->cSF);
                for (int i = 0; i < (temp+(temp == 0))*(keys[si].slc1.ll+(keys[si].slc1.ll == 0)); i++)
                {
                    MoveD(1,this);
                }
                if (settings->Win3Enable)
                    FastRun(this);
            }
            break;
        case 2:
            if (
                #ifdef __THREADS_ENABLE__
                !this->Work[this->inW].win[1]->enable &&
                #endif
                this->Work[this->inW].win[1]->El_t > 0)
            {
                temp = atoi(this->cSF);
                for (int i = 0; i < (temp+(temp == 0))*(keys[si].slc1.ll+(keys[si].slc1.ll == 0)); i++)
                {
                    MoveD(2,this);
                }
                if (settings->Win3Enable)
                    FastRun(this);
            }
            break;
        case 3:
            CD("..",this);
            this->Work[this->inW].Visual = 0;
            break;
        case 4:
            if (this->Work[this->inW].win[1]->El_t > 0)
            {
                switch (this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].Type)
                {
                    case T_DIR:
                    case T_LDIR:
                        CD(this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].name,this);
                        this->Work[this->inW].Visual = 0;
                        break;
                    case T_REG:
                    case T_LREG:
                        RunFile(this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].name);
                        break;
                }
            }
            break;
        case 5:
            if (
                #ifdef __THREADS_ENABLE__
                !this->Work[this->inW].win[1]->enable &&
                #endif
                this->Work[this->inW].win[1]->El_t > 0)
            {
                temp = atoi(this->cSF);
                if (temp == 0)
                    GoTop(this);
                else if ((size_t)temp > this->Work[this->inW].win[1]->selected[this->inW])
                {
                    for (int i = this->Work[this->inW].win[1]->selected[this->inW]; i < temp-1; i++)
                        MoveD(1,this);
                }
                else if ((size_t)temp < this->Work[this->inW].win[1]->selected[this->inW])
                {
                    for (int i = this->Work[this->inW].win[1]->selected[this->inW]; i > temp-1; i--)
                        MoveD(2,this);
                }
                if (settings->Win3Enable)
                    FastRun(this);
            }
            break;
        case 6:
            if (
                #ifdef __THREADS_ENABLE__
                !this->Work[this->inW].win[1]->enable &&
                #endif
                this->Work[this->inW].win[1]->El_t > 0)
            {
                temp = atoi(this->cSF);
                if (temp == 0)
                    GoDown(this);
                else if ((size_t)temp > this->Work[this->inW].win[1]->selected[this->inW])
                {
                    for (int i = this->Work[this->inW].win[1]->selected[this->inW]; i < temp-1; i++)
                        MoveD(1,this);
                }
                else if ((size_t)temp < this->Work[this->inW].win[1]->selected[this->inW])
                {
                    for (int i = this->Work[this->inW].win[1]->selected[this->inW]; i > temp-1; i--)
                        MoveD(2,this);
                }
                if (settings->Win3Enable)
                    FastRun(this);
            }
            break;
        case 8:
            ChangeWorkSpace(this,keys[si].slc1.ll);
            break;
        #ifdef __SORT_ELEMENTS_ENABLE__
        case 9:
            settings->SortMethod = keys[si].slc1.ll;
            break;
        #endif
        #ifdef __GET_DIR_SIZE_ENABLE__
        case 10:
            if (
                #ifdef __THREADS_ENABLE__
                !this->Work[this->inW].win[1]->enable &&
                #endif
                this->Work[this->inW].win[1]->El_t > 0 && (temp = open(this->Work[this->inW].win[1]->path,O_DIRECTORY)) != -1)
            {
                int tfd;
                int counter = 0;

                for (long long int i = 0; i < this->Work[this->inW].win[1]->El_t; i++)
                {
                    if ((this->Work[this->inW].win[1]->El[i].List[this->inW]&this->Work[this->inW].SelectedGroup) == this->Work[this->inW].SelectedGroup)
                    {
                        counter++;
                        if (this->Work[this->inW].win[1]->El[i].Type == T_DIR ||
                            this->Work[this->inW].win[1]->El[i].Type == T_LDIR)
                        {
                            if (faccessat(temp,this->Work[this->inW].win[1]->El[i].name,R_OK,0) == 0)
                            {
                                if ((tfd = openat(temp,this->Work[this->inW].win[1]->El[i].name,O_DIRECTORY)) != -1)
                                {
                                    #ifdef __GET_DIR_SIZE_ENABLE__
                                    if ((keys[si].slc1.ll&D_F) != D_F)
                                        this->Work[this->inW].win[1]->El[i].size = GetDirSize(tfd,(keys[si].slc1.ll&D_R) == D_R,(keys[si].slc1.ll&D_C) == D_C);
                                    else
                                    #endif
                                    {
                                        struct stat sFile;
                                        stat(this->Work[this->inW].win[1]->El[i].name,&sFile);
                                        this->Work[this->inW].win[1]->El[i].size = sFile.st_size;
                                    }
                                    #ifdef __HUMAN_READABLE_SIZE_ENABLE__
                                    if (this->Work[this->inW].win[1]->El[i].SizErrToDisplay == NULL)
                                        this->Work[this->inW].win[1]->El[i].SizErrToDisplay = (char*)malloc(16);
                                    MakeHumanReadAble(this->Work[this->inW].win[1]->El[i].SizErrToDisplay
                                        ,this->Work[this->inW].win[1]->El[i].size,(keys[si].slc1.ll&D_H) != D_H);
                                    #endif
                                    close(tfd);
                                }
                            }
                        }
                    }
                }

                if (counter == 0 && (this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].Type == T_DIR ||
                    this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].Type == T_LDIR))
                {
                    if (faccessat(temp,this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].name,R_OK,0) == 0)
                    {
                        if ((tfd = openat(temp,this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].name,O_DIRECTORY)) != -1)
                        {
                            #ifdef __GET_DIR_SIZE_ENABLE__
                            if ((keys[si].slc1.ll&D_F) != D_F)
                                this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].size = GetDirSize(tfd,(keys[si].slc1.ll&D_R) == D_R,(keys[si].slc1.ll&D_C) == D_C);
                            else
                            #endif
                            {
                                struct stat sFile;
                                stat(this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].name,&sFile);
                                this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].size = sFile.st_size;
                            }
                            #ifdef __HUMAN_READABLE_SIZE_ENABLE__
                            if (this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].SizErrToDisplay == NULL)
                                this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].SizErrToDisplay = (char*)malloc(16);
                            MakeHumanReadAble(this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].SizErrToDisplay
                                ,this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].size,(keys[si].slc1.ll&D_H) != D_H);
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
            this->Work[this->inW].SelectedGroup = keys[si].slc1.ll;
            if (this->Work[this->inW].Visual && this->Work[this->inW].win[1]->El_t > 0)
                this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].List[this->inW] |= this->Work[this->inW].SelectedGroup;
            break;
        case 12:
            if (
                #ifdef __THREADS_ENABLE__
                !this->Work[this->inW].win[1]->enable &&
                #endif
                this->Work[this->inW].win[1]->El_t > 0)
            {
                this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].List[this->inW] ^= this->Work[this->inW].SelectedGroup;
                MoveD(1,this);
                if (settings->Win3Enable)
                        FastRun(this);
            }
            break;
        case 13:
            this->Work[this->inW].Visual = !this->Work[this->inW].Visual;
            if (this->Work[this->inW].Visual && this->Work[this->inW].win[1]->El_t > 0)
                this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].List[this->inW] |= this->Work[this->inW].SelectedGroup;
            break;
        case 14:
            if (keys[si].slc2.ll == 0)
            {
                if (keys[si].slc1.ll == -1)
                {
                    for (size_t i = 0; i < this->ActualSize; i++)
                        for (long long int j = 0; j < this->Base[i].El_t; j++)
                            this->Base[i].El[j].List[this->inW] ^= this->Work[this->inW].SelectedGroup;
                }
                else if (keys[si].slc1.ll == 0)
                {
                    for (size_t i = 0; i < this->ActualSize; i++)
                        for (long long int j = 0; j < this->Base[i].El_t; j++)
                            this->Base[i].El[j].List[this->inW] ^= this->Work[this->inW].SelectedGroup*((this->Base[i].El[j].List[this->inW]&this->Work[this->inW].SelectedGroup) == this->Work[this->inW].SelectedGroup);
                }
                else
                {
                    for (size_t i = 0; i < this->ActualSize; i++)
                        for (long long int j = 0; j < this->Base[i].El_t; j++)
                            this->Base[i].El[j].List[this->inW] |= this->Work[this->inW].SelectedGroup;
                }
            }
            else
            {
                if (keys[si].slc1.ll == -1)
                {
                    for (long long int i = 0; i < this->Work[this->inW].win[1]->El_t; i++)
                        this->Work[this->inW].win[1]->El[i].List[this->inW] ^= this->Work[this->inW].SelectedGroup;
                }
                else if (keys[si].slc1.ll == 0)
                {
                    for (long long int i = 0; i < this->Work[this->inW].win[1]->El_t; i++)
                        this->Work[this->inW].win[1]->El[i].List[this->inW] ^= this->Work[this->inW].SelectedGroup*((this->Work[this->inW].win[1]->El[i].List[this->inW]&this->Work[this->inW].SelectedGroup) == this->Work[this->inW].SelectedGroup);
                }
                else
                {
                    for (long long int i = 0; i < this->Work[this->inW].win[1]->El_t; i++)
                        this->Work[this->inW].win[1]->El[i].List[this->inW] |= this->Work[this->inW].SelectedGroup;
                }
            }
            break;
        case 15:
            MoveGroup(this,".",(mode_t)keys[si].slc1.ll);
            UpdateSizeBasic(this);
            CD(".",this);
            break;
        case 16:
            CopyGroup(this,".",(mode_t)keys[si].slc1.ll);
            UpdateSizeBasic(this);
            CD(".",this);
            break;
        case 17:
            DeleteGroup(this,(mode_t)keys[si].slc1.ll);
            UpdateSizeBasic(this);
            CD(".",this);
            break;
        case 27:
            if (keys[si].slc1.ll == 0)
                CD(keys[si].slc2.v,this);
            else
                CD(getenv(keys[si].slc2.v),this);
            break;
    }
}
