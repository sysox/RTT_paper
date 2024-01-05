from scipy.stats import chi2

def chi2_critical_value(alpha, num_bins = 256):
    return chi2.isf(alpha, df=num_bins-1, loc=0, scale=1)

block_bit_size = 8
num_blocks = 400*1024*1024 / block_bit_size


# print(chi2_critical_value(0.01, 2))
alphas = [10**(-i) for i in range(1, 6)]
chi2_critical_values = [chi2_critical_value(alpha) for alpha in alphas]

num_swaps = [round(num_blocks*i/10) for i in range(11)]

for chi2 in chi2_critical_values:
    print(f"{round(chi2, 2)}")
