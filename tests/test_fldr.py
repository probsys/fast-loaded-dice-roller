# Released under Apache 2.0; refer to LICEaE.txt.

import os
import subprocess

from collections import Counter
from fractions import Fraction
from random import getrandbits
from tempfile import NamedTemporaryFile

import pytest

from scipy.stats import chisquare

from fldr import fldr_preprocess
from fldr import fldr_sample


def get_chisquare_pval(p_target, samples):
    f_expected = [int(len(samples)*p) for p in p_target]
    counts = Counter(samples)
    f_actual = [counts[k] for k in range(len(p_target))]
    return chisquare(f_expected, f_actual)[1]

N_sample = 10000
max_dims = 11
rep_dims = 10
a_list = [
    [getrandbits(10) + 1 for i in range(n)]
    for n in range(1, max_dims)
    for k in range(rep_dims)
]
max_failed = 0.05 * len(a_list)


num_failed_py = 0
@pytest.mark.parametrize('a', a_list)
def test_fldr_sampler_py(a):
    global num_failed_py
    m = sum(a)
    p_target = [Fraction(x, m) for x in a]
    x = fldr_preprocess(a)
    samples = [fldr_sample(x) for _i in range(N_sample)]
    if x.n == 1:
        assert all(s==0 for s in samples)
    else:
        pval = get_chisquare_pval(p_target, samples)
        accept = 0.05 < pval
        num_failed_py += (not accept)
        assert num_failed_py <= max_failed + 1


num_failed_c = 0
@pytest.mark.parametrize('a', a_list)
def test_fldr_sampler_c(a):
    global num_failed_c
    m = sum(a)
    p_target = [Fraction(x, m) for x in a]
    n = len(a)
    with NamedTemporaryFile(mode='w', prefix='fldr.', delete=False) as f:
        f.write('%d ' % (n,))
        f.write(' '.join(map(str, a)))
        f.write('\n')
        f.close()

    samples_bytes = subprocess.check_output(['fldr', str(N_sample), f.name])
    samples_str = samples_bytes.decode('utf-8')
    samples = [int(x) for x in samples_str.strip().split(' ')]
    if n == 1:
        assert all(s==0 for s in samples)
    else:
        pval = get_chisquare_pval(p_target, samples)
        accept = 0.05 < pval
        num_failed_c += (not accept)
        assert num_failed_c <= max_failed + 1

    os.remove(f.name)


@pytest.mark.parametrize('a', a_list)
def test_preprocess_identical(a):
    n = len(a)
    x = fldr_preprocess(a)

    with NamedTemporaryFile(mode='w', prefix='fldr.', delete=False) as f:
        f.write('%d ' % (n,))
        f.write(' '.join(map(str, a)))
        f.write('\n')
        f.close()

    subprocess.check_output(['fldr', str(N_sample), f.name, '1'])
    os.remove(f.name)

    with open('%s.fldr' % (f.name), 'r') as f:
        (n, m, k, r) = [int(x) for x in f.readline().split(' ')]
        assert n == x.n
        assert m == x.m
        assert k == x.k
        assert r == x.r

        h = [int(x) for x in f.readline().split(' ')]
        assert h == x.h

        for i in range(n+1):
            Hi = [int(x) for x in f.readline().split(' ')]
            assert Hi == x.H[i]

    os.remove(f.name)
