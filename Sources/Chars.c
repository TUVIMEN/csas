#include "main.h"
#include "Functions.h"
#include "Load.h"
#include "FastRun.h"
#include "Usefull.h"

extern Key* keys;
extern size_t keys_t;
extern Settings* settings;

void CD(char* path, Basic* this)
{
    if (chdir(path) != 0)
        return;

    #ifdef __FILESYSTEM_INFORMATION_ENABLE__
    statfs(".",&this->fs);
    #endif

    for (int i = settings->Borders; i < this->win[1]->_maxy-settings->Borders+1; i++)
            for (int j = settings->Borders; j < this->win[1]->_maxx; j++)
                mvwaddch(this->win[1],i,j,' ');
    wrefresh(this->win[1]);
    GetDir(".",this,1,settings->Threads);

    if (settings->Win1Enable)
    {
        for (int i = settings->Borders; i < this->win[0]->_maxy-settings->Borders+1; i++)
            for (int j = settings->Borders; j < this->win[0]->_maxx; j++)
                mvwaddch(this->win[0],i,j,' ');
        wrefresh(this->win[0]);

        if (this->Work[this->inW].win[1]->path[0] == '/' && this->Work[this->inW].win[1]->path[1] == '\0')
            settings->Win1Display = false;
        else
        {
            GetDir("..",this,0,settings->Threads);
            settings->Win1Display = true;
        }
    }

    if (settings->Win3Enable)
    {
        if (this->Work[this->inW].win[1]->El_t == 0)
        {
            for (int i = settings->Borders; i < this->win[2]->_maxy-settings->Borders+1; i++)
                for (int j = settings->Borders; j < this->win[2]->_maxx; j++)
                    mvwaddch(this->win[2],i,j,' ');
            wrefresh(this->win[2]);
        }
        if (!this->Work[this->inW].win[1]->enable && this->Work[this->inW].win[1]->El_t > 0)
            FastRun(this);
    }
}

void ChangeWorkSpace(Basic* this, int num)
{
    int temp = this->inW;
    this->inW = num;

    if (!this->Work[num].exists)
    {
        this->Work[num].exists = 1;
        CD(this->Work[temp].win[1]->path,this);
    }
    else
        CD(this->Work[num].win[1]->path,this);
}

void UpdateEvent(int* result, char* cSF, Basic* this)
{
    memset(cSF,0,64);
    short int Event = getch();
    if (Event == -1) { *result = -1; return; }

    while (Event == -1 || Event == 410 || (Event > 47 && Event < 58))
    {
        if (Event == 410) { UpdateSizeBasic(this); DrawBasic(this,-1); };
        if (Event > 47 && Event < 58) cSF[strlen(cSF)] = Event;
        Event = getch();
    }

    int* theyPass = NULL;
    size_t theyPass_t = 0;

    if ((int)Event == 27) { *result = -1; return; }
    cSF[strlen(cSF)] = Event;

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
        do {
            if (Event == 410) { UpdateSizeBasic(this); DrawBasic(this,-1); };
            if (Event > 47 && Event < 58) cSF[strlen(cSF)] = Event;
            Event = getch();
        } while (Event == -1 || Event == 410 || (StartsTheString && Event > 47 && Event < 58));
        if ((int)Event == 27) { *result = -1; return; }
        cSF[strlen(cSF)] = Event;
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

    *result = (theyPass_t != 0) ? theyPass[0] : -1;
}

void GoDown(Basic* this)
{
    if (this->Work[this->inW].Visual)
    {
        for (size_t i = this->Work[this->inW].win[1]->selected[this->inW]; i < this->Work[this->inW].win[1]->El_t; i++)
            this->Work[this->inW].win[1]->El[i].List[this->inW] |= this->Work[this->inW].SelectedGroup;
    }
    if (this->Work[this->inW].win[1]->El_t > (size_t)this->win[1]->_maxy-!settings->Borders+settings->Borders)
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

static void MoveD(int how, Basic* this)
{
    if (how == 1) //down
    {
        if (settings->WrapScroll)
        {
            if (this->Work[this->inW].win[1]->selected[this->inW] == this->Work[this->inW].win[1]->El_t-1)
            {
                GoTop(this);
                return;
            }
        }
        if (this->Work[this->inW].win[1]->El_t-1 > this->Work[this->inW].win[1]->selected[this->inW])
            this->Work[this->inW].win[1]->selected[this->inW]++;
        if (this->win[1]->_maxy+this->Work[this->inW].win[1]->Ltop[this->inW]-(settings->Borders*2) != this->Work[this->inW].win[1]->El_t-1 && this->win[1]->_maxy+this->Work[this->inW].win[1]->Ltop[this->inW]-(settings->Borders*2) < this->Work[this->inW].win[1]->selected[this->inW]+settings->MoveOffSet)
        {
            if (settings->JumpScroll)
            {
                if (this->win[1]->_maxy+this->Work[this->inW].win[1]->Ltop[this->inW]-(settings->Borders*2)+settings->JumpScrollValue > this->Work[this->inW].win[1]->El_t-1)
                    this->Work[this->inW].win[1]->Ltop[this->inW] = this->Work[this->inW].win[1]->El_t-this->win[1]->_maxy-!settings->Borders+settings->Borders;
                else
                    this->Work[this->inW].win[1]->Ltop[this->inW] += settings->JumpScrollValue;
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
        if (this->win[1]->_maxy+this->Work[this->inW].win[1]->Ltop[this->inW]-(settings->Borders*2) != 0 && this->Work[this->inW].win[1]->Ltop[this->inW] > this->Work[this->inW].win[1]->selected[this->inW]-settings->MoveOffSet)
        {
            if (settings->JumpScroll)
            {
                if (this->win[1]->_maxy+this->Work[this->inW].win[1]->Ltop[this->inW]-(settings->Borders*2)-settings->JumpScrollValue < (size_t)settings->JumpScrollValue)
                    this->Work[this->inW].win[1]->Ltop[this->inW] = 0;
                else
                    this->Work[this->inW].win[1]->Ltop[this->inW] -= settings->JumpScrollValue;
            }
            else
                this->Work[this->inW].win[1]->Ltop[this->inW]--;
        }
    }
    if (this->Work[this->inW].Visual)
        this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].List[this->inW] |= this->Work[this->inW].SelectedGroup;
}

void ExitBasic(bool* ExitTime, Basic* this)
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
        *ExitTime = true;
}

void RunEvent(int si, bool *ExitTime, Basic* this, char* cSF)
{
    int temp;
    switch (keys[si].act)
    {
        case 0:
            ExitBasic(ExitTime,this);
            break;
        case 1:
            if (!this->Work[this->inW].win[1]->enable && this->Work[this->inW].win[1]->El_t > 0)
            {
                temp = atoi(cSF);
                for (int i = 0; i < (temp+(temp == 0))*(keys[si].slc1.ll+(keys[si].slc1.ll == 0)); i++)
                {
                    MoveD(1,this);
                }
                if (settings->Win3Enable)
                    FastRun(this);
            }
            break;
        case 2:
            if (!this->Work[this->inW].win[1]->enable && this->Work[this->inW].win[1]->El_t > 0)
            {
                temp = atoi(cSF);
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
            if (!this->Work[this->inW].win[1]->enable && this->Work[this->inW].win[1]->El_t > 0)
            {
                temp = atoi(cSF);
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
            if (!this->Work[this->inW].win[1]->enable && this->Work[this->inW].win[1]->El_t > 0)
            {
                temp = atoi(cSF);
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
        case 9:
            settings->SortMethod = keys[si].slc1.ll;
            break;
        case 10:
            if (!this->Work[this->inW].win[1]->enable && this->Work[this->inW].win[1]->El_t > 0 && (temp = open(this->Work[this->inW].win[1]->path,O_DIRECTORY)) != -1)
            {
                int tfd;
                int counter = 0;

                for (size_t i = 0; i < this->Work[this->inW].win[1]->El_t; i++)
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
        case 11:
            this->Work[this->inW].SelectedGroup = keys[si].slc1.ll;
            break;
        case 12:
            if (!this->Work[this->inW].win[1]->enable && this->Work[this->inW].win[1]->El_t > 0)
            {
                this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].List[this->inW] ^= this->Work[this->inW].SelectedGroup;
                MoveD(1,this);
                if (settings->Win3Enable)
                        FastRun(this);
            }
            break;
        case 13:
            this->Work[this->inW].Visual = !this->Work[this->inW].Visual;
            if (this->Work[this->inW].Visual)
                this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].List[this->inW] |= this->Work[this->inW].SelectedGroup;
            break;
        case 14:
            if (keys[si].slc2.ll == 0)
            {
                if (keys[si].slc1.ll == -1)
                {
                    for (size_t i = 0; i < this->ActualSize; i++)
                        for (size_t j = 0; j < this->Base[i].El_t; j++)
                            this->Base[i].El[j].List[this->inW] ^= this->Work[this->inW].SelectedGroup;
                }
                else if (keys[si].slc1.ll == 0)
                {
                    for (size_t i = 0; i < this->ActualSize; i++)
                        for (size_t j = 0; j < this->Base[i].El_t; j++)
                            this->Base[i].El[j].List[this->inW] ^= this->Work[this->inW].SelectedGroup*((this->Base[i].El[j].List[this->inW]&this->Work[this->inW].SelectedGroup) == this->Work[this->inW].SelectedGroup);
                }
                else
                {
                    for (size_t i = 0; i < this->ActualSize; i++)
                        for (size_t j = 0; j < this->Base[i].El_t; j++)
                            this->Base[i].El[j].List[this->inW] |= this->Work[this->inW].SelectedGroup;
                }
            }
            else
            {
                if (keys[si].slc1.ll == -1)
                {
                    for (size_t i = 0; i < this->Work[this->inW].win[1]->El_t; i++)
                        this->Work[this->inW].win[1]->El[i].List[this->inW] ^= this->Work[this->inW].SelectedGroup;
                }
                else if (keys[si].slc1.ll == 0)
                {
                    for (size_t i = 0; i < this->Work[this->inW].win[1]->El_t; i++)
                        this->Work[this->inW].win[1]->El[i].List[this->inW] ^= this->Work[this->inW].SelectedGroup*((this->Work[this->inW].win[1]->El[i].List[this->inW]&this->Work[this->inW].SelectedGroup) == this->Work[this->inW].SelectedGroup);
                }
                else
                {
                    for (size_t i = 0; i < this->Work[this->inW].win[1]->El_t; i++)
                        this->Work[this->inW].win[1]->El[i].List[this->inW] |= this->Work[this->inW].SelectedGroup;
                }
            }
            break;
        case 15:
            MoveGroup(this,".",0);
            UpdateSizeBasic(this);
            CD(".",this);
            break;
        case 16:
            CopyGroup(this,".",0);
            UpdateSizeBasic(this);
            CD(".",this);
            break;
        case 17:
            DeleteGroup(this,(bool)keys[si].slc1.ll);
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
