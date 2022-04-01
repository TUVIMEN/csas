/*
    csas - console file manager
    Copyright (C) 2020-2022 TUVIMEN <suchora.dominik7@gmail.com>

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

#ifndef USEFUL_H
#define USEFUL_H

void exiterr();
int getinput(csas *cs);
int getinput_wch(wint_t *wch, csas *cs);
char *delchar(char *src, const size_t pos, const size_t size);
wchar_t *delwc(wchar_t *src, const size_t pos, const size_t size);
const char *path_shrink(const char *path, const size_t size, const size_t max_size);
char *size_shrink(size_t size);
#define seek_end_of_squote(x,y) (memchr(x,'\'',y)+1)
char *seek_end_of_dquote(char *src, size_t size);
int addenv(char *dest, char *src, size_t *x, size_t *y, const size_t max, size_t size);
int handle_percent(char *dest, char *src, size_t *x, size_t *y, const size_t max, xdir *dir, const int tab, const uchar sel);
char special_character(const char c);
char *get_arg(char *dest, char *src, const char delim, size_t size, size_t *count, const size_t max, csas *cs);
size_t get_line(char *dest, char *src, size_t *count, size_t size);
int config_load(const char *path, csas *cs);
void change_keys(wchar_t *dest, const char *src);
int file_run(char *path, csas *cs);
pid_t xfork(uchar flag);
int spawn(char *file, char *arg1, char *arg2, const uchar flags);
int file_rm(const int fd, const char *name);
int file_cp(const int fd1, const int fd2, const char *name, char *buffer, const mode_t flags);
int file_mv(const int fd1, const int fd2, const char *name, char *buffer, const mode_t flags);
int get_dirsize(const int fd, off_t *count, off_t *size, const uchar flags);
char *mkpath(const char *dir, const char *name);
int bulk(csas *cs, const size_t tab, const int selected, char **args, const uchar flags);
void ltoa(li num, char *result);
int ttoa(const time_t *time, char *result);
uchar isbinfile(char *src, size_t size);
char *lsperms(const mode_t mode);
uchar get_extension_group(const char *name);
size_t get_range(const char *src, ul *x, ul *y, size_t(handle_number)(const char*,ul*));
char *strtoshellpath(char *src);

#endif
