#include <math.h>
#include "symnmf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double **alloc_2d_array(int n, int d) {
    int i;
    double **arr = (double **)malloc(sizeof(double *) * n);
    for (i = 0; i < n; i++) {
        arr[i] = (double *)malloc(sizeof(double) * d);
    }
    return arr;
}

void free_2d_array(double **arr, int n) {
    int i;
    for (i = 0; i < n; i++) {
        free(arr[i]);
    }
    free(arr);
}

/* Matrix multiplication. We assume that A is m x n, B is n x d and C is m x d */
void matmul(double **A, double **B, double **C, int n, int m, int d) {
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

/* Calculate the Euclidean distance between two vectors */
float distance(double* x , double* y , int n) {
    float sum = 0;
    int i;
    for (i = 0; i < n; i++) {
        sum += pow(x[i] - y[i], 2);
    }
    return sum;
}

/*  Calculate the symmetric matrix.
    Accepts the input X and the number of vectors n as arguments */
double **sym(double **X, int n, int d) {
    double **A = alloc_2d_array(n, n);
    int i,j;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (i == j) {
                A[i][j] = 0;
                continue;
            }
            A[i][j] = distance(X[i], X[j], d);
            A[i][j] = exp(-A[i][j]/2);
        }
    }
    return A;
}

/*  Calculate the ddg matrix.
    Accepts the similarity matrix A and the number of vectors n as arguments */
double **ddg(double **A, int n) {
    int i,j;
    double **D = alloc_2d_array(n, n);
    for (i = 0; i < n; i++) {
        D[i][i] = 0;
        for (j = 0; j < n; j++) {
            D[i][i] += A[i][j];
            if (i != j) {
                D[i][j] = 0;
            }
        }
    }
    return D;
}

/*  Calculate the norm matrix.
    Accepts the similarity matrix A and the number of vectors n as arguments */
double **norm(double **A, int n) {
    int i;
    double **W = alloc_2d_array(n, n);
    double **D = ddg(A, n);
    double **D_norm = alloc_2d_array(n, n);
    for (i = 0; i < n; i++) {
        D_norm[i][i] = 1 / sqrt(D[i][i]);
    }

    /* D is used here temporarily for calculations*/
    matmul(D_norm, A, D, n, n, n);
    matmul(D, D_norm, W, n, n, n);

    free_2d_array(D, n);
    free_2d_array(D_norm, n);
    return W;
}

/*  Calculate the symmetric non-negative matrix factorization.
    Accepts the matrices H and W and the number of vectors n and the required number of clusters k as arguments */
double **symnmf(double **H , double **W, int n, int k) {
    int i, j, l , m;

    for (i = 0; i < 300; i++) {
        double norm;
        double** new_H = alloc_2d_array(n, k);
        double **numerators = alloc_2d_array(n, k);
        double **denominators = alloc_2d_array(n, k);

        matmul(W, H, numerators, n, n, k);
        /* Multiply H with H^T */
        for (j = 0; j < n; j++) {
            for (l = 0; l < n; l++) {
                denominators[j][l] = 0;
                for (m = 0; m < k; m++) {
                    denominators[j][l] += H[j][m] * H[l][m];
                }
            }
        }

        /* Element-wise operations*/
        for (j = 0; j < n; j++) {
            for (l = 0; l < k; l++) {
                new_H[j][l] = 0.5 + 0.5*(numerators[j][l] / denominators[j][l]);
                new_H[j][l] = H[j][l] * new_H[j][l];
            }
        }

        free_2d_array(numerators, n);
        free_2d_array(denominators, n);

        /* Calculate the Frobenius norm of the difference between H and new_H*/
        norm = 0;
        for (j = 0; j < n; j++) {
            for (l = 0; l < k; l++) {
                norm += pow(new_H[j][l] - H[j][l], 2);
            }
        }
        /* Break on convergence*/
        if (norm < 0.0001) {
            free_2d_array(new_H, n);
            break;
        }

        H = new_H;
        free_2d_array(new_H, n);
    }

    return H;
}

void print_2D_array(double **arr, int n, int m) {
    int i, j;
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            printf("%f ", arr[i][j]);
            if (j != m-1) printf(",");
        }
        printf("\n");
    }
}

double **get_matrix_from_file(char *filename, int *n, int *d) {
    FILE *file;
    char ch;
    int i, j;
    double **X;
    double t;

    file = fopen(filename, "r");

    /* Find number of vectors and dimension */
    *n = 0;
    *d = 0;
    while ((ch = fscanf(file, "%lf", &t)) != EOF) {
        (*d)++;
        if (ch == '\n') {
            (*n)++;
            (*d) = 0;
        }
    }

    X = alloc_2d_array(*n, *d);
    /* Read file */
    for (i = 0; i < *n; i++) {
        for (j = 0; j < *d; j++) {
            if (fscanf(file, "%lf", &X[i][j])) {};
        }
    }

    return X;
}

int main(int argc, char *argv[]){
    char *func;
    char *filename;
    double **X;
    int n, d;

    if (argc != 3) {
        printf("Usage: %s <function_name> <file_dir>\n", argv[0]);
        return 1;
    }

    func = argv[1];
    filename = argv[2];

    X = get_matrix_from_file(filename, &n, &d);

    if (strcmp(func, "sym") == 0) {
        /*Calculate the symmetric matrix*/
        double **A = sym(X, n, d);

        /*Print the symmetric matrix*/
        print_2D_array(A, n, n);

        /*Free the memory*/
        free_2d_array(X, n);
        free_2d_array(A, n);
    } 
   else if(strcmp(func, "ddg") == 0){
        /*Calculate the ddg matrix*/
        double **A = sym(X, n, d);
        double **D = ddg(A, n);

        /*Print the ddg matrix*/
        print_2D_array(D, n, n);

        /*Free the memory*/
        free_2d_array(X, n);
        free_2d_array(A, n);
        free_2d_array(D, n);
    } 
    else if(strcmp(func, "norm") == 0){
        /*Calculate the norm matrix*/
        double **A = sym(X, n, d);
        double **W = norm(A, n);

        /*Print the norm matrix*/
        print_2D_array(W, n, n);

        /*Free the memory*/
        free_2d_array(X, n);
        free_2d_array(A, n);
        free_2d_array(W, n);
    } 
    else {
        printf("Invalid function name\n");
    }

    return 0;
}
