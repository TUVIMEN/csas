#ifndef USEFUL_H
#define USEFUL_H

void exiterr();
int getinput(csas *cs);
const char *path_shrink(const char *path, const size_t size, const size_t max_size);
char *size_shrink(size_t size);

#endif
