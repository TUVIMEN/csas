#include "main.h"
#include "Bulk.h"

char* MakeCharRunAble(char* temp)
{
    for (int i = 0; i < strlen(temp); i++)
    {
        if(temp[i] == '\\' || temp[i] == '\"' || temp[i] == '\'' || temp[i] == ' ' || temp[i] == '(' || temp[i] == ')' || temp[i] == '[' || temp[i] == ']' || temp[i] == '{' || temp[i] == '}')
        {
            for (int j = strlen(temp); j > i; j--)
                temp[j] = temp[j-1];
            temp[i] = '\\';
            i++;
        }
    }
    return temp;
}

void bulk(struct ShortDir* vector, char* set0, char* set1, char* set2, char* set3, char *set4)
{
    /*
        For settings:
            set0 - editor
            set1 - shell
            set2 - cns0
            set3 - cns1
            set4 - cns2
    */

    char *buf1 = (char*)malloc(256),
         *buf2 = (char*)malloc(256);
    
    strcpy(buf1,TEMPTEMP);
    strcpy(buf2,TEMPTEMP);
    
    while(mkstemp(buf1) == -1){}
    while(mkstemp(buf2) == -1){}

    FILE* file1 = fopen(buf1, "w");

    for (struct ShortDir* i = vector; i->names_t; i++)
    {
        fprintf(file1,"//\t%s\n",i->cwd);
        for (int j = 0; j < i->names_t; j++)
            fprintf(file1,"%s\n",i->names[j]);
    }

    fclose(file1);

    if (!fork())
        execlp(set0,set0,buf1,NULL);
    wait(NULL);

    file1 = fopen(buf1,"r");
    FILE* file2 = fopen(buf2,"w");

    char singleLine[PATH_MAX+1], *tempName = (char*)malloc(PATH_MAX), *tempName2 = (char*)malloc(PATH_MAX);
    int temp_t;
    bool ThereIsSomething = false;

    fprintf(file2,"#\t%s\n\n",set1);

    for (struct ShortDir* i = vector; i->names_t; i++)
    {
        for (int j = 0; j < i->names_t; j++)
        {
            if (singleLine[0] != '\0' && singleLine[0] != '/' && singleLine[0] != '\n')
            {
                strcpy(tempName,i->cwd);
                if (tempName[1] != '\n')
                    strcat(tempName,"/");
                strcat(tempName,singleLine);
                temp_t = strlen(singleLine);
                if (singleLine[temp_t-1] == '\n')
                    singleLine[temp_t-1] = 0;

                strcpy(tempName2,i->cwd);
                if (i->cwd[1] != 0)
                    strcat(tempName2,"/");
                strcat(tempName2,i->names[j]);

                if (strcmp(tempName2,tempName) != 0)
                {
                    ThereIsSomething = true;
                    fprintf(file2,"%s %s %s %s %s\n",set2,MakeCharRunAble(tempName2),set3,MakeCharRunAble(tempName),set4);
                }
            }
            else
                j--;
            fgets(singleLine,PATH_MAX,file1);
            singleLine[strlen(singleLine)-1] = '\0';
        }
        fprintf(file2,"\n");

    }
    
    free(tempName);
    free(tempName2);

    fclose(file1);
    fclose(file2);

    if (ThereIsSomething)
    {
        if (!fork())
            execlp(set0,set0,buf2,NULL);
        wait(NULL);
        if (!fork())
            execlp(set1,set1,buf2,NULL);
        wait(NULL);
    }

    remove(buf1);
    remove(buf2);

    free(buf1);
    free(buf2);
}