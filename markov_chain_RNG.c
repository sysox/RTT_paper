//
// Created by user on 06/11/2023.
//
#include <endian.h>
#include <math.h>
#include "biased_gen.h"


float max(float v1, float v2){
    if (v1 >= v2 ){
        return v1;
    }
    else{
        return v2;
    }
}
float min(float v1, float v2){
    if (v1 <= v2 ){
        return v1;
    }
    else{
        return v2;
    }
}

float** allocate(int dim){

    float **array;
    int i;
    get_num_bins(2);
    array = (float **) malloc(dim*sizeof(float *));
    for(i = 0; i < dim; i++){
        array[i] = (float *) malloc(dim*sizeof(float));
    }
    return array;
}


void print_vec(float* T, int dim){
    int i;
    for(i = 0; i < dim; i++){
        printf("%f ", T[i]);
    }
    printf("\n");
    fflush(stdin);
}
void print_mat(float** T, int dim){
    int i, j;
    printf("\n");
    for(i = 0; i < dim; i++){
        print_vec(T[i], dim);
    }
    printf("\n");
    fflush(stdin);
}
void gen_global_probs(double req_chi2stat, int num_freqs, unsigned long long num_blocks, unsigned long long* Oi_freqs, float *Oi_probs){
    int num_bins, i;
    num_bins = num_freqs;
    gen_freqs(req_chi2stat, num_freqs, num_blocks, Oi_freqs);
    for(i = 0; i < num_bins; i++)
    {
        Oi_probs[i] = 1.0 * Oi_freqs[i] / num_blocks;
    }
}

//void  stochastic_matrix_metropolis(float *p_vec, int dim, float **T){
//    //     (after infinite steps probability of states are defined by to p_vec)
//    //     stochastic matrix T of Markov process (1st order)  such that for probability vector x holds p_vec*T = p_vec
//    //     transition matrix T (T_{i,j} = probability of transition from state i to state j)
//    //      - square matrix of dimension dim (same as p_vec)
//    //      -  rows of T = positive floats with sum = 1
//    //      metropolis - hasting alg is used below to find transition matrix T (for given p_vec)
//    //      for row matrix T: T_{i,j} = T_{i,j} min(1, T_{j,i}*p_j/T_{i,j}*p_i)  for i != j
//    //                        T_{i,i} = 1 - sum (others)
//    int i, j, k, idx1, idx2, mod, rest;
//    float init_value, **T_tmp, tmp, sum;
//
//    //allocation of tmp matrix used that will store stochastic/transition matrix from the previous iter
//    T_tmp = allocate(dim);
//
//    // basic initial (uniform) transition matrix
//    //approach 1
////    init_value = 1.0 / dim;
////    for(i = 0; i < dim; i++){
////        for(j = 0; j < dim; j++){
////            T_tmp[i][j] = init_value;
////        }
////    }
//      //approach 2
////    mod = (1 << 10);
////    for(i = 0; i < dim; i++){
////        sum = 0;
////        for(j = 0; j < dim; j++){
////            T_tmp[i][j] = next() % mod;
////            sum += T_tmp[i][j];
////        }
////        for(j = 0; j < dim; j++){
////            T_tmp[i][j] /= sum;
////        }
////
////    }
//    //approach 3
//
////    mod = (1 << 10);
////    for(i = 0; i < dim; i++){
////        sum = 0;
////        for(j = 0; j < dim; j++){
////            T_tmp[i][j] = (1 << j) + (1 << i);
////            sum += T_tmp[i][j];
////        }
////        for(j = 0; j < dim; j++){
////            T_tmp[i][j] /= sum;
////        }
////
////    }
//
//
//    print_array(T_tmp, dim);
//
//    for(k = 0; k < 1; k++)  {
//        for (i = 0; i < dim; i++) {
//            T[i][i] = 1;
//            // T_{i,j} = T_{i,j} min(1, T_{j,i}*p_j/T_{i,j}*p_i)  for i != j
//            for (j = 0; j < dim; j++) {
//                if (i == j) {
//                    continue;
//                }
//                else {
//                    tmp = T_tmp[j][i] * p_vec[j] / T_tmp[i][j] * p_vec[i];
//                    if (1 < tmp) {
//                        tmp = 1;
//                    }
//                    T[i][j] = T_tmp[i][j] * tmp;
//                    T[i][i] -= T[i][j];
//                }
//            }
//        }
//        memcpy(T_tmp, T, dim*dim*sizeof(float ));
////        print_array(T, dim);
//    }
//    print_array(T, dim);
//}


void  stochastic_matrix(float *x_probs, int dim, float **T){
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
    for(iter = 0; iter < 100; iter++){
        i = next() % dim;
        j = (i + next() % (dim - 1) + 1) % dim;     // random but different than j
        if ( (x_probs[i] == 0) || (x_probs[j] == 0))
        {
            printf("%f,%f ", x_probs[i], x_probs[j]);
            continue;
        }
        k = next() % dim;
        l = (k + next() % (dim - 1) + 1) % dim;      // random but different than k

//        *      max(-T_{i, k}, -1 +T_{i, l})       <= delta_1 <= min(T_{i, l}, 1-T_{i, k})
//        *      -x_j/x_i*min(T_{j, l}, 1-T_{j, k}) <= delta_1 <= -x_j/x_i*max(-T_{j, k}, -1 +T_{j, l})
        xji_ratio = -x_probs[j] / x_probs[i];
        minimum = max(-T[i][k], -1 + T[i][l]);
        tmp = min(T[j][l], 1 - T[j][k]);
        minimum = max(minimum, xji_ratio * tmp);

        maximum = min(T[i][l], 1 - T[i][k]);
        tmp = max(-T[j][k], -1 + T[j][l]);
        maximum = min(maximum, xji_ratio*tmp);

        if (maximum - minimum < 0.00001){
            continue;
        }
        delta1 = rand_interval(minimum, maximum, 1000);
        delta2 = delta1 / xji_ratio;

        T[i][k] += delta1;
        T[j][k] += delta2;
        T[i][l] -= delta1;
        T[j][l] -= delta2;
    }

    //    print_vec(x_probs, dim);
        print_mat(T, dim);

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