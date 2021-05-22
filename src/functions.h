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

char *cmd_alias(const char *src, Csas *cs);
char *cmd_rename(char *src, Csas *cs);
char *cmd_set(const char *src, Csas *cs);
#ifdef __LOAD_CONFIG_ENABLE__
char *cmd_source(const char *src, Csas *cs);
#endif
char *cmd_map(const char *src, Csas *cs);
char *cmd_move(const char *src, Csas *cs);
char *cmd_quit(const char *src, Csas *cs);
char *cmd_cd(const char *src, Csas *cs);
char *cmd_open_with(const char *src, Csas *cs);
char *cmd_gotop(const char *src, Csas *cs);
char *cmd_godown(const char *src, Csas *cs);
char *cmd_change_workspace(const char *src, Csas *cs);
#ifdef __FILE_SIZE_ENABLE__
char *cmd_getsize(const char *src,Csas *cs);
#endif
char *___SETGROUP(const char *src, Csas *cs);
char *cmd_fastselect(const char *src, Csas *cs);
char *cmd_togglevisual(const char *src, Csas *cs);
char *cmd_f_mod(const char *src, Csas *cs);
char *cmd_search(const char *src, Csas *cs);
char *cmd_load(const char *src, Csas *cs);
char *cmd_select(const char *src, Csas *cs);
char *cmd_exec(const char *src, Csas *cs);
char *cmd_bulk(const char *src, Csas *cs);
char *cmd_console(const char *src, Csas *cs);
char *cmd_shell(const char *src, Csas *cs);
char *cmd_filter(const char *src, Csas *cs);

#endif
