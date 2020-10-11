#include "main.h"
#include "Loading.h"
#include "Functions.h"
#include "Console.h"

#ifdef __LOAD_CONFIG_ENABLE__
extern struct AliasesT aliases[];
extern Settings* settings;

void GetFullLine(char* dest, const char* src, size_t* n)
{
    size_t x = 0;
    size_t temp = 0;

    while (src[*n])
    {
        if (src[*n] == '\n' || src[*n] == ';')
        {
            if (x == 0)
                goto END;
            else if (src[*n-1] != '\\')
                goto END;
            else
                goto RES;

            END: ;
                dest[x] = '\0';
                break;
            RES: ;
                dest[x-1] = ' ';
                (*n)++;
        }

        if (src[*n] == '"' && src[*n-1] != '\\')
        {
            dest[x++] = '\'';
            (*n)++;
            while (src[*n])
            {
                if (src[*n] == '"' && src[*n-1] != '\\')
                    break;
                if (src[*n] == '\'' && src[*n-1] != '\\')
                {
                    dest[x++] = src[(*n)++];
                    temp = FindEndOf(src+*n,'\'');
                    strncpy(dest+x,src+*n,temp);
                    dest[x+temp] = '\0';
                    (*n) += temp;
                    x += temp;
                }
                if (src[*n] == '$' && src[*n-1] != '\\' && src[*n+1] == '{')
                {
                    *n += 2;
                    char temp1[NAME_MAX];
                    temp += FindEndOf(src+*n,'}');
                    strncpy(temp1,src+*n,temp);
                    temp1[temp] = '\0';
                    char* temp2 = getenv(temp1);
                    if (temp2)
                    {
                        memcpy(dest+x,temp2,strlen(temp2));
                        x += strlen(temp2);
                    }
                    (*n) += temp+1;
                    continue;
                }
                
                dest[x++] = src[(*n)++];
            }
            dest[x++] = '\'';
            (*n)++;
            continue;
        }

        if (src[*n] == '\'' && src[*n-1] != '\\')
        {
            dest[x++] = src[(*n)++];
            temp = FindEndOf(src+*n,'\'');
            strncpy(dest+x,src+*n,temp);
            dest[x+temp] = '\0';
            (*n) += temp;
            x += temp;
        }

        if (src[*n] == '/')
        {
            if (src[*n+1] == '/')
            {
                while (src[(*n)++])
                    if (src[*n] == '\n') break;
            }
            else if (src[*n+1] == '*')
            {
                while (src[(*n)++])
                    if (src[*n] == '*' && src[*n+1] == '/') break;
                *n += 2;
            }
        }
        
        dest[x++] = src[(*n)++];
    }
}

void LoadConfig(const char* path,Basic* grf)
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
    char line[16384];

    size_t Pos;

    while (file[Pos])
    {
        GetFullLine(line,file,&Pos);
        RunCommand(line,grf);
        Pos++;
    }
    
    free(file);
}

#endif


