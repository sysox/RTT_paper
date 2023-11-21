//
// Created by user on 06/11/2023.
//
#include <endian.h>
#include <math.h>
#include "generators.h"
#include "markov_chain_RNG.h"

double** allocate(int dim){
    double **array;
    int i;

    array = (double **) malloc(dim*sizeof(double *));
    for(i = 0; i < dim; i++){
        array[i] = (double *) malloc(dim*sizeof(double));
    }
    return array;
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
    int i, j;
    printf("\n");
    for(i = 0; i < dim; i++){
        print_vec(T[i], dim);
    }
    printf("\n");
    fflush(stdin);
}
void histfreqs_to_probs(const long long int* hist_freqs, int hist_size, long long int freqs_sum, double *Oi_probs){
    int num_bins, i;
    for(i = 0; i < num_bins; i++)
    {
        Oi_probs[i] =  1.0 *hist_freqs[i] / freqs_sum;
    }
}


void  stochastic_matrix(double *x_probs, int dim, double **T){
    //    return T  dim x dim (same as p_vec) such that
    //    1. \sum_j T_{i,j} = 1 for arbitrary row i
    //    2. xT = x
    //    3. 0 <= T_{i,j} <= 1,
    int i,j,k,l, iter;
    float  minimum, maximum, tmp, xji_ratio, delta1, delta2, sum;
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
    for(iter = 0; iter < 20; iter++){
        i = xorshift32() % dim;
        j = (i + xorshift32() % (dim - 1) + 1) % dim;     // random but different than j
        if ( (x_probs[i] == 0) || (x_probs[j] == 0))
        {
//            printf("%f,%f ", x_probs[i], x_probs[j]);
            continue;
        }

        k = xorshift32() % dim;
        l = (k + xorshift32() % (dim - 1) + 1) % dim;      // random but different than k

        k = i;
        l = j;

//        *      max(-T_{i, k}, -1 +T_{i, l})       <= delta_1 <= min(T_{i, l}, 1-T_{i, k})
//        *      -x_j/x_i*min(T_{j, l}, 1-T_{j, k}) <= delta_1 <= -x_j/x_i*max(-T_{j, k}, -1 +T_{j, l})
        xji_ratio = -x_probs[j] / x_probs[i];
        minimum = fmax(-T[i][k], -1 + T[i][l]);
        tmp = fmin(T[j][l], 1 - T[j][k]);
        minimum = fmax(minimum, xji_ratio * tmp);

        maximum = fmin(T[i][l], 1 - T[i][k]);
        tmp = fmax(-T[j][k], -1 + T[j][l]);
        maximum = fmin(maximum, xji_ratio*tmp);

        if (maximum - minimum < 0.00001){
            continue;
        }
        delta1 = rand_double_range(minimum, maximum, 1000);
        delta2 = delta1 / xji_ratio;

        T[i][k] += delta1;
        T[j][k] += delta2;
        T[i][l] -= delta1;
        T[j][l] -= delta2;
    }

    //    print_vec(x_probs, dim);
    print_mat(T, dim);

    //test xT ~ x
//    for(i = 0; i < dim; i++){
//        sum = 0;
//        for(j= 0; j < dim; j++){
//            sum += x_probs[i]*T[i][j];
//        }
//        if (fabs(sum - x_probs[i]) > 0.001 ){
//            printf("%f != %f, column = %i \n", sum, x_probs[i], i);
//        }
//    }
}