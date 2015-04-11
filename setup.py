#!/usr/bin/env python
# -*- coding: utf-8 -*-
try:
    from setuptools import setup, Extension
except ImportError:
    from distutils.core import setup, Extension

import glob
import os

cmph_src_files = [
    "cindexedfastq/lib/cmph/src/bdz.c",
    "cindexedfastq/lib/cmph/src/bdz_ph.c",
    "cindexedfastq/lib/cmph/src/bm_numbers.c",
    "cindexedfastq/lib/cmph/src/bmz.c",
    "cindexedfastq/lib/cmph/src/bmz8.c",
    "cindexedfastq/lib/cmph/src/brz.c",
    "cindexedfastq/lib/cmph/src/buffer_entry.c",
    "cindexedfastq/lib/cmph/src/buffer_manager.c",
    "cindexedfastq/lib/cmph/src/chd.c",
    "cindexedfastq/lib/cmph/src/chd_ph.c",
    "cindexedfastq/lib/cmph/src/chm.c",
    "cindexedfastq/lib/cmph/src/cmph.c",
    "cindexedfastq/lib/cmph/src/cmph_benchmark.c",
    "cindexedfastq/lib/cmph/src/cmph_structs.c",
    "cindexedfastq/lib/cmph/src/compressed_rank.c",
    "cindexedfastq/lib/cmph/src/compressed_seq.c",
    "cindexedfastq/lib/cmph/src/fch.c",
    "cindexedfastq/lib/cmph/src/fch_buckets.c",
    "cindexedfastq/lib/cmph/src/graph.c",
    "cindexedfastq/lib/cmph/src/hash.c",
    "cindexedfastq/lib/cmph/src/jenkins_hash.c",
    "cindexedfastq/lib/cmph/src/linear_string_map.c",
    "cindexedfastq/lib/cmph/src/miller_rabin.c",
    "cindexedfastq/lib/cmph/src/select.c",
    "cindexedfastq/lib/cmph/src/vqueue.c",
    "cindexedfastq/lib/cmph/src/vstack.c"
]

bgzf_src_files = [
    "cindexedfastq/lib/bgzf/bgzf.c"
]

cindexedfastq_src_files = [
    "cindexedfastq/indexing.c",
    "cindexedfastq/cindexedfastq.c"
]

cindexedfastq_include_dirs = [
    "cindexedfastq/lib/cmph/src/",
    "cindexedfastq/lib/bgzf/",
    "cindexedfastq/"
]

config_file = open( "cindexedfastq/lib/cmph/src/config.h", "w" )
config_file.write( """#define VERSION "2.0"
#define HAVE_DLFCN_H 1
#define HAVE_GETOPT_H 1
#define HAVE_MATH_H 1
#define HAVE_MEMORY_H 1
#define HAVE_STDINT_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRINGS_H 1
#define HAVE_STRING_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_UNISTD_H 1""" )
config_file.close( )

cindexedfastq = Extension(
    "indexedfastq.cindexedfastq",
    cmph_src_files + bgzf_src_files + cindexedfastq_src_files,
    library_dirs=[],
    include_dirs=cindexedfastq_include_dirs,
    libraries=["z"],
    language="c",
    extra_compile_args=[],
    define_macros=[]
)

setup(name='indexedfastq',
    version='0.0.1',
    description='Indexing fastq for fast random access.',
    author = 'Mattias Frånberg',
    author_email = 'mattias.franberg@scilifelab.se',
    url = 'https://github.com/mfranberg/indexedfastq',
    license = 'BSD License',
    install_requires=[
    ],
    package_dir = { 
        "indexedfastq" : "indexedfastq" 
    },
    packages = [
        'indexedfastq'
    ],
    ext_modules = [ cindexedfastq ],
    scripts = [
    ],
    keywords = ['Bioinformtics', 'fastq', 'index'],
    classifiers = [
        "Programming Language :: Python",
        "License :: OSI Approved :: MIT License",
        "Development Status :: 4 - Beta",
        "Operating System :: MacOS :: MacOS X",
        "Operating System :: Unix",
        "Intended Audience :: Science/Research",
        "Topic :: Scientific/Engineering :: Bio-Informatics",
    ],
    long_description = 'Indexing fastq for fast random access.',
    # To provide executable scripts, use entry points in preference to the
    # "scripts" keyword. Entry points provide cross-platform support and allow
    # pip to create the appropriate form of executable for the target platform.
    entry_points={
        'console_scripts': [
        ]
    }
)
