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

void gen_freqs(double req_chi2stat, int block_size, int num_blocks, int* Oi);
void basic_dist(int *Oi, int block_size, int num_blocks, unsigned char* output);
void shuffling(unsigned char* src, int block_size, int num_blocks, int num_swaps, unsigned char* output);
void random_selection(unsigned char* src, int block_size, int num_blocks, unsigned char* output);
#endif //RTT_UNIFORMITY_TESTING_BIASED_GEN_H
