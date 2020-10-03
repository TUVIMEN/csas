#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void addKey(Key);
Basic* InitBasic();
void RunBasic(Basic*,const int,char**);
void SetBorders(Basic*, const int);
void UpdateSizeBasic(Basic*);
void DrawBasic(Basic*,const int);
void freeBasic(Basic*);

#endif