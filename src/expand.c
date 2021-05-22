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
#include "expand.h"
#include "useful.h"

extern Key *keys;
extern size_t keys_t;
extern struct option cfg_names[];
extern struct command *commands;
extern size_t commandsl;

static void rla_n_free(struct rla *s)
{
    for (register size_t i = 0; i < s->s; i++)
        free(s->a[i].p);
    free(s->a);
    s->a = NULL;
    s->s = 0;
    s->as = 0;
}

static void rla_s_free(struct rla *s)
{
    free(s->a);
    s->a = NULL;
    s->s = 0;
    s->as = 0;
}

static int expand_dir_file(WINDOW *win, char *line, size_t pos, short int off, bool *tab_was_pressed, struct rla *arr, struct WinArgs *args, bool only_dir, char **end)
{
    if (arr->sfree == NULL)
        arr->sfree = rla_n_free;
    char path[PATH_MAX],name[NAME_MAX], *cline = line+pos+findfirst(line+pos,isspace,-1);
    size_t pathl=0,linel;
    if (get_word(path,cline,PATH_MAX,&pathl,&linel) == -1)
        return -3;
    if (cline[linel] != 0)
        return -2;

    bool isfile = 0;
    if (pathl && !only_dir)
    {
        bool ggg = 0;
        if (path[pathl-1] == '/')
        {
            ggg = 1;
            path[--pathl] = 0;
        }
        struct stat st;
        if (stat(path,&st) != -1)
        {
            if ((st.st_mode&S_IFMT) != S_IFDIR)
                isfile = 1;
        }
        if (ggg)
            path[pathl++] = '/';
    }

    if (!(*tab_was_pressed) && !isfile)
    {
        if (arr->s)
        {
            arr->sfree(arr);
            arr->sfree = rla_n_free;
        }

        if (access(path,F_OK) == 0 && (path[0] != '.' || path[1] != '\0'))
        {
            name[0] = 0;
            if (path[pathl-1] != '/')
            {
                cline[linel++] = '/';
                cline[linel] = 0;
                args->x++;
                while (args->x-off >= win->_maxx) off++;
                *tab_was_pressed = 0;
                return 0;
            }
        }
        else
        {
            char *n = NULL;
            do {
                if (path[pathl-1] == '/')
                    path[--pathl] = 0;
                n = memrchr(path,'/',pathl);
                if (n)
                {
                    if (n == path)
                    {
                        n++;
                        strcpy(name,n);
                        path[0] = '/';
                        path[1] = '\0';
                        pathl = 1;
                    }
                    else
                    {
                        pathl = n-path;
                        *n = 0;
                        n++;
                        strcpy(name,n);
                    }
                }
                else
                {
                    strcpy(name,path);
                    path[0] = '.';
                    path[1] = '\0';
                    pathl = 1;
                    break;
                }
            } while (pathl && access(path,F_OK) == -1);
            refresh();
        }
        DIR *d = opendir(path);
        if (d == NULL)
            return -1;
        size_t best_match_n=0,matched_n;
        struct dirent *dir;
        while ((dir = readdir(d)))
        {
            if (dir->d_name[0] == '.' && (dir->d_name[1] == '\0' || (dir->d_name[1] == '.' && dir->d_name[2] == '\0')))
                continue;
            if (only_dir && dir->d_type != DT_DIR)
                continue;

            matched_n = 0;
            for (register size_t j = 0; dir->d_name[j] && name[j] && dir->d_name[j] == name[j]; j++)
                matched_n++;
            if (matched_n > best_match_n)
            {
                arr->sfree(arr);
                best_match_n = matched_n;
                goto ADD_TO_ARR;
            }
            if (matched_n == best_match_n)
            {
                ADD_TO_ARR: ;
                if (arr->as == arr->s)
                    arr->a = realloc(arr->a,(arr->as+=DIR_INC_RATE)*sizeof(char*));
                arr->a[arr->s++].p = strdup(dir->d_name);
            }
        }
        closedir(d);
        arr->as = 0;
        *tab_was_pressed = 1;
    }

    if (arr->s && *tab_was_pressed)
    {
        size_t i=0,j=0;
        char *n = memrchr(cline,'/',strlen(cline));
        size_t ggg = 0;
        if (n)
            ggg = n-cline+1;
        i=0;
        j=0;
        for (; arr->a[arr->as].p[i]; i++)
        {
            if (isspace(arr->a[arr->as].p[i]))
                (cline+ggg)[j++] = '\\';
            (cline+ggg)[j++] = arr->a[arr->as].p[i];
        }
        (cline+ggg)[j] = 0;
        arr->as++;
        if (arr->as == arr->s)
            arr->as = 0;
        args->x = strlen(line);

        while (args->x-off >= win->_maxx) off++;
        if (arr->s == 1)
            *tab_was_pressed = 0;
    }
    return 0;
}

int expand_file(WINDOW *win, char *line, size_t pos, short int off, bool *tab_was_pressed, struct rla *arr, struct WinArgs *args, char **end)
{
    return expand_dir_file(win,line,pos,off,tab_was_pressed,arr,args,0,end);
}

int expand_dir(WINDOW *win, char *line, size_t pos, short int off, bool *tab_was_pressed, struct rla *arr, struct WinArgs *args, char **end)
{
    return expand_dir_file(win,line,pos,off,tab_was_pressed,arr,args,1,end);
}

int expand_shell_commands(WINDOW *win, char *line, size_t pos, short int off, bool *tab_was_pressed, struct rla *arr, struct WinArgs *args, char **end)
{
    if (arr->sfree == NULL)
        arr->sfree = rla_n_free;
    char name[NAME_MAX], *cline = line+pos+findfirst(line+pos,isspace,-1);
    size_t namel,linel;
    if (get_word(name,cline,NAME_MAX,&namel,&linel) == -1)
        return -3;
    size_t i,j;
    if (cline[linel] != 0)
    {
        if (*end != NULL)
            *end = cline+linel;
        return -2;
    }
    if (!(*tab_was_pressed))
    {
        if (arr->s)
        {
            arr->sfree(arr);
            arr->sfree = rla_n_free;
        }

        size_t best_match_n=0,matched_n;
        char *token,*str,*saveptr,pv[PATH_MAX];
        strcpy(pv,getenv("PATH"));
        for (i = 0, str = pv; ; i++, str = NULL)
        {
            token = strtok_r(str,":",&saveptr);
            if (token == NULL)
                break;
            DIR *d = opendir(token);
            if (d == NULL)
                continue;
            struct dirent *dir;
            while ((dir = readdir(d)))
            {
                if (dir->d_type == DT_DIR)
                    continue;
                matched_n = 0;
                for (register size_t j = 0; dir->d_name[j] && name[j] && dir->d_name[j] == name[j]; j++)
                    matched_n++;
                if (matched_n > best_match_n)
                {
                    arr->sfree(arr);
                    best_match_n = matched_n;
                    goto ADD_TO_ARR;
                }
                if (matched_n == best_match_n)
                {
                    ADD_TO_ARR: ;
                    if (arr->as == arr->s)
                        arr->a = realloc(arr->a,(arr->as+=DIR_INC_RATE)*sizeof(char*));
                    arr->a[arr->s++].p = strdup(dir->d_name);
                }
            }
            closedir(d);
        }
        arr->as = 0;
        *tab_was_pressed = 1;
    }

    if (arr->s)
    {
        i=0;
        j=0;
        for (; arr->a[arr->as].p[i]; i++)
        {
            if (isspace(arr->a[arr->as].p[i]))
                cline[j++] = '\\';
            cline[j++] = arr->a[arr->as].p[i];
        }
        cline[j] = 0;
        arr->as++;
        if (arr->as == arr->s)
            arr->as = 0;
        args->x = strlen(line);
        if (arr->s == 1)
        {
            line[args->x++] = ' ';
            line[args->x] = 0;
            *tab_was_pressed = 0;
        }

        while (args->x-off >= win->_maxx) off++;
    }
    return 0;
}

int expand_shell(WINDOW *win, char *line, size_t pos, short int off, bool *tab_was_pressed, struct rla *arr, struct WinArgs *args, char **end)
{
    char *e;
    int ret = expand_shell_commands(win,line,pos,off,tab_was_pressed,arr,args,&e);
    if (ret == -2)
        ret = expand_file(win,line,e-line,off,tab_was_pressed,arr,args,NULL);
    return ret;
}

int expand_options(WINDOW *win, char *line, size_t pos, short int off, bool *tab_was_pressed, struct rla *arr, struct WinArgs *args, char **end)
{
    if (arr->sfree == NULL)
        arr->sfree = rla_s_free;
    char name_complete[NAME_MAX], *cline = line+pos+findfirst(line+pos,isspace,-1);
    size_t namel,linel;
    if (get_word(name_complete,cline,NAME_MAX,&namel,&linel) == -1)
        return -3;

    if (cline[linel] != 0)
        return -2;
    if (!(*tab_was_pressed))
    {
        if (arr->s)
        {
            arr->sfree(arr);
            arr->sfree = rla_s_free;
        }
        size_t best_match_n = 0, matched_n;
        for (size_t i = 0; cfg_names[i].n; i++)
        {
            matched_n = 0;
            for (int j = 0; cfg_names[i].n[j] && name_complete[j] && cfg_names[i].n[j] == name_complete[j]; j++)
                matched_n++;

            if (matched_n > best_match_n)
            {
                arr->s = 0;
                free(arr->a);
                arr->a = NULL;
                best_match_n = matched_n;
                goto ADD_TO_ARR;
            }

            if (matched_n == best_match_n)
            {
                ADD_TO_ARR: ;
                arr->a = realloc(arr->a,++arr->s*sizeof(size_t));
                arr->a[arr->s-1].s = i;
            }
        }
        arr->as = 0;
        *tab_was_pressed = 1;
    }

    if (arr->s)
    {
        strcpy(cline,cfg_names[arr->a[arr->as++].s].n);
        if (arr->as == arr->s)
            arr->as = 0;
        args->x = strlen(line);
        if (arr->s == 1)
        {
            line[args->x++] = ' ';
            line[args->x] = 0;
            *tab_was_pressed = 0;
        }

        while (args->x-off >= win->_maxx) off++;
    }
    return 0;
}

int expand_commands(WINDOW *win, char *line, size_t pos, short int off, bool *tab_was_pressed, struct rla *arr, struct WinArgs *args, char **end)
{
    if (arr->sfree == NULL)
        arr->sfree = rla_s_free;
    char name_complete[NAME_MAX], *cline = line+pos+findfirst(line+pos,isspace,-1);
    size_t namel,linel;
    if (get_word(name_complete,cline,NAME_MAX,&namel,&linel) == -1)
        return -3;

    if (cline[linel] == 0)
    {
        if (!(*tab_was_pressed))
        {
            if (arr->s)
            {
                arr->sfree(arr);
                arr->sfree = rla_s_free;
            }
            size_t best_match_n = 0, matched_n;
            for (size_t i = 0; i < commandsl; i++)
            {
                matched_n = 0;
                for (int j = 0; commands[i].name[j] && name_complete[j] && commands[i].name[j] == name_complete[j]; j++)
                    matched_n++;

                if (matched_n > best_match_n)
                {
                    arr->s = 0;
                    free(arr->a);
                    arr->a = NULL;
                    best_match_n = matched_n;
                    goto ADD_TO_ARR;
                }

                if (matched_n == best_match_n)
                {
                    ADD_TO_ARR: ;
                    arr->a = realloc(arr->a,++arr->s*sizeof(size_t));
                    arr->a[arr->s-1].s = i;
                }
            }
            arr->as = 0;
            *tab_was_pressed = 1;
        }

        if (arr->s)
        {
            strcpy(cline,commands[arr->a[arr->as++].s].name);
            if (arr->as == arr->s)
                arr->as = 0;
            args->x = strlen(line);
            if (arr->s == 1)
            {
                cline[args->x++] = ' ';
                cline[args->x] = 0;
                *tab_was_pressed = 0;
            }

            while (args->x-off >= win->_maxx) off++;
        }
    }
    else
    {
        X1: ;
        li found = -1;
        for (size_t i = 0; i < commandsl; i++)
        {
            if (namel == strlen(commands[i].name) && strncmp(name_complete,commands[i].name,namel) == 0)
            {
                found = (li)i;
                break;
            }
        }
        if (found == -1)
            return -1;
        if (commands[found].type != 'f')
        {
            if (!commands[found].func)
                return -1;
            get_word(name_complete,commands[found].func+findfirst(commands[found].func,isspace,-1),NAME_MAX,&namel,NULL);
            goto X1;
        }
        if (commands[found].expand == NULL)
            return -1;
        commands[found].expand(win,line,(cline-line)+linel,off,tab_was_pressed,arr,args,NULL);
    }
    return 0;
}