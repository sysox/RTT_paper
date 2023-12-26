//
// Created by user on 09/11/2023.
//
#include "generators.h"


///////////////////////////////////////// utils /////////////////////////////////////////
void print_bitarray(unsigned char* array, uint32_t size){
    int i, j;
    for(i = 0; i < size; i++){
        for(j = 0; j < 8; j++){
            printf("%i", (array[i] >> j) & 1 );
        }
        printf("|");
    }
    printf("\n");
}
void print_array(uint32_t* array, uint32_t size){
    int i, j;
    for(i = 0; i < size; i++){
        printf("%i ",array[i]);
    }
    printf("\n");
}
void swap(uint32_t* a, uint32_t* b){
    uint32_t tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}


///////////////////////////////////////// XOR shift generators /////////////////////////////////////////
uint32_t xorshift32_state = 1;
/* The state must be initialized to non-zero */
void seed_xorshift32(uint32_t seed){
    time_t t;
    if (seed == 0){
        seed = time(NULL);
    }
    xorshift32_state = seed;
}
uint32_t xorshift32()
{
    /* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
    xorshift32_state ^= xorshift32_state << 13;
    xorshift32_state ^= xorshift32_state >> 17;
    xorshift32_state ^= xorshift32_state << 5;
    return xorshift32_state;
}

uint64_t xorshift64_state = 1;
/* The state must be initialized to non-zero ??? */
void seed_xorshift64(uint64_t seed){
    time_t t;
    if (seed == 0){
        seed = time(NULL);
    }
    xorshift64_state = seed;
}
uint64_t xorshift64()
{
    xorshift64_state ^= xorshift64_state << 13;
    xorshift64_state ^= xorshift64_state >> 7;
    xorshift64_state ^= xorshift64_state << 17;
    return xorshift64_state;
}

/* struct xorshift128_state can alternatively be defined as a pair
   of uint64_t or a uint128_t where supported */
uint32_t xorshift128_state[4] = {1, 1, 1, 1 };
void seed_xorshift128(uint64_t seed){
    time_t t;
    if (seed == 0){
        seed = time(NULL);
    }
    xorshift128_state[3] = seed;
    xorshift128_state[2] = (seed >> 32);
}
/* The state must be initialized to non-zero */
uint32_t xorshift128()
{
    /* Algorithm "xor128" from p. 5 of Marsaglia, "Xorshift RNGs" */
    uint32_t t;
    t = (xorshift128_state[0]^(xorshift128_state[0]<<11));
    xorshift128_state[0] = xorshift128_state[1];
    xorshift128_state[1] = xorshift128_state[2];
    xorshift128_state[2] = xorshift128_state[3];
    xorshift128_state[3] = (xorshift128_state[3]^(xorshift128_state[3]>>19))^(t^(t>>8));
    return xorshift128_state[3];
}


/////////////////////////////////////////  common generators (single random value) /////////////////////////////////////////
uint64_t rand_range(uint64_t a, uint64_t b){
    // returns integer values in [a, b) i.e. except b
    uint64_t mod ;
    mod = b - a;
    return xorshift64() % mod + a;
}
uint64_t rand_whole_range(uint64_t a, uint64_t b){
    // returns integer values in [a, b] i.e. including b
    uint64_t mod ;
    mod = b - a + 1;
    return xorshift64() % mod + a;
}
double rand_double_range(double a, double b, uint64_t pool_size, uint64_t zoom){
    // returns double values in [a, b)
    // pool_size - number of doubles possibly generated from a, b
    // zoom - integer is generated from zoomed interval and then values is transformed back (zoom out)

    int mod;
    double r;
    uint64_t A, B;

    // [a, b) is mapped to [A, B) with size poolsize i.e. B-1-A = pool_size
    // B-1-A = b*zoom - a*zoom - 1 = pool_size      =>   zoom = (pool_size + 1)/ (b-a)
    if (zoom <= 0){
        zoom = (pool_size + 1)/ (b-a);
    }

    A = a*zoom;
    B = b*zoom;
    r = 1.0 * rand_range(0, B - A) / zoom;
    return r + a;
}
int multinomial_lincom(double* probs, int size, uint64_t scale_factor, uint64_t zoom){
    // for array of probabilities (with sum = 1) of some events
    // return integer (index) of some event
    // the algorithm generate value r from [0, 1] and
    // returns index i such that sum_{j=0}^{i-1} <= r <= sum_{j=0}^{i}

    int i = 0;
    float r;
    r = rand_double_range(0, 1, scale_factor, zoom);
    for(i = 0; i < size; i++) {
        if ( (r -= probs[i]) < 0){
            return i;
        }
    }
    return 0;
}
/////////////////////////////////////////  common generators(more random values) /////////////////////////////////////////
void concatenate(const uint32_t* values, uint32_t num_values, int value_bit_size, unsigned char* output){
    // takes 32 bit values and  concate them (value_bit_size least significant bits) to output
    // values[] = {2, 1, 5, 7, ... } value_bit_size = 3
    // will return bytes that represent stream of bits |010|100|101|111|...
    int i, j, freq, bits_written, byte_offset, byte_shift, out_byte_size;
    uint32_t *write_ptr;
    uint32_t block_value_le;

    bits_written = 0;
    out_byte_size = ceil(1.0*value_bit_size*num_values/8);
    memset(output, 0, out_byte_size + 3);
    for(i = 0; i < num_values; i++)
    {
        block_value_le = values[i];
        byte_offset = bits_written >> 3; // equivalent to bits_written / 8
        byte_shift = bits_written & 7;  // equivalent to bits_written % 8
        write_ptr = (uint32_t*)(output + byte_offset);
        write_ptr[0] ^= block_value_le << byte_shift;
        bits_written += value_bit_size;
    }
//    print_bitarray(output, out_byte_size + 2);
}
void multinomial_clusters(const uint32_t* hist_freqs, const uint32_t* hist_values,
                          uint32_t hist_size, uint32_t* output_values) {
    // allocated memory (output) must be 3 bytes longer than necessary!!!
    // create clusters of repeated vallues
    // hist_freqs[] = {2, 3, 1, ...} hist_values[] = {0, 1, 2, ...}
    // will fill output_values with 0, 0, 1, 1, 1, 2, ...

    int i, j, freq;
    uint32_t *write_ptr;
    uint32_t block_value_le;

    write_ptr = output_values;
    for (i = 0; i < hist_size; i++) {
        freq = hist_freqs[i];
        block_value_le = hist_values[i];
        for (j = 0; j < freq; j++) {
            write_ptr[0] = block_value_le;
            write_ptr++;
        }
    }
}
void shuffling(uint32_t* values, uint32_t num_values, uint32_t num_swaps){
    unsigned int idx1, idx2;
    uint32_t i;
    uint32_t tmp;

    //suffling - swaps
    for(i = 0; i < num_swaps; i++)
    {
        idx1 = xorshift64() % num_values;
        idx2 = xorshift64() % num_values;
        swap(values + idx1, values + idx2);
    }
}
void random_sample(const uint32_t* values, uint32_t num_values, uint32_t* sample, int sample_size){
    unsigned int idx;
    uint32_t i;

    for (i = 0; i < sample_size; i++) {
        idx = xorshift64() % num_values;                    // random block from
        sample[i] = values[idx];
    }
}

void multinomial(const uint32_t* hist_freqs, const uint32_t* hist_values, int hist_size,
                 int value_bit_size, unsigned char* output, int num_values, int num_swaps){
    int i, num_hist_values;
    uint32_t *values, *sample;

    num_hist_values = 0;
    for(i = 0; i < hist_size; i++){
        num_hist_values += hist_freqs[i];
    }

    values = (uint32_t*) malloc(num_values*sizeof(uint32_t));
    if (num_swaps > 0) {
        multinomial_clusters(hist_freqs, hist_values, hist_size, values);
        shuffling(values, num_values, num_swaps);
//        print_array(values, num_hist_values);
        concatenate(values, num_values, value_bit_size, output);
    } else{
        sample = (uint32_t*) malloc(num_values*sizeof(uint32_t));
        multinomial_clusters(hist_freqs, hist_values, hist_size, values);
//        print_array(values, num_hist_values);
        random_sample(values, num_hist_values, sample, num_values);
//        print_array(sample, num_hist_values);
        concatenate(sample, num_values, value_bit_size, output);
//        free( (void*) sample);
    }

    free((void *)values);
}

/////////////////////////////////////////  biased RNG /////////////////////////////////////////

double chi2(double Ei, int num_bins, uint32_t *Oi){
    int i;
    double res;
    res = 0;
    for(i = 0; i < num_bins; i++){
        res += (Ei - Oi[i])*(Ei - Oi[i]);
    }
    res = res / Ei;
    return res;
}

void Chi2_to_freqs(double chi2stat, int hist_size, uint32_t freq_sum, uint32_t* Oi_freqs){
    // (O_i - E_i)^2/ E_i = n(pi - p_expected)^2/p_expected
    double Ei, chi2_stat, current_value, tmp;
    int num_bins, i, floor_Ei, freq, idx1, idx2;
    uint32_t sum, n;
    uint32_t* Oi;


    num_bins = hist_size;
    n = freq_sum;
    Oi = Oi_freqs;

    memset(Oi, 0, num_bins*sizeof(*Oi_freqs));
    Ei = 1.0*n/num_bins;

    // basic initial setting with minimum Chi2stat
    floor_Ei = (int)Ei;
    freq = 1.0*(Ei - floor_Ei)*num_bins;
    chi2_stat = 0;
    for(i = 0; i < freq; i++ ){
        Oi[i] =  floor_Ei + 1;
    }
    for(i = freq; i < num_bins; i++ ){
        Oi[i] =  floor_Ei ;
    }

    chi2_stat += ( floor_Ei + 1 - Ei )*( floor_Ei + 1 - Ei )/Ei*freq;
    chi2_stat += ( floor_Ei - Ei)*( floor_Ei - Ei)/Ei*(num_bins-freq);

    //testing whether sum of Oi == number of blocks!
    sum = 0;
    for(i = 0; i < num_bins; i++)
    {
        sum += Oi[i];
    }
    if (sum != n){
        Oi[0] += n - sum;
//        printf("%llu != number of blocks %llu\n", sum, num_blocks);
    }

    if (chi2_stat > chi2stat){
        return;
    }
    while(chi2_stat < chi2stat){
        idx1 = xorshift32() % num_bins;
        idx2 = xorshift32() % num_bins;
//        printf("%i %i\n", idx1, idx2);
        current_value = ( Oi[idx1] - Ei )*(  Oi[idx1] - Ei )/Ei + ( Oi[idx2] - Ei )*(  Oi[idx2] - Ei )/Ei;
        tmp = ( Oi[idx1] - 1 - Ei )*(  Oi[idx1] - 1 - Ei )/Ei + ( Oi[idx2] + 1 - Ei )*(  Oi[idx2] + 1 - Ei )/Ei;
        if(tmp > current_value)
        {
            if ((Oi[idx1] <= 0) || (Oi[idx2] >= n))
                continue;
            Oi[idx1]--;
            Oi[idx2]++;
            chi2_stat += tmp - current_value;
//            printf("%f vs recomputed =%f\n", chi2_stat, chi2(Ei, num_bins, Oi));
        }
        else{
            tmp = ( Oi[idx1] + 1 - Ei )*(  Oi[idx1] + 1 - Ei )/Ei + ( Oi[idx2] - 1 - Ei )*(  Oi[idx2] - 1 - Ei )/Ei;
            if(tmp > current_value)
            {
                if ((Oi[idx2] <= 0) || (Oi[idx1] >= n))
                    continue;
                Oi[idx1]++;
                Oi[idx2]--;
                chi2_stat += tmp - current_value;
//                printf("%f vs recomputed =%f\n", chi2_stat, chi2(Ei, num_bins, Oi));
            }
        }

    }


    //testing whether sum of Oi == number of blocks!
    sum = 0;
    for(i = 0; i < num_bins; i++)
    {
        sum += Oi[i];
//        printf("%lli ", Oi[i]);
    }
    if (sum != n){
        printf("%u != number of blocks", sum);
    }
    printf("%f vs recomputed =%f\n\n", chi2_stat, chi2(Ei, num_bins, Oi));

}



//float* array_from_interval(float a, float b, unsigned long long map_range, int (*RNG) (void), float* array, int size){
//    int mod, i;
//    float r;
//    if (map_range <= 0){
//        map_range = -1;
//    }
//    mod = (int)((b - a)*map_range);
//    for(i = 0; i < size; i++)
//    {
//        r = 1.0 * (RNG() % mod) / map_range;
//        r += a;
//        array[i] = r;
//    }
//    return array;
//}




