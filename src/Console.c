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
#include "Loading.h"
#include "Functions.h"
#include "Chars.h"
#include "Load.h"
#include "Usefull.h"
#include "FastRun.h"
#include "Draw.h"

extern struct AliasesT aliases[];
extern Settings* settings;

struct FunctTab
{
    char* name;
    void (*function)(const char*, Basic*);
};

struct FunctTab FunctionsTable[] = {
    {"move",&___MOVE},
    {"fastselect",&___FASTSELECT},
    {"cd",&___CD},
    {"ChangeWorkSpace",&___CHANGEWORKSPACE},
    {"gotop",&___GOTOP},
    {"godown",&___GODOWN},
    {"getsize",&___GETSIZE},
    {"setgroup",&___SETGROUP},
    {"select",&___SELECT},
    {"togglevisual",&___TOGGLEVISUAL},
    {"f_mod",&___F_MOD},
    {"set",&___SET},
    {"map",&___MAP},
    {"search",&___SEARCH},
    {"load",&___LOAD},
    {"exec",&___EXEC},
    {"quit",&___QUIT},
    {"console",&___CONSOLE},
    {"bulk",&___BULK},
    #ifdef __LOAD_CONFIG_ENABLE__
    {"include",&___INCLUDE},
    #endif
    {"shell",&___SHELL},
    {"filter",&___FILTER},
    {NULL,NULL}
};

void RunCommand(const char* src, Basic* grf)
{
    size_t pos = 0, end = 0;

    pos += FindFirstCharacter(src);
    while (src[pos+end] && !isspace(src[pos+end])) end++;

    for (int i = 0; FunctionsTable[i].name; i++)
    {
        if (end == strlen(FunctionsTable[i].name) && strncmp(src+pos,FunctionsTable[i].name,end) == 0)
        {
            pos += end;
            pos += FindFirstCharacter(src+pos);
            (*FunctionsTable[i].function)(src+pos,grf);
            break;
        }
    }

}

void ConsoleResize(WINDOW* win, const struct WinArgs args)
{
    Vector2i Size = {0,0}, Pos = {0,0};

    if (args.S_Size.x != -1)
        Size.x = args.S_Size.x;
    if (args.S_Size.y != -1)
        Size.y = args.S_Size.y;
    if (args.PercentSize.x > 0)
        Size.x = args.place->_maxx*args.PercentSize.x+1;
    if (args.PercentSize.y > 0)
        Size.y = args.place->_maxy*args.PercentSize.y+1;
    if (args.MinSize.x != -1 && Size.x < args.MinSize.x)
        Size.x = args.MinSize.x;
    if (args.MinSize.y != -1 && Size.y < args.MinSize.y)
        Size.y = args.MinSize.y;
    if (args.MaxSize.x != -1 && Size.x > args.MaxSize.x)
        Size.x = args.MaxSize.x;
    if (args.MaxSize.y != -1 && Size.y > args.MaxSize.y)
        Size.y = args.MaxSize.y;

    if (args.S_Pos.x != -1)
        Pos.x = args.S_Pos.x;
    if (args.S_Pos.y != -1)
        Pos.y = args.S_Pos.y;
    if (args.PercentPos.x > 0)
        Pos.x = (args.place->_begx+args.place->_maxx)*args.PercentPos.x;
    if (args.PercentPos.y > 0)
        Pos.y = (args.place->_begy+args.place->_maxy)*args.PercentPos.y;
    if (args.MinPos.x != -1 && Pos.x < args.MinPos.x)
        Pos.x = args.MinPos.x;
    if (args.MinPos.y != -1 && Pos.y < args.MinPos.y)
        Pos.y = args.MinPos.y;
    if (args.MaxPos.x != -1 && Pos.x > args.MaxPos.x)
        Pos.x = args.MaxPos.x;
    if (args.MaxPos.y != -1 && Pos.y > args.MaxPos.y)
        Pos.y = args.MaxPos.y;

    wresize(win,Size.y,Size.x);
    mvwin(win,Pos.y,Pos.x);
    refresh();
    if (args.settings&0x1)
        wborder(win,0,0,0,0,0,0,0,0);
    wrefresh(win);
}

void ConsoleGetLine(WINDOW* win, Basic* grf, char** History, size_t size, size_t max, struct WinArgs args, char* first, char* add)
{
    curs_set(1);
    int Event;
    ConsoleResize(win,args);

    short int x = 0, y = 0, off = 0;
    int border = (args.settings&0x1) == 0x1;
    size_t first_t = strlen(first), z = size-1;
    if (add)
    {
        strcpy(History[size-1],add);
        x = strlen(add);
        while (x-off >= win->_maxx) off++;
    }

    for (;;)
    {
        for (int i = border; i < win->_maxx-border; i++)
            mvwaddch(win,border+y,i,' ');
        mvwaddstr(win,border+y,border,first);
        mvwaddnstr(win,border+y,border+first_t,History[size-1]+off,win->_maxx-border*2-first_t);
        wmove(win,border+y,border+x+first_t-off);
        wrefresh(win);

        switch (Event = getch())
        {
            case -1:
                break;
            case 10:
                goto END;
                break;
            case KEY_UP:
            case ('p'&0x1f):
                if (z > 0)
                {
                    z--;
                    strcpy(History[size-1],History[z]);
                    x = strlen(History[size-1]);
                    while (x-off >= win->_maxx) off++;
                }
                break;
            case KEY_DOWN:
            case ('n'&0x1f):
                if (z < size-1)
                {
                    z++;
                    if (z == size-1)
                    {
                        memset(History[size-1],0,max);
                        x = 0;
                        off = 0;
                        break;
                    }
                    strcpy(History[size-1],History[z]);
                    x = strlen(History[size-1]);
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
                x = strlen(History[size-1]);
                while (x-off >= win->_maxx) off++;
                break;
            case 27:
            case ('r'&0x1f):
                goto END;
            case KEY_RESIZE:
                UpdateSizeBasic(grf);
                DrawBasic(grf,-1);
                werase(win);
                ConsoleResize(win,args);
                break;
            case KEY_BACKSPACE:
            case ('h'&0x1f):
                if (x > 0)
                {
                    for (size_t i = x-1; i <= strlen(History[size-1]); i++)
                        History[size-1][i] = History[size-1][i+1];
                    x--;
                    if (off != 0)
                        off--;
                }
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
                if (History[size-1][x])
                {
                    if (border+x+first_t-off >= (size_t)win->_maxx)
                        off++;
                    x++;
                }
                break;
            case ('d'&0x1f):
                while (History[size-1][x] && !isalnum(History[size-1][x])) x++;
                while (isalnum(History[size-1][x])) x++;
                while (History[size-1][x] && !isalnum(History[size-1][x])) x++;
                break;
            case ('s'&0x1f):
                while (History[size-1][x-1] && !isalnum(History[size-1][x-1])) x--;
                while (isalnum(History[size-1][x-1])) x--;
                while (History[size-1][x-1] && !isalnum(History[size-1][x-1])) x--;
                break;
            default:
                for (int i = strlen(History[size-1]); i >= x; i--)
                    History[size-1][i] = History[size-1][i-1];
                History[size-1][x] = (char)Event;
                x++;
                if (border+x+first_t-off >= (size_t)win->_maxx)
                    off++;
                break;
        }
    }

    END: ;

    curs_set(0);
    werase(win);
    wrefresh(win);
}
