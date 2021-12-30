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

void csas_resize(csas *cs);
uint update_event(csas *cs);
int alias_run(char *src, csas *cs);
int command_run(char *src, csas *cs);
int cmd_move(char *src, csas *cs);
int cmd_scout(char *src, csas *cs);
int cmd_console(char *src, csas *cs);
int cmd_tab(char *src, csas *cs);
int cmd_source(char *src, csas *cs);
int cmd_cd(char *src, csas *cs);
int cmd_file_run(char *src, csas *cs);
int cmd_exec(char *src, csas *cs);
int cmd_rename(char *src, csas *cs);
int cmd_map(char *src, csas *cs);
int cmd_open_with(char *src, csas *cs);
int cmd_sort(char *src, csas *cs);
int cmd_set(char *src, csas *cs);
int cmd_lmove(char *src, csas *cs);
int cmd_quit(char *src, csas *cs);
int cmd_alias(char *src, csas *cs);

#endif
