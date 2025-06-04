/*
    csas - console file manager
    Copyright (C) 2020-2025 Dominik Stanisław Suchora <hexderm@gmail.com>

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

#ifndef CALC_H
#define CALC_H

size_t get_bin(const char *v, li *n);
size_t get_oct(const char *v, li *n);
size_t get_dec(const char *v, li *n);
size_t get_hex(const char *v, li *n);
size_t calc(const char *v, li *n, flexarr *vars);

#endif
