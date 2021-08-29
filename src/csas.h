#ifndef INITS_H
#define INITS_H

void xdir_free(xdir *dir);
csas *csas_init();
void csas_resize(csas *cs);
int csas_run(csas *cs, int argc, char **argv);
int csas_cd(const char *path, csas* cs);
int xfunc_add(const char *name, const uchar type, void *func, flexarr *f);
int xvar_add(void *addr, const char *name, const uchar type, void *val, flexarr *v);
int xbind_add(const char *keys, const char *value, flexarr *b);

#endif
