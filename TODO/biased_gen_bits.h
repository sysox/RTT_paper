//
// Created by user on 06/11/2023.
//

#ifndef RTT_UNIFORMITY_TESTING_BIASED_GEN_BITS_H
#define RTT_UNIFORMITY_TESTING_BIASED_GEN_BITS_H
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "biased_gen.h"

float** allocate(int dim);
void print_vec(float** T, int dim);
void print_mat(float* T, int dim);

float rand_interval(float a, float b, int map_range);
void gen_global_probs(double req_chi2stat, int num_freqs, unsigned long long num_blocks, unsigned long long* Oi_freqs, float* Oi_probs);
void  stochastic_matrix(float *p_vec, int dim, float **T);

//void basic_dist(const unsigned long long *Oi, int block_size, unsigned long long num_blocks, unsigned char* output);
//void shuffling(const unsigned char* src, int block_size, unsigned long long num_blocks, unsigned long long num_swaps, unsigned char* output);
//void random_selection(const unsigned char* src, int block_size, unsigned long long num_blocks, unsigned char* output);

#endif //RTT_UNIFORMITY_TESTING_BIASED_GEN_BITS_H
