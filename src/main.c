#include "main.h"
#include "csas.h"

int
main(int argc, char **argv)
{
    csas *cs = csas_init();
    csas_run(cs,argc,argv);

    return 0;
}
