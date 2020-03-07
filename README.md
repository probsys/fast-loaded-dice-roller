# The Fast Loaded Dice Roller

This repository contains reference implementations in C and Python
of the sampling algorithm in

> Feras A. Saad, Cameron E. Freer, Martin C. Rinard, and Vikash K. Mansinghka.
[The Fast Loaded Dice Roller: A Near-Optimal Exact Sampler for Discrete Probability Distributions](http://fsaad.mit.edu/assets/SFRM-FLDR-AISTATS-2020.pdf).
In AISTATS 2020: Proceedings of the 23rd International Conference on
Artificial Intelligence and Statistics, Proceedings of Machine Learning
Research 108, Palermo, Sicily, Italy, 2020.

The Fast Loaded Dice Roller (FLDR) is a fast algorithm for rolling an
n-sided die.  More specifically, given a list `L` of `n` integers
that sum to `m`, FLDR returns an integer `i` with probability `L[i]/m`.

## Installing

The software can be installed by running

    $ make all

This command creates two artifacts in the `build/` directory:

1. `build/bin/fldr`: A C binary providing a command-line interface to FLDR.

2. `build/lib/fldr`: A Python package implementing FLDR.

## Usage (Python Library)

The Python 3 library is implemented in `src/python`. Some
options for installing the package into your Python 3 environment include:

  Option 1. Run `python setup.py install`.

  Option 2. Add the absolute path `build/lib/fldr` to your `PYTHONPATH`

The following example is from [src/python/example.py](src/python/example.py):

```python
from fldr import fldr_preprocess
from fldr import fldr_sample

N_sample = 100
distribution = [1, 1, 2, 3, 1]
x = fldr_preprocess(distribution)
samples = [fldr_sample(x) for _i in range(N_sample)]
print(' '.join(map(str, samples)))
```

The above example can be invoked by e.g., running

    $ python setup.py build
    $ ./pythenv.sh python src/python/example.py

## Usage (Command Line Interface)

The command line script in `./build/bin/fldr` has the following interface:

    usage: ./build/bin/fldr N path

where `N` is the number of sample and `path` is the file that specifies
the target distribution (the first number in `path` should be the number
of elements in the target distribution).

For example, to generate 100 samples from {1, 1, 2, 3, 1}, run:

    $ echo '5 1 1 2 3 1' > w
    $ ./build/bin/fldr 100 w

## Usage (C Library)

The C library is implemented in `src/c`.

The following example is from [src/c/example.c](src/c/example.c):

```c
#include <stdlib.h>
#include <stdio.h>
#include "fldr.h"

int main(int argc, char **argv) {
    int N_sample = 100;
    int * samples = (int *) calloc(N_sample, sizeof(int));

    int distribution[5] = { 1, 1, 2, 3, 1 };
    struct fldr_s x = fldr_preprocess(distribution, 5);
    for (int i = 0; i < N_sample; i++) {
        samples[i] = fldr_sample(&x);
        printf("%d ", samples[i]);
    }
    printf("\n");

    free(samples);
    fldr_free(&x);
}
```

The above example can be invoked by e.g., running

    $ cd src/c
    $ make example.out
    $ ./example.out

## Tests

The test suite in `tests/` requires `pytest` and `scipy`.
Run the following command in the shell:

    $ ./check.sh

Note that the test cases are stochastic and are tested using stochastic
goodness-of-fit tests, and thus are expected to fail an average of
five times out of every 100 runs for the given significance level.

## Experiments

Implementations of the experiments and baseline exact sampling algorithms
from Section 6 of the AISTATS paper can be found at
https://github.com/probcomp/fast-loaded-dice-roller-experiments.

## Citing

Please cite the following paper:

    @inproceedings{saad2020fldr,
    title           = {The Fast Loaded Dice Roller: A Near-optimal Exact Sampler for Discrete Probability Distributions},
    author          = {Saad, Feras A. and Freer, Cameron E. and Rinard, Martin C. and Mansinghka, Vikash K.},
    booktitle       = {AISTATS 2020: Proceedings of the 23rd International Conference on Artificial Intelligence and Statistics},
    volume          = 108,
    series          = {Proceedings of Machine Learning Research},
    address         = {Palermo, Sicily, Italy},
    publisher       = {PMLR},
    year            = 2020,
    keywords        = {random variate generation, sampling, discrete random variables},
    abstract        = {This paper introduces a new algorithm for the fundamental problem of generating a random integer from a discrete probability distribution using a source of independent and unbiased random coin flips. This algorithm, which we call the Fast Loaded Dice Roller (FLDR), has efficient complexity properties in space and time: the size of the sampler is guaranteed to be linear in the number of bits needed to encode the target distribution and the sampler consumes (in expectation) at most 6.5 bits of entropy more than the information-theoretically minimal rate, independently of the values or size of the target distribution. We present an easy-to-implement, linear-time preprocessing algorithm and a fast implementation of the FLDR using unsigned integer arithmetic. Empirical evaluations establish that the FLDR is 2x--10x faster than multiple baseline algorithms for exact sampling, including the widely-used alias and interval samplers. It also uses up to 10000x less space than the information-theoretically optimal sampler, at the expense of a less than 1.5x runtime overhead.},
    note            = {(To Appear)},
    }
