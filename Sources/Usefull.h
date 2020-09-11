#ifndef USEFULL_H
#define USEFULL_H

#ifdef __HUMAN_READABLE_SIZE_ENABLE__
void MakeHumanReadAble(char*, unsigned long long int, bool);
#endif

#ifdef __GET_DIR_SIZE_ENABLE__
unsigned long long int GetDirSize(int, bool, bool);
#endif

#ifdef __COLOR_FILES_BY_EXTENSION__
void CheckFileTypeN(struct Element* List, size_t begin, size_t end);
#endif

size_t xstrsncpy(char *dst, const char *src, size_t n);
char* lsperms(int mode, int type);

void TimeToStr(time_t *, char*);

void RunFile(const char*);
void MakePathShorter(char*, int);

#endif