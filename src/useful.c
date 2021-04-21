/*
    csas - console file manager
    Copyright (C) 2020-2021 TUVIMEN <suchora.dominik7@gmail.com>

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
#include "preview.h"
#include "draw.h"
#include "inits.h"

#ifdef __COLOR_FILES_BY_EXTENSION__
extern Extensions extensions[];
#endif

extern FileSignatures signatures[];

extern Settings *cfg;

void die(int status, const char *p, ...)
{
    va_list args;
    va_start(args,p);
    vfprintf(stderr,p,args);
    va_end(args);
    exit(status);
}

int spawn(char *file, char *arg1, char *arg2, const uchar flag)
{
    if (!file || !*file) return -1;

    char *argv[4] = {NULL};

    if (!arg1 && arg2)
    {
        arg1 = arg2;
        arg2 = NULL;
    }

    argv[0] = file;
    argv[1] = arg1;
    argv[2] = arg2;

    if (flag&F_NORMAL) endwin();

    pid_t pid = xfork(flag);

    if (pid == 0)
    {
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

char *stoa(ull value)
{
    static char ret[8];
    if (value == 0)
    {
        ret[0] = '0';
        ret[1] = '\0';
        return ret;
    }

    off_t rem = 0;

    uchar too = 0;
    while (value >= 1024)
    {
        rem = value & 0x3ff;
        value >>= 10;
        too++;
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
        if (i == 1)
        {
            rem = (rem*1000)>>10;
            rem /= 10;
            if (rem%10 >= 5)
            {
                rem = (rem/10) + 1;
                if (rem == 10)
                    rem = 0;
            }
            else
                rem /= 10;
        }
        else if (i == 2)
        {
            rem = (rem*1000)>>10;
            if (rem%10 >= 5)
            {
                rem = (rem/10)+1;
                if (rem == 100)
                    rem = 0;
            }
            else
                rem /= 10;
        }
        else if (i > 0)
        {
            rem = (rem*10000)>>10;
            if (rem%10 >= 5) {
                rem = (rem/10)+1;
                if (rem == 1000)
                    rem = 0;
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

        if (ret[i-1] == '.')
            ret[i---1] = 0;
    }

    if (too != 0)
        ret[i++] = cfg->Values[too-1];

    ret[i] = '\0';

    return ret;
}

void get_dirsize(const int fd, ull *count, ull *size, const bool recursive)
{
    DIR *d = fdopendir(fd);
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
                get_dirsize(tfd,count,size,recursive);
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
uchar check_extension(const char *name)
{
    register char *ret = memrchr(name,'.',strlen(name)-1);

    if (ret == NULL) return 0;
    ret++;

    for (register size_t j = 0; extensions[j].group != 0; j++)
        if (strcasecmp(ret,extensions[j].name) == 0)
            return extensions[j].group;

    return 0;
}
#endif

char *lsperms(const int mode)
{
    const char *const rwx[] = {"---", "--x", "-w-", "-wx", "r--", "r-x", "rw-", "rwx"};
	static char bits[11] = {0};

    switch (mode & S_IFMT)
    {
        case S_IFREG:
            bits[0] = '-';
            break;
        case S_IFDIR:
            bits[0] = 'd';
            break;
        case S_IFLNK:
            bits[0] = 'l';
            break;
        case S_IFSOCK:
            bits[0] = 's';
            break;
        case S_IFIFO:
            bits[0] = 'p';
            break;
        case S_IFBLK:
            bits[0] = 'b';
            break;
        case S_IFCHR:
            bits[0] = 'c';
            break;
        default:
            bits[0] = '?';
            break;
    }

    memcpy(bits+1,rwx[(mode >> 6)&7],4);
    memcpy(bits+4,rwx[(mode >> 3)&7],4);
    memcpy(bits+7,rwx[mode&7],4);

	return bits;
}

pid_t xfork(uchar flag)
{
    int status;
    pid_t p = fork();
    struct sigaction act = {.sa_handler=SIG_DFL};

    if (p > 0)
    {
        sigaction(SIGHUP,&(struct sigaction){.sa_handler=SIG_IGN},NULL);
		sigaction(SIGTSTP,&act,NULL);
    }
    else if (p == 0)
    {
        if (flag&F_WAIT)
        {
            sigaction(SIGHUP,&act,NULL);
            sigaction(SIGINT,&act,NULL);
            sigaction(SIGQUIT,&act,NULL);
            sigaction(SIGTSTP,&act,NULL);
		}
        else
        {
            p = fork();
			if (p > 0)
				_exit(EXIT_SUCCESS);
			else if (p == 0)
            {
				sigaction(SIGHUP,&act,NULL);
				sigaction(SIGINT,&act,NULL);
				sigaction(SIGQUIT,&act,NULL);
				sigaction(SIGTSTP,&act,NULL);
				setsid();
				return p;
			}
			_exit(EXIT_FAILURE);
        }
    }

    if (!(flag&F_WAIT))
	    waitpid(p,&status,0);

    return p;
}

void file_run(char *path)
{
    if (strcmp(cfg->FileOpener,"NULL") != 0)
        spawn(cfg->FileOpener,path,NULL,F_NORMAL|F_WAIT);
    else
    {
        struct stat sfile;
        if (stat(path,&sfile) == -1)
            return;
        if (!(sfile.st_mode&S_IRUSR))
            return;

        if (sfile.st_size == 0)
        {
            spawn(cfg->editor,path,NULL,F_NORMAL|F_WAIT);
            return;
        }

        int fd;
        if ((fd = open(path,O_RDONLY)) == -1)
            return;

        size_t buf_t = 2048;

        if (buf_t > (size_t)sfile.st_size)
            buf_t = (size_t)sfile.st_size;

        char buf[PATH_MAX];

        buf_t = read(fd,buf,buf_t-1);

        int bina = 0;
        bool binary = false;

        for (register size_t i = 0; i < buf_t; i++)
            bina += 1*!(isascii(buf[i]));

        binary = bina > 32;

        char *nest = (char*)malloc(32);

        for (register int i = 0; signatures[i].sig != NULL; i++)
        {
            if (signatures[i].binary == binary)
            {
                memset(nest,0,32);
                lseek(fd,signatures[i].pos,signatures[i].from);

                buf_t = read(fd,nest,signatures[i].len);

                if (strcmp(nest,signatures[i].sig) == 0)
                {
                    spawn(signatures[i].comma_com,path,NULL,signatures->run_in_bg ? F_SILENT : F_NORMAL|F_WAIT);
                    return;
                }
            }
        }

        free(nest);

        if (binary == false)
            spawn(cfg->editor,path,NULL,F_NORMAL|F_WAIT);

        close(fd);
    }
}

void file_rm(const int fd, const char *name)
{
    struct stat sfile;
    fstatat(fd,name,&sfile,0);

    if ((sfile.st_mode& S_IFMT) == S_IFDIR)
    {
        int temp;
        if ((temp = openat(fd,name,O_DIRECTORY)) != -1)
        {
            DIR *d = fdopendir(temp);
            struct dirent *dir;
            if (d)
            {
                while ((dir = readdir(d)))
                    if (!(dir->d_name[0] == '.' && (dir->d_name[1] == '\0' || (dir->d_name[1] == '.' && dir->d_name[2] == '\0'))))
                        file_rm(temp,dir->d_name);
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

void file_cp(const int fd1, const int fd2, const char *name, char *buffer, const mode_t arg)
{
    struct stat sfile;
    int fd3, fd4;
    fstatat(fd2,name,&sfile,0);

    char *temp = (char*)malloc(NAME_MAX);
    strcpy(temp,name);
    ull num = 0;

    if (!(arg&M_MERGE && (sfile.st_mode&S_IFMT) == S_IFDIR))
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
            file_rm(fd1,temp);
    }

    if ((sfile.st_mode& S_IFMT) == S_IFDIR)
    {
        if ((fd3 = openat(fd2,name,O_DIRECTORY)) != -1)
        {
            DIR *d = fdopendir(fd3);
            struct dirent *dir;

            if (d)
            {
                mkdirat(fd1,temp,sfile.st_mode);
                if ((fd4 = openat(fd1,temp,O_DIRECTORY)) != -1)
                {
                    while ((dir = readdir(d)))
                        if (!(dir->d_name[0] == '.' && (dir->d_name[1] == '\0' || (dir->d_name[1] == '.' && dir->d_name[2] == '\0'))))
                            file_cp(fd4,fd3,dir->d_name,buffer,arg);
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
            if ((fd4 = openat(fd1,temp,O_WRONLY|O_CREAT,sfile.st_mode)) != -1)
            {
                int bytesread;
                while ((bytesread = read(fd3,buffer,cfg->CopyBufferSize)) > 0)
                    write(fd4,buffer,bytesread);

                close(fd4);
            }

            close(fd3);
        }
    }

    free(temp);
}

void file_mv(const int fd1, const int fd2, const char *name, char *buffer, const mode_t arg)
{
    struct stat sfile;
    int fd3, fd4;
    fstatat(fd2,name,&sfile,0);

    char *temp = (char*)malloc(NAME_MAX);
    strcpy(temp,name);
    ull num = 0;

    if (!(arg&M_MERGE && (sfile.st_mode&S_IFMT) == S_IFDIR))
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
            file_rm(fd1,temp);
    }

    if ((sfile.st_mode& S_IFMT) == S_IFDIR)
    {
        if ((fd3 = openat(fd2,name,O_DIRECTORY)) != -1)
        {
            DIR *d = fdopendir(fd3);
            struct dirent *dir;

            if (d)
            {
                mkdirat(fd1,temp,sfile.st_mode);
                if ((fd4 = openat(fd1,temp,O_DIRECTORY)) != -1)
                {
                    while ((dir = readdir(d)))
                        if (!(dir->d_name[0] == '.' && (dir->d_name[1] == '\0' || (dir->d_name[1] == '.' && dir->d_name[2] == '\0'))))
                            file_mv(fd4,fd3,dir->d_name,buffer,arg);
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
            if ((fd4 = openat(fd1,temp,O_WRONLY|O_CREAT,sfile.st_mode)) != -1)
            {
                int bytesread;
                while ((bytesread = read(fd3,buffer,cfg->CopyBufferSize)) > 0)
                    write(fd4,buffer,bytesread);

                close(fd4);
                unlinkat(fd2,name,0);
            }

            close(fd3);
        }
    }

    free(temp);
}

size_t ttoa(const time_t *time, char *result)
{
    struct tm *tis = localtime(time);
    return sprintf(result,"%d-%02d-%.2d %02d:%02d ",tis->tm_year+1900,tis->tm_mon+1,tis->tm_mday,tis->tm_hour,tis->tm_min);
}

void path_shrink(char *path, const int max_size)
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

char *mkpath(const char *dir, const char *name)
{
    static char path[PATH_MAX];
    strcpy(path,dir);
    size_t dir_t = strlen(dir);
    if (path[0] == '/' && path[1] != '\0')
        path[dir_t++] = '/';
    strcpy(path+dir_t,name);
    return path;
}

size_t findfirst(const char *src, int (*func)(int))
{
    size_t pos = 0;
    while (func(src[pos])) pos++;
    return pos;
}

extern struct AliasesT aliases[];

size_t atov(void *dest, const char *src)
{
    size_t PosBegin = 0, PosEnd = 0;
    char temp[8192];
    if (src[PosBegin] == '{')
    {
        for (int i = 0; src[PosBegin] != '}'; i++)
        {
            PosBegin++;
            PosEnd = 0;
            PosBegin += findfirst(src+PosBegin,isspace);
            while (src[PosBegin+PosEnd] && !isspace(src[PosBegin+PosEnd]) && src[PosBegin+PosEnd] != ',' && src[PosBegin+PosEnd] != '}')
                PosEnd++;

            strncpy(temp,src+PosBegin,PosEnd);
            temp[PosEnd] = '\0';

            PosBegin += PosEnd;
            PosBegin += findfirst(src+PosBegin,isspace);

            atov(&(*(li**)dest)[i],temp);
        }
        PosBegin++;
    }
    else if (src[PosBegin] == '\'')
    {
        PosBegin++;
        PosEnd = find_endof(src+PosBegin,'\'');
        strncpy(*(char**)dest,src+PosBegin,PosEnd);
        (*(char**)dest)[PosEnd] = '\0';
        PosBegin += PosEnd+2;
    }
    else if (src[PosBegin] == '"')
    {
        PosBegin++;
        PosEnd = find_endof(src+PosBegin,'"');
        for (size_t i = PosBegin, x = 0; i <= PosEnd; i++, x++)
        {
            if (src[i] == '\\' && src[i+1])
            {
                i++;
                (*(char**)dest)[x] = charconv(src[i]);
                continue;
            }

            if (src[i] == '$' && src[i+1] == '{')
            {
                i += 2;
                size_t end = find_endof(src+i,'}');
                strncpy(temp,src+i,end);
                temp[end] = 0;
                char *temp2 = getenv(temp);
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
                PosBegin += findfirst(src+PosBegin,isspace);
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

wchar_t charconv(const char c)
{
    register char ret = c;
    switch (c)
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

    return btowc(ret);
}

wchar_t *atok(char *src, wchar_t *dest)
{
    for (size_t i = 0, h = 0; src[i]; i++)
    {
        if (src[i] == '<' && src[i+1] == 'C' && src[i+2] == '-' && src[i+3] && src[i+4] == '>')
        {
            for (int g = 0; g < 4; g++)
                for (int j = i+(g == 3); src[j]; j++)
                    src[j] = src[j+1];
            dest[h++] = src[i]&0x1f;
        }
        else if (strncasecmp(src+i,"<space>",7) == 0)
        {
            for (int g = 0; g < 6; g++)
                for (int j = i; src[j]; j++)
                    src[j] = src[j+1];
            dest[h++] = btowc(' ');
        }
        else if (strncasecmp(src+i,"<esc>",5) == 0)
        {
            for (int g = 0; g < 4; g++)
                for (int j = i; src[j]; j++)
                    src[j] = src[j+1];
            dest[h++] = KEY_EXIT;
        }
        else if (strncasecmp(src+i,"<left>",6) == 0)
        {
            for (int g = 0; g < 5; g++)
                for (int j = i; src[j]; j++)
                    src[j] = src[j+1];
            dest[h++] = KEY_LEFT;
        }
        else if (strncasecmp(src+i,"<right>",7) == 0)
        {
            for (int g = 0; g < 6; g++)
                for (int j = i; src[j]; j++)
                    src[j] = src[j+1];
            dest[h++] = KEY_RIGHT;
        }
        else if (strncasecmp(src+i,"<up>",4) == 0)
        {
            for (int g = 0; g < 3; g++)
                for (int j = i; src[j]; j++)
                    src[j] = src[j+1];
            dest[h++] = KEY_UP;
        }
        else if (strncasecmp(src+i,"<down>",6) == 0)
        {
            for (int g = 0; g < 5; g++)
                for (int j = i; src[j]; j++)
                    src[j] = src[j+1];
            dest[h++] = KEY_DOWN;
        }
        else if (strncasecmp(src+i,"<br>",4) == 0)
        {
            for (int g = 0; g < 3; g++)
                for (int j = i; src[j]; j++)
                    src[j] = src[j+1];
            dest[h++] = KEY_ENTER;
        }
        else if (src[i] == '\\' && src[i+1])
        {
            size_t j;
            for (j = i; j < strlen(src)-1; j++)
                src[j] = src[j+1];
            src[j+1] = '\0';
            dest[h++] = charconv(src[i]);
        }
        else
            dest[h++] = btowc(src[i]);
    }
    return dest;
}

size_t atop(char *dest, const char *src)
{
    size_t pos = 0, x;
    if (src[pos] == '"' || src[pos] == '\'')
    {
        pos = atov(&dest,src);
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

size_t find_endof(const char *src, const char res)
{
    size_t end = 0;
    while (src[end++])
    {
        if (src[end] == res && src[end-1] != '\\')
            break;
    }
    return end;
}

char *atob(char *temp)
{
    for (size_t i = 0; i < strlen(temp); i++)
    {
        if(temp[i] == '\\' || temp[i] == '\"' || temp[i] == '\'' || temp[i] == ' ' || temp[i] == '(' || temp[i] == ')' || temp[i] == '[' || temp[i] == ']' || temp[i] == '{' || temp[i] == '}')
        {
            for (size_t j = strlen(temp)+1; j > i; j--)
                temp[j] = temp[j-1];
            temp[i] = '\\';
            i++;
        }
    }
    return temp;
}
