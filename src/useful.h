#ifndef USEFUL_H
#define USEFUL_H

void exiterr();
int getinput(csas *cs);
char *delchar(char *src, const size_t pos, const size_t size);
const char *path_shrink(const char *path, const size_t size, const size_t max_size);
char *size_shrink(size_t size);
#define seek_end_of_squote(x,y) memchr(x,'\'',y)
char *seek_end_of_dquote(char *src, size_t size);
int addenv(char *dest, char *src, size_t *x, size_t *y, size_t size);
int handle_percent(char *dest, char *src, size_t *x, size_t *y, xdir *dir);
char special_character(const char c);
char *get_path(char *dest, char *src, size_t size, xdir *dir);
void get_line(char *dest, char *src, size_t *pos, size_t size);
int config_load(const char *path, csas *cs);
int file_run(char *path);
pid_t xfork(uchar flag);
int spawn(char *file, char *arg1, char *arg2, const uchar flags);

#endif
