#include "main.h"
#include "useful.h"
#include "csas.h"
#include "sort.h"
#include "load.h"

int
load_dir(xdir *dir, const mode_t flags)
{
    ret_errno(dir==NULL,EINVAL,-1);
    char *path = dir->path;
    ret_errno(path==NULL,EINVAL,-1);
    ret_errno(dir->files!=NULL,EINVAL,-1);
    DIR *dp;
    if (!(dp = opendir(path)))
        return -1;
    int fd = dirfd(dp);
    struct dirent *ep;
    size_t s = 0,nlen;
    while (readdir(dp)) s++;
    s -= 2;
    if (s == 0)
        return 0;
    dir->size = s;
    dir->files = malloc(s*sizeof(xfile));
    if (dir->files == NULL)
        exiterr();
    rewinddir(dp);
    xfile *files = dir->files;
    struct stat statbuf;
    s = 0;
    while ((ep = readdir(dp))) {
        const char *dname = ep->d_name;
        if (dname[0] == '.' && (dname[1] == 0 || (dname[1] == '.' && dname[2] == 0)))
            continue;
        xfile *f = &files[s];
        nlen = strlen(dname);
        f->nlen = nlen++;
        f->name = memcpy(malloc(nlen),dname,nlen);
        fstatat(fd,dname,&statbuf,AT_SYMLINK_NOFOLLOW);
        memset(f->sel,0,TABS);
        f->mode = statbuf.st_mode;
        f->size = statbuf.st_size;
        f->flags = 0;
        if ((f->mode&S_IFMT) == S_IFLNK) {
            if (fstatat(fd,dname,&statbuf,0) != 0)
                f->flags |= SLINK_MISSING;
            else if ((statbuf.st_mode&S_IFMT) == S_IFDIR)
                f->flags |= SLINK_TO_DIR;
        }
        s++;
    }
    return 0;
}

li
getdir(const char *path, flexarr *dirs, const uchar flags)
{
    ret_errno(dirs==NULL,EINVAL,-1);
    if (access(path,R_OK) != 0)
        return -1;

    size_t i;
    li ret=0;
    char rpath[PATH_MAX];
    size_t rpathl;
    if (realpath(path,rpath) == NULL)
        return -1;
    rpathl = strlen(rpath);

    xdir *d = (xdir*)dirs->v;
    uchar found = 0;
    for (i = 0; i < dirs->size; i++) {
        if (rpathl == strlen(d[i].path) && memcmp(d[i].path,rpath,rpathl) == 0) {
            found = 1;
            break;
        }
    }

    if (found) {
        if (flags&D_MODE_ONCE) {
            if (flags&D_CHDIR)
                if (chdir(path) != 0)
                    return -1;
            ret = i;
            goto END;
        }
        ret = i;
        d = &d[i];
    } else {
        d = flexarr_inc(dirs);
        ret = dirs->size-1;
        d->path = malloc(PATH_MAX);
        memcpy(d->path,rpath,rpathl+1);
        d->plen = rpathl;
        d->files = NULL;
        memset(d->sel,0,TABS*sizeof(size_t));
        memset(d->scroll,0,TABS*sizeof(size_t));
        memset(&d->ctime,0,sizeof(struct timespec));
    }

    struct stat statbuf;
    if (lstat(path,&statbuf) != 0)
        return -1;
    if (flags&D_CHDIR)
        if (chdir(path) != 0)
            return -1;
    if (flags&D_MODE_CHANGE)
        if (memcmp(&d->ctime,&statbuf.st_ctim,sizeof(struct timespec)) == 0)
            goto END;
    d->ctime = statbuf.st_ctim;
    
    if (d->files != NULL) {
        for (i = 0; i < d->size; i++)
            free(d->files[i].name);
        free(d->files);
        d->files = NULL;
        d->size = 0;
    }
    if (load_dir(d,flags) != 0)
        return -1;
    xfile_sort(d->files,d->size,SORT_CNAME|SORT_DIR_DISTINCTION|SORT_LDIR_DISTINCTION);
    
    END:
    if (!(flags&D_RECURSIVE))
        return ret;
    
    struct dirent *dir;
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
    closedir(dirp);
    return 0;
}

















