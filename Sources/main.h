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
#include <pthread.h>
#include <linux/limits.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/inotify.h>
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
#define DIR_BASE_STABLE_RATE 65536

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
// ONLY FOR BARS
#define B_UHNAME        0x400000
#define B_DIR           0x800000
#define B_NAME          0x1000000
#define B_WORKSPACES    0x2000000
#define B_POSITION      0x4000000
#define B_FTYPE         0x8000000
#define B_SFTYPE        0x10000000
#define B_FBSIZE        0x20000000
#define B_FBLOCKS       0x40000000
#define B_FHBLOCKS      0x80000000
#define B_FBFREE        0x100000000
#define B_FHBFREE       0x200000000
#define B_FBAVAIL       0x400000000
#define B_FHBAVAIL      0x800000000
#define B_FFILES        0x1000000000
#define B_FFFREE        0x2000000000
#define B_FFSID         0x4000000000
#define B_FNAMELEN      0x8000000000
#define B_FFRSIZE       0x10000000000
#define B_FFLAGS        0x20000000000
#define B_FGROUP        0x40000000000
#define B_MODES         0x80000000000

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
    unsigned int flags;
    unsigned char *List;
    ino_t inode;

    #ifdef __FILE_SIZE_ENABLE__
    unsigned long long int size;
    #endif

    #ifdef __MTIME_ENABLE__
    time_t mtime;
    #endif
    #ifdef __ATIME_ENABLE__
    time_t atime;
    #endif
    #ifdef __CTIME_ENABLE__
    time_t ctime;
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
    size_t El_t;
    struct Element* El;
    pthread_t thread;
    bool enable;
    size_t *selected;
    size_t *Ltop;
    int fd;
    int wd;
    unsigned char sort_m;
};

typedef struct
{
    bool Visual;
    unsigned char SelectedGroup;
    bool exists;
    struct Dir* win[3];
} WorkSpace;

typedef struct
{
    bool Threads;
    char* shell;
    char* Values;
    char* editor;
    long long int BarSettings;
    char* UserHostPattern;
    int CopyBufferSize;
    uint32_t INOTIFY_MASK;
    int MoveOffSet;
    bool WrapScroll;
    bool JumpScroll;
    int JumpScrollValue;
    bool UserRHost;
    bool StatusBarOnTop;
    bool Win1Enable;
    bool Win1Display;
    bool Win3Enable;
    bool Win3Display;
    bool Bar1Enable;
    bool Bar2Enable;
    float* WinSizeMod;
    bool Borders;
    bool FillBlankSpace;
    int* WindowBorder;
    bool EnableColor;
    int DelayBetweenFrames;
    bool NumberLines;
    bool NumberLinesOff;
    bool NumberLinesFromOne;
    int DisplayingC;
    #ifdef __SHOW_HIDDEN_FILES_ENABLE__
    bool ShowHiddenFiles;
    #endif
    #ifdef __SORT_ELEMENTS_ENABLE__
    unsigned char SortMethod;
    int* BetterFiles;
    #endif
    #ifdef __BLOCK_SIZE_ELEMENTS_ENABLE__
    size_t BlockSize;
    #endif
    char DirSizeMethod;
    int C_Error;
    #ifdef __COLOR_FILES_BY_EXTENSION__
    int C_FType_A;
    int C_FType_I;
    int C_FType_V;
    #endif
    int C_Selected;
    int C_Exec_set;
    int C_Exec;
    int C_BLink;
    int C_Dir;
    int C_Reg;
    int C_Fifo;
    int C_Sock;
    int C_Dev;
    int C_BDev;
    int C_LDir;
    int C_LReg;
    int C_LFifo;
    int C_LSock;
    int C_LDev;
    int C_LBDev;
    int C_Other;
    int C_User_S_D;
    int C_Bar_Dir;
    int C_Bar_Name;
    int C_Bar_WorkSpace;
    int C_Bar_WorkSpace_Selected;
    int C_Group_0;
    int C_Group_1;
    int C_Group_2;
    int C_Group_3;
    int C_Group_4;
    int C_Group_5;
    int C_Group_6;
    int C_Group_7;
} Settings;

typedef struct
{
    int wx, wy, WinMiddle;
    WINDOW *win[5];
    size_t ActualSize;
    size_t AllocatedSize;
    struct Dir* Base;
    int inW;
    WorkSpace Work[WORKSPACE_N];
    char* NameHost;
    #ifdef __FILESYSTEM_INFORMATION_ENABLE__
    struct statfs fs;
    #endif
} Basic;

typedef struct {
    char* keys;
    int act;
    union {
        void* v;
        double d;
        long long int ll;
    } slc1, slc2;
} Key;

struct SetEntry {
    char* name;
    char type;
    void* value;
};

struct AliasesT {
    char* name;
    long long int v;
};
