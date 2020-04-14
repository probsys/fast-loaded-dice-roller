/*
  Name:     example.c
  Purpose:  Example of running FLDR (floating-point weights).
  Author:   F. A. Saad
  Copyright (C) 2020 Feras A. Saad, All Rights Reserved.

  Released under Apache 2.0; refer to LICENSE.txt
*/

#include <stdlib.h>
#include <stdio.h>
#include "fldrf.h"

int main(int argc, char **argv) {
    int N_sample = 100;
    int *samples = calloc(N_sample, sizeof(*samples));

    double distribution[5] = { 1./4, 0.13, 1.12 };
    fldrf_preprocess_t *x = fldrf_preprocess(distribution, 5);
    for (int i = 0; i < N_sample; i++) {
        samples[i] = fldrf_sample(x);
        printf("%d ", samples[i]);
    }
    printf("\n");

    free(samples);
    fldrf_free(x);
}
