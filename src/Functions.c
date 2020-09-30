#include "main.h"
#include "Functions.h"
#include "Usefull.h"
#include "Sort.h"
#include "Chars.h"
#include "Load.h"

#include "config.h"

#ifdef __LOAD_CONFIG_ENABLE__
#include "Loading.h"
#endif

void addKey(Key this)
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
        if (strcmp(this.keys,keys[i].keys) == 0)
        {
            found = (long int)i;
            break;
        }
    }

    if (found == -1)
        found = (long int)keys_t++;
    
    strcpy(keys[found].keys,this.keys);
    keys[found].act = this.act;
    keys[found].slc1 = this.slc1;
    keys[found].slc2 = this.slc2;
}

void KeyInit()
{
    addKey((Key){"q",0,.slc1.v=NULL,.slc2.v=NULL});
    addKey((Key){"j",1,.slc1.ll=1,.slc2.v=NULL});
    addKey((Key){"J",1,.slc1.ll=16,.slc2.v=NULL});
    addKey((Key){"k",2,.slc1.ll=1,.slc2.v=NULL});
    addKey((Key){"K",2,.slc1.ll=16,.slc2.v=NULL});
    addKey((Key){"h",3,.slc1.v=NULL,.slc2.v=NULL});
    addKey((Key){"l",4,.slc1.v=NULL,.slc2.v=NULL});
    addKey((Key){"gff",27,.slc1.ll=0,.slc2.v="/run/media/kpp/fil"}); //0 - is env ; 1 - path/env
    addKey((Key){"gfm",27,.slc1.ll=0,.slc2.v="/run/media/kpp/f/Muzyka"});
    addKey((Key){"gfk",27,.slc1.ll=0,.slc2.v="/run/media/kpp/f/ksiegi"});
    addKey((Key){"gh",27,.slc1.ll=1,.slc2.v="HOME"});
    addKey((Key){"g/",27,.slc1.ll=0,.slc2.v="/"});
    addKey((Key){"gd",27,.slc1.ll=0,.slc2.v="/dev"});
    addKey((Key){"ge",27,.slc1.ll=0,.slc2.v="/etc"});
    addKey((Key){"gm",27,.slc1.ll=1,.slc2.v="MEDIA"}); //variable
    addKey((Key){"gM",27,.slc1.ll=0,.slc2.v="/mnt"});
    addKey((Key){"go",27,.slc1.ll=0,.slc2.v="/opt"});
    addKey((Key){"gs",27,.slc1.ll=0,.slc2.v="/srv"});
    addKey((Key){"gp",27,.slc1.ll=0,.slc2.v="/tmp"});
    addKey((Key){"gu",27,.slc1.ll=0,.slc2.v="/usr"});
    addKey((Key){"gv",27,.slc1.ll=0,.slc2.v="/var"});
    addKey((Key){"gg",5,.slc1.v=NULL,.slc2.v=NULL});
    addKey((Key){"G",6,.slc1.v=NULL,.slc2.v=NULL});
    addKey((Key){"z1",8,.slc1.ll=0,.slc2.v=NULL});
    addKey((Key){"z2",8,.slc1.ll=1,.slc2.v=NULL});
    addKey((Key){"z3",8,.slc1.ll=2,.slc2.v=NULL});
    addKey((Key){"z4",8,.slc1.ll=3,.slc2.v=NULL});
    addKey((Key){"z5",8,.slc1.ll=4,.slc2.v=NULL});
    addKey((Key){"z6",8,.slc1.ll=5,.slc2.v=NULL});
    addKey((Key){"z7",8,.slc1.ll=6,.slc2.v=NULL});
    addKey((Key){"z8",8,.slc1.ll=7,.slc2.v=NULL});
    addKey((Key){"z9",8,.slc1.ll=8,.slc2.v=NULL});
    addKey((Key){"z0",8,.slc1.ll=9,.slc2.v=NULL});
    #ifdef __SORT_ELEMENTS_ENABLE__
    addKey((Key){"oe",9,.slc1.ll=SORT_NONE,.slc2.v=NULL});
    addKey((Key){"oE",9,.slc1.ll=SORT_NONE|SORT_REVERSE,.slc2.v=NULL});
    addKey((Key){"or",9,.slc1.ll=SORT_TYPE,.slc2.v=NULL});
    addKey((Key){"oR",9,.slc1.ll=SORT_TYPE|SORT_REVERSE,.slc2.v=NULL});
    addKey((Key){"ob",9,.slc1.ll=SORT_CHIR,.slc2.v=NULL});
    addKey((Key){"oB",9,.slc1.ll=SORT_CHIR|SORT_REVERSE,.slc2.v=NULL});
    addKey((Key){"os",9,.slc1.ll=SORT_SIZE,.slc2.v=NULL});
    addKey((Key){"oS",9,.slc1.ll=SORT_SIZE|SORT_REVERSE,.slc2.v=NULL});
    addKey((Key){"otm",9,.slc1.ll=SORT_MTIME,.slc2.v=NULL});
    addKey((Key){"otM",9,.slc1.ll=SORT_MTIME|SORT_REVERSE,.slc2.v=NULL});
    addKey((Key){"otc",9,.slc1.ll=SORT_CTIME,.slc2.v=NULL});
    addKey((Key){"otC",9,.slc1.ll=SORT_CTIME|SORT_REVERSE,.slc2.v=NULL});
    addKey((Key){"ota",9,.slc1.ll=SORT_ATIME,.slc2.v=NULL});
    addKey((Key){"otA",9,.slc1.ll=SORT_ATIME|SORT_REVERSE,.slc2.v=NULL});
    addKey((Key){"og",9,.slc1.ll=SORT_GID,.slc2.v=NULL});
    addKey((Key){"oG",9,.slc1.ll=SORT_GID|SORT_REVERSE,.slc2.v=NULL});
    addKey((Key){"ou",9,.slc1.ll=SORT_UID,.slc2.v=NULL});
    addKey((Key){"oU",9,.slc1.ll=SORT_UID|SORT_REVERSE,.slc2.v=NULL});
    addKey((Key){"om",9,.slc1.ll=SORT_LNAME,.slc2.v=NULL});
    addKey((Key){"oM",9,.slc1.ll=SORT_LNAME|SORT_REVERSE,.slc2.v=NULL});
    addKey((Key){"on",9,.slc1.ll=SORT_NAME,.slc2.v=NULL});
    addKey((Key){"oN",9,.slc1.ll=SORT_NAME|SORT_REVERSE,.slc2.v=NULL});
    #endif
    #ifdef __GET_DIR_SIZE_ENABLE__
    addKey((Key){"dch",10,.slc1.ll=D_C,.slc2.v=NULL});
    addKey((Key){"dCh",10,.slc1.ll=D_C|D_R,.slc2.v=NULL});
    addKey((Key){"dsh",10,.slc1.ll=0,.slc2.v=NULL});
    addKey((Key){"dSh",10,.slc1.ll=D_R,.slc2.v=NULL});
    addKey((Key){"dfh",10,.slc1.ll=D_F,.slc2.v=NULL});
    #ifdef __HUMAN_READABLE_SIZE_ENABLE__
    addKey((Key){"dcH",10,.slc1.ll=D_C|D_H,.slc2.v=NULL});
    addKey((Key){"dCH",10,.slc1.ll=D_C|D_R|D_H,.slc2.v=NULL});
    addKey((Key){"dsH",10,.slc1.ll=D_H,.slc2.v=NULL});
    addKey((Key){"dSH",10,.slc1.ll=D_R|D_H,.slc2.v=NULL});
    addKey((Key){"dfH",10,.slc1.ll=D_F|D_H,.slc2.v=NULL});
    #endif
    #endif
    addKey((Key){"x1",11,.slc1.ll=GROUP_0,.slc2.v=NULL});
    addKey((Key){"x2",11,.slc1.ll=GROUP_1,.slc2.v=NULL});
    addKey((Key){"x3",11,.slc1.ll=GROUP_2,.slc2.v=NULL});
    addKey((Key){"x4",11,.slc1.ll=GROUP_3,.slc2.v=NULL});
    addKey((Key){"x5",11,.slc1.ll=GROUP_4,.slc2.v=NULL});
    addKey((Key){"x6",11,.slc1.ll=GROUP_5,.slc2.v=NULL});
    addKey((Key){"x7",11,.slc1.ll=GROUP_6,.slc2.v=NULL});
    addKey((Key){"x8",11,.slc1.ll=GROUP_7,.slc2.v=NULL});
    addKey((Key){" ",12,.slc1.v=NULL,.slc2.v=NULL});
    addKey((Key){"V",13,.slc1.v=NULL,.slc2.v=NULL});
    addKey((Key){"vta",14,.slc1.ll=-1,.slc2.ll=0});
    addKey((Key){"vth",14,.slc1.ll=-1,.slc2.ll=1});
    addKey((Key){"vda",14,.slc1.ll=0,.slc2.ll=0});
    addKey((Key){"vdh",14,.slc1.ll=0,.slc2.ll=1});
    addKey((Key){"vea",14,.slc1.ll=1,.slc2.ll=0});
    addKey((Key){"veh",14,.slc1.ll=1,.slc2.ll=1});
    addKey((Key){"mm",15,.slc1.ll=M_CHNAME,.slc2.v=NULL});
    addKey((Key){"mr",15,.slc1.ll=M_REPLACE,.slc2.v=NULL});
    addKey((Key){"md",15,.slc1.ll=M_DCPY,.slc2.v=NULL});
    addKey((Key){"mM",15,.slc1.ll=M_CHNAME|M_MERGE,.slc2.v=NULL});
    addKey((Key){"mR",15,.slc1.ll=M_REPLACE|M_MERGE,.slc2.v=NULL});
    addKey((Key){"mD",15,.slc1.ll=M_DCPY|M_MERGE,.slc2.v=NULL});
    addKey((Key){"pp",16,.slc1.ll=M_CHNAME,.slc2.v=NULL});
    addKey((Key){"pr",16,.slc1.ll=M_REPLACE,.slc2.v=NULL});
    addKey((Key){"pd",16,.slc1.ll=M_DCPY,.slc2.v=NULL});
    addKey((Key){"pP",16,.slc1.ll=M_CHNAME|M_MERGE,.slc2.v=NULL});
    addKey((Key){"pR",16,.slc1.ll=M_REPLACE|M_MERGE,.slc2.v=NULL});
    addKey((Key){"pD",16,.slc1.ll=M_DCPY|M_MERGE,.slc2.v=NULL});
    addKey((Key){"dd",17,.slc1.ll=1,.slc2.v=NULL});
    addKey((Key){"dD",17,.slc1.ll=0,.slc2.v=NULL});
}

Settings* SettingsInit()
{
    Settings* this = (Settings*)malloc(sizeof(Settings));

    #ifdef __THREADS_ENABLE__
    this->Threads = 0;
    #endif
    this->FileOpener                    = (char*)malloc(PATH_MAX);
    strcpy(this->FileOpener,"NULL");
    this->shell                         = (char*)malloc(PATH_MAX);
    strcpy(this->shell,"/bin/sh");
    this->editor                        = (char*)malloc(PATH_MAX);
    strcpy(this->editor,"/bin/nvim");
    this->Values                        = (char*)malloc(PATH_MAX);
    strcpy(this->Values,"BKMGTPEZY");
    this->BarSettings                   =  B_UHNAME | B_DIR | B_NAME | B_WORKSPACES | DP_LSPERMS | DP_SMTIME | DP_PWNAME | DP_GRNAME | B_POSITION | B_FHBFREE | B_FGROUP | B_MODES | B_CSF;
    this->UserHostPattern               = (char*)malloc(PATH_MAX);
    strcpy(this->UserHostPattern,"%s@%s");
    this->UserRHost                     = false;
    this->CopyBufferSize                = 131072;
    this->INOTIFY_MASK                  = IN_DELETE | IN_DELETE_SELF | IN_CREATE | IN_MOVE | IN_MOVE_SELF;
    this->MoveOffSet                    = 0.1;
    this->WrapScroll                    = false;
    this->JumpScroll                    = false;
    this->JumpScrollValue               = 16;
    this->StatusBarOnTop                = false;
    this->Win1Enable                    = true;
    this->Win3Enable                    = true;
    this->Bar1Enable                    = true;
    this->Bar2Enable                    = true;
    this->WinSizeMod                    = (float*)malloc(2*sizeof(float));
    this->WinSizeMod[0]                 = 0.132f;
    this->WinSizeMod[1]                 = 0.368f;
    this->Borders                       = false;
    this->FillBlankSpace                = true;
    this->WindowBorder                  = (int*)malloc(8*sizeof(int));
    this->WindowBorder[0]               = 0;
    this->WindowBorder[1]               = 0;
    this->WindowBorder[2]               = 0;
    this->WindowBorder[3]               = 0;
    this->WindowBorder[4]               = 0;
    this->WindowBorder[5]               = 0;
    this->WindowBorder[6]               = 0;
    this->WindowBorder[7]               = 0;
    this->EnableColor                   = true;
    this->DelayBetweenFrames            = 1;
    this->NumberLines                   = false;
    this->NumberLinesOff                = false;
    this->NumberLinesFromOne            = false;
    this->DisplayingC                   = DP_HSIZE;
    #ifdef __SHOW_HIDDEN_FILES_ENABLE__
    this->ShowHiddenFiles               = true;
    #endif
    #ifdef __SORT_ELEMENTS_ENABLE__
    this->SortMethod                    = SORT_NAME;
    this->BetterFiles                   = (int*)calloc(24,sizeof(int));
    this->BetterFiles[0]                = T_DIR;
    this->BetterFiles[1]                = T_LDIR;
    #endif
    #ifdef __BLOCK_SIZE_ELEMENTS_ENABLE__
    this->BlockSize                     = 1024;
    #endif
    this->DirSizeMethod                 = D_C;
    this->C_Error                       = COLOR_PAIR(4) | A_BOLD | A_REVERSE;
    #ifdef __COLOR_FILES_BY_EXTENSION__
    this->C_FType_A                     = COLOR_PAIR(4);
    this->C_FType_I                     = COLOR_PAIR(2);
    this->C_FType_V                     = COLOR_PAIR(6);
    #endif
    this->C_Selected		            = A_REVERSE | A_BOLD;
    this->C_Exec_set		            = A_BOLD;
    this->C_Exec		                = COLOR_PAIR(10);
    this->C_BLink		                = COLOR_PAIR(1);
    this->C_Dir		                    = COLOR_PAIR(1) | A_BOLD;
    this->C_Reg		                    = A_NORMAL;
    this->C_Fifo		                = COLOR_PAIR(9) | A_ITALIC;
    this->C_Sock		                = COLOR_PAIR(9) | A_ITALIC;
    this->C_Dev		                    = COLOR_PAIR(9);
    this->C_BDev		                = COLOR_PAIR(9);
    this->C_LDir		                = COLOR_PAIR(5) | A_BOLD;
    this->C_LReg		                = COLOR_PAIR(5);
    this->C_LFifo		                = COLOR_PAIR(5);
    this->C_LSock		                = COLOR_PAIR(5);
    this->C_LDev		                = COLOR_PAIR(5);
    this->C_LBDev		                = COLOR_PAIR(5);
    this->C_Other		                = COLOR_PAIR(0);
    this->C_User_S_D		            = COLOR_PAIR(6) | A_BOLD;
    this->C_Bar_Dir		                = COLOR_PAIR(1) | A_BOLD;
    this->C_Bar_Name		            = A_NORMAL | A_BOLD;
    this->C_Bar_WorkSpace		        = A_NORMAL | A_BOLD;
    this->C_Bar_WorkSpace_Selected	    = COLOR_PAIR(6) | A_REVERSE | A_BOLD;
    this->C_Group_0		                = COLOR_PAIR(2);
    this->C_Group_1		                = COLOR_PAIR(1);
    this->C_Group_2		                = COLOR_PAIR(7);
    this->C_Group_3		                = COLOR_PAIR(4);
    this->C_Group_4		                = COLOR_PAIR(5);
    this->C_Group_5		                = COLOR_PAIR(6);
    this->C_Group_6		                = COLOR_PAIR(9);
    this->C_Group_7		                = COLOR_PAIR(10);
    this->C_Bar_E                       = 0;
    this->C_Bar_F                       = 0;

    return this;
}

Basic* InitBasic()
{
    Basic* this = (Basic*)malloc(sizeof(Basic));

    #ifdef __FILESYSTEM_INFORMATION_ENABLE__
    statfs(".",&this->fs);
    #endif

    this->ExitTime = false;
    this->cSF_E = false;
    this->cSF = (char*)malloc(64);

    KeyInit();
    settings = SettingsInit();

    #ifdef __LOAD_CONFIG_ENABLE__
    char* temp = (char*)malloc(PATH_MAX);
    LoadConfig("/etc/.csasrc",settings);
    sprintf(temp,"%s/.csasrc",getenv("HOME"));
    LoadConfig(temp,settings);
    sprintf(temp,"%s/.config/csas/.csasrc",getenv("HOME"));
    LoadConfig(temp,settings);
    free(temp);
    #endif

    settings->Win1Display = settings->Win1Enable;
    settings->Win3Display = settings->Win3Enable;

    initscr();

    if (has_colors() && settings->EnableColor)
    {
        start_color();
        use_default_colors();

        for (int i = 0; i < 16; i++)
            init_pair(i,i,-1);
    }

    this->win[0] = newwin(0,0,0,0);
    this->win[1] = newwin(0,0,0,0);
    this->win[2] = newwin(0,0,0,0);
    this->win[3] = newwin(0,0,0,0);
    this->win[4] = newwin(0,0,0,0);

    this->WinMiddle = 0;

    this->ActualSize = 0;
    this->AllocatedSize = 0;
    this->Base = NULL;
    this->H_Host = (char*)malloc(64);
    this->H_User = (char*)malloc(64);
    getlogin_r(this->H_User,31);
    gethostname(this->H_Host,31);

    for (int i = 0; i < WORKSPACE_N; i++)
    {
        this->Work[i].exists = 0;
        this->Work[i].SelectedGroup = GROUP_0;
        this->Work[i].Visual = 0;
        for (int j = 0; j < 3; j++)
        {
            this->Work[i].win[j] = NULL;
        }
    }

    this->inW = 0;

    this->Work[this->inW].exists = 1;

    UpdateSizeBasic(this);

    endwin();
    return this;
}

void RunBasic(Basic* this, const int argc, char** argv)
{
    initscr();
    noecho();

    curs_set(0);
    keypad(stdscr,TRUE);
    if (settings->DelayBetweenFrames > -1)
        halfdelay(settings->DelayBetweenFrames);
    else
        nodelay(stdscr,true);

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
    
    CD(".",this);

    do {
        clock_gettime(1,&MainTimer);
        ActualTime = MainTimer.tv_sec;

        DrawBasic(this,-1);

        si = UpdateEvent(this);
        if (si != -1)
            RunEvent(si,this);

        if (ActualTime != PastTime)
        {
            PastTime = ActualTime;
            GetDir(".",this,1
            #ifdef __THREADS_ENABLE__
            ,false
            #endif
            );
            if (settings->Win1Display)
                GetDir("..",this,0
                #ifdef __THREADS_ENABLE__
                ,false
                #endif
                );
        }
    } while (!this->ExitTime);

    endwin();

    freeBasic(this);
}

void UpdateSizeBasic(Basic* this)
{
    clear();
    getmaxyx(stdscr,this->wy,this->wx);

    if (settings->Bar1Enable)
    {
        wresize(this->win[3],1,this->wx);
        mvwin(this->win[3],0,1);
        wclear(this->win[3]);
    }
    if (settings->Bar2Enable)
    {
        wresize(this->win[4],1,this->wx);
        mvwin(this->win[4],(this->wy-1)*!settings->StatusBarOnTop+settings->StatusBarOnTop-(!settings->Bar1Enable*settings->StatusBarOnTop),0);
        wclear(this->win[4]);
    }

    if (settings->Win1Enable)
    {
        wresize(this->win[0],this->wy-2+!settings->Bar1Enable+!settings->Bar2Enable,this->wx*settings->WinSizeMod[0]);
        mvwin(this->win[0],1+settings->StatusBarOnTop-!settings->Bar1Enable-(!settings->Bar2Enable*settings->StatusBarOnTop),0);
        this->WinMiddle = this->win[0]->_maxx;
        wclear(this->win[0]);
    }
    wresize(this->win[1],this->wy-2+!settings->Bar1Enable+!settings->Bar2Enable,(this->wx*(settings->WinSizeMod[1]*settings->Win3Enable))+(!settings->Win3Enable*(this->wx-this->WinMiddle)));
    mvwin(this->win[1],1+settings->StatusBarOnTop-!settings->Bar1Enable-(!settings->Bar2Enable*settings->StatusBarOnTop),this->WinMiddle);
    wclear(this->win[1]);
    if (settings->Win3Enable)
    {
        wresize(this->win[2],this->wy-2+!settings->Bar1Enable+!settings->Bar2Enable,this->wx-this->win[1]->_maxx-this->WinMiddle);
        mvwin(this->win[2],1+settings->StatusBarOnTop-!settings->Bar1Enable-(!settings->Bar2Enable*settings->StatusBarOnTop),this->win[1]->_maxx+this->WinMiddle);
        wclear(this->win[2]);
    }

    refresh();
    if (settings->Borders)
    {
        if (settings->Win1Enable)
        {
            wborder(this->win[0],settings->WindowBorder[0],settings->WindowBorder[1],settings->WindowBorder[2],settings->WindowBorder[3],settings->WindowBorder[4],settings->WindowBorder[5],settings->WindowBorder[6],settings->WindowBorder[7]);
            wrefresh(this->win[0]);
        }
        wborder(this->win[1],settings->WindowBorder[0],settings->WindowBorder[1],settings->WindowBorder[2],settings->WindowBorder[3],settings->WindowBorder[4],settings->WindowBorder[5],settings->WindowBorder[6],settings->WindowBorder[7]);
        wrefresh(this->win[1]);
        if (settings->Win3Enable)
        {
            wborder(this->win[2],settings->WindowBorder[0],settings->WindowBorder[1],settings->WindowBorder[2],settings->WindowBorder[3],settings->WindowBorder[4],settings->WindowBorder[5],settings->WindowBorder[6],settings->WindowBorder[7]);
            wrefresh(this->win[2]);
        }
    }
}

static int ColorEl(const struct Element* this, const bool Select)
{
    int set = 0, col = 0;

    if (this->flags & S_IXUSR)
    {
        set |= settings->C_Exec_set;
        col = settings->C_Exec;
    }

    if (Select)
        set |= settings->C_Selected;

    switch(this->Type)
    {
        case T_REG:
            #ifdef __COLOR_FILES_BY_EXTENSION__
            switch(this->FType)
            {
                case 'A':
                    col = settings->C_FType_A;
                    break;
                case 'I':
                    col = settings->C_FType_I;
                    break;
                case 'V':
                    col = settings->C_FType_V;
                    break;
            }

            #endif
            break;
        case T_LREG:
            col = settings->C_LReg;
            break;
        case T_DIR:
            col = settings->C_Dir;
            break;
        case T_LDIR:
            col = settings->C_LDir;
            break;
        case T_LSOCK:
            col = settings->C_LSock;
            break;
        case T_SOCK:
            col = settings->C_Sock;
            break;
        case T_FIFO:
            col = settings->C_Fifo;
            break;
        case T_LFIFO:
            col = settings->C_LFifo;
            break;
        case T_DEV:
            col = settings->C_Dev;
            break;
        case T_LDEV:
            col = settings->C_LDev;
            break;
        case T_BLINK:
            col = settings->C_BLink;
            break;
        case T_BDEV:
            col = settings->C_BDev;
            break;
        case T_LBDEV:
            col = settings->C_LBDev;
            break;
    }


    return set | col;
}

static void ByIntToStr(size_t* size, const int Settings, char* result, struct Element* this)
{
    #ifdef __FILE_GROUPS_ENABLE__
    struct group  *gr;
    #endif
    #ifdef __FILE_OWNERS_ENABLE__
    struct passwd *pw;
    #endif
    *size = 0;

    if (Settings&DP_LSPERMS)
        *size += sprintf(result+*size,"%s ",lsperms(this->flags,this->Type));
    #ifdef __FILE_SIZE_ENABLE__
    if (Settings&DP_SIZE)
        *size += sprintf(result+*size,"%lld ",this->size);
    #endif
    #ifdef __HUMAN_READABLE_SIZE_ENABLE__
    if (Settings&DP_HSIZE)
    {
        if (this->SizErrToDisplay == NULL)
        {
            this->SizErrToDisplay = (char*)malloc(16);
            MakeHumanReadAble(this->SizErrToDisplay,this->size,
            ((settings->DirSizeMethod&D_H) != D_H)&&(this->Type == T_DIR || this->Type == T_LDIR));
        }
        *size += sprintf(result+*size,"%s ",this->SizErrToDisplay);
    }
    #endif
    #ifdef __FILE_SIZE_ENABLE__
    #ifdef __BLOCK_SIZE_ELEMENTS_ENABLE__
    if (Settings&DP_BLOCKS)
        *size += sprintf(result+*size,"%lld ",this->size/settings->BlockSize);
    #endif
    #endif
    if (Settings&DP_TYPE)
        *size += sprintf(result+*size,"%d ",this->Type);
    #ifdef __COLOR_FILES_BY_EXTENSION__
    if (Settings&DP_FTYPE && this->FType > 32)
        *size += sprintf(result+*size,"%c ",this->FType);
    #endif

    #ifdef __FILE_OWNERS_ENABLE__
    if (this->pw != 1001)
    {
        pw = getpwuid(this->pw);
        if (Settings&DP_PWDIR)
            *size += sprintf(result+*size,"%s ",pw->pw_dir);
        if (Settings&DP_PWGECOS)
            *size += sprintf(result+*size,"%s ",pw->pw_gecos);
        if (Settings&DP_PWGID)
            *size += sprintf(result+*size,"%d ",pw->pw_gid);
        if (Settings&DP_PWNAME)
            *size += sprintf(result+*size,"%s ",pw->pw_name);
        if (Settings&DP_PWPASSWD)
            *size += sprintf(result+*size,"%s ",pw->pw_passwd);
        if (Settings&DP_PWSHELL)
            *size += sprintf(result+*size,"%s ",pw->pw_shell);
        if (Settings&DP_PWUID)
            *size += sprintf(result+*size,"%d ",pw->pw_uid);
    }
    #endif

    #ifdef __FILE_GROUPS_ENABLE__
    if (this->gr != 1001)
    {
        gr = getgrgid(this->gr);

        if (Settings&DP_GRGID)
            *size += sprintf(result+*size,"%d ",gr->gr_gid);
        if (Settings&DP_GRNAME)
            *size += sprintf(result+*size,"%s ",gr->gr_name);
        if (Settings&DP_GRPASSWD)
            *size += sprintf(result+*size,"%s ",gr->gr_passwd);
    }
    #endif

    #ifdef __ATIME_ENABLE__
    if (Settings&DP_ATIME)
        *size += sprintf(result+*size,"%ld ",this->atime);
    if (Settings&DP_SATIME)
        *size += TimeToStr(&this->atime,result+*size);
    #endif
    #ifdef __MTIME_ENABLE__
    if (Settings&DP_MTIME)
        *size += sprintf(result+*size,"%ld ",this->mtime);
    if (Settings&DP_SMTIME)
        *size += TimeToStr(&this->mtime,result+*size);
    #endif

    #ifdef __CTIME_ENABLE__
    if (Settings&DP_CTIME)
        *size += sprintf(result+*size,"%ld ",this->ctime);
    if (Settings&DP_SCTIME)
        *size += TimeToStr(&this->ctime,result+*size);
    #endif

}

void DrawBasic(Basic* this, const int which)
{
    static int color, line_off1, line_off2;;
    static char NameTemp[NAME_MAX];
    static char MainTemp[PATH_MAX];
    static char temp[96];
    static size_t cont_s[4];

    for (int i = 0; i < 3; i++)
    {
        if (which != -1 && i != which)
            continue;
        if (i == 0 && (!settings->Win1Enable || !settings->Win1Display))
            continue;
        if (i == 2 && (!settings->Win3Enable || !settings->Win3Display))
            continue;

        wattron(this->win[i],settings->C_Error);
        #ifdef __THREADS_ENABLE__
        if (this->Work[this->inW].win[i]->enable)
        {
            //wclear(this->win[i]);
            snprintf(NameTemp,this->win[i]->_maxx-((settings->Borders+1)+2),"LOADING");
            mvwaddstr(this->win[i],settings->Borders,settings->Borders+3,NameTemp);
            wattroff(this->win[i],settings->C_Error);
            wrefresh(this->win[i]);
            break;
        }
        #endif
        if ((long long int)this->Work[this->inW].win[i]->El_t == (long long int)-1)
        {
            //wclear(this->win[i]);
            snprintf(NameTemp,this->win[i]->_maxx-((settings->Borders+1)+2),"NOT ACCESSIBLE");
            mvwaddstr(this->win[i],settings->Borders,settings->Borders+3,NameTemp);
            wattroff(this->win[i],settings->C_Error);
            wrefresh(this->win[i]);
            break;
        }
        if ((long long int)this->Work[this->inW].win[i]->El_t == (long long int)0)
        {
            //wclear(this->win[i]);
            snprintf(NameTemp,this->win[i]->_maxx-((settings->Borders+1)+2),"EMPTY");
            mvwaddstr(this->win[i],settings->Borders,settings->Borders+3,NameTemp);
            wattroff(this->win[i],settings->C_Error);
            wrefresh(this->win[i]);
            break;
        }
        wattroff(this->win[i],settings->C_Error);

        if ((long long int)this->Work[this->inW].win[i]->selected[this->inW] >= this->Work[this->inW].win[i]->El_t)
        {
            this->Work[this->inW].win[i]->selected[this->inW] = this->Work[this->inW].win[i]->El_t-1;
            if (this->Work[this->inW].win[i]->El_t-1-this->win[i]->_maxy < 0)
                this->Work[this->inW].win[i]->Ltop[this->inW] = 0;
            else
                this->Work[this->inW].win[i]->Ltop[this->inW] = this->Work[this->inW].win[i]->El_t-1-this->win[i]->_maxy;
        }
        if (this->Work[this->inW].win[i]->Ltop[this->inW]+this->win[i]->_maxy < this->Work[this->inW].win[i]->selected[this->inW])
            this->Work[this->inW].win[i]->Ltop[this->inW] = this->Work[this->inW].win[i]->selected[this->inW]-this->win[i]->_maxy;

        line_off1 = 0;

        for (long long int j = this->Work[this->inW].win[i]->Ltop[this->inW]; j-this->Work[this->inW].win[i]->Ltop[this->inW] < (size_t)this->win[i]->_maxy-(settings->Borders*2)+1; j++)
        {
            if (j == this->Work[this->inW].win[i]->El_t)
            {
                if (this->Work[this->inW].win[i]->Ltop[this->inW] != 0)
                    this->Work[this->inW].win[i]->Ltop[this->inW] = this->Work[this->inW].win[i]->El_t-1-this->win[i]->_maxy;
                break;
            }
            #ifdef __COLOR_FILES_BY_EXTENSION__
            CheckFileExtension(this->Work[this->inW].win[i]->El[j].name,&this->Work[this->inW].win[i]->El[j].FType);
            #endif
            color = ColorEl(&this->Work[this->inW].win[i]->El[j],(j == (long long int)this->Work[this->inW].win[i]->selected[this->inW]));

            #ifdef __SORT_ELEMENTS_ENABLE__
            if (this->Work[this->inW].win[i]->sort_m != settings->SortMethod)
            {
                this->Work[this->inW].win[i]->sort_m = settings->SortMethod;
                if (this->Work[this->inW].win[i]->El_t > 0)
                    SortEl(this->Work[this->inW].win[i]->El,this->Work[this->inW].win[i]->El_t,settings->SortMethod);
            }
            #endif

            if (settings->FillBlankSpace)
                wattron(this->win[i],color);
            for (int g = settings->Borders+1; g < this->win[i]->_maxx-settings->Borders-1+((i == 2)*2)*!settings->Borders+(((i == 1)*2)*!settings->Win3Enable)*!settings->Borders; g++)
                mvwaddch(this->win[i],settings->Borders+j-this->Work[this->inW].win[i]->Ltop[this->inW],g+settings->Borders,' ');
            if (!settings->FillBlankSpace)
                wattron(this->win[i],color);

            cont_s[0] = 0;
            MainTemp[0] = '\0';
            NameTemp[0] = '\0';
            line_off2 = 0;

            if (i == 1)
            {
                if (settings->DisplayingC != 0)
                {
                    ByIntToStr(&cont_s[0],settings->DisplayingC,MainTemp,&this->Work[this->inW].win[i]->El[j]);

                    mvwaddstr(this->win[i],settings->Borders+j-this->Work[this->inW].win[i]->Ltop[this->inW],this->win[i]->_maxx-cont_s[0]-1+(((i == 1)*2)*!settings->Win3Enable)*!settings->Borders,MainTemp);
                }

                if (settings->NumberLinesOff)
                {
                    line_off1 = 0;
                    cont_s[1] = this->Work[this->inW].win[i]->El_t;
                    while (cont_s[1] > 9) { cont_s[1] /= 10; line_off1++; }

                    line_off2 = 0;
                    cont_s[1] = j;
                    while (cont_s[1] > 9) { cont_s[1] /= 10; line_off2++; };
                }

                if (settings->NumberLines)
                {
                    sprintf(temp,"%lld ",j+settings->NumberLinesFromOne);
                    strcat(NameTemp,temp);
                }
            }

            strcat(NameTemp,this->Work[this->inW].win[i]->El[j].name);
            cont_s[1] = strlen(NameTemp);

            if ((long long int)this->win[i]->_maxx < (long long int)(4+cont_s[0]+settings->Borders+1))
                NameTemp[0] = '\0';
            else if ((size_t)cont_s[1] > this->win[i]->_maxx-cont_s[0]-2-((settings->Borders+1)+1)-settings->Borders)
            {
                NameTemp[this->win[i]->_maxx-cont_s[0]-2-((settings->Borders+1)+1)-settings->Borders] = '~';
                NameTemp[this->win[i]->_maxx-cont_s[0]-1-((settings->Borders+1)+1)-settings->Borders] = '\0';
            }

            mvwaddstr(this->win[i],settings->Borders+j-this->Work[this->inW].win[i]->Ltop[this->inW],(settings->Borders*2)+2+(line_off1-line_off2),NameTemp);

            wattroff(this->win[i],color);

            if (this->Work[this->inW].win[i]->El[j].List[this->inW]&this->Work[this->inW].SelectedGroup&GROUP_0)
                color = settings->C_Group_0;
            else if (this->Work[this->inW].win[i]->El[j].List[this->inW]&this->Work[this->inW].SelectedGroup&GROUP_1)
                color = settings->C_Group_1;
            else if (this->Work[this->inW].win[i]->El[j].List[this->inW]&this->Work[this->inW].SelectedGroup&GROUP_2)
                color = settings->C_Group_2;
            else if (this->Work[this->inW].win[i]->El[j].List[this->inW]&this->Work[this->inW].SelectedGroup&GROUP_3)
                color = settings->C_Group_3;
            else if (this->Work[this->inW].win[i]->El[j].List[this->inW]&this->Work[this->inW].SelectedGroup&GROUP_4)
                color = settings->C_Group_4;
            else if (this->Work[this->inW].win[i]->El[j].List[this->inW]&this->Work[this->inW].SelectedGroup&GROUP_5)
                color = settings->C_Group_5;
            else if (this->Work[this->inW].win[i]->El[j].List[this->inW]&this->Work[this->inW].SelectedGroup&GROUP_6)
                color = settings->C_Group_6;
            else if (this->Work[this->inW].win[i]->El[j].List[this->inW]&this->Work[this->inW].SelectedGroup&GROUP_7)
                color = settings->C_Group_7;
            else
                color = 0;

            wattron(this->win[i],(color|A_REVERSE)*(color > 0));
            mvwaddch(this->win[i],settings->Borders+j-this->Work[this->inW].win[i]->Ltop[this->inW],(settings->Borders*2),' ');
            wattroff(this->win[i],(color|A_REVERSE)*(color > 0));


        }

        wrefresh(this->win[i]);
    }

    if ((which == 3 || which == -1) && settings->Bar1Enable)
    {
        // 3
        cont_s[2] = 0;

        if ((settings->BarSettings & B_WORKSPACES) == B_WORKSPACES)
        {
            for (int i = 0; i < WORKSPACE_N; i++)
                    cont_s[2] += this->Work[i].exists;

            cont_s[2] *= 3;
        }

        cont_s[3] = 0;

        if ((settings->BarSettings & B_UHNAME) == B_UHNAME)
        {
            wattron(this->win[3],settings->C_User_S_D);
            if (settings->UserRHost)
                sprintf(temp,settings->UserHostPattern,this->H_Host,this->H_User);
            else
                sprintf(temp,settings->UserHostPattern,this->H_User,this->H_Host);
            mvwaddstr(this->win[3],0,0,temp);
            wattroff(this->win[3],settings->C_User_S_D);
            cont_s[3] = strlen(temp);
        }

        if (
            #ifdef __THREADS_ENABLE__
            !this->Work[this->inW].win[1]->enable &&
            #endif
            this->Work[this->inW].win[1]->El_t > 0)
        {
            if ((settings->BarSettings & B_DIR) == B_DIR)
            {
                strcpy(MainTemp,this->Work[this->inW].win[1]->path);
                if (!(this->Work[this->inW].win[1]->path[0] == '/' && this->Work[this->inW].win[1]->path[1] == '\0'))
                {
                    strcat(MainTemp,"/");
                    MakePathShorter(MainTemp,this->win[3]->_maxx-(cont_s[3]+1+cont_s[2]+strlen(this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].name)));
                }

                wattron(this->win[3],settings->C_Bar_Dir);
                mvwprintw(this->win[3],0,cont_s[3]," %s",MainTemp);
                wattroff(this->win[3],settings->C_Bar_Dir);

                cont_s[3] += strlen(MainTemp)+1;
            }
            if ((settings->BarSettings & B_NAME) == B_NAME)
            {
                wattron(this->win[3],settings->C_Bar_Name);
                mvwaddstr(this->win[3],0,cont_s[3],this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].name);
                wattroff(this->win[3],settings->C_Bar_Name);
            }
        }

        if ((settings->BarSettings & B_WORKSPACES) == B_WORKSPACES)
        {
            cont_s[2] /= 3;

            if (cont_s[2] > 1)
            {
                cont_s[2] = 2;
                for (int i = WORKSPACE_N-1; i > -1; i--)
                {
                    if (this->Work[i].exists)
                    {
                        if (i == this->inW)
                            wattron(this->win[3],settings->C_Bar_WorkSpace_Selected);
                        else
                            wattron(this->win[3],settings->C_Bar_WorkSpace);

                        mvwprintw(this->win[3],0,this->win[3]->_maxx-cont_s[2]," %d ",i);

                        if (i == this->inW)
                            wattroff(this->win[3],settings->C_Bar_WorkSpace_Selected);
                        else
                            wattroff(this->win[3],settings->C_Bar_WorkSpace);

                        cont_s[2] += 3;
                    }
                }
            }
        }

        wrefresh(this->win[3]);
        for (int i = 0; i < this->win[3]->_maxx+1; i++)
            mvwaddch(this->win[3],0,i,' ');
        // 3
    }

    if (settings->Bar2Enable && (which == -1 || which == 4))
    {
        if (
            #ifdef __THREADS_ENABLE__
            !this->Work[this->inW].win[1]->enable &&
            #endif
            this->Work[this->inW].win[1]->El_t > 0)
        {
            ByIntToStr(&cont_s[0],settings->BarSettings,MainTemp,&this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]]);
            wattron(this->win[4],settings->C_Bar_F);
            mvwaddstr(this->win[4],0,0,MainTemp);
            wattroff(this->win[4],settings->C_Bar_E);

            MainTemp[0] = '\0';
        }

        cont_s[0] = 0;

        if (settings->BarSettings & B_CSF && this->cSF_E)
            cont_s[0] += sprintf(MainTemp+cont_s[0],"%s",this->cSF);
        if (settings->BarSettings & B_MODES)
            if (this->Work[this->inW].Visual)
                cont_s[0] += sprintf(MainTemp+cont_s[0]," VISUAL");
        if (settings->BarSettings & B_FGROUP)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %dW",this->Work[this->inW].SelectedGroup);
        #ifdef __FILESYSTEM_INFORMATION_ENABLE__
        if (settings->BarSettings & B_FTYPE)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %lx",this->fs.f_type);
        if (settings->BarSettings & B_SFTYPE)
        {
            switch (this->fs.f_type)
            {
                case 0xadf5:  cont_s[0] += sprintf(MainTemp+cont_s[0]," ADFS_SUPER_MAGIC"); break;
                case 0xadff:  cont_s[0] += sprintf(MainTemp+cont_s[0]," AFFS_SUPER_MAGIC"); break;
                case 0x5346414F:  cont_s[0] += sprintf(MainTemp+cont_s[0]," AFS_SUPER_MAGIC"); break;
                case 0x0187:  cont_s[0] += sprintf(MainTemp+cont_s[0]," AUTOFS_SUPER_MAGIC"); break;
                case 0x73757245:  cont_s[0] += sprintf(MainTemp+cont_s[0]," CODA_SUPER_MAGIC"); break;
                case 0x28cd3d45:  cont_s[0] += sprintf(MainTemp+cont_s[0]," CRAMFS_MAGIC"); break;
                case 0x453dcd28:  cont_s[0] += sprintf(MainTemp+cont_s[0]," CRAMFS_MAGIC_WEND"); break;
                case 0x64626720:  cont_s[0] += sprintf(MainTemp+cont_s[0]," DEBUGFS_MAGIC"); break;
                case 0x73636673:  cont_s[0] += sprintf(MainTemp+cont_s[0]," SECURITYFS_MAGIC"); break;
                case 0xf97cff8c:  cont_s[0] += sprintf(MainTemp+cont_s[0]," SELINUX_MAGIC"); break;
                case 0x43415d53:  cont_s[0] += sprintf(MainTemp+cont_s[0]," SMACK_MAGIC"); break;
                case 0x858458f6:  cont_s[0] += sprintf(MainTemp+cont_s[0]," RAMFS_MAGIC"); break;
                case 0x01021994:  cont_s[0] += sprintf(MainTemp+cont_s[0]," TMPFS_MAGIC"); break;
                case 0x958458f6:  cont_s[0] += sprintf(MainTemp+cont_s[0]," HUGETLBFS_MAGIC"); break;
                case 0x73717368:  cont_s[0] += sprintf(MainTemp+cont_s[0]," SQUASHFS_MAGIC"); break;
                case 0xf15f:  cont_s[0] += sprintf(MainTemp+cont_s[0]," ECRYPTFS_SUPER_MAGIC"); break;
                case 0x414A53:  cont_s[0] += sprintf(MainTemp+cont_s[0]," EFS_SUPER_MAGIC"); break;
                case 0xE0F5E1E2:  cont_s[0] += sprintf(MainTemp+cont_s[0]," EROFS_SUPER_MAGIC_V1"); break;
                case 0xEF53:  cont_s[0] += sprintf(MainTemp+cont_s[0]," EXT_SUPER_MAGIC"); break;
                case 0xabba1974:  cont_s[0] += sprintf(MainTemp+cont_s[0]," XENFS_SUPER_MAGIC"); break;
                case 0x9123683E:  cont_s[0] += sprintf(MainTemp+cont_s[0]," BTRFS_SUPER_MAGIC"); break;
                case 0x3434:  cont_s[0] += sprintf(MainTemp+cont_s[0]," NILFS_SUPER_MAGIC"); break;
                case 0xF2F52010:  cont_s[0] += sprintf(MainTemp+cont_s[0]," F2FS_SUPER_MAGIC"); break;
                case 0xf995e849:  cont_s[0] += sprintf(MainTemp+cont_s[0]," HPFS_SUPER_MAGIC"); break;
                case 0x9660:  cont_s[0] += sprintf(MainTemp+cont_s[0]," ISOFS_SUPER_MAGIC"); break;
                case 0x72b6:  cont_s[0] += sprintf(MainTemp+cont_s[0]," JFFS2_SUPER_MAGIC"); break;
                case 0x58465342:  cont_s[0] += sprintf(MainTemp+cont_s[0]," XFS_SUPER_MAGIC"); break;
                case 0x6165676C:  cont_s[0] += sprintf(MainTemp+cont_s[0]," PSTOREFS_MAGIC"); break;
                case 0xde5e81e4:  cont_s[0] += sprintf(MainTemp+cont_s[0]," EFIVARFS_MAGIC"); break;
                case 0x00c0ffee:  cont_s[0] += sprintf(MainTemp+cont_s[0]," HOSTFS_SUPER_MAGIC"); break;
                case 0x794c7630:  cont_s[0] += sprintf(MainTemp+cont_s[0]," OVERLAYFS_SUPER_MAGIC"); break;
                case 0x137F:  cont_s[0] += sprintf(MainTemp+cont_s[0]," MINIX_SUPER_MAGIC"); break;
                case 0x138F:  cont_s[0] += sprintf(MainTemp+cont_s[0]," MINIX_SUPER_MAGIC2"); break;
                case 0x2468:  cont_s[0] += sprintf(MainTemp+cont_s[0]," MINIX2_SUPER_MAGIC"); break;
                case 0x2478:  cont_s[0] += sprintf(MainTemp+cont_s[0]," MINIX2_SUPER_MAGIC2"); break;
                case 0x4d5a:  cont_s[0] += sprintf(MainTemp+cont_s[0]," MINIX3_SUPER_MAGIC"); break;
                case 0x4d44:  cont_s[0] += sprintf(MainTemp+cont_s[0]," MSDOS_SUPER_MAGIC"); break;
                case 0x564c:  cont_s[0] += sprintf(MainTemp+cont_s[0]," NCP_SUPER_MAGIC"); break;
                case 0x6969:  cont_s[0] += sprintf(MainTemp+cont_s[0]," NFS_SUPER_MAGIC"); break;
                case 0x7461636f:  cont_s[0] += sprintf(MainTemp+cont_s[0]," OCFS2_SUPER_MAGIC"); break;
                case 0x9fa1:  cont_s[0] += sprintf(MainTemp+cont_s[0]," OPENPROM_SUPER_MAGIC"); break;
                case 0x002f:  cont_s[0] += sprintf(MainTemp+cont_s[0]," QNX4_SUPER_MAGIC"); break;
                case 0x68191122:  cont_s[0] += sprintf(MainTemp+cont_s[0]," QNX6_SUPER_MAGIC"); break;
                case 0x6B414653:  cont_s[0] += sprintf(MainTemp+cont_s[0]," AFS_FS_MAGIC"); break;
                case 0x52654973:  cont_s[0] += sprintf(MainTemp+cont_s[0]," REISERFS_SUPER_MAGIC"); break;
                case 0x517B:  cont_s[0] += sprintf(MainTemp+cont_s[0]," SMB_SUPER_MAGIC"); break;
                case 0x27e0eb:  cont_s[0] += sprintf(MainTemp+cont_s[0]," CGROUP_SUPER_MAGIC"); break;
                case 0x63677270:  cont_s[0] += sprintf(MainTemp+cont_s[0]," CGROUP2_SUPER_MAGIC"); break;
                case 0x7655821:  cont_s[0] += sprintf(MainTemp+cont_s[0]," RDTGROUP_SUPER_MAGIC"); break;
                case 0x57AC6E9D:  cont_s[0] += sprintf(MainTemp+cont_s[0]," STACK_END_MAGIC"); break;
                case 0x74726163:  cont_s[0] += sprintf(MainTemp+cont_s[0]," TRACEFS_MAGIC"); break;
                case 0x01021997:  cont_s[0] += sprintf(MainTemp+cont_s[0]," V9FS_MAGIC"); break;
                case 0x62646576:  cont_s[0] += sprintf(MainTemp+cont_s[0]," BDEVFS_MAGIC"); break;
                case 0x64646178:  cont_s[0] += sprintf(MainTemp+cont_s[0]," DAXFS_MAGIC"); break;
                case 0x42494e4d:  cont_s[0] += sprintf(MainTemp+cont_s[0]," BINFMTFS_MAGIC"); break;
                case 0x1cd1:  cont_s[0] += sprintf(MainTemp+cont_s[0]," DEVPTS_SUPER_MAGIC"); break;
                case 0x6c6f6f70:  cont_s[0] += sprintf(MainTemp+cont_s[0]," BINDERFS_SUPER_MAGIC"); break;
                case 0xBAD1DEA:  cont_s[0] += sprintf(MainTemp+cont_s[0]," FUTEXFS_SUPER_MAGIC"); break;
                case 0x50495045:  cont_s[0] += sprintf(MainTemp+cont_s[0]," PIPEFS_MAGIC"); break;
                case 0x9fa0:  cont_s[0] += sprintf(MainTemp+cont_s[0]," PROC_SUPER_MAGIC"); break;
                case 0x534F434B:  cont_s[0] += sprintf(MainTemp+cont_s[0]," SOCKFS_MAGIC"); break;
                case 0x62656572:  cont_s[0] += sprintf(MainTemp+cont_s[0]," SYSFS_MAGIC"); break;
                case 0x9fa2:  cont_s[0] += sprintf(MainTemp+cont_s[0]," USBDEVICE_SUPER_MAGIC"); break;
                case 0x11307854:  cont_s[0] += sprintf(MainTemp+cont_s[0]," MTD_INODE_FS_MAGIC"); break;
                case 0x09041934:  cont_s[0] += sprintf(MainTemp+cont_s[0]," ANON_INODE_FS_MAGIC"); break;
                case 0x73727279:  cont_s[0] += sprintf(MainTemp+cont_s[0]," BTRFS_TEST_MAGIC"); break;
                case 0x6e736673:  cont_s[0] += sprintf(MainTemp+cont_s[0]," NSFS_MAGIC"); break;
                case 0xcafe4a11:  cont_s[0] += sprintf(MainTemp+cont_s[0]," BPF_FS_MAGIC"); break;
                case 0x5a3c69f0:  cont_s[0] += sprintf(MainTemp+cont_s[0]," AAFS_MAGIC"); break;
                case 0x5a4f4653:  cont_s[0] += sprintf(MainTemp+cont_s[0]," ZONEFS_MAGIC"); break;
                case 0x15013346:  cont_s[0] += sprintf(MainTemp+cont_s[0]," UDF_SUPER_MAGIC"); break;
                case 0x13661366:  cont_s[0] += sprintf(MainTemp+cont_s[0]," BALLOON_KVM_MAGIC"); break;
                case 0x58295829:  cont_s[0] += sprintf(MainTemp+cont_s[0]," ZSMALLOC_MAGIC"); break;
                case 0x444d4142:  cont_s[0] += sprintf(MainTemp+cont_s[0]," DMA_BUF_MAGIC"); break;
                case 0x454d444d:  cont_s[0] += sprintf(MainTemp+cont_s[0]," DEVMEM_MAGIC"); break;
                case 0x33:  cont_s[0] += sprintf(MainTemp+cont_s[0]," Z3FOLD_MAGIC"); break;
                case 0xc7571590:  cont_s[0] += sprintf(MainTemp+cont_s[0]," PPC_CMM_MAGIC"); break;
            }
        }
        if (settings->BarSettings & B_FBSIZE)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld",this->fs.f_bsize);
        if (settings->BarSettings & B_FBLOCKS)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld",this->fs.f_blocks);
        #ifdef __HUMAN_READABLE_SIZE_ENABLE__
        if (settings->BarSettings & B_FHBLOCKS)
        {
            MakeHumanReadAble(temp,this->fs.f_blocks*this->fs.f_bsize,false);
            cont_s[0] += sprintf(MainTemp+cont_s[0],temp);
        }
        if (settings->BarSettings & B_FHBFREE)
        {
            MakeHumanReadAble(temp,this->fs.f_bfree*this->fs.f_bsize,false);
            cont_s[0] += sprintf(MainTemp+cont_s[0],temp);
        }
        if (settings->BarSettings & B_FHBAVAIL)
        {
            MakeHumanReadAble(temp,this->fs.f_bavail*this->fs.f_bsize,false);
            cont_s[0] += sprintf(MainTemp+cont_s[0],temp);
        }
        #endif
        if (settings->BarSettings & B_FBFREE)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld",this->fs.f_bfree);
        if (settings->BarSettings & B_FBAVAIL)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld",this->fs.f_bavail);
        if (settings->BarSettings & B_FFILES)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld",this->fs.f_files);
        if (settings->BarSettings & B_FFFREE)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld",this->fs.f_ffree);
        if (settings->BarSettings & B_FFSID)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %d %d",this->fs.f_fsid.__val[0],this->fs.f_fsid.__val[1]);
        if (settings->BarSettings & B_FNAMELEN)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld",this->fs.f_namelen);
        if (settings->BarSettings & B_FFRSIZE)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld",this->fs.f_frsize);
        if (settings->BarSettings & B_FFLAGS)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld",this->fs.f_flags);
        #endif
        if (
            #ifdef __THREADS_ENABLE__
            !this->Work[this->inW].win[1]->enable &&
            #endif
            this->Work[this->inW].win[1]->El_t > 0)
        {
            if (settings->BarSettings & B_POSITION)
                cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld/%lld",this->Work[this->inW].win[1]->selected[this->inW]+1,this->Work[this->inW].win[1]->El_t);
        }

        wattron(this->win[4],settings->C_Bar_E);
        mvwaddstr(this->win[4],0,this->win[4]->_maxx-cont_s[0]+1,MainTemp);
        wattroff(this->win[4],settings->C_Bar_E);

        wrefresh(this->win[4]);
        for (int i = 0; i < this->win[4]->_maxx+1; i++)
            mvwaddch(this->win[4],0,i,' ');
        //4
    }
}

void freeBasic(Basic* this)
{
    #ifdef __THREADS_ENABLE__
    for (size_t i = 0; i < this->ActualSize; i++)
        if (this->Base[i].enable)
            pthread_cancel(this->Base[i].thread);
    #endif

    for (int i = 0; i < 4; i++)
        delwin(this->win[i]);

    free(this->cSF);
    free(this->H_Host);
    free(this->H_User);

    /*for (int i = 0; i < this->ActualSize; i++)
    {
        if (this->Base[i].El_t != -1)
        {
            inotify_rm_watch(this->Base[i].fd,this->Base[i].wd);
            close(this->Base[i].fd);
            free(this->Base[i].path);
            free(this->Base[i].selected);
            free(this->Base[i].Ltop);

            for (int j = 0; j < this->Base[i].El_t; j++)
            {
                free(this->Base[i].El[j].name);
                #ifdef __HUMAN_READABLE_SIZE_ENABLE__
                free(this->Base[i].El[j].SizErrToDisplay);
                #endif
            }
        }
        free(this->Base[i].El);
    }

    free(this->Base);*/
    free(this);
}

