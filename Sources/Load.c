#include "main.h"
#include "Load.h"

#ifdef __SORT_ELEMENTS_ENABLE__
#include "Sort.h"
#endif

#include "Usefull.h"

extern Settings* settings;

void* LoadDir(void *arg)
{
    struct Dir* this = (struct Dir*)arg;

    DIR* d = opendir(this->path);
    struct dirent* dir;

    size_t buffer_size = this->El_t;
    struct stat sFile;

    int fd, typeOFF = 0;

    if (d)
    {
        int tfd;
        fd = dirfd(d);

        if (this->El_t > 0)
        {
            struct Element* newEl = NULL;
            size_t newEl_t = 0;
            buffer_size = 0;
            size_t begin = 0, end = this->El_t;

            for (int i = 0; (dir = readdir(d)); i++)
            {
                #ifdef __SHOW_HIDDEN_FILES_ENABLE__
                if(!ShowHiddenFiles)
                    if (dir->d_name[0] == '.')
                        continue;
                #endif
                if (dir->d_name[0] == '.' && (dir->d_name[1] == '\0' || (dir->d_name[1] == '.' && dir->d_name[2] == '\0')))
                    continue;

                if (newEl_t == buffer_size)
                    newEl = (struct Element*)realloc(newEl,(buffer_size+=DIR_INC_RATE)*sizeof(struct Element));

                #ifdef __HUMAN_READABLE_SIZE_ENABLE__
                newEl[newEl_t].SizErrToDisplay = NULL;
                #endif
                newEl[newEl_t].name = (char*)malloc(dir->d_reclen);

                strcpy(newEl[newEl_t].name, dir->d_name);
                if (fstatat(fd,dir->d_name,&sFile,0) != 0)
                {
                    fstatat(fd,dir->d_name,&sFile,AT_SYMLINK_NOFOLLOW);
                    newEl[newEl_t].Type = 7;
                }

                if (newEl[newEl_t].Type != 7)
                {
                    if (dir->d_type == 10)
                        typeOFF = T_BLINK;

                    switch (sFile.st_mode & S_IFMT)
                    {
                        case S_IFBLK:  newEl[newEl_t].Type = T_BDEV+typeOFF;     break;
                        case S_IFCHR:  newEl[newEl_t].Type = T_DEV+typeOFF;      break;
                        case S_IFDIR:  newEl[newEl_t].Type = T_DIR+typeOFF;      break;
                        case S_IFIFO:  newEl[newEl_t].Type = T_FIFO+typeOFF;     break;
                        case S_IFREG:  newEl[newEl_t].Type = T_REG+typeOFF;      break;
                        case S_IFSOCK: newEl[newEl_t].Type = T_SOCK+typeOFF;     break;
                        default:       newEl[newEl_t].Type = T_OTHER;            break;
                    }

                    typeOFF = 0;
                }

                newEl[newEl_t].inode = dir->d_ino;

                #ifdef __MTIME_ENABLE__
                newEl[newEl_t].mtime = sFile.st_mtime;
                #endif
                #ifdef __ATIME_ENABLE__
                newEl[newEl_t].atime = sFile.st_atime;
                #endif
                #ifdef __CTIME_ENABLE__
                newEl[newEl_t].ctime = sFile.st_ctime;
                #endif

                newEl[newEl_t].flags = sFile.st_mode;

                #ifdef __GET_DIR_SIZE_ENABLE__
                if ((settings->DirSizeMethod&D_F) != D_F && (sFile.st_mode & S_IFMT) == S_IFDIR)
                {
                    if (faccessat(fd,dir->d_name,R_OK,0) == 0 && (tfd = openat(fd,dir->d_name,O_DIRECTORY)) != -1)
                    {
                        newEl[newEl_t].size = GetDirSize(tfd,(settings->DirSizeMethod&D_R) == D_R,(settings->DirSizeMethod&D_C) == D_C);
                        close(tfd);
                    }
                    else
                        newEl[newEl_t].size = -1;
                }
                else
                #endif
                #ifdef __FILE_SIZE_ENABLE__
                    newEl[newEl_t].size = sFile.st_size*(newEl[newEl_t].Type != 7);
                #endif

                #ifdef __COLOR_FILES_BY_EXTENSION__
                newEl[newEl_t].FType = 1;
                #endif

                newEl[newEl_t].List = calloc(WORKSPACE_N,1);
                for (size_t i = begin; i < end; i++) //killer
                {
                    if (this->El[i].inode == dir->d_ino)
                    {
                        //if (strcmp(this->El[i].name,dir->d_name) == 0)
                        //{
                            begin += 1*(i == begin);
                            end -= (i == end);
                            memcpy(newEl[newEl_t].List,this->El[i].List,WORKSPACE_N);
                            break;
                        //}
                    }
                }

                #ifdef __FILE_OWNERS_ENABLE__
                newEl[newEl_t].pw = sFile.st_uid;
                #endif
                #ifdef __FILE_GROUPS_ENABLE__
                newEl[newEl_t].gr = sFile.st_gid;
                #endif

                ++(newEl_t);
            }
            for (size_t i = 0; i < this->El_t; i++)
            {
                free(this->El[i].List);
                free(this->El[i].name);
                free(this->El[i].SizErrToDisplay);
            }
            free(this->El);
            this->El = newEl;
            this->El_t = newEl_t;
        }
        else
        {
            while ((dir = readdir(d)))
            {
                #ifdef __SHOW_HIDDEN_FILES_ENABLE__
                if(!ShowHiddenFiles)
                    if (dir->d_name[0] == '.')
                        continue;
                #endif
                if (dir->d_name[0] == '.' && (dir->d_name[1] == '\0' || (dir->d_name[1] == '.' && dir->d_name[2] == '\0')))
                    continue;

                if (this->El_t == buffer_size)
                    this->El = (struct Element*)realloc(this->El,(buffer_size+=DIR_INC_RATE)*sizeof(struct Element));

                #ifdef __HUMAN_READABLE_SIZE_ENABLE__
                this->El[this->El_t].SizErrToDisplay = NULL;
                #endif
                this->El[this->El_t].name = (char*)malloc(dir->d_reclen);

                strcpy(this->El[this->El_t].name, dir->d_name);
                if (fstatat(fd,dir->d_name,&sFile,0) != 0)
                {
                    fstatat(fd,dir->d_name,&sFile,AT_SYMLINK_NOFOLLOW);
                    this->El[this->El_t].Type = 7;
                }

                if (this->El[this->El_t].Type != 7)
                {
                    if (dir->d_type == 10)
                        typeOFF = T_BLINK;

                    switch (sFile.st_mode & S_IFMT)
                    {
                        case S_IFBLK:  this->El[this->El_t].Type = T_BDEV+typeOFF;     break;
                        case S_IFCHR:  this->El[this->El_t].Type = T_DEV+typeOFF;      break;
                        case S_IFDIR:  this->El[this->El_t].Type = T_DIR+typeOFF;      break;
                        case S_IFIFO:  this->El[this->El_t].Type = T_FIFO+typeOFF;     break;
                        case S_IFREG:  this->El[this->El_t].Type = T_REG+typeOFF;      break;
                        case S_IFSOCK: this->El[this->El_t].Type = T_SOCK+typeOFF;     break;
                        default:       this->El[this->El_t].Type = T_OTHER;            break;
                    }

                    typeOFF = 0;
                }

                this->El[this->El_t].inode = dir->d_ino;
                //printf("%s %ld %ld %ld\n",dir->d_name,dir->d_ino,dir->d_off,dir->d_reclen);

                #ifdef __MTIME_ENABLE__
                this->El[this->El_t].mtime = sFile.st_mtime;
                #endif
                #ifdef __ATIME_ENABLE__
                this->El[this->El_t].atime = sFile.st_atime;
                #endif
                #ifdef __CTIME_ENABLE__
                this->El[this->El_t].ctime = sFile.st_ctime;
                #endif

                this->El[this->El_t].flags = sFile.st_mode;

                #ifdef __GET_DIR_SIZE_ENABLE__
                if ((settings->DirSizeMethod&D_F) != D_F && (sFile.st_mode & S_IFMT) == S_IFDIR)
                {
                    if (faccessat(fd,dir->d_name,R_OK,0) == 0 && (tfd = openat(fd,dir->d_name,O_DIRECTORY)) != -1)
                    {
                        this->El[this->El_t].size = GetDirSize(tfd,(settings->DirSizeMethod&D_R) == D_R,(settings->DirSizeMethod&D_C) == D_C);
                        close(tfd);
                    }
                    else
                        this->El[this->El_t].size = -1;
                }
                else
                #endif
                #ifdef __FILE_SIZE_ENABLE__
                    this->El[this->El_t].size = sFile.st_size*(this->El[this->El_t].Type != 7);
                #endif

                #ifdef __COLOR_FILES_BY_EXTENSION__
                this->El[this->El_t].FType = 1;
                #endif

                this->El[this->El_t].List = calloc(WORKSPACE_N,1);

                #ifdef __FILE_OWNERS_ENABLE__
                this->El[this->El_t].pw = sFile.st_uid;
                #endif
                #ifdef __FILE_GROUPS_ENABLE__
                this->El[this->El_t].gr = sFile.st_gid;
                #endif

                ++(this->El_t);
            }
        }
        closedir(d);
    }
    else
    {
        this->El_t = -1;
        this->enable = false;
        pthread_detach(this->thread);
        pthread_exit(NULL);
    }

    if (buffer_size != this->El_t)
        this->El = (struct Element*)realloc(this->El,(this->El_t)*sizeof(struct Element));

    if (this->El_t > 0)
    {
        #ifdef __SORT_ELEMENTS_ENABLE__
        SortEl(this->El,this->El_t,this->sort_m);
        #endif
        #ifdef __COLOR_FILES_BY_EXTENSION__
        CheckFileTypeN(this->El,0,this->El_t);
        #endif
    }

    this->enable = false;
    pthread_detach(this->thread);
    pthread_exit(NULL);
}

// The despair
void GetDir(char* path, Basic* this, int Which, bool threaded)
{
    char* temp = (char*)malloc(PATH_MAX);

    realpath(path,temp);

    int found = -1;
    for (size_t i = 0; i < this->ActualSize; i++)
    {
        if (strcmp(this->Base[i].path,temp) == 0)
        {
            found = i;
            break;
        }
    }

    if (found == -1)
    {
        if (this->ActualSize == this->AllocatedSize)
        {
            this->Base = (struct Dir*)realloc(this->Base,(this->AllocatedSize+=DIR_BASE_STABLE_RATE)*sizeof(struct Dir));
            for (size_t i = this->AllocatedSize-DIR_BASE_STABLE_RATE; i < this->AllocatedSize; i++)
            {
                this->Base[i].El = NULL;
                this->Base[i].El_t = 0;
                this->Base[i].enable = false;
                this->Base[i].path = NULL;
                this->Base[i].Ltop = (size_t*)malloc(WORKSPACE_N*sizeof(size_t));
                this->Base[i].selected = (size_t*)malloc(WORKSPACE_N*sizeof(size_t));
                for (int j = 0; j < WORKSPACE_N; j++)
                {
                    this->Base[i].Ltop[j] = 0;
                    this->Base[i].selected[j] = 0;
                }
                this->Base[i].fd = -1;
                this->Base[i].wd = -1;
            }
        }
        found = this->ActualSize;
        ++this->ActualSize;
    }

    this->Work[this->inW].win[Which] = &this->Base[found];
    if (this->Base[found].path == NULL)
        this->Base[found].path = (char*)malloc(PATH_MAX);
    strcpy(this->Base[found].path,temp);
    free(temp);

    bool IsChangeInDir = false;

    if (this->Base[found].fd == -1)
    {
        this->Base[found].fd = inotify_init1(IN_NONBLOCK);
        this->Base[found].wd = inotify_add_watch(this->Base[found].fd,this->Base[found].path,settings->INOTIFY_MASK);
        IsChangeInDir = true;
    }
    else
    {
        char buf[sizeof(struct inotify_event)+NAME_MAX+1] __attribute__ ((aligned(8)));
        if (read(this->Base[found].fd,buf,sizeof(struct inotify_event)+NAME_MAX+1) > 0)
            IsChangeInDir = true;
    }

    if (IsChangeInDir)
    {
        memset(this->Base[found].selected,0,WORKSPACE_N);
        memset(this->Base[found].Ltop,0,WORKSPACE_N);
        this->Base[found].sort_m = settings->SortMethod;
        if (this->Base[found].enable)
            pthread_join(this->Base[found].thread,NULL);
        this->Base[found].enable = true;

        pthread_create(&this->Base[found].thread,NULL,LoadDir,&this->Base[found]);
        if (!threaded)
            pthread_join(this->Base[found].thread,NULL);
    
        //endwin();
        //printf("%s %ld %ld %ld\n",this->Base[found].path,this->Base[found].El_t, this->ActualSize, this->AllocatedSize);
        //initscr();
    }
}
