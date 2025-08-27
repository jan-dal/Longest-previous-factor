#include "lpf.h"
#include "lcp.h"
#include "util.h"
#include "tuple.h"
#include "constants.h"
#include "suffix_array.h"

#include <stdlib.h>
#include <stdio.h>

/**
* @brief Create a helper table for the LPF algorithm.
*
* The table consists of entries which hold information about
* the previous entry and the next entry similar to a double
* linked list. Its purpose is to track logically removed
* entries and updated entries from the SA and LCP array.
*
* Each binode entry holds the LCP value with its closest previous
* and next neighbours which were not yet removed.
*
* Example:
* If we remove entry SA[i] 0 < i < len(SA) then the value
* of LCP[i+1] should now reflect the LCP of SA[i-1] and SA[i+1].
*
*   
* @param[in] lcp The LCP array.
* @param[in] len Length of the LCP array.
*
* @return Returns the LCP tracking/helper array.
**/
binode *create_adjacent(int *lcp, int len) {
    binode *adj = malloc(len * sizeof(binode));

    adj[0].prev = -1;
    adj[0].prev_val = 0;
    adj[0].next = 1;
    adj[0].next_val = lcp[1];

    adj[len-1].prev = len-2;
    adj[len-1].prev_val = lcp[len-1];
    adj[len-1].next = -1;
    adj[len-1].next_val = 0;

    for (int i = 1; i < len-1; i++) {
        adj[i].prev = i-1;
        adj[i].prev_val = lcp[i];
        adj[i].next = i+1;
        adj[i].next_val = lcp[i+1];
    }

    return adj;
}

/**
* @brief Create the longest previous substring table.
*
* Definition: 
* Given a string w[1...n], LPF[i] = k such that w[i...i+k-1] 
* occurs somewhere before i that is there exists j where
* w[i...i+k-1] = w[j...j+k-1] and j < i.

* The algorithm uses the observation that if we have SA and LCP
* for str[1...n] then LPF[n] = LCP[n]. If we then remove this
* entry (SA[SA^(-1)[n]]) from the table then we can calculate n-1 
* and so on.
*
* Each time the LCP table needs to be updated which is a constant
* operation. LCP of SA[i-1] and SA[i+1] is:
* min(LCP[i], LCP[i+1])
*   
* @param[in] str Input text.
* @param[in] str_len Length of text.
*
* @return Returns the LPF array.
**/
int *lpf_array(int *str, int str_len) {
    int *sa = suffix_array(str, str_len);
    int *sar = reverse_array(sa, str_len);
    int *lcp = lcp_array(str, sa, sar,str_len);  
    int *lpf = calloc(str_len, sizeof(int));
    binode *adj = create_adjacent(lcp, str_len);

    LOG_FUNC(print_suffix_array, str, sa, str_len);

    LOG_FUNC(printf_line, sa, str_len);
    LOG_FUNC(printf_line, sar, str_len);
    LOG_FUNC(printf_line, lcp, str_len);

    // print_suffix_array(str, sa, str_len);
    // printf_array(lcp, str_len);

    int index, prev, next, prev_val, next_val;

    for (int i = str_len-1; i > 0; i--) {
        index = sar[i];
        prev = adj[index].prev;
        next = adj[index].next;
        prev_val = adj[index].prev_val;
        next_val = adj[index].next_val;

        // printf("Index: %d PREV: %d %d, NEXT: %d %d\n", index, prev, prev_val, next, next_val);

        if (prev_val < next_val) {
            lpf[i] = next_val;
        } else {
            lpf[i] = prev_val;
        }

        if (next >= 0) {
            adj[next].prev = prev;
            adj[next].prev_val = next_val > prev_val ? prev_val : next_val;
        }
        if (prev >= 0) {
            adj[prev].next = next;
            adj[prev].next_val = next > 0 ? adj[next].prev_val : 0;
        }
    }
    free(sa);
    free(sar);
    free(lcp);
    free(adj);

    return lpf;
}

// Used to validate the lpf algorithm
int *lpf_array_naive(int *str, int str_len) {
    int *lpf = calloc(str_len, sizeof(int));
    int max, pmax;

    for (int i = 0; i < str_len; i++) {
        max = 0;
        for (int j = i-1; j >= 0; j--) {
            pmax = 0;
            for (int len = 0; len < str_len - i; len++) {
                if (str[i+len] == str[j+len]) {
                    pmax++;
                } else {
                    break;
                }
            }
            max = max < pmax ? pmax : max;
        }
        lpf[i] = max;
    }

    return lpf;
}

void print_lpf_array(int *str, int *lpf, int len) {
    int j, k; 

    for (int i = 0; i < len; i++) {
        k = lpf[i];
        printf("LPF[%d] = %d\t", i, lpf[i]);
        
        if (k > 0) {
            printf_line(str, i, " ");
            printf_line(str+i, k, " ");
            printf_line(str+i+k, len - i - k, "\t");

            j = find_prev(str, i, k);
        
            printf_line(str, j, " ");
            printf_line(str+j, k, " ");
            printf_line(str+j+k, len - j - k, "\n");
        } else {
            printf_line(str, i, " ");
            printf_line(str+i, 1, " ");
            printf_line(str+i+1, len - i - 1, "\n");
        }
    }
}

void print_lpf_array_summary(int *str, int *lpf, int len) {
    printf("LPF = ");
    printf_array(lpf, len);
    print_lpf_array(str, lpf, len);
}