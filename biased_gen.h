//
// Created by user on 26/10/2023.
//

#ifndef RTT_UNIFORMITY_TESTING_BIASED_GEN_H
#define RTT_UNIFORMITY_TESTING_BIASED_GEN_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

void gen_freqs(double req_chi2stat, int block_size, unsigned long long num_blocks, unsigned long long* Oi);
void basic_dist(const unsigned long long *Oi, int block_size, unsigned long long num_blocks, unsigned char* output);
void shuffling(const unsigned char* src, int block_size, unsigned long long num_blocks, unsigned long long num_swaps, unsigned char* output);
void random_selection(const unsigned char* src, int block_size, unsigned long long num_blocks, unsigned char* output);
#endif //RTT_UNIFORMITY_TESTING_BIASED_GEN_H
