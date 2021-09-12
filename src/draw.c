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
extern li Error_C;
extern li NumberLines;
extern li NumberLinesOffset;
extern li NumberLinesStartFrom;
extern li Archive_C;
extern li Image_C;
extern li Video_C;
extern li SizeInBytes;
extern li FileSystemInfo;
extern li Borders;
extern li Border_C;
extern li MultipaneView;
extern li LeftWindowSize;
extern li CenterWindowSize;
extern li RightWindowSize;

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
    char tmp[PATH_MAX],*t;
    xdir *dir = &CTAB(1);
    size_t i=0,j,ctab=cs->ctab,sel=dir->sel[ctab];
    mvhline(y,0,' ',COLS);

    if (dir->size) {
        xfile *file = &dir->files[sel];
        t = lsperms(file->mode);
        addstr(t);
        addch(' ');
        if (SizeInBytes) {
            ltoa(file->size,tmp);
            t = tmp;
        } else {
            t = size_shrink(file->size);
        }
        addstr(t);
        addch(' ');
        t = tmp;
        ttoa(&file->mtime,tmp);
        addstr(t);

        if (file->flags&(SLINK_TO_DIR|SLINK_MISSING) || (file->mode&S_IFMT) == S_IFDIR) {
            tmp[0] = ' ';
            tmp[1] = '-';
            tmp[2] = '>';
            ssize_t c = readlink(file->name,tmp+3,PATH_MAX-3);
            if (c != -1) {
                tmp[c+3] = 0;
                addstr(t);
            }
        }
        
        if (dir->size > 0) {
            i = snprintf(tmp,16,"%lu/%lu",sel+1,dir->size);
            if (i)
                mvaddnstr(LINES-1,COLS-i,tmp,i);
        } else {
            mvaddch(LINES-1,COLS-1,'0');
            i = 1;
        }
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
    if (dir->size == 0) {
        endwin();
        printf("gsag\n");
        refresh();
        werase(win);
        wattron(win,Error_C|A_REVERSE);
        if (dir->flags&SEACCES)
            mvwaddstr(win,0,1,"NOT ACCESSIBLE");
        else
            mvwaddstr(win,0,1,"EMPTY");
        wattroff(win,Error_C|A_REVERSE);
        wrefresh(win);
        return;
    }

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

void
draw_borders()
{
    int outline=0,separators=0;
    if (Borders&B_OUTLINE)
        outline = 1;
    if (Borders&B_SEPARATORS)
        separators = 1;
    if (!MultipaneView) {
        if (outline) {
            attron(Border_C);
            mvhline(1,1,ACS_HLINE,COLS-2);
            mvhline(LINES-2,1,ACS_HLINE,COLS-2);
            mvvline(2,0,ACS_VLINE,LINES-4);
            mvvline(2,COLS-1,ACS_VLINE,LINES-4);
            mvaddch(1,0,ACS_ULCORNER);
            mvaddch(1,COLS-1,ACS_URCORNER);
            mvaddch(LINES-2,0,ACS_LLCORNER);
            mvaddch(LINES-2,COLS-1,ACS_LRCORNER);
            attroff(Border_C);
        }
        return;
    }

    attron(Border_C);

    li sum = CenterWindowSize+LeftWindowSize+RightWindowSize,t1,t2;
    t1 = (COLS/sum)*LeftWindowSize;
    t2 = t1+1;
    if (separators)
        mvvline(1,t1,ACS_VLINE,LINES-2);
    if (outline) {
        mvhline(1,1,ACS_HLINE,COLS-2);
        mvhline(LINES-2,1,ACS_HLINE,COLS-2);
        mvvline(2,0,ACS_VLINE,LINES-4);
        mvvline(2,COLS-1,ACS_VLINE,LINES-4);
        mvaddch(1,0,ACS_ULCORNER);
        mvaddch(1,COLS-1,ACS_URCORNER);
        mvaddch(LINES-2,0,ACS_LLCORNER);
        mvaddch(LINES-2,COLS-1,ACS_LRCORNER);
    }
    if (outline && separators) {
        mvaddch(1,t1,ACS_URCORNER);
        mvaddch(LINES-2,t1,ACS_LRCORNER);
    }
    t1 = (COLS/sum)*CenterWindowSize;
    t2 += t1;

    if (separators)
        mvvline(1,t2,ACS_VLINE,LINES-2);
    if (outline && separators) {
        mvaddch(1,t2,ACS_URCORNER);
        mvaddch(LINES-2,t2,ACS_LRCORNER);
    }

    attroff(Border_C);
}
