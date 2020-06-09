/*
  Name:     fldr.h
  Purpose:  Fast sampling of random integers (floating-point weights).
  Author:   F. A. Saad
  Copyright (C) 2020 Feras A. Saad, All Rights Reserved.

  Released under Apache 2.0; refer to LICENSE.txt
*/

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fldrf.h"
#include "flip.h"

#define DBL_MAX_WIDTH (int)ceil(log2(DBL_MAX))

fldrf_preprocess_t * fldrf_preprocess(double *a, int n) {

    struct double_s *ratios[n];
    for (int i = 0 ; i < n; i++) {
        ratios[i] = as_integer_ratio(a[i]);
    }

    normalize_double_s(ratios, n);

    struct array_s mantissas[n];
    for (int i = 0; i < n; i++) {
        mantissas[i] = align_mantissa(ratios[i]);
    }

    struct array_s m = binary_sum(mantissas, n);
    int k;
    struct array_s r = compute_reject_bits(m, &k);

    int *h = calloc(k, sizeof(*h));
    int *H = calloc((n+1)*k, sizeof(*H));

    int d;
    for(int j = 0; j < k; j++) {
        d = 0;
        for (int i = 0 ; i < n; i++) {
            H[i*k + j] = -1;
            int idx = j - (k - mantissas[i].length);
            bool w = (0 <= idx) ? mantissas[i].items[idx] : 0;
            h[j] += w;
            if (w) {
                H[d*k + j] = i;
                d += 1;
            }
        }
        H[n*k + j] = -1;
        int idx = j - (k - r.length);
        bool w = (0 <= idx) ? r.items[idx] : 0;
        h[j] += w;
        if (w) {
            H[d*k + j] = n;
            d += 1;
        }
    }

    for (int i = 0; i < n; i++) {
        array_s_free(mantissas[i]);
        double_s_free(ratios[i]);
    }

    fldrf_preprocess_t *x = malloc(sizeof(*x));
    x->n = n;
    x->m = m;
    x->k = k;
    x->r = r;
    x->h = h;
    x->H = H;
    return x;
}

void normalize_double_s(struct double_s *d[], int n) {
    int max_exponent = d[0]->exponent;
    for (int i = 1; i < n; i++) {
        if (max_exponent < d[i]->exponent) {
            max_exponent = d[i]->exponent;
        }
    }
    for (int i = 0; i < n; i++) {
        int offset = max_exponent - d[i]->exponent;
        d[i]->offset += offset;
        d[i]->width += offset;
        d[i]->exponent = max_exponent;
    }
}

struct double_s * as_integer_ratio(double x) {
    int exponent;
    double m = frexp(x, &exponent);

    int i = 0;
    while (m != floor(m)) {
        assert(i < DBL_MAX_WIDTH);
        m *= 2.;
        exponent -= 1;
        i++;
    }

    int width;
    struct array_s mantissa = decimal_to_binary(m, &width);

    int offset;
    if (0 < exponent) {
        offset = exponent;
        exponent = 0;
    } else {
        offset = 0;
        exponent = abs(exponent);
    }

    struct double_s *d = malloc(sizeof(*d));
    d->mantissa = mantissa;
    d->width = width;
    d->offset = offset;
    d->exponent = exponent;
    return d;
}

struct array_s decimal_to_binary(double x, int *width) {
    assert(x == floor(x));

    struct array_s bits = array_s_alloc(DBL_MAX_WIDTH);

    int w = 0;
    while (0 < x) {
        bits.items[w++] = fmod(x, 2);
        x = floor(x/2);
    }

    *width = w;
    return bits;
}

struct array_s align_mantissa(struct double_s *d) {
    struct array_s mantissa = array_s_alloc(d->width + d->offset);

    int start = d->width - 1;
    for (int i = 0; i < d->width; i++) {
        mantissa.items[start-i] = d->mantissa.items[i];
    }

    return mantissa;
}

struct array_s compute_reject_bits(struct array_s m, int *k) {

    struct array_s r;
    if ((m.items[0] == 1) && (array_sum(m) == 1)) {
        *k = m.length - 1;
        r.length = 0;
        r.items = NULL;
    } else {
        *k = m.length;
        struct array_s pow_2k = array_s_alloc(*k + 1);
        pow_2k.items[0] = 1;
        r = binary_sub(pow_2k, m);
        array_s_free(pow_2k);
        assert(r.length <= *k);
    }

    return r;
}

struct array_s binary_sum(struct array_s arrays[], int n) {
    if (n == 1) {
        return arrays[0];
    }

    struct array_s m;
    struct array_s m_prev;
    m = binary_add(arrays[0], arrays[1]);
    m_prev = m;
    for (int i = 2; i < n; i ++) {
        m = binary_add(m_prev, arrays[i]);
        array_s_free(m_prev);
        m_prev = m;
    }

    return m;
}

struct array_s binary_add(struct array_s a, struct array_s b) {
    int la = a.length;
    int lb = b.length;
    int l = lb < la ? la : lb;
    int c = 0;
    int length = l+1;
    struct array_s y = array_s_alloc(length);

    for (int i = 1; i < l + 1; i++) {
        int ai = (0 <= la - i) ? a.items[la - i] : 0;
        int bi = (0 <= lb - i) ? b.items[lb - i] : 0;
        y.items[l+1-i] = ((ai ^ bi) ^ c);
        c = (ai & bi) | (ai & c) | (bi & c);
    }
    if (c == 1) {
       y.items[0] = 1;
    }

    remove_leading_zeros(&y);
    return y;
}

struct array_s binary_sub(struct array_s a, struct array_s b){
    int la = a.length;
    int lb = b.length;
    int l = lb < la ? la : lb;
    int c = 0;
    int length = l+1;
    struct array_s y = array_s_alloc(length);

    for (int i = 1; i < l + 1; i++) {
        int ai = (0 <= la - i) ? a.items[la - i] : 0;
        int bi = (0 <= lb - i) ? b.items[lb - i] : 0;
        y.items[l+1-i] = ((ai ^ bi) ^ c);
        c = c & (!(ai ^ bi)) | (!ai & bi);
    }
    if (c == 1) {
       y.items[0] = 1;
    }

    remove_leading_zeros(&y);
    return y;
}

void remove_leading_zeros(struct array_s *a) {
    int j = 0;
    while ((j < a->length) && (a->items[j] == 0)) {
        j++;
    }

    if (j == 0) {
        ;
    } else if (j == a->length) {
        a->length = 0;
        free(a->items);
        a->items = NULL;
    }
    else {
        int length = a->length - j;
        memmove(a->items, a->items + j, length * sizeof(*a->items));
        a->items = realloc(a->items, length * sizeof(*a->items));
        a->length = length;
    }
}

int array_sum(struct array_s a) {
    int x = 0;
    for (int i = 0; i < a.length; i++) {
        x += a.items[i];
    }
    return x;
}

int fldrf_sample(fldrf_preprocess_t *x) {
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

struct array_s array_s_alloc(int length) {
   struct array_s a;
   a.length = length;
   a.items = calloc(length, sizeof(*a.items));
   return a;
}

void array_s_free(struct array_s x) {
    free(x.items);
}

void double_s_free(struct double_s *d) {
    array_s_free(d->mantissa);
    free(d);
}

void fldrf_free(fldrf_preprocess_t *x) {
    array_s_free(x->m);
    array_s_free(x->r);
    free(x->h);
    free(x->H);
    free(x);
}
