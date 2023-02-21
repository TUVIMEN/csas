/*
    csas - console file manager
    Copyright (C) 2020-2023 Dominik Stanisław Suchora <suchora.dominik7@gmail.com>

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
#include "flexarr.h"

flexarr *
flexarr_init(const size_t nmemb, const size_t inc_r)
{
  flexarr *ret = calloc(sizeof(flexarr),1);
  ret->inc_r = inc_r;
  ret->nmemb = nmemb;
  return ret;
}

void *
flexarr_inc(flexarr *f)
{
  if (f->size == f->asize) {
    void *v = realloc(f->v,(f->asize+=f->inc_r)*f->nmemb);
    if (v == NULL)
      return NULL;
    f->v = v;
  }
  return f->v+(f->size++*f->nmemb);
}

void *
flexarr_dec(flexarr *f)
{
  if (f->size == 0)
    return NULL;
  return f->v+(f->size--*f->nmemb);
}

void *
flexarr_set(flexarr *f, const size_t s)
{
  if (f->size >= s || f->asize >= s)
    return NULL;
  void *v = realloc(f->v,s*f->nmemb);
  if (v == NULL)
    return NULL;
  return f->v = v;
}

void *
flexarr_clearb(flexarr *f)
{
  void *v = realloc(f->v,f->size*f->nmemb);
  if (v == NULL)
    return NULL;
  return f->v = v;
}

void
flexarr_free(flexarr *f)
{
  free(f->v);
  f->v = NULL;
  f->size = 0;
  f->asize = 0;
  free(f);
}
