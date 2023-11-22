//
// Created by user on 06/11/2023.
//


#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "markov_chain_RNG.h"
#include "generators.h"


int main() {
    unsigned char *output;
    double chi2stat = 3000;
    int value_bit_size = 8, i, Ei;
    int chain_size = 100000000;

    unsigned long long  Oi_freqs[256] = {};

    seed_xorshift32(6);

    output = (unsigned char*)malloc(sizeof(unsigned char )*(chain_size+3));
    Chi2_MC(chi2stat, value_bit_size, chain_size, output);
    for(i = 0; i < chain_size; i++){
        Oi_freqs[output[i]]++;
    }
    Ei = chain_size/256;
    chi2stat = chi2(Ei, 256, Oi_freqs);
    printf("%lf", chi2stat);



    return 0;
}
