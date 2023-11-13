//
// Created by user on 06/11/2023.
//


#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "TODO/biased_gen_bits.h"

#define block_bit_size 3
#define dim 10

int main() {
    float **T, Oi_probs[dim], req_chi2stat;
    int  num_blocks;
    unsigned long long Oi_freqs[dim];
    num_blocks = 1000;
    req_chi2stat = 10;

    T = allocate(dim);
    gen_global_probs(req_chi2stat, dim, num_blocks, Oi_freqs, Oi_probs);
    stochastic_matrix(Oi_probs, dim, T);

    return 0;
}
