//
// Created by user on 06/11/2023.
//


#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "markov_chain_RNG.h"

#define block_bit_size 3
#define dim 4

int main() {
    double **T, Oi_probs[dim] = {0.1, 0.2, 0.6, 0.1}, req_chi2stat;
    int  num_blocks;
    unsigned long long Oi_freqs[dim];
    num_blocks = 1000;
    req_chi2stat = 10;

    T = allocate(dim);
    stochastic_matrix(Oi_probs, dim, T);

    return 0;
}
