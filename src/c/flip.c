/*
  Name:     flip.c
  Purpose:  Generating a sequence of pseudo-random bits.
  Author:   F. A. Saad
  Copyright (C) 2020 Feras A. Saad, All Rights Reserved.

  Released under Apache 2.0; refer to LICENSE.txt
*/

#include <stdlib.h>

#include "flip.h"

static int k = 31;
static int flip_word = 0;
static int flip_pos = 31;

int flip(void){
    if (flip_pos == k) {
        flip_word = rand();
        flip_pos = 0;
    }
    return (flip_word >> flip_pos++) & 1;
}

// Old implementation scans from lower-to-higher order bits,
// but rand() has poor quality on lower order bits.
// int flip(void){
//     static int k = 31;
//     static int flip_word = 0;
//     static int flip_pos = 0;
//     if (flip_pos == 0) {
//         flip_word = rand();
//         flip_pos = k;
//     }
//     return (flip_word >> --flip_pos) & 1;
// }
