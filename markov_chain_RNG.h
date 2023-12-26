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

void histfreqs_to_probs(const uint32_t* hist_freqs, int hist_size, long long int freqs_sum, double *Oi_probs);
void stochastic_matrix(double *stationary_probability_vec, int dim, double **stochastic_mat, int num_iters);
void markov_chain_seq(double **stochastic_mat, int dim, const uint32_t* state_values,
                      int seq_size, uint32_t* output_chain_values, uint64_t scale_factor);
void Chi2_MC(double chi2stat, int value_bit_size, int chain_size, unsigned char* output);
#endif //RTT_UNIFORMITY_TESTING_MASRKOV_CHAIN_RNG_H