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

extern Settings* settings;

void* PreviewRun(void* arg)
{
    Basic* grf = (Basic*)arg;

    close(grf->preview_fd);
    grf->preview_fd = -1;

    char buffer[1024];
    int fd;
    if ((fd = open(GET_ESELECTED(grf->current_workspace,1).name,O_RDONLY)) == -1)
        goto END;
    struct stat sfile;
    if (fstat(fd,&sfile) == -1)
        goto END;
    //#ifdef __SAVE_PREVIEW__
    //if (sfile.st_mtim.tv_sec == GET_ESELECTED(grf->current_workspace,1).mtim && GET_ESELECTED(grf->current_workspace,1).preview)
    //{

    //    goto END;
    //}
    //#endif
    if (!(sfile.st_mode&S_IRUSR))
        goto END;
    if (sfile.st_size == 0)
        goto END;
    size_t buf_t;
    if ((buf_t = read(fd,buffer,1024)) < 1)
        goto END;

    size_t bina = 0;
    bool binary = false;

    for (size_t i = 0; i < buf_t; i++)
        bina += 1*!(isascii(buffer[i]));

    binary = (bina<<1);

    if (!binary)
    {
        if (!(settings->PreviewSettings&PREV_ASCII))
        {
            close(fd);
            goto END;
        }
        grf->preview_settings |= F_TEXT;
        grf->preview_settings ^= F_WRAP*((grf->preview_settings&F_WRAP) == F_WRAP);
        grf->preview_fd = fd;
    }
    else
    {
        close(fd);
        if (!(settings->PreviewSettings&PREV_BINARY))
            goto END;

        int pipes[2];
        if (pipe(pipes) == -1)
            goto END;

        if (fork() == 0)
        {
            close(pipes[0]);
            dup2(pipes[1],1);
            execlp("file","file","-b",GET_ESELECTED(grf->current_workspace,1).name,NULL);
            _exit(1);
        }
        else
        {
            close(pipes[1]);
            wait(NULL);
            grf->preview_settings |= F_TEXT|F_WRAP;
            grf->preview_fd = pipes[0];
        }
    }

    END: ;
    #ifdef __THREADS_FOR_FILE_ENABLE__
    if (settings->ThreadsForFile)
    {
        pthread_detach(pthread_self());
        pthread_exit(NULL);
    }
    #endif
    return NULL;
}

void Preview(Basic* grf)
{
    if (grf->workspaces[grf->current_workspace].win[1] == -1 ||
    #ifdef __THREADS_FOR_DIR_ENABLE__
    GET_DIR(grf->current_workspace,1)->enable ||
    #endif
    GET_DIR(grf->current_workspace,1)->size < 1 || settings->PreviewSettings&PREV_NONE)
        return;

    werase(grf->win[2]);
    if (settings->Borders)
        SetBorders(grf,2);

    if ((GET_ESELECTED(grf->current_workspace,1).type&T_GT) == T_DIR)
    {
        settings->Win3Display = true;
        GetDir(GET_ESELECTED(grf->current_workspace,1).name,grf,grf->current_workspace,2,settings->DirLoadingMode
        #ifdef __FOLLOW_PARENT_DIR__
        ,NULL
        #endif
        #ifdef __THREADS_FOR_DIR_ENABLE__
        ,settings->ThreadsForDir
        #endif
        );
        return;
    }

    settings->Win3Display = false;

    if ((GET_ESELECTED(grf->current_workspace,1).type&T_GT) == T_REG)
    {
        #ifdef __THREADS_FOR_FILE_ENABLE__
        if (settings->ThreadsForFile)
        {
            pthread_t th;
            pthread_create(&th,NULL,PreviewRun,(void*)grf);
        }
        else
        #endif
        PreviewRun((void*)grf);

    }


    wrefresh(grf->win[2]);
}
