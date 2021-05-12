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
#include "loading.h"

#ifdef __COLOR_FILES_BY_EXTENSION__
extern Extensions extensions[];
#endif

extern FileSignatures signatures[];

static struct sigaction oldsighup;
static struct sigaction oldsigtstp;

extern li s_CopyBufferSize;
extern char *s_FileOpener;
extern char *s_Values;
extern char *s_editor;

void die(const char *p, ...)
{
    va_list args;
    va_start(args,p);
    vfprintf(stderr,p,args);
    va_end(args);
    fputc('\n',stderr);
    exit(-1);
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

    	sigaction(SIGHUP, &oldsighup, NULL);
    	sigaction(SIGTSTP, &oldsigtstp, NULL);

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
        ret[i++] = s_Values[too-1];

    ret[i] = '\0';

    return ret;
}

int get_dirsize(const int fd, ull *count, ull *size, const uchar flag)
{
    DIR *d = fdopendir(fd);
    if (d == NULL)
        return -1;
    struct dirent *dir;

    int tfd;
    struct stat st;

    while ((dir = readdir(d)))
    {
        if (dir->d_name[0] == '.' && (dir->d_name[1] == '\0' || (dir->d_name[1] == '.' && dir->d_name[2] == '\0')))
            continue;
        if (flag&D_R && dir->d_type == 4 &&
            faccessat(fd,dir->d_name,R_OK,0) == 0 && (tfd = openat(fd,dir->d_name,O_DIRECTORY)) != -1)
        {
            get_dirsize(tfd,count,size,flag);
            close(tfd);
        }
        else
        {
            if (flag&D_C)
                (*count)++;
            if (flag&D_S)
            {
                fstatat(fd,dir->d_name,&st,AT_SYMLINK_NOFOLLOW);
                if ((st.st_mode&S_IFMT) == S_IFREG)
                    *size += st.st_size;
            }
        }
    }

    closedir(d);

    return 0;
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
        sigaction(SIGHUP,&(struct sigaction){.sa_handler=SIG_IGN},&oldsighup);
		sigaction(SIGTSTP,&act,&oldsigtstp);
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
    if (strcmp(s_FileOpener,"NULL") != 0)
        spawn(s_FileOpener,path,NULL,F_NORMAL|F_WAIT);
    else
    {
        struct stat sfile;
        if (stat(path,&sfile) == -1)
            return;
        if (!(sfile.st_mode&S_IRUSR))
            return;

        if (sfile.st_size == 0)
        {
            spawn(s_editor,path,NULL,F_NORMAL|F_WAIT);
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

        size_t bina = 0;
        bool binary = false;

        for (register size_t i = 0; i < buf_t; i++)
            if (!((buf[i] >= 0x07 && buf[i] <= 0xd) || (buf[i] >= 0x20 && buf[i] <= 0x7e)))
                bina++;

        binary = bina>>1;

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
            spawn(s_editor,path,NULL,F_NORMAL|F_WAIT);

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
                while ((bytesread = read(fd3,buffer,s_CopyBufferSize)) > 0)
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
                while ((bytesread = read(fd3,buffer,s_CopyBufferSize)) > 0)
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

char *atov(void *dest, const char *src, size_t *size, Csas *cs, const uchar flag)
{
    static char *messages[] = {
        "wasn't expecting an array","wasn't expecting a string",
        "wasn't expecting a integer","wasn't expecting a float"
    };
    size_t PosBegin = 0, PosEnd = 0;
    char temp[8192];
    if (src[PosBegin] == '{')
    {
        if (!(flag&SET_T_A))
            return messages[0];
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

            char *r = atov(&(*(li**)dest)[i],temp,NULL,cs,flag&(~SET_T_A));
            if (r != NULL)
                return r;
        }
        PosBegin++;
    }
    else if (src[PosBegin] == '\'')
    {
        if (!(flag&SET_T_P))
            return messages[1];
        PosBegin++;
        PosEnd = strchr(src+PosBegin,'\'')-(src+PosBegin);
        strncpy(*(char**)dest,src+PosBegin,PosEnd);
        (*(char**)dest)[PosEnd] = '\0';
        PosBegin += PosEnd+2;
    }
    else if (src[PosBegin] == '"')
    {
        if (!(flag&SET_T_P))
            return messages[1];
        PosBegin++;
        PosEnd = strchr(src+PosBegin,'"')-(src+PosBegin)+1;
        size_t i = PosBegin, x = 0;
        while (i < PosEnd)
        {
            if (src[i] == '$')
            {
                get_special(*(char**)dest,src,&i,&x,cs);
                continue;
            }
            (*(char**)dest)[x++] = src[i++];
        }
        (*(char**)dest)[x] = '\0';
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

            if (type == 0)
            {
                if (!(flag&SET_T_UI) && !(flag&SET_T_I))
                    return messages[2];
                *(li*)dest |= atol(temp);
            }
            else if (type == 1)
            {
                if (!(flag&SET_T_F))
                    return messages[3];
                *(double*)dest = atof(temp);
                PosBegin += PosEnd;
                PosBegin += findfirst(src+PosBegin,isspace);
                goto END;
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

    END: ;
    if (size != NULL)
        *size += PosBegin;
    return NULL;
}

wchar_t charconv(const char c)
{
    register char ret = c&127;
    static char chararray[128] = {
        0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,
        26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,0,
        49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,
        72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,
        95,96,7,8,99,100,101,12,103,104,105,106,107,108,109,10,111,112,113,13,
        115,9,117,11,119,120,121,122,123,124,125,126,127
    };

    return btowc(chararray[(int)ret]);
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

size_t atop(char *dest, const char *src, Csas *cs)
{
    size_t pos = 0, x;
    src += findfirst(src,isspace);
    if (src[pos] == '"' || src[pos] == '\'')
    {
        atov(&dest,src,&pos,cs,SET_T_P);
    }
    else
    {
        x = 0;
        while (src[pos] && !isspace(src[pos]))
        {
            if (src[pos] == '\\')
            {
                dest[x++] = src[++pos];
                pos++;
                continue;
            }

            if (src[pos] == '$')
            {
                get_special(dest,src,&pos,&x,cs);
                continue;
            }

            dest[x++] = src[pos++];
        }
        dest[x] = '\0';
    }

    return pos;
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
