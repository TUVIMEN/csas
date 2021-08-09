#ifndef INITS_H
#define INITS_H

void xdir_free(xdir *dir);
csas *csas_init();
void csas_resize(csas *cs);
void csas_run(csas *cs, int argc, char **argv);
int xfunc_add(const char *name, const uchar type, void *func, flexarr *f);

#endif
