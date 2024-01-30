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
    printf("Use: %s --file=<output> [--size=<MBytes>] [--chi2=<num>] [--swaps=<num>] [--blocksize=<num>] [--seed=<num>] [--type=multinomial|mult_exact|mult_random|mc]\n", arg0);
    return EXIT_FAILURE;
}

int main(int argc, char *argv[]) {
    unsigned char *output;
    int i, block_bit_size, hist_size;
    char opt[129], val[129], *file, *end;
    enum { MULTINOMIAL, MULT_EXACT, MULT_RANDOM, MC } type;
    char *type_name[] = {"multinomial", "multinomial_exact", "multinomial_random", "mc"};
    unsigned long long size_bytes, num_blocks, num_swaps, seed;
    double chi2;
    FILE *fp;

    // defaults
    file = NULL;
    block_bit_size = 8;
    num_swaps = 0;
    chi2 = 1000.;
    size_bytes = 100 * 1024 *1024;
    seed = 3;
    type = MULTINOMIAL;

    for (i = 1; i < argc; i++) {
        if (sscanf(argv[i], "--%128[^=]=%128s", opt, val) != 2)
            return help(argv[0], NULL);

        if (!strcmp(opt, "file")) {
            file = strdup(val);
        } else if (!strcmp(opt, "type")) {
            if (!strcmp(val, "mc"))
                type = MC;
            else if (!strcmp(val, "multinomial"))
                type = MULTINOMIAL;
            else if (!strcmp(val, "mult_exact"))
                type = MULT_EXACT;
            else if (!strcmp(val, "mult_random"))
                type = MULT_RANDOM;
            else
                return help(argv[0], "Invalid type.");
        } else if (!strcmp(opt, "size")) {
            size_bytes = strtoull(val, &end, 10);
            if (*end || errno == ERANGE || size_bytes < 10 || size_bytes > 400)
                return help(argv[0], "Invalid file size.");
            size_bytes *= (1024 * 1024);
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

    if (block_bit_size < 1 || block_bit_size > 16)
        return help(argv[0], "Blocksize can be 1..16 only.");

    hist_size = 1 << block_bit_size;

    printf("PARAMS: file %s, type %s, size %llu, blocksize %i (bins %i), chi2 %f, swaps %llu, seed %llu\n",
           file, type_name[type], size_bytes, block_bit_size, hist_size, chi2, num_swaps, seed);

    num_blocks = (size_bytes * 8) / block_bit_size;

    output = malloc(size_bytes + 3);

    if (!output) {
        printf("Cannot allocate memory.\n");
        return EXIT_FAILURE;
    }

    seed_xorshift32(seed);
    seed_xorshift64(seed);

    if (type == MULTINOMIAL || type == MULT_EXACT || type == MULT_RANDOM) {
        uint32_t freqs[hist_size];
        uint32_t values[hist_size];
        Chi2_to_freqs(chi2, hist_size, num_blocks, freqs);

        for(i = 0; i < hist_size; i++)
            values[i] = i;

        if (type == MULTINOMIAL) {
            // num_swaps == 0 => no clusters
            // or exact frequencies but if num_swaps is small probably clusters of blocks
            multinomial(freqs, values, hist_size, block_bit_size, output, num_blocks, num_swaps);
        } else if (type == MULT_EXACT) {
            multinomial_exact(freqs, values, hist_size, block_bit_size, output, num_blocks);
        } else if (type == MULT_RANDOM) {
            multinomial_not_exact(freqs, values, hist_size, block_bit_size, output, num_blocks);
        }
    } else if (type == MC) {
        Chi2_MC(chi2, block_bit_size, num_blocks, output);
    } else
        abort();

    printf("Output chi2: %f\n", chi2_buffer(output, size_bytes, block_bit_size));

    //printf("Output chi2 [1/4]: %f\n\n",  chi2_buffer(output,                  size_bytes/4, block_bit_size));
    //printf("Output chi2 [2/4]: %f\n\n",  chi2_buffer(output+size_bytes/4,     size_bytes/4, block_bit_size));
    //printf("Output chi2 [3/4]: %f\n\n",  chi2_buffer(output+2*(size_bytes/4), size_bytes/4, block_bit_size));
    //printf("Output chi2 [4/4]: %f\n\n",  chi2_buffer(output+3*(size_bytes/4), size_bytes/4, block_bit_size));

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

    return EXIT_SUCCESS;
}
