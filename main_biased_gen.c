//
// Created by user on 26/10/2023.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "generators.h"
#include "markov_chain_RNG.h"

static int help(const char *arg0, const char *err) {
    if (err)
        printf("%s\n\n", err);
    printf("Use: %s --file=<output> [--size=<MBytes>] [--chi2=<num>] [--swaps=<num>] [--blocksize=<num>] [--seed=<num>]\n", arg0);
    return EXIT_FAILURE;
}

int main(int argc, char *argv[]) {
    unsigned char *output;
    int i, block_bit_size, hist_size;
    char opt[129], val[129], *file, *end;
    unsigned long long size_bytes, num_blocks, num_swaps, *Oi, seed;
    double chi2;
    FILE *fp;

    // defaults
    file = NULL;
    block_bit_size = 1;
    hist_size = 1 << block_bit_size;
    num_swaps = 0;
    chi2 = 1000.;
    size_bytes = 100 * 1024 *1024;
    seed = 3;

    for (i = 1; i < argc; i++) {
        if (sscanf(argv[i], "--%128[^=]=%128s", opt, val) != 2)
            return help(argv[0], NULL);

        if (!strcmp(opt, "file")) {
            file = strdup(val);
        } else if (!strcmp(opt, "size")) {
            size_bytes = strtoull(val, &end, 10);
            if (*end || errno == ERANGE)
                return help(argv[0], "Invalid file size.");
        } else if (!strcmp(opt, "blocksize")) {
            block_bit_size = strtol(val, &end, 10);
            if (*end || errno == ERANGE)
                return help(argv[0], "Invalid block size.");
        } else if (!strcmp(opt, "chi2")) {
            chi2 = strtod(val, &end);
            if (*end || errno == ERANGE)
                return help(argv[0], "Invalid chi2.");
        } else if (!strcmp(opt, "swaps")) {
            num_swaps = strtoull(val, &end, 10);
            if (*end || errno == ERANGE)
                return help(argv[0], "Invalid swaps.");
        } else if (!strcmp(opt, "seed")) {
            seed = strtoull(val, &end, 10);
            if (*end || errno == ERANGE)
                return help(argv[0], "Invalid seed.");
        } else
                return help(argv[0], "Invalid parameter name.");
    }

    if (!file)
        return help(argv[0], "File parameter is mandatory.");

    if (!size_bytes)
        return help(argv[0], "File cannot be empty.");

    if (block_bit_size < 1 || block_bit_size > 3)
        return help(argv[0], "Blocksize can be 1..3 only.");

    printf("PARAMS: file %s, size %llu, blocksize %i (bins %i), chi2 %f, swaps %llu, seed %llu\n",
           file, size_bytes, block_bit_size, hist_size, chi2, num_swaps, seed);

    num_blocks = (size_bytes * 8) / block_bit_size;

    output = malloc(size_bytes + 3);
    Oi = malloc(hist_size * sizeof(*Oi));

    if (!output || !Oi) {
        printf("Cannot allocate memory.\n");
        return EXIT_FAILURE;
    }

    seed_xorshift32(seed);

/** multinomial */
/*
    uint32_t freqs[hist_size];
    uint32_t values[hist_size];
    Chi2_to_freqs(chi2, hist_size, num_blocks, freqs);

    for(i = 0; i < hist_size; i++)
        values[i] = i;

    //no clusters
    multinomial(freqs, values, hist_size, block_bit_size, output, num_blocks, -1);

    //exact frequencies but if num_swaps is small probably clusters of blocks
    //multinomial(freqs, values, hist_size, block_bit_size, output, num_blocks, num_blocks*0.1);
*/


    Chi2_MC(chi2, block_bit_size, num_blocks, output);

    fp = fopen(file, "w");
    if (!fp) {
        printf("File %s cannot be created.\n", file);
        return EXIT_FAILURE;
    }

    if (fwrite(output, size_bytes, 1, fp) != 1)
        printf("Error writing file %s.\n", file);

    fclose(fp);

    free(file);
    free(output);
    free(Oi);

    return EXIT_SUCCESS;
}
