/*
  Name:     fldr.h
  Purpose:  Fast sampling of random integers.
  Author:   F. A. Saad
  Copyright (C) 2020 Feras A. Saad, All Rights Reserved.

  Released under Apache 2.0; refer to LICENSE.txt
*/

#ifndef FLDR_H
#define FLDR_H

struct fldr_s {
    int n;
    int m;
    int k;
    int r;
    int *h;
    int *H;
};
typedef struct fldr_s fldr_preprocess_t;

int ceil_log2(unsigned long long x);
fldr_preprocess_t * fldr_preprocess(int *a, int n);
int fldr_sample(fldr_preprocess_t *x);
void fldr_free(fldr_preprocess_t *x);

#endif
