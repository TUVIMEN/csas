/*
    csas - console file manager
    Copyright (C) 2020-2022 TUVIMEN <suchora.dominik7@gmail.com>

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
int command_run(char *src, size_t size, csas *cs);
int cmd_move(int argc, char **argv, csas *cs);
int cmd_scout(int argc, char **argv, csas *cs);
int cmd_console(int argc, char **argv, csas *cs);
int cmd_tab(int argc, char **argv, csas *cs);
int cmd_source(int argc, char **argv, csas *cs);
int cmd_cd(int argc, char **argv, csas *cs);
int cmd_file_run(int argc, char **argv, csas *cs);
int cmd_exec(int argc, char **argv, csas *cs);
int cmd_rename(int argc, char **argv, csas *cs);
int cmd_map(int argc, char **argv, csas *cs);
int cmd_unmap(int argc, char **argv, csas *cs);
int cmd_trap(int argc, char **argv, csas *cs);
int cmd_open_with(int argc, char **argv, csas *cs);
int cmd_sort(int argc, char **argv, csas *cs);
int cmd_set(int argc, char **argv, csas *cs);
int cmd_unset(int argc, char **argv, csas *cs);
int cmd_lmove(int argc, char **argv, csas *cs);
int cmd_quit(int argc, char **argv, csas *cs);
int cmd_alias(int argc, char **argv, csas *cs);

#endif
