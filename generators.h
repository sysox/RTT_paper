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

void print_bitarray(unsigned char* array, uint32_t size);
void print_array(uint32_t* array, uint32_t size);
void swap(uint32_t* a, uint32_t* b);
double chi2(double Ei, int num_bins, uint32_t *Oi);

void seed_xorshift32(uint32_t seed);
uint32_t xorshift32();

void seed_xorshift64(uint64_t seed);
uint64_t xorshift64();

void seed_xorshift128(uint64_t seed);
uint32_t xorshift128();

// returns integer from in [a, b) i.e. EXCEPT b
uint64_t rand_range(uint64_t a, uint64_t b);

// returns integer from in [a, b] i.e. INCLUDING b
uint64_t rand_whole_range(uint64_t a, uint64_t b);

// returns double in [a, b),  scale factor represents "granularity/precision/step" of returned values
double rand_double_range(double a, double b, uint64_t pool_size, uint64_t zoom);

// generates index of some event according to probabilities (probs - probabilities of events)
int multinomial_lincom(double* probs, int size, uint64_t scale_factor, uint64_t zoom);

// concatenation of bit-blocks of given size (value_bit_size) stored as LSB bits in values,
// output should be allocated larger than necessary (+3 Bytes!!!)
void concatenate(const uint32_t* values, uint32_t num_values, int value_bit_size, unsigned char* output);

// array of values (hist_values) with given frequencies (hist_freqs):  v_0, v_0, ..., v_1, ...v_1, ...
void multinomial_clusters(const uint32_t* hist_freqs, const uint32_t* hist_values,
                          uint32_t hist_size, uint32_t* output_values);


// swaps iteratively values inside array
void shuffling(uint32_t* values, uint32_t num_values, uint32_t num_swaps);

// random sample from values (random indices - possible repeated values even if values are unique )
void random_sample(const uint32_t* values, uint32_t num_values, uint32_t* sample, int sample_size);

//multinomial distribution - with exact frequencies of values  (shuffled with given number of swaps),
// if num_swaps < 0 random sampling is selected (not exact frequencies, possibly repeated values )
// as results values (blocks of bits are  concatenated)
void multinomial(const uint32_t* hist_freqs, const uint32_t* hist_values, int hist_size,
                 int value_bit_size, unsigned char* output, int num_values, int num_swaps);

// generate histogram frequencies corresponding to chi2stat
void Chi2_to_freqs(double chi2stat, int hist_size, uint32_t freq_sum,
                   uint32_t* Oi_freqs);

#endif //RTT_UNIFORMITY_TESTING_GENERATORS_H
