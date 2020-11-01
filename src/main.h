/*
    csas - console file manager
    Copyright (C) 2020 TUVIMEN <suchora.dominik7@gmail.com>

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

#include "HardConfig.h"

#include <stdlib.h>
#include <stdio.h>
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
#include <locale.h>
#include <sys/mman.h>
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
#ifdef __FILESYSTEM_INFORMATION_ENABLE__
#include <sys/statfs.h>
#endif

typedef long int li;
typedef long long int ll;
typedef long double ldb;
typedef unsigned char uchar;
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

#define GET_DIR(x,y) grf->Base[grf->Work[x].win[y]]
#define GET_SELECTED(x,y) GET_DIR(x,y)->selected[x]
#define GET_ESELECTED(x,y) GET_DIR(x,y)->El[GET_SELECTED(x,y)]

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
#define T_BLINK 7
#define T_LDIR 8
#define T_LREG 9
#define T_LDEV 10
#define T_LBDEV 11
#define T_LSOCK 12
#define T_LFIFO 13
#define T_OTHER 14

#ifdef __SORT_ELEMENTS_ENABLE__

    #define SORT_NONE 0
    #define SORT_TYPE 1

    #define SORT_CHIR 32
    #define SORT_SIZE 33
    #define SORT_NAME 34
    #define SORT_LNAME 35
    #define SORT_MTIME 36
    #define SORT_ATIME 37
    #define SORT_CTIME 38
    #define SORT_GID 39
    #define SORT_UID 40

    #define SORT_REVERSE 128

#endif

#define D_F 0x1 //like file
#define D_R 0x2 //recursive
#define D_C 0x4 //count
#define D_H 0x8 //human readable

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

#define GROUP(x) 1<<x
#define M_REPLACE 0x1 //replace file
#define M_MERGE   0x2 //merge directory
#define M_DCPY    0x4 //don't copy if file exists
#define M_CHNAME  0x8 //if file exists change name

struct Element
{
    char* name;
    uchar Type;
    #ifdef __MODE_ENABLE__
    unsigned int flags;
    #endif
    uchar *List;
    #ifdef __INODE_ENABLE__
    ino_t inode;
    #endif

    #ifdef __FILE_SIZE_ENABLE__
    ull size;
    #endif

    #ifdef __ATIME_ENABLE__
    struct timespec atim;
    #endif
    #ifdef __MTIME_ENABLE__
    struct timespec mtim;
    #endif
    #ifdef __CTIME_ENABLE__
    struct timespec ctim;
    #endif

    #ifdef __COLOR_FILES_BY_EXTENSION__
    uchar FType;
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
    char* Name;
    char group;
} Extensions;
#endif

typedef struct
{
    bool RunInBG;
    bool binary;
    long pos;
    int from;
    char* sig;
    int len;
    char* comma_com;
} FileSignatures;

#endif

struct Dir
{
    char* path;
    ll El_t;
    ll oldEl_t;
    struct Element* El;
    #ifdef __THREADS_FOR_DIR_ENABLE__
    pthread_t thread;
    bool enable;
    #endif
    size_t *selected;
    size_t *Ltop;
    uchar sort_m;
    struct timespec ctime;
    ino_t inode;
    bool Changed;
    bool filter_set;
    char* filter;
};

typedef struct
{
    bool Visual;
    uchar SelectedGroup;
    bool exists;
    int win[3];
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
    li LoadConfig;
    #endif
    char* shell;
    char* Values;
    char* editor;
    char* FileOpener;
    char* UserHostPattern;
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
    double* WinSizeMod;
    li Borders;
    li FillBlankSpace;
    li* WindowBorder;
    li EnableColor;
    li DelayBetweenFrames;
    li SDelayBetweenFrames;
    li DirLoadingMode;
    li NumberLines;
    li NumberLinesOff;
    li NumberLinesFromOne;
    li DisplayingC;
    #ifdef __SHOW_HIDDEN_FILES_ENABLE__
    li ShowHiddenFiles;
    #endif
    #ifdef __SORT_ELEMENTS_ENABLE__
    li SortMethod;
    li* BetterFiles;
    #endif
    li DirSizeMethod;
    li C_Error;
    #ifdef __COLOR_FILES_BY_EXTENSION__
    li C_FType_A;
    li C_FType_I;
    li C_FType_V;
    #endif
    li C_Selected,C_Exec_set,C_Exec,C_BLink,C_Dir,
        C_Reg,C_Fifo,C_Sock,C_Dev,C_BDev,C_LDir,C_LReg,
        C_LFifo,C_LSock,C_LDev,C_LBDev,C_Other,C_User_S_D,
        C_Bar_Dir,C_Bar_Name,C_Bar_WorkSpace,C_Bar_WorkSpace_Selected,
        *C_Group,C_Bar_F,C_Bar_E,C_Borders;
} Settings;

#define F_TEXT 0x1
#define F_WRAP 0x2

typedef struct
{
    int wx, wy, WinMiddle;
    WINDOW *win[6];
    size_t ActualSize;
    size_t AllocatedSize;
    struct Dir** Base;
    int inW;
    WorkSpace Work[WORKSPACE_N];
    #ifdef __USER_NAME_ENABLE__
    char* H_User;
    char* H_Host;
    #endif
    bool ExitTime;
    char* cSF;
    bool cSF_E;
    #ifdef __FILESYSTEM_INFORMATION_ENABLE__
    struct statfs fs;
    #endif
    struct
    {
        size_t allocated;
        size_t size;
        size_t max_size;
        size_t inc_r;
        size_t alloc_r;
        char** History;
    } ConsoleHistory;
    struct
    {
        size_t allocated;
        size_t size;
        size_t inc_r;
        size_t pos;
        char** List;
    } SearchList;
    int preview_fd;
    li FastRunSettings;
} Basic;

typedef struct {
    char* keys;
    char* value;
} Key;

struct SetEntry {
    char* name;
    void* value;
};

struct AliasesT {
    char* name;
    ll v;
};
