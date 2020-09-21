#include "main.h"
#include "Usefull.h"
#include "Functions.h"

extern char Values[];
#ifdef __COLOR_FILES_BY_EXTENSION__
extern Extensions extensions[];
#endif

extern FileSignatures signatures[];
extern char* editor;

#ifdef __HUMAN_READABLE_SIZE_ENABLE__
void MakeHumanReadAble(char* pointer, unsigned long long int rvalue, bool isDir)
{
    if (rvalue == 0)
    {
        pointer[0] = '0';
        pointer[1] = '\0';
        return;
    }

    if (isDir)
    {
        sprintf(pointer," %lld",rvalue);
        return;
    }

    long double value = (long double)rvalue;
    char Too = 0;
    while (value > 1024)
    {
        value /= 1024;
        Too++;
    }

    sprintf(pointer," %Lf",value);
    if (pointer[MAX_END_READABLE] == '.')
    {
        pointer[MAX_END_READABLE] = ' ';
        pointer[MAX_END_READABLE+1] = Values[Too];
        pointer[MAX_END_READABLE+2] = '\0';
    }
    else
    {
        pointer[MAX_END_READABLE+1] = ' ';
        pointer[MAX_END_READABLE+2] = Values[Too];
        pointer[MAX_END_READABLE+3] = '\0';
    }


}
#endif


#ifdef __GET_DIR_SIZE_ENABLE__
unsigned long long int GetDirSize(int fd, bool Recursive, bool Count)
{
    unsigned long long int size = 0;

    DIR* d = fdopendir(fd);
    struct dirent *dir;

    int tfd;

    if (d)
    {
        if (Count)
        {
            while ((dir = readdir(d)))
            {
                if (dir->d_name[0] == '.' && (dir->d_name[1] == '\0' || (dir->d_name[1] == '.' && dir->d_name[2] == '\0')))
                    continue;
                if (Recursive && dir->d_type == 4)
                {
                    if (faccessat(fd,dir->d_name,R_OK,0) == 0 && (tfd = openat(fd,dir->d_name,O_DIRECTORY)) != -1)
                    {
                        size += GetDirSize(tfd,Recursive,Count);
                        close(tfd);
                    }
                }
                size++;
            }
        }
        else
        {
            struct stat ST;
            while ((dir = readdir(d)))
            {
                if (dir->d_name[0] == '.' && (dir->d_name[1] == '\0' || (dir->d_name[1] == '.' && dir->d_name[2] == '\0')))
                    continue;
                if (dir->d_type == 4)
                {
                    if (Recursive)
                    {
                        if (faccessat(fd,dir->d_name,R_OK,0) == 0 && (tfd = openat(fd,dir->d_name,O_DIRECTORY)) != -1)
                        {
                            size += GetDirSize(tfd,Recursive,Count);
                            close(tfd);
                        }
                    }
                }
                else
                {
                    fstatat(fd,dir->d_name,&ST,AT_SYMLINK_NOFOLLOW);
                    if ((ST.st_mode & S_IFMT) == S_IFREG)
                        size += ST.st_size;
                }
            }

        }
        closedir(d);
    }

    return size;
}
#endif

#ifdef __COLOR_FILES_BY_EXTENSION__
void CheckFileTypeN(struct Element* List, size_t begin, size_t end)
{
    static char stemp[NAME_MAX];
    size_t tse, nse;
    bool found;

    for (int i = begin;  List[i].FType == 1 && i < end; i++)
    {
        found = false;
        tse = strlen(List[i].name);
        nse = 0;

        for (int j = tse-1; j; j--)
        {
            if (List[i].name[j] == '.' && j < tse-1)
            {
                found = true;
                tse = j;
                break;
            }
        }

        if (found)
        {
            strcpy(stemp,List[i].name+tse+1);
            nse = strlen(stemp);

            for (int j = 0; j < nse; j++)
                stemp[j] ^= 32*(stemp[j] > 96 && stemp[j] < 123);

            for (int j = 0; extensions[j].group != 0; j++)
                if (strcmp(stemp,extensions[j].Name) == 0)
                    List[i].FType = extensions[j].group;
        }
        else
            List[i].FType = 0;

    }

}
#endif

size_t xstrsncpy(char *dst, const char *src, size_t n)
{
	char *end = memccpy(dst, src, '\0', n);

	if (!end) {
		dst[n - 1] = '\0';
		end = dst + n;
	}

	return end - dst;
}


char* lsperms(int mode, int type)
{
    static const char * const rwx[] = {"---", "--x", "-w-", "-wx", "r--", "r-x", "rw-", "rwx"};
	static char bits[11] = {0};

    if (type == T_DIR)
        bits[0] = 'd';
    else if (type == T_REG)
        bits[0] = '-';
    else if (type == T_BDEV)
        bits[0] = 'b';
    else if (type == T_DEV)
        bits[0] = 'c';
    else if (type == T_SOCK)
        bits[0] = 's';
    else if (type == T_FIFO)
        bits[0] = 'p';
    else if (type > 6 && type < 14)
        bits[0] = 'l';
    else
       bits[0] = '?';

	xstrsncpy(&bits[1], rwx[(mode >> 6) & 7], 4);
	xstrsncpy(&bits[4], rwx[(mode >> 3) & 7], 4);
	xstrsncpy(&bits[7], rwx[(mode & 7)], 4);

	if (mode & S_ISUID)
		bits[3] = (mode & 0100) ? 's' : 'S';
	if (mode & S_ISGID)
		bits[6] = (mode & 0010) ? 's' : 'l';
	if (mode & S_ISVTX)
		bits[9] = (mode & 0001) ? 't' : 'T';

	return bits;
}

void RunFile(const char* path)
{
    struct stat sFile;
    if (stat(path,&sFile) == -1)
        return;
    if (!(sFile.st_mode&S_IRUSR))
        return;

    if (sFile.st_size == 0)
    {
        if (fork() == 0)
            execlp(editor,editor,path,NULL);
        wait(NULL);
    }

    int fd = open(path,O_RDONLY);

    size_t buf_t = 2048;

    if (buf_t > sFile.st_size)
        buf_t = sFile.st_size;

    char* buf = (char*)malloc(buf_t);

    buf_t = read(fd,buf,buf_t-1);

    int bina = 0;
    bool binary = false;

    for (int i = 0; i < buf_t; i++)
            bina += 1*(buf[i] != 10 && (buf[i] < 32 || buf[i] > 126));

    binary = bina > 32;

    free(buf);
    char* nest = (char*)malloc(32);

    for (int i = 0; signatures[i].sig != NULL; i++)
    {
        if (signatures[i].binary == binary)
        {
            memset(nest,0,32);
            lseek(fd,signatures[i].pos,signatures[i].from);

            buf_t = read(fd,nest,signatures[i].len);

            if (strcmp(nest,signatures[i].sig) == 0)
            {
                if (!signatures[i].RunInBG)
                    endwin();
                if (fork() == 0)
                {
                    if (signatures[i].RunInBG)
                    {
                        int fdc;
                        if ((fdc = open("/dev/null",O_WRONLY)) != -1)
                        {
                            dup2(fdc,1);
                            dup2(fdc,2);
                            close(fdc);
                        }
                    }
                    execlp(signatures[i].comma_com,signatures[i].comma_com,path,NULL);
                }
                if (!signatures[i].RunInBG)
                {
                    wait(NULL);
                    initscr();
                }

                close(fd);
                free(nest);
                return;
            }
        }
    }

    free(nest);

    if (binary == false)
    {
        endwin();
        if (fork() == 0)
            execlp(editor,editor,path,NULL);
        wait(NULL);
        initscr();
    }


    close(fd);
}

extern bool Bar2Enable;

void DeleteFile(int fd, char* name)
{
    struct stat sFile;
    fstatat(fd,name,&sFile,0);
    
    if ((sFile.st_mode& S_IFMT) == S_IFDIR)
    {
        int temp;
        if ((temp = openat(fd,name,O_DIRECTORY)) != -1)
        {
            DIR* d = fdopendir(temp);
            struct dirent *dir;
            if (d)
            {
                while (dir = readdir(d))
                    if (!(dir->d_name[0] == '.' && (dir->d_name[1] == '\0' || (dir->d_name[1] == '.' && dir->d_name[2] == '\0'))))
                        DeleteFile(temp,dir->d_name);
                closedir(d);
            }
            close(temp);
        }
        unlinkat(fd,name,AT_REMOVEDIR);
    }
    else
    {
        unlinkat(fd,name,0);
    }
}

void DeleteGroup(Basic* this, bool here)
{
    int count = 0;

    if (here)
    {
        for (int i = 0; i < this->Work[this->inW].win[1]->El_t; i++)
            if ((this->Work[this->inW].win[1]->El[i].List[this->inW]&this->Work[this->inW].SelectedGroup) == this->Work[this->inW].SelectedGroup)
                count++;
    }
    else
    {
        for (int i = 0; i < this->ActualSize; i++)
            for (int j = 0; j < this->Base[i].El_t; j++)
                if ((this->Base[i].El[j].List[this->inW]&this->Work[this->inW].SelectedGroup) == this->Work[this->inW].SelectedGroup)
                    count++;
    }

    int Event = -1;

    do {
        if (Event == 410)
        {
            DrawBasic(this,-1);
            UpdateSizeBasic(this);
        }

        if (Bar2Enable)
        {
            for (int i = 0; i < this->win[4]->_maxx; i++)
                mvwaddch(this->win[4],0,i,' ');
            if (count > 0)
                mvwprintw(this->win[4],0,0,"Confirm deletion of %d files (y/N)",count);
            else if (!this->Work[this->inW].win[1]->enable && this->Work[this->inW].win[1]->El_t > 0)
                mvwprintw(this->win[4],0,0,"Confirm deletion of %s (y/N)",this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].name);
            wrefresh(this->win[4]);
        }
        Event = getch();
    } while (Event == -1 || Event == 410);

    if (Event == 'y' || Event == 'Y')
    {
        int fd;
        if (count == 0 && !this->Work[this->inW].win[1]->enable && this->Work[this->inW].win[1]->El_t > 0)
        {
            if ((fd = open(this->Work[this->inW].win[1]->path,O_DIRECTORY)) != -1)
            {
                DeleteFile(fd,this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].name);
                close(fd);
            }
        }
        else if (here)
        {
            if ((fd = open(this->Work[this->inW].win[1]->path,O_DIRECTORY)) != -1)
            {
                for (int i = 0; i < this->Work[this->inW].win[1]->El_t; i++)
                    if ((this->Work[this->inW].win[1]->El[i].List[this->inW]&this->Work[this->inW].SelectedGroup) == this->Work[this->inW].SelectedGroup)
                        DeleteFile(fd,this->Work[this->inW].win[1]->El[i].name);
                close(fd);
            }
            
        }
        else
        {
            for (int i = 0; i < this->ActualSize; i++)
            {
                if ((fd = open(this->Base[i].path,O_DIRECTORY)) != -1)
                {
                    for (int j = 0; j < this->Base[i].El_t; j++)
                        if ((this->Base[i].El[j].List[this->inW]&this->Work[this->inW].SelectedGroup) == this->Work[this->inW].SelectedGroup)
                            DeleteFile(fd,this->Base[i].El[j].name);
                    close(fd);
                }
            }
        }
    }
}

extern int CopyBufferSize;

void CopyFile(int fd1, int fd2, char* name, char* buffer, mode_t arg)
{
    struct stat sFile;
    int fd3, fd4;
    fstatat(fd2,name,&sFile,0);

    char* temp = (char*)malloc(NAME_MAX);
    strcpy(temp,name);
    unsigned long long int num = 0;

    while (faccessat(fd1,temp,F_OK,0) != -1)
    {
        if (snprintf(temp,NAME_MAX-1,"%s_%lld",name,num) == NAME_MAX-1)
        {
            free(temp);
            return;
        }
        num++;
    }

    if ((sFile.st_mode& S_IFMT) == S_IFDIR)
    {
        if ((fd3 = openat(fd2,name,O_DIRECTORY)) != -1)
        {
            DIR* d = fdopendir(fd3);
            struct dirent *dir;
            
            if (d)
            {
                mkdirat(fd1,temp,sFile.st_mode);
                if ((fd4 = openat(fd1,temp,O_DIRECTORY)) != -1)
                {
                    while (dir = readdir(d))
                        if (!(dir->d_name[0] == '.' && (dir->d_name[1] == '\0' || (dir->d_name[1] == '.' && dir->d_name[2] == '\0'))))
                            CopyFile(fd4,fd3,dir->d_name,buffer,arg);
                    close(fd4);
                }
                closedir(d);
            }
            
            close(fd3);
        }
    }
    else
    {
        if ((fd3 = openat(fd2,name,O_RDONLY)) != -1)
        {
            if ((fd4 = openat(fd1,temp,O_WRONLY|O_CREAT,sFile.st_mode)) != -1)
            {
                int bytesread;
                while ((bytesread = read(fd3,buffer,CopyBufferSize)) > 0)
                    write(fd4,buffer,bytesread);

                close(fd4);
            }
            
            close(fd3);
        }
    }

    free(temp);
}

void CopyGroup(Basic* this, const char* target, mode_t arg)
{
    int fd1, fd2;
    struct stat sFile1, sFile2;
    char* buffer = (char*)malloc(CopyBufferSize);

    if ((fd1 = open(target,O_DIRECTORY)) != -1)
    {
        if (fstat(fd1,&sFile1) == -1)
        {
            close(fd1);
            free(buffer);
            return;
        }

        for (int i = 0; i < this->ActualSize; i++)
        {
            if ((fd2 = open(this->Base[i].path,O_DIRECTORY)) != -1)
            {
                if (fstat(fd2,&sFile2) == -1)
                {
                    close(fd2);
                    continue;
                }

                for (int j = 0; j < this->Base[i].El_t; j++)
                {
                    if ((this->Base[i].El[j].List[this->inW]&this->Work[this->inW].SelectedGroup) == this->Work[this->inW].SelectedGroup)
                    {
                        CopyFile(fd1,fd2,this->Base[i].El[j].name,buffer,arg);
                    }
                }
                close(fd2);
            }
        }
        close(fd1);
    }
    free(buffer);
}

void MoveFile(int fd1, int fd2, char* name, char* buffer, mode_t arg)
{
    struct stat sFile;
    int fd3, fd4;
    fstatat(fd2,name,&sFile,0);

    char* temp = (char*)malloc(NAME_MAX);
    strcpy(temp,name);
    unsigned long long int num = 0;

    while (faccessat(fd1,temp,F_OK,0) != -1)
    {
        if (snprintf(temp,NAME_MAX-1,"%s_%lld",name,num) == NAME_MAX-1)
        {
            free(temp);
            return;
        }
        num++;
    }

    if ((sFile.st_mode& S_IFMT) == S_IFDIR)
    {
        if ((fd3 = openat(fd2,name,O_DIRECTORY)) != -1)
        {
            DIR* d = fdopendir(fd3);
            struct dirent *dir;
            
            if (d)
            {
                mkdirat(fd1,temp,sFile.st_mode);
                if ((fd4 = openat(fd1,temp,O_DIRECTORY)) != -1)
                {
                    while (dir = readdir(d))
                        if (!(dir->d_name[0] == '.' && (dir->d_name[1] == '\0' || (dir->d_name[1] == '.' && dir->d_name[2] == '\0'))))
                            MoveFile(fd4,fd3,dir->d_name,buffer,arg);
                    close(fd4);
                }
                closedir(d);
            }
            
            close(fd3);
            unlinkat(fd2,name,AT_REMOVEDIR);
        }
    }
    else
    {
        if ((fd3 = openat(fd2,name,O_RDONLY)) != -1)
        {
            if ((fd4 = openat(fd1,temp,O_WRONLY|O_CREAT,sFile.st_mode)) != -1)
            {
                int bytesread;
                while ((bytesread = read(fd3,buffer,CopyBufferSize)) > 0)
                    write(fd4,buffer,bytesread);

                close(fd4);
                unlinkat(fd2,name,0);
            }
            
            close(fd3);
        }
    }

    free(temp);
}

void MoveGroup(Basic* this, const char* target, mode_t arg)
{
    int fd1, fd2;
    struct stat sFile1, sFile2;
    char* buffer = (char*)malloc(CopyBufferSize);

    if ((fd1 = open(target,O_DIRECTORY)) != -1)
    {
        if (fstat(fd1,&sFile1) == -1)
        {
            close(fd1);
            free(buffer);
            return;
        }

        for (int i = 0; i < this->ActualSize; i++)
        {
            if ((fd2 = open(this->Base[i].path,O_DIRECTORY)) != -1)
            {
                if (fstat(fd2,&sFile2) == -1)
                {
                    close(fd2);
                    continue;
                }

                for (int j = 0; j < this->Base[i].El_t; j++)
                {
                    if ((this->Base[i].El[j].List[this->inW]&this->Work[this->inW].SelectedGroup) == this->Work[this->inW].SelectedGroup)
                    {
                        if (sFile1.st_dev == sFile2.st_dev)
                        {
                            char* temp = (char*)malloc(NAME_MAX);
                            strcpy(temp,this->Base[i].El[j].name);
                            unsigned long long int num = 0;

                            while (faccessat(fd1,temp,F_OK,0) != -1)
                            {
                                if (snprintf(temp,NAME_MAX-1,"%s_%lld",this->Base[i].El[j].name,num) == NAME_MAX-1)
                                {
                                    free(temp);
                                    return;
                                }
                                num++;
                            }
                            renameat(fd2,this->Base[i].El[j].name,fd1,temp);
                        }
                        else
                            MoveFile(fd1,fd2,this->Base[i].El[j].name,buffer,arg);
                    }
                }
                close(fd2);
            }
        }
        close(fd1);
    }
    free(buffer);
}

void TimeToStr(time_t *time, char* result)
{
    struct tm *tis = gmtime(time);
    sprintf(result,"%d-%.2d-%.2d %.2d:%.2d ",tis->tm_year+1900,tis->tm_mon+1,tis->tm_mday,tis->tm_hour+2,tis->tm_min);
}

void MakePathShorter(char* path, int max_size)
{
    int size = strlen(path), bottom;
    if (size > 2)
        bottom = 2;
    else
        return;

    while (size > max_size)
    {
        while (path[bottom] != '/')
        {
            for (int j = bottom; j < size; j++)
                path[j] = path[j+1];
            path[size-1] = '\0';
            --size;
        }
        bottom+=2;
        if (bottom > size-1)
            return;
    }

}
