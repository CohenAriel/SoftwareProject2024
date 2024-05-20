import sys
import numpy as np
import pandas as pd

np.random.seed(0)

k = int(sys.argv[1])
goal = sys.argv[2]
file_name = sys.argv[3]

# Load the data in the file
X = np.loadtxt(file_name)

def print_matrix(matrix):
    for line in matrix:
        print(*line, sep=', ')

# goal: Can get the following values:
# i. symnmf: Perform full the symNMF as described in 1 and output H.
# ii. sym: Calculate and output the similarity matrix as described in 1.1.
# iii. ddg: Calculate and output the Diagonal Degree Matrix as described in 1.2
# iv. norm: Calculate and output the normalized similarity matrix as described in 1.3.
match goal:
    case 'symnmf':
        W = norm(X)
        H = np.random.uniform(0, 2 * (np.mean(W)/k)^0.5, (len(X), k))
        H = symnmf(H, W)
        print_matrix(H)
    case 'sym':
        A = sym(X)
        print_matrix(A)
    case 'ddg':
        D = ddg(X)
        print_matrix(D)
    case 'norm':
        W = norm(X)
        print_matrix(W)
    case _:
        print('An Error Has Occurred')
        sys.exit(1)
