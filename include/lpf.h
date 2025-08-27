#ifndef LPF_H
#define LPF_H

struct binode {
    int prev;
    int next;
    int prev_val;
    int next_val;
};
typedef struct binode binode;

int *lpf_array(int *str, int str_len);
int *lpf_array_naive(int *str, int str_len);
void print_lpf_array(int *str, int *lpf, int len);
void print_lpf_array_summary(int *str, int *lpf, int len);

#endif