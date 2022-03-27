/*
    csas - console file manager
    Copyright (C) 2020-2022 TUVIMEN <suchora.dominik7@gmail.com>

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
#include "expand.h"

static int
expand_files(char *line, size_t pos, size_t *size, uchar *tabp, flexarr *arg, uchar *free_names, const uchar only_dir, csas *cs)
{
    char path[PATH_MAX],name[NAME_MAX],*r,*cline;
    size_t linel,pathl;
    do {
        cline = line+pos;
        linel = strlen(cline);
        r = get_path(path,cline,' ',linel,PATH_MAX,cs);
        if (r == NULL)
            return -1;
        linel = r-cline+1;
        pos += linel+1;
    } while (cline[linel] != 0);

    if (!(*tabp)) {
        if (arg->size) {
            if (*free_names) {
                for (size_t i = 0; i < arg->size; i++)
                    free(((char**)arg->v)[i]);
                *free_names = 0;
            }
            arg->size = 0;
        }

        pathl = strlen(path);
        if (path[pathl-1] == '/') {
            name[0] = 0;
        } else {
            r = memrchr(path,'/',pathl);
            if (r) {
                *(r++) = 0;
                strcpy(name,r);
            } else {
                strcpy(name,path);
                path[0] = 0;
            }
            if (path[0] == 0) {
                path[0] = '.';
                path[1] = '/';
                path[2] = 0;
            }
        }
        
        DIR *d = opendir(path);
        if (d == NULL)
            return -1;
        size_t best_match_n=0,matched_n;
        struct dirent *dir;
        while ((dir = readdir(d))) {
            if (dir->d_name[0] == '.' && (dir->d_name[1] == '\0' || (dir->d_name[1] == '.' && dir->d_name[2] == '\0')))
                continue;
            if (only_dir && dir->d_type != DT_DIR)
                continue;
        
            matched_n = 0;
            for (register size_t j = 0; dir->d_name[j] && name[j] && dir->d_name[j] == name[j]; j++)
                matched_n++;
        
            if (matched_n > best_match_n) {
                if (*free_names) {
                    for (size_t j = 0; j < arg->size; j++)
                        free(((char**)arg->v)[j]);
                    *free_names = 0;
                }
                arg->size = 0;
                best_match_n = matched_n;
                goto ADD_TO_ARR;
            }
            
            if (matched_n == best_match_n) {
                ADD_TO_ARR: ;
                *free_names = 1;
                *(char**)flexarr_inc(arg) = strdup(dir->d_name);
            }
        }
        closedir(d);
        arg->asize = 0;
        *tabp = 1;
    }

    if (arg->size) {
        char *n = ((char**)arg->v)[arg->asize++];
        size_t s = strlen(n);
        while (linel && cline[linel] != '/')
            linel--;
        if (cline[linel] == '/')
            linel++;
        *size = pos+s+linel;
        memcpy(cline+linel,n,s+1);
        if (arg->asize == arg->size)
            arg->asize = 0;
        if (arg->size == 1) {
            struct stat statbuf;
            stat(cline,&statbuf);
            if ((statbuf.st_mode&S_IFMT) == S_IFDIR)
                line[(*size)++] = '/';
            else
                line[(*size)++] = ' ';
            line[*size] = 0;
            *tabp = 0;
        }
    }

    return 0;
}

int
expand_file(char *line, size_t pos, size_t *size, uchar *tabp, flexarr *arg, uchar *free_names, csas *cs)
    {return expand_files(line,pos,size,tabp,arg,free_names,0,cs);}
int
expand_dir(char *line, size_t pos, size_t *size, uchar *tabp, flexarr *arg, uchar *free_names, csas *cs)
    {return expand_files(line,pos,size,tabp,arg,free_names,1,cs);}

int
expand_shell_commands(char *line, size_t pos, size_t *size, uchar *tabp, flexarr *arg, uchar *free_names, csas *cs)
{
    char *cline = line+pos;
    size_t end=0;
    while (!isspace(cline[end]) && end+pos < *size)
        end++;

    if (cline[end] != 0)
        return -1;

    if (!(*tabp)) {
        if (arg->size) {
            if (*free_names) {
                for (size_t i = 0; i < arg->size; i++)
                    free(((char**)arg->v)[i]);
                *free_names = 0;
            }
            arg->size = 0;
        }
        size_t best_match_n = 0, matched_n;
        char *token,*str,*saveptr,pv[PATH_MAX];
        strcpy(pv,getenv("PATH"));
        for (str = pv; ; str = NULL) {
            token = strtok_r(str,":",&saveptr);
            if (token == NULL)
                break;
            DIR *d = opendir(token);
            if (d == NULL)
                continue;
            struct dirent *dir;
            while ((dir = readdir(d))) {
                if (dir->d_type == DT_DIR)
                    continue;
                
                matched_n = 0;
                for (int j = 0; dir->d_name[j] && cline[j] && dir->d_name[j] == cline[j]; j++)
                    matched_n++;
                
                if (matched_n > best_match_n) {
                    if (*free_names) {
                        for (size_t j = 0; j < arg->size; j++)
                            free(((char**)arg->v)[j]);
                        *free_names = 0;
                    }
                    arg->size = 0;
                    best_match_n = matched_n;
                    goto ADD_TO_ARR;
                }
                
                if (matched_n == best_match_n) {
                    ADD_TO_ARR: ;
                    *free_names = 1;
                    *(char**)flexarr_inc(arg) = strdup(dir->d_name);
                }
            }
            closedir(d);
        }
        arg->asize = 0;
        *tabp = 1;
    }

    if (arg->size) {
        char *n = ((char**)arg->v)[arg->asize++];
        size_t s = strlen(n);
        *size = pos+s;
        memcpy(cline,n,s+1);
        if (arg->asize == arg->size)
            arg->asize = 0;
        if (arg->size == 1) {
            line[(*size)++] = ' ';
            line[*size] = 0;
            *tabp = 0;
        }
    }
    return 0;
}

int
expand_shell(char *line, size_t pos, size_t *size, uchar *tabp, flexarr *arg, uchar *free_names, csas *cs)
{
    int ret = expand_shell_commands(line,pos,size,tabp,arg,free_names,cs);
    if (ret == -1) {
        while (!isspace(line[pos]) && pos < *size)
            pos++;
        while (isspace(line[pos]) && pos < *size)
            pos++;
        ret = expand_file(line,pos,size,tabp,arg,free_names,cs);
    }
    return ret;
}

int
expand_vars(char *line, size_t pos, size_t *size, uchar *tabp, flexarr *arg, uchar *free_names, csas *cs)
{
    char *cline = line+pos;
    size_t end=0;
    flexarr *vars = cs->vars;
    xvar *v = (xvar*)vars->v;
    while (!isspace(cline[end]) && end+pos < *size)
        end++;

    if (cline[end] != 0)
        return -1;
    if (!(*tabp)) {
        if (arg->size) {
            if (*free_names) {
                for (size_t i = 0; i < arg->size; i++)
                    free(((char**)arg->v)[i]);
                *free_names = 0;
            }
            arg->size = 0;
        }
        size_t best_match_n = 0, matched_n;
        for (size_t i = 0; i < vars->size; i++) {
            matched_n = 0;
            for (int j = 0; v[i].name[j] && cline[j] && v[i].name[j] == cline[j]; j++)
                matched_n++;
            if (matched_n < best_match_n)
                continue;

            if (matched_n > best_match_n) {
                arg->size = 0;
                best_match_n = matched_n;
            }
            *(size_t*)flexarr_inc(arg) = i;
        }
        arg->asize = 0;
        *tabp = 1;
    }

    if (arg->size) {
        char *n = v[((size_t*)arg->v)[arg->asize++]].name;
        size_t s = strlen(n);
        *size = pos+s;
        memcpy(cline,n,s+1);
        if (arg->asize == arg->size)
            arg->asize = 0;
        if (arg->size == 1) {
            line[(*size)++] = ' ';
            line[*size] = 0;
            *tabp = 0;
        }
    }
    return 0;
}

int
expand_commands(char *line, size_t pos, size_t *size, uchar *tabp, flexarr *arg, uchar *free_names, csas *cs)
{
    char *cline = line+pos;
    size_t end=0;
    flexarr *functions = cs->functions;
    xfunc *f = (xfunc*)functions->v;
    while (!isspace(cline[end]) && end+pos < *size)
        end++;

    if (cline[end] == 0) {
        if (!(*tabp)) {
            if (arg->size) {
                if (*free_names) {
                    for (size_t i = 0; i < arg->size; i++)
                        free(((char**)arg->v)[i]);
                    *free_names = 0;
                }
                arg->size = 0;
            }
            size_t best_match_n = 0, matched_n;
            for (size_t i = 0; i < functions->size; i++) {
                matched_n = 0;
                for (int j = 0; f[i].name[j] && cline[j] && f[i].name[j] == cline[j]; j++)
                    matched_n++;

                if (matched_n > best_match_n) {
                    arg->size = 0;
                    best_match_n = matched_n;
                    goto ADD_TO_ARR;
                }

                if (matched_n == best_match_n) {
                    ADD_TO_ARR: ;
                    *(size_t*)flexarr_inc(arg) = i;
                }
            }
            arg->asize = 0;
            *tabp = 1;
        }

        if (arg->size) {
            char *n = f[((size_t*)arg->v)[arg->asize++]].name;
            size_t s = strlen(n);
            *size = pos+s;
            memcpy(cline,n,s+1);
            if (arg->asize == arg->size)
                arg->asize = 0;
            if (arg->size == 1) {
                line[(*size)++] = ' ';
                line[*size] = 0;
                *tabp = 0;
            }
        }
    } else {
        char *name = cline;
        size_t namel = end;
        X1: ;
        li found = -1;
        for (size_t i = 0; i < functions->size; i++) {
            if (namel == strlen(f[i].name) && memcmp(name,f[i].name,namel) == 0) {
                found = (li)i;
                break;
            }
        }
        if (found == -1)
            return -1;
        if (f[found].type == 'a') {
            if (!f[found].func)
                return -1;
            name = f[found].func;
            namel = 0;
            while (!isspace(name[namel]) && name[namel])
                name++;
            goto X1;
        }
        if (f[found].expand == NULL)
            return 0;
        while (isspace(cline[end]))
            end++;
        f[found].expand(line,end,size,tabp,arg,free_names,cs);
    }
    return 0;
}
