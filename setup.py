# Copyright 2020 MIT Probabilistic Computing Project.
# See LICENSE.txt

import os
import re
from setuptools import setup

# Determine the version (hardcoded).
dirname = os.path.dirname(os.path.realpath(__file__))
vre = re.compile('__version__ = \'(.*?)\'')
m = open(os.path.join(dirname, 'src', 'python', '__init__.py')).read()
__version__ = vre.findall(m)[0]

setup(
    name='fldr',
    version=__version__,
    description='Fast Loaded Dice Roller',
    license='Apache-2.0',
    long_description=open('README.md').read(),
    long_description_content_type='text/markdown',
    classifiers=[
        'Development Status :: 2 - Pre-Alpha',
        'Intended Audience :: Science/Research',
        'License :: OSI Approved :: Apache Software License',
        'Programming Language :: Python :: 3',
    ],
    packages=[
        'fldr',
        'fldr.tests',
    ],
    package_dir={
        'fldr': 'src/python',
        'fldr.tests': 'tests',
    },
    extras_require={
        'tests' : ['pytest', 'scipy']
    }
)
