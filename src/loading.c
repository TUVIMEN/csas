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
#include "loading.h"
#include "inits.h"
#include "console.h"
#include "useful.h"

#ifdef __LOAD_CONFIG_ENABLE__
extern struct AliasesT aliases[];
extern Settings* settings;

void GetFullLine(char* dest, const char* src, size_t* n)
{
    size_t x = 0;

    while (src[*n])
    {
        if (src[*n] == '\n' || src[*n] == ';')
        {
            if (x == 0)
                goto END;
            else if (src[*n-1] != '\\')
                goto END;
            else
                goto RES;

            END: ;
                dest[x] = '\0';
                break;
            RES: ;
                dest[x-1] = ' ';
                (*n)++;
        }

        if (src[*n] == '/')
        {
            if (src[*n+1] == '/')
            {
                while (src[(*n)++])
                    if (src[*n] == '\n') break;
            }
            else if (src[*n+1] == '*')
            {
                while (src[(*n)++])
                    if (src[*n] == '*' && src[*n+1] == '/') break;
                *n += 2;
            }
        }

        if ((src[*n] == '\'' || src[*n] == '"') && src[*n-1] != '\\')
        {
            char temp = src[*n];
            dest[x++] = src[(*n)++];
            temp = FindEndOf(src+*n,temp == '"' ? '"' : '\'');
            strncpy(dest+x,src+*n,temp);
            dest[x+temp] = '\0';
            (*n) += temp;
            x += temp;
        }

        dest[x++] = src[(*n)++];
    }
}

void LoadConfig(const char* path, Basic* grf)
{
    int fd;
    if ((fd = open(path,O_RDONLY)) == -1)
        return;
    
    char temp[PATH_MAX];
    strcpy(temp,path);

    if (temp[0] == '/')
    {
        size_t end = strlen(temp)-1;
        for (; temp[end] != '/'; end--) temp[end] = 0;
    }

    chdir(temp);

    struct stat sfile;
    if (fstat(fd,&sfile) == -1)
    {
        close(fd);
        return;
    }

    char* file = mmap(NULL,sfile.st_size,PROT_READ,MAP_PRIVATE,fd,0);
    close(fd);

    char line[16384];

    size_t pos = 0;

    while (file[pos])
    {
        GetFullLine(line,file,&pos);
        RunCommand(line,grf);
        pos++;
    }

    munmap(file,sfile.st_size);

    chdir(grf->workspaces[grf->current_workspace].path);
}

#endif
