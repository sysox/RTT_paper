from scipy.stats import chi2

def chi2_critical_value(alpha, num_bins = 256):
    return chi2.isf(alpha, df=num_bins-1, loc=0, scale=1)

block_bit_size = 8
num_blocks = 400*1024*1024 / block_bit_size

# print(chi2_critical_value(0.01, 2))
alphas_small = {10**(-i):chi2_critical_value(10**(-i)) for i in range(1, 6)}
alphas_large = {1-10**(-i): chi2_critical_value(1 - 10**(-i))  for i in range(1, 6)}
alphas = {}
alphas.update(alphas_small)
alphas.update(alphas_large)
alphas[0.5] = chi2_critical_value(0.5)

# num_swaps = [round(num_blocks*i/10) for i in range(11)]

for alpha, chi2  in alphas.items():
    print(f"alpha={alpha}:chi2={round(chi2, 2)} ")
