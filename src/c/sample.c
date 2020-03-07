/*
  Name:     sample.c
  Purpose:  Command line interface for FLDR.
  Author:   F. A. Saad
  Copyright (C) 2020 Feras A. Saad, All Rights Reserved.

  Released under Apache 2.0; refer to LICENSE.txt
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fldr.h"

int * load_array(FILE *fp, int length) {
    int *a = (int *) calloc(length, sizeof(int));
    for (int i = 0; i < length; i++) {
        fscanf(fp, "%d", &a[i]);
    }
    return a;
}

void write_fldr_s(char *fname, struct fldr_s *x) {
    char *fname_s = strcat(fname, ".fldr");
    FILE *fp = fopen(fname_s, "w");

    fprintf(fp, "%d %d %d %d", x->n, x->m, x->k, x->r);
    fprintf(fp, "\n");

    for (int j = 0; j < x->k; j++) {
        if (j < x->k - 1) {
            fprintf(fp, "%d ", x->h[j]);
        } else {
            fprintf(fp, "%d", x->h[j]);
        }
    }
    fprintf(fp, "\n");

    for (int row = 0; row < x->n + 1; row++){
        for (int col = 0; col < x->k; col++) {
            if (col < x->k - 1) {
                fprintf(fp, "%d ", x->H[row*x->k + col]);
            } else {
                fprintf(fp, "%d", x->H[row*x->k + col]);
            }
        }
        if (row < x->n) {
            fprintf(fp, "\n");
        }
    }
    fprintf(fp, "\n");

    fclose(fp);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("usage: %s N path\n", argv[0]);
        exit(0);
    }
    int N = atoi(argv[1]);
    char *path = argv[2];

    // Load the target distribution.
    FILE *fp = fopen(path, "r");
    int n;
    fscanf(fp, "%d", &n);
    int *a = load_array(fp, n);
    fclose(fp);

    // Obtain the samples.
    int * samples = (int *) calloc(N, sizeof(int));
    struct fldr_s x = fldr_preprocess(a, n);
    for (int i = 0; i < N; i++) {
        samples[i] = fldr_sample(&x);
    }

    // Print the samples.
    for (int i = 0; i < N; i ++) {
        printf("%d ", samples[i]);
    }
    printf("\n");

    // Write the structure.
    if (argc == 4) {
        write_fldr_s(path, &x);
    }

    // Free the heap.
    free(a);
    free(samples);
    fldr_free(&x);

    return 0;
}
