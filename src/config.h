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

#ifndef CONFIG_H
#define CONFIG_H

struct AliasesT aliases[] = {
    {"false",0},{"true",1},{"COL_0",COLOR_PAIR(0x0)},{"COL_1",COLOR_PAIR(0x1)},
    {"COL_2",COLOR_PAIR(0x2)},{"COL_3",COLOR_PAIR(0x3)},{"COL_4",COLOR_PAIR(0x4)},
    {"COL_5",COLOR_PAIR(0x5)},{"COL_6",COLOR_PAIR(0x6)},{"COL_7",COLOR_PAIR(0x7)},
    {"COL_8",COLOR_PAIR(0x8)},{"COL_9",COLOR_PAIR(0x9)},{"COL_A",COLOR_PAIR(0xa)},
    {"COL_B",COLOR_PAIR(0xb)},{"COL_C",COLOR_PAIR(0xc)},{"COL_D",COLOR_PAIR(0xd)},
    {"COL_E",COLOR_PAIR(0xe)},{"COL_F",COLOR_PAIR(0xf)},{"A_BOLD",A_BOLD},{"A_REVERSE",A_REVERSE},
    {"A_UNDERLINE",A_UNDERLINE},{"A_VERTICAL",A_VERTICAL},{"A_HORIZONTAL",A_HORIZONTAL},
    {"A_INVIS",A_INVIS},{"A_ITALIC",A_ITALIC},{"A_LEFT",A_LEFT},{"A_NORMAL",A_NORMAL},
    {"A_PROTECT",A_PROTECT},{"A_UNDERLINE",A_UNDERLINE},{"A_CHARTEXT",A_CHARTEXT},{"A_RIGHT",A_RIGHT},
    {"A_TOP",A_TOP},{"A_LOW",A_LOW},{"A_STANDOUT",A_STANDOUT},{"A_DIM",A_DIM},{"A_BLINK",A_BLINK},
    {"PREV_DIR",PREV_DIR},{"PREV_FILE",PREV_FILE},{"PREV_BINARY",PREV_BINARY},{"PREV_ASCII",PREV_ASCII},
    {"NAME_MAX",NAME_MAX},{"PATH_MAX",PATH_MAX},{"DIR_INC_RATE",DIR_INC_RATE},
    {"DIR_BASE_RATE",DIR_BASE_RATE},{"T_DIR",T_DIR},{"T_REG",T_REG},{"T_DEV",T_DEV},{"T_BDEV",T_BDEV},
    {"T_SOCK",T_SOCK},{"T_FIFO",T_FIFO},{"T_OTHER",T_OTHER},{"T_SYMLINK",T_SYMLINK},{"T_FILE_MISSING",T_FILE_MISSING},
    #ifdef __SORT_ELEMENTS_ENABLE__
    {"SORT_NONE",SORT_NONE},{"SORT_TYPE",SORT_TYPE},{"SORT_SIZE",SORT_SIZE},{"SORT_NAME",SORT_NAME},{"SORT_LNAME",SORT_LNAME},
    #ifdef __MTIME_ENABLE__
    {"SORT_MTIME",SORT_MTIME},
    #endif
    #ifdef __ATIME_ENABLE__
    {"SORT_ATIME",SORT_ATIME},
    #endif
    #ifdef __CTIME_ENABLE__
    {"SORT_CTIME",SORT_CTIME},
    #endif
    {"SORT_GID",SORT_GID},{"SORT_UID",SORT_UID},{"SORT_REVERSE",SORT_REVERSE},
    {"SORT_BETTER_FILES",SORT_BETTER_FILES},
    #endif
    {"D_S",D_S},{"D_F",D_F},{"D_R",D_R},{"D_C",D_C},{"D_H",D_H},{"M_REPLACE",M_REPLACE},{"M_MERGE",M_MERGE},
    {"M_DCPY",M_DCPY},{"M_CHNAME",M_CHNAME},{"GROUP_0",GROUP(0)},{"GROUP_1",GROUP(1)},{"GROUP_2",GROUP(2)},
    {"GROUP_3",GROUP(3)},{"GROUP_4",GROUP(4)},{"GROUP_5",GROUP(5)},{"GROUP_6",GROUP(6)},{"GROUP_7",GROUP(7)},
    #ifdef __FILE_SIZE_ENABLE__
    {"DP_SIZE",DP_SIZE},
    #endif
    {"DP_LSPERMS",DP_LSPERMS},
    #ifdef __BLOCKS_ENABLE__
    {"DP_BLOCKS",DP_BLOCKS},
    #endif
    #ifdef __NLINK_ENABLE__
    {"DP_NLINK",DP_NLINK},
    #endif
    #ifdef __BLK_SIZE_ENABLE__
    {"DP_BLK_SIZE",DP_BLK_SIZE},
    #endif
    #ifdef __DEV_ENABLE__
    {"DP_DEV",DP_DEV},
    #endif
    #ifdef __RDEV_ENABLE__
    {"DP_RDEV",DP_RDEV},
    #endif
    #ifdef __INODE_ENABLE__
    {"DP_INODE",DP_INODE},
    #endif
    {"DP_TYPE",DP_TYPE},
    #ifdef __MTIME_ENABLE__
    {"DP_MTIME",DP_MTIME},{"DP_SMTIME",DP_SMTIME},
    #endif
    #ifdef __ATIME_ENABLE__
    {"DP_ATIME",DP_ATIME},{"DP_SATIME",DP_SATIME},
    #endif
    #ifdef __CTIME_ENABLE__
    {"DP_CTIME",DP_CTIME},{"DP_SCTIME",DP_SCTIME},
    #endif
    #ifdef __COLOR_FILES_BY_EXTENSION__
    {"DP_FTYPE",DP_FTYPE},
    #endif
    #ifdef __FILE_GROUPS_ENABLE__
    {"DP_GRNAME",DP_GRNAME},{"DP_GRGID",DP_GRGID},{"DP_GRPASSWD",DP_GRPASSWD},
    #endif
    #ifdef __FILE_OWNERS_ENABLE__
    {"DP_PWNAME",DP_PWNAME},{"DP_PWDIR",DP_PWDIR},{"DP_PWGECOS",DP_PWGECOS},{"DP_PWGID",DP_PWGID},{"DP_PWPASSWD",DP_PWPASSWD},{"DP_PWSHELL",DP_PWSHELL},{"DP_PWUID",DP_PWUID},
    #endif
    {"DP_HSIZE",DP_HSIZE},{"B_UHNAME",B_UHNAME},{"B_DIR",B_DIR},{"B_NAME",B_NAME},
    {"B_WORKSPACES",B_WORKSPACES},{"B_POSITION",B_POSITION},
    #ifdef __FILESYSTEM_INFO_ENABLE__
    {"B_FTYPE",B_FTYPE},{"B_SFTYPE",B_SFTYPE},{"B_FBSIZE",B_FBSIZE},{"B_FBLOCKS",B_FBLOCKS},
    {"B_FHBLOCKS",B_FHBLOCKS},{"B_FBFREE",B_FBFREE},{"B_FHBFREE",B_FHBFREE},{"B_FBAVAIL",B_FBAVAIL},
    {"B_FHBAVAIL",B_FHBAVAIL},{"B_FFILES",B_FFILES},{"B_FFFREE",B_FFFREE},{"B_FFSID",B_FFSID},
    {"B_FNAMELEN",B_FNAMELEN},{"B_FFRSIZE",B_FFRSIZE},{"B_FFLAGS",B_FFLAGS},
    #endif
    {"B_MODES",B_MODES},{"B_FGROUP",B_FGROUP},{"B_CSF",B_CSF},{NULL,0}
};

#ifdef __THREADS_FOR_DIR_ENABLE__
li s_ThreadsForDir = 0;
#endif
#ifdef __THREADS_FOR_FILE_ENABLE__
li s_ThreadsForFile = 0;
#endif
#ifdef __LOAD_CONFIG_ENABLE__
li s_config_load = 1;
#endif
char *s_shell=NULL,*s_Values=NULL,*s_editor=NULL,*s_FileOpener=NULL,*s_UserHostPattern=NULL,*s_BinaryPreview=NULL;
double s_MoveOffSet=0.1,s_JumpScrollValue=0.5,*s_WinSizeMod=NULL;
li s_Bar1Settings=B_DIR|B_WORKSPACES|B_POSITION|B_FGROUP|B_MODES|B_CSF,s_PreviewSettings=0,
    s_Bar2Settings=DP_LSPERMS|DP_SMTIME|DP_HSIZE,s_CopyBufferSize=131072,s_WrapScroll=true,s_DisplayingC=0,
    s_JumpScroll=false,s_UserRHost=false,s_PreviewMaxThreads=8,s_StatusBarOnTop=false,s_Win1Enable=false,
    s_Win1Display=false,s_Win3Enable=false,s_Win3Display=false,s_Bar1Enable=true,s_Bar2Enable=true,
    s_Borders=false,s_FillBlankSpace=false,*s_WindowBorder,s_EnableColor=true,s_DelayBetweenFrames=1024,
    s_SDelayBetweenFrames=1,s_DirLoadingMode=0,s_NumberLines=false,s_NumberLinesOff=false,s_NumberLinesFromOne=false;
#ifdef __SORT_ELEMENTS_ENABLE__
li s_SortMethod=SORT_NAME|SORT_BETTER_FILES,*s_BetterFiles=NULL;
#endif
li s_DirSizeMethod=D_F,s_C_Error=COLOR_PAIR(1)|A_BOLD|A_REVERSE;
#ifdef __COLOR_FILES_BY_EXTENSION__
li s_C_FType_A=COLOR_PAIR(1),s_C_FType_I=COLOR_PAIR(3),s_C_FType_V=COLOR_PAIR(5);
#endif
li s_C_Selected=A_REVERSE|A_BOLD,s_C_Exec_set=A_BOLD,s_C_Exec_col=COLOR_PAIR(2),s_C_Dir=COLOR_PAIR(4)|A_BOLD,
    s_C_Reg=A_NORMAL,s_C_Fifo=COLOR_PAIR(3)|A_ITALIC,s_C_Sock=COLOR_PAIR(3)|A_ITALIC,s_C_Dev=COLOR_PAIR(3),
    s_C_BDev=COLOR_PAIR(6),s_C_Other=COLOR_PAIR(0),s_C_FileMissing=COLOR_PAIR(5),
    s_C_SymLink=COLOR_PAIR(6),s_C_User_S_D=COLOR_PAIR(2)|A_BOLD,s_C_Bar_WorkSpace_Selected=COLOR_PAIR(2)|A_REVERSE|A_BOLD,
    s_C_Bar_Dir=COLOR_PAIR(4)|A_BOLD,s_C_Bar_Name=A_NORMAL|A_BOLD,s_C_Bar_WorkSpace=A_NORMAL|A_BOLD,*s_C_Group=NULL,
    s_C_Bar_F=COLOR_PAIR(0),s_C_Bar_E=COLOR_PAIR(0),s_C_Borders=0;

struct option cfg_names[] = {
    #ifdef __THREADS_FOR_DIR_ENABLE__
    {"ThreadsForDir",SET_T_B,&s_ThreadsForDir},
    #endif
    #ifdef __THREADS_FOR_FILE_ENABLE__
    {"ThreadsForFile",SET_T_B,&s_ThreadsForFile},
    #endif
    #ifdef __LOAD_CONFIG_ENABLE__
    {"config_load",SET_T_B,&s_config_load},
    #endif
    {"shell",SET_T_P,&s_shell},
    {"Values",SET_T_P,&s_Values},
    {"editor",SET_T_P,&s_editor},
    {"FileOpener",SET_T_P,&s_FileOpener},
    {"UserHostPattern",SET_T_P,&s_UserHostPattern},
    {"Bar1Settings",SET_T_UI,&s_Bar1Settings},
    {"Bar2Settings",SET_T_UI,&s_Bar2Settings},
    {"CopyBufferSize",SET_T_UI,&s_CopyBufferSize},
    {"MoveOffSet",SET_T_F,&s_MoveOffSet},
    {"WrapScroll",SET_T_B,&s_WrapScroll},
    {"JumpScroll",SET_T_B,&s_JumpScroll},
    {"JumpScrollValue",SET_T_F,&s_JumpScrollValue},
    {"UserRHost",SET_T_B,&s_UserRHost},
    {"PreviewMaxThreads",SET_T_UI,&s_PreviewMaxThreads},
    {"StatusBarOnTop",SET_T_B,&s_StatusBarOnTop},
    {"Win1Enable",SET_T_B,&s_Win1Enable},
    {"Win1Display",SET_T_B,&s_Win1Display},
    {"Win3Enable",SET_T_B,&s_Win3Enable},
    {"Win3Display",SET_T_B,&s_Win3Display},
    {"Bar1Enable",SET_T_B,&s_Bar1Enable},
    {"Bar2Enable",SET_T_B,&s_Bar2Enable},
    {"WinSizeMod",SET_T_F|SET_T_A,&s_WinSizeMod},
    {"Borders",SET_T_F|SET_T_A,&s_Borders},
    {"BinaryPreview",SET_T_P,&s_BinaryPreview},
    {"FillBlankSpace",SET_T_B,&s_FillBlankSpace},
    {"WindowBorder",SET_T_UI|SET_T_A,&s_WindowBorder},
    {"EnableColor",SET_T_B,&s_EnableColor},
    {"DelayBetweenFrames",SET_T_UI,&s_DelayBetweenFrames},
    {"SDelayBetweenFrames",SET_T_UI,&s_SDelayBetweenFrames},
    {"DirLoadingMode",SET_T_UI,&s_DirLoadingMode},
    {"NumberLines",SET_T_B,&s_NumberLines},
    {"NumberLinesOff",SET_T_B,&s_NumberLinesOff},
    {"NumberLinesFromOne",SET_T_B,&s_NumberLinesFromOne},
    {"DisplayingC",SET_T_UI,&s_DisplayingC},
    {"PreviewSettings",SET_T_UI,&s_PreviewSettings},
    #ifdef __SORT_ELEMENTS_ENABLE__
    {"SortMethod",SET_T_UI,&s_SortMethod},
    {"BetterFiles",SET_T_UI|SET_T_A,&s_BetterFiles},
    #endif
    {"DirSizeMethod",SET_T_UI,&s_DirSizeMethod},
    {"C_Error",SET_T_UI,&s_C_Error},
    #ifdef __COLOR_FILES_BY_EXTENSION__
    {"C_FType_A",SET_T_UI,&s_C_FType_A},
    {"C_FType_I",SET_T_UI,&s_C_FType_I},
    {"C_FType_V",SET_T_UI,&s_C_FType_V},
    #endif
    {"C_Selected",SET_T_UI,&s_C_Selected},
    {"C_Exec_set",SET_T_UI,&s_C_Exec_set},
    {"C_Exec_col",SET_T_UI,&s_C_Exec_col},
    {"C_Dir",SET_T_UI,&s_C_Dir},
    {"C_Reg",SET_T_UI,&s_C_Reg},
    {"C_Fifo",SET_T_UI,&s_C_Fifo},
    {"C_Sock",SET_T_UI,&s_C_Sock},
    {"C_Dev",SET_T_UI,&s_C_Dev},
    {"C_BDev",SET_T_UI,&s_C_BDev},
    {"C_Other",SET_T_UI,&s_C_Other},
    {"C_FileMissing",SET_T_UI,&s_C_FileMissing},
    {"C_SymLink",SET_T_UI,&s_C_SymLink},
    {"C_User_S_D",SET_T_UI,&s_C_User_S_D},
    {"C_Bar_WorkSpace_Selected",SET_T_UI,&s_C_Bar_WorkSpace_Selected},
    {"C_Bar_Dir",SET_T_UI,&s_C_Bar_Dir},
    {"C_Bar_Name",SET_T_UI,&s_C_Bar_Name},
    {"C_Bar_WorkSpace",SET_T_UI,&s_C_Bar_WorkSpace},
    {"C_Group",SET_T_UI|SET_T_A,&s_C_Group},
    {"C_Bar_F",SET_T_UI,&s_C_Bar_F},
    {"C_Bar_E",SET_T_UI,&s_C_Bar_E},
    {"C_Borders",SET_T_UI,&s_C_Borders}, 
    {NULL,0,NULL}
};

#ifdef __COLOR_FILES_BY_EXTENSION__
Extensions extensions[] = {
    {"aiff",'V'}, {"mkv",'V'}, {"avi",'V'}, {"flac",'V'}, {"flv",'V'},
    {"m4a",'V'}, {"m4b",'V'}, {"m4p",'V'}, {"mp2",'V'}, {"avchd",'V'},
    {"mp3",'V'}, {"mp4",'V'}, {"mpe",'V'}, {"mpg",'V'}, {"mpv",'V'},
    {"ogg",'V'}, {"mpeg",'V'}, {"pcm",'V'}, {"qt",'V'}, {"swf",'V'},
    {"wav",'V'}, {"aac",'V'}, {"wma",'V'}, {"wmv",'V'}, {"vob",'V'},
    {"mov",'V'}, {"webm",'V'}, {"rvi",'V'}, {"3g2",'V'}, {"3gp",'V'},
    {"aa",'V'}, {"ac3",'V'}, {"mj2",'V'}, {"mpc",'V'}, {"mpc8",'V'},
    {"m4v",'V'}, {"oga",'V'}, {"ogv",'V'}, {"rvi",'V'}, {"alac",'V'},
    {"rmvb",'V'},
    //Video || Audio
    {"gif",'I'}, {"jpeg",'I'}, {"jpg",'I'}, {"png",'I'}, {"raw",'I'},
    {"tiff",'I'}, {"bmp",'I'}, {"tga",'I'}, {"gvs",'I'}, {"webp",'I'},
    {"svg",'I'},
    //Image
    {"tar",'A'}, {"jar",'A'}, {"rar",'A'}, {"bz2",'A'}, {"exe",'A'},
    {"gz",'A'}, {"img",'A'}, {"iso",'A'}, {"lzma",'A'}, {"tbz2",'A'},
    {"tgz",'A'}, {"z",'A'}, {"zx",'A'}, {"zip",'A'}, {"7z",'A'},
    {"xz",'A'},
    //Archive
    {NULL,'\0'}
};
#endif

struct command *commands = NULL;
size_t commandsl = 0;
size_t commandsa = 0;

FileSignatures signatures[] = {
    { 1,     0,     0,      SEEK_SET,       "<svg",4,"eog"},
    { 1,     1,     0,      SEEK_SET,       "%PDF",4,"zathura"},
    { 1,     1,     0,      SEEK_SET,       "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A",8,"sxiv"},
    { 1,     1,     0,      SEEK_SET,       "\xFF\xD8",2,"sxiv"},
    { 1,     1,     0,      SEEK_SET,       "BM",2,"sxiv"},
    { 1,     1,     0,      SEEK_SET,       "GIF87a",6,"sxiv"},
    { 1,     1,     0,      SEEK_SET,       "GIF89a",6,"sxiv"},
    { 1,     1,     0,      SEEK_SET,       "\x49\x49\x2A\x00",4,"sxiv"},
    { 1,     1,     0,      SEEK_SET,       "\x49\x20\x49",3,"sxiv"},
    { 1,     1,     0,      SEEK_SET,       "\x49\x49\x1A\x00\x00\x00\x48\x45\x41\x50\x43\x43\x44\x52\x02\x00",16,"sxiv"},
    { 1,     1,     0,      SEEK_SET,       "\x49\x49\x2A\x00\x10\x00\x00\x00\x43\x52",10,"sxiv"},
    { 1,     1,     0,      SEEK_SET,       "WEBP",4,"sxiv"},
    { 0,     1,     0,      SEEK_SET,       "\x4F\x67\x67\x53",4,"mpv"},
    { 0,     1,     0,      SEEK_SET,       "\x52\x49\x46",3,"mpv"},
    { 0,     1,     0,      SEEK_SET,       "\x57\x41\x56\x45",4,"mpv"},
    { 0,     1,     0,      SEEK_SET,       "\x41\x56\x49\x20",4,"mpv"},
    { 0,     1,     0,      SEEK_SET,       "\x30\x26\xB2\x75\x8E\x66\xCF\x11",8,"mpv"},
    { 0,     1,     0,      SEEK_SET,       "\xA6\xD9\x00\xAA\x00\x62\xCE\x6C",8,"mpv"},
    { 0,     1,     0,      SEEK_SET,       "\xFF\xFB",2,"mpv"},
    { 0,     1,     0,      SEEK_SET,       "\xFF\xF2",2,"mpv"},
    { 0,     1,     0,      SEEK_SET,       "\xFF\xF3",2,"mpv"},
    { 0,     1,     0,      SEEK_SET,       "WAVEfmt ",8,"mpv"},
    { 0,     1,     0,      SEEK_SET,       "fLaC\0\0\0\"",8,"mpv"},
    { 1,     1,     0,      SEEK_SET,       "\x1A\x45\xDF\xA3",4,"mpv"},
    { 1,     1,     4,      SEEK_SET,       "ftypisom",8,"mpv"},
    { 1,     1,     4,      SEEK_SET,       "ftypM4A ",8,"mpv"},
    { 1,     1,     4,      SEEK_SET,       "ftypM4V ",8,"mpv"},
    { 1,     1,     4,      SEEK_SET,       "ftypMSNV",8,"mpv"},
    { 1,     1,     4,      SEEK_SET,       "ftypmp42",8,"mpv"},
    { 1,     1,     4,      SEEK_SET,       "ftypqt ",7,"mpv"},
    { 1,     1,     4,      SEEK_SET,       "ftyp3g",6,"vlc"},
    { 1,     1,     0,      SEEK_SET,       "\xFF\xF1",2,"mpv"},
    { 1,     1,     0,      SEEK_SET,       "\xFF\xF9",2,"mpv"},
    { 1,     1,     0,      SEEK_SET,       "\x1F\x9D",2,"file-roller"},
    { 1,     1,     0,      SEEK_SET,       "\x1F\xA0",2,"file-roller"},
    { 1,     1,     0,      SEEK_SET,       "\x42\x5A\x68",3,"file-roller"},
    { 1,     1,     0,      SEEK_SET,       "\x4C\x5A\x49\x50",4,"file-roller"},
    { 1,     1,     0,      SEEK_SET,       "\x4D\x5A",2,"file-roller"},
    { 1,     1,     0,      SEEK_SET,       "\x50\x4B\x03\x04",4,"file-roller"},
    { 1,     1,     0,      SEEK_SET,       "\x50\x4B\x05\x06",4,"file-roller"},
    { 1,     1,     0,      SEEK_SET,       "\x50\x4B\x07\x08",4,"file-roller"},
    { 1,     1,     0,      SEEK_SET,       "\x5A\x4D",2,"file-roller"},
    { 1,     1,     0,      SEEK_SET,       "\x52\x61\x72\x21\x1A\x07\x00",7,"file-roller"},
    { 1,     1,     0,      SEEK_SET,       "\x52\x61\x72\x21\x1A\x07\x01\x00",8,"file-roller"},
    { 1,     1,     0,      SEEK_SET,       "\x37\x7A\xBC\xAF\x27\x1C",6,"file-roller"},
    { 1,     1,     0,      SEEK_SET,       "\x1F\x8B",2,"file-roller"},
    { 1,     1,     0,      SEEK_SET,       "\xFD\x37\x7A\x58\x5A\x00",6,"file-roller"},
    { 1,     1,     0x101,  SEEK_SET,       "\x75\x73\x74\x61\x72\x00\x30\x30",8,"file-roller"},
    { 1,     1,     0x101,  SEEK_SET,       "\x75\x73\x74\x61\x72\x20\x20\x00",8,"file-roller"},
    { 1,     1,     0x8001, SEEK_SET,       "\x43\x44\x30\x30\x31",5,"file-roller"},
    { 1,     1,     0x8801, SEEK_SET,       "\x43\x44\x30\x30\x31",5,"file-roller"},
    { 1,     1,     0x9001, SEEK_SET,       "\x43\x44\x30\x30\x31",5,"file-roller"},
    { 0,     0,     0,      0,              NULL,0,NULL}
};

Key *keys = NULL;
size_t keys_t = 0;
size_t keys_a = 0;

#endif
