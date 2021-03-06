/*
    csas - console file manager
    Copyright (C) 2020-2021 TUVIMEN <suchora.dominik7@gmail.com>

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

char *csas_strerror();
void die(const char *p, ...);

int spawn(char *file, char *arg1, char *arg2, const uchar mode);

char *stoa(ull value); //size to string

pid_t xfork(uchar flag);

int get_dirsize(const int fd, ull *count, ull *size, const uchar flag);

#ifdef __COLOR_FILES_BY_EXTENSION__
uchar check_extension(const char *name);
#endif

char *lsperms(const int mode);

size_t ttoa(const time_t *time, char *result); //time to string

int strverscasecmp(const char *s1, const char *s2);

void file_run(char *path);
void path_shrink(char *path, const int max_size);

void file_rm(const int fd, const char *name);
void file_cp(const int fd1, const int fd2, const char *name, char *buffer, const mode_t arg);
void file_mv(const int fd1, const int fd2, const char *name, char *buffer, const mode_t arg);

char *atob(char *s); //path to shell arg
char *mkpath(const char *dir, const char *name); //combines dir and file name to path
size_t findfirst(const char *src, int (*func)(int), size_t n);
int get_word(char *dest, char *src, size_t n, size_t *dsize, size_t *ssize);
int atov(void *dest, const char *src, size_t *size, csas *cs, const uchar flag); //converts string to value
wchar_t charconv(const char c); //changes char after '\\'
size_t atop(char *dest, const char *src, const char delim, csas *cs); //changes string to path
wchar_t *atok(char *src,  wchar_t  *dest); //changes string to keys

flexarr *flexarr_init(const size_t nmemb, const size_t inc_r, const size_t max_size);
void *flexarr_inc(flexarr *f);
void flexarr_free(flexarr *f);

#endif