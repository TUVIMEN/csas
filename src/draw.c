#include "main.h"
#include "draw.h"

void
printmsg(const int attr, const char *fmt, ...)
{
    va_list va_args;
    va_start(va_args,fmt);
    mvhline(LINES-1,0,' ',COLS);
    attron(attr);
    move(LINES,0);
    vw_printw(stdscr,fmt,va_args);
    attroff(attr);
    va_end(va_args);
}
