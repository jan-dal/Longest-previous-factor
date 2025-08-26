#include "constants.h"

#ifndef SUFFIX_ARRAY_H
#define SUFFIX_ARRAY_H

int *suffix_array(int *str, int str_len);
int *_reverse_suffix_array(int *positions, int pos_len, int out_len) ;

int *merge(int *str, int str_len, tuple_info *tinfo0, tuple_info *tinfo12);
int *reorder(int *arr, int *sorting, int len);
int *_create_str(int *str, int *positions, int len, int a);

void print_suffix(int *str, int str_len, int pos);
void print_suffix_array(int *str, int *sa, int len);

#endif