//
// Created by user on 06/11/2023.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "markov_chain_RNG.h"
#include "generators.h"

#define value_bit_size 6
#define hist_size ((1 << value_bit_size))
int main() {
    unsigned char *output;
    double chi2stat;
    int i, Ei;
    int chain_size = 1000000;

    unsigned long long  Oi_freqs[hist_size] = {};


    seed_xorshift32(3);
    chi2stat = 1000;


    output = (unsigned char*)malloc(sizeof(unsigned char )*(chain_size+3));
    Chi2_MC(chi2stat, value_bit_size, chain_size, output);


    return 0;
}
