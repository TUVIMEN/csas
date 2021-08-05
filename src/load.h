#ifndef LOAD_H
#define LOAD_H

int load_dir(xdir *dir, const mode_t flags);
int get_dir(const char *path, flexarr *dir, size_t *num, const mode_t flags);

#endif
