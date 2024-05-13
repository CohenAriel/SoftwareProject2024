import sys
import numpy as np
import pandas as pd

np.random.seed(0)

k = int(sys.argv[1])
goal = sys.argv[2]
file_name = sys.argv[3]

# Load the data in the file
data = np.loadtxt(file_name)

#goal: Can get the following values:
#i. symnmf: Perform full the symNMF as described in 1 and output H.
#ii. sym: Calculate and output the similarity matrix as described in 1.1.
#iii. ddg: Calculate and output the Diagonal Degree Matrix as described in 1.2
##iv. norm: Calculate and output the normalized similarity matrix as described in 1.3.
match goal:
    case 'symnmf':
        # Perform full the symNMF as described in 1 and output H.
        # Initialize W and H
        W = np.random.rand(data.shape[0], k)
        H = np.random.rand(data.shape[1], k)
        # Perform symNMF
        for i in range(1000):
            H = H * (W.T @ data) / (W.T @ W @ H)
            W = W * (data @ H) / (W @ H @ H.T)
        np.savetxt('H.txt', H)
    case 'sym':
        # Calculate and output the similarity matrix as described in 1.1.
        S = data @ data.T
        np.savetxt('S.txt', S)
    case 'ddg':
        # Calculate and output the Diagonal Degree Matrix as described in 1.2
        D = np.diag(np.sum(data, axis=1))
        np.savetxt('D.txt', D)
    case 'norm':
        # Calculate and output the normalized similarity matrix as described in 1.3.
        S = data @ data.T
        D = np.diag(1 / np.sqrt(np.diag(S)))
        S_norm = D @ S @ D
        np.savetxt('S_norm.txt', S_norm)
    case _:
        print('An Error Has Occurred')
        sys.exit(1)