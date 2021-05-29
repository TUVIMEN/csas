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

int update_event(csas *cs);
void csas_exit(csas *cs, const bool force);
void move_to(csas *cs, const int ws, const int which, const char *name);

int cmd_alias(const char *src, csas *cs);
int cmd_rename(char *src, csas *cs);
int cmd_set(const char *src, csas *cs);
#ifdef __LOAD_CONFIG_ENABLE__
int cmd_source(const char *src, csas *cs);
#endif
int cmd_map(const char *src, csas *cs);
int cmd_move(const char *src, csas *cs);
int cmd_quit(const char *src, csas *cs);
int cmd_cd(const char *src, csas *cs);
int cmd_open_with(const char *src, csas *cs);
int cmd_gotop(const char *src, csas *cs);
int cmd_godown(const char *src, csas *cs);
int cmd_change_workspace(const char *src, csas *cs);
#ifdef __FILE_SIZE_ENABLE__
int cmd_getsize(const char *src,csas *cs);
#endif
char *___SETGROUP(const char *src, csas *cs);
int cmd_fastselect(const char *src, csas *cs);
int cmd_togglevisual(const char *src, csas *cs);
int cmd_f_mod(const char *src, csas *cs);
int cmd_search(const char *src, csas *cs);
int cmd_load(const char *src, csas *cs);
int cmd_select(const char *src, csas *cs);
int cmd_exec(const char *src, csas *cs);
int cmd_bulk(const char *src, csas *cs);
int cmd_console(const char *src, csas *cs);
int cmd_filter(const char *src, csas *cs);

#endif
