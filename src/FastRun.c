/*
    csas - console file manager
    Copyright (C) 2020 TUVIMEN <suchora.dominik7@gmail.com>

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
#include "FastRun.h"
#include "Load.h"
#include "Draw.h"
#include "Usefull.h"

extern Settings* settings;

void* FileRun(void* arg)
{
    Basic* grf = (Basic*)arg;

    char buffer[257];
    int fd;
    if ((fd = open(GET_ESELECTED(grf->inW,1).name,O_RDONLY)) == -1)
        goto END;
    struct stat sFile;
    if (fstat(fd,&sFile) == -1)
        goto END;
    if (!(sFile.st_mode&S_IRUSR))
        goto END;
    if (sFile.st_size == 0)
        goto END;
    size_t buf_t = 256;
    if ((buf_t = read(fd,buffer,buf_t)) < 1)
        goto END;

    int bina = 0;
    bool binary = false;

    for (size_t i = 0; i < buf_t; i++)
        bina += 1*(buffer[i] != 10 && (buffer[i] < 32 || buffer[i] > 126));

    binary = bina>3;

    close(grf->preview_fd);
    grf->preview_fd = -1;

    if (!binary)
    {
        grf->FastRunSettings |= F_TEXT;
        grf->FastRunSettings ^= F_WRAP*((grf->FastRunSettings&F_WRAP) == F_WRAP);
        grf->preview_fd = fd;
    }
    else
    {
        close(fd);
        int pipes[2];
        if (pipe(pipes) == -1)
            goto END;

        if (fork() == 0) //Delay must be small to see
        {
            close(pipes[0]);
            dup2(pipes[1],1);
            execlp("file","file","-b",GET_ESELECTED(grf->inW,1).name,NULL);
            _exit(1);
        }
        else
        {
            close(pipes[1]);
            wait(NULL);
            grf->FastRunSettings |= F_TEXT;
            grf->FastRunSettings |= F_WRAP;
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

void FastRun(Basic* grf)
{
    if (grf->Work[grf->inW].win[1] == -1 ||
    #ifdef __THREADS_FOR_DIR_ENABLE__
    GET_DIR(grf->inW,1)->enable ||
    #endif
    GET_DIR(grf->inW,1)->El_t < 1)
        return;

    werase(grf->win[2]);
    if (settings->Borders)
        SetBorders(grf,2);

    if (GET_ESELECTED(grf->inW,1).Type == T_DIR || GET_ESELECTED(grf->inW,1).Type == T_LDIR)
    {
        settings->Win3Display = true;
        GetDir(GET_ESELECTED(grf->inW,1).name,grf,grf->inW,2,settings->DirLoadingMode
        #ifdef __THREADS_FOR_DIR_ENABLE__
        ,settings->ThreadsForDir
        #endif
        );
        return;
    }

    settings->Win3Display = false;

    if (GET_ESELECTED(grf->inW,1).Type == T_REG || GET_ESELECTED(grf->inW,1).Type == T_LREG)
    {
        #ifdef __THREADS_FOR_FILE_ENABLE__
        if (settings->ThreadsForFile)
        {
            pthread_t th;
            pthread_create(&th,NULL,FileRun,(void*)grf);
        }
        else
        #endif
        FileRun((void*)grf);

    }


    wrefresh(grf->win[2]);
}
