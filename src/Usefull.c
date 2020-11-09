/*
    csas - console file manager
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

int spawn(char* file, char* arg1, char* arg2, const uchar flag)
{
    if (!file || !*file) return -1;

    char* argv[4] = {NULL};

    if (!arg1 && arg2)
    {
        arg1 = arg2;
        arg2 = NULL;
    }

    argv[0] = file;
    argv[1] = arg1;
    argv[2] = arg2;

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

char* MakeHumanReadAble(ull value)
{
    static char ret[8];
    if (value == 0)
    {
        ret[0] = '0';
        ret[1] = '\0';
        return ret;
    }

    off_t rem = 0;

    uchar Too = 0;
    while (value >= 1024)
    {
        rem = value & 0x3ff;
        value >>= 10;
        Too++;
    }

    uchar i;

    for (i = 0; value != 0; i++)
    {
        ret[i] = value%10+48;
        value /= 10;
    }
    for (uchar temp, j = 0, g = i-1; j < g; j++, g--)
    {
        temp = ret[j];
        ret[j] = ret[g];
        ret[g] = temp;
    }

    if (rem != 0 && i < 3)
    {
        rem = (rem*1000) >> 10;
        if (i == 2)
        {
            rem /= 10;
            if (rem%10 >= 5)
            {
                rem = (rem/10) + 1;
                rem = rem*(rem != 10);
            }
            else
                rem /= 10;
        }
        else
        {
            if (rem%10 >= 5)
            {
                rem = (rem/10) + 1;
                rem = rem*(rem != 100);
            }
            else
                rem /= 10;
        }

        ret[i++] = '.';
        for (; rem != 0; i++)
        {
            ret[i] = rem%10+48;
            rem /= 10;
        }
        if (ret[i-3] == '.')
        {
            uchar temp = ret[i-1];
            ret[i-1] = ret[i-2];
            ret[i-2] = temp;
        }
    }

    if (Too != 0)
        ret[i++] = settings->Values[Too-1];

    ret[i] = '\0';

    return ret;
}

void GetDirSize(const int fd, ull* count, ull* size, const bool recursive)
{
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
            if (recursive && dir->d_type == 4 &&
                faccessat(fd,dir->d_name,R_OK,0) == 0 && (tfd = openat(fd,dir->d_name,O_DIRECTORY)) != -1)
            {
                GetDirSize(tfd,count,size,recursive);
                close(tfd);
            }
            else
            {
                (*count)++;
                fstatat(fd,dir->d_name,&ST,AT_SYMLINK_NOFOLLOW);
                switch (ST.st_mode & S_IFMT)
                {
                    case S_IFREG:
                    case S_IFDIR:
                        *size += ST.st_size;
                    break;
                }
            }
        }

        closedir(d);
    }
}

#ifdef __COLOR_FILES_BY_EXTENSION__
uchar check_extension(const char* name)
{
    register char* ret = memrchr(name,'.',strlen(name)-1);

    if (ret == NULL) return 0;
    ret++;

    for (register size_t j = 0; extensions[j].group != 0; j++)
        if (strcasecmp(ret,extensions[j].Name) == 0)
            return extensions[j].group;

    return 0;
}
#endif

char* lsperms(const int mode, const int type)
{
    const char* const rwx[] = {"---", "--x", "-w-", "-wx", "r--", "r-x", "rw-", "rwx"};
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

        for (register size_t i = 0; i < buf_t; i++)
            bina += 1*!(isascii(buf[i]));

        binary = bina > 32;

        char* nest = (char*)malloc(32);

        for (register int i = 0; signatures[i].sig != NULL; i++)
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
    ull num = 0;

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
    ull num = 0;

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
    struct tm *tis = localtime(time);
    return sprintf(result,"%d-%02d-%.2d %02d:%02d ",tis->tm_year+1900,tis->tm_mon+1,tis->tm_mday,tis->tm_hour,tis->tm_min);
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
    while (isspace(src[pos])) pos++;
    return pos;
}

extern struct AliasesT aliases[];

size_t StrToValue(void* dest, const char* src)
{
    size_t PosBegin = 0, PosEnd = 0;
    static char temp[8192];
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

            StrToValue(&(*(li**)dest)[i],temp);
        }
        PosBegin++;
    }
    else if (src[PosBegin] == '\'')
    {
        PosBegin++;
        PosEnd = FindEndOf(src+PosBegin,'\'');
        strncpy(*(char**)dest,src+PosBegin,PosEnd);
        (*(char**)dest)[PosEnd] = '\0';
        PosBegin += PosEnd+2;
    }
    else if (src[PosBegin] == '"')
    {
        PosBegin++;
        PosEnd = FindEndOf(src+PosBegin,'"');
        for (size_t i = PosBegin, x = 0; i <= PosEnd; i++, x++)
        {
            if (src[i] == '\\' && src[i+1])
            {
                i++;
                (*(char**)dest)[x] = CharConv(src[i]);
                continue;
            }

            if (src[i] == '$' && src[i+1] == '{')
            {
                i += 2;
                size_t end = FindEndOf(src+i,'}');
                strncpy(temp,src+i,end);
                temp[end] = 0;
                char* temp2 = getenv(temp);
                if (temp2)
                {
                    size_t end1 = strlen(temp2);
                    memcpy(*((char**)dest)+x,temp2,end1);
                    x += end1-1;
                }

                i += end;
                continue;
            }

            (*(char**)dest)[x] = src[i];
        }
        PosBegin += PosEnd+2;
    }
    else
    {
        int type;
        *(li*)dest = 0;

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

            if (type == 0) { *(li*)dest |= atol(temp); }
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
                        *(li*)dest |= aliases[i].v;
                        break;
                    }

            PosBegin += PosEnd;
        } while (src[PosBegin++] == '|');

    }
    return PosBegin;
}

char CharConv(const char dest)
{
    char ret = dest;
    switch (dest)
    {
        case '0': ret = '\x0'; break;
        case 'a': ret = '\x7'; break;
        case 'b': ret = '\x8'; break;
        case 't': ret = '\x9'; break;
        case 'n': ret = '\xA'; break;
        case 'v': ret = '\xB'; break;
        case 'f': ret = '\xC'; break;
        case 'r': ret = '\xD'; break;
    }
    return ret;
}

char* StrToKeys(char* dest)
{
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
        else if (dest[i] == '\\' && dest[i+1])
        {
            size_t j;
            for (j = i; j < strlen(dest)-1; j++)
                dest[j] = dest[j+1];
            dest[j+1] = '\0';
            dest[i] = CharConv(dest[i]);
        }
    }
    return dest;
}

size_t StrToPath(char* dest, const char* src)
{
    size_t pos = 0, x;
    if (src[pos] == '"' || src[pos] == '\'')
    {
        pos = StrToValue(&dest,src);
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

    return pos;
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
