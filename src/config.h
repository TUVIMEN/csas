#ifndef CONFIG_H
#define CONFIG_H

char FileOpener[PATH_MAX] = "";
char Editor[PATH_MAX] = "vim";
li Exit = 0;
size_t BufferSize = (1<<16);
li DirLoadingMode = D_MODE_ONCE;
li DirSizeMethod = D_F;
const char *TTEMPLATE = "/tmp/CSAS-XXXXXX";
uchar SortMethod = SORT_CNAME|SORT_DIR_DISTINCTION|SORT_LDIR_DISTINCTION;
li Visual = 0;
li MoveOffset = 3;
li WrapScroll = 0;
li JumpScroll = 0;
li JumpScrollValue = 0;
li Color = 1;
li HostnameInTitlebar = 1;
li NumberLines = 0;
li NumberLinesOffset = 0;
li NumberLinesStartFrom = 0;
li IdleDelay = 2000;
char hostname[NAME_MAX],*username;
li Sel_C = A_REVERSE|A_BOLD;
li Reg_C = COLOR_PAIR(DEFAULT);
li Exec_C = COLOR_PAIR(GREEN);
li Dir_C = COLOR_PAIR(BLUE)|A_BOLD;
li Link_C = COLOR_PAIR(CYAN);
li Chr_C = COLOR_PAIR(MAGENTA);
li Blk_C = COLOR_PAIR(MAGENTA);
li Fifo_C = COLOR_PAIR(BLUE);
li Sock_C = COLOR_PAIR(MAGENTA);
li Missing_C = COLOR_PAIR(MAGENTA);
li Other_C = COLOR_PAIR(DEFAULT);
li Error_C = A_BOLD|COLOR_PAIR(RED);
li Bar_C = COLOR_PAIR(GREEN);
li Host_C = COLOR_PAIR(GREEN)|A_BOLD;
li Archive_C = COLOR_PAIR(RED);
li Image_C = COLOR_PAIR(YELLOW);
li Video_C = COLOR_PAIR(MAGENTA);
li SizeInBytes = 0;
li Linemode = L_SIZE;
li UpdateFile = 0;
li ColorByExtension = 0;
li FileSystemInfo = 0;
li MultipaneView = 0;
li FollowParentDir = 1;
li LeftWindowSize = 1;
li CenterWindowSize = 3;
li RightWindowSize = 4;

int sel_colors[] = {
    YELLOW,GREEN,RED,BLUE,
    CYAN,MAGENTA,WHITE,BLACK
};

fext extensions[] = {
    //Video || Audio
    {"aiff",'V'}, {"mkv",'V'}, {"avi",'V'}, {"flac",'V'}, {"flv",'V'},
    {"m4a",'V'}, {"m4b",'V'}, {"m4p",'V'}, {"mp2",'V'}, {"avchd",'V'},
    {"mp3",'V'}, {"mp4",'V'}, {"mpe",'V'}, {"mpg",'V'}, {"mpv",'V'},
    {"ogg",'V'}, {"mpeg",'V'}, {"pcm",'V'}, {"qt",'V'}, {"swf",'V'},
    {"wav",'V'}, {"aac",'V'}, {"wma",'V'}, {"wmv",'V'}, {"vob",'V'},
    {"mov",'V'}, {"webm",'V'}, {"rvi",'V'}, {"3g2",'V'}, {"3gp",'V'},
    {"aa",'V'}, {"ac3",'V'}, {"mj2",'V'}, {"mpc",'V'}, {"mpc8",'V'},
    {"m4v",'V'}, {"oga",'V'}, {"ogv",'V'}, {"rvi",'V'}, {"alac",'V'},
    {"rmvb",'V'},
    //Images
    {"gif",'I'}, {"jpeg",'I'}, {"jpg",'I'}, {"png",'I'}, {"raw",'I'},
    {"tiff",'I'}, {"bmp",'I'}, {"tga",'I'}, {"gvs",'I'}, {"webp",'I'},
    {"svg",'I'},
    //Archives
    {"tar",'A'}, {"jar",'A'}, {"rar",'A'}, {"bz2",'A'}, {"exe",'A'},
    {"gz",'A'}, {"img",'A'}, {"iso",'A'}, {"lzma",'A'}, {"tbz2",'A'},
    {"tgz",'A'}, {"z",'A'}, {"zx",'A'}, {"zip",'A'}, {"7z",'A'},
    {"xz",'A'},{"lz4",'A'},
    {NULL,'\0'}
};

fsig signatures[] = {
    { F_SILENT,                 0,      SEEK_SET,       "<svg",4,"eog"},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "%PDF",4,"zathura"},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A",8,"sxiv"},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\xFF\xD8",2,"sxiv"},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "BM",2,"sxiv"},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "GIF87a",6,"sxiv"},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "GIF89a",6,"sxiv"},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x49\x49\x2A\x00",4,"sxiv"},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x49\x20\x49",3,"sxiv"},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x49\x49\x1A\x00\x00\x00\x48\x45\x41\x50\x43\x43\x44\x52\x02\x00",16,"sxiv"},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x49\x49\x2A\x00\x10\x00\x00\x00\x43\x52",10,"sxiv"},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "RIFF",4,"sxiv"},
    { F_NORMAL|F_WAIT|F_BIN,    0,      SEEK_SET,       "\x52\x49\x46",3,"mpv"},
    { F_NORMAL|F_WAIT|F_BIN,    0,      SEEK_SET,       "\x57\x41\x56\x45",4,"mpv"},
    { F_NORMAL|F_WAIT|F_BIN,    0,      SEEK_SET,       "\x4F\x67\x67\x53\x00\x02\x00\x00",8,"mpv"},
    { F_NORMAL|F_WAIT|F_BIN,    0,      SEEK_SET,       "\x41\x56\x49\x20",4,"mpv"},
    { F_NORMAL|F_WAIT|F_BIN,    0,      SEEK_SET,       "\x30\x26\xB2\x75\x8E\x66\xCF\x11",8,"mpv"},
    { F_NORMAL|F_WAIT|F_BIN,    0,      SEEK_SET,       "\xA6\xD9\x00\xAA\x00\x62\xCE\x6C",8,"mpv"},
    { F_NORMAL|F_WAIT|F_BIN,    0,      SEEK_SET,       "\xFF\xFB",2,"mpv"},
    { F_NORMAL|F_WAIT|F_BIN,    0,      SEEK_SET,       "\xFF\xF2",2,"mpv"},
    { F_NORMAL|F_WAIT|F_BIN,    0,      SEEK_SET,       "\xFF\xF3",2,"mpv"},
    { F_NORMAL|F_WAIT|F_BIN,    0,      SEEK_SET,       "WAVEfmt ",8,"mpv"},
    { F_NORMAL|F_WAIT|F_BIN,    0,      SEEK_SET,       "fLaC\0\0\0\"",8,"mpv"},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x1A\x45\xDF\xA3",4,"mpv"},
    { F_SILENT|F_BIN,           4,      SEEK_SET,       "ftypisom",8,"mpv"},
    { F_SILENT|F_BIN,           4,      SEEK_SET,       "ftypM4A ",8,"mpv"},
    { F_SILENT|F_BIN,           4,      SEEK_SET,       "ftypM4V ",8,"mpv"},
    { F_SILENT|F_BIN,           4,      SEEK_SET,       "ftypMSNV",8,"mpv"},
    { F_SILENT|F_BIN,           4,      SEEK_SET,       "ftypmp42",8,"mpv"},
    { F_SILENT|F_BIN,           4,      SEEK_SET,       "ftypqt ",7,"mpv"},
    { F_SILENT|F_BIN,           4,      SEEK_SET,       "ftyp3g",6,"vlc"},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\xFF\xF1",2,"mpv"},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\xFF\xF9",2,"mpv"},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x1F\x9D",2,"file-roller"},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x1F\xA0",2,"file-roller"},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x42\x5A\x68",3,"file-roller"},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x4C\x5A\x49\x50",4,"file-roller"},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x4D\x5A",2,"file-roller"},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x50\x4B\x03\x04",4,"file-roller"},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x50\x4B\x05\x06",4,"file-roller"},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x50\x4B\x07\x08",4,"file-roller"},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x5A\x4D",2,"file-roller"},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x52\x61\x72\x21\x1A\x07\x00",7,"file-roller"},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x52\x61\x72\x21\x1A\x07\x01\x00",8,"file-roller"},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x37\x7A\xBC\xAF\x27\x1C",6,"file-roller"},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x1F\x8B",2,"file-roller"},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\xFD\x37\x7A\x58\x5A\x00",6,"file-roller"},
    { F_SILENT|F_BIN,           0x101,  SEEK_SET,       "\x75\x73\x74\x61\x72\x00\x30\x30",8,"file-roller"},
    { F_SILENT|F_BIN,           0x101,  SEEK_SET,       "\x75\x73\x74\x61\x72\x20\x20\x00",8,"file-roller"},
    { F_SILENT|F_BIN,           0x8001, SEEK_SET,       "\x43\x44\x30\x30\x31",5,"file-roller"},
    { F_SILENT|F_BIN,           0x8801, SEEK_SET,       "\x43\x44\x30\x30\x31",5,"file-roller"},
    { F_SILENT|F_BIN,           0x9001, SEEK_SET,       "\x43\x44\x30\x30\x31",5,"file-roller"},
    { 0,                        0,      0,              NULL,0,NULL}
};

#endif
