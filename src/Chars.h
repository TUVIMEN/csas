/*
    csas - console file manager
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

#ifndef CHARS_H
#define CHARS_H

int UpdateEvent(Basic* grf);
void ExitBasic(Basic* grf, const bool force);

void ___SET(const char* src, Basic* grf);
#ifdef __LOAD_CONFIG_ENABLE__
void ___INCLUDE(const char* src, Basic* grf);
#endif
void ___MAP(const char* src, Basic* grf);
void ___MOVE(const char* src, Basic* grf);
void ___QUIT(const char* src, Basic* grf);
void ___CD(const char* src, Basic* grf);
void ___GOTOP(const char* src, Basic* grf);
void ___GODOWN(const char* src, Basic* grf);
void ___CHANGEWORKSPACE(const char* src, Basic* grf);
#ifdef __GET_DIR_SIZE_ENABLE__
void ___GETSIZE(const char* src,Basic* grf);
#endif
void ___SETGROUP(const char* src, Basic* grf);
void ___FASTSELECT(const char* src, Basic* grf);
void ___TOGGLEVISUAL(const char* src, Basic* grf);
void ___F_COPY(const char* src, Basic* grf);
void ___F_MOVE(const char* src, Basic* grf);
void ___F_DELETE(const char* src, Basic* grf);
void ___SEARCH(const char* src, Basic* grf);
void ___LOAD(const char* src, Basic* grf);
void ___SELECT(const char* src, Basic* grf);
void ___EXEC(const char* src, Basic* grf);
void ___BULK(const char* src, Basic* grf);
void ___CONSOLE(const char* src, Basic* grf);
void ___RENAME(const char* src, Basic* grf);
void ___FILTER(const char* src, Basic* grf);

#endif
