/*
    csas - console file manager
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
#include "load.h"

#ifdef __SORT_ELEMENTS_ENABLE__
#include "sort.h"
#endif

#include "useful.h"
#include "draw.h"
#include "preview.h"
#include "inits.h"
#include "functions.h"

extern Settings* settings;

static uchar mode_to_type(const mode_t mode)
{
    switch (mode & S_IFMT)
    {
        case S_IFBLK:  return T_BDEV;
        case S_IFCHR:  return T_DEV;
        case S_IFDIR:  return T_DIR;
        case S_IFIFO:  return T_FIFO;
        case S_IFREG:  return T_REG;
        case S_IFSOCK: return T_SOCK;
        default:       return T_OTHER;
    }
    return 0;
}

static void* LoadDir(void *arg)
{
    struct Dir* grf = (struct Dir*)arg;

    DIR* d;
    if ((d = opendir(grf->path)) == NULL)
    {
        grf->permission_denied = 1;
        #ifdef __THREADS_FOR_DIR_ENABLE__
        grf->enable = false;
        pthread_detach(grf->thread);
        pthread_exit(NULL);
        #else
        return NULL;
        #endif
    }

    struct dirent* dir;

    size_t buffer_size = grf->size;
    struct stat sfile, sfile2;

    int fd;
    
    #ifdef __FILE_SIZE_ENABLE__
    int tfd;
    ull size, count;
    #endif
    fd = dirfd(d);

    bool isOld = grf->size > 0;

    #ifdef __RESCUE_SELECTED_IF_DIR_CHANGE_ENABLE__
    struct Element* oldEl = NULL;
    size_t oldsize = 0, begin, end;

    if (isOld)
    {
        begin = 0;
        end = grf->size;
        oldEl = grf->el;
        oldsize = grf->size;
        grf->el = NULL;
        grf->size = 0;
    }
    #else
    if (isOld)
        freeEl(&grf->el,&grf->size);
    #endif

    buffer_size = 0;
    size_t name_lenght;

    while ((dir = readdir(d)))
    {
        #ifdef __SHOW_HIDDEN_FILES_ENABLE__
        if(!ShowHiddenFiles)
            if (dir->d_name[0] == '.')
                continue;
        #endif
        if (dir->d_name[0] == '.' && (dir->d_name[1] == '\0' || (dir->d_name[1] == '.' && dir->d_name[2] == '\0')))
            continue;

        if (grf->size == buffer_size)
            grf->el = (struct Element*)realloc(grf->el,(buffer_size+=DIR_INC_RATE)*sizeof(struct Element));

        name_lenght = strlen(dir->d_name)+1;
        grf->el[grf->size].nlen = name_lenght;
        grf->el[grf->size].name = memcpy(malloc(name_lenght),dir->d_name,name_lenght);

        grf->el[grf->size].type = 0;

        fstatat(fd,dir->d_name,&sfile,AT_SYMLINK_NOFOLLOW);

        if ((sfile.st_mode&S_IFMT) == S_IFLNK)
        {
            if (fstatat(fd,dir->d_name,&sfile2,0) == -1)
                grf->el[grf->size].type |= T_FILE_MISSING;
            else
            {
                grf->el[grf->size].type = mode_to_type(sfile2.st_mode);
                grf->el[grf->size].type |= T_SYMLINK;
            }
        }
        else
            grf->el[grf->size].type = mode_to_type(sfile.st_mode);

        #ifdef __INODE_ENABLE__
        grf->el[grf->size].inode = dir->d_ino;
        #endif

        #ifdef __MTIME_ENABLE__
        grf->el[grf->size].mtim = sfile.st_mtim.tv_sec;
        #endif
        #ifdef __ATIME_ENABLE__
        grf->el[grf->size].atim = sfile.st_atim.tv_sec;
        #endif
        #ifdef __CTIME_ENABLE__
        grf->el[grf->size].ctim = sfile.st_ctim.tv_sec;
        #endif
        #ifdef __MODE_ENABLE__
        grf->el[grf->size].mode = sfile.st_mode;
        #endif

        #ifdef __FILE_SIZE_ENABLE__
        if ((settings->DirSizeMethod&D_F) != D_F && (grf->el[grf->size].type&T_GT) == T_DIR)
        {
            if (faccessat(fd,dir->d_name,R_OK,0) == 0 && (tfd = openat(fd,dir->d_name,O_DIRECTORY)) != -1)
            {
                count = 0;
                size = 0;
                GetDirSize(tfd,&count,&size,(settings->DirSizeMethod&D_R)==D_R);
                grf->el[grf->size].size = (settings->DirSizeMethod&D_C)==D_C ? count : size;
                close(tfd);
            }
            else
                grf->el[grf->size].size = 0;
        }
        else
            grf->el[grf->size].size = sfile.st_size*((grf->el[grf->size].type&T_SYMLINK) == 0)
            +sfile2.st_size*((grf->el[grf->size].type&T_SYMLINK) != 0);
        #endif

        #ifdef __COLOR_FILES_BY_EXTENSION__
        grf->el[grf->size].ftype = 1;
        #endif

        memset(grf->el[grf->size].list,0,WORKSPACE_N);
        #ifdef __RESCUE_SELECTED_IF_DIR_CHANGE_ENABLE__
        if (isOld)
        {
            for (register size_t i = begin; i < end; i++)
            {
                #ifdef __RESCUE_SELECTED_IF_DIR_CHANGE_ENABLE__
                    #ifdef __FAST_RESCUE__
                    if (oldEl[i].inode == dir->d_ino)
                    #else
                    if (strcmp(oldEl[i].name,dir->d_name) == 0)
                    #endif
                #endif
                    #ifdef __CHECK_IF_FILE_HAS_THE_SAME_NAME__
                    if (strcmp(oldEl[i].name,dir->d_name) == 0)
                    #endif
                    {
                        begin += 1*(i == begin);
                        end -= (i == end);
                        memcpy(grf->el[grf->size].list,oldEl[i].list,WORKSPACE_N);
                        break;
                    }
            }
        }
        #endif

        #ifdef __FILE_OWNERS_ENABLE__
        grf->el[grf->size].pw = sfile.st_uid;
        #endif
        #ifdef __FILE_GROUPS_ENABLE__
        grf->el[grf->size].gr = sfile.st_gid;
        #endif

        #ifdef __DEV_ENABLE__
        grf->el[grf->size].dev = sfile.st_dev;
        #endif
        #ifdef __NLINK_ENABLE__
        grf->el[grf->size].nlink = sfile.st_nlink;
        #endif
        #ifdef __RDEV_ENABLE__
        grf->el[grf->size].rdev = sfile.st_rdev;
        #endif
        #ifdef __BLK_SIZE_ENABLE__
        grf->el[grf->size].blksize = sfile.st_blksize;
        #endif
        #ifdef __BLOCKS_ENABLE__
        grf->el[grf->size].blocks = sfile.st_blocks;
        #endif

        grf->size++;
    }

    #ifdef __RESCUE_SELECTED_IF_DIR_CHANGE_ENABLE__
    if (isOld)
        freeEl(&oldEl,&oldsize);
    #endif

    closedir(d);
    close(fd);

    if (buffer_size != grf->size)
        grf->el = (struct Element*)realloc(grf->el,(grf->size)*sizeof(struct Element));

    if (grf->size > 0)
    {
        #ifdef __SORT_ELEMENTS_ENABLE__
        SortEl(grf->el,grf->size,settings->SortMethod);
        #endif
    }

    #ifdef __THREADS_FOR_DIR_ENABLE__
    grf->enable = false;
    pthread_detach(grf->thread);
    pthread_exit(NULL);
    #else
    return NULL;
    #endif
}

void GetDir(const char* path, Basic* grf, const int workspace, const int Which, const char mode
#ifdef __THREADS_FOR_DIR_ENABLE__
,const bool threaded
#endif
)
{
    // mode
    // 0 - if exists exit
    // 1 - if doesn't exist or changed load
    // 2 - always load

    static char temp[PATH_MAX];
    strcpy(temp,grf->workspaces[workspace].path);

    if (strcmp(path,".") != 0)
    {
        size_t end;
        if (path[0] == '/')
            strcpy(temp,path);
        else if (strcmp(path,"..") == 0)
        {
            end = strlen(temp)-1;
            for (; end != 1 && temp[end] != '/'; end--) temp[end] = 0;
            temp[end] = 0;
        }
        else
        {
            end = strlen(temp);
            if (temp[end-1] != '/')
            {
                temp[end] = '/';
                strcpy(temp+end+1,path);
            }
            else
                strcpy(temp+end,path);
        }
    }

    struct stat sfile1;
    if (stat(temp,&sfile1) != 0)
        return;

    bool exists = false;

    int found = -1;
    
    for (size_t i = 0; i < grf->size; i++)
    {
        if (strcmp(grf->base[i]->path,temp) == 0)
        {
            found = i;
            exists = true;
            break;
        }
    }

    if (found == -1)
    {
        if (grf->size == grf->asize)
        {
            grf->base = (struct Dir**)realloc(grf->base,(grf->asize+=DIR_BASE_STABLE_RATE)*sizeof(struct Dir*));
            for (size_t i = grf->asize-DIR_BASE_STABLE_RATE; i < grf->asize; i++)
            {
                grf->base[i] = (struct Dir*)malloc(sizeof(struct Dir));
                grf->base[i]->el = NULL;
                grf->base[i]->size = 0;
                #ifdef __THREADS_FOR_DIR_ENABLE__
                grf->base[i]->enable = false;
                #endif
                grf->base[i]->path = NULL;
                grf->base[i]->ltop = (size_t*)calloc(WORKSPACE_N,sizeof(size_t));
                grf->base[i]->selected = (size_t*)calloc(WORKSPACE_N,sizeof(size_t));
                grf->base[i]->move_to = (char**)calloc(WORKSPACE_N,sizeof(char*));
                grf->base[i]->changed = false;
                grf->base[i]->oldsize = 0;
                grf->base[i]->filter_set = false;
                grf->base[i]->filter = NULL;
            }
        }
        found = grf->size++;
    }

    grf->workspaces[workspace].win[Which] = found;

    if (!exists)
    {
        grf->base[found]->permission_denied = 0;
        grf->base[found]->inode = sfile1.st_ino;
        grf->base[found]->ctime = sfile1.st_ctim;
        grf->base[found]->path = strcpy(malloc(PATH_MAX),temp);
    }
    else
    {
        if (sfile1.st_ctim.tv_sec != grf->base[found]->ctime.tv_sec || sfile1.st_ctim.tv_nsec != grf->base[found]->ctime.tv_nsec)
        {
            grf->base[found]->ctime = sfile1.st_ctim;
            grf->base[found]->changed = true;
        }
    }

    if (grf->base[found]->permission_denied)
        return;

    size_t begin = 0;
    if (settings->Win1Enable)
    {
        strcpy(temp,grf->workspaces[workspace].path);
        begin = strlen(temp)-1;
        while (temp[begin] != '/') begin--;
        begin++;
    }

    if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        !grf->base[found]->enable &&
        #endif
        (exists && (mode == 0 || (mode == 1 && !grf->base[found]->changed))) && strcmp(path,"..") == 0)
    {
        if (strcmp(temp+begin,GET_ESELECTED(workspace,Which).name) != 0)
            move_to(GET_DIR(workspace,!settings->Win1Enable),workspace,temp+begin);
    }

    if (mode == 0 && exists)
        return;

    if (mode == 1 && exists && !grf->base[found]->changed)
        return;

    #ifdef __SORT_ELEMENTS_ENABLE__
    grf->base[found]->sort_m = settings->SortMethod;
    #endif

    #ifdef __THREADS_FOR_DIR_ENABLE__
    if (grf->base[found]->enable)
        pthread_join(grf->base[found]->thread,NULL);
    grf->base[found]->enable = true;

    pthread_create(&grf->base[found]->thread,NULL,LoadDir,grf->base[found]);
    if (!threaded)
        pthread_join(grf->base[found]->thread,NULL);
    #else
    LoadDir(grf->base[found]);
    #endif
    grf->base[found]->changed = false;
    
    if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        !grf->base[found]->enable &&
        #endif
        strcmp(path,"..") == 0)
    {
        if (strcmp(temp+begin,GET_ESELECTED(workspace,Which).name) != 0)
            move_to(GET_DIR(workspace,!settings->Win1Enable),workspace,temp+begin);
    }
}

void CD(const char* path, const int workspace, Basic* grf)
{
    if (chdir(grf->workspaces[workspace].path) != 0)
		return;
    if (chdir(path) != 0)
        return;

    size_t end;

    static char temp[PATH_MAX];
    size_t begin = 0;
    if (!settings->Win1Enable)
    {
        strcpy(temp,grf->workspaces[workspace].path);
        begin = strlen(temp)-1;
        while (temp[begin] != '/') begin--;
        begin++;
    }

    if (strcmp(path,".") != 0)
    {
        if (path[0] == '/')
            strcpy(grf->workspaces[workspace].path,path);
        else if (strcmp(path,"..") == 0)
        {
            end = strlen(grf->workspaces[workspace].path)-1;
            for (; end != 1 && grf->workspaces[workspace].path[end] != '/'; end--) grf->workspaces[workspace].path[end] = 0;
            grf->workspaces[workspace].path[end] = 0;
        }
        else
        {
            end = strlen(grf->workspaces[workspace].path);
            if (grf->workspaces[workspace].path[end-1] != '/')
            {
                grf->workspaces[workspace].path[end] = '/';
                strcpy(grf->workspaces[workspace].path+end+1,path);
            }
            else
                strcpy(grf->workspaces[workspace].path+end,path);
        }
    }

    #ifdef __FILESYSTEM_INFO_ENABLE__
    statfs(".",&grf->fs);
    #endif

    werase(grf->win[1]);
    wrefresh(grf->win[1]);

    GetDir(".",grf,workspace,1,settings->DirLoadingMode
    #ifdef __THREADS_FOR_DIR_ENABLE__
    ,settings->ThreadsForDir
    #endif
    );

    if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        !GET_DIR(workspace,1)->enable &&
        #endif
        !settings->Win1Enable && strcmp(path,"..") == 0)
    {
        if (strcmp(temp+begin,GET_ESELECTED(workspace,1).name) != 0)
            move_to(GET_DIR(workspace,1),workspace,temp+begin);
    }

    if (settings->Win1Enable)
    {
        werase(grf->win[0]);
        if (settings->Borders)
            SetBorders(grf,0);
        wrefresh(grf->win[0]);

        if (GET_DIR(workspace,1)->path[0] == '/' && GET_DIR(workspace,1)->path[1] == '\0')
            settings->Win1Display = false;
        else
        {
            GetDir("..",grf,workspace,0,settings->DirLoadingMode
            #ifdef __THREADS_FOR_DIR_ENABLE__
            ,settings->ThreadsForDir
            #endif
            );
            settings->Win1Display = true;
        }
    }

    if (workspace == grf->current_workspace && settings->Win3Enable)
    {
        if (GET_DIR(workspace,1)->size == 0)
        {
            werase(grf->win[2]);
            if (settings->Borders)
                SetBorders(grf,2);
            wrefresh(grf->win[2]);
        }
        if (
            #ifdef __THREADS_FOR_DIR_ENABLE__
            !GET_DIR(workspace,1)->enable &&
            #endif
            GET_DIR(workspace,1)->size > 0)
            Preview(grf);
    }
}
