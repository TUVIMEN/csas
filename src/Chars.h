#ifndef CHARS_H
#define CHARS_H

int UpdateEvent(Basic*);

void ___SET(const char*, Basic*);
#ifdef __LOAD_CONFIG_ENABLE__
void ___INCLUDE(const char*, Basic*);
#endif
void ___MAP(const char*, Basic*);
void ___MOVE(const char*, Basic*);
void ___QUIT(const char*, Basic*);
/*
    csas - terminal file manager
    Copyright (C) 2020 TUVIMEN <suchora.dominik7@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

void ___CD(const char*, Basic*);
void ___GOTOP(const char*, Basic*);
void ___GODOWN(const char*, Basic*);
void ___CHANGEWORKSPACE(const char*, Basic*);
#ifdef __GET_DIR_SIZE_ENABLE__
void ___GETSIZE(const char*,Basic*);
#endif
void ___SETGROUP(const char*, Basic*);
void ___FASTSELECT(const char*, Basic*);
void ___TOGGLEVISUAL(const char*, Basic*);
void ___F_COPY(const char*, Basic*);
void ___F_MOVE(const char*, Basic*);
void ___F_DELETE(const char*, Basic*);
void ___LOAD(const char*, Basic*);
void ___SELECT(const char*, Basic*);
void ___EXEC(const char*, Basic*);
void ___BULK(const char*, Basic*);
void ___CONSOLE(const char*, Basic*);


#endif