#include "lpf.h"
#include "lcp.h"
#include "util.h"
#include "tuple.h"
#include "constants.h"
#include "benchmark.h"
#include "suffix_array.h"

#include <bits/getopt_core.h>
#include <getopt.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static const char *short_opts = "io:hbsvl";
static const struct option long_opts[] = {
    {"help",    no_argument,       NULL, OPT_HELP},
    {"lpf", no_argument, NULL, OPT_LPF},
    {"suffix", no_argument, NULL, OPT_SUFFIX},
    {"input", no_argument, NULL, OPT_INPUT},
    {"benchmark",  no_argument, NULL, OPT_BENCHMARK},
    {"validate",  no_argument, NULL, OPT_VALIDATE},
    {"size", required_argument, NULL, OPT_SIZE},
    {"asize", required_argument, NULL, OPT_ASIZE},
    {"tries", required_argument, NULL, OPT_TRIES},
    {"fast", no_argument, NULL, OPT_FAST},
    {"qsort", no_argument, NULL, OPT_QSORT},
    {"naive", no_argument, NULL, OPT_NAIVE},
    {"random", no_argument, NULL, OPT_RANDOM},
    {"fibonacci", no_argument, NULL, OPT_FIBONACCI},
    {"no-output", no_argument, NULL, OPT_NO_OUTPUT},
    {NULL, 0, NULL, 0} // End marker
};

void print_help() {
    printf("Usage: ./lpf [MODE|h] [<ALGORITHM>] [<PARAMETERS>]\n\n");

    printf("Options:\n");
    printf("  -h, --help                Display this help message\n\n");

    printf("MODE:\n");
    printf("  -i, --input <ALGORITHM> [--no-output]\n");
    printf("                            Run the specified algorithm from input\n");    
    printf("  -b, --benchmark <ALGORITHM> <STR_TYPE> <tries> [-o <file>]\n");    
    printf("                            Benchmark LPF or suffix arrays (save output with -o)\n");
    printf("  -v, --validate <ALGORITHM> <STR_TYPE> <tries>\n");
    printf("                            Validate LPF or suffix arrays against naive (probably correct) method\n\n");

    printf("STR_TYPE: (default: random)\n");
    printf("  --random <size> <asize> - Use random generated strings of length <size> and alphabet size <asize>\n");
    printf("  --fibonacci <size>      - Use the <size>'th fibonacci string\n\n");
    
    printf("ALGORITHM:\n");
    printf("  -l, --lpf [ALG_OPTS]    - Calculates the LPF array using the implementation alg_options.\n");
    printf("  -s, --suffix [ALG_OPTS] - Calculates the suffix array using the implementation alg_options.\n\n");

    printf("ALG_OPTS: (default: fast)\n");
    printf("  --fast                  - Optimized implementation\n");
    printf("  --qsort                 - Implementation using qsort\n");
    printf("  --naive                 - Simplest implementation\n\n");

    printf("Examples:\n");
    printf("  ./lpf -i -s --qsort\n");
    printf("      Calculate suffix from input using the qsort implementation\n\n");
    printf("  ./lpf -i -l --no-output\n");
    printf("      Calculate the optimized LPF array and suppress any output\n\n");
    printf("  ./lpf -v -l --size 1000000 --tries 10 --asize 26\n");
    printf("      Validate LPF arrays on random strings of length 1M, 10 times and alphabet size = 26\n\n");
    printf("  ./lpf -v -s -fibonacci --size 30 --tries 5\n");
    printf("      Validate suffix arrays on the 30th fibonacci string, 5 times\n\n");
    printf("  ./lpf -b -l --random --fast --size 10000 --tries 10 --asize 4 -o benchmark.csv\n");
    printf("      Benchmark suffix arrays on random words of length 10000, 10 times, alphabet size 4, save to file\n");
}


void suffix_array_from_input(int no_output) { 
    int *str;
    
    int len = read_array_from_stdin(&str, no_output);

    int *sa = suffix_array(str, len);
    int *sar = reverse_array(sa,  len);
    int *lcp = lcp_array(str, sa, sar, len);

    if (!no_output) {
        print_suffix_array_summary(str, sa, lcp, len);
    }

    free(sa);
    free(sar);
    free(str);
    free(lcp);
}

void lpf_array_from_input(int no_output) {
    int *str;

    int len = read_array_from_stdin(&str, no_output);

    int *lpf = lpf_array(str, len);

    if (!no_output) {
        print_lpf_array_summary(str, lpf, len);
    }

    free(str);
    free(lpf);
}

void run_validation(Options config) {
    switch (config.alg) {
        case LPF:
            validate_lpf(config.size, config.tries, config.asize);
            return;
        case SUFFIX_ARRAY:
            validate_suffix_array(config.size, config.tries, config.asize);
            return;
        default:
            validate_lpf(config.size, config.tries, config.asize);
    } 
}

void run_program(Options config) {
    switch(config.mode) {
        case INPUT:
            switch (config.alg) {
                case LPF:
                    lpf_array_from_input(config.no_output);
                    break;
                case SUFFIX_ARRAY:
                    suffix_array_from_input(config.no_output);
                    break;
            }
            return;
        case BENCHMARK:
            benchmark_runner(config);
            return;
        case VALIDATE:
            run_validation(config);
            return;
    }
}
 
int main(int argc, char *argv[]) {
    int opt;
    Options config = {0};

    while ((opt = getopt_long(argc, argv, short_opts, long_opts, NULL)) != -1) {
        switch (opt) {
            case OPT_HELP:
                print_help();
                exit(0);
            case OPT_SUFFIX:
                config.alg = SUFFIX_ARRAY;
                break;
            case OPT_LPF:
                config.alg = LPF;
                break;
            case OPT_BENCHMARK:
                config.mode = BENCHMARK;
                break;
            case OPT_VALIDATE:
                config.mode = VALIDATE;
                break;
            case OPT_INPUT:
                config.mode = INPUT;
                break;
            case OPT_SIZE:
                config.size = atoi(optarg);
                break;
            case OPT_TRIES:
                config.tries = atoi(optarg);
                break;
            case OPT_ASIZE:
                config.asize = atoi(optarg);
                break;
            case OPT_RANDOM:
                config.str_type = RANDOM;
                break;
            case OPT_FIBONACCI:
                config.str_type = FIBONACCI;
                break;
            case OPT_FAST:
                config.alg_impl = FAST;
                break;
            case OPT_QSORT:
                config.alg_impl = QSORT;
                break;
            case OPT_NAIVE:
                config.alg_impl = NAIVE;
                break;
            case OPT_NO_OUTPUT:
                config.no_output = 1;
                break;
            case OPT_OUTPUT:
                config.out_file = strdup(optarg);
                break;
            case '?':
                print_help();
                return 0;
            default:
                print_help();
                return 0;
        }
    }
    if (argc == 1) {
        print_help();
    }

    if (optind < argc) {
        printf("Unrecognized arguments: ");
        while (optind < argc) {
            printf("%s ", argv[optind++]);
        }
        printf("\n");
        exit(1);
    }

    run_program(config);
    
    free(config.out_file);
    return 0;
}

