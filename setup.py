from setuptools import Extension, setup

module = Extension("symnmf", sources=['geomodule.c'])
setup(name='symnmf',
     version='1.0',
     description=' A clustering algorithm that is based on symmetric Non-negative Matrix Factorization (symNMF)',
     ext_modules=[module])