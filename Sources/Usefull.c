#include "main.h"
#include "Usefull.h"

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
    {
       bits[0] = '?';
    }

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

int MoveFile(struct ShortDir* f)
{

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
