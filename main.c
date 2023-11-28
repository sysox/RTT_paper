//
// Created by user on 09/11/2023.
//

#include "generators.h"

int main(){
    int i, hist_size;
    uint32_t freqs[3] = {2, 1, 3};
    uint32_t values[3] = {3, 5, 4};
    uint32_t clusters[6];
    double chi2stat;
    uint32_t freqs2[3];
    unsigned char output[13];
//    multinomial_clusters(freqs,values, 3, clusters);
//    print_array(clusters, 6);
//    concatenate(clusters, 6, 3, output);
//    print_bitarray(output, 3);

//    seed_xorshift64(3);
//    multinomial(freqs, values, 3, 8, output, 10, 0);
//    print_bitarray(output, 10);
    chi2stat = 2;
    hist_size = 3;

    Chi2_to_freqs(chi2stat, hist_size, 100, freqs2);
    multinomial(freqs2, values, hist_size, 8, output, 10, -1);
    print_bitarray(output, 10);
}