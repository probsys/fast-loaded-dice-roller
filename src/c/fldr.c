/*
  Name:     fldr.c
  Purpose:  Fast sampling of random integers.
  Author:   F. A. Saad
  Copyright (C) 2020 Feras A. Saad, All Rights Reserved.

  Released under Apache 2.0; refer to LICENSE.txt
*/

#include <stdbool.h>
#include <stdlib.h>

#include "fldr.h"
#include "flip.h"

int ceil_log2(unsigned long long x) {
  static const unsigned long long t[6] = {
    0xFFFFFFFF00000000ull,
    0x00000000FFFF0000ull,
    0x000000000000FF00ull,
    0x00000000000000F0ull,
    0x000000000000000Cull,
    0x0000000000000002ull
  };

  int y = (((x & (x - 1)) == 0) ? 0 : 1);
  int j = 32;
  int i;

  for (i = 0; i < 6; i++) {
    int k = (((x & t[i]) == 0) ? 0 : j);
    y += k;
    x >>= k;
    j >>= 1;
  }

  return y;
}

fldr_preprocess_t * fldr_preprocess(int *a, int n) {
    int m = 0;
    for (int i = 0; i < n; i++) {
        m += a[i];
    }
    int k = ceil_log2(m);
    int r = (1 << k) - m;

    int *h = calloc(k, sizeof(*h));
    int *H = calloc((n+1)*k, sizeof(*H));

    int d;
    for(int j = 0; j < k; j++) {
        d = 0;
        for (int i = 0 ; i < n; i++) {
            H[i*k + j] = -1;
            bool w = (a[i] >> ((k-1) -j)) & 1;
            h[j] += w;
            if (w) {
                H[d*k + j] = i;
                d += 1;
            }
        }
        H[n*k + j] = -1;
        bool w = (r >> ((k-1) - j)) & 1;
        h[j] += w;
        if (w) {
            H[d*k + j] = n;
            d += 1;
        }
    }

    fldr_preprocess_t *x = malloc(sizeof(*x));
    x->n = n;
    x->m = m;
    x->k = k;
    x->r = r;
    x->h = h;
    x->H = H;
    return x;
}

int fldr_sample(fldr_preprocess_t *x) {
    if (x->n == 1) {
        return 0;
    }
    int n = x->n;
    int k = x->k;
    int *H = x->H;
    int *h = x->h;
    int c = 0;
    int d = 0;
    while (true) {
        int b = flip();
        d = 2*d + (1 - b);
        if (d < h[c]) {
            int z = H[d*k + c];
            if (z < n) {
                return z;
            } else {
                d = 0;
                c = 0;
            }
        } else {
            d = d - h[c];
            c = c + 1;
        }
    }
}

void fldr_free(fldr_preprocess_t *x) {
    free(x->h);
    free(x->H);
    free(x);
}
