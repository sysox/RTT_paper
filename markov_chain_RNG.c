//
// Created by user on 06/11/2023.
//
#include <endian.h>
#include <math.h>
#include "generators.h"
#include "markov_chain_RNG.h"

static double** double_array_alloc(int dim){
    double **array;
    int i;

    array = (double **) malloc(dim*sizeof(double *));
    for(i = 0; i < dim; i++){
        array[i] = (double *) malloc(dim*sizeof(double));
    }
    return array;
}

static void double_array_free(double **array, int dim){
    for(int i = 0; i < dim; i++){
        free(array[i]);
    }
    free(array);
}

void print_vec(double * T, int dim){
    int i;
    for(i = 0; i < dim; i++){
        printf("%f ", T[i]);
    }
    printf("\n");
    fflush(stdin);
}
void print_mat(double ** T, int dim){
    int i;
    printf("\n");
    for(i = 0; i < dim; i++){
        print_vec(T[i], dim);
    }
    printf("\n");
    fflush(stdin);
}
void histfreqs_to_probs(const uint32_t* hist_freqs, unsigned int hist_size, uint32_t freqs_sum, double *Oi_probs){
    unsigned int i;
    for(i = 0; i < hist_size; i++)
    {
        Oi_probs[i] =  1.0 *hist_freqs[i] / freqs_sum;
    }
}
void stochastic_matrix(double *stationary_probability_vec, unsigned int dim, double **stochastic_mat, unsigned int num_iters){
    //    return T  dim x dim (same as p_vec) such that
    //    T - stochastic matrix, x_probs - stationary_probability_vec
    //    1. \sum_j T_{i,j} = 1 for arbitrary row i
    //    2. xT = x
    //    3. 0 <= T_{i,j} <= 1,
    unsigned int i,j,k,l, iter;
    float  minimum, maximum, tmp, xji_ratio, delta1, delta2, sum;
    double **T, *x_probs;
    T = stochastic_mat;
    x_probs = stationary_probability_vec;
    // initialize
    for(i = 0; i < dim; i++){
        for(j = 0; j < dim; j++){
            T[i][j] = 0;
        }
        T[i][i] = 1;
    }

    // iterative change
    // 4 elements ,
    /*
     *           k           l
     *    i   delta_1    -delta_1
     *
     *    j   delta_2    -delta_2
     *
     *  1.  row sums not changed!
     *  2. x_i*delta_1 + x_j*delta_2 = 0
     *      delta_2 = -delta_1*x_i/x_j  - OK
     *
     *  3.  0 <=  delta_1 + T_{i, k} <= 1   =>    -T_{i, k} <=  delta_1  <= 1 -T_{i, k}
     *      0 <=  delta_2 + T_{j, k} <= 1   =>    -T_{j, k} <=  delta_2  <= 1 -T_{j, k}
     *      0 <= -delta_1 + T_{i, l} <= 1   =>  -1 +T_{i, l} <=  delta_1  <=    T_{i, l}
     *      0 <= -delta_2 + T_{j, l} <= 1   =>  -1 +T_{j, l} <=  delta_2  <=    T_{j, l}                - OK
     *  =>
     *      max(-T_{i, k}, -1 +T_{i, l}) <= delta_1 <= min(T_{i, l}, 1-T_{i, k})
     *      max(-T_{j, k}, -1 +T_{j, l}) <= delta_2 <= min(T_{j, l}, 1-T_{j, k}) =>                     - OK
     *
     *      max(-T_{j, k}, -1 +T_{j, l})<= -delta_1*x_i/x_j <= min(T_{j, l}, 1-T_{j, k}) =>
     *      -x_j/x_i*min(T_{j, l}, 1-T_{j, k}) <= delta_1 <= -x_j/x_i*max(-T_{j, k}, -1 +T_{j, l})      - OK
     *
     *      max(-T_{i, k}, -1 +T_{i, l})       <= delta_1 <= min(T_{i, l}, 1-T_{i, k})
     *      -x_j/x_i*min(T_{j, l}, 1-T_{j, k}) <= delta_1 <= -x_j/x_i*max(-T_{j, k}, -1 +T_{j, l})      - OK
     */
    for(iter = 0; iter < num_iters; iter++){
        k = i = xorshift32() % dim;
        l = j = (i + xorshift32() % (dim - 1) + 1) % dim;     // random but different than i
        if ( (x_probs[i] == 0) || (x_probs[j] == 0))
        {
//            printf("%f,%f ", x_probs[i], x_probs[j]);
            continue;
        }



//        *      max(-T_{i, k}, -1 +T_{i, l})       <= delta_1 <= min(T_{i, l}, 1-T_{i, k})
//        *      -x_j/x_i*min(T_{j, l}, 1-T_{j, k}) <= delta_1 <= -x_j/x_i*max(-T_{j, k}, -1 +T_{j, l})
        xji_ratio = -x_probs[j] / x_probs[i];
        minimum = fmax(-T[i][k], -1 + T[i][l]);
        tmp = fmin(T[j][l], 1 - T[j][k]);
        minimum = fmax(minimum, xji_ratio * tmp);

        maximum = fmin(T[i][l], 1 - T[i][k]);
        tmp = fmax(-T[j][k], -1 + T[j][l]);
        maximum = fmin(maximum, xji_ratio*tmp);

        if (maximum - minimum < 0.0001){
            continue;
        }
        delta1 = rand_double_range(minimum, maximum, dim*10000);
        delta2 = delta1 / xji_ratio;

        T[i][k] += delta1;
        T[j][k] += delta2;
        T[i][l] -= delta1;
        T[j][l] -= delta2;
    }

//    print_vec(x_probs, dim);
//    print_mat(T, dim);

    //test xT ~ x
    for(i = 0; i < dim; i++){
        sum = 0;
        for(j= 0; j < dim; j++){
            sum += x_probs[i]*T[i][j];
        }
        if (fabs(sum - x_probs[i]) > 0.001 ){
            printf("%f != %f, column = %i \n", sum, x_probs[i], i);
        }
    }
}
void markov_chain_seq(double **stochastic_mat, unsigned int dim, const uint32_t* state_values,
                      uint32_t seq_size, uint32_t* output_chain_values, uint64_t scale_factor){

    uint32_t index, i;
    index = xorshift32() % dim;

    for(i = 0; i < seq_size; i++){
        output_chain_values[i] = state_values[index];
        index = multinomial_lincom_probs(stochastic_mat[index], dim, scale_factor);
    }
}

static double chi2_from_data(unsigned int num_bins, uint32_t num_values, uint32_t* values){
    double chi2_stat;
    uint32_t* Oi;
    uint32_t i,idx, Ei;
    Ei = 1.0*num_values/num_bins;

    Oi = calloc(sizeof(uint32_t), num_bins);
    for(i = 0; i < num_values; i++){
        idx = values[i];
        Oi[idx]++;
    }

    chi2_stat = 0;
    for (i = 0; i < num_bins; ++i) {
        chi2_stat += ((double)Oi[i] - Ei)*((double)Oi[i] - Ei)/Ei;
    }
    free(Oi);
    return chi2_stat;
}

void Chi2_MC(double chi2stat, unsigned int value_bit_size, uint32_t chain_size, unsigned char* output){

    uint32_t* Oi_freqs;
    int dim, num_iters, i;
    double *Oi_probs, **stochastic_mat, chi2_final;
    uint64_t scale_factor;
    uint32_t *output_chain_values, *state_values;

    dim = 1 << value_bit_size;

    stochastic_mat = double_array_alloc(dim);
    Oi_freqs = calloc(sizeof(uint32_t), dim);
    Oi_probs = calloc(sizeof(double), dim);
    state_values = calloc(sizeof(uint32_t), dim);
    output_chain_values = calloc(sizeof(uint32_t), chain_size);

    Chi2_to_freqs(chi2stat, dim, chain_size, Oi_freqs);
    histfreqs_to_probs(Oi_freqs, dim, chain_size, Oi_probs);

    num_iters = 10*dim*dim;
    stochastic_matrix(Oi_probs, dim, stochastic_mat, num_iters);

    scale_factor = dim*10000;
    for(i = 0; i < dim; i++) {
        state_values[i] = i;
    }

    markov_chain_seq(stochastic_mat, dim, state_values, chain_size, output_chain_values, scale_factor);

    //test chi2
    chi2_final = chi2_from_data(dim, chain_size,output_chain_values);
    printf("chi2 of data generated using Markov process %f\n", chi2_final);
    concatenate(output_chain_values, chain_size, value_bit_size, output);

    double_array_free(stochastic_mat, dim);
    free(Oi_freqs);
    free(Oi_probs);
    free(state_values);
    free(output_chain_values);
}
