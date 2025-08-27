#include "constants.h"

#ifndef BENCHMARK_H
#define BENCHMARK_H

void benchmark_runner(Options config);
void benchmark(int *(*f)(int*, int), data_frame *data, Options config, int datapoint);
long long timeit(int *(*f)(int*, int), int *str, int str_len);
int *random_str(int *str, int str_len, int asize);
void validate_suffix_array(int str_len, int tries, int asize);
void validate_lpf(int str_len, int tries, int asize);

#endif