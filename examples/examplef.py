# Released under Apache 2.0; refer to LICENSE.txt

from fldr import fldrf_preprocess
from fldr import fldr_sample

N_sample = 100
distribution = [0.25, 0.13, 1.12]
x = fldrf_preprocess(distribution)
samples = [fldr_sample(x) for _i in range(N_sample)]
print(' '.join(map(str, samples)))
