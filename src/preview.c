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

#include "main.h"
#include "preview.h"
#include "load.h"
#include "draw.h"
#include "useful.h"

extern li s_Win3Display;
extern li s_Borders;
extern li s_PreviewSettings;
extern char *s_BinaryPreview;
extern li s_PreviewMaxThreads;
extern li s_DirLoadingMode;
extern li s_ThreadsForFile;
extern li s_ThreadsForDir;

void
run_preview(WINDOW *w, uchar *c, ssize_t size, uli flags)
{
    register int posx = 1+s_Borders*2, posy = s_Borders;

    for (register ssize_t i = 0; i < size && posy <= w->_maxy-s_Borders; i++) {
        if (flags&F_WRAP && w->_maxx-(s_Borders*2)-1 < posx) {
            posy++;
            posx = 1+s_Borders*2;
            mvwaddch(w,posy,posx++,c[i]);
        } else if (c[i] == '\n') {
            posy++;
            posx = 1+(s_Borders<<1);
        }
        else
            mvwaddch(w,posy,posx++,c[i]);
    }
}

#ifdef __THREADS_FOR_FILE_ENABLE__
static int threads_size = 0;
#endif

static void *
getfromfile(void *arg)
{
    #ifdef __THREADS_FOR_FILE_ENABLE__
    threads_size++;
    #endif
    char *buffer = NULL;
    csas *cs = (csas*)arg;
    int fd;
    #ifndef __SAVE_PREVIEW__
    cs->previewl = 0;
    #endif
    if ((fd = open(G_ES(cs->current_ws,1).name,O_RDONLY)) == -1)
        goto END;
    struct stat sfile;
    if (fstat(fd,&sfile) == -1)
        goto END;
    if (!(sfile.st_mode&S_IRUSR))
        goto END;
    if (sfile.st_size == 0)
        goto END;

    #ifdef __SAVE_PREVIEW__
    if ((sfile.st_mtim.tv_sec == G_ES(cs->current_ws,1).mtim && G_ES(cs->current_ws,1).cpreview)
    #ifdef __THREADS_FOR_FILE_ENABLE__
    || G_ES(cs->current_ws,1).spreview&F_TREAD_ENABLE
    #endif
    )
        goto END;

    #ifdef __THREADS_FOR_FILE_ENABLE__
    G_ES(cs->current_ws,1).spreview |= F_TREAD_ENABLE;
    #endif

    G_ES(cs->current_ws,1).previewl = 0;
    if (G_ES(cs->current_ws,1).cpreview != NULL) {
        free(G_ES(cs->current_ws,1).cpreview);
        G_ES(cs->current_ws,1).cpreview = NULL;
    }
    #endif

    buffer = malloc((PREVIEW_MAX > 2048) ? PREVIEW_MAX : 2048);
    ssize_t buffl;
    if ((buffl = read(fd,buffer,2048)) < 1)
        goto END;

    uchar bina = 0;

    for (ssize_t i = 0; i < buffl; i++) {
        if ((buffer[i] >= 0x07 && buffer[i] <= 0xd) || (buffer[i] >= 0x20 && buffer[i] <= 0x7e))
            continue;
        bina = 1;
        break;
    }

    if (!bina) {
        if (!(s_PreviewSettings&PREV_ASCII))
            goto END_t;
        lseek(fd,0,SEEK_SET);
        #ifndef __SAVE_PREVIEW__
        cs->previewl = read(fd,cs->cpreview,PREVIEW_MAX);
        cs->spreview = F_TEXT;
        #else
        buffl = read(fd,buffer,PREVIEW_MAX);
        if (buffl != 0) {
            G_ES(cs->current_ws,1).cpreview = malloc(buffl);
            memcpy(G_ES(cs->current_ws,1).cpreview,buffer,buffl);
            G_ES(cs->current_ws,1).previewl = buffl;
        }
        G_ES(cs->current_ws,1).spreview = F_TEXT;
        #endif
    } else {
        if (!(s_PreviewSettings&PREV_BINARY))
            goto END_t;

        int pipes[2];
        if (pipe(pipes) == -1)
            goto END_t;

        if (vfork() == 0) {
            dup2(pipes[1],1);
            close(pipes[0]);
            close(pipes[1]);
            execlp(s_BinaryPreview,s_BinaryPreview,G_ES(cs->current_ws,1).name,NULL);
            _exit(1);
        } else {
            close(pipes[1]);
            while (wait(NULL) != -1);
            #ifndef __SAVE_PREVIEW__
            cs->previewl = read(pipes[0],cs->cpreview,PREVIEW_MAX);
            cs->spreview = F_TEXT|F_WRAP;
            #else
            buffl = read(pipes[0],buffer,PREVIEW_MAX);
            if (buffl != 0) {
                G_ES(cs->current_ws,1).cpreview = malloc(buffl);
                memcpy(G_ES(cs->current_ws,1).cpreview,buffer,buffl);
                G_ES(cs->current_ws,1).previewl = buffl;
            }
            G_ES(cs->current_ws,1).spreview = F_TEXT|F_WRAP;
            #endif
            close(pipes[0]);
        }
    }

    END_t:
    #ifdef __THREADS_FOR_FILE_ENABLE__
    G_ES(cs->current_ws,1).spreview &= ~F_TREAD_ENABLE;
    #endif

    END: ;
    free(buffer);
    #ifdef __THREADS_FOR_FILE_ENABLE__
    threads_size--;
    #endif
    close(fd);
    #ifdef __THREADS_FOR_FILE_ENABLE__
    if (s_ThreadsForFile) {
        pthread_detach(pthread_self());
        pthread_exit(NULL);
    }
    #endif
    return NULL;
}

void
get_preview(csas *cs)
{
    #ifdef __THREADS_FOR_FILE_ENABLE__
    if (threads_size >= s_PreviewMaxThreads)
        return;
    #endif
    if (cs->ws[cs->current_ws].win[1] == -1 ||
    #ifdef __THREADS_FOR_DIR_ENABLE__
    G_D(cs->current_ws,1)->enable ||
    #endif
    G_D(cs->current_ws,1)->size < 1)
        return;

    werase(cs->win[2]);
    if (s_Borders) setborders(cs,2);

    if (s_PreviewSettings&PREV_DIR && (G_ES(cs->current_ws,1).type&T_GT) == T_DIR) {
        s_Win3Display = true;
        getdir(G_ES(cs->current_ws,1).name,cs,cs->current_ws,2,s_DirLoadingMode
        #ifdef __FOLLOW_PARENT_DIR__
        ,NULL
        #endif
        #ifdef __THREADS_FOR_DIR_ENABLE__
        ,s_ThreadsForDir
        #endif
        );
        return;
    }

    s_Win3Display = false;

    if (s_PreviewSettings&PREV_FILE && (G_ES(cs->current_ws,1).type&T_GT) == T_REG) {
        #ifdef __THREADS_FOR_FILE_ENABLE__
        if (s_ThreadsForFile)
        {
            pthread_t th;
            pthread_create(&th,NULL,getfromfile,(void*)cs);
        }
        else
        #endif
        getfromfile((void*)cs);
    }

    wrefresh(cs->win[2]);
}
