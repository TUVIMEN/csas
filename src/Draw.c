/*
    csas - terminal file manager
    Copyright (C) 2020 TUVIMEN <suchora.dominik7@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "main.h"
#include "Draw.h"
#include "FastRun.h"
#include "Sort.h"
#include "Usefull.h"

extern Settings* settings;

void DrawText(WINDOW* grf, int fd, char* buffer, off_t offset, int whence, bool wrap)
{
    lseek(fd,offset,whence);
    int posx = 1+settings->Borders*2, posy = settings->Borders;

    while (read(fd,buffer,1) > 0 && posy <= grf->_maxy-settings->Borders)
    {
        if ((wrap && grf->_maxx-(settings->Borders*2)-1 < posx))
        {
            posy++;
            posx = 1+settings->Borders*2;
            mvwaddch(grf,posy,posx++,buffer[0]);
            continue;
        }
        if (buffer[0] == '\n')
        {
            posy++;
            posx = 1+settings->Borders*2;
            continue;
        }
        else
            mvwaddch(grf,posy,posx++,buffer[0]);
    }
}

void SetBorders(Basic* grf, const int which)
{
    if ((which == -1 || which == 0) && settings->Win1Enable)
    {
        wattron(grf->win[0],settings->C_Borders);
        wborder(grf->win[0],settings->WindowBorder[0],settings->WindowBorder[1],settings->WindowBorder[2],settings->WindowBorder[3],settings->WindowBorder[4],settings->WindowBorder[5],settings->WindowBorder[6],settings->WindowBorder[7]);
        wattroff(grf->win[0],settings->C_Borders);
    }
    if (which == -1 || which == 1)
    {
        wattron(grf->win[1],settings->C_Borders);
        wborder(grf->win[1],settings->WindowBorder[0],settings->WindowBorder[1],settings->WindowBorder[2],settings->WindowBorder[3],settings->WindowBorder[4],settings->WindowBorder[5],settings->WindowBorder[6],settings->WindowBorder[7]);
        wattroff(grf->win[1],settings->C_Borders);
    }
    if ((which == -1 || which == 2) && settings->Win3Enable)
    {
        wattron(grf->win[2],settings->C_Borders);
        wborder(grf->win[2],settings->WindowBorder[0],settings->WindowBorder[1],settings->WindowBorder[2],settings->WindowBorder[3],settings->WindowBorder[4],settings->WindowBorder[5],settings->WindowBorder[6],settings->WindowBorder[7]);
        wattroff(grf->win[2],settings->C_Borders);
    }
}

static int ColorEl(const struct Element* src, const bool Select)
{
    int set = 0, col = 0;

    #ifdef __MODE_ENABLE__
    if (src->flags & S_IXUSR)
    {
        set |= settings->C_Exec_set;
        col = settings->C_Exec;
    }
    #endif

    if (Select)
        set |= settings->C_Selected;

    switch(src->Type)
    {
        case T_REG:
            #ifdef __COLOR_FILES_BY_EXTENSION__
            switch(src->FType)
            {
                case 'A': col = settings->C_FType_A; break;
                case 'I': col = settings->C_FType_I; break;
                case 'V': col = settings->C_FType_V; break;
            }
            #endif
            break;
        case T_LREG: col = settings->C_LReg; break;
        case T_DIR: col = settings->C_Dir; break;
        case T_LDIR: col = settings->C_LDir; break;
        case T_LSOCK: col = settings->C_LSock; break;
        case T_SOCK: col = settings->C_Sock; break;
        case T_FIFO: col = settings->C_Fifo; break;
        case T_LFIFO: col = settings->C_LFifo; break;
        case T_DEV: col = settings->C_Dev; break;
        case T_LDEV: col = settings->C_LDev; break;
        case T_BLINK: col = settings->C_BLink; break;
        case T_BDEV: col = settings->C_BDev; break;
        case T_LBDEV: col = settings->C_LBDev; break;
    }

    return set | col;
}

#ifdef __FILESYSTEM_INFORMATION_ENABLE__
static char* fsName(const long int ftype)
{
    switch (ftype)
    {
        case 0xadf5:      return " ADFS_SUPER_MAGIC";
        case 0xadff:      return " AFFS_SUPER_MAGIC";
        case 0x5346414F:  return " AFS_SUPER_MAGIC";
        case 0x0187:      return " AUTOFS_SUPER_MAGIC";
        case 0x73757245:  return " CODA_SUPER_MAGIC";
        case 0x28cd3d45:  return " CRAMFS_MAGIC";
        case 0x453dcd28:  return " CRAMFS_MAGIC_WEND";
        case 0x64626720:  return " DEBUGFS_MAGIC";
        case 0x73636673:  return " SECURITYFS_MAGIC";
        case 0xf97cff8c:  return " SELINUX_MAGIC";
        case 0x43415d53:  return " SMACK_MAGIC";
        case 0x858458f6:  return " RAMFS_MAGIC";
        case 0x01021994:  return " TMPFS_MAGIC";
        case 0x958458f6:  return " HUGETLBFS_MAGIC";
        case 0x73717368:  return " SQUASHFS_MAGIC";
        case 0xf15f:      return " ECRYPTFS_SUPER_MAGIC";
        case 0x414A53:    return " EFS_SUPER_MAGIC";
        case 0xE0F5E1E2:  return " EROFS_SUPER_MAGIC_V1";
        case 0xEF53:      return " EXT_SUPER_MAGIC";
        case 0xabba1974:  return " XENFS_SUPER_MAGIC";
        case 0x9123683E:  return " BTRFS_SUPER_MAGIC";
        case 0x3434:      return " NILFS_SUPER_MAGIC";
        case 0xF2F52010:  return " F2FS_SUPER_MAGIC";
        case 0xf995e849:  return " HPFS_SUPER_MAGIC";
        case 0x9660:      return " ISOFS_SUPER_MAGIC";
        case 0x72b6:      return " JFFS2_SUPER_MAGIC";
        case 0x58465342:  return " XFS_SUPER_MAGIC";
        case 0x6165676C:  return " PSTOREFS_MAGIC";
        case 0xde5e81e4:  return " EFIVARFS_MAGIC";
        case 0x00c0ffee:  return " HOSTFS_SUPER_MAGIC";
        case 0x794c7630:  return " OVERLAYFS_SUPER_MAGIC";
        case 0x137F:      return " MINIX_SUPER_MAGIC";
        case 0x138F:      return " MINIX_SUPER_MAGIC2";
        case 0x2468:      return " MINIX2_SUPER_MAGIC";
        case 0x2478:      return " MINIX2_SUPER_MAGIC2";
        case 0x4d5a:      return " MINIX3_SUPER_MAGIC";
        case 0x4d44:      return " MSDOS_SUPER_MAGIC";
        case 0x564c:      return " NCP_SUPER_MAGIC";
        case 0x6969:      return " NFS_SUPER_MAGIC";
        case 0x7461636f:  return " OCFS2_SUPER_MAGIC";
        case 0x9fa1:      return " OPENPROM_SUPER_MAGIC";
        case 0x002f:      return " QNX4_SUPER_MAGIC";
        case 0x68191122:  return " QNX6_SUPER_MAGIC";
        case 0x6B414653:  return " AFS_FS_MAGIC";
        case 0x52654973:  return " REISERFS_SUPER_MAGIC";
        case 0x517B:      return " SMB_SUPER_MAGIC";
        case 0x27e0eb:    return " CGROUP_SUPER_MAGIC";
        case 0x63677270:  return " CGROUP2_SUPER_MAGIC";
        case 0x7655821:   return " RDTGROUP_SUPER_MAGIC";
        case 0x57AC6E9D:  return " STACK_END_MAGIC";
        case 0x74726163:  return " TRACEFS_MAGIC";
        case 0x01021997:  return " V9FS_MAGIC";
        case 0x62646576:  return " BDEVFS_MAGIC";
        case 0x64646178:  return " DAXFS_MAGIC";
        case 0x42494e4d:  return " BINFMTFS_MAGIC";
        case 0x1cd1:      return " DEVPTS_SUPER_MAGIC";
        case 0x6c6f6f70:  return " BINDERFS_SUPER_MAGIC";
        case 0xBAD1DEA:   return " FUTEXFS_SUPER_MAGIC";
        case 0x50495045:  return " PIPEFS_MAGIC";
        case 0x9fa0:      return " PROC_SUPER_MAGIC";
        case 0x534F434B:  return " SOCKFS_MAGIC";
        case 0x62656572:  return " SYSFS_MAGIC";
        case 0x9fa2:      return " USBDEVICE_SUPER_MAGIC";
        case 0x11307854:  return " MTD_INODE_FS_MAGIC";
        case 0x09041934:  return " ANON_INODE_FS_MAGIC";
        case 0x73727279:  return " BTRFS_TEST_MAGIC";
        case 0x6e736673:  return " NSFS_MAGIC";
        case 0xcafe4a11:  return " BPF_FS_MAGIC";
        case 0x5a3c69f0:  return " AAFS_MAGIC";
        case 0x5a4f4653:  return " ZONEFS_MAGIC";
        case 0x15013346:  return " UDF_SUPER_MAGIC";
        case 0x13661366:  return " BALLOON_KVM_MAGIC";
        case 0x58295829:  return " ZSMALLOC_MAGIC";
        case 0x444d4142:  return " DMA_BUF_MAGIC";
        case 0x454d444d:  return " DEVMEM_MAGIC";
        case 0x33:        return " Z3FOLD_MAGIC";
        case 0xc7571590:  return " PPC_CMM_MAGIC";
    }
    return " UNKNOWN";
}
#endif

static void ByIntToStr(size_t* size, const int Settings, char* result, struct Element* grf)
{
    #ifdef __FILE_GROUPS_ENABLE__
    struct group  *gr;
    #endif
    #ifdef __FILE_OWNERS_ENABLE__
    struct passwd *pw;
    #endif
    *size = 0;

    #ifdef __MODE_ENABLE__
    if (Settings&DP_LSPERMS)
        *size += sprintf(result+*size,"%s ",lsperms(grf->flags,grf->Type));
    #endif
    #ifdef __FILE_SIZE_ENABLE__
    if (Settings&DP_SIZE)
        *size += sprintf(result+*size,"%lld ",grf->size);
    #endif
    #ifdef __HUMAN_READABLE_SIZE_ENABLE__
    if (Settings&DP_HSIZE)
    {
        if (grf->SizErrToDisplay == NULL)
        {
            grf->SizErrToDisplay = (char*)malloc(16);
            MakeHumanReadAble(grf->SizErrToDisplay,grf->size,
            ((settings->DirSizeMethod&D_H) != D_H)&&(grf->Type == T_DIR || grf->Type == T_LDIR));
        }
        *size += sprintf(result+*size,"%s ",grf->SizErrToDisplay);
    }
    #endif
    #ifdef __BLOCKS_ENABLE__
    if (Settings&DP_BLOCKS)
        *size += sprintf(result+*size,"%ld ",grf->blocks);
    #endif
    if (Settings&DP_TYPE)
        *size += sprintf(result+*size,"%d ",grf->Type);
    #ifdef __COLOR_FILES_BY_EXTENSION__
    if (Settings&DP_FTYPE && grf->FType > 32)
        *size += sprintf(result+*size,"%c ",grf->FType);
    #endif
    #ifdef __NLINK_ENABLE__
    if (Settings&DP_NLINK)
        *size += sprintf(result+*size,"%ld ",grf->nlink);
    #endif
    #ifdef __INODE_ENABLE__
    if (Settings&DP_INODE)
        *size += sprintf(result+*size,"%ld ",grf->inode);
    #endif
    #ifdef __FILE_OWNERS_ENABLE__
    if (grf->pw != 1001) //!
    {
        pw = getpwuid(grf->pw);
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
    if (grf->gr != 1001)
    {
        gr = getgrgid(grf->gr);

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
        *size += sprintf(result+*size,"%ld ",grf->atim.tv_sec);
    if (Settings&DP_SATIME)
        *size += TimeToStr(&grf->atim.tv_sec,result+*size);
    #endif
    #ifdef __MTIME_ENABLE__
    if (Settings&DP_MTIME)
        *size += sprintf(result+*size,"%ld ",grf->mtim.tv_sec);
    if (Settings&DP_SMTIME)
        *size += TimeToStr(&grf->mtim.tv_sec,result+*size);
    #endif
    #ifdef __CTIME_ENABLE__
    if (Settings&DP_CTIME)
        *size += sprintf(result+*size,"%ld ",grf->ctim.tv_sec);
    if (Settings&DP_SCTIME)
        *size += TimeToStr(&grf->ctim.tv_sec,result+*size);
    #endif
    #ifdef __BLK_SIZE_ENABLE__
    if (Settings&DP_BLK_SIZE)
        *size += sprintf(result+*size,"%ld ",grf->blksize);
    #endif
    #ifdef __DEV_ENABLE__
    if (Settings&DP_DEV)
        *size += sprintf(result+*size,"%ld ",grf->dev);
    #endif
    #ifdef __RDEV_ENABLE__
    if (Settings&DP_RDEV)
        *size += sprintf(result+*size,"%ld ",grf->rdev);
    #endif

}

void DrawBasic(Basic* grf, const int which)
{
    static int color, line_off1, line_off2;;
    static char NameTemp[NAME_MAX];
    static char MainTemp[PATH_MAX];
    static char temp[96];
    static size_t cont_s[4];

    if (settings->Win3Display && grf->Work[grf->inW].win[2] == -1)
        FastRun(grf);

    for (int i = 0; i < 3; i++)
    {
        if (which != -1 && i != which)
            continue;
        if (grf->Work[grf->inW].win[i] == -1)
            continue;
        if (i == 0 && (!settings->Win1Enable || !settings->Win1Display))
            continue;
        if (i == 2 && (!settings->Win3Enable
        #ifdef __THREADS_FOR_DIR_ENABLE__
        || GET_DIR(grf->inW,1).enable
        #endif
        ))
            continue;

        if (i == 2 && !settings->Win3Display && grf->preview_fd != -1 && grf->FastRunSettings&F_TEXT)
        {
            DrawText(grf->win[i],grf->preview_fd,temp,0,SEEK_SET,grf->FastRunSettings&F_WRAP);
            wrefresh(grf->win[i]);
            continue;
        }

        if (i == 2 && !settings->Win3Display)
            continue;

        wattron(grf->win[i],settings->C_Error);
        #ifdef __THREADS_FOR_DIR_ENABLE__
        if (GET_DIR(grf->inW,i).enable)
        {
            snprintf(NameTemp,grf->win[i]->_maxx-((settings->Borders+1)+2),"LOADING");
            mvwaddstr(grf->win[i],settings->Borders,settings->Borders+3,NameTemp);
            wattroff(grf->win[i],settings->C_Error);
            wrefresh(grf->win[i]);
            continue;
        }
        #endif
        if ((long long int)GET_DIR(grf->inW,i).El_t == (long long int)-1)
        {
            snprintf(NameTemp,grf->win[i]->_maxx-((settings->Borders+1)+2),"NOT ACCESSIBLE");
            mvwaddstr(grf->win[i],settings->Borders,settings->Borders+3,NameTemp);
            wattroff(grf->win[i],settings->C_Error);
            wrefresh(grf->win[i]);
            continue;
        }
        if ((long long int)GET_DIR(grf->inW,i).El_t == (long long int)0)
        {
            snprintf(NameTemp,grf->win[i]->_maxx-((settings->Borders+1)+2),"EMPTY");
            mvwaddstr(grf->win[i],settings->Borders,settings->Borders+3,NameTemp);
            wattroff(grf->win[i],settings->C_Error);
            wrefresh(grf->win[i]);
            continue;
        }
        wattroff(grf->win[i],settings->C_Error);

        line_off1 = 0;

        for (long long int j = GET_DIR(grf->inW,i).Ltop[grf->inW]; j-GET_DIR(grf->inW,i).Ltop[grf->inW] < (size_t)grf->win[i]->_maxy-(settings->Borders*2)+1 && j < GET_DIR(grf->inW,i).El_t; j++)
        {
            #ifdef __COLOR_FILES_BY_EXTENSION__
            if (GET_DIR(grf->inW,i).El[j].FType == 1)
                GET_DIR(grf->inW,i).El[j].FType = CheckFileExtension(GET_DIR(grf->inW,i).El[j].name);
            #endif
            color = ColorEl(&GET_DIR(grf->inW,i).El[j],(j == (long long int)GET_SELECTED(grf->inW,i)));

            #ifdef __SORT_ELEMENTS_ENABLE__
            if (GET_DIR(grf->inW,i).sort_m != settings->SortMethod)
            {
                GET_DIR(grf->inW,i).sort_m = settings->SortMethod;
                if (GET_DIR(grf->inW,i).El_t > 0)
                    SortEl(GET_DIR(grf->inW,i).El,GET_DIR(grf->inW,i).El_t,settings->SortMethod);
            }
            #endif

            if (settings->FillBlankSpace)
                wattron(grf->win[i],color);
            for (int g = settings->Borders+1; g < grf->win[i]->_maxx-settings->Borders-1+((i == 2)*2)*!settings->Borders+(((i == 1)*2)*!settings->Win3Enable)*!settings->Borders; g++)
                mvwaddch(grf->win[i],settings->Borders+j-GET_DIR(grf->inW,i).Ltop[grf->inW],g+settings->Borders,' ');
            if (!settings->FillBlankSpace)
                wattron(grf->win[i],color);

            cont_s[0] = 0;
            MainTemp[0] = '\0';
            NameTemp[0] = '\0';
            line_off2 = 0;

            if (i == 1)
            {
                if (settings->DisplayingC != 0)
                {
                    ByIntToStr(&cont_s[0],settings->DisplayingC,MainTemp,&GET_DIR(grf->inW,i).El[j]);

                    mvwaddstr(grf->win[i],settings->Borders+j-GET_DIR(grf->inW,i).Ltop[grf->inW],grf->win[i]->_maxx-cont_s[0]-1+(((i == 1)*2)*!settings->Win3Enable)*!settings->Borders,MainTemp);
                }

                if (settings->NumberLinesOff)
                {
                    line_off1 = 0;
                    cont_s[1] = GET_DIR(grf->inW,i).El_t;
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

            strcat(NameTemp,GET_DIR(grf->inW,i).El[j].name);
            cont_s[1] = strlen(NameTemp);

            if ((long long int)grf->win[i]->_maxx < (long long int)(4+cont_s[0]+settings->Borders+1))
                NameTemp[0] = '\0';
            else if ((size_t)cont_s[1] > grf->win[i]->_maxx-cont_s[0]-2-((settings->Borders+1)+1)-settings->Borders)
            {
                NameTemp[grf->win[i]->_maxx-cont_s[0]-2-((settings->Borders+1)+1)-settings->Borders] = '~';
                NameTemp[grf->win[i]->_maxx-cont_s[0]-1-((settings->Borders+1)+1)-settings->Borders] = '\0';
            }

            mvwaddstr(grf->win[i],settings->Borders+j-GET_DIR(grf->inW,i).Ltop[grf->inW],(settings->Borders*2)+2+(line_off1-line_off2),NameTemp);

            wattroff(grf->win[i],color);

            if (GET_DIR(grf->inW,i).El[j].List[grf->inW]&grf->Work[grf->inW].SelectedGroup)
            {
                switch (grf->Work[grf->inW].SelectedGroup)
                {
                    case 1<<0:
                        color = settings->C_Group_0;
                        break;
                    case 1<<1:
                        color = settings->C_Group_1;
                        break;
                    case 1<<2:
                        color = settings->C_Group_2;
                        break;
                    case 1<<3:
                        color = settings->C_Group_3;
                        break;
                    case 1<<4:
                        color = settings->C_Group_4;
                        break;
                    case 1<<5:
                        color = settings->C_Group_5;
                        break;
                    case 1<<6:
                        color = settings->C_Group_6;
                        break;
                    case 1<<7:
                        color = settings->C_Group_7;
                        break;
                }
            }
            else
                color = 0;

            wattron(grf->win[i],(color|A_REVERSE)*(color > 0));
            mvwaddch(grf->win[i],settings->Borders+j-GET_DIR(grf->inW,i).Ltop[grf->inW],(settings->Borders*2),' ');
            wattroff(grf->win[i],(color|A_REVERSE)*(color > 0));
        }

        if (settings->Borders)
            SetBorders(grf,-1);
        wrefresh(grf->win[i]);
    }

    if ((which == 3 || which == -1) && settings->Bar1Enable)
    {
        // 3
        cont_s[2] = 0;

        if ((settings->Bar1Settings & B_WORKSPACES) == B_WORKSPACES)
        {
            for (int i = 0; i < WORKSPACE_N; i++)
                    cont_s[2] += grf->Work[i].exists;

            cont_s[2] *= 3;
        }

        cont_s[3] = 0;

        #ifdef __USER_NAME_ENABLE__
        if ((settings->Bar1Settings & B_UHNAME) == B_UHNAME)
        {
            wattron(grf->win[3],settings->C_User_S_D);
            if (settings->UserRHost)
                sprintf(temp,settings->UserHostPattern,grf->H_Host,grf->H_User);
            else
                sprintf(temp,settings->UserHostPattern,grf->H_User,grf->H_Host);
            mvwaddstr(grf->win[3],0,0,temp);
            wattroff(grf->win[3],settings->C_User_S_D);
            cont_s[3] = strlen(temp);
        }
        #endif

        if (
            #ifdef __THREADS_FOR_DIR_ENABLE__
            !GET_DIR(grf->inW,1).enable &&
            #endif
            GET_DIR(grf->inW,1).El_t > 0)
        {
            if ((settings->Bar1Settings & B_DIR) == B_DIR)
            {
                strcpy(MainTemp,GET_DIR(grf->inW,1).path);
                if (!(GET_DIR(grf->inW,1).path[0] == '/' && GET_DIR(grf->inW,1).path[1] == '\0'))
                {
                    strcat(MainTemp,"/");
                    MakePathShorter(MainTemp,grf->win[3]->_maxx-(cont_s[3]+1+cont_s[2]+strlen(GET_ESELECTED(grf->inW,1).name)));
                }

                wattron(grf->win[3],settings->C_Bar_Dir);
                mvwprintw(grf->win[3],0,cont_s[3]," %s",MainTemp);
                wattroff(grf->win[3],settings->C_Bar_Dir);

                cont_s[3] += strlen(MainTemp)+1;
            }
            if ((settings->Bar1Settings & B_NAME) == B_NAME)
            {
                wattron(grf->win[3],settings->C_Bar_Name);
                mvwaddstr(grf->win[3],0,cont_s[3],GET_ESELECTED(grf->inW,1).name);
                wattroff(grf->win[3],settings->C_Bar_Name);
            }
        }

        if ((settings->Bar1Settings & B_WORKSPACES) == B_WORKSPACES)
        {
            cont_s[2] /= 3;

            if (cont_s[2] > 1)
            {
                cont_s[2] = 2;
                for (int i = WORKSPACE_N-1; i > -1; i--)
                {
                    if (grf->Work[i].exists)
                    {
                        if (i == grf->inW)
                            wattron(grf->win[3],settings->C_Bar_WorkSpace_Selected);
                        else
                            wattron(grf->win[3],settings->C_Bar_WorkSpace);

                        mvwprintw(grf->win[3],0,grf->win[3]->_maxx-cont_s[2]," %d ",i);

                        if (i == grf->inW)
                            wattroff(grf->win[3],settings->C_Bar_WorkSpace_Selected);
                        else
                            wattroff(grf->win[3],settings->C_Bar_WorkSpace);

                        cont_s[2] += 3;
                    }
                }
            }
        }

        wrefresh(grf->win[3]);
        werase(grf->win[3]);
        // 3
    }

    if (settings->Bar2Enable && (which == -1 || which == 4))
    {
        // 4
        if (
            #ifdef __THREADS_FOR_DIR_ENABLE__
            !GET_DIR(grf->inW,1).enable &&
            #endif
            GET_DIR(grf->inW,1).El_t > 0)
        {
            ByIntToStr(&cont_s[0],settings->Bar2Settings,MainTemp,&GET_ESELECTED(grf->inW,1));
            wattron(grf->win[4],settings->C_Bar_F);
            mvwaddstr(grf->win[4],0,0,MainTemp);
            wattroff(grf->win[4],settings->C_Bar_E);

            MainTemp[0] = '\0';
        }

        cont_s[0] = 0;

        if (settings->Bar1Settings & B_CSF && grf->cSF_E)
            cont_s[0] += sprintf(MainTemp+cont_s[0],"%s",grf->cSF);
        if (settings->Bar1Settings & B_MODES)
        {
            #ifdef __INOTIFY_ENABLE__
            if (GET_DIR(grf->inW,1).Changed)
                cont_s[0] += sprintf(MainTemp+cont_s[0]," C");
            #endif

            if (grf->Work[grf->inW].Visual)
                cont_s[0] += sprintf(MainTemp+cont_s[0]," VISUAL");
        }
        if (settings->Bar1Settings & B_FGROUP)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %dW",grf->Work[grf->inW].SelectedGroup);
        #ifdef __FILESYSTEM_INFORMATION_ENABLE__
        if (settings->Bar1Settings & B_FTYPE)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %lx",grf->fs.f_type);
        if (settings->Bar1Settings & B_SFTYPE)
            cont_s[0] += sprintf(MainTemp+cont_s[0],fsName(grf->fs.f_type));
        if (settings->Bar1Settings & B_FBSIZE)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld",grf->fs.f_bsize);
        if (settings->Bar1Settings & B_FBLOCKS)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld",grf->fs.f_blocks);
        #ifdef __HUMAN_READABLE_SIZE_ENABLE__
        if (settings->Bar1Settings & B_FHBLOCKS)
        {
            MakeHumanReadAble(temp,grf->fs.f_blocks*grf->fs.f_bsize,false);
            cont_s[0] += sprintf(MainTemp+cont_s[0],"%s",temp);
        }
        if (settings->Bar1Settings & B_FHBFREE)
        {
            MakeHumanReadAble(temp,grf->fs.f_bfree*grf->fs.f_bsize,false);
            cont_s[0] += sprintf(MainTemp+cont_s[0],"%s",temp);
        }
        if (settings->Bar1Settings & B_FHBAVAIL)
        {
            MakeHumanReadAble(temp,grf->fs.f_bavail*grf->fs.f_bsize,false);
            cont_s[0] += sprintf(MainTemp+cont_s[0],"%s",temp);
        }
        #endif
        if (settings->Bar1Settings & B_FBFREE)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld",grf->fs.f_bfree);
        if (settings->Bar1Settings & B_FBAVAIL)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld",grf->fs.f_bavail);
        if (settings->Bar1Settings & B_FFILES)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld",grf->fs.f_files);
        if (settings->Bar1Settings & B_FFFREE)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld",grf->fs.f_ffree);
        if (settings->Bar1Settings & B_FFSID)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %d %d",grf->fs.f_fsid.__val[0],grf->fs.f_fsid.__val[1]);
        if (settings->Bar1Settings & B_FNAMELEN)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld",grf->fs.f_namelen);
        if (settings->Bar1Settings & B_FFRSIZE)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld",grf->fs.f_frsize);
        if (settings->Bar1Settings & B_FFLAGS)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld",grf->fs.f_flags);
        #endif
        if (
            #ifdef __THREADS_FOR_DIR_ENABLE__
            !GET_DIR(grf->inW,1).enable &&
            #endif
            GET_DIR(grf->inW,1).El_t > 0)
        {
            if (settings->Bar1Settings & B_POSITION)
                cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld/%lld",GET_SELECTED(grf->inW,1)+1,GET_DIR(grf->inW,1).El_t);
        }

        wattron(grf->win[4],settings->C_Bar_E);
        mvwaddstr(grf->win[4],0,grf->win[4]->_maxx-cont_s[0]+1,MainTemp);
        wattroff(grf->win[4],settings->C_Bar_E);

        wrefresh(grf->win[4]);
        werase(grf->win[4]);
        // 4
    }
}
