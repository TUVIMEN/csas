/*
    csas - console file manager
    Copyright (C) 2020-2025 Dominik Stanisław Suchora <suchora.dominik7@gmail.com>

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

size_t console_getline(char **history, size_t size, char *first, char *add, li offset, csas *cs,
    int (*expand)(char *line, size_t pos, size_t *size, uchar *tabp, flexarr *arg, uchar *free_names, csas *cs));

#endif
