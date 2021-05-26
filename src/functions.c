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
#include "expand.h"

extern int csas_errno;
extern Key *keys;
extern size_t keys_t;
extern struct option cfg_names[];
extern struct command *commands;
extern size_t commandsl;

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
extern li s_C_Error;

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

static void move_d(li n, Csas *cs, const int ws, const int which)
{
    li f = n;
    uchar ff = 0;
    if (f < 0)
    {
        f *= -1;
        ff |= 2;
    }
    else
        ff |= 4;
    if (s_WrapScroll && f > (li)G_D(ws,which)->size-1)
        ff |= 1;
    if (f == 1)
        ff = 0;
    f = G_S(ws,which);
    if ((li)(n+G_S(ws,which)) < 0)
    {
        if (!s_WrapScroll)
        {
            if (G_S(ws,which) != 0)
                gotop(cs,ws,which);
            return;
        }
        n = G_D(ws,which)->size+G_S(ws,which)+n;
    }
    else
    {
        n += G_S(ws,which);
        if (s_WrapScroll)
            n %= G_D(ws,which)->size;
    }

    if (n > (li)G_D(ws,which)->size-1)
        n =  G_D(ws,which)->size-1;

    G_S(ws,which) = n;

    li maxy = cs->win[which]->_maxy;
    if (s_Borders)
        maxy -= 2;
    int t = G_D(ws,which)->ltop[ws], moff = maxy*s_MoveOffSet;
    if (maxy >= (li)G_D(ws,which)->size-1 || n < moff)
    {
        t = 0;
        goto END;
    }

    if (n >= (li)G_D(ws,which)->size-moff)
    {
        t = G_D(ws,which)->size-maxy-1;
        goto END;
    }

    if (n > t+maxy-moff && n < (li)G_D(ws,which)->size-moff)
    {
        t = n-maxy+moff;
        if (s_JumpScroll)
            t += maxy*s_JumpScrollValue;
        goto END;
    }

    if (n < t+moff && n > moff-1)
    {
        t = n-moff;
        if (s_JumpScroll)
            t -= maxy*s_JumpScrollValue;
        goto END;
    }

    END: ;
    G_D(ws,which)->ltop[ws] = t;
    if (cs->ws[ws].visual)
    {
        if (ff == 0)
            G_ES(ws,which).list[ws] |= 1<<cs->ws[ws].sel_group;
        else if (ff&1)
            for (size_t i = 0; i < G_D(ws,which)->size; i++)
                G_D(ws,which)->el[i].list[ws] |= 1<<cs->ws[ws].sel_group;
        else if (ff&2)
            for (li i = n; i < f; i++)
                G_D(ws,which)->el[i].list[ws] |= 1<<cs->ws[ws].sel_group;
        else if (ff&4)
            for (li i = f; i < n+1; i++)
                G_D(ws,which)->el[i].list[ws] |= 1<<cs->ws[ws].sel_group;
    }
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

    register size_t i;
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
        move_d(i-G_S(ws,which),cs,ws,which);
    else if (i < G_S(ws,which))
        move_d(G_S(ws,which)-i,cs,ws,which);
}

static char *bulk(Csas *cs, const int ws, const int selected, char **args, const uchar flag)
{
    char tmpfile[PATH_MAX];
    strcpy(tmpfile,TEMPTEMP);
    int fd = mkstemp(tmpfile);
    FILE *file = fdopen(fd,"w+");
    if (!file)
        return NULL;

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

        comment_write = flag&0x4;
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

                    if (!(flag&0x4) && filecopy[pos] == '/' && filecopy[pos+1] && filecopy[pos+1] == '/')
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
    return NULL;
}


int cmd_alias(char *src, Csas *cs)
{
    csas_errno = 0;
    size_t pos = 0;

    char name[NAME_MAX],temp[8192];
    pos += atop(name,src,' ',cs);
    pos += findfirst(src+pos,isspace,-1);
    size_t s = atop(temp,src+pos,' ',cs);
    temp[s] = 0;
    addcommand(name,'a',temp,s+1,NULL);
    return 0;
}

int cmd_rename(char *src, Csas *cs)
{
    csas_errno = 0;
    struct WinArgs args = {stdscr,0,0,-1,1,1,-1,-1,-1,-1,-1,-1,-1,-1,1,-1,-1,-1,-1,0};
    char name[NAME_MAX];
    char *n = name;
    memcpy(name,G_ES(cs->current_ws,1).name,G_ES(cs->current_ws,1).nlen);
    console_getline(cs->win[5],cs,&n,1,0,&args,"rename ",G_ES(cs->current_ws,1).name,NULL);
    return rename(G_ES(cs->current_ws,1).name,n);
}

int cmd_set(char *src, Csas *cs)
{
    csas_errno = 0;
    size_t pos = 0;

    size_t end = 0;
    while (src[pos+end] && !isspace(src[pos+end])) end++;
    int gga = -1;

    for (int i = 0;cfg_names[i].n; i++)
        if (strlen(cfg_names[i].n) == end && strncmp(src+pos,cfg_names[i].n,end) == 0) { gga = i; break; }

    if (gga == -1)
    {
        csas_errno = CSAS_ENOP;
        return -1;
    }

    pos += end;
    pos += findfirst(src+pos,isspace,-1);

    return atov(cfg_names[gga].v,src+pos,&pos,cs,cfg_names[gga].t);
}

#ifdef __LOAD_CONFIG_ENABLE__
int cmd_source(char *src, Csas *cs)
{
    csas_errno = 0;
    char temp[8192];

    atop(temp,src,' ',cs);
    config_load(temp,cs);
    return 0;
}
#endif

int cmd_map(char *src, Csas *cs)
{
    csas_errno = 0;
    size_t pos = 0;
    char temp1[64], temp2[PATH_MAX];
    while (src[pos] && !isspace(src[pos])) pos++;
    strncpy(temp1,src,pos);
    temp1[pos] = '\0';

    pos += atop(temp2,src+pos,' ',cs);

    addkey(temp1,temp2);
    return 0;
}

int cmd_move(char *src, Csas *cs)
{
    csas_errno = 0;
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
                            pos += findfirst(src+pos,isspace,-1);
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
        return 0;

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
                move_d(rot*(mul2+(mul2 == 0))*mul1,cs,ws,1);
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
    return 0;
}

int cmd_quit(char *src, Csas *cs)
{
    csas_errno = 0;
    bool force = false;
    if (src[0] && src[0] == '-' && src[1] && src[1] == 'f')
        force = true;
    csas_exit(cs,force);
    return 0;
}

int cmd_cd(char *src, Csas *cs)
{
    csas_errno = 0;
    size_t pos = 0;
    int ws = cs->current_ws;
    char path[8192];
    memset(path,0,8191);

    while (src[pos])
    {
        if (src[pos] == '-' && src[pos+1] && src[pos+1] == 'w')
        {
            pos += 2;
            pos += findfirst(src+pos,isspace,-1);
            ws = atoi(src+pos);
            while (isdigit(src[pos])) pos++;
        }
        else
            pos += atop(path,src+pos,' ',cs);

        pos += findfirst(src+pos,isspace,-1);
    }

    cs->ws[ws].exists = true;
    change_workspace(cs,ws);

    return csas_cd(path,ws,cs);
}

int cmd_gotop(char *src, Csas *cs)
{
    csas_errno = 0;
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
                        {
                        char t = src[pos];
                        pos++;
                        pos += findfirst(src+pos,isspace,-1);
                        if (t == 't')
                            target = atol(src+pos);
                        else
                            ws = atoi(src+pos);
                        }
                        while (isdigit(src[pos])) pos++;
                        break;
                }
            } while (src[pos] && !isspace(src[pos]));
        }

        pos += findfirst(src+pos,isspace,-1);
        if (src[pos+1] == '\0')
            break;
    }

    if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        !G_D(ws,1)->enable &&
        #endif
        G_D(ws,1)->size > 0)
    {
        gotop(cs,ws,1);
        
        if (target > G_S(ws,1))
            move_d((target-1)-G_S(ws,1),cs,ws,1);
        else if (target < G_S(ws,1))
            move_d(G_S(ws,1)-(target-1),cs,ws,1);
        if (ws == cs->current_ws && s_Win3Enable)
            get_preview(cs);
    }
    return 0;
}

int cmd_godown(char *src, Csas *cs)
{
    csas_errno = 0;
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
                        {
                        char t = src[pos];
                        pos++;
                        pos += findfirst(src+pos,isspace,-1);
                        if (t == 't')
                            target = atol(src+pos);
                        else
                            ws = atoi(src+pos);
                        }
                        while (isdigit(src[pos])) pos++;
                        break;
                }
            } while (src[pos] && !isspace(src[pos]));
        }

        pos += findfirst(src+pos,isspace,-1);
        if (src[pos+1] == '\0')
            break;
    }

    if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        !G_D(ws,1)->enable &&
        #endif
        G_D(ws,1)->size > 0)
    {
        godown(cs,ws,1);
        if (target > G_S(ws,1))
            move_d((target-1)-G_S(ws,1),cs,ws,1);
        else if (target < G_S(ws,1))
            move_d(G_S(ws,1)-(target-1),cs,ws,1);
        if (ws == cs->current_ws && s_Win3Enable)
            get_preview(cs);
    }
    return 0;
}

int cmd_change_workspace(char *src, Csas *cs)
{
    csas_errno = 0;
    change_workspace(cs,atoi(src));
    return 0;
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

int cmd_getsize(char *src, Csas *cs)
{
    csas_errno = 0;
    size_t pos = 0;
    uchar flag = D_S;
    int selected = -1, ws = cs->current_ws;
    char *path = (char*)calloc(sizeof(char),PATH_MAX);

    while (src[pos])
    {
        pos += findfirst(src+pos,isspace,-1);

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
                            pos += findfirst(src+pos,isspace,-1);
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
            pos += atop(path,src+pos,' ',cs);

        if (src[pos+1] == '\0')
            break;
    }

    if (!cs->ws[cs->current_ws].exists)
    {
        free(path);
        return 0;
    }

    if (ws != cs->current_ws)
		if (chdir(G_D(ws,1)->path) != 0)
			return -1;

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
            return 0;
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
    return 0;
}

#endif

char *___SETGROUP(char *src, Csas *cs)
{
    csas_errno = 0;
    cs->ws[cs->current_ws].sel_group = atoi(src);
    if (cs->ws[cs->current_ws].visual && G_D(cs->current_ws,1)->size > 0)
        G_ES(cs->current_ws,1).list[cs->current_ws] |= 1<<cs->ws[cs->current_ws].sel_group;
    return 0;
}

int cmd_fastselect(char *src, Csas *cs)
{
    csas_errno = 0;
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
    return 0;
}

int cmd_open_with(char *src, Csas *cs)
{
    csas_errno = 0;
    if (G_D(cs->current_ws,1)->size == 0)
        return 0;
    char temp[4096];
    size_t t1 = findfirst(src,isspace,-1),
        t2 = findfirst(src+t1,isalnum,-1);
    memcpy(temp,src+t1,t2);
    temp[t2] = 0;
    return spawn(temp,G_ES(cs->current_ws,1).name,NULL,F_NORMAL|F_WAIT);
}

int cmd_togglevisual(char *src, Csas *cs)
{
    csas_errno = 0;
    cs->ws[cs->current_ws].visual = !cs->ws[cs->current_ws].visual;
    if (cs->ws[cs->current_ws].visual && G_D(cs->current_ws,1)->size > 0)
        G_ES(cs->current_ws,1).list[cs->current_ws] |= 1<<cs->ws[cs->current_ws].sel_group;
    return 0;;
}

int cmd_f_mod(char *src, Csas *cs)
{
    csas_errno = 0;
    uchar Action = 0;
    mode_t arg = 0;
    size_t pos = 0;

    switch (src[0])
    {
        case 'd': case 'D': Action = 1; break;
        case 'c': case 'C': Action = 2; break;
        case 'm': case 'M': Action = 3; break;
        default: return 0;
    }

    pos++;
    pos += findfirst(src+pos,isspace,-1);

    int selected = -1, ws = cs->current_ws;
    char *path = (char*)calloc(sizeof(char),PATH_MAX);
    char *target = (char*)calloc(sizeof(char),PATH_MAX);
    int fd1, fd2, fd3;
    char *buffer = Action == 1 ? NULL : (char*)malloc(s_CopyBufferSize);

    while (src[pos])
    {
        pos += findfirst(src+pos,isspace,-1);

        if (src[pos] == '-')
        {
            do {
                pos++;
                switch (src[pos])
                {
                    case 'o':
                        pos++;
                        pos += atop(target,src+pos,' ',cs);
                        break;
                    case 'w':
                    case 's':
                        {
                            int itemp = (src[pos] == 'w' ? 1 : 0);
                            pos++;
                            pos += findfirst(src+pos,isspace,-1);
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
            pos += atop(path,src+pos,' ',cs);

        if (src[pos+1] == '\0')
            break;
    }

    if (!cs->ws[cs->current_ws].exists)
        goto END;

    if (ws != cs->current_ws)
        if (chdir(G_D(ws,1)->path) != 0)
			return -1;

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
    return 0;
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

int cmd_load(char *src, Csas *cs)
{
    csas_errno = 0;
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
                    case 'm': //mode
                        pos++;
                        pos += findfirst(src+pos,isspace,-1);
                        mode += atoi(src+pos);
                        while (isdigit(src[pos])) pos++;
                        break;
                    #ifdef __THREADS_FOR_DIR_ENABLE__
                    case 't': flag |= 0x1; break; //threads
                    #endif
                    case 'R': flag |= 0x2; break; //recursive
                }

            } while (src[pos] && !isspace(src[pos]));
        }

        if (src[pos+1] == '\0')
            break;
        else
            pos += findfirst(src+pos,isspace,-1);
    }

    int r = getdir(".",cs,cs->current_ws,1,mode
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
    return r;
}

int cmd_select(char *src, Csas *cs)
{
    csas_errno = 0;
    size_t pos = 0;
    int mode = 1;
    bool recursive = 0;
    int selected = -1, workspace1 = cs->current_ws, workspace2 = cs->current_ws, toselected = -1;
    char *path = (char*)calloc(sizeof(char),PATH_MAX);

    while (src[pos])
    {
        pos += findfirst(src+pos,isspace,-1);

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
                        pos+= findfirst(src+pos,isspace,-1);
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
                            pos += findfirst(src+pos,isspace,-1);
                            if (itemp)
                                workspace1 = atoi(src+pos);
                            else
                                workspace2 = atoi(src+pos);
                            while (isdigit(src[pos])) pos++;
                        }
                        break;
                    case 's':
                        pos++;
                        pos += findfirst(src+pos,isspace,-1);
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
            pos += atop(path,src+pos,' ',cs);

        if (src[pos+1] == '\0')
            break;
    }

    if (!cs->ws[cs->current_ws].exists)
    {
        free(path);
        return 0;
    }

    if (workspace1 != cs->current_ws)
        if (chdir(G_D(workspace1,1)->path) != 0)
			return -1;

    if (selected == -2)
        selected = cs->ws[workspace1].sel_group;
    if (toselected == -1)
        toselected = cs->ws[workspace2].sel_group;
    if (selected == -3)
    {
        if (!recursive)
        {
            free(path);
            return 0;
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
    return 0;
}

int cmd_exec(char *src, Csas *cs)
{
    csas_errno = 0;
    size_t pos = 0;
    uchar flag = F_MULTI;
    char temp[LINE_SIZE_MAX];

    while (src[pos])
    {
        if (src[pos] == '-')
        {
            do {
                pos++;

                switch (src[pos])
                {
                    case 's': flag |= F_SILENT; break;
                    case 'n': flag |= F_NORMAL; break;
                    case 'c': flag |= F_CONFIRM; break;
                    case 'w': flag |= F_WAIT; break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }
        else
        {
            atop(temp,src+pos,'\0',cs);
            break;
        }

        pos += findfirst(src+pos,isspace,-1);
    }

    return spawn(temp,NULL,NULL,flag);
}

int cmd_bulk(char *src, Csas *cs)
{
    csas_errno = 0;
    size_t pos = 0;
    int ws = cs->current_ws, selected = -1;
    uchar flag = 0;
    char *temp[6];
    for (int i = 0; i < 6; i++)
        temp[i] = (char*)calloc(sizeof(char),PATH_MAX);
    char path[PATH_MAX] = ".";

    while (src[pos])
    {
        if (src[pos] && src[pos] == '-')
        {
            do {
                pos++;
                switch (src[pos])
                {
                    case 'n': flag |= 0x4; break; //no path in comment
                    case 'N': flag &= ~0x4; break;
                    case 'f': flag |= 0x1; break; //full path
                    case 'w':
                        pos++;
                        pos += findfirst(src+pos,isspace,-1);
                        ws = atoi(src+pos);
                        if (ws == -1) ws = cs->current_ws;
                        if (src[pos] == '-') pos++;
                        while (isdigit(src[pos])) pos++;
                        break;
                    case 's':
                        pos++;
                        pos += findfirst(src+pos,isspace,-1);
                        if (src[pos] == '-')
                        {
                            selected = -1;
                            pos++;
                        } else if (src[pos] == 's')
                        {
                            selected = 1<<cs->ws[ws].sel_group;
                            pos++;
                        }
                        else
                        {
                            selected = 1<<atoi(src+pos);
                            while (isdigit(src[pos])) pos++;
                        }
                        break;
                    case 'R': flag |= 0x2; break; //recursive
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
                            pos += atop(temp[itemp],src+pos,' ',cs);
                        }
                        break;
                }

            } while (src[pos] && !isspace(src[pos]));
        }
        else
            pos += atop(path,src+pos,' ',cs);
        pos++;
    }

    if (!cs->ws[ws].exists)
        return 0;

    if (cs->current_ws != ws)
        if (chdir(G_D(ws,1)->path) != 0)
			return -1;

    if (path[0])
        realpath(path,temp[0]);
    bulk(cs,ws,selected,temp,flag);
    for (int i = 0; i < 6; i++)
        free(temp[i]);
    if (cs->current_ws != ws)
        chdir(G_D(ws,1)->path);
    return 0;
}

int cmd_console(char *src, Csas *cs)
{
    csas_errno = 0;
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
                    case 'a':
                        pos++;
                        pos += atop(add_text,src+pos,' ',cs);
                        break;
                    case 'f':
                        pos++;
                        pos += atop(first_text,src+pos,' ',cs);
                        break;
                    case 'b': flags |= 1; break; //borders
                    case 'n':
                        pos++;
                        pos += findfirst(src+pos,isspace,-1);
                        n = atoi(src+pos);
                        break;
                    case 's':
                        pos++;
                        pos += findfirst(src+pos,isspace,-1);
                        if (src[pos] != 'x')
                        {
                            size_t end = findfirst(src+pos,isdigit,-1);
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
                            size_t end = findfirst(src+pos,isdigit,-1);
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
                        pos += findfirst(src+pos,isspace,-1);
                        if (src[pos] != 'x')
                        {
                            size_t end = findfirst(src+pos,isdigit,-1);
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
                            size_t end = findfirst(src+pos,isdigit,-1);
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
        return 0;

    args.cfg = flags;

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

        console_getline(cs->win[5],cs,cs->consolehistory.history,cs->consolehistory.size,cs->consolehistory.alloc_r-1,&args,first_text,add_text[0] ? add_text : NULL,expand_commands);
        char *line = cs->consolehistory.history[cs->consolehistory.size-1];
        if (strcmp(line,"exit") == 0)
            break;
        int r = command_run(line,cs);
        if (r != 0)
        {
            char *fc = strchr(line,' ');
            if (fc)
                set_message(cs,s_C_Error,"%s: %s",fc,csas_strerror());
            else
                set_message(cs,s_C_Error,"%s",csas_strerror());
        }
        if (n != -1)
            n--;
        args.y++;
    }
    return 0;
}

int cmd_search(char *src, Csas *cs)
{
    csas_errno = 0;
    if (
        #ifdef __THREADS_FOR_DIR_ENABLE__
        G_D(cs->current_ws,1)->enable ||
        #endif
        G_D(cs->current_ws,1)->size < 1)
        return 0;
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
                        pos += findfirst(src+pos,isspace,-1);
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
                        pos += findfirst(src+pos,isspace,-1);
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
                        pos += atop(temp,src+pos,' ',cs);
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
                        move_d(j-G_S(cs->current_ws,1),cs,cs->current_ws,1);
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
            cmd_search("-n 1",cs);
            break;
    }
    return 0;
}

int cmd_filter(char *src, Csas *cs)
{
    csas_errno = 0;
    if (G_D(cs->current_ws,1)->oldsize == 0)
        G_D(cs->current_ws,1)->oldsize = G_D(cs->current_ws,1)->size;
    else
        G_D(cs->current_ws,1)->size = G_D(cs->current_ws,1)->oldsize;

    if (src[0] == '/')
    {
        G_D(cs->current_ws,1)->filter_set = false;
        return 0;
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
    return 0;
}
