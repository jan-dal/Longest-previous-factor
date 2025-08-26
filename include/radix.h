#include "constants.h"

#ifndef RADIX_H
#define RADIX_H

int *radix_sort(tuple_info *tinfo, int stages);
int *counting_sort(int (*values)[TUPLE_SIZE], int *prev_sorting, int n, int out_len, int stage);

#endif