#include "main.h"
#include "Usefull.h"
#include "FastRun.h"
#include "Draw.h"
#include "Functions.h"

#ifdef __COLOR_FILES_BY_EXTENSION__
extern Extensions extensions[];
#endif

extern FileSignatures signatures[];

extern Settings* settings;

#ifdef __HUMAN_READABLE_SIZE_ENABLE__
void MakeHumanReadAble(char* pointer, const unsigned long long int rvalue, const bool isDir)
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
    unsigned char Too = 0;
    while (value > 1024)
    {
        value /= 1024;
        Too++;
    }

    sprintf(pointer," %Lf",value);
    if (pointer[MAX_END_READABLE] == '.')
    {
        pointer[MAX_END_READABLE] = ' ';
        pointer[MAX_END_READABLE+1] = settings->Values[Too];
        pointer[MAX_END_READABLE+2] = '\0';
    }
    else
    {
        pointer[MAX_END_READABLE+1] = ' ';
        pointer[MAX_END_READABLE+2] = settings->Values[Too];
        pointer[MAX_END_READABLE+3] = '\0';
    }
}
#endif

#ifdef __GET_DIR_SIZE_ENABLE__
unsigned long long int GetDirSize(const int fd, const bool Recursive, const bool Count)
{
    unsigned long long int size = 0;

    DIR* d = fdopendir(fd);
    struct dirent *dir;

    int tfd;
    struct stat ST;

    if (d)
    {
        while ((dir = readdir(d)))
        {
            if (dir->d_name[0] == '.' && (dir->d_name[1] == '\0' || (dir->d_name[1] == '.' && dir->d_name[2] == '\0')))
                continue;
            if (Recursive && dir->d_type == 4 &&
                faccessat(fd,dir->d_name,R_OK,0) == 0 && (tfd = openat(fd,dir->d_name,O_DIRECTORY)) != -1)
            {
                size += GetDirSize(tfd,Recursive,Count);
                close(tfd);
            }
            else
            {
                if (Count)
                    size++;
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
void CheckFileExtension(const char* name, unsigned char* FType)
{
    if (*FType == 0)
        return;

    static char stemp[NAME_MAX];
    static size_t tse, nse;
    static bool found;

    found = false;
    tse = strlen(name);
    nse = 0;

    for (size_t j = tse-1; j; j--)
    {
        if (name[j] == '.' && j < tse-1)
        {
            found = true;
            tse = j;
            break;
        }
    }

    if (found)
    {
        strcpy(stemp,name+tse+1);
        nse = strlen(stemp);

        for (size_t j = 0; j < nse; j++)
            stemp[j] ^= 32*(stemp[j] > 96 && stemp[j] < 123);

        for (size_t j = 0; extensions[j].group != 0; j++)
            if (strcmp(stemp,extensions[j].Name) == 0)
                *FType = extensions[j].group;
    }
    else
        *FType = 0;
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

char* lsperms(const int mode, const int type)
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

    if (strcmp(settings->FileOpener,"NULL") != 0)
    {
        endwin();
        if (fork() == 0)
            execlp(settings->FileOpener,settings->FileOpener,path,NULL);
        wait(NULL);
        initscr();
    }
    else
    {
        struct stat sFile;
        if (stat(path,&sFile) == -1)
            return;
        if (!(sFile.st_mode&S_IRUSR))
            return;

        if (sFile.st_size == 0)
        {
            endwin();
            if (fork() == 0)
                execlp(settings->editor,settings->editor,path,NULL);
            wait(NULL);
            initscr();
            return;
        }

        int fd;
        if ((fd = open(path,O_RDONLY)) == -1)
            return;

        size_t buf_t = 2048;

        if (buf_t > (size_t)sFile.st_size)
            buf_t = (size_t)sFile.st_size;

        static char buf[PATH_MAX];

        buf_t = read(fd,buf,buf_t-1);

        int bina = 0;
        bool binary = false;

        for (size_t i = 0; i < buf_t; i++)
                bina += 1*(buf[i] != 10 && (buf[i] < 32 || buf[i] > 126));

        binary = bina > 32;

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
                execlp(settings->editor,settings->editor,path,NULL);
            wait(NULL);
            initscr();
        }

        close(fd);
    }
}

void DeleteFile(const int fd, const char* name)
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
                while ((dir = readdir(d)))
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

void DeleteGroup(Basic* grf, const bool here)
{
    int count = 0;

    if (here)
    {
        for (long long int i = 0; i < grf->Work[grf->inW].win[1]->El_t; i++)
            if ((grf->Work[grf->inW].win[1]->El[i].List[grf->inW]&grf->Work[grf->inW].SelectedGroup) == grf->Work[grf->inW].SelectedGroup)
                count++;
    }
    else
    {
        for (size_t i = 0; i < grf->ActualSize; i++)
            for (long long int j = 0; j < grf->Base[i].El_t; j++)
                if ((grf->Base[i].El[j].List[grf->inW]&grf->Work[grf->inW].SelectedGroup) == grf->Work[grf->inW].SelectedGroup)
                    count++;
    }

    int Event = -1;

    do {
        if (Event == 410)
        {
            DrawBasic(grf,-1);
            UpdateSizeBasic(grf);
        }

        if (settings->Bar2Enable)
        {
            werase(grf->win[4]);
            if (count > 0)
                mvwprintw(grf->win[4],0,0,"Confirm deletion of %d files (y/N)",count);
            else if (
                #ifdef __THREADS_FOR_DIR_ENABLE__
                !grf->Work[grf->inW].win[1]->enable &&
                #endif
                grf->Work[grf->inW].win[1]->El_t > 0)
                mvwprintw(grf->win[4],0,0,"Confirm deletion of %s (y/N)",grf->Work[grf->inW].win[1]->El[grf->Work[grf->inW].win[1]->selected[grf->inW]].name);
            wrefresh(grf->win[4]);
        }
        Event = getch();
    } while (Event == -1 || Event == 410);

    if (Event == 'y' || Event == 'Y')
    {
        int fd;
        if (count == 0 &&
        #ifdef __THREADS_FOR_DIR_ENABLE__
        !grf->Work[grf->inW].win[1]->enable &&
        #endif
        grf->Work[grf->inW].win[1]->El_t > 0)
        {
            if ((fd = open(grf->Work[grf->inW].win[1]->path,O_DIRECTORY)) != -1)
            {
                DeleteFile(fd,grf->Work[grf->inW].win[1]->El[grf->Work[grf->inW].win[1]->selected[grf->inW]].name);
                close(fd);
            }
        }
        else if (here)
        {
            if ((fd = open(grf->Work[grf->inW].win[1]->path,O_DIRECTORY)) != -1)
            {
                for (long long int i = 0; i < grf->Work[grf->inW].win[1]->El_t; i++)
                    if ((grf->Work[grf->inW].win[1]->El[i].List[grf->inW]&grf->Work[grf->inW].SelectedGroup) == grf->Work[grf->inW].SelectedGroup)
                        DeleteFile(fd,grf->Work[grf->inW].win[1]->El[i].name);
                close(fd);
            }

        }
        else
        {
            for (size_t i = 0; i < grf->ActualSize; i++)
            {
                if ((fd = open(grf->Base[i].path,O_DIRECTORY)) != -1)
                {
                    for (long long int j = 0; j < grf->Base[i].El_t; j++)
                        if ((grf->Base[i].El[j].List[grf->inW]&grf->Work[grf->inW].SelectedGroup) == grf->Work[grf->inW].SelectedGroup)
                            DeleteFile(fd,grf->Base[i].El[j].name);
                    close(fd);
                }
            }
        }
    }
}

void CopyFile(const int fd1, const int fd2, const char* name, char* buffer, const mode_t arg)
{
    struct stat sFile;
    int fd3, fd4;
    fstatat(fd2,name,&sFile,0);

    char* temp = (char*)malloc(NAME_MAX);
    strcpy(temp,name);
    unsigned long long int num = 0;

    if (!(arg&M_MERGE && (sFile.st_mode&S_IFMT) == S_IFDIR))
    {
        if (arg&M_CHNAME)
        {
            while (faccessat(fd1,temp,F_OK,0) == 0)
            {
                if (snprintf(temp,NAME_MAX-1,"%s_%lld",name,num) == NAME_MAX-1)
                {
                    free(temp);
                    return;
                }
                num++;
            }
        }
        else if (arg&M_DCPY)
        {
            if (faccessat(fd1,temp,F_OK,0) != 0)
            {
                free(temp);
                return;
            }
        }
        else if (arg&M_REPLACE)
            DeleteFile(fd1,temp);
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
                    while ((dir = readdir(d)))
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
                while ((bytesread = read(fd3,buffer,settings->CopyBufferSize)) > 0)
                    write(fd4,buffer,bytesread);

                close(fd4);
            }

            close(fd3);
        }
    }

    free(temp);
}

void CopyGroup(Basic* grf, const char* target, const mode_t arg)
{
    int fd1, fd2;
    struct stat sFile1, sFile2;
    char* buffer = (char*)malloc(settings->CopyBufferSize);

    if ((fd1 = open(target,O_DIRECTORY)) != -1)
    {
        if (fstat(fd1,&sFile1) == -1)
        {
            close(fd1);
            free(buffer);
            return;
        }

        for (size_t i = 0; i < grf->ActualSize; i++)
        {
            if ((fd2 = open(grf->Base[i].path,O_DIRECTORY)) != -1)
            {
                if (fstat(fd2,&sFile2) == -1)
                {
                    close(fd2);
                    continue;
                }

                for (long long int j = 0; j < grf->Base[i].El_t; j++)
                {
                    if ((grf->Base[i].El[j].List[grf->inW]&grf->Work[grf->inW].SelectedGroup) == grf->Work[grf->inW].SelectedGroup)
                    {
                        CopyFile(fd1,fd2,grf->Base[i].El[j].name,buffer,arg);
                    }
                }
                close(fd2);
            }
        }
        close(fd1);
    }
    free(buffer);
}

void MoveFile(const int fd1, const int fd2, const char* name, char* buffer, const mode_t arg)
{
    struct stat sFile;
    int fd3, fd4;
    fstatat(fd2,name,&sFile,0);

    char* temp = (char*)malloc(NAME_MAX);
    strcpy(temp,name);
    unsigned long long int num = 0;

    if (!(arg&M_MERGE && (sFile.st_mode&S_IFMT) == S_IFDIR))
    {
        if (arg&M_CHNAME)
        {
            while (faccessat(fd1,temp,F_OK,0) == 0)
            {
                if (snprintf(temp,NAME_MAX-1,"%s_%lld",name,num) == NAME_MAX-1)
                {
                    free(temp);
                    return;
                }
                num++;
            }
        }
        else if (arg&M_DCPY)
        {
            if (faccessat(fd1,temp,F_OK,0) != 0)
            {
                free(temp);
                return;
            }
        }
        else if (arg&M_REPLACE)
            DeleteFile(fd1,temp);
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
                    while ((dir = readdir(d)))
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
                while ((bytesread = read(fd3,buffer,settings->CopyBufferSize)) > 0)
                    write(fd4,buffer,bytesread);

                close(fd4);
                unlinkat(fd2,name,0);
            }

            close(fd3);
        }
    }

    free(temp);
}

void MoveGroup(Basic* grf, const char* target, const mode_t arg)
{
    int fd1, fd2;
    struct stat sFile1, sFile2;
    char* buffer = (char*)malloc(settings->CopyBufferSize);

    if ((fd1 = open(target,O_DIRECTORY)) != -1)
    {
        if (fstat(fd1,&sFile1) == -1)
        {
            close(fd1);
            free(buffer);
            return;
        }

        for (size_t i = 0; i < grf->ActualSize; i++)
        {
            if ((fd2 = open(grf->Base[i].path,O_DIRECTORY)) != -1)
            {
                if (fstat(fd2,&sFile2) == -1)
                {
                    close(fd2);
                    continue;
                }

                for (long long int j = 0; j < grf->Base[i].El_t; j++)
                {
                    if ((grf->Base[i].El[j].List[grf->inW]&grf->Work[grf->inW].SelectedGroup) == grf->Work[grf->inW].SelectedGroup)
                    {
                        if (sFile1.st_dev == sFile2.st_dev)
                        {
                            char* temp = (char*)malloc(NAME_MAX);
                            strcpy(temp,grf->Base[i].El[j].name);
                            unsigned long long int num = 0;

                            while (faccessat(fd1,temp,F_OK,0) != -1)
                            {
                                if (snprintf(temp,NAME_MAX-1,"%s_%lld",grf->Base[i].El[j].name,num) == NAME_MAX-1)
                                {
                                    free(temp);
                                    return;
                                }
                                num++;
                            }
                            renameat(fd2,grf->Base[i].El[j].name,fd1,temp);
                        }
                        else
                            MoveFile(fd1,fd2,grf->Base[i].El[j].name,buffer,arg);
                    }
                }
                close(fd2);
            }
        }
        close(fd1);
    }
    free(buffer);
}

size_t TimeToStr(const time_t *time, char* result)
{
    struct tm *tis = gmtime(time);
    return sprintf(result,"%d-%.2d-%.2d %.2d:%.2d ",tis->tm_year+1900,tis->tm_mon+1,tis->tm_mday,tis->tm_hour+2,tis->tm_min);
}

void MakePathShorter(char* path, const int max_size)
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

size_t FindFirstCharacter(const char* src)
{
    size_t pos = 0;
    while (src[pos] && isspace(src[pos])) pos++;
    return pos;
}

extern struct AliasesT aliases[];

size_t StrToValue(void* dest, const char* src, char* temp)
{
    size_t PosBegin = 0, PosEnd = 0;
    if (src[PosBegin] == '{')
    {
        char temp2[NAME_MAX];
        for (int i = 0; src[PosBegin] != '}'; i++)
        {
            PosBegin++;
            PosEnd = 0;
            PosBegin += FindFirstCharacter(src+PosBegin);
            while (src[PosBegin+PosEnd] && !isspace(src[PosBegin+PosEnd]) && src[PosBegin+PosEnd] != ',' && src[PosBegin+PosEnd] != '}')
                PosEnd++;
            
            strncpy(temp2,src+PosBegin,PosEnd);
            temp2[PosEnd] = '\0';

            PosBegin += PosEnd;
            PosBegin += FindFirstCharacter(src+PosBegin);

            StrToValue(&(*(long int**)dest)[i],temp2,temp);
        }
        PosBegin++;
    }
    else if (src[PosBegin] == '\'')
    {
        PosBegin++;
        PosEnd += FindEndOf(src+PosBegin,'\'');
        strncpy(*(char**)dest,src+PosBegin,PosEnd);
        (*(char**)dest)[PosEnd] = '\0';
        PosBegin += PosEnd+1;
    }
    else
    {
        int type;
        *(bool*)dest = 0;

        do {
            PosEnd = 0;
            type = 0;
            while (src[PosBegin+PosEnd] && !isspace(src[PosBegin+PosEnd]) && src[PosBegin+PosEnd] != '|')
            {
                if (src[PosBegin+PosEnd] == '.')
                    type |= 0x1;
                else if (isalpha(src[PosBegin+PosEnd]))
                    type |= 0x2;
                PosEnd++;
            }
            memcpy(temp,src+PosBegin,PosEnd);
            temp[PosEnd] = '\0';

            if (type == 0) { *(long int*)dest |= atol(temp); }
            else if (type == 1)
            {
                *(double*)dest = atof(temp);
                PosBegin += PosEnd;
                PosBegin += FindFirstCharacter(src+PosBegin);
                return PosBegin;
            }
            else
                for (int i = 0; aliases[i].name; i++)
                    if (strlen(aliases[i].name) == PosEnd && strncmp(temp,aliases[i].name,PosEnd) == 0)
                    {
                        *(long int*)dest |= aliases[i].v;
                        break;
                    }
            
            PosBegin += PosEnd;
        } while (src[PosBegin++] == '|');
    
    }
    return PosBegin;
}


char* StrConv(char* dest)
{
    for (int i = 0; dest[i]; i++)
    {
        if (dest[i] == '\\' && dest[i+1])
        {
            for (int j = i; dest[j]; j++)
                dest[j] = dest[j+1];

            switch (dest[i])
            {
                case '0':
                    dest[i] = '\x0';
                    break;
                case 'a':
                    dest[i] = '\x7';
                    break;
                case 'b':
                    dest[i] = '\x8';
                    break;
                case 't':
                    dest[i] = '\x9';
                    break;
                case 'n':
                    dest[i] = '\xA';
                    break;
                case 'v':
                    dest[i] = '\xB';
                    break;
                case 'f':
                    dest[i] = '\xC';
                    break;
                case 'r':
                    dest[i] = '\xD';
                    break;
                case '\'':
                    dest[i] = '\x27';
                    break;
                case '\\':
                    dest[i] = '\x5C';
                    break;
            }
        }
    }
    return dest;
}

char* StrToKeys(char* dest)
{
    StrConv(dest);
    for (int i = 0; dest[i]; i++)
    {
        if (dest[i] == '<' && dest[i+1] == 'C' && dest[i+2] == '-' && dest[i+3] && dest[i+4] == '>')
        {
            for (int g = 0; g < 4; g++)
                for (int j = i+(g == 3); dest[j]; j++)
                    dest[j] = dest[j+1];
            dest[i] &= 0x1f;
        }
        else if (strncmp(dest+i,"<space>",7) == 0)
        {
            for (int g = 0; g < 6; g++)
                for (int j = i; dest[j]; j++)
                    dest[j] = dest[j+1];
            dest[i] = ' ';  
        }
        else if (strncmp(dest+i,"<esc>",5) == 0)
        {
            for (int g = 0; g < 4; g++)
                for (int j = i; dest[j]; j++)
                    dest[j] = dest[j+1];
            dest[i] = 27;
        }
    }
    return dest;
}

size_t FindEndOf(const char* src, const char res)
{
    size_t end = 0;
    while (src[end++])
    {
        if (src[end] == res && src[end-1] != '\\')
            break;
    }
    return end;
}



