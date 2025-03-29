/*
    csas - console file manager
    Copyright (C) 2020-2025 Dominik Stanisław Suchora <suchora.dominik7@gmail.com>

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

#include "main.h"
#include "csas.h"
#include "useful.h"
#include "draw.h"
#include "functions.h"
#include "load.h"

extern li IdleDelay;
extern li Color;
extern li FileSystemInfo;
extern li Error_C;
extern li Exit;
extern li UpdateFile;
extern li DirLoadingMode;

static void
initcurses()
{
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    timeout(IdleDelay);
    noecho();
    nonl();
    //intrflush(stdscr,FALSE);
    keypad(stdscr,TRUE);
    curs_set(FALSE);
    if (Color && has_colors()) {
        short bg;
        start_color();
        use_default_colors();
        bg = -1;
        init_pair(RED,COLOR_RED,bg);
        init_pair(GREEN,COLOR_GREEN,bg);
        init_pair(YELLOW,COLOR_YELLOW,bg);
        init_pair(BLUE,COLOR_BLUE,bg);
        init_pair(CYAN,COLOR_CYAN,bg);
        init_pair(MAGENTA,COLOR_MAGENTA,bg);
        init_pair(WHITE,COLOR_WHITE,bg);
        init_pair(BLACK,COLOR_BLACK,bg);
    }
    atexit((void (*)(void)) endwin);
}

static void
usage(const char *argv0)
{
    fprintf(stderr,"Usage: %s [OPTION]... [PATH]\n\n"\
      "Options:\n"\
      "  -f FILE\tuse FILE as configuration file\n" \
      "  -c\t\tdo not load configuration file\n" \
      "  -h\t\tshow help\n" \
      "  -v\t\tshow version\n",argv0);
    exit(1);
}

static void
load_conf_path(csas *cs)
{
    char *conf=getenv("CSAS_CONFIG"),*t;
    if (conf && config_load(conf,cs) != -1)
        return;

    t = getenv("XDG_CONFIG_HOME");
    if (t && config_load(mkpath(t,"csasrc"),cs) != -1)
        return;

    t = getenv("HOME");
    if (t && config_load(mkpath(t,".csasrc"),cs) != -1)
        return;

    config_load("/etc/csasrc",cs);
}

int
main(int argc, char **argv)
{
    char *path = ".",*conf=NULL;
    uchar noconf = 0;
    int opt;

    while ((opt = getopt(argc,argv,"vhcf:")) != -1) {
        switch (opt) {
            case 'v':
                fprintf(stderr,"%s\n",VERSION);
                exit(1);
                break;
            case 'h': usage(argv[0]); break;
            case 'c': noconf = 1; conf = NULL; break;
            case 'f':
                conf = optarg;
                break;
            default:
                exit(1);
        }
    }
    if (optind < argc)
        path = argv[argc-1];

    opterr = 0;

    csas *cs = csas_init();
    if (!noconf) {
        if (conf == NULL) {
            load_conf_path(cs);
        } else
            config_load(conf,cs);
    }
    initcurses();

    wins_resize(cs->wins);
    if (csas_cd(path,cs) == -1)
        return -1;
    cs->tabs[cs->ctab].flags |= T_EXISTS;

    struct timespec timer;
    int e;
    time_t t1,t2=0;
    struct stat statbuf;

    if (FileSystemInfo)
        statfs(".",&cs->fs);

    while (!Exit) {
        clock_gettime(1,&timer);
        t1 = timer.tv_sec;

        csas_draw(cs);

        REPEAT: ;
        if ((e = update_event(cs)) != -1) {
            if (alias_run(BINDINGS[e].value,strlen(BINDINGS[e].value),cs) == -1) {
                printmsg(Error_C,"%s: %s",BINDINGS[e].value,strerror(errno));
                refresh();
                goto REPEAT;
            }
            if (FileSystemInfo)
                statfs(".",&cs->fs);
        }

        if (UpdateFile) {
            register xdir *d = &CTAB(1);
            if (d->files->size)
                xfile_update(&((xfile*)d->files->v)[d->sel[cs->ctab]]);
        }

        if (t1 != t2) {
            register xdir *d = &CTAB(1);
            t2 = t1;
            if (lstat(d->path,&statbuf) != 0)
                continue;
            if (memcmp(&statbuf.st_ctim,&d->ctime,sizeof(struct timespec)) != 0) {
                if (DirLoadingMode&D_MODE_ONCE) {
                    d->flags |= S_CHANGED;
                } else {
                    getdir(d->path,cs->dirs,DirLoadingMode);
                }
            }
        }
    }
    endwin();
    csas_free(cs);

    return 0;
}
