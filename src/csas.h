/*
    csas - console file manager
    Copyright (C) 2020-2023 Dominik Stanisław Suchora <suchora.dominik7@gmail.com>

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

#ifndef INITS_H
#define INITS_H

csas *csas_init();
void csas_resize(csas *cs);
void wins_resize(WINDOW **wins);
int csas_cd(const char *path, csas* cs);
void csas_draw(csas *cs);
void csas_free(csas *cs);
int xfunc_add(const char *name, const uchar type, void *func, void *expand, flexarr *f);
int xvar_add(void *addr, const char *name, const uchar type, void *val, flexarr *v);
int xbind_add(char *keys, const char *value, flexarr *b);

#endif
