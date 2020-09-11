#include "main.h"
#include "Load.h"

#ifdef __SORT_ELEMENTS_ENABLE__
#include "Sort.h"
#endif

#ifdef __SHOW_HIDDEN_FILES_ENABLE__
extern bool ShowHiddenFiles;
#endif
#ifdef __SORT_ELEMENTS_ENABLE__
#include "Usefull.h"
extern unsigned char SortMethod;
#endif

#ifdef __GET_DIR_SIZE_ENABLE__
extern bool DirSizeMethod[];
#endif


void* LoadDir(void *arg)
{
    struct Dir* this = (struct Dir*)arg;

    if (this->El_t > 0)
    {
        for (int i = 0; i < this->El_t; i++)
        {
            #ifdef __HUMAN_READABLE_SIZE_ENABLE__
            //if (this->El[i].SizErrToDisplay != NULL)
                free(this->El[i].SizErrToDisplay);
            #endif

            free(this->El[i].name);
        }
        free(this->El);
        this->El = NULL;
        this->El_t = 0;
    }

    DIR* d = opendir(this->path);
    struct dirent* dir;

    size_t buffer_size = this->El_t;
    struct stat sFile;

    int fd, typeOFF = 0;

    if (d)
    {
        int tfd;
        fd = dirfd(d);

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
            this->El[this->El_t].name = (char*)malloc(NAME_MAX);

            strcpy(this->El[this->El_t].name, dir->d_name);
            fstatat(fd,dir->d_name,&sFile,0);

            if (dir->d_type == 10)
            {
                if (faccessat(fd,dir->d_name,F_OK,0) == -1)
                    this->El[this->El_t].Type = 7;
                typeOFF = T_BLINK;
            }

            if (this->El[this->El_t].Type != 7)
            {
                switch (sFile.st_mode & __S_IFMT)
                {
                    case __S_IFBLK:  this->El[this->El_t].Type = T_BDEV+typeOFF;     break;
                    case __S_IFCHR:  this->El[this->El_t].Type = T_DEV+typeOFF;      break;
                    case __S_IFDIR:  this->El[this->El_t].Type = T_DIR+typeOFF;      break;
                    case __S_IFIFO:  this->El[this->El_t].Type = T_FIFO+typeOFF;     break;
                    case __S_IFREG:  this->El[this->El_t].Type = T_REG+typeOFF;      break;
                    case __S_IFSOCK: this->El[this->El_t].Type = T_SOCK+typeOFF;     break;
                    default:         this->El[this->El_t].Type = T_OTHER;            break;
                }

                typeOFF = 0;
            }

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
            if (!DirSizeMethod[0] && (sFile.st_mode & __S_IFMT) == __S_IFDIR)
            {
                if ((tfd = openat(fd,dir->d_name,__O_DIRECTORY)) != -1)
                {
                    this->El[this->El_t].size = GetDirSize(tfd,DirSizeMethod[1],DirSizeMethod[2]);
                    close(tfd);
                }
            }
            else
            #endif
            #ifdef __FILE_SIZE_ENABLE__
                this->El[this->El_t].size = sFile.st_size;
            #endif

            #ifdef __COLOR_FILES_BY_EXTENSION__
            this->El[this->El_t].FType = 1;
            #endif

            #ifdef __FILE_OWNERS_ENABLE__
            this->El[this->El_t].pw = sFile.st_uid;
            #endif
            #ifdef __FILE_GROUPS_ENABLE__
            this->El[this->El_t].gr = sFile.st_gid;
            #endif

            ++(this->El_t);
        }
        closedir(d);
    }
    else
    {
        this->El_t = -1;
        this->enable = false;
        #ifdef __THREADS_ENABLE__
        pthread_detach(this->thread);
        #endif
        pthread_exit(NULL);
    }

    if (buffer_size != this->El_t)
        this->El = (struct Element*)realloc(this->El,(this->El_t)*sizeof(struct Element));

    #ifdef __SORT_ELEMENTS_ENABLE__
    if (this->El_t > 0)
        SortEl(this->El,this->El_t,SortMethod);
    #endif

    this->enable = false;
    #ifdef __THREADS_ENABLE__
    pthread_detach(this->thread);
    #endif
    pthread_exit(NULL);
}

extern int32_t INOTIFY_MASK;

// The despair
void GetDir(char* path, Basic* this, int Which, bool threaded)
{
    char* temp = (char*)malloc(PATH_MAX);

    realpath(path,temp);

    int found = -1;
    for (int i = 0; i < this->ActualSize; i++)
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
            for (int i = this->AllocatedSize-DIR_BASE_STABLE_RATE; i < this->AllocatedSize; i++)
            {
                this->Base[i].El = NULL;
                this->Base[i].El_t = 0;
                this->Base[i].enable = false;
                this->Base[i].path = NULL;
                this->Base[i].Ltop = 0;
                this->Base[i].selected = 0;
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
        this->Base[found].wd = inotify_add_watch(this->Base[found].fd,this->Base[found].path,INOTIFY_MASK);
        IsChangeInDir = true;
    }
    else
    {
        char buf[48];
        read(this->Base[found].fd,buf,47);
        if (buf[0] == 1)
        {
            lseek(this->Base[found].fd,0,SEEK_END);
            IsChangeInDir = true;
        }
    }

    if (IsChangeInDir)
    {
        if (this->Base[found].enable)
            pthread_join(this->Base[found].thread,NULL);
        this->Base[found].enable = true;

        pthread_create(&this->Base[found].thread,NULL,LoadDir,&this->Base[found]);
        #ifdef __THREADS_ENABLE__
            if (!threaded)
                pthread_join(this->Base[found].thread,NULL);
        #else
            pthread_join(this->Base[found].thread,NULL);
        #endif

        //endwin();
        //printf("%s %ld %ld %ld\n",this->Base[found].path,this->Base[found].El_t, this->ActualSize, this->AllocatedSize);
        //initscr();
    }
}
