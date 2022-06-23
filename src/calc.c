/*
    csas - console file manager
    Copyright (C) 2020-2022 Dominik Stanisław Suchora <suchora.dominik7@gmail.com>

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

#include "main.h"
#include "calc.h"

size_t
get_bin(const char *v, li *n)
{
    size_t pos=0;
    *n = 0;
    while (v[pos] == '0' || v[pos] == '1')
        *n = (*n<<1)|(v[pos++]-48);
    return pos;
}

size_t
get_oct(const char *v, li *n)
{
    size_t pos=0;
    *n = 0;
    while (v[pos] >= '0' && v[pos] <= '7')
        *n = (*n<<3)|(v[pos++]-48);
    return pos;
}


size_t
get_dec(const char *v, li *n)
{
    size_t pos=0;
    *n = 0;
    while (isdigit(v[pos]))
        *n = (*n*10)+(v[pos++]-48);
    return pos;
}

static int
hextodec(int n)
{
    if (n >= '0' && n <= '9')
        return n-48;
    if (n >= 'A' && n <= 'F')
        return n-55;
    if (n >= 'a' && n <= 'f')
        return n-87;
    return 0;
}

size_t
get_hex(const char *v, li *n)
{
    size_t pos=0;
    *n = 0;
    while (isxdigit(v[pos]))
        *n = (*n<<4)|hextodec(v[pos++]);
    return pos;
}

static size_t
get_num(const char *v, li *n, flexarr *vars)
{
    size_t pos=0;
    char c=0;

    if (*v == '-' || *v == '~') {
        c = *v;
        pos++;
    }

    if (isalpha(v[pos]) || v[pos] == '_') {
        size_t begin = pos;
        while (isalnum(v[pos]) || v[pos] == '_')
            pos++;
        size_t i;
        uchar found = 0;
        xvar *var = (xvar*)vars->v;
        for (i = 0; i < vars->size; i++) {
            if (pos-begin == strlen(var[i].name) && memcmp(v+begin,var[i].name,pos-begin) == 0) {
                found = 1;
                break;
            }
        }
        if (!found || var[i].type&XVAR_STRING) {
            *n = 0;
            return pos;
        }
        *n = *(li*)var[i].v;
        goto END;
    }

    if (v[pos] == '0' && v[pos+1] && v[pos+1] != '.') {
        pos++;
        if (v[pos] == 'b') {
            pos++;
            pos += get_bin(v+pos,n);
        } else if (v[pos] == 'x') {
            pos++;
            pos += get_hex(v+pos,n);
        } else if (isdigit(v[pos])) {
            pos += get_oct(v+pos,n);
        }
        goto END;
    }
    
    if (v[pos] == '(') {
        pos++;
        pos += calc(v+pos,n,vars);
        while (v[pos] == ')')
            pos++;
    } else {
        pos += get_dec(v+pos,n);
    }
    END: ;
    if (c == '-')
        *n *= -1;
    if (c == '~')
        *n = ~*n;
    return pos;
}

static size_t
level_1(const char *v, li *n, flexarr *vars)
{
    size_t pos=0;
    char c=0;
    li num=0,t;
    do {
        while (isspace(v[pos]))
            pos++;
        pos += get_num(v+pos,&t,vars);
        while (isspace(v[pos]))
            pos++;
        if (c == 0)
            num = t;
        if (c == '|')
            num |= t;
        if (c == '&')
            num &= t;
        if (c == '^')
            num ^= t;
        c = *(v+pos);
        pos++;
    } while (c == '|' || c == '&' || c == '^');
    *n = num;
    return pos-1;
}

static size_t
level_2(const char *v, li *n, flexarr *vars)
{
    size_t pos=0;
    char c=0;
    li num=0,t;
    do {
        while (isspace(v[pos]))
            pos++;
        pos += level_1(v+pos,&t,vars);
        while (isspace(v[pos]))
            pos++;
        if (c == 0)
            num = t;
        if (c == '*')
            num *= t;
        if (c == '/' || c == '%') {
            if (t == 0)
                num = 0;
            else if (c == '/')
                num /= t;
            else
                num %= t;
        }
        c = *(v+pos);
        pos++;
    } while (c == '*' || c == '/' || c == '%');
    *n = num;
    return pos-1;
}

size_t
level_3(const char *v, li *n, flexarr *vars)
{
    size_t pos=0;
    char c=0;
    li num=0,t;
    do {
        while (isspace(v[pos]))
            pos++;
        pos += level_2(v+pos,&t,vars);
        while (isspace(v[pos]))
            pos++;
        if (c == 0)
            num = t;
        if (c == '>')
            num >>= t;
        if (c == '<')
            num <<= t;
        c = *(v+pos);
        pos++;
        if (c != '>' && c != '<')
            break;
        c = *(v+pos);
        pos++;
    } while (c == '>' || c == '<');
    *n = num;
    return pos-1;
}

size_t
calc(const char *v, li *n, flexarr *vars)
{
    size_t pos=0;
    char c=0;
    li num=0,t;
    do {
        while (isspace(v[pos]))
            pos++;
        pos += level_3(v+pos,&t,vars);
        while (isspace(v[pos]))
            pos++;
        if (c == 0)
            num = t;
        if (c == '+')
            num += t;
        if (c == '-')
            num -= t;
        c = *(v+pos);
        pos++;
    } while (c == '+' || c == '-');
    *n = num;
    return pos-1;
}
