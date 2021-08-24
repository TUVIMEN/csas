#include "main.h"
#include "csas.h"
#include "load.h"
#include "functions.h"
#include "useful.h"
#include "calc.h"
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

int
xbind_add(const char *keys, const char *value, flexarr *b)
{
    size_t i,size;
    ret_errno(keys==NULL||value==NULL||b==NULL,EINVAL,-1);
    ret_errno((size=strlen(keys))>BINDING_KEY_MAX||strlen(value)>PATH_MAX,EOVERFLOW,-1);

    wchar_t k[BINDING_KEY_MAX];
    change_keys(k,keys);
    
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
    xbind_add("q","quit",b);
    xbind_add("Q","quit -f",b);
    xbind_add("k","move -d",b);
    xbind_add("j","move -u",b);
    xbind_add("gg","move -s",b);
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
    xbind_add("gh","cd ~",b);
    xbind_add(" ","fastselect",b);
    xbind_add(":","console",b);
    xbind_add("z1","tab 0",b);
    xbind_add("z2","tab 1",b);
    xbind_add("z3","tab 2",b);
    xbind_add("z4","tab 3",b);
    xbind_add("z5","tab 4",b);
    xbind_add("z6","tab 5",b);
    xbind_add("z7","tab 6",b);
    xbind_add("z8","tab 7",b);
    xbind_add("z9","tab 8",b);
    xbind_add("z0","tab 9",b);
    xbind_add("x1","tab -g0",b);
    xbind_add("x2","tab -g1",b);
    xbind_add("x3","tab -g2",b);
    xbind_add("x4","tab -g3",b);
    xbind_add("x5","tab -g4",b);
    xbind_add("x6","tab -g5",b);
    xbind_add("x7","tab -g6",b);
    xbind_add("x8","tab -g7",b);
    xbind_add("vta","select -ts - -o .",b);
    xbind_add("vth","select -ts - -o . .",b);
    xbind_add("vda","select -ds - -o .",b);
    xbind_add("vdh","select -ds - -o . .",b);
    xbind_add("vea","select -es - -o .",b);
    xbind_add("veh","select -es - -o . .",b);
    xbind_add("dct","ds -cs s",b);
    xbind_add("dCt","ds -crs s",b);
    xbind_add("dst","ds -s s",b);
    xbind_add("dSt","ds -rs s",b);
    xbind_add("dft","ds -fs s",b);
    xbind_add("dch","ds -cs .",b);
    xbind_add("dCh","ds -crs .",b);
    xbind_add("dsh","ds -s .",b);
    xbind_add("dSh","ds -rs .",b);
    xbind_add("dfh","ds -fs .",b);
    xbind_add("mm","fmod -Ms . -c -o .",b);
    xbind_add("mr","fmod -Ms . -r -o .",b);
    xbind_add("md","fmod -Ms . -d -o .",b);
    xbind_add("mM","fmod -Ms . -cm -o .",b);
    xbind_add("mR","fmod -Ms . -rm -o .",b);
    xbind_add("mD","fmod -Ms . -dm -o .",b);
    xbind_add("pp","fmod -Cs . -c -o .",b);
    xbind_add("pr","fmod -Cs . -r -o .",b);
    xbind_add("pd","fmod -Cs . -d -o .",b);
    xbind_add("pP","fmod -Cs . -cm -o .",b);
    xbind_add("pR","fmod -Cs . -rm -o .",b);
    xbind_add("pD","fmod -Cs . -dm -o .",b);
    xbind_add("Dd","fmod -Ds . .",b);
    xbind_add("DD","fmod -Ds .",b);
    xbind_add("Dt","fmod -Ds s",b);
    xbind_add("R","load -m1 .",b);
    xbind_add("a","rename %s",b);
    xbind_add("r","open_with %s",b);
    xbind_add("/","console \"search -N \"",b);
    xbind_add("n","search -n 1",b);
    xbind_add("N","search -p 1",b);
    xbind_add("f","console \"filter -N \"",b);
}

int
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

    if (funcs->type == 'a')
        free(funcs->func);

    funcs->type = type;
    funcs->func = func;

    return 0;
}


static void
add_functions(flexarr *f)
{
    xfunc_add("move",'f',cmd_move,f);
    xfunc_add("cd",'f',cmd_cd,f);
    xfunc_add("file_run",'f',cmd_file_run,f);
    xfunc_add("source",'f',cmd_source,f);
    xfunc_add("load",'f',cmd_load,f);
    xfunc_add("fastselect",'f',cmd_fastselect,f);
    xfunc_add("console",'f',cmd_console,f);
    xfunc_add("tab",'f',cmd_tab,f);
    xfunc_add("select",'f',cmd_select,f);
    xfunc_add("exec",'f',cmd_exec,f);
    xfunc_add("open_with",'f',cmd_open_with,f);
    xfunc_add("ds",'f',cmd_ds,f);
    xfunc_add("bulk",'f',cmd_bulk,f);
    xfunc_add("fmod",'f',cmd_fmod,f);
    xfunc_add("rename",'f',cmd_rename,f);
    xfunc_add("search",'f',cmd_search,f);
    xfunc_add("filter",'f',cmd_filter,f);
    xfunc_add("map",'f',cmd_map,f);
    xfunc_add("set",'f',cmd_set,f);
    xfunc_add("alias",'f',cmd_alias,f);
    xfunc_add("quit",'f',cmd_quit,f);
}

int
xvar_add(void *addr, const char *name, const uchar type, void *val, flexarr *v)
{
    ret_errno(name==NULL||v==NULL,EINVAL,-1);
    ret_errno(strlen(name)>FUNCTIONS_NAME_MAX,EOVERFLOW,-1);

    xvar *vars = (xvar*)v->v;
    size_t i;
    uchar found = 0;
    for (i = 0; i < v->size; i++) {
        if (strcmp(name,vars[i].name) == 0) {
            found = 1;
            break;
        }
    }

    if (found == 0) {
        vars = (xvar*)flexarr_inc(v);
        vars->name = malloc(VARS_NAME_MAX);
        strcpy(vars->name,name);
        vars->v = NULL;
    } else {
        vars = &vars[i];
        if ((vars->type == 's' || vars->type == 'i') && vars->type != type) {
            free(vars->v);
            vars->v = NULL;
        }
    }

    switch (type&~0x80) {
        case 's':
            addr = vars->v;
            if (!vars->v)
                addr = malloc(PATH_MAX);
            // fall through
        case 'S':
            vars->v = addr;
            if (val)
                strcpy(vars->v,val);
            break;
        case 'i':
            if (!vars->v)
                addr = malloc(sizeof(li));
            else
                addr = vars->v;
            // fall through
        case 'I':
            vars->v = addr;
            if (type&0x80)
                *(li*)vars->v = (li)val;
            else if (val)
                calc(val,vars->v,v);
            break;
    }

    vars->type = type&~0x80;
    return 0;
}

static void
add_vars(flexarr *v)
{
    xvar_add(&BufferSize,"BufferSize",'I',NULL,v);
    xvar_add(&COLS,"COLS",'I',NULL,v);
    xvar_add(&LINES,"LINES",'I',NULL,v);
    xvar_add(&Sort,"Sort",'I',NULL,v);
    xvar_add(NULL,"s_none",'i'|0x80,(void*)SORT_NONE,v);
    xvar_add(NULL,"s_name",'i'|0x80,(void*)SORT_NAME,v);
    xvar_add(NULL,"s_cname",'i'|0x80,(void*)SORT_CNAME,v);
    xvar_add(NULL,"s_size",'i'|0x80,(void*)SORT_SIZE,v);
    xvar_add(NULL,"s_type",'i'|0x80,(void*)SORT_TYPE,v);
    xvar_add(NULL,"s_reverse",'i'|0x80,(void*)SORT_REVERSE,v);
    xvar_add(NULL,"s_ddist",'i'|0x80,(void*)SORT_DIR_DISTINCTION,v);
    xvar_add(NULL,"s_lddist",'i'|0x80,(void*)SORT_LDIR_DISTINCTION,v);
    xvar_add(NULL,"s_rddist",'i'|0x80,(void*)SORT_REVERSE_DIR_DISTINCTIONS,v);
    xvar_add(&Visual,"Visual",'I',NULL,v);
    xvar_add(&MoveOffset,"MoveOffset",'I',NULL,v);
    xvar_add(&WrapScroll,"WrapScroll",'I',NULL,v);
    xvar_add(&JumpScroll,"JumpScroll",'I',NULL,v);
    xvar_add(&JumpScrollValue,"JumpScrollValue",'I',NULL,v);
}

csas *
csas_init()
{
    csas *ret = malloc(sizeof(csas));
    memset(ret->tabs,0,sizeof(xtab)*TABS);
    ret->ctab = 0;
    ret->dirs = flexarr_init(sizeof(xdir),DIR_INCR);
    ret->consoleh = flexarr_init(sizeof(char**),HISTORY_INCR);
    ret->vars = flexarr_init(sizeof(xvar),VARS_INCR);
    ret->functions = flexarr_init(sizeof(xfunc),FUNCTIONS_INCR);
    ret->bindings = flexarr_init(sizeof(xbind),BINDINGS_INCR);
    initcurses();
    ret->win = subwin(stdscr,LINES-2,COLS,1,0);

    add_functions(ret->functions);
    add_bindings(ret->bindings);
    add_vars(ret->vars);

    return ret;
}

static void
csas_draw(csas *cs)
{
    draw_tbar(0,cs);
    draw_bbar(LINES-1,cs);
    draw_dir(cs->win,&CTAB,cs);
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
    li n = getdir(path,cs->dirs,D_CHDIR);
    if (n == -1)
        exiterr();
    cs->tabs[cs->ctab].t = (size_t)n;
    cs->tabs[cs->ctab].flags |= T_EXISTS;

    config_load("/etc/csasrc",cs);

    struct timespec timer;
    int e;
    time_t t1,t2=0;
    struct stat statbuf;

    while (!Exit) {
        clock_gettime(1,&timer);
        t1 = timer.tv_sec;

        csas_draw(cs);

        REPEAT: ;
        if ((e = update_event(cs)) != -1) {
            if (command_run(BINDINGS[e].value,cs) == -1) {
                printmsg(ERROR_C,"%s: %s",BINDINGS[e].value,strerror(errno));
                refresh();
                goto REPEAT;
            }
        }

        if (t1 != t2) {
            xdir *d = &CTAB;
            t2 = t1;
            if (lstat(d->path,&statbuf) != 0)
                continue;
            if (memcmp(&statbuf.st_ctim,&d->ctime,sizeof(struct timespec)) != 0) {
                if (DirLoadingMode == D_MODE_ONCE)
                    d->flags |= S_CHANGED;
                else
                    getdir(d->path,cs->dirs,DirLoadingMode);
            }
        }
    }
}

void
xdir_free(xdir *dir)
{
    flexarr_free(dir->searchlist);
    free(dir->path);
    dir->path = NULL;
    dir->plen = 0;
    for (size_t i = 0; i < dir->asize; i++)
        free(dir->files[i].name);
    free(dir->files);
    dir->files = NULL;
    dir->size = 0;
}
