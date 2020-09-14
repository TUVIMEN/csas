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
    short int Event = '\0';

    Event = getch();

    if (Event == -1)
    {
        *result = -1;
        return;
    }

    if (Event == 410)
    {
        UpdateSizeBasic(this);
        *result = -1;
        return;
    }

    int* theyPass = NULL;
    size_t theyPass_t = 0;

    if (strlen(cSF) > 30)
        for (int i = 30; i < strlen(cSF); i++)
            for (int j = 29; j < 64; j++)
                    cSF[j-1] = cSF[j]; 

    if ((int)Event == 27) { cSF[0] = '\0'; *result = -1; return; }
        cSF[strlen(cSF)] = Event;

    for (int i = 0; keys[i].keys != NULL; i++)
    {
        if (cSF[strlen(cSF)-1] == keys[i].keys[0])
        {
            theyPass = (int*)realloc(theyPass,(theyPass_t+1)*sizeof(int));
            theyPass_t++;
            theyPass[theyPass_t-1] = i;
        }
    }    

    for (int i = 1; theyPass_t > 1; i++)
    {
        do { if (Event == 410) UpdateSizeBasic(this); Event = getch(); } while (Event == -1 || Event == 410);
        if ((int)Event == 27) { cSF[0] = '\0'; *result = -1; return; }
        cSF[strlen(cSF)] = Event;

        int* abcs = NULL;
        size_t abcs_t = 0;

        for (int j = 0; j < theyPass_t; j++)
        {
            if (cSF[strlen(cSF)-1] == keys[theyPass[j]].keys[i])
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
    this->Work[this->inW].win[1]->Ltop[this->inW] = this->Work[this->inW].win[1]->El_t-this->win[1]->_maxy-!Borders+Borders;
    this->Work[this->inW].win[1]->selected[this->inW] = this->Work[this->inW].win[1]->El_t-1;
    if (Win3Enable)
        FastRun(this);
}

static void GoTop(Basic* this)
{
    this->Work[this->inW].win[1]->selected[this->inW] = 0;
    this->Work[this->inW].win[1]->Ltop[this->inW] = 0;
    if (Win3Enable)
        FastRun(this);
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

void RunEvent(int si, bool *ExitTime, Basic* this)
{
    switch (keys[si].act)
    {
        case 0:
            *ExitTime = true;
            break;
        case 1:
            if (!this->Work[this->inW].win[1]->enable && this->Work[this->inW].win[1]->El_t > 0)
            {
                MoveD(1,0,this);
                if (Win3Enable)
                    FastRun(this);
            }
            break;
        case 2:
            if (!this->Work[this->inW].win[1]->enable && this->Work[this->inW].win[1]->El_t > 0)
            {
                MoveD(2,0,this);
                if (Win3Enable)
                    FastRun(this);
            }
            break;
        case 3:
            CD("..",this);
            break;
        case 4:
            if (!this->Work[this->inW].win[1]->enable && this->Work[this->inW].win[1]->El_t > 0)
            {
                switch(this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].Type)
                {
                    case T_DIR:
                    case T_LDIR:
                        CD(this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].name,this);
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
            break;
        case 6:
            GoDown(this);
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
        case 27:
            if ((bool)keys[si].slc1 == 0)
                CD(keys[si].slc2,this);
            else
                CD(getenv(keys[si].slc2),this);
            break;
    }
}

