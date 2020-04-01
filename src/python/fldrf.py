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
        if DBL_MAX_WIDTH <= i:
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

class double_s:
    def __init__(self, mantissa, width, offset, exponent):
        self.mantissa = mantissa
        self.width = width
        self.offset = offset
        self.exponent = exponent

# DBL_MAX from <float.h> typically (2^53-1)*(2^(1023-52)) ~ 2^1024.
DBL_MAX_WIDTH = 1024

def fldr_preprocess_float_c(a):
    n = len(a)
    doubles = normalize_floats_c(a)
    mantissas = [align_mantissa(d) for d in doubles]
    m = binary_sum(mantissas)
    (r, k) = compute_reject_bits(m)

    h = [0] * k
    H = [[-1]*k for _i in range(n+1)]
    for j in range(k):
        d = 0
        for i in range(n):
            idx = j - (k - len(mantissas[i]))
            w = mantissas[i][idx] if 0 <= idx else 0
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
    doubles = [as_integer_ratio_c(x) for x in a]
    max_exponent = max(d.exponent for d in doubles)
    for d in doubles:
        offset = max_exponent - d.exponent
        d.offset += offset
        d.width += offset
        d.exponent = max_exponent
    return doubles

def as_integer_ratio_c(x):
    m, exponent = frexp(x)
    i = 0
    while (m != floor(m)):
        m *= 2.
        exponent -= 1
        i += 1
        if DBL_MAX_WIDTH <= i:
            assert False, 'Failed to converge: %1.5f' % (x,)
    mantissa, width = decimal_to_binary(m)
    if 0 < exponent:
        offset = exponent
        exponent = 0
    else:
        offset = 0
        exponent = abs(exponent)
    return double_s(mantissa, width, offset, exponent)

def decimal_to_binary(x):
    assert x == floor(x)
    bits = [0]*DBL_MAX_WIDTH
    width = 0
    while x > 0:
        bits[width] = int(fmod(x, 2))
        x = floor(x/2)
        width += 1
    return bits, width

def align_mantissa(double):
    mantissa = [0] * (double.width + double.offset)
    start = double.width - 1
    for i in range(0, double.width):
        mantissa[start-i] = double.mantissa[i]
    return mantissa

def compute_reject_bits(m):
    if m[0] == 1 and sum(m) == 1:
        k = len(m) - 1
        r = []
    else:
        k = len(m)
        m_pow_2k = [1] + [0] * (k)
        r = binary_sub(m_pow_2k, m)
        assert len(r) <= k
    return r, k

def binary_sum(arrays):
    m = binary_add(arrays[0], arrays[1])
    for i in range(2, len(arrays)):
        m = binary_add(m, arrays[i])
    return m

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
