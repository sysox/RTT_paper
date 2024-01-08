//
// Created by user on 09/11/2023.
//


#define  BLOCK_BIT_SIZE 3
#define  BINS_COUNT (1 << BLOCK_BIT_SIZE)

#include  "generators.h"

int main(){
    int i, num_blocks, num_bytes;
    uint32_t freqs[BINS_COUNT];
    uint32_t values[BINS_COUNT];
    double chi2stat;
    unsigned char* output;
    unsigned char buffer[10000];

    //    multinomial_clusters(freqs,values, 3, clusters);
//    print_array(clusters, 6);
//    concatenate(clusters, 6, 3, output);
//    print_bitarray(output, 3);

//    seed_xorshift64(3);
//    multinomial(freqs, values, 3, 8, output, 10, 0);
//    print_bitarray(output, 10);
//    chi2stat = 2;
//    num_blocks = 10000;
//    num_bytes = num_blocks*BLOCK_BIT_SIZE / 8 + 3;
//    output = (unsigned char*)malloc(num_bytes);
//    for(i = 0; i < BINS_COUNT; i++)
//    {
//        values[i] = i;
//    }
//
//    Chi2_to_freqs(chi2stat, BINS_COUNT, num_blocks, freqs);
//    multinomial_exact(freqs, values, BINS_COUNT, BLOCK_BIT_SIZE, output, 10);
//    multinomial_not_exact(freqs, values, BINS_COUNT, BLOCK_BIT_SIZE, output, 10);
//
//
//
//    //no clusters
//    multinomial(freqs, values, BINS_COUNT, BLOCK_BIT_SIZE, output, num_blocks, -1);
//
//    //exact frequencies but if num_swaps is small probably clusters of blocks
//    multinomial(freqs, values, BINS_COUNT, BLOCK_BIT_SIZE, output, num_blocks, num_blocks*0.1);


    memset(buffer, 0x33, sizeof(buffer));
    printf("chi2=%lf", chi2_buffer(buffer, sizeof(buffer),  3));


}