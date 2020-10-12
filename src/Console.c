#include "main.h"
#include "Loading.h"
#include "Functions.h"
#include "Chars.h"
#include "Load.h"
#include "Usefull.h"
#include "FastRun.h"

extern struct AliasesT aliases[];
extern Settings* settings;

struct FunctTab
{
    char* name;
    void (*function)(const char*, Basic*);
};

struct FunctTab FunctionsTable[] = {
    {"move",&___MOVE},
    {"fastselect",&___FASTSELECT},
    {"cd",&___CD},
    {"ChangeWorkSpace",&___CHANGEWORKSPACE},
    {"gotop",&___GOTOP},
    {"godown",&___GODOWN},
    #ifdef __GET_DIR_SIZE_ENABLE__
    {"getsize",&___GETSIZE},
    #endif
    {"setgroup",&___SETGROUP},
    {"select",&___SELECT},
    {"togglevisual",&___TOGGLEVISUAL},
    {"f_move",&___F_MOVE},
    {"f_copy",&___F_COPY},
    {"f_delete",&___F_DELETE},
    {"set",&___SET},
    {"map",&___MAP},
    {"load",&___LOAD},
    {"exec",&___EXEC},
    {"quit",&___QUIT},
    {"include",&___INCLUDE},
    {NULL,NULL}
};

void RunCommand(const char* src, Basic* grf)
{
    size_t pos = 0, end = 0;

    pos += FindFirstCharacter(src);
    while (src[pos+end] && !isspace(src[pos+end])) end++;

    for (int i = 0; FunctionsTable[i].name; i++)
    {
        if (end == strlen(FunctionsTable[i].name) && strncmp(src+pos,FunctionsTable[i].name,end) == 0)
        {
            pos += end;
            pos += FindFirstCharacter(src+pos);
            (*FunctionsTable[i].function)(src+pos,grf);
            break;
        }
    }

}
