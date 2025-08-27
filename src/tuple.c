#include "tuple.h"
#include "constants.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
* @brief Assign to the input tuples a new letter from the alphabet.
*
* Assuming that the list of tuples was sorted (using radix sort for example)
* we can easily assign a unique letter to a tuple by incrementing the name
* when we encounter a new tuple, which reduces the alphabet size to the
* range [1, 2/3n]
*
* @param[in] tinfo Information about the tuples (must be sorted)
*
* @return Returns a pointer to an array with new names corresponding to tuples in str.
**/
int *name_tuples(tuple_info *tinfo) {
    int *tuple_names = malloc(sizeof(int) * (tinfo->total_blocks + ADDITIONAL_PADDING));
    int *sorting = tinfo->tuple_sorting; 
    tuple_names[sorting[0]] = 1;

    int name = 1;
    for (int i = 1; i < tinfo->total_blocks; i++) {
        // int cmp = compare_tuples(tinfo->values[sorting[i-1]], tinfo->values[sorting[i]], TUPLE_SIZE);
        int cmp = memcmp(tinfo->values[sorting[i-1]], tinfo->values[sorting[i]], TUPLE_SIZE * sizeof(int));
        if (cmp != 0) {
            name++;
        }
        tuple_names[sorting[i]] = name;
    }
    for (int i = tinfo->total_blocks; i < tinfo->total_blocks + ADDITIONAL_PADDING; i++) {
        tuple_names[i] = 0;
    }
    tinfo->max_name = name;
    return tuple_names;
}

/**
* @brief Creates the tuple info structure for str.
*
* The structure hold information about every group of 3 
* consecutive letters which begin on positions i where
* i mod 3 = 1 or i mod 3 = 2. That means we take roughly
* 2/3 of all letters from the input string. The structure
* later is extended to hold information about the sorting
* and naming of the tuples. 
*
* @param[in] str The input string.
* @param[in] str_len String length.
*
**/
tuple_info *str_to_tuples(int *str, int str_len) {
    LOG_MESSAGE("Creating new tuples\n");
    tuple_info *tinfo = malloc(sizeof(tuple_info));
    int extra_block = str_len % 3 == 1;
    tinfo->total_blocks = (str_len - (str_len + 2)/3) + extra_block;

    tinfo->positions = malloc(tinfo->total_blocks * sizeof(int));   
    tinfo->tuple_type = malloc(tinfo->total_blocks * sizeof(int));
    tinfo->values = malloc(tinfo->total_blocks * sizeof(*tinfo->values));
    tinfo->tuple_sorting = NULL;

    int k = 0, i = 1, max_val = 0, val;
    while(i < str_len + extra_block) {
        tinfo->positions[k] = i;
        tinfo->tuple_type[k] = 1;
        for (int q = 0; q < TUPLE_SIZE; q++) {
            val = str[i+q];
            tinfo->values[k][q] = str[i+q];
            if (val > max_val) {
                max_val = val;
            }
        }
        k++;
        i += 3;
    }

    i = 2;
    while(i < str_len) {
        tinfo->positions[k] = i;
        tinfo->tuple_type[k] = 2;
        for (int q = 0; q < TUPLE_SIZE; q++) {
            val = str[i+q];
            tinfo->values[k][q] = val;
            if (val > max_val) {
                max_val = val;
            }
        }
        k++;
        i += 3;
    }
    tinfo->max_val = max_val;
    
    LOG_MESSAGE("Allocated %d tuples.\n", tinfo->total_blocks);
    return tinfo;
}

/**
* @brief Creates the tuple info structure for str on positions divisible by 3.
*
* After the algorithm created SA12 (suffix array for entries
* on positions mod 3 = 1,2) it proceeds to do the same for 
* entries on positions mod 3 = 0. It uses the gathered information
* from SA12 that is: If we add a letter at the beginning
* of each sorted suffix which starts on positions mod 3 = 1
* (we create suffixes mod 3 = 0) then to sort the new words we
* just need to sort according to the added letters which 
* requires just one pass of the radix algorithm. 
*
* @param[in] tinfo12 Should hold information about SA12 to this point
* @param[in] str The input string.
* @param[in] str_len The input string length.
*
**/
tuple_info *create_t0_ordered(tuple_info *tinfo12, int *str, int str_len) {
    LOG_MESSAGE("Creating t0\n");
    tuple_info *tinfo0 = malloc(sizeof(tuple_info));

    tinfo0->total_blocks = (str_len+2)/3;
    tinfo0->positions = malloc(tinfo0->total_blocks * sizeof(int));
    tinfo0->values = malloc(tinfo0->total_blocks * sizeof(*tinfo0->values));

    tinfo0->tuple_type = NULL;
    tinfo0->tuple_sorting = NULL;

    int max_val = 0, val;
    int k = 0;
    for (int i = 0; i < tinfo12->total_blocks; i++) {
        int pos = tinfo12->positions[i];
        if (tinfo12->tuple_type[i] == 1) {
            tinfo0->positions[k] = pos - 1;
            val = str[pos - 1];
            tinfo0->values[k++][TUPLE_SIZE-1] = val;
            if (val > max_val) {
                max_val = val;
            }
        }
    }
    tinfo0->max_val = max_val;
    
    LOG_MESSAGE("t0 created\n");
    return tinfo0;
}

/**
* @brief Compare 2 tuples of a given length.
*
* @param[in] t1 Tuple 1.
* @param[in] t2 Tuple 2.
* @param[in] len Length of the tuples.
*
* @return Returns k where k <= 0 if t1 <= t2 else k > 0.
**/
int compare_tuples(int *t1, int *t2, int len) {
    for (int i = 0; i < len; i++) {
        if (t1[i] != t2[i]) {
            return t1[i] - t2[i];
        }
    }
    return 0;
}

void cleanup_tinfo(tuple_info *tinfo) {
    free(tinfo->values);
    free(tinfo->positions);
    free(tinfo->tuple_type);
    free(tinfo->tuple_sorting);
    free(tinfo);
}
