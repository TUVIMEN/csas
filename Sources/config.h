#ifndef CONFIG_H
#define CONFIG_H

char Values[] = {'B','K','M','G','T','P','E','Z','Y'};
char* editor = "/usr/bin/nvim";

long long int BarsSettings = B_UHNAME | B_DIR | B_NAME | B_WORKSPACES | DP_LSPERMS | DP_SMTIME | DP_PWNAME | DP_GRNAME
                    | B_POSITION | B_FHBFREE;

char* UserHostPattern = "%s@%s";
//0 - User Host 1 - Host User
bool UserRHost = 0;

uint32_t INOTIFY_MASK = IN_DELETE | IN_DELETE_SELF | IN_CREATE | IN_MOVE;

int MoveOffSet = 8;
bool WrapScroll = false;
bool JumpScroll = false;
    int JumpScrollValue = 16;

bool StatusBarOnTop = false;
bool Win1Enable = true;
bool Win1Display;
bool Win3Enable = true;
bool Win3Display;
bool Bar1Enable = true;
bool Bar2Enable = true;
float WinSizeMod[2] = {0.132,0.368};

bool Borders = false;
bool FillBlankSpace = true;
int WindowBorder[8] = {0,0,0,0,0,0,0,0};

bool EnableColor = true;
int DelayBetweenFrames = 1;

bool NumberLines = false;
    bool NumberLinesOff = false;
    bool NumberLinesFromOne = false;

int DisplayingC = DP_HSIZE;

#ifdef __SHOW_HIDDEN_FILES_ENABLE__
bool ShowHiddenFiles = true;
#endif

#ifdef __SORT_ELEMENTS_ENABLE__
    /*               Method     Reverse*/
unsigned char SortMethod = SORT_NAME;
int BetterFiles[] = {T_DIR,T_LDIR,0}; //Must be zero at the end
#endif

#ifdef __BLOCK_SIZE_ELEMENTS_ENABLE__
size_t BlockSize = 1024;
#endif

/*                       Like File   Recursive   Count*/
char DirSizeMethod = D_C;

int C_Error = COLOR_PAIR(4) | A_BOLD | A_REVERSE;
#ifdef __COLOR_FILES_BY_EXTENSION__
int C_FType_A = COLOR_PAIR(4);
int C_FType_I = COLOR_PAIR(2);
int C_FType_V = COLOR_PAIR(6);
#endif
int C_Selected = A_REVERSE | A_BOLD;
int C_Exec_set = A_BOLD;
int C_Exec = COLOR_PAIR(10);
int C_BLink = COLOR_PAIR(1);
int C_Dir = COLOR_PAIR(1) | A_BOLD;
int C_Reg = A_NORMAL;
int C_Fifo = COLOR_PAIR(9) | A_ITALIC;
int C_Sock = COLOR_PAIR(9) | A_ITALIC;
int C_Dev = COLOR_PAIR(9);
int C_BDev = COLOR_PAIR(9);
int C_LDir = COLOR_PAIR(5) | A_BOLD;
int C_LReg = COLOR_PAIR(5);
int C_LFifo = COLOR_PAIR(5);
int C_LSock = COLOR_PAIR(5);
int C_LDev = COLOR_PAIR(5);
int C_LBDev = COLOR_PAIR(5);
int C_Other = COLOR_PAIR(0);
int C_User_S_D = COLOR_PAIR(6) | A_BOLD;
int C_Bar_Dir = COLOR_PAIR(1) | A_BOLD;
int C_Bar_Name = A_NORMAL | A_BOLD;
int C_Bar_WorkSpace = A_NORMAL | A_BOLD;
int C_Bar_WorkSpace_Selected = COLOR_PAIR(6) | A_REVERSE | A_BOLD;

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

Key keys[] = {
    {"q",0,NULL,NULL},
    {"j",1,NULL,NULL},
    {"k",2,NULL,NULL},
    {"h",3,NULL,NULL},
    {"l",4,NULL,NULL},
    {"gff",27,0,"/run/media/kpp/fil"}, //0 - is env ; 1 - path/env
    {"gfm",27,0,"/run/media/kpp/f/Muzyka"},
    {"gfk",27,0,"/run/media/kpp/f/ksiegi"},
    {"gh",27,1,"HOME"},
    {"g/",27,0,"/"},
    {"gd",27,0,"/dev"},
    {"ge",27,0,"/etc"},
    {"gm",27,1,"MEDIA"}, //variable
    {"gM",27,0,"/mnt"},
    {"go",27,0,"/opt"},
    {"gs",27,0,"/srv"},
    {"gp",27,0,"/tmp"},
    {"gu",27,0,"/usr"},
    {"gv",27,0,"/var"},
    {"gg",5,NULL,NULL},
    {"G",6,NULL,NULL},
    {"z1",8,0,NULL},
    {"z2",8,1,NULL},
    {"z3",8,2,NULL},
    {"z4",8,3,NULL},
    {"z5",8,4,NULL},
    {"z6",8,5,NULL},
    {"z7",8,6,NULL},
    {"z8",8,7,NULL},
    {"z9",8,8,NULL},
    {"z0",8,9,NULL},
    {"oe",9,SORT_NONE,NULL},
    {"oE",9,SORT_NONE|SORT_REVERSE,NULL},
    {"or",9,SORT_TYPE,NULL},
    {"oR",9,SORT_TYPE|SORT_REVERSE,NULL},
    {"ob",9,SORT_CHIR,NULL},
    {"oB",9,SORT_CHIR|SORT_REVERSE,NULL},
    {"os",9,SORT_SIZE,NULL},
    {"oS",9,SORT_SIZE|SORT_REVERSE,NULL},
    {"otm",9,SORT_MTIME,NULL},
    {"otM",9,SORT_MTIME|SORT_REVERSE,NULL},
    {"otc",9,SORT_CTIME,NULL},
    {"otC",9,SORT_CTIME|SORT_REVERSE,NULL},
    {"ota",9,SORT_ATIME,NULL},
    {"otA",9,SORT_ATIME|SORT_REVERSE,NULL},
    {"og",9,SORT_GID,NULL},
    {"oG",9,SORT_GID|SORT_REVERSE,NULL},
    {"ou",9,SORT_UID,NULL},
    {"oU",9,SORT_UID|SORT_REVERSE,NULL},
    {"om",9,SORT_LNAME,NULL},
    {"oM",9,SORT_LNAME|SORT_REVERSE,NULL},
    {"on",9,SORT_NAME,NULL},
    {"oN",9,SORT_NAME|SORT_REVERSE,NULL},
    {"dch",10,D_C,NULL},
    {"dcH",10,D_C|D_H,NULL},
    {"dCh",10,D_C|D_R,NULL},
    {"dCH",10,D_C|D_R|D_H,NULL},
    {"dsh",10,0,NULL},
    {"dsH",10,D_H,NULL},
    {"dSh",10,D_R,NULL},
    {"dSH",10,D_R|D_H,NULL},
    {"dfh",10,D_F,NULL},
    {"dfH",10,D_F|D_H,NULL},
    {NULL,0,NULL,NULL}
};

#endif
