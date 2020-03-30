/*
  Name:     example.c
  Purpose:  Example of running FLDR.
  Author:   F. A. Saad
  Copyright (C) 2020 Feras A. Saad, All Rights Reserved.

  Released under Apache 2.0; refer to LICENSE.txt
*/

#include <stdlib.h>
#include <stdio.h>
#include "fldr.h"

int main(int argc, char **argv) {
    int N_sample = 100;
    int * samples = calloc(N_sample, sizeof(int));

    int distribution[5] = { 1, 1, 2, 3, 1 };
    fldr_preprocess_t *x = fldr_preprocess(distribution, 5);
    for (int i = 0; i < N_sample; i++) {
        samples[i] = fldr_sample(x);
        printf("%d ", samples[i]);
    }
    printf("\n");

    free(samples);
    fldr_free(x);
}
