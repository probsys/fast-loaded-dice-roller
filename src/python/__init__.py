# Released under Apache 2.0; refer to LICENSE.txt

__version__ = '1.4.5'

from .fldr import fldr_preprocess_int
from .fldr import fldr_sample
from .fldrf import fldr_preprocess_float_c

fldr_preprocess = fldr_preprocess_int
fldrf_preprocess = fldr_preprocess_float_c
