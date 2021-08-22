#include "main.h"
#include "useful.h"
#include "draw.h"

extern int sel_colors[];

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

static int
color_by_mode(const mode_t m, const uchar f)
{
    switch (m&S_IFMT) {
        case S_IFREG:
            if (m&(S_IXUSR|S_IXGRP|S_IXOTH))
                return EXEC_C;
            else
                return REG_C;
        case S_IFDIR: return DIR_C;
        case S_IFLNK:
            if (f&SLINK_MISSING)
                return MISSING_C;
            return LINK_C;
        case S_IFCHR: return CHR_C;
        case S_IFBLK: return BLK_C;
        case S_IFIFO: return FIFO_C;
        case S_IFSOCK: return SOCK_C;
        default: return OTHER_C;
    }
}

static void
draw_path(int y, csas *cs)
{
    xdir *dir = &CTAB;
    xfile *file = dir->files;
    size_t i=0,ctab=cs->ctab;
    attr_set(A_BOLD,DIR_C,NULL);
    if (dir->size > 0) {
        if (dir->path[0] == '/' && dir->path[1])
            i++;
        i = file[dir->sel[ctab]].nlen;
    }
    mvaddnstr(y,0,path_shrink(dir->path,dir->plen,COLS-i),dir->plen);
    if (dir->size > 0) {
        if (dir->path[0] == '/' && dir->path[1])
            addch('/');
        attr_set(A_BOLD,REG_C,NULL);
        addnstr(file[dir->sel[ctab]].name,file[dir->sel[ctab]].nlen);
    }
}

static void
draw_tabs(int y, csas *cs)
{
    xtab *tabs = cs->tabs;
    uint n = 0;
    for (uint i = 0; i < TABS; i++)
        if (tabs[i].flags&T_EXISTS)
            n++;
   if (n <= 1)
       return;
    move(y,COLS-n*3);
    char str[3];
    str[0] = str[2] = ' ';
    for (uint i = 0; i < TABS; i++) {
        if (tabs[i].flags&T_EXISTS) {
            if (i == cs->ctab)
                attr_set(A_BOLD|A_REVERSE,BAR_C,NULL);
            else
                attr_set(A_BOLD,0,NULL);
            str[1] = i+48;
            addnstr(str,3);
        }
    }
}

void
draw_tbar(int y, csas *cs)
{
    mvhline(y,0,' ',COLS);
    draw_path(y,cs);
    draw_tabs(y,cs);
    attr_set(0,0,NULL);
}

void
draw_bbar(int y, csas *cs)
{
    char t[16];
    size_t i=0,j;
    xdir *dir = &CTAB;
    mvhline(y,0,' ',COLS);

    if (dir->size > 0) {
        i = snprintf(t,16,"%lu/%lu",dir->sel[cs->ctab]+1,dir->size);
        if (i)
            mvaddnstr(LINES-1,COLS-i,t,i);
    } else {
        mvaddch(LINES-1,COLS-1,'0');
        i = 1;
    }

    j = strlen(cs->typed);
    i += j;
    mvaddnstr(LINES-1,COLS-i-1,cs->typed,j);
    
    t[0] = cs->tabs[cs->ctab].sel+48;
    t[1] = 'T';
    t[2] = 0;
    i += 3;
    mvaddnstr(LINES-1,COLS-i-1,t,2);
    
}

void
draw_dir(WINDOW *win, xdir *dir, csas *cs)
{
    size_t i,j;
    size_t ctab=cs->ctab;
    xfile *file = dir->files;
    int color,maxx=win->_maxx+1,maxy=win->_maxy+1;
    if (dir->sel[ctab] < dir->scroll[ctab])
        dir->scroll[ctab] = dir->sel[ctab];
    else if (dir->sel[ctab] > dir->scroll[ctab]+maxy-1)
        dir->scroll[ctab] = dir->sel[ctab]-maxy+1;

    for (i = 0, j = dir->scroll[ctab]; i < (size_t)maxy && j < dir->size; i++, j++) {
        if (file[j].sel[ctab]&(1<<cs->tabs[ctab].sel))
            wattr_set(win,A_REVERSE,sel_colors[cs->tabs[ctab].sel],NULL);
        mvwaddch(win,i,0,' ');
        wattr_set(win,0,0, NULL);

        color =  color_by_mode(file[j].mode,file[j].flags);
        if (j == dir->sel[ctab])
            wattr_on(win,SEL_C,NULL);
        wcolor_set(win,color,NULL);
        mvwhline(win,i,1,' ',maxx);

        char *size = size_shrink(file[j].size);
        size_t sizel = strlen(size);

        if (file[j].nlen >= maxx-sizel-5) {
            mvwaddnstr(win,i,2,file[j].name,maxx-sizel-5);
            waddch(win,'~');
        } else
            mvwaddnstr(win,i,2,file[j].name,file[j].nlen);
        mvwaddnstr(win,i,maxx-sizel-1,size,sizel);
        wattroff(win,-1);
    }
    for (; i < (size_t)maxy; i++)
        mvwhline(win,i,0,' ',maxx);
    wrefresh(win);
}
