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

void get_env(char *dest, const char *src, size_t *n, size_t *x)
{
    size_t end = strchr(src+*n,'}')-(src+*n);
    char temp[NAME_MAX], *temp2;
    strncpy(temp,src+*n,end);
    temp[end] = 0;
    temp2 = getenv(temp);
    if (temp2)
    {
        size_t end1 = strlen(temp2);
        memcpy(dest+*x,temp2,end1);
        *x += end1-1;
    }
    *n += end+1;
    (*x)++;
}

void get_clearline(char *dest, const char *src, size_t *n)
{
    *n += findfirst(src+(*n),isspace,-1);
    size_t x = 0;

    while (src[*n])
    {
        if (src[*n] == '\\')
        {
            (*n)++;
            if (src[*n] == '\n')
            {
                dest[x++] = ' ';
                if (src[++(*n)] == 0) {dest[x] = 0; break;}
            }
            else if (src[*n] == ' ')
            {
                dest[x++] = src[*n-1];
                dest[x++] = src[(*n)++];
            }
            else
            {
                dest[x++] = charconv(src[*n]);
                if (src[*n] == 0) break;
                (*n)++;
            }
            continue;
        }

        if (src[*n] == '\n' || src[*n] == ';')
        {
            dest[x] = '\0';
            break;
        }

        if (src[*n] == '\'')
        {
            dest[x++] = src[(*n)++];
            while (src[*n])
            {
                if (src[*n] == '\'')
                {
                    dest[x++] = src[(*n)++];
                    break;
                }
                dest[x++] = src[(*n)++];
            }
            continue;
        }

        if (src[*n] == '"')
        {
            dest[x++] = src[(*n)++];
            while (src[*n])
            {
                if (src[*n] == '\\')
                {
                    dest[x++] = charconv(src[++(*n)]);
                    if (src[*n] == 0) break;
                    (*n)++;
                    continue;
                }
                
                if (src[*n] == '"')
                {
                    dest[x++] = src[(*n)++];
                    break;
                }
                dest[x++] = src[(*n)++];
            }
            continue;
        }

        if (src[*n] == '/')
        {
            if (src[*n+1] == '/')
            {
                (*n) += 2;
                while (src[*n])
                {
                    if (src[*n] == '\\')
                    {
                        if (src[++(*n)] == 0) break;
                        if (src[++(*n)] == 0) break;
                        continue;
                    }
                    if (src[*n] == '\n') break;
                    (*n)++;
                }
                (*n)++;
            }
            else if (src[*n+1] == '*')
            {
                *n += 2;
                if (src[(*n)] != 0)
                {
                    while (src[(*n)++])
                    {
                        if (src[*n] == '*' && src[*n+1] == '/') break;
                    }
                    *n += 3;
                }
            }
            dest[x++] = src[(*n)++];
            continue;
        }

        if (src[*n] == '$' && src[*n+1] == '{')
        {
            dest[x++] = src[(*n)++];
            dest[x++] = src[*n];

            while (src[*n])
            {
                if (src[*n] == '\\')
                {
                    dest[x++] = charconv(src[++(*n)]);
                    if (src[*n] == 0) break;
                        (*n)++;
                    continue;
                }
                if (src[*n] == '}')
                {
                    dest[x++] = src[(*n)++];   
                    break;
                }
                dest[x++] = src[(*n)++];
            }
            continue;
        }

        dest[x++] = src[(*n)++];
    }
}

void config_load(const char *path, Csas *cs)
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

    char *file = mmap(NULL,sfile.st_size,PROT_READ,MAP_PRIVATE,fd,0);
    close(fd);

    char line[LINE_SIZE_MAX];
    int r;

    endwin();
    for (size_t pos = 0, i = 0; file[pos]; i++)
    {
        get_clearline(line,file,&pos);
        r = command_run(line,cs);
        if (r != 0)
            fprintf(stderr,"%s: %s\n%lu:\t%s\n",path,csas_strerror(),i,line);
        pos++;
    }
    refresh();

    munmap(file,sfile.st_size);
    chdir(cs->ws[cs->current_ws].path);
}

#endif
