/*
  Name:     fldr.h
  Purpose:  Fast sampling of random integers (floating-point weights).
  Author:   F. A. Saad
  Copyright (C) 2020 Feras A. Saad, All Rights Reserved.

  Released under Apache 2.0; refer to LICENSE.txt
*/

#ifndef FLDRF_H
#define FLDRF_H

struct array_s {
    int length;
    int *items;
};

struct double_s {
    struct array_s mantissa;
    int width;
    int offset;
    int exponent;
};

struct fldrf_s {
    int n;
    struct array_s m;
    int k;
    struct array_s r;
    int *h;
    int *H;
};
typedef struct fldrf_s fldrf_preprocess_t;

fldrf_preprocess_t * fldrf_preprocess(double *a, int n);

void normalize_double_s(struct double_s *d[], int n);
struct double_s * as_integer_ratio(double x);
struct array_s decimal_to_binary(double x, int *width);
struct array_s align_mantissa(struct double_s *m);

struct array_s array_s_alloc(int length);
struct array_s compute_reject_bits(struct array_s m, int *k);
struct array_s binary_sum(struct array_s arrays[], int n);
struct array_s binary_add(struct array_s a, struct array_s b);
struct array_s binary_sub(struct array_s a, struct array_s b);
void remove_leading_zeros(struct array_s *a);
int array_sum(struct array_s a);

int fldrf_sample(fldrf_preprocess_t *x);

void fldrf_free(fldrf_preprocess_t *x);
void array_s_free(struct array_s x);
void double_s_free(struct double_s *d);

#endif
