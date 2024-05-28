# include <math.h>
# include "symnmf.h"

void free_2d_array(double **arr) {
    int n = sizeof(arr) / sizeof(arr[0]);
    int i;
    for (i = 0; i < n; i++) {
        free(arr[i]);
    }
    free(arr);
}

// Matrix multiplication. We assume that A is m x n, B is n x d and C is m x d.
void **matmul(double **A, double **B, double **C, int n, int m, int d) {
    int i, j, k;
    for (i = 0; i < m; i++) {
        for (j = 0; j < d; j++) {
            C[i][j] = 0;
            for (k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

double **norm(double **X) {
    double **W;
    double **A = sym(X);
    double **D = ddg(A);

    int n = sizeof(X) / sizeof(X[0]);
    double **D_norm = (double *)malloc(sizeof(double) * n);
    for (int i = 0; i < n; i++) {
        D_norm[i][i] = 1 / sqrt(D[i][i]);
    }

    // D is used here temporarily for calculations
    matmul(D_norm, A, D, n, n, n);
    matmul(D, D_norm, W, n, n, n);

    free_2d_array(A);
    free_2d_array(D);
    free_2d_array(D_norm);
    return W;
}

double **symnmf(double **H , double **W) {
    int n = sizeof(H) / sizeof(H[0]);
    int k = sizeof(H[0]) / sizeof(H[0][0]);

    int i, j, l;
    for (i = 0; i < 300; i++) {
        double** new_H;
        double **numerators = (double *)malloc(sizeof(double) * n * k);
        double **denominators = (double *)malloc(sizeof(double) * n * k);

        matmul(W, H, numerators, n, n, k);
        // Multiply H with H^T
        for (j = 0; j < n; j++) {
            for (l = 0; l < n; l++) {
                denominators[j][l] = 0;
                for (int m = 0; m < k; m++) {
                    denominators[j][l] += H[j][m] * H[l][m];
                }
            }
        }

        // Element-wise operations
        for (j = 0; j < n; j++) {
            for (l = 0; l < k; l++) {
                new_H[j][l] = 0.5 + 0.5*(numerators[j][l] / denominators[j][l]);
                new_H[j][l] = H[j][l] * new_H[j][l];
            }
        }

        free_2d_array(numerators);
        free_2d_array(denominators);

        // Calculate the Frobenius norm of the difference between H and new_H
        double norm = 0;
        for (j = 0; j < n; j++) {
            for (l = 0; l < k; l++) {
                norm += pow(new_H[j][l] - H[j][l], 2);
            }
        }
        // Break on convergence
        if (norm < 0.0001) {
            free_2d_array(new_H);
            break;
        }

        H = new_H;
        free_2d_array(new_H);
    }

    return H;
}
