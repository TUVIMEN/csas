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
#include "loading.h"
#include "inits.h"
#include "functions.h"
#include "load.h"
#include "useful.h"
#include "preview.h"
#include "draw.h"

extern struct AliasesT aliases[];
extern Settings* settings;

struct command
{
    char* name;
    void (*function)(const char*, Basic*);
};

struct command commands[] = {
    {"move",___MOVE},
    {"fastselect",___FASTSELECT},
    {"cd",___CD},
    {"ChangeWorkSpace",___CHANGEWORKSPACE},
    {"gotop",___GOTOP},
    {"godown",___GODOWN},
    #ifdef __FILE_SIZE_ENABLE__
    {"getsize",___GETSIZE},
    #endif
    {"setgroup",___SETGROUP},
    {"select",___SELECT},
    {"togglevisual",___TOGGLEVISUAL},
    {"f_mod",___F_MOD},
    {"set",___SET},
    {"map",___MAP},
    {"search",___SEARCH},
    {"load",___LOAD},
    {"exec",___EXEC},
    {"quit",___QUIT},
    {"console",___CONSOLE},
    {"bulk",___BULK},
    #ifdef __LOAD_CONFIG_ENABLE__
    {"include",___INCLUDE},
    #endif
    {"shell",___SHELL},
    {"filter",___FILTER},
    {NULL,NULL}
};

void RunCommand(const char* src, Basic* grf)
{
    size_t pos = 0, end = 0;
    
    pos += FindFirstCharacter(src);
    while (src[pos+end] && !isspace(src[pos+end])) end++;

    for (int i = 0; commands[i].name; i++)
    {
        if (end == strlen(commands[i].name) && strncmp(src+pos,commands[i].name,end) == 0)
        {
            pos += end;
            pos += FindFirstCharacter(src+pos);
            (*commands[i].function)(src+pos,grf);
            break;
        }
    }

}

void ConsoleResize(WINDOW* win, const struct WinArgs args)
{
    Vector2i Size = {0,0}, Pos = {0,0};

    if (args.s_size.x != -1)
        Size.x = args.s_size.x;
    if (args.s_size.y != -1)
        Size.y = args.s_size.y;
    if (args.p_size.x > 0)
        Size.x = args.place->_maxx*args.p_size.x+1;
    if (args.p_size.y > 0)
        Size.y = args.place->_maxy*args.p_size.y+1;
    if (args.min_size.x != -1 && Size.x < args.min_size.x)
        Size.x = args.min_size.x;
    if (args.min_size.y != -1 && Size.y < args.min_size.y)
        Size.y = args.min_size.y;
    if (args.max_size.x != -1 && Size.x > args.max_size.x)
        Size.x = args.max_size.x;
    if (args.max_size.y != -1 && Size.y > args.max_size.y)
        Size.y = args.max_size.y;

    if (args.s_pos.x != -1)
        Pos.x = args.s_pos.x;
    if (args.s_pos.y != -1)
        Pos.y = args.s_pos.y;
    if (args.p_pos.x > 0)
        Pos.x = (args.place->_begx+args.place->_maxx)*args.p_pos.x;
    if (args.p_pos.y > 0)
        Pos.y = (args.place->_begy+args.place->_maxy)*args.p_pos.y;
    if (args.min_pos.x != -1 && Pos.x < args.min_pos.x)
        Pos.x = args.min_pos.x;
    if (args.min_pos.y != -1 && Pos.y < args.min_pos.y)
        Pos.y = args.min_pos.y;
    if (args.max_pos.x != -1 && Pos.x > args.max_pos.x)
        Pos.x = args.max_pos.x;
    if (args.max_pos.y != -1 && Pos.y > args.max_pos.y)
        Pos.y = args.max_pos.y;

    wresize(win,Size.y,Size.x);
    mvwin(win,Pos.y,Pos.x);
    refresh();
    if (args.settings&0x1)
        wborder(win,0,0,0,0,0,0,0,0);
    wrefresh(win);
}

void ConsoleGetLine(WINDOW* win, Basic* grf, char** history, size_t size, size_t max, struct WinArgs args, char* first, char* add)
{
    curs_set(1);
    int Event;
    ConsoleResize(win,args);

    short int x = 0, y = 0, off = 0;
    int border = (args.settings&0x1) == 0x1;
    size_t first_t = strlen(first), z = size-1;
    if (add)
    {
        strcpy(history[size-1],add);
        x = strlen(add);
        while (x-off >= win->_maxx) off++;
    }

    char name_complete[NAME_MAX];
    int* name_complete_arr = NULL;
    size_t name_complete_t = 0;
    size_t name_complete_actual = 0;
    bool tab_was_pressed = 0;
    size_t name_n;

    for (;;)
    {
        for (int i = border; i < win->_maxx-border; i++)
            mvwaddch(win,border+y,i,' ');
        mvwaddstr(win,border+y,border,first);
        mvwaddnstr(win,border+y,border+first_t,history[size-1]+off,win->_maxx-border*2-first_t);
        wmove(win,border+y,border+x+first_t-off);
        wrefresh(win);

        switch (Event = getch())
        {
            case -1:
                break;
            case 9:
            {
                name_n = 0;
                while (history[size-1][name_n] && !isspace(history[size-1][name_n]))
                {
                    name_complete[name_n] = history[size-1][name_n];
                    name_n++;
                }
                name_complete[name_n] = 0;

                if (history[size-1][name_n] == 0)
                {
                    if (!tab_was_pressed)
                    {
                        size_t best_match_n = 0, matched_n;
                        for (int i = 0; commands[i].name; i++)
                        {
                            matched_n = 0;
                            for (int j = 0; commands[i].name[j] && name_complete[j] && commands[i].name[j] == name_complete[j]; j++)
                                matched_n++;

                            if (matched_n > best_match_n)
                            {
                                name_complete_t = 0;
                                free(name_complete_arr);
                                name_complete_arr = NULL;
                                best_match_n = matched_n;
                                goto ADD_TO_ARR;
                            }

                            if (matched_n == best_match_n)
                            {
                                ADD_TO_ARR: ;
                                name_complete_arr = (int*)realloc(name_complete_arr,++name_complete_t*sizeof(int));
                                name_complete_arr[name_complete_t-1] = i;
                            }
                        }
                        name_complete_actual = 0;
                        tab_was_pressed = 1;
                    }

                    if (name_complete_t)
                    {
                        strcpy(history[size-1],commands[name_complete_arr[name_complete_actual++]].name);
                        name_complete_actual = name_complete_actual*(name_complete_actual != name_complete_t);
                        x = strlen(history[size-1]);
                        if (name_complete_t == 1)
                        {
                            history[size-1][x++] = ' ';
                            history[size-1][x] = 0;
                            tab_was_pressed = 0;
                        }

                        while (x-off >= win->_maxx) off++;
                    }
                }
            }
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
                    x = strlen(history[size-1]);
                    while (x-off >= win->_maxx) off++;
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
                        x = 0;
                        off = 0;
                        break;
                    }
                    strcpy(history[size-1],history[z]);
                    x = strlen(history[size-1]);
                    while (x-off >= win->_maxx) off++;
                }
                break;
            case ('l'&0x1f):
                werase(win);
                y = 0;
                ConsoleResize(win,args);
                break;
            case ('a'&0x1f):
                x = 0;
                off = 0;
                break;
            case ('e'&0x1f):
                x = strlen(history[size-1]);
                while (x-off >= win->_maxx) off++;
                break;
            case 27:
            case ('r'&0x1f):
                history[size-1][0] = 0;
                goto END;
            case KEY_RESIZE:
                UpdateSizeBasic(grf);
                DrawBasic(grf,-1);
                werase(win);
                ConsoleResize(win,args);
                break;
            case KEY_BACKSPACE:
            case ('h'&0x1f):
                tab_was_pressed = 0;
                if (x > 0)
                {
                    for (size_t i = x-1; i <= strlen(history[size-1]); i++)
                        history[size-1][i] = history[size-1][i+1];
                    x--;
                    if (off != 0) off--;
                }
                else
                    goto END;
                break;
            case ('w'&0x1f):
                tab_was_pressed = 0;
                if (x > 0)
                {
                    do {
                        for (size_t i = x-1; i <= strlen(history[size-1]); i++)
                            history[size-1][i] = history[size-1][i+1];
                        x--;
                        if (off != 0) off--;
                    } while (x != 0 && history[size-1][x-1] != ' ');
                }
                else
                    goto END;
                break;
            case KEY_LEFT:
            case ('b'&0x1f):
                if (x > 0)
                {
                    if (off != 0 && border+x+first_t-off == (size_t)win->_maxx/2)
                        off--;
                    x--;
                }
                break;
            case KEY_RIGHT:
            case ('f'&0x1f):
                if (history[size-1][x])
                {
                    if (border+x+first_t-off >= (size_t)win->_maxx)
                        off++;
                    x++;
                }
                break;
            case ('d'&0x1f):
                while (history[size-1][x] && !isalnum(history[size-1][x])) x++;
                while (isalnum(history[size-1][x])) x++;
                while (history[size-1][x] && !isalnum(history[size-1][x])) x++;
                break;
            case ('s'&0x1f):
                while (history[size-1][x-1] && !isalnum(history[size-1][x-1])) x--;
                while (isalnum(history[size-1][x-1])) x--;
                while (history[size-1][x-1] && !isalnum(history[size-1][x-1])) x--;
                break;
            default:
                tab_was_pressed = 0;
                int i = strlen(history[size-1]);
                history[size-1][i+1] = 0;
                for (; i >= x; i--)
                    history[size-1][i] = history[size-1][i-1];
                history[size-1][x] = (char)Event;
                x++;
                if (border+x+first_t-off >= (size_t)win->_maxx) off++;
                break;
        }
    }

    END: ;

    free(name_complete_arr);

    curs_set(0);
    werase(win);
    wrefresh(win);
}
