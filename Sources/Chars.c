#include "main.h"
#include "Functions.h"
#include "Load.h"
#include "FastRun.h"
#include "Usefull.h"

extern Key keys[];

extern bool Win3Enable;
extern bool Win1Enable;
extern bool Win1Display;
extern bool Borders;
extern unsigned char SortMethod;

void CD(char* path, Basic* this)
{
    if (chdir(path) != 0)
        return;
    
    #ifdef __FILESYSTEM_INFORMATION_ENABLE__
    statfs(".",&this->fs);
    #endif

    for (int i = Borders; i < this->win[1]->_maxy-Borders+1; i++)
            for (int j = Borders; j < this->win[1]->_maxx; j++)
                mvwaddch(this->win[1],i,j,' ');
    wrefresh(this->win[1]);
    GetDir(".",this,1,0);

    if (Win1Enable)
    {
        for (int i = Borders; i < this->win[0]->_maxy-Borders+1; i++)
            for (int j = Borders; j < this->win[0]->_maxx; j++)
                mvwaddch(this->win[0],i,j,' ');
        wrefresh(this->win[0]);
    
        if (this->Work[this->inW].win[1]->path[0] == '/' && this->Work[this->inW].win[1]->path[1] == '\0')
            Win1Display = false;
        else
        {
            GetDir("..",this,0,1);
            Win1Display = true;
        }
    }

    if (Win3Enable)
    {
        if (this->Work[this->inW].win[1]->El_t == 0)
        {
            for (int i = Borders; i < this->win[2]->_maxy-Borders+1; i++)
                for (int j = Borders; j < this->win[2]->_maxx; j++)
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
    short int Event = '\0';

    do {
            if (Event == 410) { UpdateSizeBasic(this); DrawBasic(this,-1); };
            if (Event > 47 && Event < 58) cSF[strlen(cSF)] = Event;
            Event = getch();
        } while (Event == -1 || Event == 410 || (Event > 47 && Event < 58));

    int* theyPass = NULL;
    size_t theyPass_t = 0;

    if ((int)Event == 27) { *result = -1; return; }
    cSF[strlen(cSF)] = Event;

    for (int i = 0; keys[i].keys != NULL; i++)
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

        for (int j = 0; j < theyPass_t; j++)
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
        for (int i = this->Work[this->inW].win[1]->selected[this->inW]; i < this->Work[this->inW].win[1]->El_t; i++)
            this->Work[this->inW].win[1]->El[i].List[this->inW] |= this->Work[this->inW].SelectedGroup;
    }
    if (this->Work[this->inW].win[1]->El_t > this->win[1]->_maxy-!Borders+Borders)
        this->Work[this->inW].win[1]->Ltop[this->inW] = this->Work[this->inW].win[1]->El_t-this->win[1]->_maxy-!Borders+Borders;
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

extern int MoveOffSet;
extern bool WrapScroll;
extern bool JumpScroll;
extern bool JumpScrollValue;

static void MoveD(int how, int multi, Basic* this)
{
    if (how == 1) //down
    {
        if (WrapScroll)
        {
            if (this->Work[this->inW].win[1]->selected[this->inW] == this->Work[this->inW].win[1]->El_t-1)
            {
                GoTop(this);
                return;
            }
        }
        if (this->Work[this->inW].win[1]->El_t-1 > this->Work[this->inW].win[1]->selected[this->inW])
            this->Work[this->inW].win[1]->selected[this->inW]++;
        if (this->win[1]->_maxy+this->Work[this->inW].win[1]->Ltop[this->inW]-(Borders*2) != this->Work[this->inW].win[1]->El_t-1 && this->win[1]->_maxy+this->Work[this->inW].win[1]->Ltop[this->inW]-(Borders*2) < this->Work[this->inW].win[1]->selected[this->inW]+MoveOffSet)
        {
            if (JumpScroll)
            {
                if (this->win[1]->_maxy+this->Work[this->inW].win[1]->Ltop[this->inW]-(Borders*2)+JumpScrollValue > this->Work[this->inW].win[1]->El_t-1)
                    this->Work[this->inW].win[1]->Ltop[this->inW] = this->Work[this->inW].win[1]->El_t-this->win[1]->_maxy-!Borders+Borders;
                else
                    this->Work[this->inW].win[1]->Ltop[this->inW] += JumpScrollValue;
            }
            else
                this->Work[this->inW].win[1]->Ltop[this->inW]++;
        }
    }
    if (how == 2) //up
    {
        if (WrapScroll)
        {
            if (this->Work[this->inW].win[1]->selected[this->inW] == 0)
            {
                GoDown(this);
                return;
            }
        }
        if (0 < this->Work[this->inW].win[1]->selected[this->inW])
            this->Work[this->inW].win[1]->selected[this->inW]--;
        if (this->win[1]->_maxy+this->Work[this->inW].win[1]->Ltop[this->inW]-(Borders*2) != 0 && this->Work[this->inW].win[1]->Ltop[this->inW] > this->Work[this->inW].win[1]->selected[this->inW]-MoveOffSet)
        {
            if (JumpScroll)
            {
                if (this->win[1]->_maxy+this->Work[this->inW].win[1]->Ltop[this->inW]-(Borders*2)-JumpScrollValue < JumpScrollValue)
                    this->Work[this->inW].win[1]->Ltop[this->inW] = 0;
                else
                    this->Work[this->inW].win[1]->Ltop[this->inW] -= JumpScrollValue;
            }
            else
                this->Work[this->inW].win[1]->Ltop[this->inW]--;
        }
    }
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
    switch (keys[si].act)
    {
        case 0:
            ExitBasic(ExitTime,this);
            break;
        case 1:
            if (!this->Work[this->inW].win[1]->enable && this->Work[this->inW].win[1]->El_t > 0)
            {
                MoveD(1,0,this);
                if (this->Work[this->inW].Visual)
                    this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].List[this->inW] |= this->Work[this->inW].SelectedGroup;
                if (Win3Enable)
                    FastRun(this);
            }
            break;
        case 2:
            if (!this->Work[this->inW].win[1]->enable && this->Work[this->inW].win[1]->El_t > 0)
            {
                MoveD(2,0,this);
                if (this->Work[this->inW].Visual)
                    this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].List[this->inW] |= this->Work[this->inW].SelectedGroup;
                if (Win3Enable)
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
            GoTop(this);
            if (Win3Enable)
                FastRun(this);
            break;
        case 6:
            GoDown(this);
            if (Win3Enable)
                FastRun(this);
            break;
        case 8:
            ChangeWorkSpace(this,keys[si].slc1);
            break;
        case 9:
            SortMethod = keys[si].slc1;
            break;
        case 10:
            if (this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].Type == T_DIR ||
                this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].Type == T_LDIR)
            {
                char* temp = (char*)malloc(PATH_MAX);
                int tfd;
                strcpy(temp,this->Work[this->inW].win[1]->path);
                if (!(this->Work[this->inW].win[1]->path[0] == '/' && this->Work[this->inW].win[1]->path[1] == '\0'))
                    strcat(temp,"/");
                strcat(temp,this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].name);

                if (access(temp,R_OK) == 0)
                {
                    tfd = open(temp,O_DIRECTORY);
                    #ifdef __GET_DIR_SIZE_ENABLE__
                    if (((int)keys[si].slc1&D_F) != D_F)
                        this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].size = GetDirSize(tfd,((int)keys[si].slc1&D_R) == D_R,((int)keys[si].slc1&D_C) == D_C);
                    else
                    #endif
                    {
                        struct stat sFile;
                        stat(temp,&sFile);
                        this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].size = sFile.st_size;
                    }
                    #ifdef __HUMAN_READABLE_SIZE_ENABLE__
                    if (this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].SizErrToDisplay == NULL)
                        this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].SizErrToDisplay = (char*)malloc(16);
                    MakeHumanReadAble(this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].SizErrToDisplay
                        ,this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].size,((int)keys[si].slc1&D_H) != D_H);
                    #endif
                    close(tfd);
                }
                free(temp);
            }
            break;
        case 11:
            this->Work[this->inW].SelectedGroup = (unsigned char)keys[si].slc1;
            break;
        case 12:
            if (!this->Work[this->inW].win[1]->enable && this->Work[this->inW].win[1]->El_t > 0)
            {
                this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].List[this->inW] ^= this->Work[this->inW].SelectedGroup;
                MoveD(1,0,this);
                if (Win3Enable)
                        FastRun(this);
            }
            break;
        case 13:
            this->Work[this->inW].Visual = !this->Work[this->inW].Visual;
            if (this->Work[this->inW].Visual)
                this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].List[this->inW] |= this->Work[this->inW].SelectedGroup;
            break;
        case 14:
            if ((int)keys[si].slc2 == 0)
            {
                if ((int)keys[si].slc1 == -1)
                {
                    for (int i = 0; i < this->ActualSize; i++)
                        for (int j = 0; j < this->Base[i].El_t; j++)
                            this->Base[i].El[j].List[this->inW] ^= this->Work[this->inW].SelectedGroup;
                }
                else if ((int)keys[si].slc1 == 0)
                {
                    for (int i = 0; i < this->ActualSize; i++)
                        for (int j = 0; j < this->Base[i].El_t; j++)
                            this->Base[i].El[j].List[this->inW] ^= this->Work[this->inW].SelectedGroup*((this->Base[i].El[j].List[this->inW]&this->Work[this->inW].SelectedGroup) == this->Work[this->inW].SelectedGroup);
                }
                else
                {
                    for (int i = 0; i < this->ActualSize; i++)
                        for (int j = 0; j < this->Base[i].El_t; j++)
                            this->Base[i].El[j].List[this->inW] |= this->Work[this->inW].SelectedGroup;
                }
            }
            else
            {
                if ((int)keys[si].slc1 == -1)
                {
                    for (int i = 0; i < this->Work[this->inW].win[1]->El_t; i++)
                        this->Work[this->inW].win[1]->El[i].List[this->inW] ^= this->Work[this->inW].SelectedGroup;
                }
                else if ((int)keys[si].slc1 == 0)
                {
                    for (int i = 0; i < this->Work[this->inW].win[1]->El_t; i++)
                        this->Work[this->inW].win[1]->El[i].List[this->inW] ^= this->Work[this->inW].SelectedGroup*((this->Work[this->inW].win[1]->El[i].List[this->inW]&this->Work[this->inW].SelectedGroup) == this->Work[this->inW].SelectedGroup);
                }
                else
                {
                    for (int i = 0; i < this->Work[this->inW].win[1]->El_t; i++)
                        this->Work[this->inW].win[1]->El[i].List[this->inW] |= this->Work[this->inW].SelectedGroup;
                }
            }
            break;
        case 27:
            if ((bool)keys[si].slc1 == 0)
                CD(keys[si].slc2,this);
            else
                CD(getenv(keys[si].slc2),this);
            break;
    }
}

