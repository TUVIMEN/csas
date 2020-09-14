#include "main.h"
#include "FastRun.h"
#include "Load.h"
#include "Functions.h"

extern bool Borders;
extern bool Win3Enable;
extern bool Win3Display;

void FastRun(Basic* this)
{
    for (int i = Borders; i < this->win[2]->_maxy-Borders+1; i++)
        for (int j = Borders; j < this->win[2]->_maxx+1-Borders; j++)
            mvwaddch(this->win[2],i,j,' ');

    if (this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].Type == T_DIR ||
        this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].Type == T_LDIR)
    {
        Win3Display = true;
        GetDir(this->Work[this->inW].win[1]->El[this->Work[this->inW].win[1]->selected[this->inW]].name,this,2,1);
        return;
    }

    Win3Display = false;

    wrefresh(this->win[2]);

}
