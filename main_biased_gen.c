//
// Created by user on 26/10/2023.
//

#include <stdio.h>
#include "biased_gen.h"

void print_array(unsigned char* array, int size){
    int i = 0;
    printf("\n");
    for(i = 0; i < size; i++)
    {
        printf("%02x ", array[i]);
    }
    printf("\n");
    fflush(stdin);
}
int main(){
    unsigned char *output, *reference;
    int i, j,  num_blocks, block_size, num_swaps, *Oi, num_bins;
    float probs[256] = {0.3, 0.5, 0.2};
    block_size = 2;
    num_blocks = 1245;
    num_bins = (1<<(block_size*8));

    reference = (unsigned char *) malloc(block_size*num_blocks);
    if (reference == NULL)printf("reference not allocated");
    output = (unsigned char *) malloc(block_size*num_blocks);
    if (output == NULL)printf("output not allocated");
    Oi = (int *)malloc(num_bins*sizeof(int));
    if (Oi == NULL)printf("Oi not allocated");




    block_size = 1;
    num_blocks = 10000000;
    num_swaps = 1.0*num_blocks;


    gen_freqs(1000, block_size, num_blocks, Oi);

//    basic_dist(Oi, block_size, num_blocks, reference);
////    print_array(reference, num_blocks*block_size);
//
//    shuffling(reference, block_size, num_blocks, num_swaps, output);
////    print_array(output, num_blocks*block_size);
//
//    random_selection(reference, block_size, num_blocks, output);
//    print_array(output, num_blocks*block_size);

    free((void *)output);
    free((void *)reference);
    free((void *)Oi);
    return 0;
}