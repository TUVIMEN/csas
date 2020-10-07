#ifndef LOAD_H
#define LOAD_H

void* LoadDir(void*);
void GetDir(const char*, Basic*, const int, const char
#ifdef __THREADS_FOR_DIR_ENABLE__
, const bool
#endif
);
void CD(const char*, Basic*);

#endif
