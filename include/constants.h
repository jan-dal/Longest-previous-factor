#ifndef CONSTANTS_H
#define CONSTANTS_H

#define ALPHABET_SIZE 128
#define DIV 3

#define MIN_LEN 127
#define ADDITIONAL_PADDING 3
#define DEBUG 0

#define CSV_HEADER_LEN 5
#define TIME_BENCHMARK_HEADER "IN_LEN,STR_TYPE,TRY,ALPHABET_SIZE,ALG(NS)"

#if DEBUG
    #define LOG_MESSAGE(...) printf(__VA_ARGS__)
    #define LOG_FUNC(func, ...) func(__VA_ARGS__)
#else
    #define LOG_MESSAGE(...)
    #define LOG_FUNC(...)
#endif

#define NANO_TO_SEC(nano) (double)(nano / 1e9)

enum Mode {
    INPUT,
    BENCHMARK,
    VALIDATE,
};
typedef enum Mode Mode;

enum Cli_opts {
    OPT_HELP      = 'h',
    OPT_LPF       = 'l',
    OPT_SUFFIX    = 's',
    OPT_BENCHMARK = 'b',
    OPT_INPUT     = 'i',
    OPT_OUTPUT    = 'o',
    OPT_VALIDATE  = 'v',
    OPT_SIZE,
    OPT_ASIZE,
    OPT_TRIES,
    OPT_FAST,
    OPT_QSORT,
    OPT_NAIVE,
    OPT_RANDOM,
    OPT_FIBONACCI,
    OPT_NO_OUTPUT,
};

enum Algorithm {
    SUFFIX_ARRAY,
    LPF,
};
typedef enum Algorithm Algorithm;

enum Algorithm_impl {
    FAST,
    QSORT,
    NAIVE,
};
typedef enum Algorithm_impl Algorithm_impl;

enum StrType {
    RANDOM,
    FIBONACCI,
};
typedef enum StrType StrType;

typedef struct {
    Mode mode;
    Algorithm alg;
    Algorithm_impl alg_impl;
    StrType str_type;
    char *out_file;
    int size;
    int tries;
    int asize;
    int no_output;
} Options;

struct DataFrame {
    char *header;
    char *filename;
    long long **data;
    int datapoints;
    int header_len;
};
typedef struct DataFrame DataFrame;

#endif