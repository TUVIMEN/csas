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

#define DIR_INC_RATE 128
#define DIR_BASE_STABLE_RATE 64

#define TEMPTEMP "/tmp/CSAS-XXXXXX"

#define G_D(x,y) cs->base[cs->ws[x].win[y]] // GET_DIR
#define G_S(x,y) G_D(x,y)->selected[x]      // GET_SELECTED
#define G_ES(x,y) G_D(x,y)->el[G_S(x,y)]    // GET_ESELECTED

#define F_SILENT 0x1
#define F_NORMAL 0x2
#define F_CONFIRM 0x4
#define F_WAIT 0x8

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

    #define SORT_SIZE 33
    #define SORT_NAME 34
    #define SORT_LNAME 35
    #define SORT_MTIME 36
    #define SORT_ATIME 37
    #define SORT_CTIME 38
    #define SORT_GID 39
    #define SORT_UID 40

    #define SORT_IF 1073741823
    #define SORT_BETTER_FILES 1073741824
    #define SORT_REVERSE 2147483648

#endif

#define D_F 0x1 //like file
#define D_R 0x2 //recursive
#define D_C 0x4 //count
#define D_H 0x8 //human readable

#define PREV_DIR 0x1
#define PREV_FILE 0x2
#define PREV_BINARY 0x4
#define PREV_ASCII 0x8

#define F_TEXT 0x2
#define F_WRAP 0x4

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

#define GROUP(x)  1<<x
#define M_REPLACE 0x1 //replace file
#define M_MERGE   0x2 //merge directory
#define M_DCPY    0x4 //don't copy if file exists
#define M_CHNAME  0x8 //if file exists change name

struct Element
{
    char *name;

    uchar list[WORKSPACE_N];

    ushort nlen;

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
    uli spreview;
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

typedef struct
{
    #ifdef __THREADS_FOR_DIR_ENABLE__
    li ThreadsForDir;
    #endif
    #ifdef __THREADS_FOR_FILE_ENABLE__
    li ThreadsForFile;
    #endif
    #ifdef __LOAD_CONFIG_ENABLE__
    li config_load;
    #endif
    char *shell;
    char *Values;
    char *editor;
    char *FileOpener;
    char *UserHostPattern;
    li Bar1Settings;
    li Bar2Settings;
    li CopyBufferSize;
    double MoveOffSet;
    li WrapScroll;
    li JumpScroll;
    double JumpScrollValue;
    li UserRHost;
    li StatusBarOnTop;
    li Win1Enable;
    li Win1Display;
    li Win3Enable;
    li Win3Display;
    li Bar1Enable;
    li Bar2Enable;
    double *WinSizeMod;
    li Borders;
    char *BinaryPreview;
    li FillBlankSpace;
    li *WindowBorder;
    li EnableColor;
    li DelayBetweenFrames;
    li SDelayBetweenFrames;
    li DirLoadingMode;
    li NumberLines;
    li NumberLinesOff;
    li NumberLinesFromOne;
    li DisplayingC;
    ll PreviewSettings;
    #ifdef __SORT_ELEMENTS_ENABLE__
    li SortMethod;
    li *BetterFiles;
    #endif
    li DirSizeMethod;
    li C_Error;
    #ifdef __COLOR_FILES_BY_EXTENSION__
    li C_FType_A;
    li C_FType_I;
    li C_FType_V;
    #endif
    li C_Selected,C_Exec_set,C_Exec_col,C_Dir,
        C_Reg,C_Fifo,C_Sock,C_Dev,C_BDev,C_Other,C_FileMissing,
        C_SymLink,C_User_S_D,C_Bar_WorkSpace_Selected,
        C_Bar_Dir,C_Bar_Name,C_Bar_WorkSpace, *C_Group,
        C_Bar_F,C_Bar_E,C_Borders;
} Settings;

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
        char* *history;
    } consolehistory;
    struct
    {
        size_t allocated;
        size_t size;
        size_t inc_r;
        size_t pos;
        char* *list;
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

typedef struct {
    int x;
    int y;
} Vector2i;

typedef struct {
    float x;
    float y;
} Vector2f;

struct WinArgs {
    WINDOW *place;
    Vector2i s_size;
    Vector2f p_size;
    Vector2i min_size;
    Vector2i max_size;
    Vector2i s_pos;
    Vector2f p_pos;
    Vector2i min_pos;
    Vector2i max_pos;
    int cfg;
};
