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

void benchmark_runner(Options config) {
    int *(*f)(int*, int);
    FILE *file;
    char *filename, *header = TIME_BENCHMARK_HEADER;

    switch (config.alg) {
        case SUFFIX_ARRAY:
            printf("Running benchmark on suffix arrays %d times with random strings[1...%d], |∑| = %d\n", 
                config.tries, config.size, config.asize); 
            filename = SA_BENCH_FILENAME;

            if (config.alg_impl == FAST) {
                f = suffix_array;
            } else if (config.alg_impl == QSORT) {
                f = suffix_array_qsort;
            } else {
                f = suffix_array;
            }
            break;
        case LPF:
            filename = LPF_BENCH_FILENAME;

            if (config.alg_impl == FAST) {
                f = lpf_array;
            } else if (config.alg_impl == NAIVE) {
                f = lpf_array_naive;
            } else {
                f = lpf_array;
            }
            break;
    }

    int datapoints = 28;
    data_frame *data = create_data_frame(datapoints, filename, header);

    benchmark(f, data, config, 0);

    file = fopen(filename, "a");

    if (file == NULL) {
        perror("Error opening file");
    }
    // write_to_csv(data, file);
    cleanup_data(data);
    fclose(file); 
}

void benchmark(int *(*f)(int*, int), data_frame *data, Options config, int datapoint) {
    long long nano = 0;
    double seconds = 0;
    int *str = NULL;

    switch (config.str_type) {
        case FIBONACCI:
            str = fib_str(str, config.size);
            config.size = fibonacci(config.size);
            config.asize = 2;
            printf("Running benchmark on lpf arrays %d times with fibonacci strings[1...%d], |∑| = %d\n", 
                    config.tries, config.size, config.asize);
            break;
        case RANDOM:
            printf("Running benchmark on lpf arrays %d times with random strings[1...%d], |∑| = %d\n", 
                config.tries, config.size, config.asize); 
            break;
    }   

    for (int i = 0; i < config.tries; i++) {
        if (config.str_type == RANDOM) {
            str = random_str(str, config.size, config.asize);
        }
        nano += timeit(f, str, config.size);
    }
    free(str);

    nano /= config.tries;
    seconds = NANO_TO_SEC(nano);

    data->data[datapoint][0] = config.size;
    data->data[datapoint][1] = config.str_type;
    data->data[datapoint][2] = config.tries;
    data->data[datapoint][3] = config.asize;
    data->data[datapoint][4] = nano;
    
    printf("%lld ns per call (%f seconds), total: %f seconds\n", nano, seconds, seconds * config.tries);
}

long long timeit(int *(*f)(int*, int), int *str, int str_len) {
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);
    int *arr = f(str, str_len);
    clock_gettime(CLOCK_MONOTONIC, &end);
    long nano = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
    free(arr);
    return nano;
}
