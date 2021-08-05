#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void csas_resize(csas *cs);
uint update_event(csas *cs);
int command_run(char *src, csas *cs);
int cmd_move(char *src, csas *cs);
int cmd_fastselect(char *src, csas *cs);
int cmd_cd(char *src, csas *cs);
int cmd_file_run(char *src, csas *cs);

#endif
