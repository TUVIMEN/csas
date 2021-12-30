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
#include "useful.h"
#include "csas.h"
#include "sort.h"
#include "load.h"

extern li SortMethod;
extern li DirSizeMethod;

int
xfile_update(xfile *f)
{
    struct stat statbuf;
    if (!f || lstat(f->name,&statbuf) != 0)
        return -1;
    f->mode = statbuf.st_mode;
    register off_t size = statbuf.st_size;
    f->mtime = statbuf.st_mtime;
    f->flags = 0;
    if ((f->mode&S_IFMT) == S_IFLNK) {
        if (stat(f->name,&statbuf) != 0)
            f->flags |= SLINK_MISSING;
        else if ((statbuf.st_mode&S_IFMT) == S_IFDIR)
            f->flags |= SLINK_TO_DIR;
    }
    if (!(DirSizeMethod&D_F) && (f->flags&SLINK_TO_DIR || (statbuf.st_mode&S_IFMT) == S_IFDIR))
        return 0;
    f->size = size;
    return 0;
}

int
load_dir(xdir *dir)
{
    dir->size = 0;
    ret_errno(dir==NULL,EINVAL,-1);
    char *path = dir->path;
    ret_errno(path==NULL,EINVAL,-1);
    ret_errno(dir->files!=NULL,EINVAL,-1);
    DIR *dp;
    if (!(dp = opendir(path)))
        return -1;
    int fd = dirfd(dp),t;
    struct dirent *ep;
    size_t size=0,asize=0,names_size=0,names_asize=0,nlen;
    xfile *files = NULL;
    char *names = NULL,*dname;
    struct stat statbuf;

    while ((ep = readdir(dp))) {
        dname = ep->d_name;
        if (dname[0] == '.' && (dname[1] == 0 || (dname[1] == '.' && dname[2] == 0)))
            continue;
        if (size == asize) {
            asize += FILE_INCR;
            void *pmem = realloc(files,asize*sizeof(xfile));
            if (pmem == NULL) {
                if (files)
                    free(files);
                return -1;
            }
            files = pmem;
        }
        xfile *f = &files[size];
        if (names_size+NAME_MAX >= names_asize) {
            names_asize += NAMES_INCR*NAME_MAX;
            void *pmem = realloc(names,names_asize);
            if (pmem == NULL) {
                if (names)
                    free(names);
                return -1;
            }
            names = pmem;
        }
        nlen = strlen(dname);
        f->nlen = nlen++;
        memcpy(names+names_size,dname,nlen);
        f->name = (void*)names_size;
        names_size += nlen;

        fstatat(fd,dname,&statbuf,AT_SYMLINK_NOFOLLOW);
        memset(f->sel,0,TABS);
        f->mode = statbuf.st_mode;
        f->size = statbuf.st_size;
        f->mtime = statbuf.st_mtime;
        f->flags = 0;
        if ((f->mode&S_IFMT) == S_IFLNK) {
            if (fstatat(fd,dname,&statbuf,0) != 0)
                f->flags |= SLINK_MISSING;
            else if ((statbuf.st_mode&S_IFMT) == S_IFDIR)
                f->flags |= SLINK_TO_DIR;
        }
        if (!(DirSizeMethod&D_F) && (f->flags&SLINK_TO_DIR || ep->d_type == DT_DIR)) {
            f->size = 0;
            if ((t = openat(fd,dname,O_DIRECTORY)) != -1) {
                get_dirsize(t,&f->size,&f->size,DirSizeMethod);
                close(t);
            }
        }
        size++;
    }
    closedir(dp);
    if (size != asize) {
        void *pmem = realloc(files,size*sizeof(xfile));
        if (pmem == NULL) {
            free(files);
            return -1;
        }
        files = pmem;
    }
    if (names_size != names_asize) {
        void *pmem = realloc(names,names_size);
        if (pmem == NULL) {
            free(files);
            return -1;
        }
        names = pmem;
    }
    if (size)
        for (size_t i = 0; i < size; i++)
            files[i].name += (size_t)names;
    dir->size = size;
    dir->files = files;
    dir->names = names;
    return 0;
}

li
getdir(const char *path, flexarr *dirs, const uchar flags)
{
    ret_errno(dirs==NULL,EINVAL,-1);

    size_t i;
    li ret=0;
    char rpath[PATH_MAX];
    size_t rpathl;
    if (realpath(path,rpath) == NULL)
        return -1;
    rpathl = strlen(rpath);

    struct stat statbuf;
    if (lstat(path,&statbuf) != 0)
        return -1;
    if (flags&D_FOLLOW && (statbuf.st_mode&S_IFMT) == S_IFLNK) {
        if (stat(rpath,&statbuf) != 0)
            return -1;
    }
    if ((statbuf.st_mode&S_IFMT) != S_IFDIR) {
        errno = ENOTDIR;
        return -1;
    }

    xdir *d = (xdir*)dirs->v;
    uchar found = 0;
    for (i = 0; i < dirs->size; i++) {
        if (rpathl == d[i].plen && memcmp(d[i].path,rpath,rpathl) == 0) {
            found = 1;
            break;
        }
    }

    if (found) {
        d = &d[i];
        if (flags&D_MODE_ONCE) {
            if (d->sort != SortMethod) {
                xfile_sort(d->files,d->size,SortMethod);
                d->sort = SortMethod;
            }
            if (flags&D_CHDIR)
                if (chdir(path) != 0)
                    return -1;
            ret = i;
            goto END;
        }
        ret = i;
    } else {
        d = flexarr_inc(dirs);
        ret = dirs->size-1;
        d->path = malloc(PATH_MAX);
        memcpy(d->path,rpath,rpathl+1);
        d->plen = rpathl;
        d->files = NULL;
        d->size = 0;
        d->asize = 0;
        memset(d->sel,0,TABS*sizeof(size_t));
        memset(d->scroll,0,TABS*sizeof(size_t));
        memset(&d->ctime,0,sizeof(struct timespec));
        d->searchlist = flexarr_init(sizeof(char*),SEARCHLIST_INCR);
        d->searchlist_pos = 0;
        d->sort = 0;
        d->flags = 0;
    }

    if (access(path,R_OK) != 0) {
        d->flags |= SEACCES;
        return ret;
    }

    if (flags&D_CHDIR)
        if (chdir(path) != 0)
            return -1;
    if (flags&D_MODE_CHANGE) {
        if (memcmp(&d->ctime,&statbuf.st_ctim,sizeof(struct timespec)) == 0) {
            if (d->sort != SortMethod) {
                xfile_sort(d->files,d->size,SortMethod);
                d->sort = SortMethod;
            }
            goto END;
        }
    }
    d->ctime = statbuf.st_ctim;
    
    if (d->files != NULL) {
        free(d->names);
        d->names = NULL;
        free(d->files);
        d->files = NULL;
        d->size = 0;
        d->asize = 0;
    }
    if (load_dir(d) != 0)
        return -1;
    if (d->size) {
        for (size_t i = 0; i < TABS; i++) {
            if (d->sel[i] >= d->size)
                d->sel[i] = d->size-1;
        }
    }
    d->flags &= ~S_CHANGED;
    d->asize = d->size;
    xfile_sort(d->files,d->size,SortMethod);
    d->sort = SortMethod;
    
    END:
    if (!(flags&D_RECURSIVE))
        return ret;

    xfile *files = d->files;
    rpath[rpathl++] = '/';
    for (size_t i = 0; i < ((xdir*)dirs->v)[ret].asize; i++) {
        if ((files[i].mode&S_IFMT) != S_IFDIR && (flags&D_FOLLOW && !(files[i].flags&SLINK_TO_DIR)))
            continue;
        memcpy(rpath+rpathl,files[i].name,files[i].nlen);
        rpath[rpathl+files[i].nlen] = 0;
        getdir(rpath,dirs,flags);
    }
    
    /*struct dirent *dir;
    DIR *dirp = opendir(rpath);
    if (dirp == NULL)
        return -1;
    rpath[rpathl++] = '/';
    while ((dir = readdir(dirp))) {
        if (dir->d_name[0] == '.' && (dir->d_name[1] == '\0' || (dir->d_name[1] == '.' && dir->d_name[2] == '\0')))
            continue;
        if (dir->d_type != DT_DIR)
            continue;
        memcpy(rpath+rpathl,dir->d_name,strlen(dir->d_name)+1);
        getdir(rpath,dirs,flags);
    }
    closedir(dirp);*/
    return ret;
}
