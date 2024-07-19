# ifndef SYMNMF_H_
# define SYMNMF_H_

void free_2d_array(double** arr, int n);
double** symnmf(double** H, double** W, int n, int k);
double** sym(double** X, int n, int d);
double** ddg(double** A, int n);
double** norm(double** A, int n);

# endif