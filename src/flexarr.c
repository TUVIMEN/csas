/*
    reliq - html searching tool
    Copyright (C) 2020-2024 Dominik Stanisław Suchora <suchora.dominik7@gmail.com>

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

#include <stdlib.h>
#include <string.h>
#include <err.h>

#include "flexarr.h"

static void
alloc_error()
{
    err(69,"could not allocate");
}

void *
xmalloc(size_t size)
{
    void *ret = malloc(size);
    if (ret == NULL)
        alloc_error();
    return ret;
}

void *
xcalloc(size_t nmemb, size_t size)
{
    void *ret = calloc(nmemb,size);
    if (ret == NULL)
        alloc_error();
    return ret;
}

void *
xrealloc(void *ptr,size_t size)
{
    void *ret = realloc(ptr,size);
    if (size && ret == NULL)
        alloc_error();
    return ret;
}

flexarr *
flexarr_init(const size_t elsize, const size_t inc_r)
{
  flexarr *ret = xcalloc(1,sizeof(flexarr));
  ret->inc_r = inc_r;
  ret->elsize = elsize;
  return ret;
}

void *
flexarr_inc(flexarr *f)
{
  if (f->size == f->asize)
    f->v = xrealloc(f->v,(f->asize+=f->inc_r)*f->elsize);
  return f->v+(f->size++*f->elsize);
}

void *
flexarr_dec(flexarr *f)
{
  if (f->size == 0)
    return NULL;
  return f->v+(f->size--*f->elsize);
}

void *
flexarr_set(flexarr *f, const size_t s) //set number of allocated elements to s
{
  if (f->size >= s || f->asize >= s)
    return NULL;
  f->asize = s;
  return f->v = xrealloc(f->v,s*f->elsize);;
}

void *
flexarr_alloc(flexarr *f, const size_t s) //allocate additional s amount of elements
{
  if (s == 0 || f->asize-f->size >= s)
    return f->v;
  f->asize = f->size+s;
  return f->v = xrealloc(f->v,(f->size+s)*f->elsize);
}

void *
flexarr_add(flexarr *dst, const flexarr *src) //append contents of src to dst
{
  if (dst->size+src->size > dst->asize)
    if (flexarr_alloc(dst,src->size) == NULL)
      return NULL;
  memcpy(dst->v+(dst->size*dst->elsize),src->v,src->size*dst->elsize);
  dst->size += src->size;
  return dst->v;
}

void *
flexarr_clearb(flexarr *f) //clear buffer
{
  if (f->size == f->asize || !f->v)
      return NULL;
  f->asize = f->size;
  return f->v = xrealloc(f->v,f->size*f->elsize);
}

void
flexarr_conv(flexarr *f, void **v, size_t *s) //convert from flexarr to normal array
{
  flexarr_clearb(f);
  *s = f->size;
  if (!f->size && f->v) {
    free(f->v);
    f->v = NULL;
  }
  *v = f->v;
  free(f);
}

void
flexarr_free(flexarr *f)
{
  if (!f)
      return;
  if (f->asize)
    free(f->v);
  f->v = NULL;
  f->size = 0;
  f->asize = 0;
  free(f);
}
