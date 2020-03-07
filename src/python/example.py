# Released under Apache 2.0; refer to LICENSE.txt

from fldr import fldr_preprocess
from fldr import fldr_sample

N_sample = 100
distribution = [1, 1, 2, 3, 1]
x = fldr_preprocess(distribution)
samples = [fldr_sample(x) for _i in range(N_sample)]
print(' '.join(map(str, samples)))
