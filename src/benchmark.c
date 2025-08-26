#include "lpf.h"
#include "util.h"
#include "tuple.h"
#include "benchmark.h"
#include "constants.h"
#include "suffix_array.h"
#include "suffix_array_qsort.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

void validate_lpf(int str_len, int tries, int asize) {
    printf("Validating LPF arrays %d times with random strings [1...%d], |Σ| = %d\n", tries, str_len, asize); 

    int *str = malloc((str_len + ADDITIONAL_PADDING) * sizeof(int));
   
    for (int i = 0; i < tries; i++) {
        random_str(str, str_len, asize);
        int *lpf1 = lpf_array(str, str_len);
        int *lpf2 = lpf_array_naive(str, str_len);

        if (memcmp(lpf1, lpf2, str_len * sizeof(int)) != 0) {
            printf("Mismatch found on try %d\n", i + 1);
            printf("Input string:\n");
            printf_array(str, str_len);
            print_lpf_array(str, lpf1, str_len);
            printf("\n");
            print_lpf_array(str, lpf2, str_len);
            
            free(str);            
            free(lpf1);
            free(lpf2);        
            return;
        }
        free(lpf1);
        free(lpf2);
    }
    free(str);
    
    printf("All %d tries succeeded!\n", tries);
}

void validate_suffix_array(int str_len, int tries, int asize) {
    printf("Validating suffix arrays %d times with random strings[1...%d], |∑| = %d\n", tries, str_len, asize); 
    int *sa1, *sa2;
    int *str = malloc((str_len + ADDITIONAL_PADDING) * sizeof(int));

    for (int i = 0; i < tries; i++) {
        str = random_str(str, str_len, asize);
        sa1 = suffix_array(str, str_len);
        sa2 = suffix_array_qsort(str, str_len);
        int bug = 0;
        for (int k = 0; k < str_len; k++) {
            if (sa1[k] != sa2[k]) {
                printf("BUG: Suffix array differ! %d\n", k);
                fwrite(str + sa1[k],  sizeof(int), str_len - sa1[k],stdout);
                printf("\n");
                fwrite(str + sa2[k],  sizeof(int), str_len - sa2[k],stdout);
                printf("\n");
                bug = 1; 
            }
        }
        if (bug) {
            printf_array(str, str_len);
        // print_suffix_array(str, sa1, str_len);
        // printf("\n");
        // print_suffix_array(str, sa2, str_len);
            free(sa1);
            free(sa2);
            free(str);   
            return;
        }
        free(sa1);
        free(sa2);
    }
    free(str);   
    printf("SUCCESS!\n");
}

void benchmark_runner(Algorithm alg, StrType str_type, int str_len, int tries, int asize) {
    int *(*f)(int*, int);
    int *(*f_naive)(int*, int);
    FILE *file;
    char *filename, *header = TIME_BENCHMARK_HEADER;

    switch (alg) {
        case SUFFIX_ARRAY:
            printf("Running benchmark on suffix arrays %d times with random strings[1...%d], |∑| = %d\n", tries, str_len, asize); 
            f = suffix_array, f_naive = suffix_array_qsort; 
            filename = SA_BENCH_FILENAME;
            break;
        case LCP:
            printf("Running benchmark on lcp arrays %d times with random strings[1...%d], |∑| = %d\n", tries, str_len, asize); 
            return;
        case LPF:
            printf("Running benchmark on lpf arrays %d times with random strings[1...%d], |∑| = %d\n", tries, str_len, asize); 
            f = lpf_array, f_naive = lpf_array_naive;
            filename = LPF_BENCH_FILENAME;
            break;
        default:
            return; 
    }

    file = fopen(filename, "a");

    if (file == NULL) {
        perror("Error opening file");
    }

    int datapoints = 28;
    data_frame *data = create_data_frame(datapoints, filename, header);

    // for (int i = 0; i < datapoints; i++) {
    //     // str_len = (2 << i) + (2 << (i-1));
    //     // str_len = (2 << i);
    //     str_len = i+2;
    //     printf("String length: %d\n", str_len);
    //     benchmark(f, f_naive, data, str_type, str_len, tries, asize, i);
    //     printf("\n");
    // }

    if (str_type == FIBONACCI) {
        benchmark(f, f_naive, data, str_type,  str_len, tries, asize, 0);
    } else {
        benchmark(f, f_naive, data, str_type, str_len, tries, asize, 0);
    }



    // write_to_csv(data, file);
    cleanup_data(data);
    fclose(file); 
}

void benchmark(int *(*f)(int*, int), int *(*f_naive)(int*, int), data_frame *data, StrType str_type, int str_len, int tries, int asize, int datapoint) {
    double ssa = 0, ssaq = 0;
    long long nssa = 0, nssaq = 0;
    int *str = NULL;
    int n = 0;

    for (int i = 0; i < tries; i++) {
        switch (str_type) {
            case RANDOM:
                str = random_str(str, str_len, asize);
                n = str_len;
                break;
            case FIBONACCI:
                str = fib_str(str, str_len);
                n = fibonacci(str_len);
                asize = 2;
                break;
            default:
                str = random_str(str, str_len, asize);
                n = str_len;
                break;
        }

        ssa += timeit(f, str, n);
        // ssaq += timeit(f_naive, str, n);
    }
    free(str);

    ssa /= tries;
    ssaq /= tries;
    nssa = SEC_TO_NANO(ssa);
    nssaq = SEC_TO_NANO(ssaq);

    data->data[datapoint][0] = n;
    data->data[datapoint][1] = str_type;
    data->data[datapoint][2] = tries;
    data->data[datapoint][3] = asize;
    data->data[datapoint][4] = nssa;
    data->data[datapoint][5] = nssaq;
    
    printf("Alg1: %lld ns per call (%f seconds), total: %f seconds\n", nssa, ssa, ssa * tries);
    printf("Alg2: %lld ns per call (%f seconds), total: %f seconds\n", nssaq, ssaq, ssaq * tries);
}

double timeit(int *(*f)(int*, int), int *str, int str_len) {
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);
    int *arr = f(str, str_len);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    free(arr);
    return seconds;
}
