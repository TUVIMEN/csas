#include "main.h"
#include "Functions.h"
#include "Usefull.h"
#include "Sort.h"
#include "Chars.h"
#include "Load.h"
#include "FastRun.h"
#include "Draw.h"

#include "config.h"

#ifdef __LOAD_CONFIG_ENABLE__
#include "Loading.h"
#endif

void addKey(Key grf)
{
    if (keys_t == keys_a)
    {
        keys = (Key*)realloc(keys,(keys_a+=32)*sizeof(Key));
        for (size_t i = keys_t; i < keys_a; i++)
            keys[i].keys = (char*)malloc(64);
    }

    long int found = -1;

    for (size_t i = 0; i < keys_t; i++)
    {
        if (strcmp(grf.keys,keys[i].keys) == 0)
        {
            found = (long int)i;
            break;
        }
    }

    if (found == -1)
        found = (long int)keys_t++;

    strcpy(keys[found].keys,grf.keys);
    keys[found].act = grf.act;
    keys[found].slc[0] = grf.slc[0];
    keys[found].slc[1] = grf.slc[1];
}

void KeyInit()
{
    addKey((Key){"q",0,.slc[0].v=NULL,.slc[1].v=NULL});
    addKey((Key){"j",1,.slc[0].ll=1,.slc[1].v=NULL});
    addKey((Key){"J",1,.slc[0].ll=16,.slc[1].v=NULL});
    addKey((Key){"k",2,.slc[0].ll=1,.slc[1].v=NULL});
    addKey((Key){"K",2,.slc[0].ll=16,.slc[1].v=NULL});
    addKey((Key){"h",3,.slc[0].v=NULL,.slc[1].v=NULL});
    addKey((Key){"l",4,.slc[0].v=NULL,.slc[1].v=NULL});
    addKey((Key){"gff",27,.slc[0].ll=0,.slc[1].v="/run/media/kpp/fil"}); //0 - is env ; 1 - path/env
    addKey((Key){"gfm",27,.slc[0].ll=0,.slc[1].v="/run/media/kpp/f/Muzyka"});
    addKey((Key){"gfk",27,.slc[0].ll=0,.slc[1].v="/run/media/kpp/f/ksiegi"});
    addKey((Key){"gh",27,.slc[0].ll=1,.slc[1].v="HOME"});
    addKey((Key){"g/",27,.slc[0].ll=0,.slc[1].v="/"});
    addKey((Key){"gd",27,.slc[0].ll=0,.slc[1].v="/dev"});
    addKey((Key){"ge",27,.slc[0].ll=0,.slc[1].v="/etc"});
    addKey((Key){"gm",27,.slc[0].ll=1,.slc[1].v="MEDIA"}); //variable
    addKey((Key){"gM",27,.slc[0].ll=0,.slc[1].v="/mnt"});
    addKey((Key){"go",27,.slc[0].ll=0,.slc[1].v="/opt"});
    addKey((Key){"gs",27,.slc[0].ll=0,.slc[1].v="/srv"});
    addKey((Key){"gp",27,.slc[0].ll=0,.slc[1].v="/tmp"});
    addKey((Key){"gu",27,.slc[0].ll=0,.slc[1].v="/usr"});
    addKey((Key){"gv",27,.slc[0].ll=0,.slc[1].v="/var"});
    addKey((Key){"gg",5,.slc[0].v=NULL,.slc[1].v=NULL});
    addKey((Key){"G",6,.slc[0].v=NULL,.slc[1].v=NULL});
    addKey((Key){"z1",8,.slc[0].ll=0,.slc[1].v=NULL});
    addKey((Key){"z2",8,.slc[0].ll=1,.slc[1].v=NULL});
    addKey((Key){"z3",8,.slc[0].ll=2,.slc[1].v=NULL});
    addKey((Key){"z4",8,.slc[0].ll=3,.slc[1].v=NULL});
    addKey((Key){"z5",8,.slc[0].ll=4,.slc[1].v=NULL});
    addKey((Key){"z6",8,.slc[0].ll=5,.slc[1].v=NULL});
    addKey((Key){"z7",8,.slc[0].ll=6,.slc[1].v=NULL});
    addKey((Key){"z8",8,.slc[0].ll=7,.slc[1].v=NULL});
    addKey((Key){"z9",8,.slc[0].ll=8,.slc[1].v=NULL});
    addKey((Key){"z0",8,.slc[0].ll=9,.slc[1].v=NULL});
    #ifdef __SORT_ELEMENTS_ENABLE__
    addKey((Key){"oe",9,.slc[0].ll=SORT_NONE,.slc[1].v=NULL});
    addKey((Key){"oE",9,.slc[0].ll=SORT_NONE|SORT_REVERSE,.slc[1].v=NULL});
    addKey((Key){"or",9,.slc[0].ll=SORT_TYPE,.slc[1].v=NULL});
    addKey((Key){"oR",9,.slc[0].ll=SORT_TYPE|SORT_REVERSE,.slc[1].v=NULL});
    addKey((Key){"ob",9,.slc[0].ll=SORT_CHIR,.slc[1].v=NULL});
    addKey((Key){"oB",9,.slc[0].ll=SORT_CHIR|SORT_REVERSE,.slc[1].v=NULL});
    addKey((Key){"os",9,.slc[0].ll=SORT_SIZE,.slc[1].v=NULL});
    addKey((Key){"oS",9,.slc[0].ll=SORT_SIZE|SORT_REVERSE,.slc[1].v=NULL});
    addKey((Key){"otm",9,.slc[0].ll=SORT_MTIME,.slc[1].v=NULL});
    addKey((Key){"otM",9,.slc[0].ll=SORT_MTIME|SORT_REVERSE,.slc[1].v=NULL});
    addKey((Key){"otc",9,.slc[0].ll=SORT_CTIME,.slc[1].v=NULL});
    addKey((Key){"otC",9,.slc[0].ll=SORT_CTIME|SORT_REVERSE,.slc[1].v=NULL});
    addKey((Key){"ota",9,.slc[0].ll=SORT_ATIME,.slc[1].v=NULL});
    addKey((Key){"otA",9,.slc[0].ll=SORT_ATIME|SORT_REVERSE,.slc[1].v=NULL});
    addKey((Key){"og",9,.slc[0].ll=SORT_GID,.slc[1].v=NULL});
    addKey((Key){"oG",9,.slc[0].ll=SORT_GID|SORT_REVERSE,.slc[1].v=NULL});
    addKey((Key){"ou",9,.slc[0].ll=SORT_UID,.slc[1].v=NULL});
    addKey((Key){"oU",9,.slc[0].ll=SORT_UID|SORT_REVERSE,.slc[1].v=NULL});
    addKey((Key){"om",9,.slc[0].ll=SORT_LNAME,.slc[1].v=NULL});
    addKey((Key){"oM",9,.slc[0].ll=SORT_LNAME|SORT_REVERSE,.slc[1].v=NULL});
    addKey((Key){"on",9,.slc[0].ll=SORT_NAME,.slc[1].v=NULL});
    addKey((Key){"oN",9,.slc[0].ll=SORT_NAME|SORT_REVERSE,.slc[1].v=NULL});
    #endif
    #ifdef __GET_DIR_SIZE_ENABLE__
    addKey((Key){"dch",10,.slc[0].ll=D_C,.slc[1].v=NULL});
    addKey((Key){"dCh",10,.slc[0].ll=D_C|D_R,.slc[1].v=NULL});
    addKey((Key){"dsh",10,.slc[0].ll=0,.slc[1].v=NULL});
    addKey((Key){"dSh",10,.slc[0].ll=D_R,.slc[1].v=NULL});
    addKey((Key){"dfh",10,.slc[0].ll=D_F,.slc[1].v=NULL});
    #ifdef __HUMAN_READABLE_SIZE_ENABLE__
    addKey((Key){"dcH",10,.slc[0].ll=D_C|D_H,.slc[1].v=NULL});
    addKey((Key){"dCH",10,.slc[0].ll=D_C|D_R|D_H,.slc[1].v=NULL});
    addKey((Key){"dsH",10,.slc[0].ll=D_H,.slc[1].v=NULL});
    addKey((Key){"dSH",10,.slc[0].ll=D_R|D_H,.slc[1].v=NULL});
    addKey((Key){"dfH",10,.slc[0].ll=D_F|D_H,.slc[1].v=NULL});
    #endif
    #endif
    addKey((Key){"x1",11,.slc[0].ll=GROUP_0,.slc[1].v=NULL});
    addKey((Key){"x2",11,.slc[0].ll=GROUP_1,.slc[1].v=NULL});
    addKey((Key){"x3",11,.slc[0].ll=GROUP_2,.slc[1].v=NULL});
    addKey((Key){"x4",11,.slc[0].ll=GROUP_3,.slc[1].v=NULL});
    addKey((Key){"x5",11,.slc[0].ll=GROUP_4,.slc[1].v=NULL});
    addKey((Key){"x6",11,.slc[0].ll=GROUP_5,.slc[1].v=NULL});
    addKey((Key){"x7",11,.slc[0].ll=GROUP_6,.slc[1].v=NULL});
    addKey((Key){"x8",11,.slc[0].ll=GROUP_7,.slc[1].v=NULL});
    addKey((Key){" ",12,.slc[0].v=NULL,.slc[1].v=NULL});
    addKey((Key){"V",13,.slc[0].v=NULL,.slc[1].v=NULL});
    addKey((Key){"vta",14,.slc[0].ll=-1,.slc[1].ll=0});
    addKey((Key){"vth",14,.slc[0].ll=-1,.slc[1].ll=1});
    addKey((Key){"vda",14,.slc[0].ll=0,.slc[1].ll=0});
    addKey((Key){"vdh",14,.slc[0].ll=0,.slc[1].ll=1});
    addKey((Key){"vea",14,.slc[0].ll=1,.slc[1].ll=0});
    addKey((Key){"veh",14,.slc[0].ll=1,.slc[1].ll=1});
    addKey((Key){"mm",15,.slc[0].ll=M_CHNAME,.slc[1].v=NULL});
    addKey((Key){"mr",15,.slc[0].ll=M_REPLACE,.slc[1].v=NULL});
    addKey((Key){"md",15,.slc[0].ll=M_DCPY,.slc[1].v=NULL});
    addKey((Key){"mM",15,.slc[0].ll=M_CHNAME|M_MERGE,.slc[1].v=NULL});
    addKey((Key){"mR",15,.slc[0].ll=M_REPLACE|M_MERGE,.slc[1].v=NULL});
    addKey((Key){"mD",15,.slc[0].ll=M_DCPY|M_MERGE,.slc[1].v=NULL});
    addKey((Key){"pp",16,.slc[0].ll=M_CHNAME,.slc[1].v=NULL});
    addKey((Key){"pr",16,.slc[0].ll=M_REPLACE,.slc[1].v=NULL});
    addKey((Key){"pd",16,.slc[0].ll=M_DCPY,.slc[1].v=NULL});
    addKey((Key){"pP",16,.slc[0].ll=M_CHNAME|M_MERGE,.slc[1].v=NULL});
    addKey((Key){"pR",16,.slc[0].ll=M_REPLACE|M_MERGE,.slc[1].v=NULL});
    addKey((Key){"pD",16,.slc[0].ll=M_DCPY|M_MERGE,.slc[1].v=NULL});
    addKey((Key){"dd",17,.slc[0].ll=1,.slc[1].v=NULL});
    addKey((Key){"dD",17,.slc[0].ll=0,.slc[1].v=NULL});
}

Settings* SettingsInit()
{
    Settings* grf = (Settings*)malloc(sizeof(Settings));

    #ifdef __THREADS_FOR_DIR_ENABLE__
    grf->ThreadsForDir = 0;
    #endif
    #ifdef __THREADS_FOR_FILE_ENABLE__
    grf->ThreadsForFile = 0;
    #endif
    grf->FileOpener                    = (char*)malloc(PATH_MAX);
    strcpy(grf->FileOpener,"NULL");
    grf->shell                         = (char*)malloc(PATH_MAX);
    strcpy(grf->shell,"/bin/sh");
    grf->editor                        = (char*)malloc(PATH_MAX);
    strcpy(grf->editor,"/bin/nvim");
    grf->Values                        = (char*)malloc(PATH_MAX);
    strcpy(grf->Values,"BKMGTPEZY");
    grf->Bar1Settings                  = B_UHNAME | B_DIR | B_NAME | B_WORKSPACES | B_POSITION | B_FHBFREE | B_FGROUP | B_MODES | B_CSF;
    grf->Bar2Settings                  = DP_LSPERMS | DP_SMTIME | DP_PWNAME | DP_GRNAME;
    grf->UserHostPattern               = (char*)malloc(PATH_MAX);
    strcpy(grf->UserHostPattern,"%s@%s");
    grf->UserRHost                     = false;
    grf->CopyBufferSize                = 131072;
    #ifdef __INOTIFY_ENABLE__
    grf->INOTIFY_MASK                  = IN_DELETE | IN_DELETE_SELF | IN_CREATE | IN_MOVE | IN_MOVE_SELF;
    #endif
    grf->MoveOffSet                    = 0.1;
    grf->WrapScroll                    = false;
    grf->JumpScroll                    = false;
    grf->JumpScrollValue               = 16;
    grf->StatusBarOnTop                = false;
    grf->Win1Enable                    = true;
    grf->Win3Enable                    = true;
    grf->Bar1Enable                    = true;
    grf->Bar2Enable                    = true;
    grf->WinSizeMod                    = (double*)malloc(2*sizeof(double));
    grf->WinSizeMod[0]                 = 0.132f;
    grf->WinSizeMod[1]                 = 0.368f;
    grf->Borders                       = false;
    grf->FillBlankSpace                = true;
    grf->WindowBorder                  = (long int*)malloc(8*sizeof(long int));
    grf->WindowBorder[0]               = 0;
    grf->WindowBorder[1]               = 0;
    grf->WindowBorder[2]               = 0;
    grf->WindowBorder[3]               = 0;
    grf->WindowBorder[4]               = 0;
    grf->WindowBorder[5]               = 0;
    grf->WindowBorder[6]               = 0;
    grf->WindowBorder[7]               = 0;
    grf->EnableColor                   = true;
    grf->DelayBetweenFrames            = 8;
    grf->SDelayBetweenFrames           = 1;
    grf->NumberLines                   = false;
    grf->NumberLinesOff                = false;
    grf->NumberLinesFromOne            = false;
    grf->DirLoadingMode                = 1;
    grf->DisplayingC                   = DP_HSIZE;
    #ifdef __SHOW_HIDDEN_FILES_ENABLE__
    grf->ShowHiddenFiles               = true;
    #endif
    #ifdef __SORT_ELEMENTS_ENABLE__
    grf->SortMethod                    = SORT_NAME;
    grf->BetterFiles                   = (long int*)calloc(24,sizeof(long int));
    grf->BetterFiles[0]                = T_DIR;
    grf->BetterFiles[1]                = T_LDIR;
    #endif
    grf->DirSizeMethod                 = D_C;
    grf->C_Error                       = COLOR_PAIR(4) | A_BOLD | A_REVERSE;
    #ifdef __COLOR_FILES_BY_EXTENSION__
    grf->C_FType_A                     = COLOR_PAIR(4);
    grf->C_FType_I                     = COLOR_PAIR(2);
    grf->C_FType_V                     = COLOR_PAIR(6);
    #endif
    grf->C_Selected		            = A_REVERSE | A_BOLD;
    grf->C_Exec_set		            = A_BOLD;
    grf->C_Exec		                = COLOR_PAIR(10);
    grf->C_BLink		                = COLOR_PAIR(1);
    grf->C_Dir		                    = COLOR_PAIR(1) | A_BOLD;
    grf->C_Reg		                    = A_NORMAL;
    grf->C_Fifo		                = COLOR_PAIR(9) | A_ITALIC;
    grf->C_Sock		                = COLOR_PAIR(9) | A_ITALIC;
    grf->C_Dev		                    = COLOR_PAIR(9);
    grf->C_BDev		                = COLOR_PAIR(9);
    grf->C_LDir		                = COLOR_PAIR(5) | A_BOLD;
    grf->C_LReg		                = COLOR_PAIR(5);
    grf->C_LFifo		                = COLOR_PAIR(5);
    grf->C_LSock		                = COLOR_PAIR(5);
    grf->C_LDev		                = COLOR_PAIR(5);
    grf->C_LBDev		                = COLOR_PAIR(5);
    grf->C_Other		                = COLOR_PAIR(0);
    grf->C_User_S_D		            = COLOR_PAIR(6) | A_BOLD;
    grf->C_Bar_Dir		                = COLOR_PAIR(1) | A_BOLD;
    grf->C_Bar_Name		            = A_NORMAL | A_BOLD;
    grf->C_Bar_WorkSpace		        = A_NORMAL | A_BOLD;
    grf->C_Bar_WorkSpace_Selected	    = COLOR_PAIR(6) | A_REVERSE | A_BOLD;
    grf->C_Group_0		                = COLOR_PAIR(2);
    grf->C_Group_1		                = COLOR_PAIR(1);
    grf->C_Group_2		                = COLOR_PAIR(7);
    grf->C_Group_3		                = COLOR_PAIR(4);
    grf->C_Group_4		                = COLOR_PAIR(5);
    grf->C_Group_5		                = COLOR_PAIR(6);
    grf->C_Group_6		                = COLOR_PAIR(9);
    grf->C_Group_7		                = COLOR_PAIR(10);
    grf->C_Bar_E                       = 0;
    grf->C_Bar_F                       = 0;

    return grf;
}

Basic* InitBasic()
{
    Basic* grf = (Basic*)malloc(sizeof(Basic));

    #ifdef __FILESYSTEM_INFORMATION_ENABLE__
    statfs(".",&grf->fs);
    #endif

    grf->ExitTime = false;
    grf->cSF_E = false;
    grf->cSF = (char*)malloc(64);

    KeyInit();
    settings = SettingsInit();

    #ifdef __LOAD_CONFIG_ENABLE__
    LoadConfig("/etc/.csasrc");
    char* HomeTemp = getenv("HOME");
    if (HomeTemp != NULL)
    {
        char* temp = (char*)malloc(PATH_MAX);
        sprintf(temp,"%s/.csasrc",HomeTemp);
        LoadConfig(temp);
        sprintf(temp,"%s/.config/csas/.csasrc",HomeTemp);
        LoadConfig(temp);
        free(temp);
    }
    #endif

    settings->Win1Display = settings->Win1Enable;
    settings->Win3Display = settings->Win3Enable;

    grf->preview_fd = -1;

    initscr();

    if (has_colors() && settings->EnableColor)
    {
        start_color();
        use_default_colors();

        for (int i = 0; i < 16; i++)
            init_pair(i,i,-1);
    }

    for (int i = 0; i < 5; i++)
        grf->win[i] = newwin(0,0,0,0);

    grf->WinMiddle = 0;

    grf->ActualSize = 0;
    grf->AllocatedSize = 0;
    grf->Base = NULL;
    grf->FastRunSettings = 0;

    #ifdef __USER_NAME_ENABLE__
    grf->H_Host = (char*)malloc(64);
    grf->H_User = (char*)malloc(64);
    getlogin_r(grf->H_User,31);
    gethostname(grf->H_Host,31);
    #endif

    for (int i = 0; i < WORKSPACE_N; i++)
    {
        grf->Work[i].exists = 0;
        grf->Work[i].SelectedGroup = GROUP_0;
        grf->Work[i].Visual = 0;
        for (int j = 0; j < 3; j++)
        {
            grf->Work[i].win[j] = NULL;
        }
    }

    grf->inW = 0;

    grf->Work[grf->inW].exists = 1;

    UpdateSizeBasic(grf);

    endwin();
    return grf;
}

void SetDelay(long int del)
{
    if (del > -1)
        halfdelay(del);
    else
        nodelay(stdscr,true); //!
}

void RunBasic(Basic* grf, const int argc, char** argv)
{
    initscr();
    noecho();

    curs_set(0);
    keypad(stdscr,TRUE);
    SetDelay(settings->DelayBetweenFrames);

    time_t ActualTime, PastTime = 0;
    struct timespec MainTimer;

    int si;

    // Error
    // loading many directories from first loaded directory after loading directories
    // from other directory causes Segmentation fault
    // So i load the main directory, because he's in most cases small

    if (chdir(argv[1]) != 0)
    {
        endwin();
        fprintf(stderr,"Error while entering: %s\n",argv[1]);
        fflush(stderr);
        return;
    }

    CD(".",grf);

    do {
        clock_gettime(1,&MainTimer);
        ActualTime = MainTimer.tv_sec;

        #ifdef __THREADS_FOR_DIR_ENABLE__
        if (grf->Work[grf->inW].win[2] != NULL && grf->Work[grf->inW].win[0] != NULL)
        {
            if (grf->Work[grf->inW].win[0]->enable || grf->Work[grf->inW].win[2]->enable)
                SetDelay(settings->SDelayBetweenFrames);
            else
                SetDelay(settings->DelayBetweenFrames);
        }
        else
            SetDelay(settings->SDelayBetweenFrames);
        #endif

        DrawBasic(grf,-1);

        si = UpdateEvent(grf);
        if (si != -1)
            RunEvent(si,grf);

        if (ActualTime != PastTime)
        {
            PastTime = ActualTime;
            GetDir(".",grf,1,settings->DirLoadingMode
                #ifdef __THREADS_FOR_DIR_ENABLE__
                ,settings->ThreadsForDir
                #endif
                );
            if (settings->Win1Display)
                GetDir("..",grf,0,settings->DirLoadingMode
                #ifdef __THREADS_FOR_DIR_ENABLE__
                ,settings->ThreadsForDir
                #endif
                );
        }
    } while (!grf->ExitTime);

    endwin();

    freeBasic(grf);
}

void UpdateSizeBasic(Basic* grf)
{
    clear();
    getmaxyx(stdscr,grf->wy,grf->wx);

    if (settings->Bar1Enable)
    {
        wresize(grf->win[3],1,grf->wx);
        mvwin(grf->win[3],0,1);
        werase(grf->win[3]);
    }
    if (settings->Bar2Enable)
    {
        wresize(grf->win[4],1,grf->wx);
        mvwin(grf->win[4],(grf->wy-1)*!settings->StatusBarOnTop+settings->StatusBarOnTop-(!settings->Bar1Enable*settings->StatusBarOnTop),0);
        werase(grf->win[4]);
    }

    if (settings->Win1Enable)
    {
        wresize(grf->win[0],grf->wy-2+!settings->Bar1Enable+!settings->Bar2Enable,grf->wx*settings->WinSizeMod[0]);
        mvwin(grf->win[0],1+settings->StatusBarOnTop-!settings->Bar1Enable-(!settings->Bar2Enable*settings->StatusBarOnTop),0);
        grf->WinMiddle = grf->win[0]->_maxx;
        werase(grf->win[0]);
    }
    wresize(grf->win[1],grf->wy-2+!settings->Bar1Enable+!settings->Bar2Enable,(grf->wx*(settings->WinSizeMod[1]*settings->Win3Enable))+(!settings->Win3Enable*(grf->wx-grf->WinMiddle)));
    mvwin(grf->win[1],1+settings->StatusBarOnTop-!settings->Bar1Enable-(!settings->Bar2Enable*settings->StatusBarOnTop),grf->WinMiddle);
    werase(grf->win[1]);
    if (settings->Win3Enable)
    {
        wresize(grf->win[2],grf->wy-2+!settings->Bar1Enable+!settings->Bar2Enable,grf->wx-grf->win[1]->_maxx-grf->WinMiddle);
        mvwin(grf->win[2],1+settings->StatusBarOnTop-!settings->Bar1Enable-(!settings->Bar2Enable*settings->StatusBarOnTop),grf->win[1]->_maxx+grf->WinMiddle);
        werase(grf->win[2]);
    }

    refresh();
    if (settings->Borders)
        SetBorders(grf,-1);
}

void freeBasic(Basic* grf)
{
    #ifdef __THREADS_FOR_DIR_ENABLE__
    for (size_t i = 0; i < grf->ActualSize; i++)
        if (grf->Base[i].enable)
            pthread_cancel(grf->Base[i].thread);
    #endif

    for (int i = 0; i < 4; i++)
        delwin(grf->win[i]);

    free(grf->cSF);
    #ifdef __USER_NAME_ENABLE__
    free(grf->H_Host);
    free(grf->H_User);
    #endif

    /*for (int i = 0; i < grf->ActualSize; i++)
    {
        if (grf->Base[i].El_t != -1)
        {
            inotify_rm_watch(grf->Base[i].fd,grf->Base[i].wd);
            close(grf->Base[i].fd);
            free(grf->Base[i].path);
            free(grf->Base[i].selected);
            free(grf->Base[i].Ltop);

            for (int j = 0; j < grf->Base[i].El_t; j++)
            {
                free(grf->Base[i].El[j].name);
                #ifdef __HUMAN_READABLE_SIZE_ENABLE__
                free(grf->Base[i].El[j].SizErrToDisplay);
                #endif
            }
        }
        free(grf->Base[i].El);
    }

    free(grf->Base);*/
    free(grf);
}
