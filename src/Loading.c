#include "main.h"
#include "Loading.h"
#include "Functions.h"

#ifdef __LOAD_CONFIG_ENABLE__
extern struct AliasesT aliases[];

void LoadConfig(char* path, Settings* this)
{
    int fd;
    if ((fd = open(path,O_RDONLY)) == -1)
        return;

    struct stat sFile;
    if (fstat(fd,&sFile) == -1)
        return;

    char* file = (char*)malloc(sFile.st_size+1);
    read(fd,file,sFile.st_size);
    close(fd);

    char *str1, *str2, *str3, *token, *subtoken, *subtoken2;
    char *saveptr1, *saveptr2, *saveptr3;
    int g,h,j, setting1,setting2;//, setting3;
    Key TempKey;

    struct SetEntry SetEntries[] = {
        {"FileOpener",1,&this->FileOpener},
        {"shell",1,&this->shell},{"editor",1,&this->editor},{"BarSettings",2,&this->BarSettings},
        {"UserHostPattern",1,&this->UserHostPattern},{"CopyBufferSize",2,&this->CopyBufferSize},
        {"INOTIFY_MASK",2,&this->INOTIFY_MASK},{"MoveOffSet",3,&this->MoveOffSet},
        {"WrapScroll",0,&this->WrapScroll},{"JumpScrollValue",3,&this->JumpScrollValue},
        {"StatusBarOnTop",0,&this->StatusBarOnTop},{"WinSizeMod",4,&this->WinSizeMod},
        {"Win1Enable",0,&this->Win1Enable},{"Win3Enable",0,&this->Win3Enable},{"UserRHost",0,&this->UserRHost},
        {"Bar1Enable",0,&this->Bar1Enable},{"Bar2Enable",0,&this->Bar2Enable},
        {"Borders",0,&this->Borders},{"FillBlankSpace",0,&this->FillBlankSpace},
        {"WindowBorder",5,&this->WindowBorder},{"EnableColor",0,&this->EnableColor},
        {"DelayBetweenFrames",2,&this->DelayBetweenFrames},{"NumberLines",0,&this->NumberLines},
        {"NumberLinesOff",0,&this->NumberLinesOff},{"NumberLinesFromOne",0,&this->NumberLinesFromOne},
        {"DisplayingC",2,&this->DisplayingC},{"JumpScroll",0,&this->JumpScroll},{"Values",1,&this->Values},
        #ifdef __SHOW_HIDDEN_FILES_ENABLE__
        {"ShowHiddenFiles",0,&this->ShowHiddenFiles},
        #endif
        #ifdef __SORT_ELEMENTS_ENABLE__
        {"SortMethod",2,&this->SortMethod},{"BetterFiles",5,&this->BetterFiles},
        #endif
        #ifdef __BLOCK_SIZE_ELEMENTS_ENABLE__
        {"BlockSize",2,&this->BlockSize},
        #endif
        {"DirSizeMethod",2,&this->DirSizeMethod},{"C_Error",2,&this->C_Error},
        #ifdef __COLOR_FILES_BY_EXTENSION__
        {"C_FType_A",2,&this->C_FType_A},{"C_FType_I",2,&this->C_FType_I},{"C_FType_V",2,&this->C_FType_V},
        #endif
        {"C_Selected",2,&this->C_Selected},{"C_Exec_set",2,&this->C_Exec_set},{"C_Exec",2,&this->C_Exec},
        {"C_BLink",2,&this->C_BLink},{"C_Dir",2,&this->C_Dir},{"C_Reg",2,&this->C_Reg},{"C_Fifo",2,&this->C_Fifo},
        {"C_Sock",2,&this->C_Sock},{"C_Dev",2,&this->C_Dev},{"C_BDev",2,&this->C_BDev},{"C_LDir",2,&this->C_LDir},
        {"C_LReg",2,&this->C_LReg},{"C_LFifo",2,&this->C_LFifo},{"C_LSock",2,&this->C_LSock},{"C_LDev",2,&this->C_LDev},
        {"C_LBDev",2,&this->C_LBDev},{"C_Other",2,&this->C_Other},{"C_User_S_D",2,&this->C_User_S_D},
        {"C_Bar_Dir",2,&this->C_Bar_Dir},{"C_Bar_Name",2,&this->C_Bar_Name},{"C_Bar_WorkSpace",2,&this->C_Bar_WorkSpace},
        {"C_Bar_WorkSpace_Selected",2,&this->C_Bar_WorkSpace_Selected},{"C_Group_0",2,&this->C_Group_0},
        {"C_Group_1",2,&this->C_Group_1},{"C_Group_2",2,&this->C_Group_2},{"C_Group_3",2,&this->C_Group_3},
        {"C_Group_4",2,&this->C_Group_4},{"C_Group_5",2,&this->C_Group_5},{"C_Group_6",2,&this->C_Group_6},
        {"C_Group_7",2,&this->C_Group_7},{"C_Bar_F",2,&this->C_Bar_F},{"C_Bar_E",2,&this->C_Bar_E},
        #ifdef __THREADS_ENABLE__
        {"Threads",0,&this->Threads},
        #endif
        {NULL,0,NULL}
    };

    for (j = 1, str1 = file;  ; j++, str1 = NULL)
    {
        token = strtok_r(str1,"\n;",&saveptr1);
        if (token == NULL)
            break;
        setting1 = -1;
        setting2 = -1;
        TempKey.act = -1;
        TempKey.keys = NULL;
        TempKey.slc1.ll = 0;
        TempKey.slc2.ll = 0;

        for (str2 = token, h = 0; ; str2 = NULL, h++)
        {
            subtoken = strtok_r(str2, " =", &saveptr2);
            if (subtoken == NULL)
                break;

            if (h == 0)
            {
                if (strcmp(subtoken,"set") == 0)
                    setting1 = 1;
                else if (strcmp(subtoken,"include") == 0)
                    setting1 = 2;
                else if (strcmp(subtoken,"map") == 0)
                    setting1 = 3;
            }
            if (h == 1 && setting1 != -1)
            {
                if (setting1 == 1)
                {
                    for (int i = 0; SetEntries[i].name != NULL; i++)
                    {
                        if (strcmp(subtoken,SetEntries[i].name) == 0)
                        {
                            setting2 = i;
                            break;
                        }
                    }
                }
                else if (setting1 == 2)
                {
                    LoadConfig(subtoken,this);
                    break;
                }
                else if (setting1 == 3)
                {
                    TempKey.keys = subtoken;
                }
            }
            if (h == 2)
            {
                if (setting1 == 1 && setting2 != -1)
                {
                    switch (SetEntries[setting2].type)
                    {
                        case 0: //bool
                            if (subtoken[0] == '1' || subtoken[0] == 't' || subtoken[0] == 'T')
                                *(bool*)SetEntries[setting2].value = 1;
                            else
                                *(bool*)SetEntries[setting2].value = 0;
                            break;
                        case 1: //char*
                            strcpy(*(char**)SetEntries[setting2].value,subtoken);
                            break;
                        case 2: //int
                            *(long long int*)SetEntries[setting2].value = 0;
                            for (str3 = subtoken, g = 0; ; str3 = NULL, g++)
                            {
                                subtoken2 = strtok_r(str3, ",|", &saveptr3);
                                if (subtoken2 == NULL)
                                    break;

                                if (subtoken2[0] > 47 && subtoken2[0] < 58)
                                    *(long long int*)SetEntries[setting2].value |= atoll(subtoken2);
                                else
                                    for (int i = 0; aliases[i].name != NULL; i++)
                                    {
                                        if (strcmp(subtoken2,aliases[i].name) == 0)
                                        {
                                            *(long long int*)SetEntries[setting2].value |= aliases[i].v;
                                            break;
                                        }
                                    }
                            }
                            break;
                        case 3: //float
                            *(float*)SetEntries[setting2].value = atof(subtoken);
                            break;
                        case 4:
                            for (str3 = subtoken, g = 0; ; str3 = NULL, g++)
                            {
                                subtoken2 = strtok_r(str3, ",|", &saveptr3);
                                if (subtoken2 == NULL)
                                    break;
                                (*(float**)SetEntries[setting2].value)[g] = atof(subtoken2);
                            }
                            break;
                        case 5:
                            for (str3 = subtoken, g = 0; ; str3 = NULL, g++)
                            {
                                subtoken2 = strtok_r(str3, ",|", &saveptr3);
                                if (subtoken2 == NULL)
                                    break;

                                if (subtoken2[0] > 47 && subtoken2[0] < 58)
                                {
                                    (*(long long int**)SetEntries[setting2].value)[g] = atoi(subtoken2);
                                }
                                else
                                    for (int i = 0; aliases[i].name != NULL; i++)
                                    {
                                        if (strcmp(subtoken2,aliases[i].name) == 0)
                                        {
                                            (*((int**)SetEntries[setting2].value))[g] = aliases[i].v;
                                            break;
                                        }
                                    }
                            }
                            break;
                    }
                    break;
                }
                else if (setting1 == 3)
                {
                    TempKey.act = atoi(subtoken);
                }
            }
            if (h == 3)
            {
                if (setting1 == 3)
                    TempKey.slc1.ll = atoll(subtoken);
            }
            if (h == 4)
            {
                if (setting1 == 3)
                {
                    TempKey.slc2.ll = atoll(subtoken);
                    addKey(TempKey);
                }
            }

        }
    }


    free(file);
}

#endif
