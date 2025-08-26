#ifndef SUFFIX_ARRAY_QSORT_H
#define SUFFIX_ARRAY_QSORT_H

int compare(const void *a, const void *b);
int *suffix_array_qsort(int *str, int str_len);

#endif