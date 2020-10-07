#include "main.h"
#include "Functions.h"

int main(int argc, char** argv)
{
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

    Basic* Armor = InitBasic();
    RunBasic(Armor,argc,argv);

    return 0;
}
