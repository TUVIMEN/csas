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

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

int update_event(Csas *cs);
void csas_exit(Csas *cs, const bool force);
void move_to(Csas *cs, const int ws, const int which, const char *name);

char *___SET(const char *src, Csas *cs);
#ifdef __LOAD_CONFIG_ENABLE__
char *___INCLUDE(const char *src, Csas *cs);
#endif
char *___MAP(const char *src, Csas *cs);
char *___MOVE(const char *src, Csas *cs);
char *___QUIT(const char *src, Csas *cs);
char *___CD(const char *src, Csas *cs);
char *___OPEN_WITH(const char *src, Csas *cs);
char *___GOTOP(const char *src, Csas *cs);
char *___GODOWN(const char *src, Csas *cs);
char *___CHANGEWORKSPACE(const char *src, Csas *cs);
#ifdef __FILE_SIZE_ENABLE__
char *___GETSIZE(const char *src,Csas *cs);
#endif
char *___SETGROUP(const char *src, Csas *cs);
char *___FASTSELECT(const char *src, Csas *cs);
char *___TOGGLEVISUAL(const char *src, Csas *cs);
char *___F_MOD(const char *src, Csas *cs);
char *___SEARCH(const char *src, Csas *cs);
char *___LOAD(const char *src, Csas *cs);
char *___SELECT(const char *src, Csas *cs);
char *___EXEC(const char *src, Csas *cs);
char *___BULK(const char *src, Csas *cs);
char *___CONSOLE(const char *src, Csas *cs);
char *___SHELL(const char *src, Csas *cs);
char *___FILTER(const char *src, Csas *cs);

#endif
