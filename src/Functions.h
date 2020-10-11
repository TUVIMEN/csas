#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void addKey(const Key);
Basic* InitBasic();
void RunBasic(Basic*,const int,char**);
void UpdateSizeBasic(Basic*);
void freeBasic(Basic*);

#endif