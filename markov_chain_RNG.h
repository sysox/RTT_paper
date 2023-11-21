//
// Created by user on 06/11/2023.
//

#ifndef RTT_UNIFORMITY_TESTING_MASRKOV_CHAIN_RNG_H
#define RTT_UNIFORMITY_TESTING_MASRKOV_CHAIN_RNG_H
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "biased_gen.h"

float** allocate(int dim);
void print_vec(float** T, int dim);
void print_mat(float* T, int dim);

void gen_global_probs(double req_chi2stat, int num_freqs, unsigned long long num_blocks, unsigned long long* Oi_freqs, float* Oi_probs);
void  stochastic_matrix(float *p_vec, int dim, float **T);

#endif //RTT_UNIFORMITY_TESTING_MASRKOV_CHAIN_RNG_H
