/*
    csas - console file manager
    Copyright (C) 2020-2025 Dominik Stanisław Suchora <hexderm@gmail.com>

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

char FileOpener[PATH_MAX] = "";
char Editor[PATH_MAX] = "vim";
char Shell[PATH_MAX] = "/bin/sh";
li Exit = 0;
size_t BufferSize = (1<<16);
li DirLoadingMode = D_MODE_ONCE|D_FOLLOW;
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
li Error_C = A_BOLD|COLOR_PAIR(RED)|A_REVERSE;
li Bar_C = A_BOLD|A_REVERSE|COLOR_PAIR(GREEN);
li Path_C = A_BOLD;
li Host_C = COLOR_PAIR(GREEN)|A_BOLD;
li Archive_C = COLOR_PAIR(RED);
li Image_C = COLOR_PAIR(YELLOW);
li Video_C = COLOR_PAIR(MAGENTA);
li Document_C = COLOR_PAIR(BLUE);
li Border_C = COLOR_PAIR(RED)|A_BOLD;
li Borders = 0;
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
li ShowKeyBindings = 0;
li PreviewSettings = P_DIR;
li OpenAllImages = 0;
li PreviewWidth = 0;
li PreviewHeight = 0;
li PreviewPosx = 0;
li PreviewPosy = 0;
li Verbose = 1;
li PID = 0;
li StartTime = 0;
char BinaryPreview[PATH_MAX] = "";
flexarr *trap_exit = NULL;
flexarr *trap_preview = NULL;
flexarr *trap_newdir = NULL;
flexarr *trap_chdir = NULL;
flexarr *trap_filerun = NULL;

int sel_colors[] = {
    YELLOW,GREEN,RED,BLUE,
    CYAN,MAGENTA,WHITE,BLACK
};

const fext extensions[] = {
    //Video || Audio
    {"aiff",'V'},{"mkv",'V'},{"avi",'V'},{"flac",'V'},{"flv",'V'},
    {"m4a",'V'},{"m4b",'V'},{"m4p",'V'},{"mp2",'V'},{"avchd",'V'},
    {"mp3",'V'},{"mp4",'V'},{"mpe",'V'},{"mpg",'V'},{"mpv",'V'},
    {"ogg",'V'},{"mpeg",'V'},{"pcm",'V'},{"qt",'V'},{"swf",'V'},
    {"wav",'V'},{"aac",'V'},{"wma",'V'},{"wmv",'V'},{"vob",'V'},
    {"mov",'V'},{"webm",'V'},{"rvi",'V'},{"3g2",'V'},{"3gp",'V'},
    {"aa",'V'},{"ac3",'V'},{"mj2",'V'},{"mpc",'V'},{"mpc8",'V'},
    {"m4v",'V'},{"oga",'V'},{"ogv",'V'},{"rvi",'V'},{"alac",'V'},
    {"rmvb",'V'},{"opus",'V'},{"divx",'V'},{"ts",'V'},{"tsa",'V'},
    {"tsv",'V'},{"m2p",'V'},{"mka",'V'},{"mks",'V'},{"mk3d",'V'},
    {"asf",'V'},{"aif",'V'},
    //Images
    {"gif",'I'},{"jpeg",'I'},{"jpg",'I'},{"png",'I'},{"raw",'I'},
    {"tiff",'I'},{"bmp",'I'},{"tga",'I'},{"gvs",'I'},{"webp",'I'},
    {"svg",'I'},{"ico",'I'},{"heic",'I'},{"jp2",'I'},{"j2k",'I'},
    {"jpf",'I'},{"jpf",'I'},{"jpm",'I'},{"jpg2",'I'},{"j2c",'I'},
    {"jpc",'I'},{"mj2",'I'},{"qoif",'I'},{"bpg",'I'},{"icns",'I'},
    {"tif",'I'},{"cr2",'I'},{"cr3",'I'},{"cin",'I'},{"dpx",'I'},
    {"exr",'I'},{"flif",'I'},
    //Archives
    {"tar",'A'},{"lz",'A'},{"jar",'A'},{"rar",'A'},{"bz2",'A'},{"exe",'A'},
    {"gz",'A'},{"img",'A'},{"iso",'A'},{"lzma",'A'},{"tbz2",'A'},
    {"tgz",'A'},{"z",'A'},{"zx",'A'},{"zip",'A'},{"7z",'A'},
    {"xz",'A'},{"lz4",'A'},{"zst",'A'},{"a",'A'},
    //Documents
    {"pdf",'D'},{"pdf",'D'},{"ps",'D'},{"djvu",'D'},{"djv",'D'},{"epub",'D'},
    {"docx",'D'},{"odp",'D'},{"pptx",'D'},{"xlsx",'D'},{"xpi",'D'},{"odf",'D'},
    {"ods",'D'},{"ps",'D'},{"eps",'D'},{"epsf",'D'},{"pk3",'D'},{"pk4",'D'},
    {"xpi",'D'},{"vsdx",'D'},{"doc",'D'},
    {NULL,'\0'}
};

const fsig signatures[] = {
    //Documents
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "%PDF-",5,OP_DOC},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "%!PS",4,OP_DOC},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x0D\x44\x4F\x43",4,OP_DOC},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "%!PS-Adobe-3.0 ESPF-3.0",23,OP_DOC},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "%!PS-Adobe-3.1 ESPF-3.0",23,OP_DOC},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "AT&TFORM",4,OP_DOC},
    //Images
    { F_SILENT,                 0,      SEEK_SET,       "<svg",4,OP_SVG},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A",8,OP_IMAGE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\xFF\xD8",2,OP_IMAGE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "BM",2,OP_IMAGE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "icns",4,OP_IMAGE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "GIF87a",6,OP_IMAGE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "GIF89a",6,OP_IMAGE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x49\x49\x2A\x00",4,OP_IMAGE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x4D\x4D\x00\x2A",4,OP_IMAGE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x49\x49\x2B\x00",4,OP_IMAGE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x4D\x4D\x00\x2B",4,OP_IMAGE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x49\x20\x49",3,OP_IMAGE},
    { F_SILENT|F_BIN,           8,      SEEK_SET,       "WEBP",4,OP_IMAGE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x49\x49\x1A\x00\x00\x00\x48\x45\x41\x50\x43\x43\x44\x52\x02\x00",16,OP_IMAGE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x49\x49\x2A\x00\x10\x00\x00\x00\x43\x52",10,OP_IMAGE},
    { F_SILENT|F_BIN,           4,      SEEK_SET,       "ftypheic",8,OP_IMAGE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "ftypcrx",7,OP_IMAGE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x80\x2A\x5F\xD7",4,OP_IMAGE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "SDPX",4,OP_IMAGE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "XPDS",4,OP_IMAGE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "FLIF",4,OP_IMAGE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x76\x2F\x31\x01",4,OP_IMAGE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x71\x6f\x69\x66",4,OP_IMAGE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x00\x00\x00\x0C\x6A\x50\x20\x20\x0D\x0A\x87\x0A",12,OP_IMAGE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\xFF\x4F\xFF\x51",4,OP_IMAGE},
    //Video
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "RIFF",4,OP_VIDEO},
    { F_NORMAL|F_WAIT|F_BIN,    0,      SEEK_SET,       "\x52\x49\x46",3,OP_VIDEO},
    { F_NORMAL|F_WAIT|F_BIN,    0,      SEEK_SET,       "\x49\x44\x33",3,OP_VIDEO},
    { F_NORMAL|F_WAIT|F_BIN,    0,      SEEK_SET,       "OggS",4,OP_VIDEO},
    { F_NORMAL|F_WAIT|F_BIN,    0,      SEEK_SET,       "\x57\x41\x56\x45",4,OP_VIDEO},
    { F_NORMAL|F_WAIT|F_BIN,    0,      SEEK_SET,       "\x4F\x67\x67\x53\x00\x02\x00\x00",8,OP_VIDEO},
    { F_NORMAL|F_WAIT|F_BIN,    0,      SEEK_SET,       "\x41\x56\x49\x20",4,OP_VIDEO},
    { F_NORMAL|F_WAIT|F_BIN,    0,      SEEK_SET,       "\x30\x26\xB2\x75\x8E\x66\xCF\x11",8,OP_VIDEO},
    { F_NORMAL|F_WAIT|F_BIN,    0,      SEEK_SET,       "\xA6\xD9\x00\xAA\x00\x62\xCE\x6C",8,OP_VIDEO},
    { F_NORMAL|F_WAIT|F_BIN,    0,      SEEK_SET,       "\xFF\xFB",2,OP_VIDEO},
    { F_NORMAL|F_WAIT|F_BIN,    0,      SEEK_SET,       "\xFF\xF2",2,OP_VIDEO},
    { F_NORMAL|F_WAIT|F_BIN,    0,      SEEK_SET,       "\xFF\xF3",2,OP_VIDEO},
    { F_NORMAL|F_WAIT|F_BIN,    0,      SEEK_SET,       "ID3",3,OP_VIDEO},
    { F_NORMAL|F_WAIT|F_BIN,    0,      SEEK_SET,       "WAVEfmt ",8,OP_VIDEO},
    { F_NORMAL|F_WAIT|F_BIN,    0,      SEEK_SET,       "fLaC",4,OP_VIDEO},
    { F_NORMAL|F_WAIT|F_BIN,    0,      SEEK_SET,       "\x30\x26\xB2\x75\x8E\x66\xCF\x11\xA6\xD9\x00\xAA\x00\x62\xCE\x6C",16,OP_VIDEO},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x1A\x45\xDF\xA3",4,OP_VIDEO},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x00\x00\x01\xBA",4,OP_VIDEO},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x00\x00\x01\xB3",4,OP_VIDEO},
    { F_SILENT|F_BIN,           4,      SEEK_SET,       "ftypisom",8,OP_VIDEO},
    { F_SILENT|F_BIN,           4,      SEEK_SET,       "ftypM4A ",8,OP_VIDEO},
    { F_SILENT|F_BIN,           4,      SEEK_SET,       "ftypM4V ",8,OP_VIDEO},
    { F_SILENT|F_BIN,           4,      SEEK_SET,       "ftypMSNV",8,OP_VIDEO},
    { F_SILENT|F_BIN,           4,      SEEK_SET,       "ftypmp42",8,OP_VIDEO},
    { F_SILENT|F_BIN,           4,      SEEK_SET,       "ftypqt ",7,OP_VIDEO},
    { F_SILENT|F_BIN,           4,      SEEK_SET,       "ftyp3g",6,OP_VIDEO},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\xFF\xF1",2,OP_VIDEO},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\xFF\xF9",2,OP_VIDEO},
    { F_SILENT|F_BIN,           188,    SEEK_SET,       "G",1,OP_VIDEO},
    //Archives
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x1F\x9D",2,OP_ARCHIVE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x1F\xA0",2,OP_ARCHIVE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x42\x5A\x68",3,OP_ARCHIVE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x4C\x5A\x49\x50",4,OP_ARCHIVE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x4D\x5A",2,OP_ARCHIVE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x50\x4B\x03\x04",4,OP_ARCHIVE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x50\x4B\x05\x06",4,OP_ARCHIVE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x50\x4B\x07\x08",4,OP_ARCHIVE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x5A\x4D",2,OP_ARCHIVE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x52\x61\x72\x21\x1A\x07\x00",7,OP_ARCHIVE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x52\x61\x72\x21\x1A\x07\x01\x00",8,OP_ARCHIVE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x37\x7A\xBC\xAF\x27\x1C",6,OP_ARCHIVE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\x1F\x8B",2,OP_ARCHIVE},
    { F_SILENT|F_BIN,           0,      SEEK_SET,       "\xFD\x37\x7A\x58\x5A\x00",6,OP_ARCHIVE},
    { F_SILENT|F_BIN,           0x101,  SEEK_SET,       "\x75\x73\x74\x61\x72\x00\x30\x30",8,OP_ARCHIVE},
    { F_SILENT|F_BIN,           0x101,  SEEK_SET,       "\x75\x73\x74\x61\x72\x20\x20\x00",8,OP_ARCHIVE},
    { F_SILENT|F_BIN,           0x8001, SEEK_SET,       "\x43\x44\x30\x30\x31",5,OP_ARCHIVE},
    { F_SILENT|F_BIN,           0x8801, SEEK_SET,       "\x43\x44\x30\x30\x31",5,OP_ARCHIVE},
    { F_SILENT|F_BIN,           0x9001, SEEK_SET,       "\x43\x44\x30\x30\x31",5,OP_ARCHIVE},
    { 0,                        0,      0,              NULL,0,NULL}
};

#endif
