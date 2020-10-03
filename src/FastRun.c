#include "main.h"
#include "FastRun.h"
#include "Load.h"
#include "Functions.h"
#include "Usefull.h"

extern Settings* settings;

void FastRun(Basic* this)
{
    werase(this->win[2]);
    if (settings->Borders)
        SetBorders(this,2);

    if (this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].Type == T_DIR ||
        this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].Type == T_LDIR)
    {
        settings->Win3Display = true;
        GetDir(this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].name,this,2
        #ifdef __THREADS_ENABLE__
        ,settings->Threads
        #endif
        );
        return;
    }

    settings->Win3Display = false;
    static char buffer[16384];

    if (this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].Type == T_REG ||
        this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].Type == T_LREG)
    {
        int fd;
        if ((fd = open(this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].name,O_RDONLY)) == -1)
        {
            wrefresh(this->win[2]);
            return;
        }
        struct stat sFile;
        if (fstat(fd,&sFile) == -1)
        {
            wrefresh(this->win[2]);
            return;
        }
        if (!(sFile.st_mode&S_IRUSR))
            return;

        if (sFile.st_size == 0)
        {
            wrefresh(this->win[2]);
            return;
        }

        size_t buf_t = 256;

        if ((buf_t = read(fd,buffer,buf_t)) < 1)
        {
            wrefresh(this->win[2]);
            return;
        }

        int bina = 0;
        bool binary = false;

        for (size_t i = 0; i < buf_t; i++)
            bina += 1*(buffer[i] != 10 && (buffer[i] < 32 || buffer[i] > 126));

        binary = bina*0.05;

        if (!binary)
        {
            lseek(fd,0,SEEK_SET);
            int posx = 1+settings->Borders*2, posy = settings->Borders;
            while ((buf_t = read(fd,buffer,1)) > 0 && posy <= this->win[2]->_maxy-settings->Borders && posx <= this->win[2]->_maxx-settings->Borders)
            {
                if (buffer[0] == '\n')
                {
                    posy++;
                    posx = 1+settings->Borders*2;
                    continue;
                }
                else
                    mvwaddch(this->win[2],posy,posx++,buffer[0]);
            }
        }
        else
        {
            
        }
    }

    wrefresh(this->win[2]);

}
