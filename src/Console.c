/*
    csas - terminal file manager
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
    {"console",&___CONSOLE},
    {"bulk",&___BULK},
    #ifdef __LOAD_CONFIG_ENABLE__
    {"include",&___INCLUDE},
    #endif
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
