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
#include "draw.h"
#include "load.h"
#include "preview.h"
#include "useful.h"
#include "inits.h"
#include "console.h"
#include "loading.h"

extern Key *keys;
extern size_t keys_t;
extern struct option cfg_names[];

extern li s_WrapScroll;
extern li s_Bar2Enable;
extern li s_JumpScroll;
extern double s_MoveOffSet;
extern li s_Borders;
extern double s_JumpScrollValue;
extern char *s_shell;
extern li s_Win1Enable;
extern li s_Win3Enable;
extern li s_CopyBufferSize;
extern li s_DirLoadingMode;

void change_workspace(Csas *cs, const int num)
{
    register int temp = cs->current_ws;
    cs->current_ws = num;

    if (!cs->ws[num].exists)
    {
        cs->ws[num].exists = 1;
        cs->ws[num].show_message = false;
        cs->ws[num].path = strcpy(malloc(PATH_MAX),G_D(temp,1)->path);
        csas_cd(G_D(temp,1)->path,num,cs);
    }
    else
        csas_cd(G_D(num,1)->path,num,cs);
}

int update_event(Csas *cs)
{
    int event = getch();
    if (event == -1) { return -1; }

    memset(cs->typed_keys,0,63);
    cs->was_typed = true;

    while (event == -1 || event == KEY_RESIZE || (event > 47 && event < 58))
    {
        if (event == KEY_RESIZE) { update_size(cs); csas_draw(cs,-1); }
        if (event > 47 && event < 58) { cs->typed_keys[strlen(cs->typed_keys)] = event; csas_draw(cs,4); }
        event = getch();
    }

    int *passed = NULL;
    size_t passedl = 0;

    if ((int)event == 27) { cs->was_typed = false; return -1; }
    cs->typed_keys[strlen(cs->typed_keys)] = event;

    for (size_t i = 0; i < keys_t; i++)
    {
        if (event == keys[i].keys[0])
        {
            passed = (int*)realloc(passed,(passedl+1)*sizeof(int));
            passedl++;
            passed[passedl-1] = i;
            #ifdef __SHOW_KEY_BINDINGS__
            for (int j = 0; j < cs->wx; j++)
                mvaddch(cs->wy-s_Bar2Enable-passedl,j,' ');
            mvprintw(cs->wy-s_Bar2Enable-passedl,0," %c\t%s",wctob(keys[i].keys[1]),keys[i].value);
            #endif
        }
    }

    bool starts_string = 0;

    for (int i = 1; passedl > 1; i++)
    {
        csas_draw(cs,-1);
        do {
            if (event == KEY_RESIZE)
            {
                update_size(cs);
                #ifdef __SHOW_KEY_BINDINGS__
                erase();
                refresh();
                #endif
                csas_draw(cs,-1);
                #ifdef __SHOW_KEY_BINDINGS__
                for (size_t j = 0; j < passedl; j++)
                {
                    for (int g = 0; g < cs->wx; g++)
                        mvaddch(cs->wy-1-s_Bar2Enable-j,g,' ');
                    mvprintw(cs->wy-1-s_Bar2Enable-j,0," %c\t%s",wctob(keys[passed[j]].keys[i]),keys[passed[j]].value);
                }
                #endif
            }
            if (event > 47 && event < 58) { cs->typed_keys[strlen(cs->typed_keys)] = event; csas_draw(cs,4); }
            event = getch();
        } while (event == -1 || event == KEY_RESIZE || (starts_string && event > 47 && event < 58));
        if (event == 27) { cs->was_typed = false; return -1; }
        cs->typed_keys[strlen(cs->typed_keys)] = event;
        starts_string = 1;

        int *tmp_passed = NULL;
        size_t tmp_passedl = 0;

        #ifdef __SHOW_KEY_BINDINGS__
        erase();
        refresh();
        #endif

        for (size_t j = 0; j < passedl; j++)
        {
            if (event == keys[passed[j]].keys[i])
            {
                tmp_passed = (int*)realloc(tmp_passed,(tmp_passedl+1)*sizeof(int));
                tmp_passedl++;
                tmp_passed[tmp_passedl-1] = passed[j];
                #ifdef __SHOW_KEY_BINDINGS__
                for (int g = 0; g < cs->wx; g++)
                    mvaddch(cs->wy-s_Bar2Enable-tmp_passedl,g,' ');
                mvprintw(cs->wy-s_Bar2Enable-tmp_passedl,0," %c\t%s",wctob(keys[passed[j]].keys[i+1]),keys[passed[j]].value);
                #endif
            }
        }

        free(passed);
        passed = tmp_passed;
        passedl = tmp_passedl;
    }

    cs->was_typed = false;
    #ifdef __SHOW_KEY_BINDINGS__
    erase();
    refresh();
    csas_draw(cs,-1);
    #endif
    return (passedl == 0) ? -1 : passed[0];
}

static void godown(Csas *cs, const int ws, const int which)
{
    if (cs->ws[ws].visual)
    {
        for (size_t i = G_S(ws,which); i < G_D(ws,which)->size; i++)
            G_D(ws,which)->el[i].list[ws] |= 1<<cs->ws[ws].sel_group;
    }
    if (G_D(ws,which)->size > (size_t)cs->win[which]->_maxy-!s_Borders+s_Borders)
        G_D(ws,which)->ltop[ws] = G_D(ws,which)->size-cs->win[which]->_maxy-!s_Borders+s_Borders;
    else
        G_D(ws,which)->ltop[ws] = 0;
    G_S(ws,which) = G_D(ws,which)->size-1;
}

static void gotop(Csas *cs, const int ws, const int which)
{
    if (cs->ws[ws].visual)
    {
        for (int i = G_S(ws,which); i > -1; i--)
            G_D(ws,which)->el[i].list[ws] |= 1<<cs->ws[ws].sel_group;
    }
    G_S(ws,which) = 0;
    G_D(ws,which)->ltop[ws] = 0;
}

static void move_d(const char how, Csas *cs, const int ws, const int which)
{
    if (G_S(ws,which) == G_D(ws,which)->size-1 && how == 1)
    {
        if (s_WrapScroll) gotop(cs,ws,which);
        return;
    }
    else if (G_S(ws,which) == 0 && how == -1)
    {
        if (s_WrapScroll) godown(cs,ws,which);
        return;
    }

    G_S(ws,which) += how;

    if ((how == 1)*((cs->win[which]->_maxy+G_D(ws,which)->ltop[ws]-(s_Borders*2)) < G_D(ws,which)->size-1)+(how == -1)*((cs->win[which]->_maxy+G_D(ws,which)->ltop[ws]-(s_Borders*2)) > 0) &&
        (how == 1)*(cs->win[which]->_maxy+G_D(ws,which)->ltop[ws]-(s_Borders*2) < G_S(ws,which)+(int)(cs->win[which]->_maxy*s_MoveOffSet))+(how == -1)*(G_D(ws,which)->ltop[ws] > G_S(ws,which)-(int)(cs->win[which]->_maxy*s_MoveOffSet)))
    {
        if (s_JumpScroll)
        {
            if ((how == 1)*((cs->win[which]->_maxy+G_D(ws,which)->ltop[ws]-(s_Borders*2)+(int)(cs->win[which]->_maxy*s_JumpScrollValue)) > G_D(ws,which)->size-1)
                +(how == -1)*(G_D(ws,which)->ltop[ws] > G_S(ws,which)-(int)(cs->win[which]->_maxy*s_MoveOffSet)))
                G_D(ws,which)->ltop[ws] = (how == 1)*(G_D(ws,which)->size-cs->win[which]->_maxy-!s_Borders+s_Borders);
            else
                G_D(ws,which)->ltop[ws] += (int)(how*cs->win[which]->_maxy*s_JumpScrollValue);
        }
        else
            G_D(ws,which)->ltop[ws] += how;
    }

    G_ES(ws,which).list[ws] |= (1<<cs->ws[ws].sel_group)*cs->ws[ws].visual;
}

void csas_exit(Csas *cs, const bool force)
{
    if (force)
        goto END;

    int count = 0;
    for (int i = 0; i < WORKSPACE_N; i++)
        count += cs->ws[i].exists;

    cs->ws[cs->current_ws].exists = 0;

    if (count > 1)
    {
        for (int i = cs->current_ws+1; i < WORKSPACE_N; i++)
            if (cs->ws[i].exists)
            {
                change_workspace(cs,i);
                return;
            }
        for (int i = 0; i < cs->current_ws; i++)
            if (cs->ws[i].exists)
            {
                change_workspace(cs,i);
                return;
            }
    }

    END: ;
    cs->exit_time = true;
}

void move_to(Csas *cs, const int ws, const int which, const char *name)
{
    if (strcmp(G_ES(ws,which).name,name) == 0)
        return;

    register size_t i, j;
    register char found = 0;

    for (i = 0; i < G_D(ws,which)->size; i++)
        if (strcmp(G_D(ws,which)->el[i].name,name) == 0)
        {
            found = 1;
            break;
        }
    
    if (!found)
        return;

    if (i > G_S(ws,which))
        for (j = G_S(ws,which); j < i; j++)
            move_d(1,cs,ws,which);
    else if (i < G_S(ws,which))
        for (j = G_S(ws,which); j > i; j--)
            move_d(-1,cs,ws,which);
}

static void bulk(Csas *cs, const int ws, const int selected, char **args, const uchar flag)
{
    char tmpfile[PATH_MAX];
    strcpy(tmpfile,TEMPTEMP);
    int fd = mkstemp(tmpfile);
    FILE *file = fdopen(fd,"w+");
    if (!file)
        return;

    size_t pathl = strlen(args[0]);
    bool comment_write;
    char *temp;

    for (size_t i = 0; i < cs->size; i++)
    {
        if (!(
        #ifdef __THREADS_FOR_DIR_ENABLE__
        !cs->base[i]->enable &&
        #endif
        cs->base[i]->size > 0))
            continue;

        if (pathl)
        {
            if (flag & 0x2)
            {
                if (!strstr(cs->base[i]->path,args[0]))
                    continue;
            }
            else if ((strcmp(cs->base[i]->path,args[0]) != 0))
                continue;
        }

        comment_write = 0;
        for (size_t j = 0; j < cs->base[i]->size; j++)
        {
            if (selected == -1 ? 1 : (cs->base[i]->el[j].list[ws]&selected))
            {
                if (!comment_write)
                {
                    fprintf(file,"//\t%s\n",cs->base[i]->path);
                    comment_write = 1;
                }
                temp = (flag & 0x1) ? mkpath(cs->base[i]->path,cs->base[i]->el[j].name) : cs->base[i]->el[j].name;
                fprintf(file,"%s\n",temp);
            }
        }
    }

    fflush(file);
    spawn(args[2],tmpfile,NULL,F_NORMAL|F_WAIT);

    size_t pos = 0, x;
    rewind(file);

    struct stat sfile;
    fstat(fd,&sfile);
    char *filecopy = (char*)malloc(sfile.st_size+1);
    fread(filecopy,1,sfile.st_size,file);
    
    freopen(tmpfile,"w+",file);
    char buffer[PATH_MAX];
    bool writed = 0;

    fprintf(file,"#!%s\n\n",args[1]);

    if (filecopy[pos])
    {
        for (size_t i = 0; i < cs->size; i++)
        {
            if (!(
            #ifdef __THREADS_FOR_DIR_ENABLE__
            !cs->base[i]->enable &&
            #endif
            cs->base[i]->size > 0))
                continue;

            if (pathl)
            {
                if (flag & 0x2)
                {
                    if (!strstr(cs->base[i]->path,args[0]))
                        continue;
                }
                else if ((strcmp(cs->base[i]->path,args[0]) != 0))
                    continue;
            }

            for (size_t j = 0; j < cs->base[i]->size; j++)
            {
                if (selected == -1 ? 1 : (cs->base[i]->el[j].list[ws]&selected))
                {
                    while (filecopy[pos] == '\n')
                    {
                        pos++;
                        j--;
                    }
                    if (!filecopy[pos])
                        break;

                    if (filecopy[pos] == '/' && filecopy[pos+1] && filecopy[pos+1] == '/')
                    {
                        while (filecopy[pos] && filecopy[pos] != '\n') pos++;
                        j--;
                    }
                    else
                    {
                        x = 0;
                        while (filecopy[pos] && filecopy[pos] != '\n') buffer[x++] = filecopy[pos++];
                        buffer[x] = '\0';
                        temp = (flag & 0x1) ? mkpath(cs->base[i]->path,cs->base[i]->el[j].name) : cs->base[i]->el[j].name;
                        if (x > 0 && strcmp(temp,buffer) != 0)
                        {
                            fprintf(file,"%s ",args[3]);
                            if (!(flag & 0x1))
                                temp = mkpath(cs->base[i]->path,temp);
                            atob(temp);
                            fprintf(file,"%s %s ",temp,args[4]);
                            temp = mkpath(cs->base[i]->path,buffer);
                            atob(temp);
                            fprintf(file,"%s %s\n",temp,args[5]);
                            writed = 1;
                        }
                    }
                    pos++;
                }
            }
        }
    }

    fflush(file);
    free(filecopy);

    if (writed == 0)
        goto END;

    spawn(args[2],tmpfile,NULL,F_NORMAL|F_WAIT);
    spawn(args[1],tmpfile,NULL,F_NORMAL|F_WAIT|F_CONFIRM);

    END:
    fclose(file);
    unlink(tmpfile);
}

char *___SET(char *src, Csas *cs)
{
    size_t pos = 0;

    size_t end = 0;
    while (src[pos+end] && !isspace(src[pos+end])) end++;
    int gga = -1;

    for (int i = 0;cfg_names[i].n; i++)
        if (strlen(cfg_names[i].n) == end && strncmp(src+pos,cfg_names[i].n,end) == 0) { gga = i; break; }

    if (gga == -1) return NULL;

    pos += end;
    pos += findfirst(src+pos,isspace);

    return atov(cfg_names[gga].v,src+pos,&pos,cs,cfg_names[gga].t);
}

#ifdef __LOAD_CONFIG_ENABLE__
char *___INCLUDE(char *src, Csas *cs)
{
    char temp[8192];

    atop(temp,src,cs);
    config_load(temp,cs);
    return NULL;
}
#endif

char *___MAP(char *src, Csas *cs)
{
    size_t pos = 0, end = 0;
    char temp1[64];
    char temp2[PATH_MAX];
    while (src[pos+end] && !isspace(src[pos+end])) end++;
    strncpy(temp1,src+pos,end);
    temp1[end] = '\0';
    pos += end;

    pos += atop(temp2,src+pos,cs);

    addkey(temp1,temp2);
    return NULL;
}

char *___MOVE(char *src, Csas *cs)
{
    size_t pos = 0;
    char rot = -1;
    int ws = cs->current_ws, mul1 = 1, mul2 = 0;

    while (src[pos])
    {
        if (src[pos] && src[pos] == '-')
        {
            do {
                pos++;

                switch (src[pos])
                {
                    case 'd':
                        rot = 1;
                        break;
                    case 'u':
                        rot = -1;
                        break;
                    case 'l':
                        rot = 3;
                        break;
                    case 'r':
                        rot = 4;
                        break;
                    case 'c':
                    case 'w':
                        {
                            char ctemp = src[pos];
                            pos++;
                            pos += findfirst(src+pos,isspace);
                            if (ctemp == 'c')
                                mul1 = atoi(src+pos);
                            else
                                ws = atoi(src+pos);
                            while (isdigit(src[pos])) pos++;
                        }
                        break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }
        pos++;
    }

    if (!cs->ws[ws].exists)
        return NULL;

    switch (rot)
    {
        case -1:
        case 1:
            if (
                #ifdef __THREADS_FOR_DIR_ENABLE__
                !G_D(ws,1)->enable &&
                #endif
                G_D(ws,1)->size > 0)
            {
                mul2 = atoi(cs->typed_keys);
                for (register int i = 0; i < (mul2+(mul2 == 0))*mul1; i++)
                    move_d(rot,cs,ws,1);
                if (s_Win3Enable)
                    get_preview(cs);
            }
            break;
        case 3:
            if (cs->ws[ws].path[0] == '/' && cs->ws[cs->current_ws].path[1] == '\0')
                break;
            csas_cd("..",ws,cs);
            cs->ws[ws].visual = 0;
            break;
        case 4:
            if (G_D(ws,1)->size > 0)
            {
                switch (G_ES(ws,1).type&T_GT)
                {
                    case T_DIR:
                        csas_cd(G_ES(ws,1).name,ws,cs);
                        cs->ws[ws].visual = 0;
                        break;
                    case T_REG:
                        file_run(G_ES(ws,1).name);
                        break;
                }
            }
            break;
    }
    return NULL;
}

char *___QUIT(char *src, Csas *cs)
{
    bool force = false;
    if (src[0] && src[0] == '-' && src[1] && src[1] == 'f')
        force = true;
    csas_exit(cs,force);
    return NULL;
}

char *___CD(char *src, Csas *cs)
{
    size_t pos = 0;
    int ws = cs->current_ws;
    char path[8192];
    memset(path,0,8191);

    while (src[pos])
    {
        if (src[pos] == '-' && src[pos+1] && src[pos+1] == 'w')
        {
            pos += 2;
            pos += findfirst(src+pos,isspace);
            ws = atoi(src+pos);
            while (isdigit(src[pos])) pos++;
        }
        else
            pos += atop(path,src+pos,cs);

        pos += findfirst(src+pos,isspace);
    }

    cs->ws[ws].exists = true;
    change_workspace(cs,ws);

    csas_cd(path,ws,cs);
    return NULL;
}

char *___GOTOP(char *src, Csas *cs)
{
    size_t pos = 0, target = atol(cs->typed_keys);
    int ws = cs->current_ws;

    while (src[pos])
    {
        if (src[pos] == '-')
        {
            do {
                pos++;
                switch (src[pos])
                {
                    case 't':
                    case 'w':
                        pos++;
                        pos += findfirst(src+pos,isspace);
                        if (src[pos] == 't')
                            target = atol(src+pos);
                        else
                            ws = atoi(src+pos);
                        while (isdigit(src[pos])) pos++;
                        break;
                }
            } while (src[pos] && !isspace(src[pos]));
        }

        pos += findfirst(src+pos,isspace);
        if (src[pos+1] == '\0')
            break;
    }

    if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        !G_D(ws,1)->enable &&
        #endif
        G_D(ws,1)->size > 0)
    {
        if (target == 0)
            gotop(cs,ws,1);
        else if (target > G_S(ws,1))
        {
            for (size_t i = G_S(ws,1); i < target-1; i++)
                move_d(1,cs,ws,1);
        }
        else if (target < G_S(ws,1))
        {
            for (size_t i = G_S(ws,1); i > target-1; i--)
                move_d(-1,cs,ws,1);
        }
        if (ws == cs->current_ws && s_Win3Enable)
            get_preview(cs);
    }
    return NULL;
}

char *___GODOWN(char *src, Csas *cs)
{
    size_t pos = 0, target = atol(cs->typed_keys);
    int ws = cs->current_ws;

    while (src[pos])
    {
        if (src[pos] == '-')
        {
            do {
                pos++;
                switch (src[pos])
                {
                    case 't':
                    case 'w':
                        pos++;
                        pos += findfirst(src+pos,isspace);
                        if (src[pos] == 't')
                            target = atol(src+pos);
                        else
                            ws = atoi(src+pos);
                        while (isdigit(src[pos])) pos++;
                        break;
                }
            } while (src[pos] && !isspace(src[pos]));
        }

        pos += findfirst(src+pos,isspace);
        if (src[pos+1] == '\0') break;
    }

    if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        !G_D(ws,1)->enable &&
        #endif
        G_D(ws,1)->size > 0)
    {
        if (target == 0)
            godown(cs,target,1);
        else if (target > G_S(ws,1))
        {
            for (size_t i = G_S(ws,1); i < target-1; i++)
                move_d(1,cs,ws,1);
        }
        else if (target < G_S(ws,1))
        {
            for (size_t i = G_S(ws,1); i > target-1; i--)
                move_d(-1,cs,ws,1);
        }
        if (ws == cs->current_ws && s_Win3Enable)
            get_preview(cs);
    }
    return NULL;
}

char *___CHANGEWORKSPACE(char *src, Csas *cs)
{
    change_workspace(cs,atoi(src));
    return NULL;
}

#ifdef __FILE_SIZE_ENABLE__

static void GETSIZE(struct Element *el, const int fd, const uchar flag)
{
    if ((el->type&T_GT) == T_DIR)
    {
        int tfd;
        if ((tfd = openat(fd,el->name,O_DIRECTORY)) != -1)
        {
            if (flag&D_F)
            {
                struct stat sFile;
                stat(el->name,&sFile);
                el->size = sFile.st_size;
            }
            else
            {
                ull size = 0, count = 0;
                get_dirsize(tfd,&count,&size,flag);
                el->size = (flag&D_C) ? count : size;
            }
            close(tfd);
        }
    }
}

char *___GETSIZE(char *src, Csas *cs)
{
    size_t pos = 0;
    uchar flag = D_S;
    int selected = -1, ws = cs->current_ws;
    char *path = (char*)calloc(sizeof(char),PATH_MAX);

    while (src[pos])
    {
        pos += findfirst(src+pos,isspace);

        if (src[pos] == '-')
        {
            do {
                pos++;
                switch (src[pos])
                {
                    case 'w':
                    case 's':
                        {
                            int itemp = (src[pos] == 'w' ? 1 : 0);
                            pos++;
                            pos += findfirst(src+pos,isspace);
                            if (itemp)
                                ws = atoi(src+pos);
                            else
                            {
                                switch (src[pos])
                                {
                                    case '-': selected = -1; pos++; break;
                                    case '.': selected = -2; pos++; break;
                                    case 's': selected = -3; pos++; break;
                                    default: selected = 1<<atoi(src+pos); break;
                                }
                            }
                            while (isdigit(src[pos])) pos++;
                        }
                        break;
                    case 'R': flag |= 0x10; break;
                    case 'r': flag |= D_R; break;
                    case 'c': flag |= D_C; flag &= ~D_S; break;
                    case 'f': flag |= D_F; break;
                }
            } while (src[pos] && !isspace(src[pos]));
        }
        else
            pos += atop(path,src+pos,cs);

        if (src[pos+1] == '\0')
            break;
    }

    if (!cs->ws[cs->current_ws].exists)
    {
        free(path);
        return NULL;
    }

    if (ws != cs->current_ws)
		if (chdir(G_D(ws,1)->path) != 0)
			return NULL;

    if (selected == -2)
        selected = cs->ws[ws].sel_group;
    if (selected == -3)
    {
        int fd;
        if (
            #ifdef __THREADS_FOR_DIR_ENABLE__
            !G_D(ws,1)->enable &&
            #endif
            G_D(ws,1)->size > 0 && (fd = open(G_D(ws,1)->path,O_DIRECTORY)) != -1)
        GETSIZE(&G_ES(ws,1),fd,flag);
        if (!(flag&0x10))
        {
            free(path);
            return NULL;
        }
        else
            strcpy(path,mkpath(G_D(ws,1)->path,G_ES(ws,1).name));
    }

    if (path[0])
    {
        char *stemp = (char*)malloc(PATH_MAX);
        realpath(path,stemp);
        free(path);
        path = stemp;
    }

    int temp;

    for (size_t i = 0; i < cs->size; i++)
    {
        if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        cs->base[i]->enable ||
        #endif
        cs->base[i]->size < 1)
            continue;
        if (path[0])
        {
            if (flag&0x10)
            {
                if (!strstr(cs->base[i]->path,path))
                    continue;
            }
            else if (strcmp(cs->base[i]->path,path) != 0)
                continue;
        }

        if ((temp = open(cs->base[i]->path,O_DIRECTORY)) == -1)
            continue;

        for (size_t j = 0; j < cs->base[i]->size; j++)
            if (selected < 0 ? 1 : (cs->base[i]->el[j].list[ws]&(1<<selected)))
                GETSIZE(&cs->base[i]->el[j],temp,flag);
        close(temp);
    }

    free(path);
    if (ws != cs->current_ws)
        chdir(G_D(cs->current_ws,1)->path);
    return NULL;
}

#endif

char *___SETGROUP(char *src, Csas *cs)
{
    cs->ws[cs->current_ws].sel_group = atoi(src);
    if (cs->ws[cs->current_ws].visual && G_D(cs->current_ws,1)->size > 0)
        G_ES(cs->current_ws,1).list[cs->current_ws] |= 1<<cs->ws[cs->current_ws].sel_group;
    return NULL;
}

char *___FASTSELECT(char *src, Csas *cs)
{
    if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        !G_D(cs->current_ws,1)->enable &&
        #endif
        G_D(cs->current_ws,1)->size > 0)
    {
        G_ES(cs->current_ws,1).list[cs->current_ws] ^= 1<<cs->ws[cs->current_ws].sel_group;
        move_d(1,cs,cs->current_ws,1);
        if (s_Win3Enable)
            get_preview(cs);
    }
    return NULL;
}

char *___OPEN_WITH(char *src, Csas *cs)
{
    if (G_D(cs->current_ws,1)->size == 0)
        return NULL;
    char temp[4096];
    size_t t1 = findfirst(src,isspace),
        t2 = findfirst(src+t1,isalnum);
    memcpy(temp,src+t1,t2);
    temp[t2] = 0;
    spawn(temp,G_ES(cs->current_ws,1).name,NULL,F_NORMAL|F_WAIT);
    return NULL;
}

char *___TOGGLEVISUAL(char *src, Csas *cs)
{
    cs->ws[cs->current_ws].visual = !cs->ws[cs->current_ws].visual;
    if (cs->ws[cs->current_ws].visual && G_D(cs->current_ws,1)->size > 0)
        G_ES(cs->current_ws,1).list[cs->current_ws] |= 1<<cs->ws[cs->current_ws].sel_group;
    return NULL;
}

char *___F_MOD(char *src, Csas *cs)
{
    uchar Action = 0;
    mode_t arg = 0;
    size_t pos = 0;

    switch (src[0])
    {
        case 'd': case 'D': Action = 1; break;
        case 'c': case 'C': Action = 2; break;
        case 'm': case 'M': Action = 3; break;
        default: return NULL;
    }

    pos++;
    pos += findfirst(src+pos,isspace);

    int selected = -1, ws = cs->current_ws;
    char *path = (char*)calloc(sizeof(char),PATH_MAX);
    char *target = (char*)calloc(sizeof(char),PATH_MAX);
    int fd1, fd2, fd3;
    char *buffer = Action == 1 ? NULL : (char*)malloc(s_CopyBufferSize);

    while (src[pos])
    {
        pos += findfirst(src+pos,isspace);

        if (src[pos] == '-')
        {
            do {
                pos++;
                switch (src[pos])
                {
                    case 'o':
                        pos++;
                        pos += atop(target,src+pos,cs);
                        break;
                    case 'w':
                    case 's':
                        {
                            int itemp = (src[pos] == 'w' ? 1 : 0);
                            pos++;
                            pos += findfirst(src+pos,isspace);
                            if (itemp)
                                ws = atoi(src+pos);
                            else
                            {
                                switch (src[pos])
                                {
                                    case '-': selected = -1; pos++; break;
                                    case '.': selected = -2; pos++; break;
                                    case 's': selected = -3; pos++; break;
                                    default: selected = 1<<atoi(src+pos); break;
                                }
                            }
                            while (isdigit(src[pos])) pos++;
                        }
                        break;
                    case 'c': arg |= M_CHNAME; break;
                    case 'r': arg |= M_REPLACE; break;
                    case 'd': arg |= M_DCPY; break;
                    case 'm': arg |= M_MERGE; break;
                }
            } while (src[pos] && !isspace(src[pos]));
        }
        else
            pos += atop(path,src+pos,cs);

        if (src[pos+1] == '\0')
            break;
    }

    if (!cs->ws[cs->current_ws].exists)
        goto END;

    if (ws != cs->current_ws)
        if (chdir(G_D(ws,1)->path) != 0)
			return NULL;

    if (selected == -2)
        selected = cs->ws[ws].sel_group;

    if (!target[0])
        strcpy(target,".");

    if ((fd1 = open(target,O_DIRECTORY)) == -1)
        goto END;

    char *ActionName = NULL;
    switch (Action)
    {
        case 1: ActionName = "delete"; break;
        case 2: ActionName = "copy"; break;
        case 3: ActionName = "move"; break;
    }

    struct stat ST;
    ull count = 0, size = 0;

    if (selected == -3)
    {
        if (
            #ifdef __THREADS_FOR_DIR_ENABLE__
            !G_D(ws,1)->enable &&
            #endif
            G_D(ws,1)->size > 0 && (fd2 = open(G_D(ws,1)->path,O_DIRECTORY)) != -1)
        {
            if (fstatat(fd2,G_ES(ws,1).name,&ST,AT_SYMLINK_NOFOLLOW) == -1)
                goto END2;

            if ((ST.st_mode&S_IFMT) == S_IFDIR)
            {
				if ((fd3 = openat(fd2,G_ES(ws,1).name,O_RDONLY)) == -1)
					goto END2;
                get_dirsize(fd3,&count,&size,D_R|D_F|D_S);
				close(fd3);
                count++;
                size += ST.st_size;
                set_message(cs,0,"Do you want to %s %ld files(%s)? (Y/n)",ActionName,count,stoa(size));
            }
            else
                #ifdef __FILE_SIZE_ENABLE__
                set_message(cs,0,"Do you want to %s \"%s\" (%s)? (Y/n)",ActionName,G_ES(ws,1).name,stoa(G_ES(ws,1).size));
                #else
                set_message(cs,0,"Do you want to %s \"%s\"? (Y/n)",ActionName,G_ES(ws,1).name);
                #endif

            wrefresh(cs->win[5]);
            int si = -1;
            for (;;)
            {
                si = getch();
                if (si == KEY_RESIZE)
                {
                    update_size(cs);
                    csas_draw(cs,-1);
                    if ((ST.st_mode&S_IFMT) == S_IFDIR)
                        set_message(cs,0,"Do you want to %s %ld files(%s)? (Y/n)",ActionName,count,stoa(size));
                    else
                        #ifdef __FILE_SIZE_ENABLE__
                        set_message(cs,0,"Do you want to %s \"%s\" (%s)? (Y/n)",ActionName,G_ES(ws,1).name,stoa(G_ES(ws,1).size));
                        #else
                        set_message(cs,0,"Do you want to %s \"%s\"? (Y/n)",ActionName,G_ES(ws,1).name);
                        #endif
                    wrefresh(cs->win[5]);
                }
                else if (si == 'y' || si == 'Y')
                    break;
                else if (si != -1)
                    goto END2;
            }

            switch (Action)
            {
                case 1: file_rm(fd2,G_ES(ws,1).name); break;
                case 2: file_cp(fd1,fd2,G_ES(ws,1).name,buffer,arg); break;
                case 3: file_mv(fd1,fd2,G_ES(ws,1).name,buffer,arg); break;
            }
            END2: ;
            close(fd2);
        }
        close(fd1);
        goto END;
    }

    if (path[0])
    {
        char *stemp = (char*)malloc(PATH_MAX);
        realpath(path,stemp);
        free(path);
        path = stemp;
    }

    for (size_t i = 0; i < cs->size; i++)
    {
        if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        cs->base[i]->enable ||
        #endif
        cs->base[i]->size < 1)
            continue;
        if (path[0] && strcmp(cs->base[i]->path,path) != 0)
                continue;

        if ((fd2 = open(cs->base[i]->path,O_DIRECTORY)) == -1)
            continue;

        for (size_t j = 0; j < cs->base[i]->size; j++)
        {
            if (selected < 0 ? 1 : (cs->base[i]->el[j].list[ws]&(1<<selected)))
            {
                if (fstatat(fd2,cs->base[i]->el[j].name,&ST,AT_SYMLINK_NOFOLLOW) == -1)
                    continue;
                count++;
                size += ST.st_size;
                if ((ST.st_mode&S_IFMT) == S_IFDIR)
				{
					if ((fd3 = openat(fd2,cs->base[i]->el[j].name,O_RDONLY)) != -1)
					{
						get_dirsize(fd3,&count,&size,D_R|D_C|D_S);
						close(fd3);
					}
				}
            }
        }
        close(fd2);
    }

    set_message(cs,0,"Do you want to %s %ld files(%s)? (Y/n)",ActionName,count,stoa(size));
    wrefresh(cs->win[5]);
    int si = -1;
    for (;;)
    {
        si = getch();
        if (si == KEY_RESIZE)
        {
            update_size(cs);
            csas_draw(cs,-1);
            set_message(cs,0,"Do you want to %s %ld files(%s)? (Y/n)",ActionName,count,stoa(size));
            wrefresh(cs->win[5]);
        }
        else if (si == 'y' || si == 'Y')
            break;
        else if (si != -1)
            goto END;
    }

    for (size_t i = 0; i < cs->size; i++)
    {
        if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        cs->base[i]->enable ||
        #endif
        cs->base[i]->size < 1)
            continue;
        if (path[0] && strcmp(cs->base[i]->path,path) != 0)
                continue;

        if ((fd2 = open(cs->base[i]->path,O_DIRECTORY)) == -1)
            continue;

        for (size_t j = 0; j < cs->base[i]->size; j++)
        {
            if (selected < 0 ? 1 : (cs->base[i]->el[j].list[ws]&(1<<selected)))
            {
                switch (Action)
                {
                    case 1: file_rm(fd2,cs->base[i]->el[j].name); break;
                    case 2: file_cp(fd1,fd2,cs->base[i]->el[j].name,buffer,arg); break;
                    case 3: file_mv(fd1,fd2,cs->base[i]->el[j].name,buffer,arg); break;
                }
            }
        }
        close(fd2);
    }
    close(fd1);

    update_size(cs);
    csas_cd(".",cs->current_ws,cs);

    END: ;
    cs->ws[cs->current_ws].show_message = false;
    free(buffer);
    free(path);
    free(target);
    if (ws != cs->current_ws)
        chdir(G_D(cs->current_ws,1)->path);
    return NULL;
}

static void GETDIR(char *path, Csas *cs, uchar mode
#ifdef __THREADS_FOR_DIR_ENABLE__
, bool threads
#endif
)
{
    DIR *d;
    if ((d = opendir(path)) == NULL)
        return;
    struct dirent *dir;
    while ((dir = readdir(d)))
    {
        if (dir->d_name[0] == '.' && (dir->d_name[1] == '\0' || (dir->d_name[1] == '.' && dir->d_name[2] == '\0')))
            continue;
        if (dir->d_type == 4)
        {
            char path1[PATH_MAX];
            strcpy(path1,path);
            strcat(path1,"/");
            strcat(path1,dir->d_name);
            getdir(path1,cs,cs->current_ws,1,mode
            #ifdef __FOLLOW_PARENT_DIR__
            ,NULL
            #endif
            #ifdef __THREADS_FOR_DIR_ENABLE__
            ,threads
            #endif
            );
            GETDIR(path1,cs,mode
            #ifdef __THREADS_FOR_DIR_ENABLE__
            ,threads
            #endif
            );
       }
    }
    closedir(d);
    return;
}

char *___LOAD(char *src, Csas *cs)
{
    size_t pos = 0;
    int mode = s_DirLoadingMode;
    uchar flag = 0;
	werase(cs->win[0]);
	if (s_Win1Enable)
		werase(cs->win[1]);
	if (s_Win3Enable)
		werase(cs->win[2]);

    while (src[pos])
    {
        if (src[pos] == '-')
        {
            do {
                pos++;

                switch (src[pos])
                {
                    case 'm':
                        pos++;
                        pos += findfirst(src+pos,isspace);
                        mode += atoi(src+pos);
                        while (isdigit(src[pos])) pos++;
                        break;
                    #ifdef __THREADS_FOR_DIR_ENABLE__
                    case 't': flag |= 0x1; break;
                    #endif
                    case 'R': flag |= 0x2; break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }

        if (src[pos+1] == '\0')
            break;
        else
            pos += findfirst(src+pos,isspace);
    }

    getdir(".",cs,cs->current_ws,1,mode
    #ifdef __FOLLOW_PARENT_DIR__
    ,NULL
    #endif
    #ifdef __THREADS_FOR_DIR_ENABLE__
    ,(flag&0x1)
    #endif
    );
    if (flag&0x2)
    {
        char path[PATH_MAX];
        strcpy(path,".");
        GETDIR(path,cs,mode
        #ifdef __THREADS_FOR_DIR_ENABLE__
        ,(flag&0x1)
        #endif
        );
    }
    return NULL;
}

char *___SELECT(char *src, Csas *cs)
{
    size_t pos = 0;
    int mode = 1;
    bool recursive = 0;
    int selected = -1, workspace1 = cs->current_ws, workspace2 = cs->current_ws, toselected = -1;
    char *path = (char*)calloc(sizeof(char),PATH_MAX);

    while (src[pos])
    {
        pos += findfirst(src+pos,isspace);

        if (src[pos] == '-')
        {
            do {
                pos++;

                switch (src[pos])
                {
                    case 'R':
                        recursive = 1;
                        break;
                    case 'o':
                        pos++;
                        pos+= findfirst(src+pos,isspace);
                        switch(src[pos])
                        {
                            case '.':
                                toselected = -1;
                                pos++;
                                break;
                            default:
                                toselected = atoi(src+pos);
                                while (isdigit(src[pos])) pos++;
                                break;
                        }
                        break;
                    case 'w':
                    case 'W':
                        {
                            int itemp = (src[pos] == 'w' ? 1 : 0);
                            pos++;
                            pos += findfirst(src+pos,isspace);
                            if (itemp)
                                workspace1 = atoi(src+pos);
                            else
                                workspace2 = atoi(src+pos);
                            while (isdigit(src[pos])) pos++;
                        }
                        break;
                    case 's':
                        pos++;
                        pos += findfirst(src+pos,isspace);
                        switch (src[pos])
                        {
                            case '-':
                                selected = -1;
                                pos++;
                                break;
                            case '.':
                                selected = -2;
                                pos++;
                                break;
                            case 's':
                                selected = -3;
                                pos++;
                                break;
                            default:
                                selected = 1<<atoi(src+pos);
                                while (isdigit(src[pos])) pos++;
                                break;
                        }
                        break;
                    case 'e': mode = 1; break;
                    case 'd': mode = 0; break;
                    case 't': mode = -1; break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }
        else
            pos += atop(path,src+pos,cs);

        if (src[pos+1] == '\0')
            break;
    }

    if (!cs->ws[cs->current_ws].exists)
    {
        free(path);
        return NULL;
    }

    if (workspace1 != cs->current_ws)
        if (chdir(G_D(workspace1,1)->path) != 0)
			return NULL;

    if (selected == -2)
        selected = cs->ws[workspace1].sel_group;
    if (toselected == -1)
        toselected = cs->ws[workspace2].sel_group;
    if (selected == -3)
    {
        if (!recursive)
        {
            free(path);
            return NULL;
        }

        strcpy(path,mkpath(G_D(workspace1,1)->path,G_ES(workspace1,1).name));
        switch (mode)
        {
            case -1: G_ES(workspace1,1).list[workspace2] ^= 1<<toselected; break;
            case 0: G_ES(workspace1,1).list[workspace2] &= ~(1<<toselected); break;
            case 1: G_ES(workspace1,1).list[workspace2] |= 1<<toselected; break;
        }
    }

    if (path[0])
    {
        char *stemp = (char*)malloc(PATH_MAX);
        realpath(path,stemp);
        free(path);
        path = stemp;
    }

    for (size_t i = 0; i < cs->size; i++)
    {
        if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        cs->base[i]->enable ||
        #endif
        cs->base[i]->size < 1)
            continue;
        if (path[0])
        {
            if (recursive)
            {
                if (!strstr(cs->base[i]->path,path))
                    continue;
            }
            else if (strcmp(cs->base[i]->path,path) != 0)
                continue;
        }

        for (size_t j = 0; j < cs->base[i]->size; j++)
        {
            if (selected < 0 ? 1 : (cs->base[i]->el[j].list[workspace1]&(1<<selected)))
            {
                switch (mode)
                {
                    case -1: cs->base[i]->el[j].list[workspace2] ^= 1<<toselected; break;
                    case 0: cs->base[i]->el[j].list[workspace2] &= ~(1<<toselected); break;
                    case 1: cs->base[i]->el[j].list[workspace2] |= 1<<toselected; break;
                }
            }
        }
    }

    free(path);
    if (workspace1 != cs->current_ws)
        chdir(G_D(cs->current_ws,1)->path);
    return NULL;
}

char *___EXEC(char *src, Csas *cs)
{
    size_t pos = 0;
    int background = 0;
    char temp[PATH_MAX];

    while (src[pos])
    {
        if (src[pos] == '-')
        {
            do {
                pos++;

                switch (src[pos])
                {
                    case 'b':
                        background = 1;
                        break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }
        else
            pos += atop(temp,src+pos,cs);

        pos += findfirst(src+pos,isspace);
    }

    spawn(temp,NULL,NULL,background ? F_SILENT : F_NORMAL|F_WAIT);
    return NULL;
}

char *___BULK(char *src, Csas *cs)
{
    size_t pos = 0;
    int ws = cs->current_ws, selected = -1;
    uchar flag = 0;
    char *temp[6];
    for (int i = 0; i < 6; i++)
        temp[i] = (char*)calloc(sizeof(char),PATH_MAX);
    char path[PATH_MAX] = {0};

    while (src[pos])
    {
        if (src[pos] && src[pos] == '-')
        {
            do {
                pos++;
                switch (src[pos])
                {
                    case 'f':
                        flag |= 0x1;
                        break;
                    case 'w':
                        pos++;
                        pos += findfirst(src+pos,isspace);
                        ws = atoi(src+pos);
                        while (isdigit(src[pos])) pos++;
                        break;
                    case 's':
                        pos++;
                        pos += findfirst(src+pos,isspace);
                        if (src[pos] == '-')
                        {
                            selected = -1;
                            pos++;
                        } else if (src[pos] == 's')
                        {
                            selected = cs->ws[ws].sel_group;
                            pos++;
                        }
                        else
                        {
                            selected = 1<<atoi(src+pos);
                            while (isdigit(src[pos])) pos++;
                        }
                        break;
                    case 'R':
                        flag |= 0x2;
                        break;
                    case 'S':
                    case 'E':
                    case 'b':
                    case 'm':
                    case 'e':
                        {
                            int itemp;
                            switch (src[pos])
                            {
                                case 'S': itemp = 1; break;
                                case 'E': itemp = 2; break;
                                case 'b': itemp = 3; break;
                                case 'm': itemp = 4; break;
                                default: itemp = 5; break;
                            }
                            pos++;
                            pos += atop(temp[itemp],src+pos,cs);
                        }
                        break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }
        else
            pos += atop(path,src+pos,cs);
        pos++;
    }

    if (!cs->ws[ws].exists)
        return NULL;

    if (cs->current_ws != ws)
        if (chdir(G_D(ws,1)->path) != 0)
			return NULL;

    if (path[0])
        realpath(path,temp[0]);
    bulk(cs,ws,selected,temp,flag);
    for (int i = 0; i < 6; i++)
        free(temp[i]);
    if (cs->current_ws != ws)
        chdir(G_D(ws,1)->path);
    return NULL;
}

char *___CONSOLE(char *src, Csas *cs)
{
    size_t pos = 0;
    char add_text[4096], first_text[NAME_MAX] = ":";
    add_text[0] = '\0';
    int flags = 0;
    int n = 1;

    struct WinArgs args = {stdscr,0,0,-1,1,1,-1,-1,-1,-1,-1,-1,-1,-1,1,-1,-1,-1,-1,0};

    while (src[pos])
    {
        if (src[pos] && src[pos] == '-')
        {
            do {
                pos++;
                switch (src[pos])
                {
                    case 'r':
                        flags |= 16;
                        break;
                    case 'a':
                        pos++;
                        pos += atop(add_text,src+pos,cs);
                        break;
                    case 'f':
                        pos++;
                        pos += atop(first_text,src+pos,cs);
                        break;
                    case 'b':
                        flags |= 1;
                        break;
                    case 'n':
                        pos++;
                        pos += findfirst(src+pos,isspace);
                        n = atoi(src+pos);
                        break;
                    case 's':
                        pos++;
                        pos += findfirst(src+pos,isspace);
                        if (src[pos] != 'x')
                        {
                            size_t end = findfirst(src+pos,isdigit);
                            if (src[pos+end] == '%')
                            {
                                end++;
                                args.s_sizex = -1;
                                args.min_sizex = -1;
                                args.max_sizex = -1;
                                args.p_sizex = atof(src+pos)/100;
                            }
                            else
                            {
                                args.p_sizex = -1;
                                args.min_sizex = -1;
                                args.max_sizex = -1;
                                args.s_sizex = atoi(src+pos);
                            }
                            pos += end;
                        }
                        if (src[pos] == 'x')
                        {
                            pos++;
                            size_t end = findfirst(src+pos,isdigit);
                            if (src[pos+end] == '%')
                            {
                                end++;
                                args.s_sizey = -1;
                                args.min_sizey = -1;
                                args.max_sizey = -1;
                                args.p_sizey = atof(src+pos)/100;
                            }
                            else
                            {
                                args.p_sizey = -1;
                                args.min_sizey = -1;
                                args.max_sizey = -1;
                                args.s_sizey = atoi(src+pos);
                            }
                            pos += end;
                        }
                        break;
                    case 'p':
                        pos++;
                        pos += findfirst(src+pos,isspace);
                        if (src[pos] != 'x')
                        {
                            size_t end = findfirst(src+pos,isdigit);
                            if (src[pos+end] == '%')
                            {
                                end++;
                                args.s_posx = -1;
                                args.min_posx = -1;
                                args.max_posx = -1;
                                args.p_posx = atof(src+pos)/100;
                            }
                            else
                            {
                                args.p_posx = -1;
                                args.min_posx = -1;
                                args.max_posx = -1;
                                args.s_posx = atoi(src+pos);
                            }
                            pos += end;
                        }
                        if (src[pos] == 'x')
                        {
                            pos++;
                            size_t end = findfirst(src+pos,isdigit);
                            if (src[pos+end] == '%')
                            {
                                end++;
                                args.s_posy = -1;
                                args.min_posy = -1;
                                args.max_posy = -1;
                                args.p_posy = atof(src+pos)/100;
                            }
                            else
                            {
                                args.p_posy = -1;
                                args.min_posy = -1;
                                args.max_posy = -1;
                                args.s_posy = atoi(src+pos);
                            }
                            pos += end;
                        }
                        break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }
        pos++;
    }

    if (n == 0)
        return NULL;

    args.cfg = flags;

    char tmp[4096];

    while (n == -1 || n > 0)
    {
        if (cs->consolehistory.size == cs->consolehistory.max_size)
        {
            memset(cs->consolehistory.history[0],0,cs->consolehistory.alloc_r-1);
            char *tmp = cs->consolehistory.history[0];
            for (size_t i = 0; i < cs->consolehistory.size-1; i++)
                cs->consolehistory.history[i] = cs->consolehistory.history[i+1];
            cs->consolehistory.history[cs->consolehistory.size-1] = tmp;
            cs->consolehistory.size--;
        }

        if (cs->consolehistory.size == cs->consolehistory.allocated)
        {
            cs->consolehistory.history = (char**)realloc(cs->consolehistory.history,(cs->consolehistory.allocated += cs->consolehistory.inc_r)*sizeof(char*));
            for (size_t i = cs->consolehistory.allocated-cs->consolehistory.inc_r; i < cs->consolehistory.allocated; i++)
                cs->consolehistory.history[i] = (char*)calloc(sizeof(char),cs->consolehistory.alloc_r);
        }
        cs->consolehistory.size++;

        console_getline(cs->win[5],cs,cs->consolehistory.history,cs->consolehistory.size,cs->consolehistory.alloc_r-1,&args,first_text,add_text[0] ? add_text : NULL);

        size_t i = 0, x = 0;
        char *dm = cs->consolehistory.history[cs->consolehistory.size-1];
        while (dm[i])
        {
            if (dm[i] == '\'')
            {
                tmp[x++] = dm[i++];
                size_t end = (strchr(dm+i,'\'')-(dm+i))+1;
                memcpy(tmp+x,dm+i,end);
                i += end;
                x += end;
                continue;
            }
            if (src[i] == '$')
            {
                get_special(tmp,dm,&i,&x,cs);
                continue;
            }
            tmp[x++] = dm[i++];
        }
        tmp[x] = '\0';
        if (strcmp(tmp,"exit") == 0)
            break;
        command_run(tmp,cs);
        if (n != -1)
            n--;
        args.y++;
    }
    if (flags&16)
        return cs->consolehistory.history[cs->consolehistory.size-1];
    return NULL;
}

char *___SEARCH(char *src, Csas *cs)
{
    if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        G_D(cs->current_ws,1)->enable ||
        #endif
        G_D(cs->current_ws,1)->size < 1)
        return NULL;
    size_t pos = 0, mul = 1;
    int selected = -1;
    int action = -1;
    char temp[1024];

    while (src[pos])
    {
        if (src[pos] && src[pos] == '-')
        {
            do {
                pos++;
                switch (src[pos])
                {
                    case 's':
                        pos++;
                        pos += findfirst(src+pos,isspace);
                        if (src[pos] == '-')
                        {
                            selected = -1;
                            pos++;
                        }
                        else
                        {
                            selected = 1<<atoi(src+pos);
                            while (isdigit(src[pos])) pos++;
                        }
                        break;
                    case 'b':
                    case 'n':
                        action = (src[pos] == 'n' ? 2 : 1);
                        pos++;
                        pos += findfirst(src+pos,isspace);
                        mul = atoi(src+pos);
                        while (isdigit(src[pos])) pos++;
                        break;
                    case 'N':
                    case 'E':
                    case 'e':
                        switch (src[pos])
                        {
                            case 'N': action = 3; break;
                            case 'e': action = 4; break;
                            case 'E': action = 5; break;
                        }
                        pos++;
                        pos += atop(temp,src+pos,cs);
                        break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }
        pos++;
    }

    switch (action)
    {
        case 1:
        case 2:
            if (!cs->SearchList.size)
                break;
            for (size_t i = 0; i < mul; i++)
            {
                if (G_ES(cs->current_ws,1).name == cs->SearchList.list[cs->SearchList.pos])
                {
                    if (action == 1)
                    {
                        if (cs->SearchList.pos == 0)
                            cs->SearchList.pos = cs->SearchList.size-1;
                        else
                            cs->SearchList.pos--;
                    }
                    else
                    {
                        if (cs->SearchList.pos == cs->SearchList.size-1)
                            cs->SearchList.pos = 0;
                        else
                            cs->SearchList.pos++;
                    }
                }

                for (size_t j = 0; j < G_D(cs->current_ws,1)->size; j++)
                {
                    if (G_D(cs->current_ws,1)->el[j].name == cs->SearchList.list[cs->SearchList.pos])
                    {
                        if (j > G_S(cs->current_ws,1))
                            for (size_t g = G_S(cs->current_ws,1); g < j; g++)
                                move_d(1,cs,cs->current_ws,1);
                        else if (j < G_S(cs->current_ws,1))
                            for (size_t g = G_S(cs->current_ws,1); g > j; g--)
                                move_d(-1,cs,cs->current_ws,1);
                        break;
                    }
                }
            }
            break;
        case 3:
        case 4:
        case 5:
            free(cs->SearchList.list);
            cs->SearchList.list = NULL;
            cs->SearchList.allocated = 0;
            cs->SearchList.size = 0;
            cs->SearchList.pos = 0;
            int reti;

            for (size_t i = 0; i < G_D(cs->current_ws,1)->size; i++)
            {
                if (selected == -1 ? 1 : (G_D(cs->current_ws,1)->el[i].list[cs->current_ws]&selected))
                {
                    if (action == 4 || action == 5)
                    {
                        regex_t regex;
                        reti = 0;
                        reti = regcomp(&regex,temp,0);
                        if (reti) continue;
                        reti = regexec(&regex,G_D(cs->current_ws,1)->el[i].name,0,NULL,action == 5 ? REG_EXTENDED : 0);
                        if (reti) continue;
                        regfree(&regex);
                    }
                    else if (!strstr(G_D(cs->current_ws,1)->el[i].name,temp))
                        continue;

                    if (cs->SearchList.size == cs->SearchList.allocated)
                        cs->SearchList.list = (char**)realloc(cs->SearchList.list,(cs->SearchList.allocated+=cs->SearchList.inc_r)*(sizeof(char*)));
                    cs->SearchList.size++;
                    cs->SearchList.list[cs->SearchList.size-1] = G_D(cs->current_ws,1)->el[i].name;
                }
            }
            ___SEARCH("-n 1",cs);
            break;
    }
    return NULL;
}

char *___SHELL(char *src, Csas *cs)
{
	spawn(s_shell,"-c",src,F_NORMAL|F_WAIT|F_CONFIRM);

	return NULL;
}

char *___FILTER(char *src, Csas *cs)
{
    if (G_D(cs->current_ws,1)->oldsize == 0)
        G_D(cs->current_ws,1)->oldsize = G_D(cs->current_ws,1)->size;
    else
        G_D(cs->current_ws,1)->size = G_D(cs->current_ws,1)->oldsize;

    if (src[0] == '/')
    {
        G_D(cs->current_ws,1)->filter_set = false;
        return NULL;
    }

    G_D(cs->current_ws,1)->filter_set = true;
    if (!G_D(cs->current_ws,1)->filter)
        G_D(cs->current_ws,1)->filter = (char*)malloc(NAME_MAX);

    strcpy(G_D(cs->current_ws,1)->filter,src);

    struct Element temp;

    for (size_t i = 0; i < G_D(cs->current_ws,1)->size; i++)
    {
        if (!strstr(G_D(cs->current_ws,1)->el[i].name,src))
        {
            temp = G_D(cs->current_ws,1)->el[i];
            for (size_t j = i; j < G_D(cs->current_ws,1)->size-1; j++)
                G_D(cs->current_ws,1)->el[j] = G_D(cs->current_ws,1)->el[j+1];
            G_D(cs->current_ws,1)->el[G_D(cs->current_ws,1)->size-1] = temp;
            --G_D(cs->current_ws,1)->size;
            i--;
        }
    }
    return NULL;
}
