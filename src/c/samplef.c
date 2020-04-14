/*
  Name:     sample.c
  Purpose:  Command line interface for FLDR (floating-point weights).
  Author:   F. A. Saad
  Copyright (C) 2020 Feras A. Saad, All Rights Reserved.

  Released under Apache 2.0; refer to LICENSE.txt
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fldrf.h"
#include "flip.h"

double * load_array(FILE *fp, int length) {
    double *a = calloc(length, sizeof(*a));
    for (int i = 0; i < length; i++) {
        assert(fscanf(fp, "%lf", &a[i]) == 1);
    }
    return a;
}

void * write_array(FILE *fp, int *x, int n) {
    for (int j = 0; j < n; j++) {
        if (j < n - 1) {
            fprintf(fp, "%d ", x[j]);
        } else {
            fprintf(fp, "%d", x[j]);
        }
    }
    fprintf(fp, "\n");
}

void write_fldrf_s(char *fname, fldrf_preprocess_t *x) {
    char *fname_s = strcat(fname, ".fldrf");
    FILE *fp = fopen(fname_s, "w");

    fprintf(fp, "%d %d", x->n, x->k);
    fprintf(fp, "\n");

    write_array(fp, x->m.items, x->m.length);
    write_array(fp, x->r.items, x->r.length);
    write_array(fp, x->h, x->k);

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
    assert(fscanf(fp, "%d", &n) == 1);
    double *a = load_array(fp, n);
    fclose(fp);

    // Obtain the samples.
    int *samples = calloc(N, sizeof(*samples));
    fldrf_preprocess_t *x = fldrf_preprocess(a, n);

    for (int i = 0; i < N; i++) {
        samples[i] = fldrf_sample(x);
    }

    // Print the samples.
    for (int i = 0; i < N; i ++) {
        printf("%d ", samples[i]);
    }
    printf("\n");

    // Write the structure.
    if (argc == 4) {
        write_fldrf_s(path, x);
    }

    // Free the heap.
    free(a);
    free(samples);
    fldrf_free(x);

    return 0;
}
