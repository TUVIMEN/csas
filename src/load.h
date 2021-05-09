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

#ifndef LOAD_H
#define LOAD_H

#ifdef __UPDATE_FILES__
void updatefile(struct Element *el, const char *path);
#endif
int getdir(const char *path, Csas *cs, const int ws, const int which, const char mode
#ifdef __FOLLOW_PARENT_DIR__
, char *searched_name
#endif
#ifdef __THREADS_FOR_DIR_ENABLE__
, const bool threaded
#endif
);
int csas_cd(const char *path, const int ws, Csas *cs);

#endif
