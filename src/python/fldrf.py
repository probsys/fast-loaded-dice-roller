# Released under Apache 2.0; refer to LICENSE.txt

from math import floor
from math import fmod
from math import frexp

from .fldr import fldr_preprocess_int
from .fldr import fldr_s

# ========================================================================
# Python style implementation

def fldr_preprocess_float_py(a):
    integers = normalize_floats_py(a)
    return fldr_preprocess_int(integers)

def normalize_floats_py(a):
    ratios = [as_integer_ratio_py(x) for x in a]
    Z = max(r[1] for r in ratios)
    return [r[0]*int(Z/r[1]) for r in ratios]

def as_integer_ratio_py(x):
    mantissa, exponent = frexp(x)
    i = 0
    while (mantissa != floor(mantissa)):
        mantissa *= 2.
        exponent -= 1
        i += 1
        if BITS_MAX <= i:
            assert False, 'Failed to converge: %1.5f' % (x,)
    numerator = int(mantissa)
    denominator = 1
    if exponent > 0:
        numerator <<= exponent
    else:
        denominator <<= abs(exponent)
    return (numerator, denominator)

# ========================================================================
# ANSI C99 Implementation (Sketch)

# DBL_MAX from <float.h> typically (2^53-1)*(2^(1023-52)) ~ 2^1024.
BITS_MAX = 1024

def fldr_preprocess_float_c(a):
    n = len(a)
    mantissas = normalize_floats_c(a)
    arrays = [align_mantissa(m) for m in mantissas]
    m = binary_sum(arrays)
    k = len(m)
    r = compute_reject_bits(m, k)

    h = [0] * k
    H = [[-1]*k for _i in range(n+1)]
    for j in range(k):
        d = 0
        for i in range(n):
            idx = j - (k - len(arrays[i]))
            w = arrays[i][idx] if 0 <= idx else 0
            h[j] += (w > 0)
            if w > 0:
                H[d][j] = i
                d += 1
        idx = j - (k - len(r))
        w = r[idx] if 0 <= idx else 0
        h[j] += (w > 0)
        if w > 0:
            H[d][j] = n

    return fldr_s(n, m, k, r, h, H)

def normalize_floats_c(a):
    ratios = [as_integer_ratio_c(x) for x in a]
    max_exponent = max(abs(r[1]) for r in ratios)
    offsets = [max_exponent - abs(r[1]) for r in ratios]
    return [(m[0], m[1], m[2]+o) for (m, e), o in zip(ratios, offsets)]

def binary_sum(arrays):
    m = binary_add(arrays[0], arrays[1])
    for i in range(2, len(arrays)):
        m = binary_add(m, arrays[i])
    return m

def compute_reject_bits(m, k):
    if m[0] == 1 and sum(m) == 1:
        r = []
    else:
        mantissa_pow_2k = [1] + [0] * (k)
        r = binary_sub(mantissa_pow_2k, m)
        assert len(r) <= k
    return r

def as_integer_ratio_c(x):
    mantissa, exponent = frexp(x)
    i = 0
    while (mantissa != floor(mantissa)):
        mantissa *= 2.
        exponent -= 1
        i += 1
        if BITS_MAX <= i:
            assert False, 'Failed to converge: %1.5f' % (x,)
    (mantissa_bits, mantissa_k) = float_to_bits(mantissa)
    mantissa_offset = 0
    if exponent > 0:
        mantissa_offset = exponent
        exponent = 0
    return ((mantissa_bits, mantissa_k, mantissa_offset), abs(exponent))

def float_to_bits(x):
    assert x == floor(x)
    a = [0]*BITS_MAX
    width = 0
    while x > 0:
        a[width] = int(fmod(x, 2))
        x = floor(x/2)
        width += 1
    return (a, width)

def align_mantissa(mantissa):
    (bits, width, offset) = mantissa
    array = [0] * (width + offset)
    start = width - 1
    for i in range(0, width):
        array[start-i] = bits[i]
    return array

def binary_add(a, b):
    la = len(a)
    lb = len(b)
    l = max(la, lb)
    c = 0
    x = [0] * (l + 1)
    for i in range(1, l+1):
        ai = a[la - i] if 0 <= la - i else 0
        bi = b[lb - i] if 0 <= lb - i else 0
        x[l+1-i] = ((ai ^ bi) ^ c)
        c = (ai & bi) | (ai & c) | (bi & c)
    if c == 1:
        x[0] = 1
    return binary_trim_inital_zeros(x)

def binary_sub(a, b):
    la = len(a)
    lb = len(b)
    assert lb <= la
    l = max(la, lb)
    c = 0
    x = [0] * (l + 1)
    for i in range(1, l+1):
        ai = a[la - i] if 0 <= la - i else 0
        bi = b[lb - i] if 0 <= lb - i else 0
        x[l+1-i] = ((ai ^ bi) ^ c)
        c = c & (not (ai ^ bi)) | ((not ai) & bi)
    return binary_trim_inital_zeros(x)

def binary_trim_inital_zeros(x):
    j = 0
    while (j < len(x)) and (x[j] == 0):
        j += 1
    return x[j:] if j < len(x) else [0]
