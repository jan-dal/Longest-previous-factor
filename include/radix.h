#include "tuple.h"

#ifndef RADIX_H
#define RADIX_H

int *radix_sort(tuple_info *tinfo, int stages);
void counting_sort(int (*values)[TUPLE_SIZE], int max_val, int *count, int *sorting, int *prev_sorting, int out_len, int stage);

#endif