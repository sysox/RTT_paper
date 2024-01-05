//
// Created by user on 09/11/2023.
//


#define  block_bit_size 3
#define  bins_count (1 << block_bit_size)

#include "generators.h"

int main(){
    int i, num_blocks, num_bytes;
    uint32_t freqs[bins_count];
    uint32_t values[bins_count];
    uint32_t clusters[6];
    double chi2stat;
    unsigned char* output;

    //    multinomial_clusters(freqs,values, 3, clusters);
//    print_array(clusters, 6);
//    concatenate(clusters, 6, 3, output);
//    print_bitarray(output, 3);

//    seed_xorshift64(3);
//    multinomial(freqs, values, 3, 8, output, 10, 0);
//    print_bitarray(output, 10);
    chi2stat = 2;
    num_blocks = 10000;
    num_bytes = num_blocks*block_bit_size / 8 + 3;
    output = (unsigned char*)malloc(num_bytes);
    for(i = 0; i < bins_count; i++)
    {
        values[i] = i;
    }

    Chi2_to_freqs(chi2stat, bins_count, num_blocks, freqs);
    multinomial_exact(freqs, values, bins_count, block_bit_size, output, 10);
    multinomial_not_exact(freqs, values, bins_count, block_bit_size, output, 10);

    //no clusters
    multinomial(freqs, values, bins_count, block_bit_size, output, num_blocks, -1);

    //exact frequencies but if num_swaps is small probably clusters of blocks
    multinomial(freqs, values, bins_count, block_bit_size, output, num_blocks, num_blocks*0.1);


}