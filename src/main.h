#ifndef MAIN_H
#define MAIN_H

#define _GNU_SOURCE
#define _XOPEN_SOURCE_EXTENDED
#define __USE_XOPEN
#define __USE_XOPEN_EXTENDED 1

#ifndef NCURSES_WIDECHAR
#define NCURSES_WIDECHAR 1
#endif

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>
#include <libgen.h>
#include <locale.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <wchar.h>
#include <ctype.h>
#include <sys/mman.h>
#include <ncurses.h>

#include "flexarr.h"

#define DIR_INCR 32
#define BINDINGS_INCR 8
#define FUNCTIONS_INCR 8
#define HISTORY_INCR 8
#define BINDING_KEY_MAX 64
#define FUNCTIONS_NAME_MAX 256
#define LLINE_MAX (1<<12)
#define NUM_MAX 32
#define HISTORY_MAX 32
#define HEIGHT LINES-2
#define TABS 10
#define EXEC_ARGS_MAX (1<<7)
#define BUFFER_MAX (1<<16)
#define SIG_MAX (1<<10)

#define TAB(x) ((xdir*)cs->dirs->v)[cs->tabs[x].t]
#define CTAB TAB(cs->ctab)
#define BINDINGS ((xbind*)cs->bindings->v)
#define FUNCTIONS ((xfunc*)cs->functions->v)

#define SEL_C A_REVERSE|A_BOLD
#define REG_C DEFAULT
#define EXEC_C GREEN
#define DIR_C BLUE
#define LINK_C CYAN
#define CHR_C MAGENTA
#define BLK_C MAGENTA
#define FIFO_C BLUE
#define SOCK_C MAGENTA
#define MISSING_C MAGENTA
#define OTHER_C DEFAULT
#define ERROR_C A_BOLD|COLOR_PAIR(RED)
#define BAR_C GREEN

#define MOVE_SET 0x0
#define MOVE_DOWN 0x1
#define MOVE_UP 0x2

#define D_CHDIR 0x1 //change directory
#define D_MODE_ONCE 0x2 //load only once
#define D_MODE_CHANGE 0x4 //load when ctime has changed
#define D_RECURSIVE 0x8

#define T_EXISTS 0x1

//flags for spawn()
#define F_SILENT    0x1  //duplicate stdout and stderr with /dev/null
#define F_NORMAL    0x2  //close ncurses screen before executing
#define F_CONFIRM   0x4  //wait for the user confirmation after process is done
#define F_WAIT      0x8  //wait for process
#define F_MULTI     0x10 //split for arguments like system()
#define F_BIN       0x20

#define M_REPLACE 0x1 //replace file
#define M_MERGE   0x2 //merge directory
#define M_DCPY    0x4 //don't copy if file exists
#define M_CHNAME  0x8 //if file exists change name

//flags for getting directory size
#define D_F 0x1 //as file
#define D_R 0x2 //recursive
#define D_C 0x4 //count files
#define D_S 0x8 //size

#define SLINK_TO_DIR 0x1
#define SLINK_MISSING 0x2

#define ret_errno(x,y,z) if (x) { errno = (y); return (z); }
#define while_is(w,x,y,z) while ((y) < (z) && (w)((x)[(y)])) {(y)++;}
#define while_isnt(w,x,y,z) while ((y) < (z) && !(w)((x)[(y)])) {(y)++;}
#define toggleflag(x,y,z) if (x) (y) |= (z); else (y) &= ~(z)

typedef long int li;
typedef long long int ll;
typedef long double ldb;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long int uli;
typedef unsigned long long int ull;

enum {
    DEFAULT, RED, GREEN, YELLOW,
    BLUE, CYAN, MAGENTA, WHITE,
    BLACK
};

typedef struct {
    uchar flags;
    off_t offset;
    int whence;
    char *sig;
    ushort len;
    char *path;
} fsig;

typedef struct {
    int *keys;
    char *value;
} xbind;

typedef struct {
    char *name;
    uchar type;
    void *func;
} xfunc;

typedef struct {
    uchar sel[TABS];
    char *name;
    off_t size;
    mode_t mode;
    ushort nlen;
    uchar flags;
} xfile;

typedef struct {
    struct timespec ctime;
    char *path;
    xfile *files;
    size_t size;
    size_t scroll;
    size_t sel;
    ushort plen;
} xdir;

typedef struct {
    size_t t;
    uchar sel;
    uchar flags;
} tab;

typedef struct {
    tab tabs[TABS];
    flexarr *dirs;
    flexarr *bindings;
    flexarr *functions;
    flexarr *consoleh;
    WINDOW *win;
    char typed[NUM_MAX];
    uchar ctab;
} csas;

#endif
