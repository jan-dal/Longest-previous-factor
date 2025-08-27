#include "util.h"
#include "tuple.h"
#include "radix.h"
#include "constants.h"
#include "suffix_array.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <byteswap.h>

// Test cases which were wrongly calculated at first
// abbbaabbbabbb 
// babbaabbabaaaaabbabbababbbaabbaabbaabbbbababaaabaaaabaaababbbbbbabbbaabbbbabbaaaabaabaababaaabbabaabababaabbbbbbaabaaabbbbbaaaabaababbbbaabbaaaaaaababababbbbaababbababbbaabbabbaaaabaaabbbabbbabaaabbbaabbbbaaaabaabaababbbabbbbbbabaabbbabaababbabaaaabbbaabbaababbbabaaaabbbaabbaababbabbbbbbaabbbbabaaabbbbaabaaababbbbaaabaaaabaababbbaaaaabbbaabbbaabbbabbbbbbaabbbabbaababaabbbaaabbbbaaa
// 180, 181, 182, 183, 275, 276

/**
* @brief Compute the suffix array for a given string.
* 
* This algorithm is an implementation of the work of Juha Kärkkäinen and Peter Sanders: 
* https://link.springer.com/chapter/10.1007/3-540-45061-0_73.
* Given w[1...n], a suffix array SA[1...n] holds the indices i for w[i...n] (all suffixes of w)
* in lexicographic order. 
* We can calculate SA in O(n) time using the algorithm described in the above referenced work.
* The assumption is that w is over the alphabet [1, n]. To ensure this works even for small 
* input lengths we pad the input with the '\0' character to the length of 127 to cover the ascii alphabet.
* Additionally we always add '000' to ensure no overflow.
* 
* @param[in] str String over the ascii alphabet for which the suffix array should be calculated.
* 
* @return Returns a pointer to the suffix array.
**/
int *suffix_array(int *str, int str_len) {
    LOG_MESSAGE("Calculating suffix array for:\n"); 
    LOG_FUNC(printf_line, str, str_len);

    // printf("Input string lenght: %d\n", str_len);
    // printf_line(str, str_len);

    tuple_info *tinfo12 = str_to_tuples(str, str_len);
    tinfo12->tuple_sorting = radix_sort(tinfo12, TUPLE_SIZE);
    int *tuple_names = name_tuples(tinfo12);
 
    LOG_FUNC(printf_line, tuple_names, tinfo12->total_blocks);

    LOG_MESSAGE("T12 info before reordering:\n");
    LOG_FUNC(print_tuple_info, tinfo12);
 
    if (tinfo12->max_name != tinfo12->total_blocks) {
        int *sa12 = suffix_array(tuple_names, tinfo12->total_blocks);
        free(tinfo12->tuple_sorting);
        tinfo12->tuple_sorting = sa12;
    }
    free(tuple_names);

    tinfo12->positions = reorder(tinfo12->positions, tinfo12->tuple_sorting, tinfo12->total_blocks);
    tinfo12->tuple_type = reorder(tinfo12->tuple_type, tinfo12->tuple_sorting, tinfo12->total_blocks);

    tuple_info *tinfo0 = create_t0_ordered(tinfo12, str, str_len);
    int *sorting0 = counting_sort(tinfo0->values, NULL, str_len > MIN_LEN ? str_len : MIN_LEN, tinfo0->total_blocks, TUPLE_SIZE-1);
    tinfo0->tuple_sorting = sorting0;
    tinfo0->positions = reorder(tinfo0->positions, tinfo0->tuple_sorting, tinfo0->total_blocks);

    LOG_MESSAGE("T12 info after reordering:\n");
    LOG_FUNC(print_tuple_info, tinfo12);

    LOG_FUNC(print_sa_from_tinfo, str, str_len, tinfo12, "SUFFIX ARRAY FOR SA12");
    LOG_FUNC(print_sa_from_tinfo, str, str_len, tinfo0, "SUFFIX ARRAY FOR SA0");
    
    int *sa = merge(str, str_len, tinfo0, tinfo12);

    LOG_MESSAGE("Calculated suffix array:\n");
    LOG_FUNC(printf_line, sa, str_len); 

    cleanup_tinfo(tinfo0);
    cleanup_tinfo(tinfo12);

    return sa;
}

/**
* @brief Merge the array of suffixes mod 0 and the array of suffixes mod 1, mod 2.
*
* Given the suffix arrays SA0 and SA12 merge them to get the whole suffix array.
* Notice that the algorithm assumes that the positions in tinfo0 and tinfo12
* are already sorted.
*
* @param[in] str The input string.
* @param[in] str_len The length of the input string.
* @param[in] tinfo0 Tuple info mod 3 = 0.
* @param[in] tinfo12 Tuple infor mod 3 = 1,2.
*
* @return Returns the merged suffix array of SA0 and SA12.
**/
int *merge(int *str, int str_len, tuple_info *tinfo0, tuple_info *tinfo12) {
    LOG_MESSAGE("Merging\n");
    int extra_block = str_len % 3 == 1;

    int *sa12r = _reverse_suffix_array(tinfo12->positions, tinfo12->total_blocks,str_len+ADDITIONAL_PADDING);
    // to_bign(sa12r, str_len+ADDITIONAL_PADDING);

    int *str0 = _create_str(str, tinfo0->positions, tinfo0->total_blocks, 0);
    int *str10 = _create_str(str, tinfo0->positions, tinfo0->total_blocks, 1);
    int *str12 = _create_str(str, tinfo12->positions, tinfo12->total_blocks, 0);
    int *str012 = _create_str(str, tinfo12->positions, tinfo12->total_blocks, 1);

    int *sa = malloc(sizeof(int) * str_len);
    int cr, pos_12, pos_0; // Hold the comparison, positions in SA_12/SA_0
    int i0 = 0, i12 = extra_block; // Counts position in sa_12, sa_0
    int k = 0;

    int *m02 = malloc(2 * sizeof(int));
    int *m12 = malloc(2 * sizeof(int));

    int *m03 = malloc(3 * sizeof(int));
    int *m23 = malloc(3 * sizeof(int));

    while(i0 < tinfo0->total_blocks && i12 < tinfo12->total_blocks) {
        LOG_MESSAGE("\n------------------------ i : %d, j : %d\n", i12, i0);

        pos_12 = tinfo12->positions[i12];
        pos_0  = tinfo0->positions[i0];

        LOG_MESSAGE("Comparing: \n");
        LOG_MESSAGE("%d: ", pos_0);
        LOG_FUNC(printf_line, str+pos_0, str_len-pos_0);
        LOG_MESSAGE("%d: ", pos_12);
        LOG_FUNC(printf_line, str+pos_12, str_len-pos_12);
        LOG_MESSAGE("\n");
 
        if (tinfo12->tuple_type[i12] == 1) {
            LOG_MESSAGE("Case 1\n");
            m02[0] = str0[i0];
            m02[1] = sa12r[pos_0+1];

            m12[0] = str12[i12];
            m12[1] = sa12r[pos_12+1]; // We are safe from overflowing because sa_rev has -1 at the end

            LOG_MESSAGE("m0: (%d, %d) vs m12: (%d, %d)\n", m02[0], m02[1], m12[0], m12[1]);
            cr = compare_tuples(m02, m12, 2);
            // cr = memcmp(m02, m12, 2 * sizeof(int));
        } 
        else {
            LOG_MESSAGE("Case 2\n");
            m03[0] = str0[i0];
            m03[1] = str10[i0];
            m03[2] = sa12r[pos_0+2];

            m23[0] = str12[i12];
            m23[1] = str012[i12];
            m23[2] = sa12r[pos_12+2];
            
            LOG_MESSAGE("m0: (%d, %d, %d) vs m12: (%d, %d, %d)\n", m03[0], m03[1], m03[2], m23[0], m23[1], m23[2]);
            cr = compare_tuples(m03, m23, 3);
            // cr = memcmp(m03, m23, 3 * sizeof(int));
        }
        if (cr <= 0) {
            LOG_MESSAGE("%d won\n", pos_0);
            sa[k++] = pos_0;
            i0 += 1;
        } else {
            LOG_MESSAGE("%d won\n", pos_12);
            sa[k++] = pos_12;
            i12 += 1;
        }
    }
 
    while (i12 < tinfo12->total_blocks) {
        sa[k++] = tinfo12->positions[i12++];
    }
    while (i0 < tinfo0->total_blocks) {
        sa[k++] = tinfo0->positions[i0++];
    }

    free(sa12r);

    free(m02);
    free(m12);
    free(m03);
    free(m23);

    free(str0);
    free(str10);
    free(str12);
    free(str012);

    return sa;
}

/**
* @brief Reorder the input array according to a sorted-indices array.
*
* @return Returns a pointer to the sorted array.
**/
int *reorder(int *arr, int *sorting, int len) {
    int *reorder = malloc(len * sizeof(int));
    for (int i = 0; i < len; i++) {
        reorder[i] = arr[sorting[i]];
    }
    free(arr);
    return reorder;
}

/**
* @brief A trick to make merging faster.
*
* Improves spatial locallity and reduces cache misses. It orders
* the values which are needed to create pairs and triplets to
* compare in the merge part of the algorithm.
* The resulting array can be walked index by index instead of 
* random jumps. 
*
* @return Returns a pointer to a sorted array.
**/
int *_create_str(int *str, int *positions, int len, int a) {
    int *str0 = malloc(len * sizeof(int));
    for (int i=0; i < len; i++) {
        str0[i] = str[positions[i] + a];

    }
    return str0;
}

/**
* @brief Create the reverse of a suffix array (with +1 to all entries).
*
* Given suffix array SA create SA^-1 where SA^-1[i] = j+1 if SA[j] = i 
*
* @param[in] positions The positions in suffix array.
* @param[in] pos_len The length of positions. 
* @param[in] out_len Length of the whole suffix_array. (suffix array might be shorter if we create SA^-1 from SA12) 
*
* @return Returns the reversed suffix array + 1.
**/
int *_reverse_suffix_array(int *positions, int pos_len, int out_len) {
    LOG_MESSAGE("Reversing suffix array\n");
    int *sar = calloc(out_len, sizeof(int));
    for (int i = 0; i < pos_len; i++) {
        sar[positions[i]] = i+1;
    }
    LOG_MESSAGE("Reversing successful\n");
    return sar;
}

void print_suffix_array(int *str, int *sa, int len) {
    for (int i = 0; i < len; i++) {
        int pos = sa[i];
        printf("SA[%d] = %d\t", i, pos);
        fwrite(str + pos,  sizeof(int), len - pos,stdout);
        printf("\n");
    }
}

void print_suffix_array_summary(int *str, int *sa, int *lcp, int len) {
    printf("SA:  ");
    printf_array(sa, len);
    printf("LCP: ");
    printf_array(lcp+1, len-1);
    print_suffix_array(str, sa, len);
}
