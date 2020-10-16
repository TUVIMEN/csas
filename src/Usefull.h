/*
    csas - terminal file manager
    Copyright (C) 2020 TUVIMEN <suchora.dominik7@gmail.com>

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

#ifndef USEFULL_H
#define USEFULL_H

int spawn(char*, char*, char*, const unsigned char);

#ifdef __HUMAN_READABLE_SIZE_ENABLE__
void MakeHumanReadAble(char*, const unsigned long long int, const bool);
#endif

#ifdef __GET_DIR_SIZE_ENABLE__
unsigned long long int GetDirSize(int, bool, bool);
#endif

#ifdef __COLOR_FILES_BY_EXTENSION__
unsigned char CheckFileExtension(const char*);
#endif

char* lsperms(const int, const int);

size_t TimeToStr(const time_t *, char*);

void RunFile(char*);
void MakePathShorter(char*, const int);

void DeleteGroup(Basic*, const bool);
void MoveGroup(Basic*, const char*, const mode_t);
void CopyGroup(Basic*, const char*, const mode_t);

char* MakePathRunAble(char*);
char* MakePath(const char*, const char*);
size_t FindFirstCharacter(const char*);
size_t StrToValue(void*, const char*);
char* StrConv(char*);
size_t StrToPath(char*, const char*);
char* StrToKeys(char*);
size_t FindEndOf(const char*, const char);

#endif
