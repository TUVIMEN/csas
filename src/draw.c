/*
    csas - console file manager
    Copyright (C) 2020-2021 TUVIMEN <suchora.dominik7@gmail.com>

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
#include "draw.h"
#include "preview.h"
#include "sort.h"
#include "console.h"
#include "useful.h"

extern Settings *cfg;

void set_message(Csas *cs, const int attr, const char *fmt, ...)
{
    struct WinArgs args = {stdscr,
    {-1,1},{1,-1},{-1,-1},{-1,-1},
    {-1,-1},{-1,1},{-1,-1},{-1,-1},
    0};
    console_resize(cs->win[5],args);
    werase(cs->win[5]);
    va_list va_args;
    va_start(va_args,fmt);
    wattron(cs->win[5],attr);
    wmove(cs->win[5],0,0);
    vw_printw(cs->win[5],fmt,va_args);
    wattroff(cs->win[5],attr);
    va_end(va_args);
    cs->ws[cs->current_ws].show_message = true;
}

void setborders(Csas *cs, const int which)
{
    if ((which == -1 || which == 0) && cfg->Win1Enable)
    {
        wattron(cs->win[0],cfg->C_Borders);
        wborder(cs->win[0],cfg->WindowBorder[0],cfg->WindowBorder[1],cfg->WindowBorder[2],cfg->WindowBorder[3],cfg->WindowBorder[4],cfg->WindowBorder[5],cfg->WindowBorder[6],cfg->WindowBorder[7]);
        wattroff(cs->win[0],cfg->C_Borders);
    }
    if (which == -1 || which == 1)
    {
        wattron(cs->win[1],cfg->C_Borders);
        wborder(cs->win[1],cfg->WindowBorder[0],cfg->WindowBorder[1],cfg->WindowBorder[2],cfg->WindowBorder[3],cfg->WindowBorder[4],cfg->WindowBorder[5],cfg->WindowBorder[6],cfg->WindowBorder[7]);
        wattroff(cs->win[1],cfg->C_Borders);
    }
    if ((which == -1 || which == 2) && cfg->Win3Enable)
    {
        wattron(cs->win[2],cfg->C_Borders);
        wborder(cs->win[2],cfg->WindowBorder[0],cfg->WindowBorder[1],cfg->WindowBorder[2],cfg->WindowBorder[3],cfg->WindowBorder[4],cfg->WindowBorder[5],cfg->WindowBorder[6],cfg->WindowBorder[7]);
        wattroff(cs->win[2],cfg->C_Borders);
    }
}

static int colorel(const struct Element *src, const bool select)
{
    register int set = 0, col = 0;

    #ifdef __MODE_ENABLE__
    if (src->mode & S_IXUSR)
    {
        set |= cfg->C_Exec_set;
        col = cfg->C_Exec_col;
    }
    #endif

    set |= cfg->C_Selected*select;

    if (src->type&T_FILE_MISSING)
        col = cfg->C_FileMissing;
    else
    {
        if (src->type&T_SYMLINK && cfg->C_SymLink)
        {
            col = cfg->C_SymLink;
            goto END;
        }

        switch(src->type&T_GT)
        {
            case T_REG:
                #ifdef __COLOR_FILES_BY_EXTENSION__
                switch(src->ftype)
                {
                    case 'A': col = cfg->C_FType_A; break;
                    case 'I': col = cfg->C_FType_I; break;
                    case 'V': col = cfg->C_FType_V; break;
                }
                #endif
                break;
            case T_DIR: col = cfg->C_Dir; break;
            case T_SOCK: col = cfg->C_Sock; break;
            case T_FIFO: col = cfg->C_Fifo; break;
            case T_DEV: col = cfg->C_Dev; break;
            case T_BDEV: col = cfg->C_BDev; break;
            case T_OTHER: col = cfg->C_Other; break;
        }
    }

    END: ;

    return set | col;
}

#ifdef __FILESYSTEM_INFO_ENABLE__
static char *fsname(const li ftype)
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

static void flagstoa(size_t *size, const int flags, char *result, struct Element *cs)
{
    #ifdef __FILE_GROUPS_ENABLE__
    struct group  *gr;
    #endif
    #ifdef __FILE_OWNERS_ENABLE__
    struct passwd *pw;
    #endif
    *size = 0;

    #ifdef __MODE_ENABLE__
    if (flags&DP_LSPERMS)
        *size += sprintf(result+*size,"%s ",lsperms(cs->mode));
    #endif
    #ifdef __BLOCKS_ENABLE__
    if (flags&DP_BLOCKS)
        *size += sprintf(result+*size,"%ld ",cs->blocks);
    #endif
    if (flags&DP_TYPE)
        *size += sprintf(result+*size,"%d ",cs->type);
    #ifdef __COLOR_FILES_BY_EXTENSION__
    if (flags&DP_FTYPE)
    {
        if (cs->ftype > 32)
            *size += sprintf(result+*size,"%c ",cs->ftype);
        else
            *size += sprintf(result+*size,"%d ",cs->ftype);
    }
    #endif
    #ifdef __NLINK_ENABLE__
    if (flags&DP_NLINK)
        *size += sprintf(result+*size,"%ld ",cs->nlink);
    #endif
    #ifdef __INODE_ENABLE__
    if (flags&DP_INODE)
        *size += sprintf(result+*size,"%ld ",cs->inode);
    #endif
    #ifdef __FILE_OWNERS_ENABLE__
    if ((pw = getpwuid(cs->pw)) != NULL)
    {
        if (flags&DP_PWDIR)
            *size += sprintf(result+*size,"%s ",pw->pw_dir);
        if (flags&DP_PWGECOS)
            *size += sprintf(result+*size,"%s ",pw->pw_gecos);
        if (flags&DP_PWGID)
             *size += sprintf(result+*size,"%d ",pw->pw_gid);
        if (flags&DP_PWNAME)
            *size += sprintf(result+*size,"%s ",pw->pw_name);
        if (flags&DP_PWPASSWD)
            *size += sprintf(result+*size,"%s ",pw->pw_passwd);
        if (flags&DP_PWSHELL)
            *size += sprintf(result+*size,"%s ",pw->pw_shell);
        if (flags&DP_PWUID)
            *size += sprintf(result+*size,"%d ",pw->pw_uid);
    }
    #endif

    #ifdef __FILE_GROUPS_ENABLE__
    if ((gr = getgrgid(cs->gr)) != NULL)
    {
        if (flags&DP_GRGID)
            *size += sprintf(result+*size,"%d ",gr->gr_gid);
        if (flags&DP_GRNAME)
            *size += sprintf(result+*size,"%s ",gr->gr_name);
        if (flags&DP_GRPASSWD)
            *size += sprintf(result+*size,"%s ",gr->gr_passwd);
    }
    #endif

    #ifdef __ATIME_ENABLE__
    if (flags&DP_ATIME)
        *size += sprintf(result+*size,"%ld ",cs->atim);
    if (flags&DP_SATIME)
        *size += ttoa(&cs->atim,result+*size);
    #endif
    #ifdef __MTIME_ENABLE__
    if (flags&DP_MTIME)
        *size += sprintf(result+*size,"%ld ",cs->mtim);
    if (flags&DP_SMTIME)
        *size += ttoa(&cs->mtim,result+*size);
    #endif
    #ifdef __CTIME_ENABLE__
    if (flags&DP_CTIME)
        *size += sprintf(result+*size,"%ld ",cs->ctim);
    if (flags&DP_SCTIME)
        *size += ttoa(&cs->ctim,result+*size);
    #endif
    #ifdef __BLK_SIZE_ENABLE__
    if (flags&DP_BLK_SIZE)
        *size += sprintf(result+*size,"%ld ",cs->blksize);
    #endif
    #ifdef __DEV_ENABLE__
    if (flags&DP_DEV)
        *size += sprintf(result+*size,"%ld ",cs->dev);
    #endif
    #ifdef __RDEV_ENABLE__
    if (flags&DP_RDEV)
        *size += sprintf(result+*size,"%ld ",cs->rdev);
    #endif
    #ifdef __FILE_SIZE_ENABLE__
    if (flags&DP_SIZE)
        *size += sprintf(result+*size,"%lld ",cs->size);
    if (flags&DP_HSIZE)
    {
        if (cs->size < 1024)
            *size += sprintf(result+*size,"%lld ",cs->size);
        else
            *size += sprintf(result+*size,"%s ",stoa(cs->size));
    }
    #endif
    if (flags&DP_LINK_PATH)
    {
        ssize_t temp = readlink(cs->name,result+*size,PATH_MAX);
        *size += (temp != -1)*temp;
        result[(*size)++] = ' ';
        result[(*size)++] = '\0';
    }
}

void csas_draw(Csas *cs, const int which)
{
    int color, line_off1, line_off2;;
    char NameTemp[PATH_MAX];
    char MainTemp[PATH_MAX];
    char temp[96];
    size_t cont_s[4];

    if (cfg->Win3Display && cs->ws[cs->current_ws].win[2] == -1)
        get_preview(cs);

    for (int i = 0; i < 3; i++)
    {
        if (which != -1 && i != which)
            continue;
        if (cs->ws[cs->current_ws].win[i] == -1)
            continue;
        if (i == 0 && (!cfg->Win1Enable || !cfg->Win1Display))
            continue;
        if (i == 2 && (!cfg->Win3Enable
        #ifdef __THREADS_FOR_DIR_ENABLE__
        || G_D(cs->current_ws,1)->enable
        #endif
        ))
            continue;

        if (i == 2 && !cfg->Win3Display)
        {
            #ifndef __SAVE_PREVIEW__
            if (cs->spreview&F_TEXT)
            {
                run_preview(cs->win[i],cs->cpreview,cs->previewl,cs->spreview);
                wrefresh(cs->win[i]);
            }
            #else
            if (G_ES(cs->current_ws,1).spreview&F_TEXT)
            {
                run_preview(cs->win[i],G_ES(cs->current_ws,1).cpreview,
                    G_ES(cs->current_ws,1).previewl,
                    G_ES(cs->current_ws,1).spreview);
                wrefresh(cs->win[i]);
            }
            #endif
            continue;
        }

        wattron(cs->win[i],cfg->C_Error);
        #ifdef __THREADS_FOR_DIR_ENABLE__
        if (G_D(cs->current_ws,i)->enable)
        {
            snprintf(NameTemp,cs->win[i]->_maxx-((cfg->Borders+1)+2),"LOADING");
            mvwaddstr(cs->win[i],cfg->Borders,cfg->Borders+3,NameTemp);
            wattroff(cs->win[i],cfg->C_Error);
            wrefresh(cs->win[i]);
            continue;
        }
        #endif
        if (G_D(cs->current_ws,i)->permission_denied)
        {
            snprintf(NameTemp,cs->win[i]->_maxx-((cfg->Borders+1)+2),"NOT ACCESSIBLE");
            mvwaddstr(cs->win[i],cfg->Borders,cfg->Borders+3,NameTemp);
            wattroff(cs->win[i],cfg->C_Error);
            wrefresh(cs->win[i]);
            continue;
        }
        if (G_D(cs->current_ws,i)->size == 0)
        {
            snprintf(NameTemp,cs->win[i]->_maxx-((cfg->Borders+1)+2),"EMPTY");
            mvwaddstr(cs->win[i],cfg->Borders,cfg->Borders+3,NameTemp);
            wattroff(cs->win[i],cfg->C_Error);
            wrefresh(cs->win[i]);
            continue;
        }
        wattroff(cs->win[i],cfg->C_Error);

        line_off1 = 0;

        #ifdef __SORT_ELEMENTS_ENABLE__
        if (G_D(cs->current_ws,i)->sort_m != cfg->SortMethod)
        {
            G_D(cs->current_ws,i)->sort_m = cfg->SortMethod;
            if (G_D(cs->current_ws,i)->size > 0)
                sort_el(G_D(cs->current_ws,i)->el,G_D(cs->current_ws,i)->size,cfg->SortMethod);
        }
        #endif

        for (size_t j = G_D(cs->current_ws,i)->ltop[cs->current_ws]; j-G_D(cs->current_ws,i)->ltop[cs->current_ws] < (size_t)cs->win[i]->_maxy-(cfg->Borders*2)+1 && j < G_D(cs->current_ws,i)->size; j++)
        {
            #ifdef __COLOR_FILES_BY_EXTENSION__
            if (G_D(cs->current_ws,i)->el[j].ftype == 1)
                G_D(cs->current_ws,i)->el[j].ftype = check_extension(G_D(cs->current_ws,i)->el[j].name);
            #endif
            color = colorel(&G_D(cs->current_ws,i)->el[j],(j == G_S(cs->current_ws,i)));

            if (cfg->FillBlankSpace)
                wattron(cs->win[i],color);
            for (int g = cfg->Borders+1; g < cs->win[i]->_maxx-cfg->Borders-1+((i == 2)*2)*!cfg->Borders+(((i == 1)*2)*!cfg->Win3Enable)*!cfg->Borders; g++)
                mvwaddch(cs->win[i],cfg->Borders+j-G_D(cs->current_ws,i)->ltop[cs->current_ws],g+cfg->Borders,' ');
            if (!cfg->FillBlankSpace)
                wattron(cs->win[i],color);

            cont_s[0] = 0;
            MainTemp[0] = '\0';
            NameTemp[0] = '\0';
            line_off2 = 0;

            if (i == 1)
            {
                if (cfg->DisplayingC != 0)
                {
                    flagstoa(&cont_s[0],cfg->DisplayingC,MainTemp,&G_D(cs->current_ws,i)->el[j]);

                    mvwaddstr(cs->win[i],cfg->Borders+j-G_D(cs->current_ws,i)->ltop[cs->current_ws],cs->win[i]->_maxx-cont_s[0]-1+(((i == 1)*2)*!cfg->Win3Enable)*!cfg->Borders,MainTemp);
                }

                if (cfg->NumberLinesOff)
                {
                    line_off1 = 0;
                    cont_s[1] = G_D(cs->current_ws,i)->size;
                    while (cont_s[1] > 9) { cont_s[1] /= 10; line_off1++; }

                    line_off2 = 0;
                    cont_s[1] = j;
                    while (cont_s[1] > 9) { cont_s[1] /= 10; line_off2++; };
                }

                if (cfg->NumberLines)
                {
					sprintf(temp,"%lu ",j+cfg->NumberLinesFromOne);
                    strcat(NameTemp,temp);
                }
            }

            strcat(NameTemp,G_D(cs->current_ws,i)->el[j].name);
            cont_s[1] = strlen(NameTemp);

            if (cs->win[i]->_maxx < (ll)(cont_s[0]+cfg->Borders+1))
                NameTemp[0] = 0;
            else if ((size_t)cont_s[1] > cs->win[i]->_maxx-cont_s[0]-2-((cfg->Borders+1)+1)-cfg->Borders)
            {
                NameTemp[cs->win[i]->_maxx-cont_s[0]-3-((cfg->Borders+1)+1)-cfg->Borders] = '~';
                NameTemp[cs->win[i]->_maxx-cont_s[0]-2-((cfg->Borders+1)+1)-cfg->Borders] = '\0';
            }

            /*if ((ll)cs->win[i]->_maxx < (ll)(4+cont_s[0]+cfg->Borders+1))
                NameTemp[0] = '\0';
            else if ((size_t)cont_s[1] > cs->win[i]->_maxx-cont_s[0]-2-((cfg->Borders+1)+1)-cfg->Borders)
            {
                NameTemp[cs->win[i]->_maxx-cont_s[0]-3-((cfg->Borders+1)+1)-cfg->Borders] = '~';
                NameTemp[cs->win[i]->_maxx-cont_s[0]-2-((cfg->Borders+1)+1)-cfg->Borders] = '\0';
            }*/

            mvwaddstr(cs->win[i],cfg->Borders+j-G_D(cs->current_ws,i)->ltop[cs->current_ws],(cfg->Borders*2)+2+(line_off1-line_off2),NameTemp);

            wattroff(cs->win[i],color);

            if (G_D(cs->current_ws,i)->el[j].list[cs->current_ws]&(1<<cs->ws[cs->current_ws].sel_group))
                color = cfg->C_Group[cs->ws[cs->current_ws].sel_group];
            else
                color = 0;

            wattron(cs->win[i],(color|A_REVERSE)*(color > 0));
            mvwaddch(cs->win[i],cfg->Borders+j-G_D(cs->current_ws,i)->ltop[cs->current_ws],(cfg->Borders*2),' ');
            wattroff(cs->win[i],(color|A_REVERSE)*(color > 0));
        }

        if (cfg->Borders)
            setborders(cs,-1);
        wrefresh(cs->win[i]);
    }

    if ((which == 3 || which == -1) && cfg->Bar1Enable)
    {
        // 3
        cont_s[2] = 0;

        if ((cfg->Bar1Settings & B_WORKSPACES) == B_WORKSPACES)
        {
            for (int i = 0; i < WORKSPACE_N; i++)
                cont_s[2] += cs->ws[i].exists;

            cont_s[2] *= 3;
        }

        cont_s[3] = 0;

        #ifdef __USER_NAME_ENABLE__
        if ((cfg->Bar1Settings & B_UHNAME) == B_UHNAME)
        {
            wattron(cs->win[3],cfg->C_User_S_D);
            if (cfg->UserRHost)
                sprintf(temp,cfg->UserHostPattern,cs->hostn,cs->usern);
            else
                sprintf(temp,cfg->UserHostPattern,cs->usern,cs->hostn);
            mvwaddstr(cs->win[3],0,0,temp);
            wattroff(cs->win[3],cfg->C_User_S_D);
            cont_s[3] = strlen(temp);
        }
        #endif

        if (
            #ifdef __THREADS_FOR_DIR_ENABLE__
            !G_D(cs->current_ws,1)->enable &&
            #endif
            G_D(cs->current_ws,1)->size > 0)
        {
            if (G_D(cs->current_ws,1)->size <= G_S(cs->current_ws,1))
                G_S(cs->current_ws,1) = G_D(cs->current_ws,1)->size-1;
            if ((cfg->Bar1Settings & B_DIR) == B_DIR)
            {
                strcpy(MainTemp,cs->ws[cs->current_ws].path);
                if (!(cs->ws[cs->current_ws].path[0] == '/' && cs->ws[cs->current_ws].path[1] == '\0'))
                {
                    strcat(MainTemp,"/");
                    path_shrink(MainTemp,cs->win[3]->_maxx-(cont_s[3]+1+cont_s[2]+((cfg->Bar1Settings & B_NAME) == B_NAME ? strlen(G_ES(cs->current_ws,1).name) : 0)));
                }

                wattron(cs->win[3],cfg->C_Bar_Dir);
                mvwprintw(cs->win[3],0,cont_s[3]," %s",MainTemp);
                wattroff(cs->win[3],cfg->C_Bar_Dir);
                
                cont_s[3] += strlen(MainTemp)+1;
            }
            if ((cfg->Bar1Settings & B_NAME) == B_NAME)
            {
                wattron(cs->win[3],cfg->C_Bar_Name);
                mvwaddstr(cs->win[3],0,cont_s[3],G_ES(cs->current_ws,1).name);
                wattroff(cs->win[3],cfg->C_Bar_Name);
            }
        }

        if ((cfg->Bar1Settings & B_WORKSPACES) == B_WORKSPACES)
        {
            cont_s[2] /= 3;

            if (cont_s[2] > 1)
            {
                cont_s[2] = 2;
                for (int i = WORKSPACE_N-1; i > -1; i--)
                {
                    if (cs->ws[i].exists)
                    {
                        if (i == cs->current_ws)
                            wattron(cs->win[3],cfg->C_Bar_WorkSpace_Selected);
                        else
                            wattron(cs->win[3],cfg->C_Bar_WorkSpace);

                        mvwprintw(cs->win[3],0,cs->win[3]->_maxx-cont_s[2]," %d ",i);

                        if (i == cs->current_ws)
                            wattroff(cs->win[3],cfg->C_Bar_WorkSpace_Selected);
                        else
                            wattroff(cs->win[3],cfg->C_Bar_WorkSpace);

                        cont_s[2] += 3;
                    }
                }
            }
        }

        wrefresh(cs->win[3]);
        werase(cs->win[3]);
        // 3
    }

    if (cfg->Bar2Enable && (which == -1 || which == 4))
    {
        // 4
        if (
            #ifdef __THREADS_FOR_DIR_ENABLE__
            !G_D(cs->current_ws,1)->enable &&
            #endif
            G_D(cs->current_ws,1)->size > 0)
        {
            flagstoa(&cont_s[0],cfg->Bar2Settings,MainTemp,&G_ES(cs->current_ws,1));
            wattron(cs->win[4],cfg->C_Bar_F);
            mvwaddstr(cs->win[4],0,0,MainTemp);
            wattroff(cs->win[4],cfg->C_Bar_E);

            MainTemp[0] = '\0';
        }

        cont_s[0] = 0;

        if (cfg->Bar1Settings & B_CSF && cs->was_typed)
            cont_s[0] += sprintf(MainTemp+cont_s[0],"%s",cs->typed_keys);
        if (cfg->Bar1Settings & B_MODES)
        {
            if (G_D(cs->current_ws,1)->changed)
                cont_s[0] += sprintf(MainTemp+cont_s[0]," C");

            if (cs->ws[cs->current_ws].visual)
                cont_s[0] += sprintf(MainTemp+cont_s[0]," VISUAL");

            if (G_D(cs->current_ws,1)->filter_set)
                cont_s[0] += sprintf(MainTemp+cont_s[0]," f=\"%s\"",G_D(cs->current_ws,1)->filter);
        }
        if (cfg->Bar1Settings & B_FGROUP)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %dW",cs->ws[cs->current_ws].sel_group);
        #ifdef __FILESYSTEM_INFO_ENABLE__
        if (cfg->Bar1Settings & B_FTYPE)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %lx",cs->fs.f_type);
        if (cfg->Bar1Settings & B_SFTYPE)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %s",fsname(cs->fs.f_type));
        if (cfg->Bar1Settings & B_FBSIZE)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld",cs->fs.f_bsize);
        if (cfg->Bar1Settings & B_FBLOCKS)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld",cs->fs.f_blocks);
        if (cfg->Bar1Settings & B_FHBLOCKS)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %s",stoa(cs->fs.f_blocks*cs->fs.f_bsize));
        if (cfg->Bar1Settings & B_FHBFREE)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %s",stoa(cs->fs.f_bfree*cs->fs.f_bsize));
        if (cfg->Bar1Settings & B_FHBAVAIL)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %s",stoa(cs->fs.f_bavail*cs->fs.f_bsize));
        if (cfg->Bar1Settings & B_FBFREE)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld",cs->fs.f_bfree);
        if (cfg->Bar1Settings & B_FBAVAIL)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld",cs->fs.f_bavail);
        if (cfg->Bar1Settings & B_FFILES)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld",cs->fs.f_files);
        if (cfg->Bar1Settings & B_FFFREE)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld",cs->fs.f_ffree);
        if (cfg->Bar1Settings & B_FFSID)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %d %d",cs->fs.f_fsid.__val[0],cs->fs.f_fsid.__val[1]);
        if (cfg->Bar1Settings & B_FNAMELEN)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld",cs->fs.f_namelen);
        if (cfg->Bar1Settings & B_FFRSIZE)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld",cs->fs.f_frsize);
        if (cfg->Bar1Settings & B_FFLAGS)
            cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld",cs->fs.f_flags);
        #endif
        if (
            #ifdef __THREADS_FOR_DIR_ENABLE__
            !G_D(cs->current_ws,1)->enable &&
            #endif
            G_D(cs->current_ws,1)->size > 0)
        {
            if (cfg->Bar1Settings & B_POSITION)
                cont_s[0] += sprintf(MainTemp+cont_s[0]," %ld/%lu",G_S(cs->current_ws,1)+1,G_D(cs->current_ws,1)->size);
        }

        wattron(cs->win[4],cfg->C_Bar_E);
        mvwaddstr(cs->win[4],0,cs->win[4]->_maxx-cont_s[0]+1,MainTemp);
        wattroff(cs->win[4],cfg->C_Bar_E);

        wrefresh(cs->win[4]);
        werase(cs->win[4]);
        // 4
    }

    if (cs->ws[cs->current_ws].show_message)
    {
        cs->ws[cs->current_ws].show_message = false;
        wrefresh(cs->win[5]);
    }
}
