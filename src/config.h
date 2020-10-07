#ifndef CONFIG_H
#define CONFIG_H

#ifdef __LOAD_CONFIG_ENABLE__
struct AliasesT aliases[] = {
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
    {"GROUP_0",GROUP_0},
    {"GROUP_1",GROUP_1},
    {"GROUP_2",GROUP_2},
    {"GROUP_3",GROUP_3},
    {"GROUP_4",GROUP_4},
    {"GROUP_5",GROUP_5},
    {"GROUP_6",GROUP_6},
    {"GROUP_7",GROUP_7},
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
    #ifdef __INOTIFY_ENABLE__
    {"IN_ALL_EVENTS",IN_ALL_EVENTS},
    {"IN_ATTRIB",IN_ATTRIB},
    {"IN_CLOEXEC",IN_CLOEXEC},
    {"IN_CLOSE",IN_CLOSE},
    {"IN_CLOSE_NOWRITE",IN_CLOSE_NOWRITE},
    {"IN_CLOSE_WRITE",IN_CLOSE_WRITE},
    {"IN_Q_OVERFLOW",IN_Q_OVERFLOW},
    {"IN_UNMOUNT",IN_UNMOUNT},
    {"IN_OPEN",IN_OPEN},
    {"IN_ONLYDIR",IN_ONLYDIR},
    {"IN_ONESHOT",IN_ONESHOT},
    {"IN_NONBLOCK",IN_NONBLOCK},
    {"IN_MOVED_TO",IN_MOVED_TO},
    {"IN_MOVED_FROM",IN_MOVED_FROM},
    {"IN_MOVE_SELF",IN_MOVE_SELF},
    {"IN_MOVE",IN_MOVE},
    {"IN_MODIFY",IN_MODIFY},
    {"IN_MASK_CREATE",IN_MASK_CREATE},
    {"IN_MASK_ADD",IN_MASK_ADD},
    {"IN_ISDIR",IN_ISDIR},
    {"IN_IGNORED",IN_IGNORED},
    {"IN_DELETE_SELF",IN_DELETE_SELF},
    {"IN_DELETE",IN_DELETE},
    {"IN_CREATE",IN_CREATE},
    #endif
    {NULL,0}
};
#endif

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

    //First char in string can't be number!
Key *keys = NULL;
size_t keys_t = 0;
size_t keys_a = 0;

/* = {
    {"q",0,.slc1.v=NULL,.slc2.v=NULL},
    {"j",1,.slc1.ll=1,.slc2.v=NULL},
    {"J",1,.slc1.ll=16,.slc2.v=NULL},
    {"k",2,.slc1.ll=1,.slc2.v=NULL},
    {"K",2,.slc1.ll=16,.slc2.v=NULL},
    {"h",3,.slc1.v=NULL,.slc2.v=NULL},
    {"l",4,.slc1.v=NULL,.slc2.v=NULL},
    {"gff",27,.slc1.ll=0,.slc2.v="/run/media/kpp/fil"}, //0 - is env ; 1 - path/env
    {"gfm",27,.slc1.ll=0,.slc2.v="/run/media/kpp/f/Muzyka"},
    {"gfk",27,.slc1.ll=0,.slc2.v="/run/media/kpp/f/ksiegi"},
    {"gh",27,.slc1.ll=1,.slc2.v="HOME"},
    {"g/",27,.slc1.ll=0,.slc2.v="/"},
    {"gd",27,.slc1.ll=0,.slc2.v="/dev"},
    {"ge",27,.slc1.ll=0,.slc2.v="/etc"},
    {"gm",27,.slc1.ll=1,.slc2.v="MEDIA"}, //variable
    {"gM",27,.slc1.ll=0,.slc2.v="/mnt"},
    {"go",27,.slc1.ll=0,.slc2.v="/opt"},
    {"gs",27,.slc1.ll=0,.slc2.v="/srv"},
    {"gp",27,.slc1.ll=0,.slc2.v="/tmp"},
    {"gu",27,.slc1.ll=0,.slc2.v="/usr"},
    {"gv",27,.slc1.ll=0,.slc2.v="/var"},
    {"gg",5,.slc1.v=NULL,.slc2.v=NULL},
    {"G",6,.slc1.v=NULL,.slc2.v=NULL},
    {"z1",8,.slc1.ll=0,.slc2.v=NULL},
    {"z2",8,.slc1.ll=1,.slc2.v=NULL},
    {"z3",8,.slc1.ll=2,.slc2.v=NULL},
    {"z4",8,.slc1.ll=3,.slc2.v=NULL},
    {"z5",8,.slc1.ll=4,.slc2.v=NULL},
    {"z6",8,.slc1.ll=5,.slc2.v=NULL},
    {"z7",8,.slc1.ll=6,.slc2.v=NULL},
    {"z8",8,.slc1.ll=7,.slc2.v=NULL},
    {"z9",8,.slc1.ll=8,.slc2.v=NULL},
    {"z0",8,.slc1.ll=9,.slc2.v=NULL},
    {"oe",9,.slc1.ll=SORT_NONE,.slc2.v=NULL},
    {"oE",9,.slc1.ll=SORT_NONE|SORT_REVERSE,.slc2.v=NULL},
    {"or",9,.slc1.ll=SORT_TYPE,.slc2.v=NULL},
    {"oR",9,.slc1.ll=SORT_TYPE|SORT_REVERSE,.slc2.v=NULL},
    {"ob",9,.slc1.ll=SORT_CHIR,.slc2.v=NULL},
    {"oB",9,.slc1.ll=SORT_CHIR|SORT_REVERSE,.slc2.v=NULL},
    {"os",9,.slc1.ll=SORT_SIZE,.slc2.v=NULL},
    {"oS",9,.slc1.ll=SORT_SIZE|SORT_REVERSE,.slc2.v=NULL},
    {"otm",9,.slc1.ll=SORT_MTIME,.slc2.v=NULL},
    {"otM",9,.slc1.ll=SORT_MTIME|SORT_REVERSE,.slc2.v=NULL},
    {"otc",9,.slc1.ll=SORT_CTIME,.slc2.v=NULL},
    {"otC",9,.slc1.ll=SORT_CTIME|SORT_REVERSE,.slc2.v=NULL},
    {"ota",9,.slc1.ll=SORT_ATIME,.slc2.v=NULL},
    {"otA",9,.slc1.ll=SORT_ATIME|SORT_REVERSE,.slc2.v=NULL},
    {"og",9,.slc1.ll=SORT_GID,.slc2.v=NULL},
    {"oG",9,.slc1.ll=SORT_GID|SORT_REVERSE,.slc2.v=NULL},
    {"ou",9,.slc1.ll=SORT_UID,.slc2.v=NULL},
    {"oU",9,.slc1.ll=SORT_UID|SORT_REVERSE,.slc2.v=NULL},
    {"om",9,.slc1.ll=SORT_LNAME,.slc2.v=NULL},
    {"oM",9,.slc1.ll=SORT_LNAME|SORT_REVERSE,.slc2.v=NULL},
    {"on",9,.slc1.ll=SORT_NAME,.slc2.v=NULL},
    {"oN",9,.slc1.ll=SORT_NAME|SORT_REVERSE,.slc2.v=NULL},
    {"dch",10,.slc1.ll=D_C,.slc2.v=NULL},
    {"dcH",10,.slc1.ll=D_C|D_H,.slc2.v=NULL},
    {"dCh",10,.slc1.ll=D_C|D_R,.slc2.v=NULL},
    {"dCH",10,.slc1.ll=D_C|D_R|D_H,.slc2.v=NULL},
    {"dsh",10,.slc1.ll=0,.slc2.v=NULL},
    {"dsH",10,.slc1.ll=D_H,.slc2.v=NULL},
    {"dSh",10,.slc1.ll=D_R,.slc2.v=NULL},
    {"dSH",10,.slc1.ll=D_R|D_H,.slc2.v=NULL},
    {"dfh",10,.slc1.ll=D_F,.slc2.v=NULL},
    {"dfH",10,.slc1.ll=D_F|D_H,.slc2.v=NULL},
    {"x1",11,.slc1.ll=GROUP_0,.slc2.v=NULL},
    {"x2",11,.slc1.ll=GROUP_1,.slc2.v=NULL},
    {"x3",11,.slc1.ll=GROUP_2,.slc2.v=NULL},
    {"x4",11,.slc1.ll=GROUP_3,.slc2.v=NULL},
    {"x5",11,.slc1.ll=GROUP_4,.slc2.v=NULL},
    {"x6",11,.slc1.ll=GROUP_5,.slc2.v=NULL},
    {"x7",11,.slc1.ll=GROUP_6,.slc2.v=NULL},
    {"x8",11,.slc1.ll=GROUP_7,.slc2.v=NULL},
    {" ",12,.slc1.v=NULL,.slc2.v=NULL},
    {"V",13,.slc1.v=NULL,.slc2.v=NULL},
    {"vta",14,.slc1.ll=-1,.slc2.ll=0},
    {"vth",14,.slc1.ll=-1,.slc2.ll=1},
    {"vda",14,.slc1.ll=0,.slc2.ll=0},
    {"vdh",14,.slc1.ll=0,.slc2.ll=1},
    {"vea",14,.slc1.ll=1,.slc2.ll=0},
    {"veh",14,.slc1.ll=1,.slc2.ll=1},
    {"pm",15,.slc1.v=NULL,.slc2.v=NULL},
    {"pp",16,.slc1.v=NULL,.slc2.v=NULL},
    {"dd",17,.slc1.ll=1,.slc2.v=NULL},
    {"dD",17,.slc1.ll=0,.slc2.v=NULL},
    {NULL,0,.slc1.v=NULL,.slc2.v=NULL}
};*/

#endif