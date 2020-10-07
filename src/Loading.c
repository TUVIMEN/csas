#include "main.h"
#include "Loading.h"
#include "Functions.h"

#ifdef __LOAD_CONFIG_ENABLE__
extern struct AliasesT aliases[];
extern Settings* settings;

char* StrConv(char* grf)
{
    for (int i = 0; grf[i]; i++)
    {
        if (grf[i] == '\\' && grf[i+1])
        {
            for (int j = i; grf[j]; j++)
                grf[j] = grf[j+1];

            switch (grf[i])
            {
                case '0':
                    grf[i] = '\x0';
                    break;
                case 'a':
                    grf[i] = '\x7';
                    break;
                case 'b':
                    grf[i] = '\x8';
                    break;
                case 't':
                    grf[i] = '\x9';
                    break;
                case 'n':
                    grf[i] = '\xA';
                    break;
                case 'v':
                    grf[i] = '\xB';
                    break;
                case 'f':
                    grf[i] = '\xC';
                    break;
                case 'r':
                    grf[i] = '\xD';
                    break;
                case '\'':
                    grf[i] = '\x27';
                    break;
                case '\\':
                    grf[i] = '\x5C';
                    break;
            }
        }
    }
    return grf;
}

void StrToType(struct SetEntry* grf, char* file, char* temp, size_t* PosBegin)
{
    int arr_t = 0;
    bool arr = grf->type&0x8;
    size_t PosEnd = 0;
    do
    {
        switch (grf->type^(0x8*arr))
        {
            case 0:
                if (file[*PosBegin] == '1' || file[*PosBegin] == 't' || file[*PosBegin] == 'T')
                    if (arr)
                        (*((bool**)grf->value))[arr_t] = 1;
                    else
                        *(bool*)grf->value = 1;
                else
                    if (arr)
                        (*((bool**)grf->value))[arr_t] = 0;
                    else
                        *(bool*)grf->value = 0;
                while (file[*PosBegin] && (file[*PosBegin] != '\n' && file[*PosBegin] != ' ' && file[*PosBegin] != ','))
                    (*PosBegin)++;
                (*PosBegin)++;
                break;
            case 1:
                if (file[*PosBegin] && file[*PosBegin] == '\'')
                {
                    PosEnd = 0;
                    (*PosBegin)++;
                    while (file[*PosBegin+PosEnd] && (file[*PosBegin+PosEnd] != '\'' || file[*PosBegin+PosEnd-1] == '\\'))
                        PosEnd++;
                    if (PosEnd)
                    {
                        memset(temp,0,8191);
                        strncpy(temp,file+*PosBegin,PosEnd);
                        StrConv(temp);
                        if (arr)
                            strcpy((*((char***)grf->value))[arr_t],temp);
                        else
                            strcpy(*(char**)grf->value,temp);
                    }
                    *PosBegin += PosEnd+2;
                }
                break;
            case 2:
                if (arr)
                    (*((long int**)grf->value))[arr_t] = 0;
                else
                    *(long int*)grf->value = 0;
                size_t itemp;
                do {
                    PosEnd = 0;
                    while (file[*PosBegin+PosEnd] && (file[*PosBegin+PosEnd] != '|' && file[*PosBegin+PosEnd] != ',' && file[*PosBegin+PosEnd] != ' ' && file[*PosBegin+PosEnd] != '\n'))
                        PosEnd++;
                    if (file[*PosBegin] == '-' || (file[*PosBegin] > 47 && file[*PosBegin] < 58))
                    {
                        strncpy(temp,file+*PosBegin,PosEnd);
                        if (arr)
                            (*((long int**)grf->value))[arr_t] |= atoll(temp);
                        else
                            *(long int*)grf->value |= atoll(temp);
                        memset(temp,0,8191);
                    }
                    else
                    {
                        for (int j = 0; aliases[j].name; j++)
                        {
                            itemp = strlen(aliases[j].name);
                            if (itemp == PosEnd && strncmp(file+*PosBegin,aliases[j].name,itemp) == 0)
                            {
                                if (arr)
                                    (*((long int**)grf->value))[arr_t] |= aliases[j].v;
                                else
                                    *(long int*)grf->value |= aliases[j].v;
                                break;
                            }
                        }
                    }

                    *PosBegin += PosEnd+1;
                } while (!file[*PosBegin] || file[*PosBegin-1] == '|');
                break;
            case 3:
                PosEnd = 0;
                while (file[*PosBegin+PosEnd] && (file[*PosBegin+PosEnd] != '|' && file[*PosBegin+PosEnd] != ',' && file[*PosBegin+PosEnd] != ' ' && file[*PosBegin+PosEnd] != '\n'))
                    PosEnd++;
                
                strncpy(temp,file+*PosBegin,PosEnd);
                if (arr)
                    (*((double**)grf->value))[arr_t] = atof(temp);
                else
                    *(double*)grf->value = atof(temp);
                memset(temp,0,8191);
                
                *PosBegin += PosEnd+1;
                break;
        }

        arr_t++;
    } while (arr && file[*PosBegin-1] == ',');
}

char* StrToKeys(char* grf)
{
    for (int i = 0; grf[i]; i++)
    {
        if (grf[i] == '<' && grf[i+1] == 'C' && grf[i+2] == '-' && grf[i+3] && grf[i+4] == '>')
        {
            for (int g = 0; g < 4; g++)
                for (int j = i+(g == 3); grf[j]; j++)
                    grf[j] = grf[j+1];
            grf[i] &= 0x1f;
        }
        else if (strncmp(grf+i,"<space>",7) == 0)
        {
            for (int g = 0; g < 6; g++)
                for (int j = i; grf[j]; j++)
                    grf[j] = grf[j+1];
            grf[i] = ' ';  
        }
        else if (strncmp(grf+i,"<esc>",5) == 0)
        {
            for (int g = 0; g < 4; g++)
                for (int j = i; grf[j]; j++)
                    grf[j] = grf[j+1];
            grf[i] = 27;
        }
    }
    return grf;
}

void LoadConfig(const char* path)
{
    int fd;
    if ((fd = open(path,O_RDONLY)) == -1)
        return;

    struct stat sFile;
    if (fstat(fd,&sFile) == -1)
    {
        close(fd);
        return;
    }

    char* file = (char*)malloc(sFile.st_size+1);
    read(fd,file,sFile.st_size);
    close(fd);


    struct SetEntry SetEntries[] = {
        {"FileOpener",1,&settings->FileOpener},
        {"shell",1,&settings->shell},{"editor",1,&settings->editor},{"Bar1Settings",2,&settings->Bar1Settings},
        {"Bar2Settings",2,&settings->Bar2Settings},
        {"UserHostPattern",1,&settings->UserHostPattern},{"CopyBufferSize",2,&settings->CopyBufferSize},
        #ifdef __INOTIFY_ENABLE__
        {"INOTIFY_MASK",2,&settings->INOTIFY_MASK},{"DirLoadingMode",2,&settings->DirLoadingMode},
        #endif
        {"MoveOffSet",3,&settings->MoveOffSet},{"SDelayBetweenFrames",2,&settings->SDelayBetweenFrames},
        {"WrapScroll",0,&settings->WrapScroll},{"JumpScrollValue",3,&settings->JumpScrollValue},
        {"StatusBarOnTop",0,&settings->StatusBarOnTop},{"WinSizeMod",3|8,&settings->WinSizeMod},
        {"Win1Enable",0,&settings->Win1Enable},{"Win3Enable",0,&settings->Win3Enable},{"UserRHost",0,&settings->UserRHost},
        {"Bar1Enable",0,&settings->Bar1Enable},{"Bar2Enable",0,&settings->Bar2Enable},
        {"Borders",0,&settings->Borders},{"FillBlankSpace",0,&settings->FillBlankSpace},
        {"WindowBorder",2|8,&settings->WindowBorder},{"EnableColor",0,&settings->EnableColor},
        {"DelayBetweenFrames",2,&settings->DelayBetweenFrames},{"NumberLines",0,&settings->NumberLines},
        {"NumberLinesOff",0,&settings->NumberLinesOff},{"NumberLinesFromOne",0,&settings->NumberLinesFromOne},
        {"DisplayingC",2,&settings->DisplayingC},{"JumpScroll",0,&settings->JumpScroll},{"Values",1,&settings->Values},
        #ifdef __SHOW_HIDDEN_FILES_ENABLE__
        {"ShowHiddenFiles",0,&settings->ShowHiddenFiles},
        #endif
        #ifdef __SORT_ELEMENTS_ENABLE__
        {"SortMethod",2,&settings->SortMethod},{"BetterFiles",2|8,&settings->BetterFiles},
        #endif
        {"DirSizeMethod",2,&settings->DirSizeMethod},{"C_Error",2,&settings->C_Error},
        #ifdef __COLOR_FILES_BY_EXTENSION__
        {"C_FType_A",2,&settings->C_FType_A},{"C_FType_I",2,&settings->C_FType_I},{"C_FType_V",2,&settings->C_FType_V},
        #endif
        {"C_Selected",2,&settings->C_Selected},{"C_Exec_set",2,&settings->C_Exec_set},{"C_Exec",2,&settings->C_Exec},
        {"C_BLink",2,&settings->C_BLink},{"C_Dir",2,&settings->C_Dir},{"C_Reg",2,&settings->C_Reg},{"C_Fifo",2,&settings->C_Fifo},
        {"C_Sock",2,&settings->C_Sock},{"C_Dev",2,&settings->C_Dev},{"C_BDev",2,&settings->C_BDev},{"C_LDir",2,&settings->C_LDir},
        {"C_LReg",2,&settings->C_LReg},{"C_LFifo",2,&settings->C_LFifo},{"C_LSock",2,&settings->C_LSock},{"C_LDev",2,&settings->C_LDev},
        {"C_LBDev",2,&settings->C_LBDev},{"C_Other",2,&settings->C_Other},{"C_User_S_D",2,&settings->C_User_S_D},
        {"C_Bar_Dir",2,&settings->C_Bar_Dir},{"C_Bar_Name",2,&settings->C_Bar_Name},{"C_Bar_WorkSpace",2,&settings->C_Bar_WorkSpace},
        {"C_Bar_WorkSpace_Selected",2,&settings->C_Bar_WorkSpace_Selected},{"C_Group_0",2,&settings->C_Group_0},
        {"C_Group_1",2,&settings->C_Group_1},{"C_Group_2",2,&settings->C_Group_2},{"C_Group_3",2,&settings->C_Group_3},
        {"C_Group_4",2,&settings->C_Group_4},{"C_Group_5",2,&settings->C_Group_5},{"C_Group_6",2,&settings->C_Group_6},
        {"C_Group_7",2,&settings->C_Group_7},{"C_Bar_F",2,&settings->C_Bar_F},{"C_Bar_E",2,&settings->C_Bar_E},
        #ifdef __THREADS_FOR_DIR_ENABLE__
        {"ThreadsForDir",0,&settings->ThreadsForDir},
        #endif
        #ifdef __THREADS_FOR_FILE_ENABLE__
        {"ThreadsForFile",0,&settings->ThreadsForFile},
        #endif
        {NULL,0,NULL}
    };

    size_t PosBegin = 0, PosEnd = 0;
    char* temp = (char*)malloc(8192);
    Key TempKey;
    TempKey.keys = (char*)malloc(64);

    while (file[PosBegin])
    {
        if (strncmp(file+PosBegin,"set ",4) == 0)
        {
            PosBegin += 4;
            for (int i = 0; SetEntries[i].name; i++)
            {
                PosEnd = strlen(SetEntries[i].name);
                if (strncmp(file+PosBegin,SetEntries[i].name,PosEnd) == 0)
                {
                    PosBegin += PosEnd+1;
                    StrToType(&SetEntries[i],file,temp,&PosBegin);
                    break;
                }
            }
        }
        else if (strncmp(file+PosBegin,"include ",8) == 0)
        {
            PosBegin += 8;
            if (file[PosBegin] && file[PosBegin] == '\'')
            {
                PosEnd = 0;
                PosBegin++;
                while (file[PosBegin+PosEnd] && (file[PosBegin+PosEnd] != '\'' || file[PosBegin+PosEnd-1] == '\\'))
                    PosEnd++;
                if (PosEnd)
                {
                    strncpy(temp,file+PosBegin,PosEnd);
                    StrConv(temp);
                    LoadConfig(temp);
                    memset(temp,0,8191);
                }
                PosBegin += PosEnd+2;
            }
        }
        else if (strncmp(file+PosBegin,"map ",4) == 0)
        {
            size_t itemp;
            PosBegin += 4;
            PosEnd = 0;
            while (file[PosBegin+PosEnd] && file[PosBegin+PosEnd] != ' ')
                PosEnd++;
            strncpy(temp,file+PosBegin,PosEnd);
            StrToKeys(temp);
            PosBegin += PosEnd+1;
            strcpy(TempKey.keys,temp);
            memset(temp,0,8191);

            PosEnd = 0;
            while (file[PosBegin+PosEnd] && file[PosBegin+PosEnd] != ' ' && file[PosBegin+PosEnd] != '\n')
                PosEnd++;
            strncpy(temp,file+PosBegin,PosEnd);
            TempKey.act = atoi(temp);
            memset(temp,0,8181);
            PosBegin += PosEnd+1;
            TempKey.slc[0].ll = 0;
            TempKey.slc[1].ll = 0;
            if (file[PosBegin-1] == '\n')
            {
                addKey(TempKey);
                continue;
            }

            for (int g = 0; g < 2; g++)
            {
                PosEnd = 0;
                if (file[PosBegin] == '\'')
                {
                    PosBegin++;
                    while (file[PosBegin+PosEnd] && (file[PosBegin+PosEnd] != '\'' || file[PosBegin+PosEnd-1] == '\\'))
                        PosEnd++;

                    if (PosEnd)
                    {
                        strncpy(temp,file+PosBegin,PosEnd);
                        StrConv(temp);
                        TempKey.slc[g].v = malloc(PosEnd);
                        strcpy(TempKey.slc[g].v,temp);
                        memset(temp,0,8191);
                    }
                    PosBegin += PosEnd+1;
                }
                else
                {
                    do {
                        PosEnd = 0;
                        while (file[PosBegin+PosEnd] && (file[PosBegin+PosEnd] != '|' && file[PosBegin+PosEnd] != ',' && file[PosBegin+PosEnd] != ' ' && file[PosBegin+PosEnd] != '\n'))
                            PosEnd++;
                        
                        if (file[PosBegin] == '-' || (file[PosBegin] > 47 && file[PosBegin] < 58))
                        {
                            strncpy(temp,file+PosBegin,PosEnd);
                            TempKey.slc[g].ll |= atoll(temp);
                            memset(temp,0,8191);
                        }
                        else
                        {
                            for (int j = 0; aliases[j].name; j++)
                            {
                                itemp = strlen(aliases[j].name);
                                if (itemp == PosEnd && strncmp(file+PosBegin,aliases[j].name,itemp) == 0)
                                {
                                    TempKey.slc[g].ll |= aliases[j].v;
                                    break;
                                }
                            }
                        }

                        PosBegin += PosEnd+1;
                    } while (!file[PosBegin] || file[PosBegin-1] == '|');
                }
                if (file[PosBegin-1] == '\n')
                    break;
            }

            fflush(stdout);

            addKey(TempKey);
        }
        else
        {
            if (file[PosBegin] == '#')
            {
                while (file[PosBegin] != '\n' && file[PosBegin])
                    PosBegin++;
            }
            else
                PosBegin++;
        }
    }

    free(TempKey.keys);
    free(temp);
    free(file);
}

#endif


