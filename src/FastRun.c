#include "main.h"
#include "FastRun.h"
#include "Load.h"
#include "Functions.h"
#include "Usefull.h"

extern Settings* settings;

void FastRun(Basic* this)
{
    ClearWindow(this->win[2]);

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

    /*if (this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].Type == T_REG ||
        this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].Type == T_LREG)
    {
        struct stat sFile;
        if (stat(this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].name,&sFile) == -1)
            return;
        if (!(sFile.st_mode&S_IRUSR))
            return;

        if (sFile.st_size == 0)
        {
            return;
        }

        int fd = open(this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].name,O_RDONLY);

        size_t buf_t = 2048;

        char* buf = (char*)calloc(buf_t,1);

        buf_t = read(fd,buf,buf_t-1);

        int bina = 0;
        bool binary = false;

        for (int i = 0; i < buf_t; i++)
            bina += 1*(buf[i] != 10 && (buf[i] < 32 || buf[i] > 126));

        binary = bina > 32;

        if (!binary)
        {
            //mvwprintw(this->win[2],0,0,"%s is binary",this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].name);
            mvwprintw(this->win[2],0,0,"%s",buf);
        }

        free(buf);

    }*/

    wrefresh(this->win[2]);

}
