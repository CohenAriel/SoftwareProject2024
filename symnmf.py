import sys
import numpy as np
import symnmfmodule

np.random.seed(0)

k = int(sys.argv[1])
goal = sys.argv[2]
file_name = sys.argv[3]

# Load the data in the file
X = np.loadtxt(file_name, delimiter=',').tolist()

def print_matrix(matrix):
    for line in matrix:
        print(*[f'{x:.4f}' for x in line], sep=',')

# goal: Can get the following values:
# i. symnmf: Perform full the symNMF as described in 1 and output H.
# ii. sym: Calculate and output the similarity matrix as described in 1.1.
# iii. ddg: Calculate and output the Diagonal Degree Matrix as described in 1.2
# iv. norm: Calculate and output the normalized similarity matrix as described in 1.3.

if goal == 'symnmf':
    W = symnmfmodule.norm(X)
    H = np.random.uniform(0, 2 * (np.mean(W)/k)**0.5, (len(X), k)).tolist()
    H = symnmfmodule.symnmf(H, W)
    print_matrix(H)
elif goal == 'sym':
    A = symnmfmodule.sym(X)
    print_matrix(A)
elif goal == 'ddg':
    D = symnmfmodule.ddg(X)
    print_matrix(D)
elif goal == 'norm':
    W = symnmfmodule.norm(X)
    print_matrix(W)
else:
    print('An Error Has Occurred')
    sys.exit(1)
