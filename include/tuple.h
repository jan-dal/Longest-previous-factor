#include "constants.h"

#ifndef TUPLE_H
#define TUPLE_H

int *name_tuples(tuple_info *tinfo);
void cleanup_tinfo(tuple_info *tinfo);
int compare_tuples(int *t1, int *t2, int len);
tuple_info *str_to_tuples(int *str, int str_len);
tuple_info *create_t0(tuple_info *tinfo12, int *str, int str_len);
tuple_info *create_t0_ordered(tuple_info *tinfo12, int *str, int str_len);

#endif