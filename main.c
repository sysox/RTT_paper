//
// Created by user on 09/11/2023.
//

#include "generators.h"

int main(){
    int i;
    uint32_t freqs[3] = {2, 1, 3};
    uint32_t values[3] = {3, 5, 4};
    uint32_t clusters[6];
    unsigned char output[7];
    multinomial_clusters(freqs,values, 3, clusters);
    print_array(clusters, 6);
    concatenate(clusters, 6, 3, output);
//    print_bitarray(output, 3);

}