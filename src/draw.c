#include "main.h"
#include "useful.h"
#include "draw.h"

extern int sel_colors[];
extern li Linemode;
extern li Visual;
extern li MoveOffset;
extern li JumpScroll;
extern li JumpScrollValue;
extern li Color;
extern li ColorByExtension;
extern li HostnameInTitlebar;
extern char hostname[];
extern char *username;
extern li Sel_C;
extern li Reg_C;
extern li Exec_C;
extern li Dir_C;
extern li Link_C;
extern li Chr_C;
extern li Blk_C;
extern li Fifo_C;
extern li Sock_C;
extern li Missing_C;
extern li Other_C;
extern li Bar_C;
extern li Host_C;
extern li NumberLines;
extern li NumberLinesOffset;
extern li NumberLinesStartFrom;
extern li Archive_C;
extern li Image_C;
extern li Video_C;
extern li SizeInBytes;
extern li FileSystemInfo;

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
color_by_mode(const char *name, const mode_t m, const uchar f)
{
    if (!Color)
        return 0;
    switch (m&S_IFMT) {
        case S_IFREG:
            if (ColorByExtension) {
                switch (get_extension_group(name)) {
                    case 'A': return Archive_C;
                    case 'I': return Image_C;
                    case 'V': return Video_C;
                }
            }
            if (m&(S_IXUSR|S_IXGRP|S_IXOTH))
                return Exec_C;
            else
                return Reg_C;
        case S_IFDIR: return Dir_C;
        case S_IFLNK:
            if (f&SLINK_MISSING)
                return Missing_C;
            return Link_C;
        case S_IFCHR: return Chr_C;
        case S_IFBLK: return Blk_C;
        case S_IFIFO: return Fifo_C;
        case S_IFSOCK: return Sock_C;
        default: return Other_C;
    }
}

static void
draw_hostname()
{
    attron(Host_C);
    addstr(username);
    addch('@');
    addstr(hostname);
    attroff(Host_C);
    addch(' ');
}

static void
draw_path(csas *cs)
{
    xdir *dir = &CTAB(1);
    xfile *file = dir->files;
    size_t i=0,ctab=cs->ctab,sel=dir->sel[ctab];
    attron(Dir_C);
    if (dir->size > 0) {
        if (dir->path[0] == '/' && dir->path[1])
            i++;
        i = file[sel].nlen;
    }
    addnstr(path_shrink(dir->path,dir->plen,COLS-i-getcurx(stdscr)-1),dir->plen);
    if (dir->size > 0) {
        if (dir->path[0] == '/' && dir->path[1])
            addch('/');
        attroff(-1);
        attron(Reg_C|A_BOLD);
        addnstr(file[sel].name,file[sel].nlen);
    }
    attroff(-1);
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
                attron(A_BOLD|A_REVERSE|Bar_C);
            else
                attron(A_BOLD);
            str[1] = i+48;
            addnstr(str,3);
            attroff(-1);
        }
    }
}

void
draw_tbar(int y, csas *cs)
{
    mvhline(y,0,' ',COLS);
    if (HostnameInTitlebar)
        draw_hostname(cs);
    draw_path(cs);
    draw_tabs(y,cs);
}

void
draw_bbar(int y, csas *cs)
{
    char tmp[32],*t;
    xdir *dir = &CTAB(1);
    size_t i=0,j,ctab=cs->ctab,sel=dir->sel[ctab];
    mvhline(y,0,' ',COLS);

    t = lsperms(dir->files[sel].mode);
    addstr(t);
    addch(' ');
    if (SizeInBytes) {
        ltoa(dir->files[sel].size,tmp);
        t = tmp;
    } else {
        t = size_shrink(dir->files[sel].size);
    }
    addstr(t);
    addch(' ');
    t = tmp;
    ttoa(&dir->files[sel].mtime,tmp);
    addstr(t);

    if (dir->size > 0) {
        i = snprintf(tmp,16,"%lu/%lu",sel+1,dir->size);
        if (i)
            mvaddnstr(LINES-1,COLS-i,tmp,i);
    } else {
        mvaddch(LINES-1,COLS-1,'0');
        i = 1;
    }

    j = strlen(cs->typed);
    i += j;
    mvaddnstr(LINES-1,COLS-i-1,cs->typed,j);
    
    j = 0;
    if (Visual) {
        tmp[j++] = 'V';
        i++;
    }
    if (dir->flags&S_CHANGED) {
        tmp[j++] = 'C';
        i++;
    }
    if (j != 0) {
        tmp[j++] = ' ';
        i++;
    }
    tmp[j++] = cs->tabs[ctab].sel+48;
    tmp[j++] = 'S';
    tmp[j++] = 0;
    i += 3;
    mvaddnstr(LINES-1,COLS-i-1,tmp,j);

    if (FileSystemInfo&FS_ALL) {
        if (SizeInBytes) {
            ltoa(cs->fs.f_bsize*cs->fs.f_blocks,tmp);
            t = tmp;
        } else {
            t = size_shrink(cs->fs.f_bsize*cs->fs.f_blocks);
        }
        i += strlen(t)+1;
        mvaddstr(LINES-1,COLS-i-1,t);
    }
    if (FileSystemInfo&FS_FREE) {
        if (SizeInBytes) {
            ltoa(cs->fs.f_bsize*cs->fs.f_bfree,tmp);
            t = tmp;
        } else {
            t = size_shrink(cs->fs.f_bsize*cs->fs.f_bfree);
        }
        i += strlen(t)+1;
        mvaddstr(LINES-1,COLS-i-1,t);
    }
    if (FileSystemInfo&FS_AVAIL) {
        if (SizeInBytes) {
            ltoa(cs->fs.f_bsize*cs->fs.f_bavail,tmp);
            t = tmp;
        } else {
            t = size_shrink(cs->fs.f_bsize*cs->fs.f_bavail);
        }
        i += strlen(t)+1;
        mvaddstr(LINES-1,COLS-i-1,t);
    }
    if (FileSystemInfo&FS_FILES) {
        if (SizeInBytes) {
            ltoa(cs->fs.f_files,tmp);
            t = tmp;
        } else {
            t = size_shrink(cs->fs.f_files);
        }
        i += strlen(t)+1;
        mvaddstr(LINES-1,COLS-i-1,t);
    }
}

void
draw_dir(WINDOW *win, xdir *dir, csas *cs)
{
    size_t i,j,ctab=cs->ctab,scroll=dir->scroll[ctab],sel=dir->sel[ctab];
    xfile *file = dir->files;
    int color,maxx=win->_maxx+1,maxy=win->_maxy+1,offset=maxy>>MoveOffset,
        jumpvalue=maxy>>JumpScrollValue;

    if (Visual && dir->size)
        dir->files[dir->sel[ctab]].sel[ctab] |= 1<<cs->tabs[ctab].sel;

    if ((size_t)maxy >= dir->size-1) {
        dir->scroll[ctab] = 0;
    } else if (sel < scroll+offset) {
        if (sel <= (size_t)offset) {
            dir->scroll[ctab] = 0;
        } else {
            dir->scroll[ctab] = sel-offset;
            if (JumpScroll) {
                if (dir->scroll[ctab] < (size_t)jumpvalue)
                    dir->scroll[ctab] = 0;
                else
                    dir->scroll[ctab] -= jumpvalue;
            }
        }
    } else if (sel < scroll) {
        dir->scroll[ctab] = sel;
    } else if (sel == dir->size-1) {
        dir->scroll[ctab] = dir->size-maxy;
    } else if (scroll > sel) {
        dir->scroll[ctab] = dir->size-maxy;
    } else if (scroll >= dir->size-maxy) {
        dir->scroll[ctab] = dir->size-maxy;
    } else if (sel >= scroll+maxy-offset) {
        dir->scroll[ctab] = sel-maxy+1+offset;
        if (JumpScroll)
            dir->scroll[ctab] += jumpvalue;
    }
    scroll = dir->scroll[ctab];

    size_t offt=0,off1=0,off2,endl=0;
    char tmp[32],*end;
    if (NumberLinesOffset) {
        off1 = 0;
        register size_t c = (dir->size-1)+NumberLinesStartFrom;
        while (c > 9) {
            c /= 10;
            off1++;
        }
    }
    for (i = 0, j = scroll; i < (size_t)maxy && j < dir->size; i++, j++) {
        if (file[j].sel[ctab]&(1<<cs->tabs[ctab].sel))
            wattr_set(win,A_REVERSE,sel_colors[cs->tabs[ctab].sel],NULL);
        mvwaddch(win,i,0,' ');
        wattr_set(win,0,0, NULL);

        color =  color_by_mode(file[j].name,file[j].mode,file[j].flags);
        if (j == sel)
            color |= Sel_C;
        wattron(win,color);
        mvwhline(win,i,1,' ',maxx);

        if (win == cs->wins[1]) {
            if (NumberLines) {
                offt = 0;
                if (NumberLinesOffset) {
                    off2 = 0;
                    register size_t c = j+NumberLinesStartFrom;
                    while (c > 9) {
                        c /= 10;
                        off2++;
                    }
                    offt = off1-off2;
                }
                ltoa(j+NumberLinesStartFrom,tmp);
                mvwaddstr(win,i,2+offt,tmp);
                offt += strlen(tmp)+1;
            }
            
            switch (Linemode) {
                case L_SIZE:
                    if (SizeInBytes) {
                        ltoa(file[j].size,tmp);
                        end = tmp;
                    } else {
                        end = size_shrink(file[j].size);
                    }
                    endl = strlen(end);
                    break;
                case L_MTIME:
                    end = tmp;
                    endl = ttoa(&file[j].mtime,tmp);
                    break;
                case L_PERMS:
                    end = lsperms(file[j].mode);
                    endl = 10;
                    break;
                default:
                    end = NULL;
                    endl = 0;
            }
        }

        if (file[j].nlen >= maxx-endl-5-offt) {
            mvwaddnstr(win,i,2+offt,file[j].name,maxx-endl-5);
            waddch(win,'~');
        } else {
            mvwaddnstr(win,i,2+offt,file[j].name,file[j].nlen);
        }
        if (win == cs->wins[1])
            mvwaddnstr(win,i,maxx-endl-1,end,endl);
        wattroff(win,-1);
    }
    for (; i < (size_t)maxy; i++)
        mvwhline(win,i,0,' ',maxx);
    wrefresh(win);
}
