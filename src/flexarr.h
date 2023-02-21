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

#ifndef FLEXARR_H
#define FLEXARR_H

typedef struct {
  void *v;
  size_t asize;
  size_t size;
  size_t nmemb;
  size_t inc_r;
} flexarr;

flexarr *flexarr_init(const size_t nmemb, const size_t inc_r);
void *flexarr_inc(flexarr *f);
void *flexarr_dec(flexarr *f);
void *flexarr_set(flexarr *f, const size_t s);
void *flexarr_clearb(flexarr *f);
void flexarr_free(flexarr *f);

#endif
