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
#include "load.h"

#ifdef __SORT_ELEMENTS_ENABLE__
#include "sort.h"
#endif

#include "useful.h"
#include "draw.h"
#include "preview.h"
#include "inits.h"
#include "functions.h"

struct loaddir_s
{
    struct Dir *d;
    #ifdef __FOLLOW_PARENT_DIR__
    Basic *b;
    char *searched_name;
    int workspace;
    int which;
    #endif
};

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

static void *LoadDir(void *arg)
{

    struct Dir *nd = ((struct loaddir_s*)arg)->d;
    #ifdef __FOLLOW_PARENT_DIR__
    Basic *grf = ((struct loaddir_s*)arg)->b;
    char *searched_name = ((struct loaddir_s*)arg)->searched_name;
    int workpace = ((struct loaddir_s*)arg)->workspace;
    int which = ((struct loaddir_s*)arg)->which;
    #endif
    free(arg);

    DIR* d;
    if ((d = opendir(nd->path)) == NULL)
    {
        nd->permission_denied = 1;
        #ifdef __FOLLOW_PARENT_DIR__
        free(searched_name);
        #endif
        #ifdef __THREADS_FOR_DIR_ENABLE__
        nd->enable = false;
        pthread_detach(nd->thread);
        pthread_exit(NULL);
        #else
        return NULL;
        #endif
    }

    struct dirent* dir;

    size_t buffer_size = nd->size;
    struct stat sfile, sfile2;

    int fd;
    
    #ifdef __FILE_SIZE_ENABLE__
    int tfd;
    ull size, count;
    #endif
    fd = dirfd(d);

    bool isOld = nd->size > 0;

    #ifdef __RESCUE_SELECTED_IF_DIR_CHANGE_ENABLE__
    struct Element* oldEl = NULL;
    size_t oldsize = 0, begin, end;

    if (isOld)
    {
        begin = 0;
        end = nd->size;
        oldEl = nd->el;
        oldsize = nd->size;
        nd->el = NULL;
        nd->size = 0;
    }
    #else
    if (isOld)
        freeEl(&nd->el,&nd->size);
    #endif

    buffer_size = 0;
    size_t name_lenght;

    while ((dir = readdir(d)))
    {
        #ifdef __HIDE_FILES__
        if (dir->d_name[0] == '.')
            continue;
        #endif
        if (dir->d_name[0] == '.' && (dir->d_name[1] == '\0' || (dir->d_name[1] == '.' && dir->d_name[2] == '\0')))
            continue;

        if (nd->size == buffer_size)
            nd->el = (struct Element*)realloc(nd->el,(buffer_size+=DIR_INC_RATE)*sizeof(struct Element));

        name_lenght = strlen(dir->d_name)+1;
        nd->el[nd->size].nlen = name_lenght;
        nd->el[nd->size].name = memcpy(malloc(name_lenght),dir->d_name,name_lenght);

        nd->el[nd->size].type = 0;

        fstatat(fd,dir->d_name,&sfile,AT_SYMLINK_NOFOLLOW);

        if ((sfile.st_mode&S_IFMT) == S_IFLNK)
        {
            if (fstatat(fd,dir->d_name,&sfile2,0) == -1)
                nd->el[nd->size].type |= T_FILE_MISSING;
            else
            {
                nd->el[nd->size].type = mode_to_type(sfile2.st_mode);
                nd->el[nd->size].type |= T_SYMLINK;
            }
        }
        else
            nd->el[nd->size].type = mode_to_type(sfile.st_mode);

        #ifdef __INODE_ENABLE__
        nd->el[nd->size].inode = dir->d_ino;
        #endif

        #ifdef __MTIME_ENABLE__
        nd->el[nd->size].mtim = sfile.st_mtim.tv_sec;
        #endif
        #ifdef __ATIME_ENABLE__
        nd->el[nd->size].atim = sfile.st_atim.tv_sec;
        #endif
        #ifdef __CTIME_ENABLE__
        nd->el[nd->size].ctim = sfile.st_ctim.tv_sec;
        #endif
        #ifdef __MODE_ENABLE__
        nd->el[nd->size].mode = sfile.st_mode;
        #endif

        #ifdef __FILE_SIZE_ENABLE__
        if ((settings->DirSizeMethod&D_F) != D_F && (nd->el[nd->size].type&T_GT) == T_DIR)
        {
            if (faccessat(fd,dir->d_name,R_OK,0) == 0 && (tfd = openat(fd,dir->d_name,O_DIRECTORY)) != -1)
            {
                count = 0;
                size = 0;
                GetDirSize(tfd,&count,&size,(settings->DirSizeMethod&D_R)==D_R);
                nd->el[nd->size].size = (settings->DirSizeMethod&D_C)==D_C ? count : size;
                close(tfd);
            }
            else
                nd->el[nd->size].size = 0;
        }
        else
            nd->el[nd->size].size = sfile.st_size*((nd->el[nd->size].type&T_SYMLINK) == 0)
            +sfile2.st_size*((nd->el[nd->size].type&T_SYMLINK) != 0);
        #endif

        #ifdef __COLOR_FILES_BY_EXTENSION__
        nd->el[nd->size].ftype = 1;
        #endif

        memset(nd->el[nd->size].list,0,WORKSPACE_N);
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
                        memcpy(nd->el[nd->size].list,oldEl[i].list,WORKSPACE_N);
                        break;
                    }
            }
        }
        #endif

        #ifdef __FILE_OWNERS_ENABLE__
        nd->el[nd->size].pw = sfile.st_uid;
        #endif
        #ifdef __FILE_GROUPS_ENABLE__
        nd->el[nd->size].gr = sfile.st_gid;
        #endif

        #ifdef __DEV_ENABLE__
        nd->el[nd->size].dev = sfile.st_dev;
        #endif
        #ifdef __NLINK_ENABLE__
        nd->el[nd->size].nlink = sfile.st_nlink;
        #endif
        #ifdef __RDEV_ENABLE__
        nd->el[nd->size].rdev = sfile.st_rdev;
        #endif
        #ifdef __BLK_SIZE_ENABLE__
        nd->el[nd->size].blksize = sfile.st_blksize;
        #endif
        #ifdef __BLOCKS_ENABLE__
        nd->el[nd->size].blocks = sfile.st_blocks;
        #endif

        nd->size++;
    }

    #ifdef __RESCUE_SELECTED_IF_DIR_CHANGE_ENABLE__
    if (isOld)
        freeEl(&oldEl,&oldsize);
    #endif

    closedir(d);
    close(fd);

    if (buffer_size != nd->size)
        nd->el = (struct Element*)realloc(nd->el,(nd->size)*sizeof(struct Element));

    if (nd->size > 0)
    {
        #ifdef __SORT_ELEMENTS_ENABLE__
        SortEl(nd->el,nd->size,settings->SortMethod);
        #endif


        #ifdef __FOLLOW_PARENT_DIR__
        if (searched_name)
            move_to(grf,workpace,which,searched_name);
        #endif
    }

    #ifdef __FOLLOW_PARENT_DIR__
    free(searched_name);
    #endif

    #ifdef __THREADS_FOR_DIR_ENABLE__
    nd->enable = false;
    pthread_detach(nd->thread);
    pthread_exit(NULL);
    #else
    return NULL;
    #endif
}

void GetDir(const char* path, Basic* grf, const int workspace, const int Which, const char mode
#ifdef __FOLLOW_PARENT_DIR__
, char *searched_name
#endif
#ifdef __THREADS_FOR_DIR_ENABLE__
, const bool threaded
#endif
)
{
    // mode
    // 0 - if exists exit
    // 1 - if doesn't exist or changed load
    // 2 - always load

    char temp[PATH_MAX];
    if (realpath(path,temp) == NULL)
        err(-1,NULL);

    struct stat sfile1;
    if (stat(path,&sfile1) != 0)
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

    if (mode == 0 && exists)
        return;

    if (mode == 1 && exists && !grf->base[found]->changed)
        return;

    #ifdef __SORT_ELEMENTS_ENABLE__
    grf->base[found]->sort_m = settings->SortMethod;
    #endif

    struct loaddir_s *arg = malloc(sizeof(struct loaddir_s));

    arg->d = grf->base[found];
    #ifdef __FOLLOW_PARENT_DIR__
    arg->b = grf;
    arg->workspace = workspace;
    arg->which = Which;
    if (searched_name)
        arg->searched_name = strdup(searched_name);
    else
        arg->searched_name = NULL;
    #endif

    #ifdef __THREADS_FOR_DIR_ENABLE__
    if (grf->base[found]->enable)
        pthread_join(grf->base[found]->thread,NULL);
    grf->base[found]->enable = true;

    pthread_create(&grf->base[found]->thread,NULL,LoadDir,arg);
    if (!threaded)
        pthread_join(grf->base[found]->thread,NULL);
    #else
    LoadDir(arg);
    #endif
    grf->base[found]->changed = false;
}

void CD(const char* path, const int workspace, Basic* grf)
{
    char npath[PATH_MAX]
    #ifdef __FOLLOW_PARENT_DIR__
    , tpath[PATH_MAX]
    , *t = NULL;
    bool b = strcmp(path,"..") == 0
    #endif
    ;

    #ifdef __FOLLOW_PARENT_DIR__
    if (!settings->Win1Enable && b)
    {
        getcwd(tpath,PATH_MAX);
        t = memrchr(tpath,'/',strlen(tpath));
        if (t)
            t++;
    }
    #endif

    if (realpath(path,npath) == NULL || chdir(path) == -1)
    {
        set_message(grf,COLOR_PAIR(1),"%s: %s",path,strerror(errno));
        return;
    }

    strcpy(grf->workspaces[workspace].path,npath);

    #ifdef __FILESYSTEM_INFO_ENABLE__
    statfs(".",&grf->fs);
    #endif

    werase(grf->win[1]);
    wrefresh(grf->win[1]);

    GetDir(".",grf,workspace,1,settings->DirLoadingMode
    #ifdef __FOLLOW_PARENT_DIR__
    ,t
    #endif
    #ifdef __THREADS_FOR_DIR_ENABLE__
    ,settings->ThreadsForDir
    #endif
    );

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
            #ifdef __FOLLOW_PARENT_DIR__
            t = memrchr(npath,'/',strlen(npath));
            if (t)
                t++;
            #endif

            GetDir("..",grf,workspace,0,settings->DirLoadingMode
            #ifdef __FOLLOW_PARENT_DIR__
            ,t
            #endif
            #ifdef __THREADS_FOR_DIR_ENABLE__
            ,settings->ThreadsForDir
            #endif
            );

            #ifdef __FOLLOW_PARENT_DIR__
            if (t
            #ifdef __THREADS_FOR_DIR_ENABLE__
            && !GET_DIR(workspace,0)->enable
            #endif
            )
                move_to(grf,workspace,0,t);
            #endif

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
