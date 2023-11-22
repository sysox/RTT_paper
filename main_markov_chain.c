//
// Created by user on 06/11/2023.
//


#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "markov_chain_RNG.h"
#include "generators.h"


int main() {
    unsigned char output[3*100/8+4];
    double chi2stat = 10;
    int value_bit_size = 3;
    int chain_size = 100;

    seed_xorshift32(6);
    Chi2_MC(chi2stat, value_bit_size, chain_size, output);



    return 0;
}
