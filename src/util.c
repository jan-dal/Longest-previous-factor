#include "constants.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>

// Used for printing purposes
int find_prev(int *str, int i, int k) {
    for (int j = i-1; j >= 0; j--) {
        if (!memcmp(str+j, str+i, k * sizeof(int))) {
            return j;
        }
    }
    return 0;
}

void print_tuple_info(tuple_info *tinfo) {
    int *tmp = malloc(sizeof(int) * tinfo->total_blocks);
    for (int i = 0; i < tinfo->total_blocks; i++) {
        tmp[tinfo->tuple_sorting[i]] = i;
    }
    
    printf("TUPLES:\nNumber of all blocks: %d\nPOS\t|\tTYPE\t|\tSORTING\t|\tVALUES\n", tinfo->total_blocks);
    for (int i=0; i < tinfo->total_blocks; i++) {
        printf("%d\t|\t%d\t|\t%d\t|\t%d\t|\t", tinfo->positions[i], tinfo->tuple_type[i], tinfo->tuple_sorting[i], tmp[i]);

        for (int j=0; j < TUPLE_SIZE; j++) {
            int val = tinfo->values[i][j];
            printf(isprint(val) ? "%c " : "'%d' ", val);
            // printf("%d ", val);
        }
        printf("\n");
    }
    free(tmp);
}

void print_sa_from_tinfo(int *str, int str_len, tuple_info *tinfo, char *title) {
    printf("\n%s\n", title);
    printf("Sorting: ");
    for (int i = 0; i < tinfo->total_blocks; i++) {
        printf("%d ", tinfo->tuple_sorting[i]);
    } 
    printf("\n");
    for (int i = 0; i < tinfo->total_blocks; i++) {
        int pos = tinfo->positions[i];
        printf("%d ", pos);
        printf_array(str+pos, str_len-pos);
        printf("\n");
    }
    printf("\n");
}  

void printf_array(int *str, int str_len) {
    printf("[");
    for (int i = 0; i < str_len-1; i++) {
        int val = str[i];
        // printf("%d, ", val);
        printf(isprint(val) ? "%c " : "%d ", val);
        // printf("%d, ", val);
    }
    printf(isprint(str[str_len-1]) ? "%c]" : "%d]", str[str_len-1]);
    printf("\n");
}

void printf_line(int *str, int str_len, char *delim) {
    for (int i = 0; i < str_len; i++) {
        int val = str[i];
        printf(isprint(val) ? "%c" : "[%d]", val);
    }
    if (delim == NULL) {
        printf("\n");
    } else {
        printf("%s", delim);
    }
}

int *random_str(int *str, int len, int asize) {
    if (str == NULL) {
        str = calloc(len + ADDITIONAL_PADDING, sizeof(int));
    }
 
    srand((unsigned int)time(NULL));

    for (int i = 0; i < len; i++) {
        str[i] = (rand() % asize) + 97;
    }
    return str;
}

int file_has_line(char *filename) {
    FILE *file = fopen(filename, "r"); 

    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }

    char buffer[256];
    int has_line = 0;

    if (fgets(buffer, sizeof(buffer), file) != NULL) {
        has_line = 1;
    }
    fclose(file);
    return has_line;
}

void write_to_csv(DataFrame *data, FILE *file) {
    if (!file_has_line(data->filename)) {
        if (fprintf(file, "%s\n", data->header) < 0) {
                perror("Error writing header to file");
            }
    }

    for (int i = 0; i < data->datapoints; i++) {
        for (int j = 0; j < data->header_len-1; j++) {
            if (fprintf(file, "%lld,", data->data[i][j]) < 0) {
                perror("Error writing to file");
            }
        }
        if (fprintf(file, "%lld\n", data->data[i][data->header_len-1]) < 0) {
            perror("Error writing to file");
        }
    }
}

void cleanup_data(DataFrame *data) {
    for (int i = 0; i < data->datapoints; i++) {
        free(data->data[i]);
    } 
    free(data->data);
    free(data);
}

DataFrame *create_data_frame(int datapoints, char *filename, char *header) {
    DataFrame *data = malloc(sizeof(DataFrame));
    data->data = malloc(datapoints * sizeof(long long*));
    data->datapoints = datapoints;

    for (int i = 0; i < datapoints; i++) {
        data->data[i] = malloc(CSV_HEADER_LEN * sizeof(long long));
    }

    data->header_len = CSV_HEADER_LEN;
    data->header = header;
    data->filename = filename;

    return data;
}

int fibonacci(int n) {
    unsigned long long a = 1, b = 2, next;

    for (int i = 0; i < n; i++) {
        next = a + b;
        a = b; 
        b = next;
    }
    return a;
}

int *fib_str(int *str, int n) {
    if (str == NULL) {
        str = calloc(fibonacci(n) + ADDITIONAL_PADDING, sizeof(int));
    }
    str[0] = 'a';
    
    if (n >= 1) {
        str[1] = 'b';
    } else {
        return str;
    }

    int prev, curr_len;
    for (int i = 2; i <= n; i++) {
        prev = fibonacci(i-2);
        curr_len = fibonacci(i-1);
        memcpy(str+curr_len, str, prev * sizeof(int));
    }

    return str;
} 

int read_array_from_stdin(int **str, int no_output) {
    char *input = NULL;
    size_t cap = 0;
    int len;

    ssize_t nread = getdelim(&input, &cap, '\n', stdin);
    if (nread == -1) {
        fprintf(stderr, "Error: failed to read input\n");
        free(input);
        exit(EXIT_FAILURE);
    }

    // Remove trailing newline
    input[strcspn(input, "\n")] = '\0';
    len = nread-1;

    if (!no_output) {
        printf("Retrieved line of length %d\n\n", len);
    }
    LOG_FUNC(fwrite, input, nread, 1, stdout);

    // Convert to integer array
    *str = calloc(len + ADDITIONAL_PADDING, sizeof(int));
    for (int i = 0; i < len; i++) {
        (*str)[i] = (unsigned char)input[i];
    }

    free(input);

    return len;
}
