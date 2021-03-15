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

#ifndef INITS_H
#define INITS_H

void addkey(char *c, char *v);
int initcurs();
Csas *csas_init();
void csas_run(Csas *cs, const int argc, char **argv);
void update_size(Csas *cs);
void free_el(struct Element **el, size_t *size);
void csas_free(Csas *cs);

#endif