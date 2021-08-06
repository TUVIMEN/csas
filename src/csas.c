#include "main.h"
#include "csas.h"
#include "load.h"
#include "functions.h"
#include "useful.h"
#include "draw.h"
#include "config.h"

static void
initcurses()
{
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    timeout(150);
    noecho();
    nonl();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    curs_set(FALSE);
    if (has_colors()) {
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

static int
xbind_add(const char *keys, const char *value, flexarr *b)
{
    size_t i,size;
    ret_errno(keys==NULL||value==NULL||b==NULL,EINVAL,-1);
    ret_errno((size=strlen(keys))>BINDING_KEY_MAX||strlen(value)>PATH_MAX,EOVERFLOW,-1);

    wchar_t k[BINDING_KEY_MAX];
    for (i = 0; i < size; i++)
        k[i] = btowc(keys[i]);
    k[i] = 0;
    
    xbind *bind = (xbind*)b->v;

    uchar found = 0;
    for (i = 0; i < b->size; i++) {
        if (wcscmp(k,bind[i].keys) == 0) {
            found = 1;
            break;
        }
    }
    
    if (found == 0) {
        bind = (xbind*)flexarr_inc(b);
        bind->keys = malloc(BINDING_KEY_MAX*sizeof(wchar_t));
        bind->value = malloc(PATH_MAX);
        wcscpy(bind->keys,k);
    } else {
        bind = &bind[i];
    }

    strcpy(bind->value,value);

    return 0;
}

static void
add_bindings(flexarr *b)
{
    xbind_add("k","move -d",b);
    xbind_add("j","move -u",b);
    xbind_add("gg","move -s 0",b);
    xbind_add("G","move -s $",b);
    xbind_add("h","cd ..",b);
    xbind_add("l","file_run %s",b);
    xbind_add("g/","cd /",b);
    xbind_add("gu","cd /usr",b);
    xbind_add("gv","cd /var",b);
    xbind_add("gp","cd /tmp",b);
    xbind_add("gs","cd /srv",b);
    xbind_add("go","cd /opt",b);
    xbind_add("gM","cd /mnt",b);
    xbind_add("ge","cd /etc",b);
    xbind_add("gd","cd /dev",b);
    xbind_add("gh","cd ${HOME}",b);
    xbind_add(" ","fastselect",b);
    xbind_add("s","source gg",b);
}

static int
xfunc_add(const char *name, const uchar type, void *func, flexarr *f)
{
    ret_errno(name==NULL||f==NULL,EINVAL,-1);
    ret_errno(strlen(name)>FUNCTIONS_NAME_MAX,EOVERFLOW,-1);

    xfunc *funcs = (xfunc*)f->v;

    size_t i;
    uchar found = 0;
    for (i = 0; i < f->size; i++) {
        if (strcmp(name,funcs[i].name) == 0) {
            found = 1;
            break;
        }
    }

    if (found == 0) {
        funcs = (xfunc*)flexarr_inc(f);
        funcs->name = malloc(FUNCTIONS_NAME_MAX*sizeof(wchar_t));
        strcpy(funcs->name,name);
    } else {
        funcs = &funcs[i];
    }

    funcs->type = type;
    funcs->func = func;

    return 0;
}


static void
add_functions(flexarr *f)
{
    xfunc_add("move",0,cmd_move,f);
    xfunc_add("cd",0,cmd_cd,f);
    xfunc_add("file_run",0,cmd_file_run,f);
    xfunc_add("source",0,cmd_source,f);
    xfunc_add("fastselect",0,cmd_fastselect,f);
}

csas *
csas_init()
{
    csas *ret = malloc(sizeof(csas));
    memset(ret->tabs,0,sizeof(tab)*TABS);
    ret->ctab = 0;
    ret->message = 0;
    ret->dirs = flexarr_init(sizeof(xdir),DIR_INCR);
    ret->functions = flexarr_init(sizeof(xfunc),FUNCTIONS_INCR);
    ret->bindings = flexarr_init(sizeof(xbind),BINDINGS_INCR);
    initcurses();
    ret->win = subwin(stdscr,LINES-2,COLS,1,0);

    add_functions(ret->functions);
    add_bindings(ret->bindings);

    return ret;
}

static int
color_by_mode(const mode_t m, const uchar f)
{
    switch (m&S_IFMT) {
        case S_IFREG:
            if (m&(S_IXUSR|S_IXGRP|S_IXOTH))
                return EXEC_C;
            else
                return REG_C;
        case S_IFDIR: return DIR_C;
        case S_IFLNK:
            if (f&SLINK_MISSING)
                return MISSING_C;
            return LINK_C;
        case S_IFCHR: return CHR_C;
        case S_IFBLK: return BLK_C;
        case S_IFIFO: return FIFO_C;
        case S_IFSOCK: return SOCK_C;
        default: return OTHER_C;
    }
}

static void
csas_draw(csas *cs)
{
    xdir *dir = &CTAB;
    xfile *file = dir->files;
    size_t j;
    int i,color;
    
    if (dir->sel < dir->scroll)
        dir->scroll = dir->sel;
    else if (dir->sel > dir->scroll+HEIGHT-1)
        dir->scroll = dir->sel-HEIGHT+5;

    mvhline(0,0,' ',COLS); //bar1
    attr_set(A_BOLD,DIR_C,NULL);

    i = 0;
    if (dir->size > 0) {
        if (dir->path[0] == '/' && dir->path[1])
            i++;
        i = file[dir->sel].nlen;
    }
    mvaddnstr(0,0,path_shrink(dir->path,dir->plen,COLS-i),dir->plen);
    if (dir->size > 0) {
        if (dir->path[0] == '/' && dir->path[1])
            addch('/');
        attr_set(A_BOLD,REG_C,NULL);
        addnstr(file[dir->sel].name,file[dir->sel].nlen);
    }
    attr_set(0,0,NULL);

    for (i = 0, j = dir->scroll; i < HEIGHT && j < dir->size; i++, j++) { //files
        if (file[j].sel[cs->ctab]&(1<<cs->tabs[cs->ctab].sel))
            wattr_set(cs->win,A_REVERSE,sel_colors[cs->tabs[cs->ctab].sel],NULL);
        mvwaddch(cs->win,i,0,' ');
        wattr_set(cs->win,0,0, NULL);

        color =  color_by_mode(file[j].mode,file[j].flags);
        if (j == dir->sel)
            wattr_on(cs->win,SEL_C,NULL);
        wcolor_set(cs->win,color,NULL);
        mvwhline(cs->win,i,1,' ',COLS);

        char *size = size_shrink(file[j].size);
        size_t sizel = strlen(size);

        if (file[j].nlen >= COLS-sizel-5) {
            mvwaddnstr(cs->win,i,2,file[j].name,COLS-sizel-5);
            waddch(cs->win,'~');
        } else
            mvwaddnstr(cs->win,i,2,file[j].name,file[j].nlen);
        mvwaddnstr(cs->win,i,COLS-sizel-1,size,sizel);
        wattroff(cs->win,-1);
    }
    for (; i < HEIGHT; i++)
        mvwhline(cs->win,i,0,' ',COLS);

    if (!cs->message) {
        mvhline(LINES-1,0,' ',COLS); //bar2
        if (dir->size > 0) {
            char t[16];
            i = snprintf(t,16,"%lu/%lu",dir->sel+1,dir->size);
            if (i)
                mvaddnstr(LINES-1,COLS-i,t,i);
        } else {
            mvaddch(LINES-1,COLS-1,'0');
        }
    }
    cs->message = 0;

    wrefresh(cs->win);
}

void
csas_resize(csas *cs)
{
    delwin(cs->win);
    endwin();
    refresh();
    clear();
    cs->win = subwin(stdscr,LINES-2,COLS,1,0);
    csas_draw(cs);
}

void
csas_run(csas *cs, int argc, char **argv)
{
    char *path = ".";
    if (argc > 1)
        path = argv[1];
    if (get_dir(path,cs->dirs,&cs->tabs[cs->ctab].t,D_CHDIR) != 0)
        exiterr();

    int b;

    while (true) {
        csas_draw(cs);

        if ((b = update_event(cs)) != -1) {
            if (command_run(BINDINGS[b].value,cs) == -1) {
                printmsg(A_BOLD|COLOR_PAIR(RED),"%s: %s",BINDINGS[b].value,strerror(errno));
                refresh();
                cs->message = 1;
            }
        }
    }
}

void
xdir_free(xdir *dir)
{
    free(dir->path);
    dir->path = NULL;
    dir->plen = 0;
    for (size_t i = 0; i < dir->size; i++)
        free(dir->files[i].name);
    free(dir->files);
    dir->files = NULL;
    dir->size = 0;
}
