#include "constants.h"
#include "tuple.h"
#include <stdio.h>

#ifndef UTIL_H
#define UTIL_H

void print_sa_from_tinfo(int *str, int str_len, tuple_info *tinfo, char *title);
void printf_line(int *str, int str_len, char *delim);
void print_lpf_array(int *str, int *lpf, int len);
void print_tuple_info(tuple_info *tinfo);
void printf_array(int *str, int str_len);
void write_to_csv(DataFrame *data, FILE *file);
void cleanup_data(DataFrame *data);
int *random_str(int *str, int str_len, int asize);
int *fib_str(int *str, int n);
int find_prev(int *str, int i, int k);
int read_array_from_stdin(int **str, int no_output);
int fibonacci(int n);
DataFrame *create_data_frame(int datapoints, char *filename, char *header);

#endif