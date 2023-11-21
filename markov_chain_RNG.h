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

void histfreqs_to_probs(const long long int* hist_freqs, int hist_size, long long int freqs_sum, double *Oi_probs);
void stochastic_matrix(double *p_vec, int dim, double **T);

#endif //RTT_UNIFORMITY_TESTING_MASRKOV_CHAIN_RNG_H
