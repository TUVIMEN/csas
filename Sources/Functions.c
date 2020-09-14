#include "main.h"
#include "Functions.h"
#include "Usefull.h"
#include "Sort.h"

extern bool Win1Enable;
    extern bool Win1Display;
extern bool Win3Enable;
    extern bool Win3Display;
extern bool Bar1Enable;
extern bool Bar2Enable;
extern float WinSizeMod[];
extern bool Borders;
extern int WindowBorder[];
extern bool UserRHost;
extern char* UserHostPattern;
extern char DirSizeMethod;
extern unsigned char SortMethod;

Basic* InitBasic()
{
    Basic* this = (Basic*)malloc(sizeof(Basic));

    #ifdef __FILESYSTEM_INFORMATION_ENABLE__
    statfs(".",&this->fs);
    #endif

    this->win[0] = newwin(0,0,0,0);
    this->win[1] = newwin(0,0,0,0);
    this->win[2] = newwin(0,0,0,0);
    this->win[3] = newwin(0,0,0,0);
    this->win[4] = newwin(0,0,0,0);

    this->WinMiddle = 0;

    this->ActualSize = 0;
    this->AllocatedSize = 0;
    this->Base = NULL;
    this->NameHost = (char*)malloc(72);
    {
        char* UserN = (char*)malloc(32);
        char* HostN = (char*)malloc(32);
        getlogin_r(HostN,31);
        gethostname(UserN,31);
        if (UserRHost)
            sprintf(this->NameHost,UserHostPattern,UserN,HostN);
        else
            sprintf(this->NameHost,UserHostPattern,HostN,UserN);
        free(UserN);
        free(HostN);
    }

    for (int i = 0; i < WORKSPACE_N; i++)
    {
        this->Work[i].exists = 0;
        for (int j = 0; j < 3; j++)
        {
            this->Work[i].win[j] = NULL;
        }
    }

    this->inW = 0;

    this->Work[this->inW].exists = 1;

    UpdateSizeBasic(this);

    return this;
}

extern bool StatusBarOnTop;

void UpdateSizeBasic(Basic* this)
{
    clear();
    getmaxyx(stdscr,this->wy,this->wx);

    if (Bar1Enable)
    {
        wresize(this->win[3],1,this->wx);
        mvwin(this->win[3],0,1);
        wclear(this->win[3]);
    }
    if (Bar2Enable)
    {
        wresize(this->win[4],1,this->wx);
        mvwin(this->win[4],(this->wy-1)*!StatusBarOnTop+StatusBarOnTop-(!Bar1Enable*StatusBarOnTop),0);
        wclear(this->win[4]);
    }

    if (Win1Enable)
    {
        wresize(this->win[0],this->wy-2+!Bar1Enable+!Bar2Enable,this->wx*WinSizeMod[0]);
        mvwin(this->win[0],1+StatusBarOnTop-!Bar1Enable-(!Bar2Enable*StatusBarOnTop),0);
        this->WinMiddle = this->win[0]->_maxx;
        wclear(this->win[0]);
    }
    wresize(this->win[1],this->wy-2+!Bar1Enable+!Bar2Enable,(this->wx*(WinSizeMod[1]*Win3Enable))+(!Win3Enable*(this->wx-this->WinMiddle)));
    mvwin(this->win[1],1+StatusBarOnTop-!Bar1Enable-(!Bar2Enable*StatusBarOnTop),this->WinMiddle);
    wclear(this->win[1]);
    if (Win3Enable)
    {
        wresize(this->win[2],this->wy-2+!Bar1Enable+!Bar2Enable,this->wx-this->win[1]->_maxx-this->WinMiddle);
        mvwin(this->win[2],1+StatusBarOnTop-!Bar1Enable-(!Bar2Enable*StatusBarOnTop),this->win[1]->_maxx+this->WinMiddle);
        wclear(this->win[2]);
    }

    refresh();
    if (Borders)
    {
        if (Win1Enable)
        {
            wborder(this->win[0],WindowBorder[0],WindowBorder[1],WindowBorder[2],WindowBorder[3],WindowBorder[4],WindowBorder[5],WindowBorder[6],WindowBorder[7]);
            wrefresh(this->win[0]);
        }
        wborder(this->win[1],WindowBorder[0],WindowBorder[1],WindowBorder[2],WindowBorder[3],WindowBorder[4],WindowBorder[5],WindowBorder[6],WindowBorder[7]);
        wrefresh(this->win[1]);
        if (Win3Enable)
        {
            wborder(this->win[2],WindowBorder[0],WindowBorder[1],WindowBorder[2],WindowBorder[3],WindowBorder[4],WindowBorder[5],WindowBorder[6],WindowBorder[7]);
            wrefresh(this->win[2]);
        }
    }
}

extern int C_Error;
#ifdef __COLOR_FILES_BY_EXTENSION__
extern int C_FType_A;
extern int C_FType_I;
extern int C_FType_V;
#endif
extern int C_Selected;
extern int C_Exec_set;
extern int C_Exec;
extern int C_BLink;
extern int C_Dir;
extern int C_Reg;
extern int C_Fifo;
extern int C_Sock;
extern int C_Dev;
extern int C_BDev;
extern int C_LReg;
extern int C_LDir;
extern int C_LReg;
extern int C_LFifo;
extern int C_LSock;
extern int C_LDev;
extern int C_LBDev;
extern int C_Other;

int ColorEl(struct Element* this, bool Select)
{
    int set = 0, col = 0;

    if (this->flags & S_IXUSR)
    {
        set |= C_Exec_set;
        col = C_Exec;
    }

    if (Select)
        set |= C_Selected;



    switch(this->Type)
    {
        case T_REG:
            #ifdef __COLOR_FILES_BY_EXTENSION__
            switch(this->FType)
            {
                case 'A':
                    col = C_FType_A;
                    break;
                case 'I':
                    col = C_FType_I;
                    break;
                case 'V':
                    col = C_FType_V;
                    break;
            }

            #endif
            break;
        case T_LREG:
            col = C_LReg;
            break;
        case T_DIR:
            col = C_Dir;
            break;
        case T_LDIR:
            col = C_LDir;
            break;
        case T_LSOCK:
            col = C_LSock;
            break;
        case T_SOCK:
            col = C_Sock;
            break;
        case T_FIFO:
            col = C_Fifo;
            break;
        case T_LFIFO:
            col = C_LFifo;
            break;
        case T_DEV:
            col = C_Dev;
            break;
        case T_LDEV:
            col = C_LDev;
            break;
        case T_BLINK:
            col = C_BLink;
            break;
        case T_BDEV:
            col = C_BDev;
            break;
        case T_LBDEV:
            col = C_LBDev;
            break;
    }


    return set | col;
}

extern bool FillBlankSpace;
extern bool NumberLines;
extern bool NumberLinesOff;
extern bool NumberLinesFromOne;
#ifdef __BLOCK_SIZE_ELEMENTS_ENABLE__
extern size_t BlockSize;
#endif

static void ByIntToStr(int Settings, char* result, struct Element* this)
{
    static char temp[96];
    #ifdef __FILE_GROUPS_ENABLE__
    struct group  *gr;
    #endif
    #ifdef __FILE_OWNERS_ENABLE__
    struct passwd *pw;
    #endif

    if ((Settings&DP_LSPERMS) == DP_LSPERMS)
    {
        strcat(result,lsperms(this->flags,this->Type));
        strcat(result," ");
    }
    #ifdef __FILE_SIZE_ENABLE__
    if ((Settings&DP_SIZE) == DP_SIZE)
    {
        sprintf(temp,"%lld ",this->size);
        strcat(result,temp);
    }
    #endif
    #ifdef __HUMAN_READABLE_SIZE_ENABLE__
    if ((Settings&DP_HSIZE) == DP_HSIZE)
    {
        if (this->SizErrToDisplay == NULL)
        {
            this->SizErrToDisplay = (char*)malloc(16);
            MakeHumanReadAble(this->SizErrToDisplay,this->size,
            ((DirSizeMethod&D_H) != D_H)*(this->Type == T_DIR || this->Type == T_LDIR) ? true : false);
        }
        strcat(result,this->SizErrToDisplay);
        strcat(result," ");
    }
    #endif
    #ifdef __FILE_SIZE_ENABLE__
    if ((Settings&DP_BLOCKS) == DP_BLOCKS)
    {
        sprintf(temp,"%lld ",this->size/BlockSize);
        strcat(result,temp);
    }
    #endif
    if ((Settings&DP_TYPE) == DP_TYPE)
    {
        sprintf(temp,"%d ",this->Type);
        strcat(result,temp);
    }
    #ifdef __COLOR_FILES_BY_EXTENSION__
    if ((Settings&DP_FTYPE) == DP_FTYPE && this->FType > 32)
    {
        sprintf(temp,"%c ",this->FType);
        strcat(result,temp);
    }
    #endif

    #ifdef __FILE_OWNERS_ENABLE__
    if (this->pw != 1001)
    {
        pw = getpwuid(this->pw);
        if ((Settings&DP_PWDIR) == DP_PWDIR)
        {
            sprintf(temp,"%s ",pw->pw_dir);
            strcat(result,temp);
        }
        if ((Settings&DP_PWGECOS) == DP_PWGECOS)
        {
            sprintf(temp,"%s ",pw->pw_gecos);
            strcat(result,temp);
        }
        if ((Settings&DP_PWGID) == DP_PWGID)
        {
            sprintf(temp,"%d ",pw->pw_gid);
            strcat(result,temp);
        }
        if ((Settings&DP_PWNAME) == DP_PWNAME)
        {
            sprintf(temp,"%s ",pw->pw_name);
            strcat(result,temp);
        }
        if ((Settings&DP_PWPASSWD) == DP_PWPASSWD)
        {
            sprintf(temp,"%s ",pw->pw_passwd);
            strcat(result,temp);
        }
        if ((Settings&DP_PWSHELL) == DP_PWSHELL)
        {
            sprintf(temp,"%s ",pw->pw_shell);
            strcat(result,temp);
        }
        if ((Settings&DP_PWUID) == DP_PWUID)
        {
            sprintf(temp,"%d ",pw->pw_uid);
            strcat(result,temp);
        }
    }
    #endif

    #ifdef __FILE_GROUPS_ENABLE__
    if (this->gr != 1001)
    {
        gr = getgrgid(this->gr);

        if ((Settings&DP_GRGID) == DP_GRGID)
        {
            sprintf(temp,"%d ",gr->gr_gid);
            strcat(result,temp);
        }
        if ((Settings&DP_GRNAME) == DP_GRNAME)
        {
            sprintf(temp,"%s ",gr->gr_name);
            strcat(result,temp);
        }
        if ((Settings&DP_GRPASSWD) == DP_GRPASSWD)
        {
            sprintf(temp,"%s ",gr->gr_passwd);
            strcat(result,temp);
        }
    }
    #endif

    #ifdef __ATIME_ENABLE__
    if ((Settings&DP_ATIME) == DP_ATIME)
    {
        sprintf(temp,"%ld ",this->atime);
        strcat(result,temp);
    }
    if ((Settings&DP_SATIME) == DP_SATIME)
    {
        TimeToStr(&this->atime,temp);
        strcat(result,temp);
    }
    #endif
    #ifdef __MTIME_ENABLE__
    if ((Settings&DP_MTIME) == DP_MTIME)
    {
        sprintf(temp,"%ld ",this->mtime);
        strcat(result,temp);
    }
    if ((Settings&DP_SMTIME) == DP_SMTIME)
    {
        TimeToStr(&this->mtime,temp);
        strcat(result,temp);
    }
    #endif
    #ifdef __CTIME_ENABLE__
    if ((Settings&DP_CTIME) == DP_CTIME)
    {
        sprintf(temp,"%ld ",this->ctime);
        strcat(result,temp);
    }
    if ((Settings&DP_SCTIME) == DP_SCTIME)
    {
        TimeToStr(&this->ctime,temp);
        strcat(result,temp);
    }
    #endif

}

void DrawBasic(Basic* this, int which)
{
    extern int C_Bar_WorkSpace;
    extern int C_Bar_WorkSpace_Selected;
    extern int C_User_S_D;
    extern int C_Bar_Dir;
    extern int C_Bar_Name;
    extern long long int BarsSettings;
    extern int DisplayingC;

    int color;
    char* temp[3];
    temp[0] = (char*)malloc(NAME_MAX+16);
    temp[1] = (char*)malloc(PATH_MAX);
    temp[2] = (char*)malloc(96);
    size_t cont_s[4];
    int line_off1, line_off2;

    for (int i = 0; i < 3; i++)
    {
        if (which != -1 && i != which)
            continue;
        if (i == 0 && (!this->Work[this->inW].win[0] || !Win1Display))
            continue;
        if (i == 2 && (!this->Work[this->inW].win[2] || !Win3Display))
            continue;

        wattron(this->win[i],C_Error);
        if (this->Work[this->inW].win[i]->enable)
        {
            //wclear(this->win[i]);
            snprintf(temp[0],this->win[i]->_maxx-((Borders+1)+2),"LOADING");
            mvwaddstr(this->win[i],Borders,Borders+3,temp[0]);
            wattroff(this->win[i],C_Error);
            wrefresh(this->win[i]);
            break;
        }
        if (this->Work[this->inW].win[i]->El_t == -1)
        {
            //wclear(this->win[i]);
            snprintf(temp[0],this->win[i]->_maxx-((Borders+1)+2),"NOT ACCESSIBLE");
            mvwaddstr(this->win[i],Borders,Borders+3,temp[0]);
            wattroff(this->win[i],C_Error);
            wrefresh(this->win[i]);
            break;
        }
        if (this->Work[this->inW].win[i]->El_t == 0)
        {
            //wclear(this->win[i]);
            snprintf(temp[0],this->win[i]->_maxx-((Borders+1)+2),"EMPTY");
            mvwaddstr(this->win[i],Borders,Borders+3,temp[0]);
            wattroff(this->win[i],C_Error);
            wrefresh(this->win[i]);
            break;
        }
        wattroff(this->win[i],C_Error);

        line_off1 = 0;

        for (int j = this->Work[this->inW].win[i]->Ltop[this->inW]; j < this->Work[this->inW].win[i]->El_t && j-this->Work[this->inW].win[i]->Ltop[this->inW] < this->win[i]->_maxy-(Borders*2)+1; j++)
        {
            color = ColorEl(&this->Work[this->inW].win[i]->El[j],(j == this->Work[this->inW].win[i]->selected[this->inW]));

            if (this->Work[this->inW].win[i]->sort_m != SortMethod)
            {
                this->Work[this->inW].win[i]->sort_m = SortMethod;
                if (this->Work[this->inW].win[i]->El_t > 0)
                    SortEl(this->Work[this->inW].win[i]->El,this->Work[this->inW].win[i]->El_t,SortMethod);
            }

            if (FillBlankSpace)
                wattron(this->win[i],color);
            for (int g = Borders+1; g < this->win[i]->_maxx-Borders-1+((i == 2)*2)*!Borders+(((i == 1)*2)*!Win3Enable)*!Borders; g++)
                mvwaddch(this->win[i],Borders+j-this->Work[this->inW].win[i]->Ltop[this->inW],g+Borders,' ');

            wattron(this->win[i],color);

            cont_s[0] = 0;
            strcpy(temp[1]," ");
            bzero(temp[0],NAME_MAX+16);
            line_off2 = 0;

            if (i == 1)
            {
                ByIntToStr(DisplayingC,temp[1],&this->Work[this->inW].win[i]->El[j]);

                cont_s[0] += strlen(temp[1]);

                mvwaddstr(this->win[i],Borders+j-this->Work[this->inW].win[i]->Ltop[this->inW],this->win[i]->_maxx-cont_s[0]-1+(((i == 1)*2)*!Win3Enable)*!Borders,temp[1]);

                if (NumberLinesOff)
                {
                    line_off1 = 0;
                    cont_s[1] = this->Work[this->inW].win[i]->El_t;
                    while (cont_s[1] > 9) { cont_s[1] /= 10; line_off1++; }

                    line_off2 = 0;
                    cont_s[1] = j;
                    while (cont_s[1] > 9) { cont_s[1] /= 10; line_off2++; };
                }

                if (NumberLines)
                {
                    sprintf(temp[2],"%d ",j+NumberLinesFromOne);
                    strcat(temp[0],temp[2]);
                }
            }

            strcat(temp[0],this->Work[this->inW].win[i]->El[j].name);
            cont_s[1] = strlen(temp[0]);

            if (this->win[i]->_maxx < 4+cont_s[0]+Borders+1)
            {
                temp[0][0] = '\0';
            }
            else if (cont_s[1] > this->win[i]->_maxx-cont_s[0]-2-((Borders+1)+1)-Borders)
            {
                if (this->win[i]->_maxx-cont_s[0]-2-((Borders+1)+1)-Borders > -2)
                    temp[0][0] = '\0';
                else
                {
                    temp[0][this->win[i]->_maxx-cont_s[0]-2-((Borders+1)+1)-Borders] = '~';
                    temp[0][this->win[i]->_maxx-cont_s[0]-1-((Borders+1)+1)-Borders] = '\0';
                }
            }

            mvwaddstr(this->win[i],Borders+j-this->Work[this->inW].win[i]->Ltop[this->inW],(Borders*2)+2+(line_off1-line_off2),temp[0]);

            wattroff(this->win[i],color);

            if ((this->Work[this->inW].win[i]->El[j].List[this->inW]&0x1) == 0x1)
                wattron(this->win[i],COLOR_PAIR(2)|A_REVERSE);
            mvwaddch(this->win[i],Borders+j-this->Work[this->inW].win[i]->Ltop[this->inW],(Borders*2),' ');
            if ((this->Work[this->inW].win[i]->El[j].List[this->inW]&0x1) == 0x1)
                wattroff(this->win[i],COLOR_PAIR(2)|A_REVERSE);


        }

        wrefresh(this->win[i]);
    }

    if (which == 1 || which == -1)
    {
        if (Bar1Enable)
        {
            // 3
            cont_s[2] = 0;

            if ((BarsSettings & B_WORKSPACES) == B_WORKSPACES)
            {
                for (int i = 0; i < WORKSPACE_N; i++)
                    if (this->Work[i].exists)
                        cont_s[2]++;

                cont_s[2] *= 3;
            }

            cont_s[3] = 0;

            if ((BarsSettings & B_UHNAME) == B_UHNAME)
            {
                wattron(this->win[3],C_User_S_D);
                mvwaddstr(this->win[3],0,0,this->NameHost);
                wattroff(this->win[3],C_User_S_D);
                cont_s[3] = strlen(this->NameHost);
            }

            if (!this->Work[this->inW].win[1]->enable && this->Work[this->inW].win[1]->El_t > 0)
            {
                if ((BarsSettings & B_DIR) == B_DIR)
                {
                    strcpy(temp[1],this->Work[this->inW].win[1]->path);
                    if (!(this->Work[this->inW].win[1]->path[0] == '/' && this->Work[this->inW].win[1]->path[1] == '\0'))
                    {
                        strcat(temp[1],"/");
                        MakePathShorter(temp[1],this->win[3]->_maxx-(cont_s[3]+1+cont_s[2]+strlen(this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].name)));
                    }

                    wattron(this->win[3],C_Bar_Dir);
                    mvwprintw(this->win[3],0,cont_s[3]," %s",temp[1]);
                    wattroff(this->win[3],C_Bar_Dir);

                    cont_s[3] += strlen(temp[1])+1;
                }
                if ((BarsSettings & B_NAME) == B_NAME)
                {
                    wattron(this->win[3],C_Bar_Name);
                    mvwaddstr(this->win[3],0,cont_s[3],this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].name);
                    wattroff(this->win[3],C_Bar_Name);
                }
            }

            if ((BarsSettings & B_WORKSPACES) == B_WORKSPACES)
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
                                wattron(this->win[3],C_Bar_WorkSpace_Selected);
                            else
                                wattron(this->win[3],C_Bar_WorkSpace);

                            mvwprintw(this->win[3],0,this->win[3]->_maxx-cont_s[2]," %d ",i);

                            if (i == this->inW)
                                wattroff(this->win[3],C_Bar_WorkSpace_Selected);
                            else
                                wattroff(this->win[3],C_Bar_WorkSpace);

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

        if (Bar2Enable)
        {
            //4
            {
                bzero(temp[1],PATH_MAX);

                if (!this->Work[this->inW].win[1]->enable && this->Work[this->inW].win[1]->El_t > 0)
                {
                    ByIntToStr(BarsSettings,temp[1],&this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]]);
                    mvwaddstr(this->win[4],0,0,temp[1]);

                    bzero(temp[1],PATH_MAX);
                }

                #ifdef __FILESYSTEM_INFORMATION_ENABLE__
                if ((BarsSettings & B_FTYPE) == B_FTYPE)
                {
                    sprintf(temp[2]," %p",this->fs.f_type);
                    strcpy(temp[1],temp[2]);
                }

                if ((BarsSettings & B_SFTYPE) == B_SFTYPE)
                {
                    switch (this->fs.f_type)
                    {
                        case 0xadf5:  strcat(temp[1]," ADFS_SUPER_MAGIC"); break;
                        case 0xadff:  strcat(temp[1]," AFFS_SUPER_MAGIC"); break;
                        case 0x5346414F:  strcat(temp[1]," AFS_SUPER_MAGIC"); break;
                        case 0x0187:  strcat(temp[1]," AUTOFS_SUPER_MAGIC"); break;
                        case 0x73757245:  strcat(temp[1]," CODA_SUPER_MAGIC"); break;
                        case 0x28cd3d45:  strcat(temp[1]," CRAMFS_MAGIC"); break;
                        case 0x453dcd28:  strcat(temp[1]," CRAMFS_MAGIC_WEND"); break;
                        case 0x64626720:  strcat(temp[1]," DEBUGFS_MAGIC"); break;
                        case 0x73636673:  strcat(temp[1]," SECURITYFS_MAGIC"); break;
                        case 0xf97cff8c:  strcat(temp[1]," SELINUX_MAGIC"); break;
                        case 0x43415d53:  strcat(temp[1]," SMACK_MAGIC"); break;
                        case 0x858458f6:  strcat(temp[1]," RAMFS_MAGIC"); break;
                        case 0x01021994:  strcat(temp[1]," TMPFS_MAGIC"); break;
                        case 0x958458f6:  strcat(temp[1]," HUGETLBFS_MAGIC"); break;
                        case 0x73717368:  strcat(temp[1]," SQUASHFS_MAGIC"); break;
                        case 0xf15f:  strcat(temp[1]," ECRYPTFS_SUPER_MAGIC"); break;
                        case 0x414A53:  strcat(temp[1]," EFS_SUPER_MAGIC"); break;
                        case 0xE0F5E1E2:  strcat(temp[1]," EROFS_SUPER_MAGIC_V1"); break;
                        case 0xEF53:  strcat(temp[1]," EXT_SUPER_MAGIC"); break;
                        case 0xabba1974:  strcat(temp[1]," XENFS_SUPER_MAGIC"); break;
                        case 0x9123683E:  strcat(temp[1]," BTRFS_SUPER_MAGIC"); break;
                        case 0x3434:  strcat(temp[1]," NILFS_SUPER_MAGIC"); break;
                        case 0xF2F52010:  strcat(temp[1]," F2FS_SUPER_MAGIC"); break;
                        case 0xf995e849:  strcat(temp[1]," HPFS_SUPER_MAGIC"); break;
                        case 0x9660:  strcat(temp[1]," ISOFS_SUPER_MAGIC"); break;
                        case 0x72b6:  strcat(temp[1]," JFFS2_SUPER_MAGIC"); break;
                        case 0x58465342:  strcat(temp[1]," XFS_SUPER_MAGIC"); break;
                        case 0x6165676C:  strcat(temp[1]," PSTOREFS_MAGIC"); break;
                        case 0xde5e81e4:  strcat(temp[1]," EFIVARFS_MAGIC"); break;
                        case 0x00c0ffee:  strcat(temp[1]," HOSTFS_SUPER_MAGIC"); break;
                        case 0x794c7630:  strcat(temp[1]," OVERLAYFS_SUPER_MAGIC"); break;
                        case 0x137F:  strcat(temp[1]," MINIX_SUPER_MAGIC"); break;
                        case 0x138F:  strcat(temp[1]," MINIX_SUPER_MAGIC2"); break;
                        case 0x2468:  strcat(temp[1]," MINIX2_SUPER_MAGIC"); break;
                        case 0x2478:  strcat(temp[1]," MINIX2_SUPER_MAGIC2"); break;
                        case 0x4d5a:  strcat(temp[1]," MINIX3_SUPER_MAGIC"); break;
                        case 0x4d44:  strcat(temp[1]," MSDOS_SUPER_MAGIC"); break;
                        case 0x564c:  strcat(temp[1]," NCP_SUPER_MAGIC"); break;
                        case 0x6969:  strcat(temp[1]," NFS_SUPER_MAGIC"); break;
                        case 0x7461636f:  strcat(temp[1]," OCFS2_SUPER_MAGIC"); break;
                        case 0x9fa1:  strcat(temp[1]," OPENPROM_SUPER_MAGIC"); break;
                        case 0x002f:  strcat(temp[1]," QNX4_SUPER_MAGIC"); break;
                        case 0x68191122:  strcat(temp[1]," QNX6_SUPER_MAGIC"); break;
                        case 0x6B414653:  strcat(temp[1]," AFS_FS_MAGIC"); break;
                        case 0x52654973:  strcat(temp[1]," REISERFS_SUPER_MAGIC"); break;
                        case 0x517B:  strcat(temp[1]," SMB_SUPER_MAGIC"); break;
                        case 0x27e0eb:  strcat(temp[1]," CGROUP_SUPER_MAGIC"); break;
                        case 0x63677270:  strcat(temp[1]," CGROUP2_SUPER_MAGIC"); break;
                        case 0x7655821:  strcat(temp[1]," RDTGROUP_SUPER_MAGIC"); break;
                        case 0x57AC6E9D:  strcat(temp[1]," STACK_END_MAGIC"); break;
                        case 0x74726163:  strcat(temp[1]," TRACEFS_MAGIC"); break;
                        case 0x01021997:  strcat(temp[1]," V9FS_MAGIC"); break;
                        case 0x62646576:  strcat(temp[1]," BDEVFS_MAGIC"); break;
                        case 0x64646178:  strcat(temp[1]," DAXFS_MAGIC"); break;
                        case 0x42494e4d:  strcat(temp[1]," BINFMTFS_MAGIC"); break;
                        case 0x1cd1:  strcat(temp[1]," DEVPTS_SUPER_MAGIC"); break;
                        case 0x6c6f6f70:  strcat(temp[1]," BINDERFS_SUPER_MAGIC"); break;
                        case 0xBAD1DEA:  strcat(temp[1]," FUTEXFS_SUPER_MAGIC"); break;
                        case 0x50495045:  strcat(temp[1]," PIPEFS_MAGIC"); break;
                        case 0x9fa0:  strcat(temp[1]," PROC_SUPER_MAGIC"); break;
                        case 0x534F434B:  strcat(temp[1]," SOCKFS_MAGIC"); break;
                        case 0x62656572:  strcat(temp[1]," SYSFS_MAGIC"); break;
                        case 0x9fa2:  strcat(temp[1]," USBDEVICE_SUPER_MAGIC"); break;
                        case 0x11307854:  strcat(temp[1]," MTD_INODE_FS_MAGIC"); break;
                        case 0x09041934:  strcat(temp[1]," ANON_INODE_FS_MAGIC"); break;
                        case 0x73727279:  strcat(temp[1]," BTRFS_TEST_MAGIC"); break;
                        case 0x6e736673:  strcat(temp[1]," NSFS_MAGIC"); break;
                        case 0xcafe4a11:  strcat(temp[1]," BPF_FS_MAGIC"); break;
                        case 0x5a3c69f0:  strcat(temp[1]," AAFS_MAGIC"); break;
                        case 0x5a4f4653:  strcat(temp[1]," ZONEFS_MAGIC"); break;
                        case 0x15013346:  strcat(temp[1]," UDF_SUPER_MAGIC"); break;
                        case 0x13661366:  strcat(temp[1]," BALLOON_KVM_MAGIC"); break;
                        case 0x58295829:  strcat(temp[1]," ZSMALLOC_MAGIC"); break;
                        case 0x444d4142:  strcat(temp[1]," DMA_BUF_MAGIC"); break;
                        case 0x454d444d:  strcat(temp[1]," DEVMEM_MAGIC"); break;
                        case 0x33:  strcat(temp[1]," Z3FOLD_MAGIC"); break;
                        case 0xc7571590:  strcat(temp[1]," PPC_CMM_MAGIC"); break;
                    }
                }

                if ((BarsSettings & B_FBSIZE) == B_FBSIZE)
                {
                    sprintf(temp[2]," %ld",this->fs.f_bsize);
                    strcat(temp[1],temp[2]);
                }

                if ((BarsSettings & B_FBLOCKS) == B_FBLOCKS)
                {
                    sprintf(temp[2]," %ld",this->fs.f_blocks);
                    strcat(temp[1],temp[2]);
                }

                #ifdef __HUMAN_READABLE_SIZE_ENABLE__
                if ((BarsSettings & B_FHBLOCKS) == B_FHBLOCKS)
                {
                    MakeHumanReadAble(temp[2],this->fs.f_blocks*this->fs.f_bsize,false);
                    strcat(temp[1],temp[2]);
                }

                if ((BarsSettings & B_FHBFREE) == B_FHBFREE)
                {
                    MakeHumanReadAble(temp[2],this->fs.f_bfree*this->fs.f_bsize,false);
                    strcat(temp[1],temp[2]);
                }

                if ((BarsSettings & B_FHBAVAIL) == B_FHBAVAIL)
                {
                    MakeHumanReadAble(temp[2],this->fs.f_bavail*this->fs.f_bsize,false);
                    strcat(temp[1],temp[2]);
                }
                #endif

                if ((BarsSettings & B_FBFREE) == B_FBFREE)
                {
                    sprintf(temp[2]," %ld",this->fs.f_bfree);
                    strcat(temp[1],temp[2]);
                }

                if ((BarsSettings & B_FBAVAIL) == B_FBAVAIL)
                {
                    sprintf(temp[2]," %ld",this->fs.f_bavail);
                    strcat(temp[1],temp[2]);
                }

                if ((BarsSettings & B_FFILES) == B_FFILES)
                {
                    sprintf(temp[2]," %ld",this->fs.f_files);
                    strcat(temp[1],temp[2]);
                }

                if ((BarsSettings & B_FFFREE) == B_FFFREE)
                {
                    sprintf(temp[2]," %ld",this->fs.f_ffree);
                    strcat(temp[1],temp[2]);
                }

                if ((BarsSettings & B_FFSID) == B_FFSID)
                {
                    sprintf(temp[2]," %d %d",this->fs.f_fsid.__val[0],this->fs.f_fsid.__val[1]);
                    strcat(temp[1],temp[2]);
                }

                if ((BarsSettings & B_FNAMELEN) == B_FNAMELEN)
                {
                    sprintf(temp[2]," %ld",this->fs.f_namelen);
                    strcat(temp[1],temp[2]);
                }

                if ((BarsSettings & B_FFRSIZE) == B_FFRSIZE)
                {
                    sprintf(temp[2]," %ld",this->fs.f_frsize);
                    strcat(temp[1],temp[2]);
                }

                if ((BarsSettings & B_FFLAGS) == B_FFLAGS)
                {
                    sprintf(temp[2]," %ld",this->fs.f_flags);
                    strcat(temp[1],temp[2]);
                }
                #endif

                if (!this->Work[this->inW].win[1]->enable && this->Work[this->inW].win[1]->El_t > 0)
                {
                    if ((BarsSettings & B_POSITION) == B_POSITION)
                    {
                        sprintf(temp[2]," %ld/%ld",this->Work[this->inW].win[1]->selected[this->inW]+1,this->Work[this->inW].win[1]->El_t);
                        strcat(temp[1],temp[2]);
                    }
                }
            }

            mvwaddstr(this->win[4],0,this->win[4]->_maxx-strlen(temp[1])+1,temp[1]);

            wrefresh(this->win[4]);
            for (int i = 0; i < this->win[4]->_maxx+1; i++)
                mvwaddch(this->win[4],0,i,' ');
            //4
        }
    }

    free(temp[0]);
    free(temp[1]);
    free(temp[2]);
}

void freeBasic(Basic* this)
{
    for (int i = 0; i < this->ActualSize; i++)
        if (this->Base[i].enable)
            pthread_cancel(this->Base[i].thread);

    for (int i = 0; i < 4; i++)
        delwin(this->win[i]);

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
    free(this->NameHost);
    free(this);
}
