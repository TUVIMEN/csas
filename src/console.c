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
#include "console.h"

void
console_getline(char **history, size_t size, char *first, char *add, li offset, csas *cs,
    int (*expand)(char *line, size_t pos, size_t *size, uchar *tabp, flexarr *arg, uchar *free_names, csas *cs))
{
    wint_t ev[2];
    int r;
    wchar_t line[LLINE_MAX],wfirst[64];
    size_t firstl,current_line=size-1,s,off=0,x=0;
    firstl = mbstowcs(wfirst,first,63);
    s = mbstowcs(line,history[current_line],LLINE_MAX-1);
    uchar tabp=0,free_names=0;

    curs_set(1);

    if (offset < 0)
        offset = 0;
    if (add) {
        s = mbstowcs(line+s,add,LLINE_MAX-s-1);
        if (!offset)
            x = s;
    }
    if ((size_t)offset > s)
        offset = s;
    if (offset)
        x = offset;
    if (x-off >= (size_t)COLS)
        off = COLS-(x-off);

    flexarr *arg = flexarr_init(sizeof(void*),DIR_INCR);;

    for (;;) {
        mvhline(LINES-1,0,' ',COLS);
        mvaddnwstr(LINES-1,0,wfirst,firstl);
        addnwstr(line+off,COLS-firstl);
        move(LINES-1,x+firstl-off);
        refresh();

        r = getinput_wch(ev,cs);
        if (r != OK)
            continue;
        if (tabp && *ev != '\t')
            tabp = 0;
        switch (*ev) {
            case -1: break;
            case '\t':
                if (expand) {
                    r = wcstombs(history[current_line],line,LLINE_MAX-1);
                    if (r >= LLINE_MAX-1)
                        history[current_line][LLINE_MAX-1] = 0;
                    expand(history[current_line],0,&x,&tabp,arg,&free_names,cs);
                    s = x = mbstowcs(line,history[current_line],LLINE_MAX);
                    /*if (tabp)
                        x = wcslen(history[current_line]);*/
                }
                break;
            case 10:
            case '\r':
                goto END;
                break;
            case KEY_UP:
            case ('p'&0x1f):
                if (current_line > 0 && current_line) {
                    x = s = mbstowcs(line,history[--current_line],LLINE_MAX);
                }
                break;
            case KEY_DOWN:
            case ('n'&0x1f):
                if (current_line < size-1) {
                    current_line++;
                    if (current_line == size-1) {
                        line[0] = 0;
                        x = 0;
                        off = 0;
                        s = 0;
                        break;
                    }
                    x = s = mbstowcs(line,history[current_line],LLINE_MAX);
                }
                break;
            case ('a'&0x1f):
                x = 0;
                off = 0;
                break;
            case ('e'&0x1f):
                x = s;
                if (s > COLS-firstl-1)
                    off = s-COLS+firstl+1;
                break;
            case ESC:
            case ('r'&0x1f):
                line[0] = 0;
                goto END;
            case KEY_BACKSPACE:
            case ('h'&0x1f):
                if (s == 0)
                    goto END;
                delwc(line,--x,s--);
                if (off != 0)
                    off--;
                break;
            case ('w'&0x1f):
                if (x <= 0)
                    goto END;
                do {
                    delwc(line,--x,s--);
                    if (off != 0)
                        off--;
                } while (x != 0 && line[x-1] != ' ');
                break;
            case KEY_LEFT:
            case ('b'&0x1f):
                if (x > 0) {
                    if (off != 0 && x+firstl-off == (size_t)COLS>>1)
                        off--;
                    x--;
                }
                break;
            case KEY_RIGHT:
            case ('f'&0x1f):
                if (x < s) {
                    if (s > COLS-firstl-1 && off != s-COLS+firstl+1 && x+firstl-off == (size_t)COLS>>1)
                        off++;
                    x++;
                }
                break;
            case ('d'&0x1f):
                if (s == 0 || x > s-1)
                    break;
                delwc(line,x,s--);
                break;
            default:
                line[++s] = 0;
                for (size_t i = s-1; i > x; i--)
                    line[i] = line[i-1];
                line[x++] = *ev;
                if (x+firstl-off >= (size_t)COLS)
                    off++;
                break;
        }
    }

    END: ;
    if (arg->v) {
        if (free_names)
            for (size_t i = 0; i < arg->size; i++)
                free(((char**)arg->v)[i]);
        flexarr_free(arg);
    }
    curs_set(0);
    r = wcstombs(history[size-1],line,LLINE_MAX-1);
    if (r >= LLINE_MAX-1)
        history[size-1][LLINE_MAX-1] = 0;
}
