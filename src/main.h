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
#include <locale.h>
#include <sys/mman.h>
#ifdef __THREADS_ENABLE__
#include <pthread.h>
#endif
#include <linux/limits.h>
#include <signal.h>
#include <sys/types.h>
#ifdef __INOTIFY_ENABLE__
#include <sys/inotify.h>
#endif
#ifdef __FILE_OWNERS_ENABLE__
#include <pwd.h>
#endif
#ifdef __FILE_GROUPS_ENABLE__
#include <grp.h>
#endif
#ifdef __FILESYSTEM_INFORMATION_ENABLE__
#include <sys/statfs.h>
#endif

#ifndef NAME_MAX
#define NAME_MAX 256
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#ifdef __HUMAN_READABLE_SIZE_ENABLE__
    #define MAX_END_READABLE 4
#endif

#define DIR_INC_RATE 64
#define DIR_BASE_STABLE_RATE 32

#define TEMPTEMP "/tmp/CSAS-XXXXXX"

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

#define GROUP_0 0x1
#define GROUP_1 0x2
#define GROUP_2 0x4
#define GROUP_3 0x8
#define GROUP_4 0x10
#define GROUP_5 0x20
#define GROUP_6 0x40
#define GROUP_7 0x80

#define M_REPLACE 0x1 //replace file
#define M_MERGE   0x2 //merge directory
#define M_DCPY    0x4 //don't copy if file exists
#define M_CHNAME  0x8 //if file exists change name

struct ShortDir
{
    char* cwd;
    char** names;
    size_t names_t;
};

struct Element
{
    char* name;
    unsigned char Type;
    #ifdef __MODE_ENABLE__
    unsigned int flags;
    #endif
    unsigned char *List;
    #ifdef __INOTIFY_ENABLE__
    ino_t inode;
    #endif

    #ifdef __FILE_SIZE_ENABLE__
    unsigned long long int size;
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
    unsigned char FType;
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

    #ifdef __HUMAN_READABLE_SIZE_ENABLE__
    char* SizErrToDisplay;
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
    long long int El_t;
    struct Element* El;
    #ifdef __THREADS_FOR_DIR_ENABLE__
    pthread_t thread;
    bool enable;
    #endif
    size_t *selected;
    size_t *Ltop;
    #ifdef __INOTIFY_ENABLE__
    int fd;
    int wd;
    bool Changed;
    #endif
    unsigned char sort_m;
};

typedef struct
{
    bool Visual;
    unsigned char SelectedGroup;
    bool exists;
    int win[3];
} WorkSpace;

typedef struct
{
    #ifdef __THREADS_FOR_DIR_ENABLE__
    long int ThreadsForDir;
    #endif
    #ifdef __THREADS_FOR_FILE_ENABLE__
    long int ThreadsForFile;
    #endif
    char* shell;
    char* Values;
    char* editor;
    char* FileOpener;
    char* UserHostPattern;
    long int Bar1Settings;
    long int Bar2Settings;
    long int CopyBufferSize;
    #ifdef __INOTIFY_ENABLE__
    long int INOTIFY_MASK;
    #endif
    double MoveOffSet;
    long int WrapScroll;
    long int JumpScroll;
    double JumpScrollValue;
    long int UserRHost;
    long int StatusBarOnTop;
    long int Win1Enable;
    long int Win1Display;
    long int Win3Enable;
    long int Win3Display;
    long int Bar1Enable;
    long int Bar2Enable;
    double* WinSizeMod;
    long int Borders;
    long int FillBlankSpace;
    long int* WindowBorder;
    long int EnableColor;
    long int DelayBetweenFrames;
    long int SDelayBetweenFrames;
    long int DirLoadingMode;
    long int NumberLines;
    long int NumberLinesOff;
    long int NumberLinesFromOne;
    long int DisplayingC;
    #ifdef __SHOW_HIDDEN_FILES_ENABLE__
    long int ShowHiddenFiles;
    #endif
    #ifdef __SORT_ELEMENTS_ENABLE__
    unsigned char SortMethod;
    long int* BetterFiles;
    #endif
    char DirSizeMethod;
    long int C_Error;
    #ifdef __COLOR_FILES_BY_EXTENSION__
    long int C_FType_A;
    long int C_FType_I;
    long int C_FType_V;
    #endif
    long int C_Selected,C_Exec_set,C_Exec,C_BLink,C_Dir,C_Reg,C_Fifo,C_Sock,C_Dev,C_BDev,C_LDir,C_LReg,C_LFifo,C_LSock,C_LDev,C_LBDev,C_Other,C_User_S_D,C_Bar_Dir,C_Bar_Name,C_Bar_WorkSpace,C_Bar_WorkSpace_Selected,C_Group_0,C_Group_1,C_Group_2,C_Group_3,C_Group_4,C_Group_5,C_Group_6,C_Group_7,C_Bar_F,C_Bar_E,C_Borders;
} Settings;

#define F_TEXT 0x1
#define F_WRAP 0x2

typedef struct
{
    int wx, wy, WinMiddle;
    WINDOW *win[6];
    size_t ActualSize;
    size_t AllocatedSize;
    struct Dir* Base;
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
    int preview_fd;
    long int FastRunSettings;
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
    long long int v;
};
