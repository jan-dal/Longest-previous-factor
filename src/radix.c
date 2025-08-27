#include "radix.h"
#include "constants.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void print_sorting(int (*values)[TUPLE_SIZE], int *sorting, int n) {
    printf("Sorting len: %d\n", n);
    for (int i = 0; i < n; i++) {
        int idx = sorting[i];
        printf("[%d]: %d %d %d\n", idx, values[idx][0], values[idx][1], values[idx][2]);
    }
}

void print_values(int (*values)[TUPLE_SIZE], int n) {
    printf("Sorting len: %d\n", n);
    for (int i = 0; i < n; i++) {
        int idx = i;
        printf("%d %d %d\n", values[idx][0], values[idx][1], values[idx][2]);
    }
}

/**
* @brief Radix sort for the tuple structure.
*
* @param[in,out] tinfo The tuple info structure. The tuple_sorting is set at the end.
* @param[in] stages The number of loops to perform counting sort starting from the last elements.
*
**/
int *radix_sort(tuple_info *tinfo, int stages) {
    LOG_FUNC(printf, "Performing radix sort\n");
    // int n = tinfo->total_blocks * TUPLE_SIZE > MIN_LEN ? tinfo->total_blocks * TUPLE_SIZE : MIN_LEN;
    int out_len = tinfo->total_blocks;
    int max_val = tinfo->max_val > MIN_LEN ? tinfo->max_val : MIN_LEN;
    max_val++;

    LOG_FUNC(printf, "Max value: %d\n", max_val);
    LOG_FUNC(printf, "Out length: %d\n", out_len);

    int *tmp_sort = NULL;
    int *prev_sorting = calloc(out_len, sizeof(int)); 
    int *sorting = calloc(out_len, sizeof(int));
    int *count = calloc(max_val, sizeof(int));

    counting_sort(tinfo->values, max_val, count, sorting, NULL, out_len, stages-1);
    tmp_sort = prev_sorting;
    prev_sorting = sorting;
    sorting = tmp_sort; // Reuse buffer

    for (int i = stages-2; i >= 0; i--) {
        counting_sort(tinfo->values, max_val, count, sorting, prev_sorting, out_len, i);
        tmp_sort = prev_sorting;
        prev_sorting = sorting;
        sorting = tmp_sort; // Reuse buffer
    }

    LOG_FUNC(printf, "Radix sort complete\n");

    free(sorting);
    free(count);
    return prev_sorting;
}

/**
* @brief Counting sort which performs one sorting iteration for the radix sort.
*
* @param[in] values The tuples to perform the sorting on.
* @param[in] max_val The maximum value in the tuples.
* @param[in] prev_sorting The previous sorting for the tuples (on the old index). ID if this is the first round.
* @param[in] out_len The output array size.
* @param[in] stage Index in the value array.
*
**/
void counting_sort(int (*values)[TUPLE_SIZE], int max_val, int *count, int *sorting, int *prev_sorting, int out_len, int stage) {
    memset(count, 0, max_val * sizeof(int));

    for (int j = 0; j < out_len; j++) {
        count[values[j][stage]]++;
    }

    for (int j = 1; j < max_val; j++) {
        count[j] += count[j-1];
    }

    if (prev_sorting == NULL) {
        for (int j = out_len-1; j >= 0; j--) {
            int tmp = values[j][stage];
            count[tmp]--;
            sorting[count[tmp]] = j; 
        }
    } else {
        for (int j = out_len-1; j >= 0; j--) {
            int tmp = values[prev_sorting[j]][stage];
            count[tmp]--;
            sorting[count[tmp]] = prev_sorting[j]; 
        }
    }
}
