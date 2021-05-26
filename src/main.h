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

#ifndef MAIN_H
#define MAIN_H

#define _GNU_SOURCE

#ifndef NCURSES_WIDECHAR
#define NCURSES_WIDECHAR 1
#endif

#define __USE_XOPEN
#define _XOPEN_SOURCE 600

#ifndef __USE_XOPEN_EXTENDED
#define __USE_XOPEN_EXTENDED 1
#endif

#include "hconfig.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/time.h>
#include <pwd.h>
#include <grp.h>
#include <ncurses.h>
#include <sys/types.h>
#include <limits.h>
#include <regex.h>
#include <err.h>
#include <sys/mman.h>
#include <locale.h>
#ifdef __THREADS_ENABLE__
#include <pthread.h>
#endif
#include <linux/limits.h>
#include <signal.h>
#include <sys/types.h>
#ifdef __FILE_OWNERS_ENABLE__
#include <pwd.h>
#endif
#ifdef __FILE_GROUPS_ENABLE__
#include <grp.h>
#endif
#ifdef __FILESYSTEM_INFO_ENABLE__
#include <sys/statfs.h>
#endif

typedef long int li;
typedef long long int ll;
typedef long double ldb;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long int uli;
typedef unsigned long long int ull;

#ifndef NAME_MAX
#define NAME_MAX 256
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define TEMPTEMP "/tmp/CSAS-XXXXXX"

#define CSAS_ECNF 1
#define CSAS_EWE_A 2
#define CSAS_EWE_P 3
#define CSAS_EWE_I 4
#define CSAS_EWE_F 5
#define CSAS_ENOP 6

#define G_D(x,y) cs->base[cs->ws[x].win[y]] // GET_DIR
#define G_S(x,y) G_D(x,y)->selected[x]      // GET_SELECTED
#define G_ES(x,y) G_D(x,y)->el[G_S(x,y)]    // GET_ESELECTED

#define F_SILENT 0x1
#define F_NORMAL 0x2
#define F_CONFIRM 0x4
#define F_WAIT 0x8
#define F_MULTI 0x10

#define T_DIR 1
#define T_REG 2
#define T_DEV 3
#define T_BDEV 4
#define T_SOCK 5
#define T_FIFO 6
#define T_OTHER 7
#define T_GT 7
#define T_SYMLINK 8
#define T_FILE_MISSING 16

#ifdef __SORT_ELEMENTS_ENABLE__

    #define SORT_NONE 0
    #define SORT_TYPE 1
    #define SORT_SIZE 2
    #define SORT_NAME 3
    #define SORT_LNAME 4
    #define SORT_MTIME 5
    #define SORT_ATIME 6
    #define SORT_CTIME 7
    #define SORT_GID 8
    #define SORT_UID 9
    #define SORT_ZNAME 10
    #define SORT_LZNAME 10

    #define SORT_IF 1073741823
    #define SORT_BETTER_FILES 1073741824
    #define SORT_REVERSE 2147483648

#endif

#define D_F 0x1 //like file
#define D_R 0x2 //recursive
#define D_C 0x4 //count
#define D_S 0x8 //size
#define D_H 0x10 //human readable

#define PREV_DIR 0x1
#define PREV_FILE 0x2
#define PREV_BINARY 0x4
#define PREV_ASCII 0x8

#define F_TEXT 0x2
#define F_WRAP 0x4
#ifdef __THREADS_FOR_FILE_ENABLE__
#define F_TREAD_ENABLE 0x80
#endif

#define DP_SIZE         0x1
#define DP_LSPERMS      0x2
#define DP_BLOCKS       0x4
#define DP_TYPE         0x8
#define DP_MTIME        0x10
#define DP_SMTIME       0x20
#define DP_ATIME        0x40
#define DP_SATIME       0x80
#define DP_CTIME        0x100
#define DP_SCTIME       0x200
#define DP_FTYPE        0x400
#define DP_GRNAME       0x800
#define DP_GRGID        0x1000
#define DP_GRPASSWD     0x2000
#define DP_PWNAME       0x4000
#define DP_PWDIR        0x8000
#define DP_PWGECOS      0x10000
#define DP_PWGID        0x20000
#define DP_PWPASSWD     0x40000
#define DP_PWSHELL      0x80000
#define DP_PWUID        0x100000
#define DP_HSIZE        0x200000
#define DP_NLINK        0x400000
#define DP_BLK_SIZE     0x800000
#define DP_DEV          0x1000000
#define DP_RDEV         0x2000000
#define DP_INODE        0x4000000
#define DP_LINK_PATH    0x8000000

#define B_UHNAME        0x1
#define B_DIR           0x2
#define B_NAME          0x4
#define B_WORKSPACES    0x8
#define B_POSITION      0x10
#define B_FTYPE         0x20
#define B_SFTYPE        0x40
#define B_FBSIZE        0x80
#define B_FBLOCKS       0x100
#define B_FHBLOCKS      0x200
#define B_FBFREE        0x400
#define B_FHBFREE       0x800
#define B_FBAVAIL       0x1000
#define B_FHBAVAIL      0x2000
#define B_FFILES        0x4000
#define B_FFFREE        0x8000
#define B_FFSID         0x10000
#define B_FNAMELEN      0x20000
#define B_FFRSIZE       0x40000
#define B_FFLAGS        0x80000
#define B_FGROUP        0x100000
#define B_MODES         0x200000
#define B_CSF           0x400000

#define SET_T_B 1 //bool
#define SET_T_I 2 //int
#define SET_T_UI 3 //unsigned int
#define SET_T_F 4 //float
#define SET_T 7
#define SET_T_P 8 //pointer
#define SET_T_A 16 //array

#define GROUP(x)  1<<x
#define M_REPLACE 0x1 //replace file
#define M_MERGE   0x2 //merge directory
#define M_DCPY    0x4 //don't copy if file exists
#define M_CHNAME  0x8 //if file exists change name

struct Element
{
    char *name;
    ushort nlen;
    uchar list[WORKSPACE_N];
    uchar type;
    
    #ifdef __COLOR_FILES_BY_EXTENSION__
    uchar ftype;
    #endif

    #ifdef __MODE_ENABLE__
    mode_t mode;
    #endif

    #ifdef __INODE_ENABLE__
    ino_t inode;
    #endif

    #ifdef __FILE_SIZE_ENABLE__
    ull size;
    #endif

    #ifdef __ATIME_ENABLE__
    time_t atim;
    #endif
    #ifdef __MTIME_ENABLE__
    time_t mtim;
    #endif
    #ifdef __CTIME_ENABLE__
    time_t ctim;
    #endif

    #ifdef __SAVE_PREVIEW__
    uchar *cpreview;
    ssize_t previewl;
    uchar spreview;
    #endif

    #ifdef __FILE_GROUPS_ENABLE__
    gid_t gr;
    #endif
    #ifdef __FILE_OWNERS_ENABLE__
    uid_t pw;
    #endif

    #ifdef __DEV_ENABLE__
    dev_t dev;
    #endif
    #ifdef __NLINK_ENABLE__
    nlink_t nlink;
    #endif
    #ifdef __RDEV_ENABLE__
    dev_t rdev;
    #endif
    #ifdef __BLK_SIZE_ENABLE__
    blksize_t blksize;
    #endif
    #ifdef __BLOCKS_ENABLE__
    blkcnt_t blocks;
    #endif
};

#ifdef __COLOR_FILES_BY_EXTENSION__
typedef struct
{
    char *name;
    char group;
} Extensions;
#endif

typedef struct
{
    bool run_in_bg;
    bool binary;
    long pos;
    int from;
    char *sig;
    int len;
    char *comma_com;
} FileSignatures;

#endif

struct Dir
{
    char *path;
    size_t size;
    size_t oldsize;
    struct Element *el;
    size_t *selected;
    char **move_to;
    size_t *ltop;
    struct timespec ctime;
    ino_t inode;
    char *filter;
    li sort_m;
    #ifdef __THREADS_FOR_DIR_ENABLE__
    pthread_t thread;
    bool enable;
    #endif
    bool permission_denied;
    bool changed;
    bool filter_set;
};

typedef struct
{
    char *path;
    int win[3];
    bool show_message;
    bool visual;
    bool exists;
    uchar sel_group;
} WorkSpace;

#pragma pack(1)
struct option
{
    char *n;
    uchar t;
    void *v;
};

typedef struct
{
    int wx, wy, win_middle;
    WINDOW *win[6];
    size_t size;
    size_t asize;
    struct Dir* *base;
    int current_ws;
    WorkSpace ws[WORKSPACE_N];
    #ifdef __USER_NAME_ENABLE__
    char *usern;
    char *hostn;
    #endif
    bool exit_time;
    char *typed_keys;
    bool was_typed;
    #ifdef __FILESYSTEM_INFO_ENABLE__
    struct statfs fs;
    #endif
    struct
    {
        size_t allocated;
        size_t size;
        size_t max_size;
        size_t inc_r;
        size_t alloc_r;
        char **history;
    } consolehistory;
    struct
    {
        size_t allocated;
        size_t size;
        size_t inc_r;
        size_t pos;
        char **list;
    } SearchList;
    #ifndef __SAVE_PREVIEW__
    uchar *cpreview;
    ssize_t previewl;
    uli spreview;
    #endif
} Csas;

typedef struct {
    wchar_t *keys;
    char *value;
} Key;

struct SetEntry {
    char *name;
    void *value;
};

struct AliasesT {
    char *name;
    ll v;
};

struct rla
{
    union {size_t s;char *p;} *a;
    size_t s;
    size_t as;
    void (*sfree)(struct rla*);
};

struct WinArgs {
    WINDOW *place;
    short int x;
    short int y;
    int s_sizex;
    int s_sizey;
    float p_sizex;
    float p_sizey;
    int min_sizex;
    int min_sizey;
    int max_sizex;
    int max_sizey;
    int s_posx;
    int s_posy;
    float p_posx;
    float p_posy;
    int min_posx;
    int min_posy;
    int max_posx;
    int max_posy;
    int cfg;
};

struct command
{
    char *name;
    uchar type;
    void *func;
    int (*expand)(WINDOW*,char*,size_t,short int, bool*,struct rla*,struct WinArgs*,char**);
};