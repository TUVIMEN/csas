#ifndef INITS_H
#define INITS_H

void xdir_free(xdir *dir);
csas *csas_init();
void csas_resize(csas *cs);
void csas_run(csas *cs, int argc, char **argv);

#endif
