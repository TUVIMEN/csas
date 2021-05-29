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


#ifndef CONSOLE_H
#define CONSOLE_H

int command_run(char *src, csas *cs);
void console_resize(WINDOW *win, const struct winargs *args);
void console_getline(WINDOW *win, csas *cs, char **history, size_t size, size_t max, struct winargs *args, char *first, char *add,
    int (*expand)(WINDOW *win, char *line, size_t pos, short int off, bool *tab_was_pressed, struct expand_arg *arr, struct winargs *args, char **end));

#endif