#include "main.h"
#include "Load.h"

#ifdef __SORT_ELEMENTS_ENABLE__
#include "Sort.h"
#endif

#include "Usefull.h"
#include "Draw.h"
#include "FastRun.h"

extern Settings* settings;

void freeEl(struct Element** El, long long int* El_t)
{
    for (long long int i = 0; i < *El_t; i++)
    {
        free((*El)[i].List);
        free((*El)[i].name);
        #ifdef __HUMAN_READABLE_SIZE_ENABLE__
        free((*El)[i].SizErrToDisplay);
        #endif
    }
    free(*El);
    *El = NULL;
    *El_t = 0;
}

void* LoadDir(void *arg)
{
    struct Dir* grf = (struct Dir*)arg;

    DIR* d = opendir(grf->path);
    struct dirent* dir;

    long long int buffer_size = grf->El_t;
    struct stat sFile;

    int fd, typeOFF = 0;

    if (d)
    {
        #ifdef __GET_DIR_SIZE_ENABLE__
        int tfd;
        #endif
        fd = dirfd(d);

        bool isOld = grf->El_t > 0;

        #ifdef __RESCUE_SELECTED_IF_DIR_CHANGE_ENABLE__
        struct Element* oldEl = NULL;
        long long int oldEl_t = 0;
        size_t begin, end;

        if (isOld)
        {
            begin = 0;
            end = grf->El_t;
            oldEl = grf->El;
            oldEl_t = grf->El_t;
            grf->El = NULL;
            grf->El_t = 0;
        }
        #else
        if (isOld)
            freeEl(&grf->El,&grf->El_t);
        #endif

        buffer_size = 0;

        while ((dir = readdir(d)))
        {
            #ifdef __SHOW_HIDDEN_FILES_ENABLE__
            if(!ShowHiddenFiles)
                if (dir->d_name[0] == '.')
                    continue;
            #endif
            if (dir->d_name[0] == '.' && (dir->d_name[1] == '\0' || (dir->d_name[1] == '.' && dir->d_name[2] == '\0')))
                continue;

            if (grf->El_t == buffer_size)
                grf->El = (struct Element*)realloc(grf->El,(buffer_size+=DIR_INC_RATE)*sizeof(struct Element));

            #ifdef __HUMAN_READABLE_SIZE_ENABLE__
            grf->El[grf->El_t].SizErrToDisplay = NULL;
            #endif
            grf->El[grf->El_t].name = (char*)malloc(dir->d_reclen);

            strcpy(grf->El[grf->El_t].name, dir->d_name);
            if (fstatat(fd,dir->d_name,&sFile,0) != 0)
            {
                fstatat(fd,dir->d_name,&sFile,AT_SYMLINK_NOFOLLOW);
                grf->El[grf->El_t].Type = 7;
            }

            if (grf->El[grf->El_t].Type != 7)
            {
                if (dir->d_type == 10)
                    typeOFF = T_BLINK;

                switch (sFile.st_mode & S_IFMT)
                {
                    case S_IFBLK:  grf->El[grf->El_t].Type = T_BDEV+typeOFF;     break;
                    case S_IFCHR:  grf->El[grf->El_t].Type = T_DEV+typeOFF;      break;
                    case S_IFDIR:  grf->El[grf->El_t].Type = T_DIR+typeOFF;      break;
                    case S_IFIFO:  grf->El[grf->El_t].Type = T_FIFO+typeOFF;     break;
                    case S_IFREG:  grf->El[grf->El_t].Type = T_REG+typeOFF;      break;
                    case S_IFSOCK: grf->El[grf->El_t].Type = T_SOCK+typeOFF;     break;
                    default:       grf->El[grf->El_t].Type = T_OTHER;            break;
                }

                typeOFF = 0;
            }

            grf->El[grf->El_t].inode = dir->d_ino;

            #ifdef __MTIME_ENABLE__
            grf->El[grf->El_t].mtim = sFile.st_mtim;
            #endif
            #ifdef __ATIME_ENABLE__
            grf->El[grf->El_t].atim = sFile.st_atim;
            #endif
            #ifdef __CTIME_ENABLE__
            grf->El[grf->El_t].ctim = sFile.st_ctim;
            #endif
            #ifdef __MODE_ENABLE__
            grf->El[grf->El_t].flags = sFile.st_mode;
            #endif

            #ifdef __GET_DIR_SIZE_ENABLE__
            if ((settings->DirSizeMethod&D_F) != D_F && (sFile.st_mode & S_IFMT) == S_IFDIR)
            {
                if (faccessat(fd,dir->d_name,R_OK,0) == 0 && (tfd = openat(fd,dir->d_name,O_DIRECTORY)) != -1)
                {
                    grf->El[grf->El_t].size = GetDirSize(tfd,(settings->DirSizeMethod&D_R) == D_R,(settings->DirSizeMethod&D_C) == D_C);
                    close(tfd);
                }
                else
                    grf->El[grf->El_t].size = -1;
            }
            else
            #endif
            #ifdef __FILE_SIZE_ENABLE__
                grf->El[grf->El_t].size = sFile.st_size*(grf->El[grf->El_t].Type != 7);
            #endif

            #ifdef __COLOR_FILES_BY_EXTENSION__
            grf->El[grf->El_t].FType = 1;
            #endif

            grf->El[grf->El_t].List = calloc(WORKSPACE_N,1);
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
                            memcpy(grf->El[grf->El_t].List,oldEl[i].List,WORKSPACE_N);
                            break;
                        }
                }
            }
            #endif

            #ifdef __FILE_OWNERS_ENABLE__
            grf->El[grf->El_t].pw = sFile.st_uid;
            #endif
            #ifdef __FILE_GROUPS_ENABLE__
            grf->El[grf->El_t].gr = sFile.st_gid;
            #endif

            #ifdef __DEV_ENABLE__
            grf->El[grf->El_t].dev = sFile.st_dev;
            #endif
            #ifdef __NLINK_ENABLE__
            grf->El[grf->El_t].nlink = sFile.st_nlink;
            #endif
            #ifdef __RDEV_ENABLE__
            grf->El[grf->El_t].rdev = sFile.st_rdev;
            #endif
            #ifdef __BLK_SIZE_ENABLE__
            grf->El[grf->El_t].blksize = sFile.st_blksize;
            #endif
            #ifdef __BLOCKS_ENABLE__
            grf->El[grf->El_t].blocks = sFile.st_blocks;
            #endif

            grf->El_t++;
        }

        #ifdef __RESCUE_SELECTED_IF_DIR_CHANGE_ENABLE__
        if (isOld)
            freeEl(&oldEl,&oldEl_t);
        #endif

        closedir(d);
        close(fd);
    }
    else
    {
        grf->El_t = -1;
        #ifdef __THREADS_FOR_DIR_ENABLE__
        grf->enable = false;
        pthread_detach(grf->thread);
        pthread_exit(NULL);
        #else
        return NULL;
        #endif
    }

    if (buffer_size != grf->El_t)
        grf->El = (struct Element*)realloc(grf->El,(grf->El_t)*sizeof(struct Element));

    if (grf->El_t > 0)
        #ifdef __SORT_ELEMENTS_ENABLE__
        SortEl(grf->El,grf->El_t,grf->sort_m);
        #endif

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
    // 1 - if doesn't exist or was changed load
    // 2 - always load

    char* temp = (char*)malloc(PATH_MAX);

    if (realpath(path,temp) == NULL)
    {
        free(temp);
        return;
    }

    bool exists = false;

    int found = -1;
    for (size_t i = 0; i < grf->ActualSize; i++)
    {
        if (strcmp(grf->Base[i].path,temp) == 0)
        {
            found = i;
            exists = true;
            break;
        }
    }

    if (found == -1)
    {
        if (grf->ActualSize == grf->AllocatedSize)
        {
            grf->Base = (struct Dir*)realloc(grf->Base,(grf->AllocatedSize+=DIR_BASE_STABLE_RATE)*sizeof(struct Dir));
            for (size_t i = grf->AllocatedSize-DIR_BASE_STABLE_RATE; i < grf->AllocatedSize; i++)
            {
                grf->Base[i].El = NULL;
                grf->Base[i].El_t = 0;
                #ifdef __THREADS_FOR_DIR_ENABLE__
                grf->Base[i].enable = false;
                #endif
                grf->Base[i].path = NULL;
                grf->Base[i].Ltop = (size_t*)calloc(WORKSPACE_N,sizeof(size_t));
                grf->Base[i].selected = (size_t*)calloc(WORKSPACE_N,sizeof(size_t));
                #ifdef __INOTIFY_ENABLE__
                grf->Base[i].fd = -1;
                grf->Base[i].wd = -1;
                grf->Base[i].Changed = false;
                #endif
            }
        }
        found = grf->ActualSize++;
    }

    grf->Work[workspace].win[Which] = found;
    if (!exists)
    {
        grf->Base[found].path = (char*)malloc(PATH_MAX);
        strcpy(grf->Base[found].path,temp);
    }
    free(temp);

    #ifdef __INOTIFY_ENABLE__
    if (!exists)
    {
        grf->Base[found].fd = inotify_init1(IN_NONBLOCK);
        grf->Base[found].wd = inotify_add_watch(grf->Base[found].fd,grf->Base[found].path,settings->INOTIFY_MASK);
        grf->Base[found].Changed = true;
    }
    else
    {
        char buf[sizeof(struct inotify_event)+NAME_MAX+1] __attribute__ ((aligned(8)));
        if (read(grf->Base[found].fd,buf,sizeof(struct inotify_event)+NAME_MAX+1) > 0)
            grf->Base[found].Changed = true;
    }
    #endif

    if (exists && mode == 0)
        return;

    #ifdef __INOTIFY_ENABLE__
    if (!grf->Base[found].Changed && mode == 1)
        return;
    #endif

    #ifdef __SORT_ELEMENTS_ENABLE__
    grf->Base[found].sort_m = settings->SortMethod;
    #endif
    #ifdef __THREADS_FOR_DIR_ENABLE__
    if (grf->Base[found].enable)
        pthread_join(grf->Base[found].thread,NULL);
    grf->Base[found].enable = true;

    pthread_create(&grf->Base[found].thread,NULL,LoadDir,&grf->Base[found]);
    if (!threaded)
        pthread_join(grf->Base[found].thread,NULL);
    #else
    LoadDir(&grf->Base[found]);
    #endif

    grf->Base[found].Changed = false;
}

void CD(const char* path, const int workspace, Basic* grf)
{
    if (chdir(path) != 0)
        return;

    #ifdef __FILESYSTEM_INFORMATION_ENABLE__
    statfs(".",&grf->fs);
    #endif

    werase(grf->win[1]);
    wrefresh(grf->win[1]);
    GetDir(".",grf,workspace,1,settings->DirLoadingMode
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

        if (grf->Base[grf->Work[workspace].win[1]].path[0] == '/' && grf->Base[grf->Work[workspace].win[1]].path[1] == '\0')
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

    if (grf->inW == workspace && settings->Win3Enable)
    {
        if (grf->Base[grf->Work[workspace].win[1]].El_t == 0)
        {
            werase(grf->win[2]);
            if (settings->Borders)
                SetBorders(grf,2);
            wrefresh(grf->win[2]);
        }
        if (
            #ifdef __THREADS_FOR_DIR_ENABLE__
            !grf->Base[grf->Work[workspace].win[1]].enable &&
            #endif
            grf->Base[grf->Work[workspace].win[1]].El_t > 0)
            FastRun(grf);
    }
}
