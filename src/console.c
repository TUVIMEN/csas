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
#include "expand.h"
#include "load.h"
#include "useful.h"
#include "preview.h"
#include "draw.h"

extern int csas_errno;
extern struct AliasesT aliases[];

extern struct command *commands;
extern size_t commandsl;

int command_run(char *src, Csas *cs)
{
    csas_errno = 0;
    char temp[16384];
    size_t pos = 0, end = 0;
    
    pos += findfirst(src,isspace,-1);
    while (src[pos+end] && !isspace(src[pos+end])) end++;

    bool found = 0;
    for (size_t i = 0; i < commandsl; i++)
    {
        if (end == strlen(commands[i].name) && strncmp(src+pos,commands[i].name,end) == 0)
        {
            found = 1;
            pos += end;
            pos += findfirst(src+pos,isspace,-1);
            if (commands[i].type == 'f')
            {
                size_t j=0,x=0;
                char *c = src+pos;
                while (c[j])
                {
                    if (c[j] == '\'')
                    {
                        temp[x++] = c[j++];
                        size_t e = (strchr(c+j,'\'')-(c+j))+1;
                        memcpy(temp+x,c+j,e);
                        j += e;
                        x += e;
                        continue;
                    }
                    if (c[j] == '~')
                    {
                        j++;
                        char *home = getenv("HOME");
                        size_t t = strlen(home);
                        memcpy(temp+x,home,t);
                        x += t;
                        continue;
                    }
                    temp[x++] = c[j++];
                }
                temp[x] = '\0';
                return ((int (*)(const char*,Csas*))commands[i].func)(temp,cs);
            }
            else if (commands[i].type == 'a')
            {
                sprintf(temp,"%s %s",(char*)commands[i].func,src+pos);
                return command_run(temp,cs);
            }
            break;
        }
    }
    if (!found && src[0])
    {
        csas_errno = CSAS_ECNF;
        return -1;
    }
    return 0;
}

void console_resize(WINDOW *win, const struct WinArgs *args)
{
    int sizex=0,sizey=0,posx=0,posy=0;

    if (args->s_sizex != -1)
        sizex = args->s_sizex;
    if (args->s_sizey != -1)
        sizey = args->s_sizey;
    if (args->p_sizex > 0)
        sizex = args->place->_maxx*args->p_sizex+1;
    if (args->p_sizey > 0)
        sizey = args->place->_maxy*args->p_sizey+1;
    if (args->min_sizex != -1 && sizex < args->min_sizex)
        sizex = args->min_sizex;
    if (args->min_sizey != -1 && sizey < args->min_sizey)
        sizey = args->min_sizey;
    if (args->max_sizex != -1 && sizex > args->max_sizex)
        sizex = args->max_sizex;
    if (args->max_sizey != -1 && sizey > args->max_sizey)
        sizey = args->max_sizey;

    if (args->s_posx != -1)
        posx = args->s_posx;
    if (args->s_posy != -1)
        posy = args->s_posy;
    if (args->p_posx > 0)
        posx = (args->place->_begx+args->place->_maxx)*args->p_posx;
    if (args->p_posy > 0)
        posy = (args->place->_begy+args->place->_maxy)*args->p_posy;
    if (args->min_posx != -1 && posx < args->min_posx)
        posx = args->min_posx;
    if (args->min_posy != -1 && posy < args->min_posy)
        posy = args->min_posy;
    if (args->max_posx != -1 && posx > args->max_posx)
        posx = args->max_posx;
    if (args->max_posy != -1 && posy > args->max_posy)
        posy = args->max_posy;

    wresize(win,sizey,sizex);
    mvwin(win,posy,posx);
    refresh();
    if (args->cfg&0x1)
        wborder(win,0,0,0,0,0,0,0,0);
    wrefresh(win);
}

void console_getline(WINDOW *win, Csas *cs, char **history, size_t size, size_t max, struct WinArgs *args, char *first, char *add,
    int (*expand)(WINDOW *win, char *line, size_t pos, short int off, bool *tab_was_pressed, struct rla *arr, struct WinArgs *args, char **end))
{
    curs_set(1);
    int ev;
    console_resize(win,args);

    short int off = 0;
    int border = (args->cfg&0x1) == 0x1;
    size_t first_t = strlen(first), z = size-1;
    if (add)
    {
        strcpy(history[size-1],add);
        args->x = strlen(add);
        while (args->x-off >= win->_maxx) off++;
    }

    bool tab_was_pressed = 0;
    struct rla ex = {NULL,0,0,NULL};

    console_resize(win,args);

    for (;;)
    {
        for (int i = border; i < win->_maxx-border; i++)
            mvwaddch(win,border+args->y,i,' ');
        mvwaddstr(win,border+args->y,border,first);
        mvwaddnstr(win,border+args->y,border+first_t,history[size-1]+off,win->_maxx-border*2-first_t);
        wmove(win,border+args->y,border+args->x+first_t-off);
        wrefresh(win);

        switch (ev = getch())
        {
            case -1:
                break;
            case '\t':
                if (expand != NULL)
                    expand(win,history[size-1],0,off,&tab_was_pressed,&ex,args,NULL);
                break;
            case 10:
            case '\r':
                goto END;
                break;
            case KEY_UP:
            case ('p'&0x1f):
                tab_was_pressed = 0;
                if (z > 0)
                {
                    z--;
                    strcpy(history[size-1],history[z]);
                    args->x = strlen(history[size-1]);
                    while (args->x-off >= win->_maxx) off++;
                }
                break;
            case KEY_DOWN:
            case ('n'&0x1f):
                tab_was_pressed = 0;
                if (z < size-1)
                {
                    z++;
                    if (z == size-1)
                    {
                        memset(history[size-1],0,max);
                        args->x = 0;
                        off = 0;
                        break;
                    }
                    strcpy(history[size-1],history[z]);
                    args->x = strlen(history[size-1]);
                    while (args->x-off >= win->_maxx) off++;
                }
                break;
            case ('l'&0x1f):
                werase(win);
                args->y = 0;
                console_resize(win,args);
                break;
            case ('a'&0x1f):
                args->x = 0;
                off = 0;
                break;
            case ('e'&0x1f):
                args->x = strlen(history[size-1]);
                while (args->x-off >= win->_maxx) off++;
                break;
            case 27:
            case ('r'&0x1f):
                history[size-1][0] = 0;
                goto END;
            case KEY_RESIZE:
                args->y = 0;
                update_size(cs);
                csas_draw(cs,-1);
                werase(win);
                console_resize(win,args);
                break;
            case KEY_BACKSPACE:
            case ('h'&0x1f):
                tab_was_pressed = 0;
                if (args->x > 0)
                {
                    for (size_t i = args->x-1, j = strlen(history[size-1]); i <= j; i++)
                        history[size-1][i] = history[size-1][i+1];
                    args->x--;
                    if (off != 0) off--;
                }
                else
                    goto END;
                break;
            case ('w'&0x1f):
                tab_was_pressed = 0;
                if (args->x > 0)
                {
                    do {
                        for (size_t i = args->x-1, j = strlen(history[size-1]); i <= j; i++)
                            history[size-1][i] = history[size-1][i+1];
                        args->x--;
                        if (off != 0) off--;
                    } while (args->x != 0 && history[size-1][args->x-1] != ' ');
                }
                else
                    goto END;
                break;
            case KEY_LEFT:
            case ('b'&0x1f):
                if (args->x > 0)
                {
                    if (off != 0 && border+args->x+first_t-off == (size_t)win->_maxx/2)
                        off--;
                    args->x--;
                }
                break;
            case KEY_RIGHT:
            case ('f'&0x1f):
                if (history[size-1][args->x])
                {
                    if (border+args->x+first_t-off >= (size_t)win->_maxx)
                        off++;
                    args->x++;
                }
                break;
            case ('d'&0x1f):
                while (history[size-1][args->x] && !isalnum(history[size-1][args->x])) args->x++;
                while (isalnum(history[size-1][args->x])) args->x++;
                while (history[size-1][args->x] && !isalnum(history[size-1][args->x])) args->x++;
                break;
            case ('s'&0x1f):
                while (history[size-1][args->x-1] && !isalnum(history[size-1][args->x-1])) args->x--;
                while (isalnum(history[size-1][args->x-1])) args->x--;
                while (history[size-1][args->x-1] && !isalnum(history[size-1][args->x-1])) args->x--;
                break;
            default:
                tab_was_pressed = 0;
                int i = strlen(history[size-1]);
                history[size-1][i+1] = 0;
                for (; i >= args->x; i--)
                    history[size-1][i] = history[size-1][i-1];
                history[size-1][args->x] = (char)ev;
                args->x++;
                if (border+args->x+first_t-off >= (size_t)win->_maxx) off++;
                break;
        }
    }

    END: ;
    if (ex.sfree)
        ex.sfree(&ex);
    curs_set(0);
    werase(win);
    wrefresh(win);
}
