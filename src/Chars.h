#ifndef CHARS_H
#define CHARS_H

int UpdateEvent(Basic*);

void ___SET(const char*, char*);
void ___INCLUDE(const char*, char*, Basic*);
void ___MAP(const char*, char*);
void ___MOVE(const char*, Basic*);
void ___QUIT(const char*, Basic*);
void ___CD(const char*, char*, Basic*);
void ___GOTOP(const char*, Basic*);
void ___GODOWN(const char*, Basic*);
void ___CHANGEWORKSPACE(const char*, Basic*);
#ifdef __GET_DIR_SIZE_ENABLE__
void ___GETSIZE(const char*,Basic*);
#endif
void ___SETGROUP(const char*, Basic*);
void ___FASTSELECT(const char*, Basic*);
void ___TOGGLEVISUAL(Basic*);
void ___F_COPY(const char*, Basic*);
void ___F_MOVE(const char*, Basic*);
void ___F_DELETE(const char*, Basic*);
void ___LOAD(const char*);
void ___SELECT(const char*, Basic*);
void ___EXEC(const char*);


#endif