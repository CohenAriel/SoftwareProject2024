# ifndef SYMNMF_H_
# define SYMNMF_H_

void free_2d_array(double** arr);
double** symnmf(double** H, double** W);
double** sym(double** X);
double** ddg(double** X);
double** norm(double** X);

# endif
