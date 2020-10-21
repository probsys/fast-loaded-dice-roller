# The Fast Loaded Dice Roller

This repository contains reference implementations in C and Python
of the sampling algorithm in

> Feras A. Saad, Cameron E. Freer, Martin C. Rinard, and Vikash K. Mansinghka.
[The Fast Loaded Dice Roller: A Near-Optimal Exact Sampler for Discrete Probability Distributions](https://arxiv.org/pdf/2003.03830.pdf).
In AISTATS 2020: Proceedings of the 23rd International Conference on
Artificial Intelligence and Statistics, Proceedings of Machine Learning
Research 108, Palermo, Sicily, Italy, 2020.

The Fast Loaded Dice Roller (FLDR) is a fast algorithm for rolling an
n-sided dice.  More specifically, given a list `L` of `n` positive numbers,
where `L[i]` represents the relative weight of the `i`th side, FLDR returns
integer `i` with relative probability `L[i]`.

FLDR produces exact samples from the specified probability distribution:

  - For **integer** weights, the probability of returning `i` is precisely
    equal to the rational number `Fraction(L[i], m)`, where
    `m` is the sum of `L`.

  - For **floating-points** weights, each weight `L[i]` is (conceptually)
    converted to the corresponding rational number `Fraction(n[i], d[i])`
    where `n[i]` is a positive integer and `d[i]` is a power of 2. The
    rational weights are then normalized (exactly) to sum to unity. The
    preprocessing computations are never done explicitly in floating-point,
    but instead operate directly on the binary representation of
    floating-point numbers, as defined by the
    [IEEE-754](https://en.wikipedia.org/wiki/IEEE_754) standard

## Building and Installing

The Python library can be installed via pip

    pip install fldr

The C library can be built by running

    $ make all

This command creates several artifacts in the `build/` directory:

1. `build/lib/fldr`: A Python package that implements FLDR.

2. `build/lib/libfldr.a`: A static C library for C programs that use FLDR.

3. `build/include`: Contains header files for C programs that use FLDR.

4. `build/bin`: Contains executables for a command line interface to FLDR.

## Usage (Python Library)

The Python 3 library is implemented in `src/python`.
The following code from [examples/example.py](examples/example.py)
shows how to use FLDR to sample from a distribution with integer weights.

```python
from fldr import fldr_preprocess
from fldr import fldr_sample

N_sample = 100
distribution = [1, 1, 2, 3, 1]
x = fldr_preprocess(distribution)
samples = [fldr_sample(x) for _i in range(N_sample)]
print(' '.join(map(str, samples)))
```

To sample from distributions with floating-point weights, use
`fldrf_preprocess` instead of `fldr_preprocess`. For an illustration,
refer to [examples/examplef.py](examples/examplef.py).

These examples can be invoked by running:

    $ ./pythenv.sh python examples/example.py
    $ ./pythenv.sh python examples/examplef.py

## Usage (C Library)

The C library is implemented in `src/c`.

The following code from [examples/example.c](examples/example.c)
shows how to use FLDR to sample from a distribution with integer weights.

```c
#include <stdlib.h>
#include <stdio.h>
#include "fldr.h"

int main(int argc, char **argv) {
    int N_sample = 100;
    int *samples = calloc(N_sample, sizeof(*samples));

    int distribution[5] = { 1, 1, 2, 3, 1 };
    fldr_preprocess_t *x = fldr_preprocess(distribution, 5);
    for (int i = 0; i < N_sample; i++) {
        samples[i] = fldr_sample(x);
        printf("%d ", samples[i]);
    }
    printf("\n");

    free(samples);
    fldr_free(x);
}
```

To sample from distributions with floating-point weights, use
`fldrf_preprocess` instead of `fldr_preprocess`. For an illustration,
refer to [examples/examplef.c](examples/examplef.c).

These examples can be invoked by running:

    $ make -C examples
    $ ./examples/example.out
    $ ./examples/examplef.out

## Usage (Command Line Interface)

Two executables are provided:

  - `./build/bin/fldr` (integer weights)
  - `./build/bin/fldrf` (floating-point weights)

The executables have the following command line interface:

    usage: ./build/bin/fldr N path

where `N` is the number of samples to draw; `path` is the file that specifies
the target distribution (the first number in `path` should be the number
of elements in the target distribution).

For example, to generate 100 samples from { 1, 1, 2, 3, 1 }, run:

    $ echo '5 1 1 2 3 1' > w
    $ ./build/bin/fldr 100 w

To generate 100 samples from { 0.25, 0.13, 1.12 }, run:

    $ echo '3 0.25 0.13 1.12' > w
    $ ./build/bin/fldrf 100 w

## Tests

The test suite in `tests/` requires `pytest` and `scipy`.
Run the following command in the shell:

    $ ./check.sh

Note that the test cases are stochastic and are tested using stochastic
goodness-of-fit tests, and thus 5% of the stochastic test cases will on
average in any give run of the test module for the given significance
level.

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
