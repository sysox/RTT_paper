//
// Created by user on 26/10/2023.
//
#include <endian.h>
#include "biased_gen.h"

uint64_t rngstate[4] = {0,1,2,3};

void set_rng_seed(uint64_t s) {
    rngstate[0] = s;
}

static inline uint64_t rotl(const uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}

// Returns a Uint64 random number => changed to Uint32
uint32_t next(void) {
    const uint64_t result = rotl(rngstate[0] + rngstate[3], 23) + rngstate[0];
    const uint64_t t = rngstate[1] << 17;
    rngstate[2] ^= rngstate[0];
    rngstate[3] ^= rngstate[1];
    rngstate[1] ^= rngstate[2];
    rngstate[0] ^= rngstate[3];
    rngstate[2] ^= t;
    rngstate[3] = rotl(rngstate[3], 45);
    return (uint32_t)result;                    //changed
}

static double chi2(double Ei, int num_bins, unsigned long long* Oi){
    double chi2_stat;
    int i;

    chi2_stat = 0;
    for (i = 0; i < num_bins; ++i) {
        chi2_stat += ((double)Oi[i] - Ei)*((double)Oi[i] - Ei)/Ei;
    }
    return chi2_stat;
}

int get_num_bins(int block_bit_size) {
    return 1 << block_bit_size;
}

void gen_freqs(double req_chi2stat, int num_freqs, unsigned long long num_blocks, unsigned long long* Oi){
    // (O_i - E_i)^2/ E_i = n(pi - p_expected)^2/p_expected
    double Ei, chi2_stat, current_value, tmp;
    int num_bins, i, floor_Ei, freq, idx1, idx2;
    unsigned long long sum;


    num_bins = num_freqs;
    memset(Oi, 0, num_bins*sizeof(*Oi));

    Ei = 1.0*num_blocks/num_bins;

    // basic initial setting with minimum Chi2stat
    floor_Ei = (int)Ei;
    freq = 1.0*(Ei - floor_Ei)*num_bins;
    chi2_stat = 0;
    for(i = 0; i < freq; i++ ){
        Oi[i] =  floor_Ei + 1;
    }
    for(i = freq; i < num_bins; i++ ){
        Oi[i] =  floor_Ei ;
    }

    chi2_stat += ( floor_Ei + 1 - Ei )*( floor_Ei + 1 - Ei )/Ei*freq;
    chi2_stat += ( floor_Ei - Ei)*( floor_Ei - Ei)/Ei*(num_bins-freq);

    //testing whether sum of Oi == number of blocks!
    sum = 0;
    for(i = 0; i < num_bins; i++)
    {
        sum += Oi[i];
    }
    if (sum != num_blocks){
        Oi[0] += num_blocks - sum;
//        printf("%llu != number of blocks %llu\n", sum, num_blocks);
    }

    if (chi2_stat > req_chi2stat){
        return;
    }
    while(chi2_stat < req_chi2stat){
        idx1 = next() % num_bins;
        idx2 = next() % num_bins;
        current_value = ( Oi[idx1] - Ei )*(  Oi[idx1] - Ei )/Ei + ( Oi[idx2] - Ei )*(  Oi[idx2] - Ei )/Ei;
        tmp = ( Oi[idx1] - 1 - Ei )*(  Oi[idx1] - 1 - Ei )/Ei + ( Oi[idx2] + 1 - Ei )*(  Oi[idx2] + 1 - Ei )/Ei;
        if(tmp > current_value)
        {
            if ((Oi[idx1] <= 0) || (Oi[idx2] >= num_blocks))
                continue;
            Oi[idx1]--;
            Oi[idx2]++;
            chi2_stat += tmp - current_value;
//            printf("%f vs recomputed =%f\n", chi2_stat, chi2(Ei, num_bins, Oi));
        }
        else{
            tmp = ( Oi[idx1] + 1 - Ei )*(  Oi[idx1] + 1 - Ei )/Ei + ( Oi[idx2] - 1 - Ei )*(  Oi[idx2] - 1 - Ei )/Ei;
            if(tmp > current_value)
            {
                if ((Oi[idx2] <= 0) || (Oi[idx1] >= num_blocks))
                    continue;
                Oi[idx1]++;
                Oi[idx2]--;
                chi2_stat += tmp - current_value;
//                printf("%f vs recomputed =%f\n", chi2_stat, chi2(Ei, num_bins, Oi));
            }
        }

    }


    //testing whether sum of Oi == number of blocks!
    sum = 0;
    for(i = 0; i < num_bins; i++)
    {
        sum += Oi[i];
    }
    if (sum != num_blocks){
        printf("%llu != number of blocks", sum);
    }
    printf("%f vs recomputed =%f\n\n", chi2_stat, chi2(Ei, num_bins, Oi));
}

void basic_dist(const unsigned long long* Oi, int block_size,
                unsigned long long num_blocks, unsigned char* output){
    unsigned long long i, freq, to_be_written;
    int block_value, num_bins;
    unsigned char *write_ptr;
    uint32_t block_value_le;

    num_bins = get_num_bins(block_size);
    write_ptr = output;
    to_be_written = num_blocks;

    //    not shuffled with given probs
    for (block_value = 0; block_value < num_bins && to_be_written > 0; block_value++) {
        freq = Oi[block_value];
        for(i = 0; i < freq && to_be_written > 0; i++) {
            block_value_le = htole32(block_value);
            memcpy(write_ptr, &block_value_le, block_size);
            write_ptr += block_size;
            to_be_written--;
        }
    }
    //    according to rounding generate RANDOMLY rest of blocks
//    while(to_be_written > 0)
//    {
//        block_value = next() % (1 << (block_size*8));         //random block - int value from ([0,..., 2^8-1 or 2^16-1])
//        memcpy(write_ptr, &block_value, block_size);
//        write_ptr += block_size;
//        to_be_written--;
//    }
}
void shuffling(const unsigned char* src, int block_size, unsigned long long num_blocks,
               unsigned long long num_swaps, unsigned char* output){
    unsigned int idx1, idx2;
    unsigned long long i;
    unsigned char tmp[10];

    //copy unsuffled dist
    memcpy(output, src, num_blocks*block_size);

    //suffling - swaps
    for(i = 0; i < num_swaps; i++)
    {
       idx1 = next() % num_blocks;
       idx2 = next() % num_blocks;
       memcpy(tmp, output + idx1*block_size, block_size);
       memcpy(output + idx1*block_size, output + idx2*block_size, block_size);
       memcpy(output + idx2*block_size, tmp, block_size);
    }
}
void random_selection(const unsigned char* src, int block_size,
                      unsigned long long num_blocks, unsigned char* output){
    unsigned int idx;
    unsigned long long i;
    unsigned char *write_ptr;

    write_ptr = output;
    for (i = 0; i < num_blocks; i++) {
        idx = next() % num_blocks;                    // random block from
        memcpy(write_ptr, src + idx*block_size, block_size);
        write_ptr += block_size;
    }
}
