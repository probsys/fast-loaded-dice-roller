# Released under Apache 2.0; refer to LICEaE.txt.

import os
import subprocess

from itertools import product
from random import getrandbits
from random import random

import pytest

from fldr.fldrf import align_mantissa
from fldr.fldrf import double_s

def linspace(start, stop, n):
    if n == 1:
        yield stop
        return
    h = (stop - start) / (n - 1)
    for i in range(n):
        yield start + h * i

def bits_to_int(a):
    return int(''.join(map(str, a)), 2) if a else 0

xs \
    = list(linspace(0, 1, 100)) \
    + list(linspace(1, 10, 100)) \
    + list(map(float, range(0,10)))

@pytest.mark.parametrize('x', xs)
def test_as_integer_ratio_py(x):
    from fldr.fldrf import as_integer_ratio_py
    a = x.as_integer_ratio()
    b = as_integer_ratio_py(x)
    assert a == b

@pytest.mark.parametrize('x', xs)
def test_as_integer_ratio_c(x):
    from fldr.fldrf import as_integer_ratio_c
    a = x.as_integer_ratio()
    double = as_integer_ratio_c(x)
    assert double.mantissa[double.width-1] is not 0 or x == 0.0
    mantissa = align_mantissa(double)
    numerator = bits_to_int(mantissa)
    denominator = 2**double.exponent
    assert a == (numerator, denominator)

ms = [
    (double_s([0,0,0,0,0,0,0], 0, 0, None), []),
    (double_s([0,1,0,0,0,0,0], 3, 0, None), [0,1,0]),
    (double_s([1,1,0,0,0,0,0], 3, 0, None), [0,1,1]),
    (double_s([1,1,0,0,0,0,0], 3, 4, None), [0,1,1,0,0,0,0]),
    (double_s([1,1,0,1,1,0,0], 5, 3, None), [1,1,0,1,1,0,0,0]),
]
@pytest.mark.parametrize('m', ms)
def test_align_mantissa(m):
    double, solution = m
    assert align_mantissa(double) == solution

bits_list = [(0,), (0,1,), (1,),(1, 0), (0,1,1), (1,1,1), (1,1,0,1,1,0)]
@pytest.mark.parametrize('a, b', product(bits_list, bits_list))
def test_binary_add(a, b):
    from fldr.fldrf import binary_add
    int_a = bits_to_int(a)
    int_b = bits_to_int(b)
    solution = bin(int_a + int_b)
    answer = '0b%s' % (''.join(map(str, binary_add(a, b))))
    try:
        assert answer == solution
    except AssertionError:
        # Case 0b011 == 0b11, happens when a starts with 0.
        assert answer[2] == '0'
        assert answer[3:] == solution[2:]

@pytest.mark.parametrize('a, b', product(bits_list, bits_list))
def test_binary_sub(a, b):
    from fldr.fldrf import binary_sub
    int_a = bits_to_int(a)
    int_b = bits_to_int(b)
    if (int_a < int_b) or len(a) < len(b):
        return True
    solution = bin(int_a - int_b)
    answer = '0b%s' % (''.join(map(str, binary_sub(a, b))))
    assert answer == solution

N_sample = 10000
max_dims = 11
rep_dims = 10
a_list_int = [
    [getrandbits(10) + 1. for i in range(n)]
    for n in range(2, max_dims)
    for k in range(rep_dims)
]
a_list_dyadic = [
    [1, 1],
    [1, 2, 1],
    [1, 1, 2, 3, 1],
]
a_list_float = [
    [getrandbits(10) + random() for i in range(n)]
    for n in range(2, max_dims)
    for k in range(rep_dims)
]
@pytest.mark.parametrize('a', a_list_int + a_list_dyadic + a_list_float)
def test_normalize_floats_equivalent_py_pyc(a):
    from fldr.fldrf import normalize_floats_py
    from fldr.fldrf import normalize_floats_c
    integers = normalize_floats_py(a)
    mantissas = normalize_floats_c(a)
    arrays = [align_mantissa(m) for m in mantissas]
    for i, j in zip(integers, arrays):
        assert i == bits_to_int(j)

@pytest.mark.parametrize('a', a_list_int + a_list_dyadic + a_list_float + [[.1]*10])
def test_preprocess_identical_py_pyc(a):
    from fldr.fldrf import fldr_preprocess_float_py
    from fldr.fldrf import fldr_preprocess_float_c
    x_py = fldr_preprocess_float_py(a)
    x_c = fldr_preprocess_float_c(a)

    assert x_py.m == bits_to_int(x_c.m)
    assert x_py.k == x_c.k
    assert x_py.r == bits_to_int(x_c.r)
    assert x_py.h == x_c.h
    assert x_py.H == x_c.H

@pytest.mark.parametrize('a', a_list_int + a_list_dyadic + a_list_float)
def test_preprocess_identical_py_c(a):
    from fldr.fldrf import fldr_preprocess_float_c
    from tempfile import NamedTemporaryFile

    n = len(a)
    x = fldr_preprocess_float_c(a)

    with NamedTemporaryFile(mode='w', prefix='fldrf.', delete=False) as f:
        f.write('%d ' % (n,))
        f.write(' '.join(map(str, a)))
        f.write('\n')
        f.close()

    subprocess.check_output(['fldrf', str(N_sample), f.name, '1'])
    os.remove(f.name)

    with open('%s.fldrf' % (f.name), 'r') as f:
        (n, k) = [int(x) for x in f.readline().split(' ')]
        assert n == x.n
        assert k == x.k

        m = [int(x) for x in f.readline().split(' ')]
        assert x.m == m

        rline = f.readline()
        r = [int(x) for x in rline.split(' ')] if rline != os.linesep else []
        assert x.r == r

        h = [int(x) for x in f.readline().split(' ')]
        assert h == x.h

        for i in range(n+1):
            Hi = [int(x) for x in f.readline().split(' ')]
            assert Hi == x.H[i]

    os.remove(f.name)
