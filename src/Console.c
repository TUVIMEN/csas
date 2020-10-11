#include "main.h"
#include "Loading.h"
#include "Functions.h"
#include "Chars.h"
#include "Load.h"
#include "Usefull.h"
#include "FastRun.h"

extern struct AliasesT aliases[];
extern Settings* settings;

void RunCommand(char* src, Basic* grf)
{
    size_t pos = 0, end = 0;
    enum Actions set;

    pos += FindFirstCharacter(src);
    while (src[pos+end] && !isspace(src[pos+end])) end++;

    if (end == 4 && strncmp(src+pos,"move",end) == 0)
        set = A_MOVE;
    else if (end == 10 && strncmp(src+pos,"fastselect",end) == 0)
        set = A_FASTSELECT;
    else if (end == 2 && strncmp(src+pos,"cd",end) == 0)
        set = A_CD;
    else if (end == 15 && strncmp(src+pos,"ChangeWorkSpace",end) == 0)
        set = A_CHANGEWORKSPACE;
    else if (end == 5 && strncmp(src+pos,"gotop",end) == 0)
        set = A_GOTOP;
    else if (end == 6 && strncmp(src+pos,"godown",end) == 0)
        set = A_GODOWN;
    #ifdef __GET_DIR_SIZE_ENABLE__
    else if (end == 7 && strncmp(src+pos,"getsize",end) == 0)
        set = A_GETSIZE;
    #endif
    else if (end == 8 && strncmp(src+pos,"setgroup",end) == 0)
        set = A_SETGROUP;
    else if (end == 6 && strncmp(src+pos,"select",end) == 0)
        set = A_SELECT;
    else if (end == 12 && strncmp(src+pos,"togglevisual",end) == 0)
        set = A_TOGGLEVISUAL;
    else if (end == 6 && strncmp(src+pos,"f_move",end) == 0)
        set = A_F_MOVE;
    else if (end == 6 && strncmp(src+pos,"f_copy",end) == 0)
        set = A_F_COPY;
    else if (end == 8 && strncmp(src+pos,"f_delete",end) == 0)
        set = A_F_DELETE;
    else if (end == 3 && strncmp(src+pos,"set",end) == 0)
        set = A_SET;
    else if (end == 3 && strncmp(src+pos,"map",end) == 0)
        set = A_MAP;
    else if (end == 4 && strncmp(src+pos,"load",end) == 0)
        set = A_LOAD;
    else if (end == 4 && strncmp(src+pos,"exec",end) == 0)
        set = A_EXEC;
    else if (end == 4 && strncmp(src+pos,"quit",end) == 0)
        set = A_QUIT;
    else if (end == 7 && strncmp(src+pos,"include",end) == 0)
        set = A_INCLUDE;
    else    return;

    pos += end;
    pos += FindFirstCharacter(src+pos);

    char temp[8192];


    switch (set)
    {
        case A_MOVE:
            ___MOVE(src+pos,grf);
            break;
        case A_INCLUDE:
            ___INCLUDE(src+pos,temp,grf);
            break;
        case A_SET:
            ___SET(src+pos,temp);
            break;
        case A_MAP:
            ___MAP(src+pos,temp);
            break;
        case A_QUIT:
            ___QUIT(src+pos,grf);
            break;
        case A_CD:
            ___CD(src+pos,temp,grf);
            break;
        case A_GOTOP:
            ___GOTOP(src+pos,grf);
            break;
        case A_GODOWN:
            ___GODOWN(src+pos,grf);
            break;
        case A_CHANGEWORKSPACE:
            ___CHANGEWORKSPACE(src+pos,grf);
            break;
        #ifdef __GET_DIR_SIZE_ENABLE__
        case A_GETSIZE:
            ___GETSIZE(src+pos,grf);
            break;
        #endif
        case A_SETGROUP:
            ___SETGROUP(src+pos,grf);
            break;
        case A_FASTSELECT:
            ___FASTSELECT(src+pos,grf);
            break;
        case A_TOGGLEVISUAL:
            ___TOGGLEVISUAL(grf);
            break;
        case A_F_COPY:
            ___F_COPY(src+pos,grf);
            break;
        case A_F_MOVE:
            ___F_MOVE(src+pos,grf);
            break;
        case A_F_DELETE:
            ___F_DELETE(src+pos,grf);
            break;
        case A_LOAD:
            ___LOAD(src+pos);
            break;
        case A_SELECT:
            ___SELECT(src+pos,grf);
            break;
        case A_EXEC:
            ___EXEC(src+pos);
            break;
    }

}
