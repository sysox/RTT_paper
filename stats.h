//
// Created by user on 20/10/2023.
//

#ifndef RTT_UNIFORMITY_TESTING_STATS_H
#define RTT_UNIFORMITY_TESTING_STATS_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

void chi2_stats(double *pvalues, int num_pvals, int sample_size, int number_of_samples, int num_bins, double *res_stats);
void KS_stats(double *pvalues, int num_pvals, int sample_size, int number_of_samples, double *res_stats);
int load_file(const char* filename, double* pvalues);
void load_file2(const char* filename, double* pvalues, int num_pvalues);

#endif //RTT_UNIFORMITY_TESTING_STATS_H
