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


double** allocate(int dim);
void print_vec(double *T, int dim);
void print_mat(double **T, int dim);

void histfreqs_to_probs(const uint32_t* hist_freqs, unsigned int hist_size, uint32_t freqs_sum, double *Oi_probs);
void stochastic_matrix(double *stationary_probability_vec, unsigned int dim, double **stochastic_mat, unsigned int num_iters);
void markov_chain_seq(double **stochastic_mat, unsigned int dim, const uint32_t* state_values,
                      uint32_t seq_size, uint32_t* output_chain_values, uint64_t scale_factor);
void Chi2_MC(double chi2stat, unsigned int value_bit_size, uint32_t chain_size, unsigned char* output);
#endif //RTT_UNIFORMITY_TESTING_MASRKOV_CHAIN_RNG_H
