# Longest previous substring

For a given string w[1..n] we define an array LPF[1..n]. LPF[i] is the
largest k such that w[i..i + k − 1] occurs starting somewhere on the
left of i: w[i..i + k − 1] = w[i′..i′ + k − 1] with i′ < i (the previous
occurrence is allowed to overlap with w[i..i + k − 1]).

The LPF table can be then used to calculate the self-referential LZ parse of the input string.

The implementation uses an O(n) suffix array algorithm described here: [Simple Linear Work Suffix Array Construction](https://www.cs.cmu.edu/~guyb/realworld/papersS04/KaSa03.pdf)

### Example
Given the string: w[1...28] := `ababbababaaabbababaabaabbbaa`, LPF[13] = 8

<pre>
# For position 13 the longest previous substring occurrs on position 4

ababbababaaa <b>bbababaa</b> baabbbaa w[13...13 + 8 - 1]
aba <b>bbababaa</b> abbababaabaabbbaa w[4...4 + 8 - 1]
</pre>

## Results

![LPF fib results](results/lpf_fib_benchmark.png)
![SA fib results](results/sa_fib_benchmark.png)

## Build 

```
make 
```

```
Usage: ./lpf [options]
Options:
    -h, --help                                        Display this help message
    -l, --lpf                                         Calculate the lpf array
    -s, --suffix                                      Calculate the suffix and lcp arrays
    -v, --validate [sl][rf] <length> <tries> <asize>  Validate the [sl] agains the naive method <tries> random strings of length <length>
    -b, --benchmark [sl][rf] <length> <tries> <asize> Perform a benchmark with a random string[1...<length>], <tries> times
    Example:
        ./PATH/TO/PROGRAM/lpf -b lr 1000000 10 3
        ./PATH/TO/PROGRAM/lpf -s
    In case of fibonacci numbers ('f' argument) <length> means the #fibonnaci word
        ./PATH/TO/PROGRAM/lpf -v lf 10 1 2  Validate LPF arrays on the 10th fibonacci word (asize is by definition 2)
        ./PATH/TO/PROGRAM/lpf -b sr 10000 10 4  Benchmark suffix arrays on random words of length 10000, 10 times with alphabet size 4
```
