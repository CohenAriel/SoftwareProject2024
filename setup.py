from setuptools import Extension, setup

module = Extension("symnmfmodule", sources=['symnmfmodule.c', 'symnmf.c'])
setup(name='symnmfmodule',
     version='1.0',
     description=' A clustering algorithm that is based on symmetric Non-negative Matrix Factorization (symNMF)',
     ext_modules=[module])