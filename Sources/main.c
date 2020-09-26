#include "main.h"
#include "config.h"
#include "Bulk.h"
#include "Usefull.h"
#include "Load.h"

#ifdef __SORT_ELEMENTS_ENABLE__
#include "Sort.h"
#endif

#include "Functions.h"
#include "Chars.h"
#include "FastRun.h"

int main(int argc, char** argv)
{
    printf("%d\n",argc);
    setlocale(LC_ALL, "");
    signal(SIGINT,SIG_IGN);

    /*struct ShortDir* sda1 = (struct ShortDir*)malloc(sizeof(struct ShortDir)*4);
    for (int i = 0; i < 4; i++)
    {
        sda1[i].cwd = (char*)malloc(PATH_MAX);
        sda1[i].names = (char**)malloc(sizeof(char*)*6);
        for (int j = 0; j < 6; j++)
            sda1[i].names[j] = (char*)malloc(NAME_MAX);
    }

    strcpy(sda1[0].cwd,"/run");
    strcpy(sda1[0].names[0],"media");
    strcpy(sda1[0].names[1],"mount");
    strcpy(sda1[0].names[2],"log");
    strcpy(sda1[0].names[3],"lvm");
    sda1[0].names_t = 4;
    strcpy(sda1[1].cwd,"/bin");
    strcpy(sda1[1].names[0],"bash");
    strcpy(sda1[1].names[1],"sh");
    strcpy(sda1[1].names[2],"zsh");
    strcpy(sda1[1].names[3],"dash");
    sda1[1].names_t = 4;*/

    //bulk(sda1,"/bin/vim","/bin/sh","mv","","");

    //RunFile(argv[1]);

    time_t ActualTime, PastTime = 0;
    struct timespec MainTimer;
    /*for (;;)
    {
        clock_gettime(1,&MainTimer);
        ActualTime = (double)MainTimer.tv_sec+(double)MainTimer.tv_nsec/1000000000;
        printf("%f\n",ActualTime);
    }*/

    Basic* Armor = InitBasic();

    noecho();

    curs_set(0);
    keypad(stdscr,TRUE);
    if (settings->DelayBetweenFrames > -1)
        halfdelay(settings->DelayBetweenFrames);
    else
        nodelay(stdscr,true);

    if (has_colors() && settings->EnableColor)
    {
        start_color();
        use_default_colors();

        for (int i = 0; i < 16; i++)
            init_pair(i,i,-1);
    }

    bool ExitTime = false;


    int si;
    char* cSF = (char*)calloc(64,sizeof(char));

    /*for (int i = 0; i < 20001; i++)
    {
        if (i%2000 == 0)
            printf("%ld %ld %d %.f%%\n",Armor->ActualSize, Armor->AllocatedSize,i,((float)i/20001)*100);
        GetDir("/bin",Armor,0);
        GetDir("/sbin",Armor,1);
        GetDir("/lib",Armor,2);
        GetDir("/lib64",Armor,0);
    }*/

    // Error
    // loading much directories from first loaded directory after loading directories
    // from other directory causes Segmentation fault
    // So i load the main directory, because he's in most cases small
    //CD("/",Armor);
    CD(argv[1],Armor);
    //int score = 0;

    do {
        clock_gettime(1,&MainTimer);
        ActualTime = MainTimer.tv_sec;

        DrawBasic(Armor,-1);

        UpdateEvent(&si,cSF,Armor);
        if (si != -1)
            RunEvent(si,&ExitTime,Armor,cSF);
        /*else if (ActualTime != PastTime)
        {
            for (int i = 0; i < 2; i++)
            {
                if ((i == 0 && !Win1Enable) || (i == 2 && !Win3Enable) || Armor->Work[Armor->inW].win[i]->enable)
                    continue;

                GetDir(Armor->Work[Armor->inW].win[i]->path,Armor,i,Threads);
            }
        }*/

        //endwin();
        //printf("%d\n",si);
        //initscr();

        /*c = getch();
        if (c == 'q')
            ExitTime = true;
        else
        {
            if (c == 410)
                UpdateSizeBasic(Armor);
        }*/

        //score++;

        if (ActualTime != PastTime)
        {
            PastTime = ActualTime;
            GetDir(".",Armor,1,false);
            if (settings->Win1Display)
                GetDir("..",Armor,0,false);
            /*endwin();
            printf("%d\n",score);
            initscr();
            score = 0;*/
        }
    } while (!ExitTime);

    endwin();

    freeBasic(Armor);

    free(cSF);

    return 0;
}
