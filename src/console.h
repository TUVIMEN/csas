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


#ifndef CONSOLE_H
#define CONSOLE_H

void RunCommand(const char* src, Basic* grf);
void ConsoleResize(WINDOW* win, const struct WinArgs args);
void ConsoleGetLine(WINDOW* win, Basic* grf, char** history, size_t size, size_t max, struct WinArgs args, char* first, char* add);

#endif