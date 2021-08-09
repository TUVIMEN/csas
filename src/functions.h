#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void csas_resize(csas *cs);
uint update_event(csas *cs);
int command_run(char *src, csas *cs);
int cmd_move(char *src, csas *cs);
int cmd_console(char *src, csas *cs);
int cmd_tab(char *src, csas *cs);
int cmd_select(char *src, csas *cs);
int cmd_fastselect(char *src, csas *cs);
int cmd_source(char *src, csas *cs);
int cmd_cd(char *src, csas *cs);
int cmd_file_run(char *src, csas *cs);
int cmd_exec(char *src, csas *cs);
int cmd_map(char *src, csas *cs);
int cmd_alias(char *src, csas *cs);

#endif
