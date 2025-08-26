#ifndef CONSTANTS_H
#define CONSTANTS_H

#define ALPHABET_SIZE 128
#define DIV 3

#define MIN_LEN 127
#define TUPLE_SIZE 3
#define ADDITIONAL_PADDING 3
#define DEBUG 0

#define CSV_HEADER_LEN 6
#define TIME_BENCHMARK_HEADER "IN_LEN,STR_TYPE,TRIES,ALPHABET_SIZE,ALG,NALG"
#define SA_BENCH_FILENAME "results/sa_bench_results_fib.csv"
#define LPF_BENCH_FILENAME "results/lpf_bench_results.csv"

#if DEBUG
    #define LOG_MESSAGE(...) printf(__VA_ARGS__)
    #define LOG_FUNC(func, ...) func(__VA_ARGS__)
#else
    #define LOG_MESSAGE(...)
    #define LOG_FUNC(...)
#endif

#define SEC_TO_NANO(sec) (long long)(1e9 * sec)

enum Algorithm {
    SUFFIX_ARRAY,
    LCP,
    LPF,
};
typedef enum Algorithm Algorithm;

enum StrType {
    RANDOM,
    FIBONACCI,
};
typedef enum StrType StrType;

struct tuple_info {
    int *positions;
    int *tuple_sorting;
    int *tuple_type;
    int (*values)[TUPLE_SIZE];
    int total_blocks;
    int max_name;
};
typedef struct tuple_info tuple_info;

struct data_frame {
    char *header;
    char *filename;
    long long **data;
    int datapoints;
    int header_len;
};
typedef struct data_frame data_frame;

#endif