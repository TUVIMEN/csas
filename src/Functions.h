#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void addKey(Key);
Basic* InitBasic();
void RunBasic(Basic*,const int,char**);
void UpdateSizeBasic(Basic*);
void freeBasic(Basic*);

#endif