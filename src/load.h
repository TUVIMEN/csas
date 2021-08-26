#ifndef LOAD_H
#define LOAD_H

int xfile_update(xfile *f);
int load_dir(xdir *dir, const mode_t flags);
li getdir(const char *path, flexarr *dir, const uchar flags);

#endif
