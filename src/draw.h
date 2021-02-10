/*
    csas - console file manager
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

#ifndef DRAW_H
#define DRAW_H

void set_message(Basic* grf, const int attr, const char* fmt, ...);
void DrawText(WINDOW* grf, int fd, off_t offset, int whence, bool wrap);
void DrawBasic(Basic* grf,const int which);
void SetBorders(Basic* grf, const int which);

#endif