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

extern Settings *cfg;

void run_preview(WINDOW *w, uchar *c, ssize_t size, uli flags)
{
    register int posx = 1+cfg->Borders*2, posy = cfg->Borders;

    for (register ssize_t i = 0; i < size && posy <= w->_maxy-cfg->Borders; i++)
    {
        if (flags&F_WRAP && w->_maxx-(cfg->Borders*2)-1 < posx)
        {
            posy++;
            posx = 1+cfg->Borders*2;
            mvwaddch(w,posy,posx++,c[i]);
        }
        else if (c[i] == '\n')
        {
            posy++;
            posx = 1+(cfg->Borders<<1);
        }
        else
            mvwaddch(w,posy,posx++,c[i]);
    }
}

static void *getfromfile(void *arg)
{
    Csas *cs = (Csas*)arg;
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
    if (sfile.st_mtim.tv_sec == G_ES(cs->current_ws,1).mtim && G_ES(cs->current_ws,1).cpreview)
        goto END;
    G_ES(cs->current_ws,1).previewl = 0;
    #endif

    ssize_t buffl;
    char buffer[1024];
    if ((buffl = read(fd,buffer,1024)) < 1)
        goto END;

    uchar bina = 0;

    for (ssize_t i = 0; i < buffl; i++)
    {
        if ((buffer[i] >= 0x07 && buffer[i] <= 0xd) || (buffer[i] >= 0x20 && buffer[i] <= 0x7e))
            continue;
        bina = 1;
        break;
    }

    #ifdef __SAVE_PREVIEW__
    uchar buff[PREVIEW_MAX];
    #endif

    if (!bina)
    {
        if (!(cfg->PreviewSettings&PREV_ASCII))
            goto END;
        lseek(fd,0,SEEK_SET);
        #ifndef __SAVE_PREVIEW__
        cs->previewl = read(fd,cs->cpreview,PREVIEW_MAX);
        cs->spreview = F_TEXT;
        #else
        buffl = read(fd,buff,PREVIEW_MAX);
        G_ES(cs->current_ws,1).cpreview = realloc(G_ES(cs->current_ws,1).cpreview,buffl);
        memcpy(G_ES(cs->current_ws,1).cpreview,buff,buffl);
        G_ES(cs->current_ws,1).previewl = buffl;
        G_ES(cs->current_ws,1).spreview = F_TEXT;
        #endif
    }
    else
    {
        if (!(cfg->PreviewSettings&PREV_BINARY))
            goto END;

        int pipes[2];
        if (pipe(pipes) == -1)
            goto END;

        if (fork() == 0)
        {
            close(pipes[0]);
            dup2(pipes[1],1);
            execlp(cfg->BinaryPreview,cfg->BinaryPreview,G_ES(cs->current_ws,1).name,NULL);
            _exit(1);
        }
        else
        {
            
            close(pipes[1]);
            while (wait(NULL) != -1);
            #ifndef __SAVE_PREVIEW__
            cs->previewl = read(pipes[0],cs->cpreview,PREVIEW_MAX);
            cs->spreview = F_TEXT|F_WRAP;
            #else
            buffl = read(pipes[0],buff,PREVIEW_MAX);
            G_ES(cs->current_ws,1).cpreview = realloc(G_ES(cs->current_ws,1).cpreview,buffl);
            memcpy(G_ES(cs->current_ws,1).cpreview,buff,buffl);
            G_ES(cs->current_ws,1).previewl = buffl;
            G_ES(cs->current_ws,1).spreview = F_TEXT|F_WRAP;
            #endif
            close(pipes[0]);
        }
    }

    END: ;
    close(fd);
    #ifdef __THREADS_FOR_FILE_ENABLE__
    if (cfg->ThreadsForFile)
    {
        pthread_detach(pthread_self());
        pthread_exit(NULL);
    }
    #endif
    return NULL;
}

void get_preview(Csas *cs)
{
    if (cs->ws[cs->current_ws].win[1] == -1 ||
    #ifdef __THREADS_FOR_DIR_ENABLE__
    G_D(cs->current_ws,1)->enable ||
    #endif
    G_D(cs->current_ws,1)->size < 1)
        return;

    werase(cs->win[2]);
    if (cfg->Borders)
        setborders(cs,2);

    if (cfg->PreviewSettings&PREV_DIR && (G_ES(cs->current_ws,1).type&T_GT) == T_DIR)
    {
        cfg->Win3Display = true;
        getdir(G_ES(cs->current_ws,1).name,cs,cs->current_ws,2,cfg->DirLoadingMode
        #ifdef __FOLLOW_PARENT_DIR__
        ,NULL
        #endif
        #ifdef __THREADS_FOR_DIR_ENABLE__
        ,cfg->ThreadsForDir
        #endif
        );
        return;
    }

    cfg->Win3Display = false;

    if (cfg->PreviewSettings&PREV_FILE && (G_ES(cs->current_ws,1).type&T_GT) == T_REG)
    {
        #ifdef __THREADS_FOR_FILE_ENABLE__
        if (cfg->ThreadsForFile)
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
