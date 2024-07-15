/*
    csas - console file manager
    Copyright (C) 2020-2024 Dominik Stanisław Suchora <suchora.dominik7@gmail.com>

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
extern flexarr *trap_newdir;
extern flexarr *trap_chdir;

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
    ret_errno(dir==NULL,EINVAL,-1);

    flexarr *files = dir->files;
    files->size = 0;

    char *path = dir->path;
    ret_errno(path==NULL,EINVAL,-1);
    DIR *dp;
    if (!(dp = opendir(path)))
        return -1;

    flexarr *names = dir->names;
    size_t names_index=0,
           names_declared_size=0;

    if (!names) {
        names = flexarr_init(sizeof(flexarr*),NAMES_INCR);
    } else {
        names_declared_size = names->size;
        names->size = 0;
    }

    flexarr *ninside = NULL;
    char *ninsidev = NULL;
    size_t nfiles_start = 0;

    int fd = dirfd(dp),t;
    struct dirent *ep;
    size_t nlen;
    char *dname;
    struct stat statbuf;

    while ((ep = readdir(dp))) {
        dname = ep->d_name;
        if (dname[0] == '.' && (dname[1] == 0 || (dname[1] == '.' && dname[2] == 0)))
            continue;

        xfile *f = (xfile*)flexarr_inc(files);
        nlen = strlen(dname);
        f->nlen = nlen++;

        if (!names->size || ninside->size+nlen > ninside->asize) {
            if (!ninside || ninside->asize >= NAMES_INSIDE_MAX) {
                if (ninside) {
                    if (ninside->size != ninside->asize) {
                        ninside = ((flexarr**)names->v)[names_index];
                        flexarr_clearb(ninside);
                        ninsidev = ninside->v;
                    }

                    xfile *filesv = (xfile*)files->v;
                    for (size_t i = nfiles_start; i < files->size-1; i++)
                        filesv[i].name += (size_t)ninsidev;
                    nfiles_start = files->size-1;
                }

                flexarr **x = (flexarr**)flexarr_inc(names);
                size_t diff = (void*)x-(void*)names->v;
                names_index = diff ? diff/names->elsize : 0;

                if (names_index < names_declared_size) {
                    (*x)->size = 0;
                } else
                    *x = flexarr_init(sizeof(char),NAMES_INSIDE_INCR);
            }

            ninside = ((flexarr**)names->v)[names_index];
            size_t size_temp = ninside->size;
            ninside->size = ninside->asize;
            flexarr_inc(ninside);
            ninside->size = size_temp;
            ninsidev = (char*)ninside->v;
        }

        memcpy(ninsidev+ninside->size,dname,nlen);
        f->name = (void*)ninside->size;
        ninside->size += nlen;

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
    }
    closedir(dp);

    flexarr_clearb(files);

    if (names->asize) {
        names->size = names_index+1;

        if (names_index < names_declared_size) {
            flexarr **namesv = (flexarr**)names->v;
            for (size_t i = names_index+1; i < names_declared_size; i++)
                flexarr_free(namesv[i]);
        }

        flexarr_clearb(names);
        ninside = ((flexarr**)names->v)[names_index];

        if (ninside->size != ninside->asize)
            flexarr_clearb(ninside);
        ninsidev = ninside->v;
    }

    xfile *filesv = (xfile*)files->v;
    for (size_t i = nfiles_start; i < files->size; i++)
        filesv[i].name += (size_t)ninsidev;

    dir->files = files;
    dir->names = names;

    return 0;
}

li
getdir(const char *path, flexarr *dirs, const uchar flags)
{
    ret_errno(dirs==NULL,EINVAL,-1);

    uchar changed = 0;
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
    size_t dsize = 0;
    uchar found = 0;
    for (i = 0; i < dirs->size; i++) {
        if (rpathl == d[i].plen && memcmp(d[i].path,rpath,rpathl) == 0) {
            found = 1;
            break;
        }
    }

    if (found) {
        d = &d[i];
        dsize = d->files->size;
        if (flags&D_MODE_ONCE) {
            if (d->sort != SortMethod) {
                xfile_sort((xfile*)d->files->v,dsize,SortMethod);
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

        memset(d,0,sizeof(xdir));
        d->path = xmalloc(PATH_MAX);
        memcpy(d->path,rpath,rpathl+1);
        d->plen = rpathl;
        d->files = flexarr_init(sizeof(xfile),FILE_INCR);
    }

    if (access(path,R_OK) != 0) {
        d->flags |= SEACCES;
        return ret;
    }

    if (flags&D_CHDIR)
        if (chdir(path) != 0)
            return -1;
    if (flags&D_MODE_CHANGE &&
        memcmp(&d->ctime,&statbuf.st_ctim,sizeof(struct timespec)) == 0)
        goto END;

    d->ctime = statbuf.st_ctim;

    int ld_ret = load_dir(d);
    changed = 1;

    dsize = d->files->size;
    for (size_t i = 0; i < TABS; i++) {
        if (d->sel[i] >= dsize)
            d->sel[i] = dsize-1;
    }

    if (ld_ret != 0)
        return -1;

    d->flags &= ~S_CHANGED;

    END: ;
    xfile *files = (xfile*)d->files->v;
    size_t filesl = d->files->size;

    if (changed || d->sort != SortMethod) {
        xfile_sort(files,filesl,SortMethod);
        d->sort = SortMethod;
    }

    if (!(flags&D_RECURSIVE))
        return ret;

    rpath[rpathl++] = '/';
    for (size_t i = 0; i < filesl; i++) {
        if ((files[i].mode&S_IFMT) != S_IFDIR && (flags&D_FOLLOW && !(files[i].flags&SLINK_TO_DIR)))
            continue;
        memcpy(rpath+rpathl,files[i].name,files[i].nlen);
        rpath[rpathl+files[i].nlen] = 0;
        getdir(rpath,dirs,flags);
    }

    return ret;
}
