/*
    csas - terminal file manager
    Copyright (C) 2020 TUVIMEN <suchora.dominik7@gmail.com>

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
#include "Usefull.h"
#include "FastRun.h"
#include "Draw.h"
#include "Functions.h"

#ifdef __COLOR_FILES_BY_EXTENSION__
extern Extensions extensions[];
#endif

extern FileSignatures signatures[];

extern Settings* settings;

int spawn(char* file, char* arg1, char* arg2, const unsigned char flag)
{
    if (!file || !*file) return -1;

    char* argv[4] = {0};

    if (!arg1 && arg2)
    {
        arg1 = arg2;
        arg2 = NULL;
    }

    argv[0] = file;
    argv[1] = arg1;
    argv[2] = arg2;
    argv[3] = NULL;

    if (flag&F_NORMAL) endwin();

    pid_t pid = fork();

    if (pid == 0)
    {
        signal(SIGHUP, SIG_DFL);
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);

        if (flag&F_SILENT)
        {
            int fd = open("/dev/null",O_WRONLY);
            dup2(fd,1);
            dup2(fd,2);
            close(fd);
        }
        execvp(file,argv);
        _exit(1);
    }
    else
    {
        if (flag&F_WAIT)
            while (waitpid(pid,NULL,0) == -1);

        if (flag&F_NORMAL)
        {
            if (flag&F_CONFIRM)
            {
                printf("\nPress ENTER to continue");
                fflush(stdout);
                while (getch() != '\n');
            }
            refresh();
        }
    }

    return 0;
}

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
unsigned char CheckFileExtension(const char* name)
{
    static char stemp[NAME_MAX];
    static size_t tse, nse;
    static bool found;

    found = false;
    tse = strlen(name);
    nse = 0;

    for (register size_t j = tse-1; j; j--)
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

        for (register size_t j = 0; j < nse; j++)
            stemp[j] ^= 32*(stemp[j] > 96 && stemp[j] < 123);

        for (register size_t j = 0; extensions[j].group != 0; j++)
            if (strcmp(stemp,extensions[j].Name) == 0)
                return extensions[j].group;
    }
    
    return 0;
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
    static const char* const rwx[] = {"---", "--x", "-w-", "-wx", "r--", "r-x", "rw-", "rwx"};
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

    memcpy(bits+1,rwx[(mode >> 6)&7],4);
    memcpy(bits+4,rwx[(mode >> 3)&7],4);
    memcpy(bits+7,rwx[mode&7],4);

	if (mode & S_ISUID)
		bits[3] = (mode&0100) ? 's' : 'S';
	if (mode & S_ISGID)
		bits[6] = (mode&0010) ? 's' : 'l';
	if (mode & S_ISVTX)
		bits[9] = (mode&0001) ? 't' : 'T';

	return bits;
}

void RunFile(char* path)
{
    if (strcmp(settings->FileOpener,"NULL") != 0)
        spawn(settings->FileOpener,path,NULL,F_NORMAL|F_WAIT);
    else
    {
        struct stat sFile;
        if (stat(path,&sFile) == -1)
            return;
        if (!(sFile.st_mode&S_IRUSR))
            return;

        if (sFile.st_size == 0)
        {
            spawn(settings->editor,path,NULL,F_NORMAL|F_WAIT);
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
                    spawn(signatures[i].comma_com,path,NULL,signatures->RunInBG ? F_SILENT : F_NORMAL|F_WAIT);
                    return;
                }
            }
        }

        free(nest);

        if (binary == false)
            spawn(settings->editor,path,NULL,F_NORMAL|F_WAIT);

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
                if (snprintf(temp,NAME_MAX-1,"%s_%lld",name,num) == NAME_MAX)
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

char* MakePath(const char* dir, const char* name)
{
    static char path[PATH_MAX];
    strcpy(path,dir);
    size_t dir_t = strlen(dir);
    if (path[0] == '/' && path[1] != '\0')
        path[dir_t++] = '/';
    strcpy(path+dir_t,name);
    return path;
}

size_t FindFirstCharacter(const char* src)
{
    size_t pos = 0;
    while (src[pos] && isspace(src[pos])) pos++;
    return pos;
}

extern struct AliasesT aliases[];

size_t StrToValue(void* dest, const char* src)
{
    size_t PosBegin = 0, PosEnd = 0;
    char temp[8192];
    if (src[PosBegin] == '{')
    {
        for (int i = 0; src[PosBegin] != '}'; i++)
        {
            PosBegin++;
            PosEnd = 0;
            PosBegin += FindFirstCharacter(src+PosBegin);
            while (src[PosBegin+PosEnd] && !isspace(src[PosBegin+PosEnd]) && src[PosBegin+PosEnd] != ',' && src[PosBegin+PosEnd] != '}')
                PosEnd++;
            
            strncpy(temp,src+PosBegin,PosEnd);
            temp[PosEnd] = '\0';

            PosBegin += PosEnd;
            PosBegin += FindFirstCharacter(src+PosBegin);

            StrToValue(&(*(long int**)dest)[i],temp);
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
        *(long int*)dest = 0;

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
            }
        }
    }
    return dest;
}

size_t StrToPath(char* dest, const char* src)
{
    size_t pos = 0, x, end;
    if (src[pos] == '"')
    {
        pos++;
        x = 0;
        while (src[pos])
        {
            if (src[pos] == '"' && src[pos-1] != '\\')
                break;
            if (src[pos] == '$' && src[pos-1] != '\\' && src[pos+1] == '{')
            {
                pos += 2;
                char temp1[NAME_MAX];
                end = FindEndOf(src+pos,'}');
                strncpy(temp1,src+pos,end);
                temp1[end] = '\0';
                char* temp2 = getenv(temp1);
                if (temp2)
                {
                    memcpy(dest+x,temp2,strlen(temp2));
                    x += strlen(temp2);
                }
                pos += end+1;
                continue;
            }
            
            dest[x++] = src[pos++];
        }
        pos++;
    }
    else if (src[pos] == '\'')
    {
        pos++;
        end = FindEndOf(src+pos,'\'');
        strncpy(dest,src+pos,end);
        dest[end] = '\0';
        pos += end+1;
    }
    else
    {
        x = 0;
        while (src[pos] && !isspace(src[pos]))
        {
            if (src[pos] == '\\')
                dest[x] = src[++pos];
            else
                dest[x] = src[pos];
            x++;
            pos++;
        }
        dest[x] = '\0';
    }
    StrConv(dest);
    return pos;
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

char* MakePathRunAble(char* temp)
{
    for (size_t i = 0; i < strlen(temp); i++)
    {
        if(temp[i] == '\\' || temp[i] == '\"' || temp[i] == '\'' || temp[i] == ' ' || temp[i] == '(' || temp[i] == ')' || temp[i] == '[' || temp[i] == ']' || temp[i] == '{' || temp[i] == '}')
        {
            for (size_t j = strlen(temp); j > i; j--)
                temp[j] = temp[j-1];
            temp[i] = '\\';
            i++;
        }
    }
    return temp;
}

