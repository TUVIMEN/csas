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

void die(int status, const char *p, ...);

int spawn(char* file, char* arg1, char* arg2, const uchar mode);

char* MakeHumanReadAble(ull value);

void GetDirSize(const int fd, ull* count, ull* size, const bool recursive);

#ifdef __COLOR_FILES_BY_EXTENSION__
uchar check_extension(const char* name);
#endif

char* lsperms(const int mode);

size_t TimeToStr(const time_t* time, char* result);

void RunFile(char* path);
void MakePathShorter(char* path, const int max_size);

void DeleteFile(const int fd, const char* name);
void CopyFile(const int fd1, const int fd2, const char* name, char* buffer, const mode_t arg);
void MoveFile(const int fd1, const int fd2, const char* name, char* buffer, const mode_t arg);

char* MakePathRunAble(char* temp);
char* MakePath(const char* dir, const char* name);
size_t FindFirstCharacter(const char* src);
size_t StrToValue(void* dest, const char* src);
wchar_t CharConv(const char c);
size_t StrToPath(char* dest, const char* src);
wchar_t *StrToKeys(char *src,  wchar_t * dest);
size_t FindEndOf(const char* src, const char res);

#endif
