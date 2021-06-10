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

struct loaddir_s {
    struct xdir *d;
    #ifdef __FOLLOW_PARENT_DIR__
    char *searched_name;
    #endif
    #if defined(__THREADS_FOR_DIR_ENABLE__) || defined(__FOLLOW_PARENT_DIR__)
    csas *b;
    int ws;
    int which;
    #endif
};

extern li s_DirSizeMethod;
extern li s_SortMethod;
extern li s_Win3Enable;
extern li s_ThreadsForDir;
extern li s_DirLoadingMode;
extern li s_Borders;
extern li s_Win1Enable;
extern li s_Win1Display;

#ifdef __UPDATE_FILES__
void updatefile(struct xfile *xf, const char *path)
{
    int fd = open(path,O_DIRECTORY);
    if (fd == -1) return;
    struct stat st;
    if (fstatat(fd,xf->name,&st,0) == -1) {
        close(fd);
        return;
    }

    close(fd);

    #ifdef __INODE_ENABLE__
    xf->inode = st.st_ino;
    #endif

    #ifdef __MTIME_ENABLE__
    xf->mtim = st.st_mtim.tv_sec;
    #endif
    #ifdef __ATIME_ENABLE__
    xf->atim = st.st_atim.tv_sec;
    #endif
    #ifdef __CTIME_ENABLE__
    xf->ctim = st.st_ctim.tv_sec;
    #endif
    #ifdef __MODE_ENABLE__
    xf->mode = st.st_mode;
    #endif

    #ifdef __FILE_SIZE_ENABLE__
    if ((xf->type&T_GT) != T_DIR) {
        xf->size = st.st_size*((xf->type&T_SYMLINK) == 0)
        +st.st_size*((xf->type&T_SYMLINK) != 0);
    }
    #endif

    #ifdef __COLOR_FILES_BY_EXTENSION__
    xf->ftype = 1;
    #endif

    #ifdef __FILE_OWNERS_ENABLE__
    xf->pw = st.st_uid;
    #endif
    #ifdef __FILE_GROUPS_ENABLE__
    xf->gr = st.st_gid;
    #endif

    #ifdef __DEV_ENABLE__
    xf->dev = st.st_dev;
    #endif
    #ifdef __NLINK_ENABLE__
    xf->nlink = st.st_nlink;
    #endif
    #ifdef __RDEV_ENABLE__
    xf->rdev = st.st_rdev;
    #endif
    #ifdef __BLK_SIZE_ENABLE__
    xf->blksize = st.st_blksize;
    #endif
    #ifdef __BLOCKS_ENABLE__
    xf->blocks = st.st_blocks;
    #endif
}
#endif


static uchar mode_to_type(const mode_t mode)
{
    switch (mode & S_IFMT) {
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

static void *dir_load(void *arg)
{
    struct xdir *nd = ((struct loaddir_s*)arg)->d;
    #ifdef __FOLLOW_PARENT_DIR__
    char *searched_name = ((struct loaddir_s*)arg)->searched_name;
    #endif
    #if defined(__THREADS_FOR_DIR_ENABLE__) || defined(__FOLLOW_PARENT_DIR__)
    csas *cs = ((struct loaddir_s*)arg)->b;
    int ws = ((struct loaddir_s*)arg)->ws;
    int which = ((struct loaddir_s*)arg)->which;
    #endif
    free(arg);

    DIR *d;
    if ((d = opendir(nd->path)) == NULL) {
        nd->permission_denied = 1;
        #ifdef __FOLLOW_PARENT_DIR__
        free(searched_name);
        #endif
        #ifdef __THREADS_FOR_DIR_ENABLE__
        nd->enable = false;
        pthread_detach(nd->thread);
        pthread_exit(NULL);
        #endif
        return NULL;
    }

    struct dirent *dir;

    size_t buffer_size = nd->size;
    struct stat sfile, sfile2;

    int fd;
    
    #ifdef __FILE_SIZE_ENABLE__
    int tfd;
    ull size, count;
    #endif
    fd = dirfd(d);

    bool isold = nd->size > 0;

    #ifdef __RESCUE_SELECTED_IF_DIR_CHANGE_ENABLE__
    struct xfile *oldel = NULL;
    size_t oldsize = 0, begin, end;

    if (isold) {
        begin = 0;
        end = nd->size;
        oldel = nd->xf;
        oldsize = nd->size;
        nd->xf = NULL;
        nd->size = 0;
    }
    #else
    if (isold)
        free_xfile(&nd->xf,&nd->size);
    #endif

    buffer_size = 0;
    size_t name_lenght;

    while ((dir = readdir(d))) {
        #ifdef __HIDE_FILES__
        if (dir->d_name[0] == '.')
            continue;
        #endif
        if (dir->d_name[0] == '.' && (dir->d_name[1] == '\0' || (dir->d_name[1] == '.' && dir->d_name[2] == '\0')))
            continue;

        if (nd->size == buffer_size)
            nd->xf = (struct xfile*)realloc(nd->xf,(buffer_size+=DIR_INC_RATE)*sizeof(struct xfile));

        name_lenght = strlen(dir->d_name)+1;
        nd->xf[nd->size].nlen = name_lenght;
        nd->xf[nd->size].name = memcpy(malloc(name_lenght),dir->d_name,name_lenght);

        nd->xf[nd->size].type = 0;
        #ifdef __SAVE_PREVIEW__
        nd->xf[nd->size].cpreview = NULL;
        nd->xf[nd->size].previewl = 0;
        #endif

        fstatat(fd,dir->d_name,&sfile,AT_SYMLINK_NOFOLLOW);

        if ((sfile.st_mode&S_IFMT) == S_IFLNK) {
            if (fstatat(fd,dir->d_name,&sfile2,0) == -1)
                nd->xf[nd->size].type |= T_FILE_MISSING;
            else {
                nd->xf[nd->size].type = mode_to_type(sfile2.st_mode);
                nd->xf[nd->size].type |= T_SYMLINK;
            }
        }
        else
            nd->xf[nd->size].type = mode_to_type(sfile.st_mode);

        #ifdef __INODE_ENABLE__
        nd->xf[nd->size].inode = dir->d_ino;
        #endif

        #ifdef __MTIME_ENABLE__
        nd->xf[nd->size].mtim = sfile.st_mtim.tv_sec;
        #endif
        #ifdef __ATIME_ENABLE__
        nd->xf[nd->size].atim = sfile.st_atim.tv_sec;
        #endif
        #ifdef __CTIME_ENABLE__
        nd->xf[nd->size].ctim = sfile.st_ctim.tv_sec;
        #endif
        #ifdef __MODE_ENABLE__
        nd->xf[nd->size].mode = sfile.st_mode;
        #endif

        #ifdef __FILE_SIZE_ENABLE__
        if ((s_DirSizeMethod&D_F) != D_F && (nd->xf[nd->size].type&T_GT) == T_DIR) {
            if (faccessat(fd,dir->d_name,R_OK,0) == 0 && (tfd = openat(fd,dir->d_name,O_DIRECTORY)) != -1) {
                count = 0;
                size = 0;
                get_dirsize(tfd,&count,&size,s_DirSizeMethod);
                nd->xf[nd->size].size = (s_DirSizeMethod&D_C) ? count : size;
                close(tfd);
            }
            else
                nd->xf[nd->size].size = 0;
        }
        else
            nd->xf[nd->size].size = sfile.st_size*((nd->xf[nd->size].type&T_SYMLINK) == 0)
            +sfile2.st_size*((nd->xf[nd->size].type&T_SYMLINK) != 0);
        #endif

        #ifdef __COLOR_FILES_BY_EXTENSION__
        nd->xf[nd->size].ftype = 1;
        #endif

        memset(nd->xf[nd->size].list,0,WORKSPACE_N);
        #ifdef __RESCUE_SELECTED_IF_DIR_CHANGE_ENABLE__
        if (isold) {
            for (register size_t i = begin; i < end; i++) {
                #ifdef __RESCUE_SELECTED_IF_DIR_CHANGE_ENABLE__
                    #ifdef __FAST_RESCUE__
                    if (oldel[i].inode == dir->d_ino)
                    #else
                    if (strcmp(oldel[i].name,dir->d_name) == 0)
                    #endif
                #endif
                    #ifdef __CHECK_IF_FILE_HAS_THE_SAME_NAME__
                    if (strcmp(oldel[i].name,dir->d_name) == 0)
                    #endif
                    {
                        begin += 1*(i == begin);
                        end -= (i == end);
                        memcpy(nd->xf[nd->size].list,oldel[i].list,WORKSPACE_N);
                        break;
                    }
            }
        }
        #endif

        #ifdef __FILE_OWNERS_ENABLE__
        nd->xf[nd->size].pw = sfile.st_uid;
        #endif
        #ifdef __FILE_GROUPS_ENABLE__
        nd->xf[nd->size].gr = sfile.st_gid;
        #endif

        #ifdef __DEV_ENABLE__
        nd->xf[nd->size].dev = sfile.st_dev;
        #endif
        #ifdef __NLINK_ENABLE__
        nd->xf[nd->size].nlink = sfile.st_nlink;
        #endif
        #ifdef __RDEV_ENABLE__
        nd->xf[nd->size].rdev = sfile.st_rdev;
        #endif
        #ifdef __BLK_SIZE_ENABLE__
        nd->xf[nd->size].blksize = sfile.st_blksize;
        #endif
        #ifdef __BLOCKS_ENABLE__
        nd->xf[nd->size].blocks = sfile.st_blocks;
        #endif

        nd->size++;
    }

    #ifdef __RESCUE_SELECTED_IF_DIR_CHANGE_ENABLE__
    if (isold)
        free_xfile(&oldel,&oldsize);
    #endif

    closedir(d);
    close(fd);

    if (buffer_size != nd->size)
        nd->xf = (struct xfile*)realloc(nd->xf,(nd->size)*sizeof(struct xfile));

    if (nd->size > 0) {
        #ifdef __SORT_ELEMENTS_ENABLE__
        sort_xfile(nd->xf,nd->size,s_SortMethod);
        #endif

        #ifdef __FOLLOW_PARENT_DIR__
        if (searched_name)
            move_to(cs,ws,which,searched_name);
        #endif
    }

    #ifdef __FOLLOW_PARENT_DIR__
    free(searched_name);
    #endif

    #ifdef __THREADS_FOR_DIR_ENABLE__
    nd->enable = false;
    if (which == 1 && ws == cs->current_ws && s_Win3Enable)
        get_preview(cs);
    pthread_detach(nd->thread);
    pthread_exit(NULL);
    #endif
    return NULL;
}

int getdir(const char *path, csas *cs, const int ws, const int which, const char mode
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

    if (path == NULL || path[0] == '\0')
        return -1;

    char temp[PATH_MAX];

    if (realpath(path,temp) == NULL)
        return -1;

    struct stat sfile1;
    if (stat(path,&sfile1) != 0)
        return -1;

    bool exists = false;
    flexarr *base = cs->base;
    long int found = -1;
    
    for (size_t i = 0; i < base->size; i++) {
        if (strcmp(((struct xdir**)base->v)[i]->path,temp) == 0) {
            found = i;
            exists = true;
            break;
        }
    }

    if (found == -1) {
        found = base->size;
        *(struct xdir**)flexarr_inc(base) = xdir_init();
    }

    cs->ws[ws].win[which] = found;

    register struct xdir *fbase = ((struct xdir**)base->v)[found];

    if (!exists) {
        fbase->permission_denied = 0;
        fbase->inode = sfile1.st_ino;
        fbase->ctime = sfile1.st_ctim;
        fbase->path = strcpy(malloc(PATH_MAX),temp);
    } else {
        register struct timespec *sf_ctim = &sfile1.st_ctim, *fb_ctim = &fbase->ctime;
        if (sf_ctim->tv_sec != fb_ctim->tv_sec || sf_ctim->tv_nsec != fb_ctim->tv_nsec) {
            *fb_ctim = *sf_ctim;
            fbase->changed = true;
        }
    }

    if (fbase->permission_denied)
        return -1;

    if (mode == 0 && exists) return -1;

    if (mode == 1 && exists && !fbase->changed)
        return -1;

    #ifdef __SORT_ELEMENTS_ENABLE__
    fbase->sort_m = s_SortMethod;
    #endif

    struct loaddir_s *arg = malloc(sizeof(struct loaddir_s));

    arg->d = fbase;
    #ifdef __FOLLOW_PARENT_DIR__
    arg->b = cs;
    arg->ws = ws;
    arg->which = which;
    arg->searched_name = searched_name ? strdup(searched_name) : NULL;
    #endif

    #ifdef __THREADS_FOR_DIR_ENABLE__
    if (fbase->enable)
        pthread_join(fbase->thread,NULL);
    fbase->enable = true;

    pthread_create(&fbase->thread,NULL,dir_load,arg);
    if (!threaded)
        pthread_join(fbase->thread,NULL);
    #else
    dir_load(arg);
    #endif
    fbase->changed = false;

    return 0;
}

int csas_cd(const char *path, const int ws, csas *cs)
{
    char npath[PATH_MAX]
    #ifdef __FOLLOW_PARENT_DIR__
    , tpath[PATH_MAX]
    , *t = NULL;
    bool b = strcmp(path,"..") == 0
    #endif
    ;

    #ifdef __FOLLOW_PARENT_DIR__
    if (!s_Win1Enable && b) {
        getcwd(tpath,PATH_MAX);
        t = memrchr(tpath,'/',strlen(tpath));
        if (t) t++;
    }
    #endif

    if (realpath(path,npath) == NULL || chdir(path) == -1)
        return -1;

    strcpy(cs->ws[ws].path,npath);

    #ifdef __FILESYSTEM_INFO_ENABLE__
    statfs(".",&cs->fs);
    #endif

    werase(cs->win[1]);
    wrefresh(cs->win[1]);

    #ifdef __THREADS_FOR_DIR_ENABLE__
    int loaded = 
    #endif
    getdir(".",cs,ws,1,s_DirLoadingMode
    #ifdef __FOLLOW_PARENT_DIR__
    ,t
    #endif
    #ifdef __THREADS_FOR_DIR_ENABLE__
    ,s_ThreadsForDir
    #endif
    );

    register struct xdir *xdr = G_D(ws,1);

    if (s_Win1Enable) {
        werase(cs->win[0]);
        if (s_Borders)
            setborders(cs,0);
        wrefresh(cs->win[0]);

        if (xdr->path[0] == '/' && xdr->path[1] == '\0')
            s_Win1Display = false;
        else
        {
            #ifdef __FOLLOW_PARENT_DIR__
            t = memrchr(npath,'/',strlen(npath));
            if (t) t++;
            #endif

            getdir("..",cs,ws,0,s_DirLoadingMode
            #ifdef __FOLLOW_PARENT_DIR__
            ,t
            #endif
            #ifdef __THREADS_FOR_DIR_ENABLE__
            ,s_ThreadsForDir
            #endif
            );

            #ifdef __FOLLOW_PARENT_DIR__
            if (t
            #ifdef __THREADS_FOR_DIR_ENABLE__
            && !G_D(ws,0)->enable
            #endif
            )
                move_to(cs,ws,0,t);
            #endif

            s_Win1Display = true;
        }
    }

    if (
    #ifdef __THREADS_FOR_DIR_ENABLE__
    loaded == -1 &&
    #endif
        ws == cs->current_ws && s_Win3Enable) {
        if (xdr->size == 0) {
            werase(cs->win[2]);
            if (s_Borders)
                setborders(cs,2);
            wrefresh(cs->win[2]);
        }
        if (
            #ifdef __THREADS_FOR_DIR_ENABLE__
            !xdr->enable &&
            #endif
            xdr->size > 0)
            get_preview(cs);
    }
    
    update_size(cs);

    return 0;
}
