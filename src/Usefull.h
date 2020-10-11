#ifndef USEFULL_H
#define USEFULL_H

#ifdef __HUMAN_READABLE_SIZE_ENABLE__
void MakeHumanReadAble(char*, const unsigned long long int, const bool);
#endif

#ifdef __GET_DIR_SIZE_ENABLE__
unsigned long long int GetDirSize(int, bool, bool);
#endif

#ifdef __COLOR_FILES_BY_EXTENSION__
void CheckFileExtension(const char*, unsigned char*);
#endif

size_t xstrsncpy(char*, const char*, size_t);
char* lsperms(const int, const int);

size_t TimeToStr(const time_t *, char*);

void RunFile(const char*);
void MakePathShorter(char*, const int);

void DeleteGroup(Basic*, const bool);
void MoveGroup(Basic*, const char*, const mode_t);
void CopyGroup(Basic*, const char*, const mode_t);

size_t FindFirstCharacter(const char*);
size_t StrToValue(void*, const char*, char*);
char* StrConv(char*);
char* StrToKeys(char*);
size_t FindEndOf(const char*, const char);

#endif

//if (src[*n] == '"' && src[*n-1] != '\\')
        //{
            /*dest[x++] = src[(*n)++];
            while (src[*n])
            {
                if (src[*n] == '"' && src[*n-1] != '\\')
                    break;*/
                /*if (src[*n] == '\'' && src[*n-1] != '\\')
                {
                    dest[x++] = src[(*n)++];
                    temp = FindEndOf(src+*n,'\'');
                    strncpy(dest+x,src+*n,temp);
                    dest[x+temp] = '\0';
                    (*n) += temp;
                    x += temp;
                }*/
                /*if (src[*n] == '$' && src[*n-1] != '\\' && src[*n+1] == '{')
                {
                    *n += 2;
                    char temp1[NAME_MAX];
                    temp += FindEndOf(src+*n,'}');
                    strncpy(temp1,src+*n,temp);
                    temp1[temp] = '\0';
                    char* temp2 = getenv(temp1);
                    if (temp2)
                    {
                        strncpy(dest+x,temp2,strlen(temp2));
                        x += strlen(temp2);
                    }
                    (*n) += temp+1;
                }*/
                
                /*dest[x++] = src[(*n)++];
            }*/
        //}