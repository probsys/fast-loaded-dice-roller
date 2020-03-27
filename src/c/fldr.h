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

int ceil_log2(unsigned long long x);
struct fldr_s fldr_preprocess(int *a, int n);
int fldr_sample(struct fldr_s *x);
void fldr_free(struct fldr_s *x);

#endif
