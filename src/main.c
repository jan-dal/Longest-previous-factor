#include "lpf.h"
#include "lcp.h"
#include "util.h"
#include "tuple.h"
#include "constants.h"
#include "benchmark.h"
#include "suffix_array.h"
#include "suffix_array_qsort.h"

#include <getopt.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

void print_help() {
    printf("Usage: ./lpf [OPTIONS] [PARAMETERS]\n\n");

    printf("Options:\n");
    printf("  -h, --help                       Display this help message\n");
    printf("  -l, --lpf [--no-output]          Calculate the LPF array; --no-output suppresses printing\n");
    printf("  -s, --suffix [--no-output]       Calculate the suffix and LCP arrays; --no-output suppresses printing\n");
    printf("  -v, --validate <s|l><r|f> <len> <tries> [<asize>]\n");
    printf("                                   Validate LPF or suffix arrays against naive (probably correct) method\n");
    printf("                                   s = suffix array, l = LPF; r = random, f = fibonacci\n");
    printf("                                   <asize> is required only for random strings\n");
    printf("  -b, --benchmark <s|l><r|f> <fast|qsort|naive> <len> <tries> [<asize>] [-o <file>]\n");
    printf("                                   Benchmark LPF or suffix arrays\n");
    printf("                                   s = suffix array, l = LPF; r = random, f = fibonacci\n");
    printf("                                   Algorithm options:\n");
    printf("                                     fast   - benchmark optimized implementation\n");
    printf("                                     qsort  - benchmark the qsort implementation\n");
    printf("                                     naive  - benchmark naive implementation\n");
    printf("                                   <asize> is required only for random strings; -o <file> to save results\n\n");

    printf("Examples:\n");
    printf("  ./lpf -s\n");
    printf("      Calculate suffix and LCP arrays\n\n");
    printf("  ./lpf -l\n");
    printf("      Calculate LPF array and print to console\n\n");
    printf("  ./lpf -l --no-output\n");
    printf("      Calculate LPF array without printing (useful for very large strings)\n\n");
    printf("  ./lpf -v lf 10 1\n");
    printf("      Validate LPF arrays on the 10th Fibonacci word (alphabet size implied = 2)\n\n");
    printf("  ./lpf -v sr 10000 5 4\n");
    printf("      Validate suffix arrays on random words of length 10000, 5 times, alphabet size 4\n\n");
    printf("  ./lpf -b sr fast 10000 10 4 -o benchmark.csv\n");
    printf("      Benchmark suffix arrays on random words of length 10000, 10 times, alphabet size 4, save to file\n");
}


void suffix_array_from_input() { 
    char *input = NULL;
    size_t input_length = 0;
    ssize_t nread;
    int str_len;
    printf("Enter a string: ");
    if ((nread = getdelim(&input, &input_length, '\n', stdin)) != -1) {
        // Remove delimiter
        input[strcspn(input, "\n")] = '\0';
        str_len = nread-1;
        printf("Retrieved line of length %d\n", str_len);
        LOG_FUNC(fwrite, input, nread, 1, stdout);
    } else {
        exit(1);
    }  
    printf("\n");

    // Expand character string to int type this puts a limit
    int *str = malloc(sizeof(int)*(str_len+ADDITIONAL_PADDING));

    for (int i = 0; i < str_len; i++) {
        str[i] = (int)input[i];
    }
    for (int i = str_len; i < str_len+ADDITIONAL_PADDING; i++) {
        str[i] = 0;
    }
    free(input);

    int *sa = suffix_array(str, str_len);
    int *sar = reverse_array(sa,  str_len);
    int *lcp = lcp_array(str, sa, sar, str_len);

    // int *saq = suffix_array_qsort(str, str_len);
    // printf_line(saq, str_len);

    print_suffix_array(str, sa, str_len);

    printf("SA:  ");
    printf_array(sa, str_len);

    printf("LCP: ");
    printf_array(lcp+1, str_len-1);
    printf("\n");
    free(sa);
    free(sar);
    free(str);
    free(lcp);
}

void lpf_array_from_input() {
    char *input = NULL;
    size_t input_length = 0;
    ssize_t nread;
    int str_len;
    printf("Enter a string: ");
    if ((nread = getdelim(&input, &input_length, '\n', stdin)) != -1) {
        // Remove delimiter
        input[strcspn(input, "\n")] = '\0';
        str_len = nread-1;
        printf("Retrieved line of length %d\n", str_len);
        LOG_FUNC(fwrite, input, nread, 1, stdout);
    } else {
        exit(1);
    }  
    printf("\n");

    // Expand character string to int type this puts a limit
    int *str = malloc(sizeof(int)*(str_len+ADDITIONAL_PADDING));

    for (int i = 0; i < str_len; i++) {
        str[i] = (int)input[i];
    }
    for (int i = str_len; i < str_len+ADDITIONAL_PADDING; i++) {
        str[i] = 0;
    }
    free(input);

    int *lpf = lpf_array(str, str_len);

    printf("LPF = ");
    printf_array(lpf, str_len);

    print_lpf_array(str, lpf, str_len);

    free(str);
    free(lpf);
}

 
int main(int argc, char *argv[]) {
    int size, tries, asize, str_type_arg;
    char alg_type_arg;
    StrType str_type = RANDOM;
    const char *short_opts = "hb:sv:l";
    const struct option long_opts[] = {
        {"help",    no_argument,       NULL, 'h'},
        {"lpf", no_argument, NULL, 'l'},
        {"suffix", no_argument, NULL, 's'},
        {"benchmark",  required_argument, NULL, 'b'},
        {"validate",  required_argument, NULL, 'v'},
        {NULL, 0, NULL, 0} // End marker
    };
    int opt;

    while ((opt = getopt_long(argc, argv, short_opts, long_opts, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_help();
                return 0;
            case 's':
                suffix_array_from_input();
                return 0;
            case 'l':
                lpf_array_from_input();
                return 0;
            case 'b':
                alg_type_arg = argv[optind-1][0];
                str_type_arg = argv[optind-1][1];
                size = atoi(argv[optind]);
                tries = atoi(argv[optind+1]);
                asize = atoi(argv[optind+2]);

                switch (str_type_arg) {
                    case 'r':
                        str_type = RANDOM;
                        break;
                    case 'f':
                        str_type = FIBONACCI;
                        break;
                    default:
                        str_type = RANDOM;
                }

                switch (alg_type_arg) {
                    case 's':
                        benchmark_runner(SUFFIX_ARRAY, str_type, size, tries, asize);
                        return 0;
                    case 'l':
                        benchmark_runner(LPF, str_type, size, tries, asize);
                        return 0;
                    default:
                        benchmark_runner(LPF, str_type, size, tries, asize);
                }
                return 0;
            case 'v':
                alg_type_arg = argv[optind-1][0]; 
                size = atoi(argv[optind]);
                tries = atoi(argv[optind+1]);
                asize = atoi(argv[optind+2]);
                if (alg_type_arg == 's') {
                    validate_suffix_array(size, tries, asize);
                } else {
                    validate_lpf(size, tries, asize);
                }
                return 0;
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
        return 1;
    }
    return 0;
}

