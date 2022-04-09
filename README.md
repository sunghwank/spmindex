# Practical Space-Efficient Index for Structural Pattern Matching

## Description

A C++ implementation of a compact data structure for structural pattern matching described in the following paper: 

**Sung-Hwan Kim and Hwan-Gue Cho, "Practical Space-Efficient Index for Structural Pattern Matching," to appear at IWOCA 2022.**


## Compile and run

Prerequisite: sdsl-lite (https://github.com/simongog/sdsl-lite)

<pre>
<code>
$ make
$ ./suffix-array 10000 16 > example.suf  # generate a random s-string along with its even and odd suffix arrays.
$ ./count-test                           # pattern matching test module.  
</code>
</pre>

If failed to compile, you may have to specify SDSL_INCLUDE and SDSL_LIB in Makefile.
