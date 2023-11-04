//
// Created by user on 20/10/2023.
//
#include "stats.h"


/////////////////////////////////////////////////      sampling      /////////////////////////////////////////////////
//https://learncgames.com/a-fast-random-number-generator-in-c/
uint64_t rngstate[4] = {0,1,2,3};
static inline uint64_t rotl(const uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}

// Returns a Uint64 random number => changed to Uint32
uint32_t next(void) {
    const uint64_t result = rotl(rngstate[0] + rngstate[3], 23) + rngstate[0];
    const uint64_t t = rngstate[1] << 17;
    rngstate[2] ^= rngstate[0];
    rngstate[3] ^= rngstate[1];
    rngstate[1] ^= rngstate[2];
    rngstate[0] ^= rngstate[3];
    rngstate[2] ^= t;
    rngstate[3] = rotl(rngstate[3], 45);
    return (uint32_t)result;                    //changed
}

int load_file(const char* filename, double* pvalues)
{
    FILE *file;
    int count;
    double *tmp_ptr;

    file = fopen(filename,"r");
    tmp_ptr = pvalues;
    printf("%s file is being loaded ", filename);
    count = 0;
    while(!feof(file))
    {
        if (fscanf(file, "%lf", tmp_ptr) != 1){
            printf("%d-th value not read\n", count+1);
            break;
        }

        if (tmp_ptr[0] < 0 || tmp_ptr[0] > 1)
            printf("Value %lf out of range.", tmp_ptr[0]);
//        printf("%lf", *tmp_ptr);
        tmp_ptr++;
        count++;
    }
//    printf("file loaded, %d values\n", count);
    return count;
}


double str_to_double(unsigned char* str){
    int res, i;
    res = 0;
    for(i = 0; i < 8; i++){
        res *= 10;
        res += (str[i] - '0');
    }
    return res/100000000.0;
}
void load_file2(const char* filename, double* pvalues, int num_pvalues)
{
    FILE *file;
    void* tmp_array;
    int file_size, block_size, i, elements_read;
    unsigned char* ptr;

    file = fopen(filename,"rb");
    block_size = (8+2+1);
    file_size = block_size*num_pvalues;
    tmp_array = (void *) malloc(file_size);
    elements_read = fread(tmp_array, block_size, num_pvalues, file);
    if ((unsigned int)elements_read != (unsigned int)num_pvalues){
        printf("file not read completely, %d != %d ", elements_read, num_pvalues);
    }
    ptr = tmp_array;
    ptr += 2; //move behind dot 0.00046126
    for(i =0; i < num_pvalues; i++)
    {
        pvalues[i] = str_to_double(ptr);
        ptr += block_size;
    }
    free((void *) tmp_array);
}

void random_sample(double* sample, int sample_size, double *pvalues, int num_pvals)
{
    int i, idx;
    for(i = 0; i < sample_size; i++)
    {
        idx = next() % num_pvals;
        sample[i] = pvalues[idx];
    }
}

/////////////////////////////////////////////////       KS      /////////////////////////////////////////////////

int compare_doubles(const void* a, const void* b)
{
    double arg1 = *(const double*)a;
    double arg2 = *(const double*)b;

    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;

    // return (arg1 > arg2) - (arg1 < arg2); // possible shortcut
    // return arg1 - arg2; // erroneous shortcut (fails if INT_MIN is present)
}
void gsl_sort(double* sample,  int size){
    qsort(sample, size, sizeof(double), compare_doubles);
}

double ks_test(double *sample, int size)
{

    int i;
    double y, d1, d2, d_max, step;

    gsl_sort(sample, size);

    d_max = 0.0;
    step = 1.0 / (size + 1);

    /*
     for 1...n
        D1 =   i  /n  - F(i)
        D2 = (i-1)/n  - F(i)

     n = 3
        D1:   none     , 1/3 - F[1], 2/3 - F[2], 3/3 - F[3] ()
        D2:  0/3 - F[1], 1/3 - F[2], 3/3 - F[3],    none

     index shift starting from 0:
        n = 3
        D1:   none     , 1/3 - F[0], 2/3 - F[1], 3/3 - F[2] ()
        D2:  0/3 - F[0], 1/3 - F[1], 3/3 - F[2],    none
    */

    //2 Extreme cases with only one computation D1 or D2 (none)
    y = 1;
    d2 = fabs(sample[size-1] - y);
    y = 0;
    d1 = fabs(sample[0] - y);
    d_max = (d1 > d2) ? d1 : d2;

    // rest
    for (i = 1; i < size; i++) {
        y += step;
        d1 = fabs(sample[i - 1] - y);
        d2 = fabs(sample[i] - y);
        d_max = (d_max > d1) ? d_max : d1;
        d_max = (d_max > d2) ? d_max : d2;

    }
    return d_max;
}

void KS_stats(double *pvalues, int num_pvals, int sample_size, int number_of_samples, double *res_stats)
{
    int i;
    double *sample;
//    pvalues = (double*)malloc(sizeof(double) * num_pvals);
    sample = (double*)malloc(sizeof(double) * sample_size);
//    res_stats = (double*)malloc(sizeof(double) * number_of_samples);

    for(i = 0; i < number_of_samples; i++)
    {
        random_sample(sample, sample_size, pvalues, num_pvals);
        res_stats[i] = ks_test(sample, sample_size);
    }
    free((void*)sample);
}
/////////////////////////////////////////////////       Chi2      /////////////////////////////////////////////////
double chi2_test(double *sample, int sample_size, int num_bins, double* Oi, int Ei)
{
    int i, idx;
    double  stat, bin_size;
    // bin frequency Oi computation
    bin_size = 1.0 / num_bins;
//    memset(Oi, 0, num_bins*sizeof(double));
    for(i = 0; i < num_bins; i++){
        Oi[i] = 0;
    }
    for(i = 0; i < sample_size; i++)
    {
        if (sample[i] == 1){
            idx = num_bins - 1;
        }
        else{
            idx = (int)(sample[i]/bin_size);
        }
        if (idx < 0 || idx >= num_bins)
        {
            printf("idx %d out of range", idx);
        }
        Oi[idx]++;
    }
    //stat computation
    stat  = 0;
    for(i = 0; i < num_bins; i++)
    {
        stat += (Oi[i] - Ei)*(Oi[i] - Ei);
    }
    stat = stat / Ei;

    return stat;
}

void chi2_stats(double *pvalues, int num_pvals, int sample_size, int number_of_samples, int num_bins, double *res_stats)
{
    int i;
    double *sample, Ei, *Oi;
    sample = (double*)malloc(sizeof(double) * sample_size);
    Oi = (double *)malloc(sizeof(double) * num_bins);
    Ei = 1.0 * sample_size / num_bins;
//    res_stats = (double*)malloc(sizeof(double) * number_of_samples);

    for(i = 0; i < number_of_samples; i++)
    {
        random_sample(sample, sample_size, pvalues, num_pvals);
        res_stats[i] = chi2_test(sample, sample_size, num_bins, Oi, Ei);
    }
    free((void*)sample);
    free((void*)Oi);
}