#include "main.h"
#include "calc.h"

static size_t
get_bin(const char *v, li *n)
{
    size_t pos=0,end=0;
    while (v[end] == '0' || v[end] == '1')
        end++;
    int num=0;
    for (; pos != end; pos++)
        num |= (v[(end-1)-pos]-48)<<pos;
    *n = num;
    return pos;
}

static size_t
get_oct(const char *v, li *n)
{
    size_t pos=0,end=0;
    while (v[end] >= '0' && v[end] <= '7')
        end++;
    int num=0;
    num |= v[--end]-48;
    if (!end)
        goto END;
    pos++;
    for (size_t i=pos-1; i != end; i++,pos++)
        num |= (v[(end-1)-i]-48)<<(3*pos);
    END: ;
    *n = num;
    return pos;
}


static size_t
get_dec(const char *v, li *n)
{
    size_t pos=0,end=0;
    while (isdigit(v[end]))
        end++;
    li num=0,c=1;
    for (; pos != end; pos++) {
        num += (v[(end-1)-pos]-48)*c;
        c *= 10;
    }
    *n = num;
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

static size_t
get_hex(const char *v, li *n)
{
    size_t pos=0,end=0;
    while (isxdigit(v[end]))
        end++;
    li num=0;
    for (; pos != end; pos++)
        num |= hextodec(v[(end-1)-pos])<<(pos<<2);
    *n = num;
    return pos;
}

static size_t
get_num(const char *v, li *n, flexarr *vars)
{
    size_t pos=0;
    char c=0;

    if (isalpha(*v) || *v == '_') {
        pos++;
        while (isalnum(v[pos]) || v[pos] == '_')
            pos++;
        size_t i;
        uchar found = 0;
        xvar *var = (xvar*)vars->v;
        for (i = 0; i < vars->size; i++) {
            if (pos == strlen(var[i].name) && memcmp(v,var[i].name,pos) == 0) {
                found = 1;
                break;
            }
        }
        if (!found || var[i].type == 's' || var[i].type == 'S') {
            *n = 0;
            return pos;
        }
        *n = *(li*)var[i].v;
        return pos;
    }

    if (*v == '-' || *v == '~') {
        c = *v;
        pos++;
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
        if (c)
            *n *= -1;
        return pos;
    }
    
    if (v[pos] == '(') {
        pos++;
        pos += calc(v+pos,n,vars);
        while (v[pos] == ')')
            pos++;
    } else {
        pos += get_dec(v+pos,n);
    }
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
