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
