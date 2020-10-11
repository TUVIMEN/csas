#include "main.h"
#include "Draw.h"
#include "Load.h"
#include "FastRun.h"
#include "Usefull.h"
#include "Functions.h"
#include "Console.h"
#include "Loading.h"

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
        CD(grf->Work[temp].win[1]->path,num,grf);
    }
    else
        CD(grf->Work[num].win[1]->path,num,grf);
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

static void GoDown(Basic* grf, const int workspace)
{
    if (grf->Work[workspace].Visual)
    {
        for (long long int i = grf->Work[workspace].win[1]->selected[workspace]; i < grf->Work[workspace].win[1]->El_t; i++)
            grf->Work[workspace].win[1]->El[i].List[workspace] |= grf->Work[workspace].SelectedGroup;
    }
    if (grf->Work[workspace].win[1]->El_t > (long long int)grf->win[1]->_maxy-!settings->Borders+settings->Borders)
        grf->Work[workspace].win[1]->Ltop[workspace] = grf->Work[workspace].win[1]->El_t-grf->win[1]->_maxy-!settings->Borders+settings->Borders;
    else
        grf->Work[workspace].win[1]->Ltop[workspace] = 0;
    grf->Work[workspace].win[1]->selected[workspace] = grf->Work[workspace].win[1]->El_t-1;
}

static void GoTop(Basic* grf, const int workspace)
{
    if (grf->Work[workspace].Visual)
    {
        for (int i = grf->Work[workspace].win[1]->selected[workspace]; i > -1; i--)
            grf->Work[workspace].win[1]->El[i].List[workspace] |= grf->Work[workspace].SelectedGroup;
    }
    grf->Work[workspace].win[1]->selected[workspace] = 0;
    grf->Work[workspace].win[1]->Ltop[workspace] = 0;
}

static void MoveD(const int how, Basic* grf, const int workspace)
{
    if (how == 1) //down
    {
        if (settings->WrapScroll)
        {
            if ((long long int)grf->Work[workspace].win[1]->selected[workspace] == grf->Work[workspace].win[1]->El_t-1)
            {
                GoTop(grf,workspace);
                return;
            }
        }
        if (grf->Work[workspace].win[1]->El_t-1 > (long long int)grf->Work[workspace].win[1]->selected[workspace])
            grf->Work[workspace].win[1]->selected[workspace]++;
        if ((long long int)(grf->win[1]->_maxy+grf->Work[workspace].win[1]->Ltop[workspace]-(settings->Borders*2)) != grf->Work[workspace].win[1]->El_t-1 && grf->win[1]->_maxy+grf->Work[workspace].win[1]->Ltop[workspace]-(settings->Borders*2) < grf->Work[workspace].win[1]->selected[workspace]+(int)(grf->win[1]->_maxy*settings->MoveOffSet))
        {
            if (settings->JumpScroll)
            {
                if ((long long int)(grf->win[1]->_maxy+grf->Work[workspace].win[1]->Ltop[workspace]-(settings->Borders*2)+(int)(grf->win[1]->_maxy*settings->JumpScrollValue)) > grf->Work[workspace].win[1]->El_t-1)
                    grf->Work[workspace].win[1]->Ltop[workspace] = grf->Work[workspace].win[1]->El_t-grf->win[1]->_maxy-!settings->Borders+settings->Borders;
                else
                    grf->Work[workspace].win[1]->Ltop[workspace] += (int)(grf->win[1]->_maxy*settings->JumpScrollValue);
            }
            else
                grf->Work[workspace].win[1]->Ltop[workspace]++;
        }
    }
    if (how == 2) //up
    {
        if (settings->WrapScroll)
        {
            if (grf->Work[workspace].win[1]->selected[workspace] == 0)
            {
                GoDown(grf,workspace);
                return;
            }
        }
        if (0 < grf->Work[workspace].win[1]->selected[workspace])
            grf->Work[workspace].win[1]->selected[workspace]--;
        if (grf->win[1]->_maxy+grf->Work[workspace].win[1]->Ltop[workspace]-(settings->Borders*2) != 0 && grf->Work[workspace].win[1]->Ltop[workspace] > grf->Work[workspace].win[1]->selected[workspace]-(int)(grf->win[1]->_maxy*settings->MoveOffSet))
        {
            if (settings->JumpScroll)
            {
                if (grf->win[1]->_maxy+grf->Work[workspace].win[1]->Ltop[workspace]-(settings->Borders*2)-(int)(grf->win[1]->_maxy*settings->JumpScrollValue) < (size_t)(grf->win[1]->_maxy*settings->JumpScrollValue)*2)
                    grf->Work[workspace].win[1]->Ltop[workspace] = 0;
                else
                    grf->Work[workspace].win[1]->Ltop[workspace] -= (int)(grf->win[1]->_maxy*settings->JumpScrollValue);
            }
            else
                grf->Work[workspace].win[1]->Ltop[workspace]--;
        }
    }
    if (grf->Work[workspace].Visual)
        grf->Work[workspace].win[1]->El[grf->Work[workspace].win[1]->selected[workspace]].List[workspace] |= grf->Work[workspace].SelectedGroup;
}

void ExitBasic(Basic* grf, const bool force)
{
    if (force)
        goto END;

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

    END: ;
    grf->ExitTime = true;
}

void ConsoleRun(Basic* grf)
{
    curs_set(1);

    static char ConsoleBuffer[16384];
    memset(ConsoleBuffer,0,16383);
    int Event;

    int x = 0;

    while (1)
    {
        Event = getch();

        if (Event == '\n')
        {
            RunCommand(ConsoleBuffer,grf);
            break;
        }

        if (Event == -1)
            continue;

        ConsoleBuffer[x] = (char)Event;
        werase(grf->win[4]);
        mvwaddstr(grf->win[4],0,(x-10 > -1)*10,ConsoleBuffer);
        x++;
        wrefresh(grf->win[4]);
    }

    curs_set(0);
}

void ___SET(const char* src, char* temp)
{
    size_t pos = 0;

    struct SetEntry SetEntries[] = {
        {"FileOpener",&settings->FileOpener},
        {"shell",&settings->shell},{"editor",&settings->editor},{"Bar1Settings",&settings->Bar1Settings},
        {"Bar2Settings",&settings->Bar2Settings},{"C_Borders",&settings->C_Borders},
        {"UserHostPattern",&settings->UserHostPattern},{"CopyBufferSize",&settings->CopyBufferSize},
        #ifdef __INOTIFY_ENABLE__
        {"INOTIFY_MASK",&settings->INOTIFY_MASK},{"DirLoadingMode",&settings->DirLoadingMode},
        #endif
        {"MoveOffSet",&settings->MoveOffSet},{"SDelayBetweenFrames",&settings->SDelayBetweenFrames},
        {"WrapScroll",&settings->WrapScroll},{"JumpScrollValue",&settings->JumpScrollValue},
        {"StatusBarOnTop",&settings->StatusBarOnTop},{"WinSizeMod",&settings->WinSizeMod},
        {"Win1Enable",&settings->Win1Enable},{"Win3Enable",&settings->Win3Enable},{"UserRHost",&settings->UserRHost},
        {"Bar1Enable",&settings->Bar1Enable},{"Bar2Enable",&settings->Bar2Enable},
        {"Borders",&settings->Borders},{"FillBlankSpace",&settings->FillBlankSpace},
        {"WindowBorder",&settings->WindowBorder},{"EnableColor",&settings->EnableColor},
        {"DelayBetweenFrames",&settings->DelayBetweenFrames},{"NumberLines",&settings->NumberLines},
        {"NumberLinesOff",&settings->NumberLinesOff},{"NumberLinesFromOne",&settings->NumberLinesFromOne},
        {"DisplayingC",&settings->DisplayingC},{"JumpScroll",&settings->JumpScroll},{"Values",&settings->Values},
        #ifdef __SHOW_HIDDEN_FILES_ENABLE__
        {"ShowHiddenFiles",&settings->ShowHiddenFiles},
        #endif
        #ifdef __SORT_ELEMENTS_ENABLE__
        {"SortMethod",&settings->SortMethod},{"BetterFiles",&settings->BetterFiles},
        #endif
        {"DirSizeMethod",&settings->DirSizeMethod},{"C_Error",&settings->C_Error},
        #ifdef __COLOR_FILES_BY_EXTENSION__
        {"C_FType_A",&settings->C_FType_A},{"C_FType_I",&settings->C_FType_I},{"C_FType_V",&settings->C_FType_V},
        #endif
        {"C_Selected",&settings->C_Selected},{"C_Exec_set",&settings->C_Exec_set},{"C_Exec",&settings->C_Exec},
        {"C_BLink",&settings->C_BLink},{"C_Dir",&settings->C_Dir},{"C_Reg",&settings->C_Reg},{"C_Fifo",&settings->C_Fifo},
        {"C_Sock",&settings->C_Sock},{"C_Dev",&settings->C_Dev},{"C_BDev",&settings->C_BDev},{"C_LDir",&settings->C_LDir},
        {"C_LReg",&settings->C_LReg},{"C_LFifo",&settings->C_LFifo},{"C_LSock",&settings->C_LSock},{"C_LDev",&settings->C_LDev},
        {"C_LBDev",&settings->C_LBDev},{"C_Other",&settings->C_Other},{"C_User_S_D",&settings->C_User_S_D},
        {"C_Bar_Dir",&settings->C_Bar_Dir},{"C_Bar_Name",&settings->C_Bar_Name},{"C_Bar_WorkSpace",&settings->C_Bar_WorkSpace},
        {"C_Bar_WorkSpace_Selected",&settings->C_Bar_WorkSpace_Selected},{"C_Group_0",&settings->C_Group_0},
        {"C_Group_1",&settings->C_Group_1},{"C_Group_2",&settings->C_Group_2},{"C_Group_3",&settings->C_Group_3},
        {"C_Group_4",&settings->C_Group_4},{"C_Group_5",&settings->C_Group_5},{"C_Group_6",&settings->C_Group_6},
        {"C_Group_7",&settings->C_Group_7},{"C_Bar_F",&settings->C_Bar_F},{"C_Bar_E",&settings->C_Bar_E},
        #ifdef __THREADS_FOR_DIR_ENABLE__
        {"ThreadsForDir",&settings->ThreadsForDir},
        #endif
        #ifdef __THREADS_FOR_FILE_ENABLE__
        {"ThreadsForFile",&settings->ThreadsForFile},
        #endif
        {NULL,NULL}
    };

    size_t end = 0;
    while (src[pos+end] && !isspace(src[pos+end])) end++;
    int gga = -1;

    for (int i = 0; SetEntries[i].name; i++)
        if (strlen(SetEntries[i].name) == end && strncmp(src+pos,SetEntries[i].name,end) == 0) { gga = i; break; }

    if (gga == -1) return;

    pos += end;
    pos += FindFirstCharacter(src+pos);

    pos += StrToValue(SetEntries[gga].value,src+pos,temp);
}

void ___INCLUDE(const char* src, char* temp, Basic* grf)
{
    size_t pos = 0, end = 0;

    if (src[pos] != '\'')
        return;
    pos++;
    end = FindEndOf(src+pos,'\'');
    strncpy(temp,src+pos,end);
    LoadConfig(temp,grf);
}

void ___MAP(const char* src, char* temp)
{
    size_t pos = 0, end = 0;
    Key TempKey;
    TempKey.keys = (char*)malloc(64);
    while (src[pos+end] && !isspace(src[pos+end])) end++;
    strncpy(TempKey.keys,src+pos,end);
    TempKey.keys[end] = '\0';
    pos += end;
    StrToKeys(TempKey.keys);

    pos += FindFirstCharacter(src+pos);
    if (src[pos] != '\'')
        return;
    pos++;
    end = FindEndOf(src+pos,'\'');
    strncpy(temp,src+pos,end);
    temp[end] = '\0';
    pos += end;
    TempKey.value = temp;

    addKey(TempKey);
    free(TempKey.keys);
}

void ___MOVE(const char* src, Basic* grf)
{
    size_t pos = 0;
    int workspace = grf->inW, rot = 1, mul1 = 1, mul2 = 0;

    while (src[pos])
    {
        if (src[pos] && src[pos] == '-')
        {
            do {
                pos++;

                switch (src[pos])
                {
                    case 'd':
                        rot = 1;
                        break;
                    case 'u':
                        rot = 2;
                        break;
                    case 'l':
                        rot = 3;
                        break;
                    case 'r':
                        rot = 4;
                        break;
                    case 'c':
                        pos++;
                        pos += FindFirstCharacter(src+pos);
                        mul1 = atoi(src+pos);
                        while (isdigit(src[pos])) pos++;
                        break;
                    case 'w':
                        pos++;
                        pos += FindFirstCharacter(src+pos);
                        workspace = atoi(src+pos);
                        while (isdigit(src[pos])) pos++;
                        break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }
        pos++;
    }

    if (!grf->Work[workspace].exists)
        return;

    switch (rot)
    {
        case 1:
        case 2:
            if (
                #ifdef __THREADS_FOR_DIR_ENABLE__
                !grf->Work[workspace].win[1]->enable &&
                #endif
                grf->Work[workspace].win[1]->El_t > 0)
            {
                mul2 = atoi(grf->cSF);
                for (int i = 0; i < (mul2+(mul2 == 0))*mul1; i++)
                    MoveD(rot,grf,workspace);
                if (settings->Win3Enable)
                    FastRun(grf);
            }
            break;
        case 3:
            CD("..",workspace,grf);
            grf->Work[workspace].Visual = 0;
            break;
        case 4:
            if (grf->Work[workspace].win[1]->El_t > 0)
            {
                switch (grf->Work[workspace].win[1]->El[grf->Work[workspace].win[1]->selected[workspace]].Type)
                {
                    case T_DIR:
                    case T_LDIR:
                        CD(grf->Work[workspace].win[1]->El[grf->Work[workspace].win[1]->selected[workspace]].name,workspace,grf);
                        grf->Work[workspace].Visual = 0;
                        break;
                    case T_REG:
                    case T_LREG:
                        RunFile(grf->Work[workspace].win[1]->El[grf->Work[workspace].win[1]->selected[workspace]].name);
                        break;
                }
            }
            break;
    }
}

void ___QUIT(const char* src, Basic* grf)
{
    bool force = false;
    if (src[0] && src[0] == '-' && src[1] && src[1] == 'f')
        force = true;
    ExitBasic(grf,force);
}

void ___CD(const char* src, char* temp, Basic* grf)
{
    size_t pos = 0, x = 0, end;
    int workspace = grf->inW;

    while (src[pos])
    {
        if (src[pos] == '-' && src[pos+1] && src[pos+1] == 'w')
        {
            pos += 2;
            pos += FindFirstCharacter(src+pos);
            workspace = atoi(src+pos);
            while (isdigit(src[pos])) pos++;
        }
        else if (src[pos] == '\'')
        {
            pos++;
            end = FindEndOf(src+pos,'\'');
            strncpy(temp,src+pos,end);
            temp[end] = '\0';
            pos += end;
        }
        else
        {
            while (src[pos] && !isspace(src[pos]))
            {
                if (src[pos] == '\\')
                    temp[x] = src[++pos];
                else
                    temp[x] = src[pos];
                x++;
                pos++;
            }
            temp[x] = '\0';
        }

        pos += FindFirstCharacter(src+pos);
    }

    grf->Work[workspace].exists = true;
    CD(temp,workspace,grf);
}

void ___GOTOP(const char* src, Basic* grf)
{
    size_t pos = 0, target = atol(grf->cSF);
    int workspace = grf->inW;

    while (src[pos])
    {
        if (src[pos] == '-')
        {
            do {
                pos++;
                
                switch (src[pos])
                {
                    case 't':
                        pos++;
                        pos += FindFirstCharacter(src+pos);
                        target = atol(src+pos);
                        while (isdigit(src[pos])) pos++;
                        break;
                    case 'w':
                        pos++;
                        pos += FindFirstCharacter(src+pos);
                        workspace = atoi(src+pos);
                        while (isdigit(src[pos])) pos++;
                        break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }

        pos += FindFirstCharacter(src+pos);
        if (src[pos+1] == '\0')
            break;
    }

    if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        !grf->Work[workspace].win[1]->enable &&
        #endif
        grf->Work[workspace].win[1]->El_t > 0)
    {
        if (target == 0)
            GoTop(grf,workspace);
        else if (target > grf->Work[workspace].win[1]->selected[workspace])
        {
            for (size_t i = grf->Work[workspace].win[1]->selected[workspace]; i < target-1; i++)
                MoveD(1,grf,workspace);
        }
        else if (target < grf->Work[workspace].win[1]->selected[workspace])
        {
            for (size_t i = grf->Work[workspace].win[1]->selected[workspace]; i > target-1; i--)
                MoveD(2,grf,workspace);
        }
        if (workspace == grf->inW && settings->Win3Enable)
            FastRun(grf);
    }
}

void ___GODOWN(const char* src, Basic* grf)
{
    size_t pos = 0, target = atol(grf->cSF);
    int workspace = grf->inW;

    while (src[pos])
    {
        if (src[pos] == '-')
        {
            do {
                pos++;
                
                switch (src[pos])
                {
                    case 't':
                        pos++;
                        pos += FindFirstCharacter(src+pos);
                        target = atol(src+pos);
                        while (isdigit(src[pos])) pos++;
                        break;
                    case 'w':
                        pos++;
                        pos += FindFirstCharacter(src+pos);
                        workspace = atoi(src+pos);
                        while (isdigit(src[pos])) pos++;
                        break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }

        pos += FindFirstCharacter(src+pos);
        if (src[pos+1] == '\0')
            break;
    }

    if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        !grf->Work[workspace].win[1]->enable &&
        #endif
        grf->Work[workspace].win[1]->El_t > 0)
    {
        if (target == 0)
            GoDown(grf,target);
        else if (target > grf->Work[workspace].win[1]->selected[workspace])
        {
            for (size_t i = grf->Work[workspace].win[1]->selected[workspace]; i < target-1; i++)
                MoveD(1,grf,workspace);
        }
        else if (target < grf->Work[workspace].win[1]->selected[workspace])
        {
            for (size_t i = grf->Work[workspace].win[1]->selected[workspace]; i > target-1; i--)
                MoveD(2,grf,workspace);
        }
        if (workspace == grf->inW && settings->Win3Enable)
            FastRun(grf);
    }
}

void ___CHANGEWORKSPACE(const char* src, Basic* grf)
{
    ChangeWorkSpace(grf,atoi(src));
}

#ifdef __GET_DIR_SIZE_ENABLE__
void ___GETSIZE(const char* src, Basic* grf)
{
    size_t pos = 0;
    bool File = 0
        ,Count = 0
        #ifdef __HUMAN_READABLE_SIZE_ENABLE__
        ,HumanReadAble = 0
        #endif
        ,Recursive = 0;

    while (src[pos])
    {
        if (src[pos] == '-')
        {
            do {
                pos++;
                switch (src[pos])
                {
                    case 'r':
                        Recursive = 1;
                        break;
                    case 'c':
                        Count = 1;
                        break;
                    case 'f':
                        File = 1;
                        break;
                    #ifdef __HUMAN_READABLE_SIZE_ENABLE__
                    case 'h':
                        HumanReadAble = 1;
                        break;
                    #endif
                }
            } while (src[pos] && !isspace(src[pos]));
        }

        pos += FindFirstCharacter(src+pos);
        if (src[pos+1] == '\0')
            break;
    }

    int temp;
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
                            if (!File)
                                grf->Work[grf->inW].win[1]->El[i].size = GetDirSize(tfd,Recursive,Count);
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
                                ,grf->Work[grf->inW].win[1]->El[i].size,HumanReadAble);
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
                    if (!File)
                        grf->Work[grf->inW].win[1]->El[grf->Work[grf->inW].win[1]->selected[grf->inW]].size = GetDirSize(tfd,Recursive,Count);
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
                        ,grf->Work[grf->inW].win[1]->El[grf->Work[grf->inW].win[1]->selected[grf->inW]].size,HumanReadAble);
                    #endif
                    close(tfd);
                }
            }
        }
        close(temp);
    }
}
#endif

void ___SETGROUP(const char* src, Basic* grf)
{
    grf->Work[grf->inW].SelectedGroup = atoi(src);
    if (grf->Work[grf->inW].Visual && grf->Work[grf->inW].win[1]->El_t > 0)
        grf->Work[grf->inW].win[1]->El[grf->Work[grf->inW].win[1]->selected[grf->inW]].List[grf->inW] |= grf->Work[grf->inW].SelectedGroup;
}

void ___FASTSELECT(const char* src, Basic* grf)
{
    if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        !grf->Work[grf->inW].win[1]->enable &&
        #endif
        grf->Work[grf->inW].win[1]->El_t > 0)
    {
        grf->Work[grf->inW].win[1]->El[grf->Work[grf->inW].win[1]->selected[grf->inW]].List[grf->inW] ^= grf->Work[grf->inW].SelectedGroup;
        MoveD(1,grf,grf->inW);
        if (settings->Win3Enable)
            FastRun(grf);
    }
}

void ___TOGGLEVISUAL(Basic* grf)
{
    grf->Work[grf->inW].Visual = !grf->Work[grf->inW].Visual;
    if (grf->Work[grf->inW].Visual && grf->Work[grf->inW].win[1]->El_t > 0)
        grf->Work[grf->inW].win[1]->El[grf->Work[grf->inW].win[1]->selected[grf->inW]].List[grf->inW] |= grf->Work[grf->inW].SelectedGroup;
}

void ___F_COPY(const char* src, Basic* grf)
{
    size_t pos = 0;
    mode_t arg = 0;

    while (src[pos])
    {
        if (src[pos] == '-')
        {
            do {
                pos++;

                switch (src[pos])
                {
                    case 'c':
                        arg |= M_CHNAME;
                        break;
                    case 'r':
                        arg |= M_REPLACE;
                        break;
                    case 'd':
                        arg |= M_DCPY;
                        break;
                    case 'm':
                        arg |= M_MERGE;
                        break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }

        pos += FindFirstCharacter(src+pos);
        if (src[pos+1] == '\0')
            break;
    }

    CopyGroup(grf,".",arg);
    UpdateSizeBasic(grf);
    CD(".",grf->inW,grf);
}

void ___F_MOVE(const char* src, Basic* grf)
{
    size_t pos = 0;
    mode_t arg = 0;

    while (src[pos])
    {
        if (src[pos] == '-')
        {
            do {
                pos++;

                switch (src[pos])
                {
                    case 'c':
                        arg |= M_CHNAME;
                        break;
                    case 'r':
                        arg |= M_REPLACE;
                        break;
                    case 'd':
                        arg |= M_DCPY;
                        break;
                    case 'm':
                        arg |= M_MERGE;
                        break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }

        pos += FindFirstCharacter(src+pos);
        if (src[pos+1] == '\0')
            break;
    }

    MoveGroup(grf,".",arg);
    UpdateSizeBasic(grf);
    CD(".",grf->inW,grf);
}

void ___F_DELETE(const char* src, Basic* grf)
{
    size_t pos = 0;
    bool here = 1;

    while (src[pos])
    {
        if (src[pos] == '-')
        {
            do {
                pos++;

                switch (src[pos])
                {
                    case 'a':
                        here = 0;
                        break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }

        pos += FindFirstCharacter(src+pos);
        if (src[pos+1] == '\0')
            break;
    }

    DeleteGroup(grf,here);
    UpdateSizeBasic(grf);
    CD(".",grf->inW,grf);
}

void ___LOAD(const char* src)
{

}

void ___SELECT(const char* src, Basic* grf)
{
    size_t pos = 0;
    int mode = 1;
    bool here = 1;

    while (src[pos])
    {
        if (src[pos] == '-')
        {
            do {
                pos++;

                switch (src[pos])
                {
                    case 'a':
                        here = 0;
                        break;
                    case 'e':
                        mode = 1;
                        break;
                    case 'd':
                        mode = 0;
                        break;
                    case 't':
                        mode = -1;
                        break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }

        pos += FindFirstCharacter(src+pos);
        if (src[pos+1] == '\0')
            break;
    }

    if (!here)
    {
        if (mode == -1)
        {
            for (size_t i = 0; i < grf->ActualSize; i++)
                for (long long int j = 0; j < grf->Base[i].El_t; j++)
                    grf->Base[i].El[j].List[grf->inW] ^= grf->Work[grf->inW].SelectedGroup;
        }
        else if (mode == 0)
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
        if (mode == -1)
        {
            for (long long int i = 0; i < grf->Work[grf->inW].win[1]->El_t; i++)
                grf->Work[grf->inW].win[1]->El[i].List[grf->inW] ^= grf->Work[grf->inW].SelectedGroup;
        }
        else if (mode == 0)
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
}

void ___EXEC(const char* src)
{

}





