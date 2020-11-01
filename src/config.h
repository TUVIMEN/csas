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

#ifndef CONFIG_H
#define CONFIG_H

struct AliasesT aliases[] = {
    {"false",0},
    {"true",1},
    {"COL_00",COLOR_PAIR(0x0)},
    {"COL_01",COLOR_PAIR(0x1)},
    {"COL_02",COLOR_PAIR(0x2)},
    {"COL_03",COLOR_PAIR(0x3)},
    {"COL_04",COLOR_PAIR(0x4)},
    {"COL_05",COLOR_PAIR(0x5)},
    {"COL_06",COLOR_PAIR(0x6)},
    {"COL_07",COLOR_PAIR(0x7)},
    {"COL_08",COLOR_PAIR(0x8)},
    {"COL_09",COLOR_PAIR(0x9)},
    {"COL_0A",COLOR_PAIR(0xA)},
    {"COL_0B",COLOR_PAIR(0xB)},
    {"COL_0C",COLOR_PAIR(0xC)},
    {"COL_0D",COLOR_PAIR(0xD)},
    {"COL_0E",COLOR_PAIR(0xE)},
    {"COL_0F",COLOR_PAIR(0xF)},
    {"A_BOLD",A_BOLD},
    {"A_REVERSE",A_REVERSE},
    {"A_UNDERLINE",A_UNDERLINE},
    {"A_VERTICAL",A_VERTICAL},
    {"A_HORIZONTAL",A_HORIZONTAL},
    {"A_INVIS",A_INVIS},
    {"A_ITALIC",A_ITALIC},
    {"A_LEFT",A_LEFT},
    {"A_NORMAL",A_NORMAL},
    {"A_PROTECT",A_PROTECT},
    {"A_UNDERLINE",A_UNDERLINE},
    {"A_CHARTEXT",A_CHARTEXT},
    {"A_RIGHT",A_RIGHT},
    {"A_TOP",A_TOP},
    {"A_LOW",A_LOW},
    {"A_STANDOUT",A_STANDOUT},
    {"A_DIM",A_DIM},
    {"A_BLINK",A_BLINK},
    {"NAME_MAX",NAME_MAX},
    {"PATH_MAX",PATH_MAX},
    {"DIR_INC_RATE",DIR_INC_RATE},
    {"DIR_BASE_STABLE_RATE",DIR_BASE_STABLE_RATE},
    {"T_DIR",T_DIR},
    {"T_REG",T_REG},
    {"T_DEV",T_DEV},
    {"T_BDEV",T_BDEV},
    {"T_SOCK",T_SOCK},
    {"T_FIFO",T_FIFO},
    {"T_BLINK",T_BLINK},
    {"T_LDIR",T_LDIR},
    {"T_LREG",T_LREG},
    {"T_LDEV",T_LDEV},
    {"T_LBDEV",T_LBDEV},
    {"T_LSOCK",T_LSOCK},
    {"T_LFIFO",T_LFIFO},
    {"T_OTHER",T_OTHER},
    #ifdef __SORT_ELEMENTS_ENABLE__
    {"SORT_NONE",SORT_NONE},
    {"SORT_TYPE",SORT_TYPE},
    {"SORT_CHIR",SORT_CHIR},
    {"SORT_SIZE",SORT_SIZE},
    {"SORT_NAME",SORT_NAME},
    {"SORT_LNAME",SORT_LNAME},
    #ifdef __MTIME_ENABLE__
    {"SORT_MTIME",SORT_MTIME},
    #endif
    #ifdef __ATIME_ENABLE__
    {"SORT_ATIME",SORT_ATIME},
    #endif
    #ifdef __CTIME_ENABLE__
    {"SORT_CTIME",SORT_CTIME},
    #endif
    {"SORT_GID",SORT_GID},
    {"SORT_UID",SORT_UID},
    {"SORT_REVERSE",SORT_REVERSE},
    #endif
    {"D_F",D_F},
    {"D_R",D_R},
    {"D_C",D_C},
    {"D_H",D_H},
    {"M_REPLACE",M_REPLACE},
    {"M_MERGE",M_MERGE},
    {"M_DCPY",M_DCPY},
    {"M_CHNAME",M_CHNAME},
    {"GROUP_0",GROUP(0)},
    {"GROUP_1",GROUP(1)},
    {"GROUP_2",GROUP(2)},
    {"GROUP_3",GROUP(3)},
    {"GROUP_4",GROUP(4)},
    {"GROUP_5",GROUP(5)},
    {"GROUP_6",GROUP(6)},
    {"GROUP_7",GROUP(7)},
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
    {"DP_MTIME",DP_MTIME},
    {"DP_SMTIME",DP_SMTIME},
    #endif
    #ifdef __ATIME_ENABLE__
    {"DP_ATIME",DP_ATIME},
    {"DP_SATIME",DP_SATIME},
    #endif
    #ifdef __CTIME_ENABLE__
    {"DP_CTIME",DP_CTIME},
    {"DP_SCTIME",DP_SCTIME},
    #endif
    #ifdef __COLOR_FILES_BY_EXTENSION__
    {"DP_FTYPE",DP_FTYPE},
    #endif
    #ifdef __FILE_GROUPS_ENABLE__
    {"DP_GRNAME",DP_GRNAME},
    {"DP_GRGID",DP_GRGID},
    {"DP_GRPASSWD",DP_GRPASSWD},
    #endif
    #ifdef __FILE_OWNERS_ENABLE__
    {"DP_PWNAME",DP_PWNAME},
    {"DP_PWDIR",DP_PWDIR},
    {"DP_PWGECOS",DP_PWGECOS},
    {"DP_PWGID",DP_PWGID},
    {"DP_PWPASSWD",DP_PWPASSWD},
    {"DP_PWSHELL",DP_PWSHELL},
    {"DP_PWUID",DP_PWUID},
    #endif
    #ifdef __HUMAN_READABLE_SIZE_ENABLE__
    {"DP_HSIZE",DP_HSIZE},
    #endif
    {"B_UHNAME",B_UHNAME},
    {"B_DIR",B_DIR},
    {"B_NAME",B_NAME},
    {"B_WORKSPACES",B_WORKSPACES},
    {"B_POSITION",B_POSITION},
    #ifdef __FILESYSTEM_INFORMATION_ENABLE__
    {"B_FTYPE",B_FTYPE},
    {"B_SFTYPE",B_SFTYPE},
    {"B_FBSIZE",B_FBSIZE},
    {"B_FBLOCKS",B_FBLOCKS},
    {"B_FHBLOCKS",B_FHBLOCKS},
    {"B_FBFREE",B_FBFREE},
    {"B_FHBFREE",B_FHBFREE},
    {"B_FBAVAIL",B_FBAVAIL},
    {"B_FHBAVAIL",B_FHBAVAIL},
    {"B_FFILES",B_FFILES},
    {"B_FFFREE",B_FFFREE},
    {"B_FFSID",B_FFSID},
    {"B_FNAMELEN",B_FNAMELEN},
    {"B_FFRSIZE",B_FFRSIZE},
    {"B_FFLAGS",B_FFLAGS},
    #endif
    {"B_MODES",B_MODES},
    {"B_FGROUP",B_FGROUP},
    {"B_CSF",B_CSF},
    {NULL,0}
};

Settings* settings;

#ifdef __COLOR_FILES_BY_EXTENSION__
Extensions extensions[] = {
    {"AIFF",'V'}, {"MKV",'V'}, {"AVI",'V'}, {"FLAC",'V'}, {"FLV",'V'},
    {"M4A",'V'}, {"M4B",'V'}, {"M4P",'V'}, {"M4V",'V'}, {"MP2",'V'},
    {"MP3",'V'}, {"MP4",'V'}, {"MPE",'V'}, {"MPG",'V'}, {"MPV",'V'},
    {"OGG",'V'}, {"MPEG",'V'}, {"PCM",'V'}, {"QT",'V'}, {"SWF",'V'},
    {"WAV",'V'}, {"AAC",'V'}, {"WMA",'V'}, {"WMV",'V'}, {"VOB",'V'},
    {"MOV",'V'}, {"WEBM",'V'}, {"RVI",'V'}, {"3G2",'V'}, {"3GP",'V'},
    {"AA",'V'}, {"AC3",'V'}, {"MJ2",'V'}, {"MPC",'V'}, {"MPC8",'V'},
    {"M4V",'V'}, {"OGA",'V'}, {"OGV",'V'}, {"RVI",'V'}, {"ALAC",'V'},
    {"AVCHD",'V'}, //Video || Audio
    {"GIF",'I'}, {"JPEG",'I'}, {"JPG",'I'}, {"PNG",'I'}, {"RAW",'I'},
    {"TIFF",'I'}, {"BMP",'I'}, {"TGA",'I'}, {"GVS",'I'}, {"WEBP",'I'},
    {"SVG",'I'}, //Image
    {"TAR",'A'}, {"JAR",'A'}, {"RAR",'A'}, {"BZ2",'A'}, {"EXE",'A'},
    {"GZ",'A'}, {"IMG",'A'}, {"ISO",'A'}, {"LZMA",'A'}, {"TBZ2",'A'},
    {"TGZ",'A'}, {"Z",'A'}, {"ZX",'A'}, {"ZIP",'A'}, {"7Z",'A'},
    {"XZ",'A'}, //Archive
    {NULL,'\0'}
};
#endif

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