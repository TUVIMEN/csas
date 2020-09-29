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

size_t xstrsncpy(char *dst, const char *src, size_t n);
char* lsperms(const int mode, const int type);

size_t TimeToStr(const time_t *, char*);

void RunFile(const char*);
void MakePathShorter(char*, const int);

void DeleteGroup(Basic*, const bool);
void MoveGroup(Basic*, const char*, const mode_t);
void CopyGroup(Basic*, const char*, const mode_t);

void ClearWindow(WINDOW* this);

#endif