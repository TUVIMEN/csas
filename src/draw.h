#ifndef DRAW_H
#define DRAW_H

void printmsg(const int attr, const char *fmt, ...);
void draw_tbar(int y, csas *cs);
void draw_bbar(int y, csas *cs);
void draw_dir(WINDOW *win, xdir *dir, csas *cs);

#endif
