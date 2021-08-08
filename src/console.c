#include "main.h"
#include "useful.h"
#include "console.h"

void
console_getline(char **history, size_t size, char *first, char *add, csas *cs)
{
    int ev;
    size_t firstl=strlen(first),current_line=size-1,s,off=0,x=0;
    char *h=history[current_line];
    s = strlen(h);

    curs_set(1);

    if (add) {
        strcpy(h,add);
        x = s = strlen(add);
        if (x-off >= (size_t)COLS)
            off = COLS-(x-off);
    }

    for (;;) {
        mvhline(LINES-1,0,' ',COLS);
        mvaddnstr(LINES-1,0,first,firstl);
        addnstr(h+off,COLS-firstl);
        move(LINES-1,x+firstl-off);
        refresh();

        switch (ev = getinput(cs)) {
            case -1: break;
            case '\t': break;
            case 10:
            case '\r':
                goto END;
                break;
            case KEY_UP:
            case ('p'&0x1f):
                if (current_line > 0) {
                    x = s = strlen(h);
                    memcpy(h,history[--current_line],x+1);
                }
                break;
            case KEY_DOWN:
            case ('n'&0x1f):
                if (current_line < size-1) {
                    current_line++;
                    if (current_line == size-1) {
                        h[0] = 0;
                        x = 0;
                        off = 0;
                        s = 0;
                        break;
                    }
                    s = x = strlen(h);
                    memcpy(h,history[current_line],x+1);
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
            case 27:
            case ('r'&0x1f):
                h[0] = 0;
                goto END;
            case KEY_BACKSPACE:
            case ('h'&0x1f):
                if (s == 0)
                    goto END;
                delchar(h,--x,s--);
                if (off != 0)
                    off--;
                break;
            case ('w'&0x1f):
                if (x <= 0)
                    goto END;
                do {
                    delchar(h,--x,s--);
                    if (off != 0)
                        off--;
                } while (x != 0 && h[x-1] != ' ');
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
                delchar(h,x,s--);
                break;
            /*case ('d'&0x1f):
                while (h[args->x] && !isalnum(h[args->x])) args->x++;
                while (isalnum(h[args->x])) args->x++;
                while (h[args->x] && !isalnum(h[args->x])) args->x++;
                break;
            case ('s'&0x1f):
                while (h[args->x-1] && !isalnum(h[args->x-1])) args->x--;
                while (isalnum(h[args->x-1])) args->x--;
                while (h[args->x-1] && !isalnum(h[args->x-1])) args->x--;
                break;*/
            default:
                h[++s] = 0;
                for (size_t i = s-1; i > x; i--)
                    h[i] = h[i-1];
                h[x++] = (char)ev;
                if (x+firstl-off >= (size_t)COLS)
                    off++;
                break;
        }
    }

    END: ;
    curs_set(0);
}
