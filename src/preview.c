/*
    csas - console file manager
    Copyright (C) 2020-2023 Dominik Stanisław Suchora <suchora.dominik7@gmail.com>

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
#include "load.h"
#include "draw.h"
#include "useful.h"
#include "preview.h"

extern li DirLoadingMode;
extern li PreviewSettings;
extern char BinaryPreview[];
extern flexarr *trap_preview;

int
preview_get(xfile *f, csas *cs)
{
    cs->tabs[cs->ctab].wins[2] = (size_t)-1;
    if (!PreviewSettings)
        return 0;

    if ((PreviewSettings&P_DIR) && ((f->mode&S_IFMT) == S_IFDIR || (f->flags&SLINK_TO_DIR))) {
        li n = getdir(f->name,cs->dirs,DirLoadingMode);
        cs->tabs[cs->ctab].wins[2] = (size_t)n;
        if (n == -1)
            return -1;
        return 0;
    }

    if (!(PreviewSettings&P_FILE) || (f->mode&S_IFMT) != S_IFREG)
        return 0;

    int fd = open(f->name,O_RDONLY);
    if (fd == -1)
        return -1;
    ssize_t r = read(fd,cs->preview,2048);
    if (isbinfile(cs->preview,r)) {
        cs->preview[0] = 0;
        if (PreviewSettings&P_TRAP)
            return 0;
        if (!(PreviewSettings&P_BFILE)) {
            close(fd);
            return 0;
        }

        int pipes[2];
        if (pipe(pipes) == -1) {
            close(fd);
            return -1;
        }

        if (vfork() == 0) {
            dup2(pipes[1],1);
            close(pipes[0]);
            close(pipes[1]);
            execlp(BinaryPreview,BinaryPreview,f->name,NULL);
            _exit(1);
        } else {
            close(pipes[1]);
            while (wait(NULL) != -1);
            r = read(pipes[0],cs->preview,PREVIEW_MAX);
            cs->preview[r] = 0;
            close(pipes[0]);
        }
        close(fd);
        return 0;
    }
    
    cs->preview[r] = 0;
    r = read(fd,cs->preview+r,PREVIEW_MAX-r);
    if (r)
        cs->preview[r] = 0;
    close(fd);
    return 0;
}

void
preview_draw(WINDOW *win, csas *cs)
{
    if (cs->tabs[cs->ctab].wins[2] != (size_t)-1) {
        draw_dir(win,&CTAB(2),cs);
        return;
    }

    if (!cs->preview[0])
        for (size_t i = 0; i < trap_preview->size; i++)
            alias_run(((char**)trap_preview->v)[i],strlen(((char**)trap_preview->v)[i]),cs);
    
    int posy=0,posx=1;
    mvwhline(win,posy,0,' ',win->_maxx+1);
    for (register ssize_t i = 0; cs->preview[i] && posy <= win->_maxy; i++) {
        if (PreviewSettings&P_WRAP && win->_maxx <= posx) {
            posy++;
            mvwhline(win,posy,0,' ',win->_maxx+1);
            posx = 1;
        } else if (cs->preview[i] == '\n') {
            posy++;
            mvwhline(win,posy,0,' ',win->_maxx+1);
            posx = 1;
            continue;
        }
        mvwaddch(win,posy,posx++,cs->preview[i]);
    }
    for (; posy < win->_maxy+1; posy++)
        mvwhline(win,posy,0,' ',win->_maxx+1);
    wrefresh(win);
}
