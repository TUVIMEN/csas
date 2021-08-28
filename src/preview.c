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
