//
// Created by user on 09/11/2023.
//

#ifndef RTT_UNIFORMITY_TESTING_GENERATORS_H
#define RTT_UNIFORMITY_TESTING_GENERATORS_H
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

void print_bitarray(unsigned char* array, int out_byte_size);
void print_array(uint32_t* array, unsigned long long out_byte_size);


void seed_xorshift32(uint32_t seed);
uint32_t xorshift32();

void seed_xorshift64(uint64_t seed);
uint64_t xorshift64();

void seed_xorshift64(uint64_t seed);
uint32_t xorshift128();

uint64_t rand_range(uint64_t a, uint64_t b);
uint64_t rand_whole_range(uint64_t a, uint64_t b);
double rand_double_range(double a, double b, uint64_t map_range);
int multinomial_lincom(float* probs, int size, uint64_t scale_factor);


void concatenate(const uint32_t* values, unsigned long long num_values, int value_bit_size, unsigned char* output);
void multinomial_clusters(const uint32_t* hist_freqs, const uint32_t* hist_values,
                        unsigned long long hist_size, uint32_t* output_values);
void swap(uint32_t* a, uint32_t* b);
void shuffling(uint32_t* values, unsigned long long num_values, unsigned long long num_swaps);
void multinomial(const uint32_t* hist_freqs, const uint32_t* hist_values,
                 int hist_size, int value_bit_size, int num_blocks, unsigned char* output);


#endif //RTT_UNIFORMITY_TESTING_GENERATORS_H
