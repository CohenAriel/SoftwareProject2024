#include <math.h>
#include "symnmf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void free_2d_array(double **arr) {
    int n = sizeof(arr) / sizeof(arr[0]);
    int i;
    for (i = 0; i < n; i++) {
        free(arr[i]);
    }
    free(arr);
}

/*Matrix multiplication. We assume that A is m x n, B is n x d and C is m x d */
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

float distance(double* x , double* y , int n) {
    float sum = 0;
    int i;
    for (i = 0; i < n; i++) {
        sum += pow(x[i] - y[i], 2);
    }
    return sum;
}

double **sym(double **X) {
    int n = sizeof(X) / sizeof(X[0]);
    double **A = (double **)malloc(sizeof(double*) * n);
    int i,j;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (i == j) {
                A[i][j] = 0;
                continue;
            }
            A[i][j] = distance(X[i], X[j], n);
            A[i][j] = exp(-A[i][j]/2);
        }
    }
    return A;
}

double **ddg(double **X) {
    int i,j;
    double **A = sym(X);
    int n = sizeof(A) / sizeof(A[0]);
    double **D = (double **)malloc(sizeof(double*) * n);
    for (i = 0; i < n; i++) {
        D[i][i] = 0;
        for (j = 0; j < n; j++) {
            D[i][i] += A[i][j];
            if (i != j) {
                D[i][j] = 0;
            }
        }
    }
    free_2d_array(A);
    return D;
}

double **norm(double **X) {
    int i;
    int n = sizeof(X) / sizeof(X[0]);
    double **W = (double **)malloc(sizeof(double*) * n);
    double **A = sym(X);
    double **D = ddg(A);

    double **D_norm = (double **)malloc(sizeof(double*) * n);
    for (i = 0; i < n; i++) {
        D_norm[i][i] = 1 / sqrt(D[i][i]);
    }

    /* D is used here temporarily for calculations*/
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
    int i, j, l , m;

    for (i = 0; i < 300; i++) {
        double norm;
        double** new_H = (double **)malloc(sizeof(double*) * n);
        double **numerators = (double **)malloc(sizeof(double*) * n * k);
        double **denominators = (double **)malloc(sizeof(double*) * n * k);

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

        free_2d_array(numerators);
        free_2d_array(denominators);

        /* Calculate the Frobenius norm of the difference between H and new_H*/
        norm = 0;
        for (j = 0; j < n; j++) {
            for (l = 0; l < k; l++) {
                norm += pow(new_H[j][l] - H[j][l], 2);
            }
        }
        /* Break on convergence*/
        if (norm < 0.0001) {
            free_2d_array(new_H);
            break;
        }

        H = new_H;
        free_2d_array(new_H);
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

int main(int argc, char *argv[]){
    char *func;
    char *file_dir;
    double **X;
    int n, i, j;
    FILE *file;
    char ch;

    if (argc != 3) {
        printf("Usage: %s <function_name> <file_dir>\n", argv[0]);
        return 1;
    }

    func = argv[1];
    file_dir = argv[2];

    file = fopen(file_dir, "r");
    if (file == NULL) {
        printf("Error opening file\n");
        return 1;
    }

    /*Count the number of lines in the file*/
    n = 0;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            n++;
        }
    }
    /*Reset the file pointer to the beginning*/
    rewind(file);
    
    /* Allocate memory for the matrix*/
    X = (double **)malloc(sizeof(double *) * n);
    for (i = 0; i < n; i++) {
        X[i] = (double *)malloc(sizeof(double) * n);
        for (j = 0; j < n; j++) {
            fscanf(file, "%lf", &X[i][j]);
        }
    }
    fclose(file);

    if (strcmp(func, "sym") == 0) {
        /*Calculate the symmetric matrix*/
        double **A = sym(X);

        /*Print the symmetric matrix*/
        print_2D_array(A, n, n);

        /*Free the memory*/
        free_2d_array(X);
        free_2d_array(A);
    } 
   else if(strcmp(func, "ddg") == 0){
        /*Calculate the ddg matrix*/
        double **D = ddg(X);

        /*Print the ddg matrix*/
       print_2D_array(D, n, n);

        /*Free the memory*/
        free_2d_array(X);
        free_2d_array(D);
    } 
    else if(strcmp(func, "norm") == 0){
        /*Calculate the norm matrix*/
        double **W = norm(X);

        /*Print the norm matrix*/
        print_2D_array(W, n, n);

        /*Free the memory*/
        free_2d_array(X);
        free_2d_array(W);
    } 
    else {
        printf("Invalid function name\n");
    }

    return 0;
}
