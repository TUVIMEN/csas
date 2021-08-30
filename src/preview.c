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
#include "load.h"
#include "draw.h"
#include "preview.h"

extern li DirLoadingMode;

int
preview_get(xfile *f, csas *cs)
{
    if (access(f->name,R_OK) == -1) {
        cs->tabs[cs->ctab].wins[2] = (size_t)-1;
        return -1;
    }
    if ((f->mode&S_IFMT) == S_IFDIR || f->flags&SLINK_TO_DIR) {
        li n = getdir(f->name,cs->dirs,DirLoadingMode);
        cs->tabs[cs->ctab].wins[2] = (size_t)n;
        if (n == -1)
            return -1;
        return 0;
    }
    cs->tabs[cs->ctab].wins[2] = (size_t)-1;
    return 0;
}

void
preview_draw(WINDOW *win, csas *cs)
{
    if (cs->tabs[cs->ctab].wins[2] != (size_t)-1) {
        draw_dir(win,&CTAB(2),cs);
        return;
    }
    werase(win);
    wrefresh(win);
}
