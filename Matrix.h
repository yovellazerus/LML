
#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define EPSILON ((T)1e-6)

#define MATRIX_PRINT(mat) Matrix_print(mat, #mat)
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define ABS(x) (((x)<((T)0)) ? -(x) : (x))

typedef enum ERROR_CODE{OK, NO_MEM, D_ERROR, NO_MAT, BAD_INDEX, UNKNON} ERROR_CODE;
typedef struct Matrix_t* Matrix;
typedef double T;

Matrix Matrix_create(int rows, int cols, T* data);
ERROR_CODE Matrix_destroy(Matrix mat);
Matrix Matrix_copy(Matrix mat);

Matrix Matrix_id(int size);
Matrix Matrix_rand(int rows, int cols, T appToAbs);
Matrix Matrix_get_row(Matrix mat, int row);
Matrix Matrix_get_col(Matrix mat, int col);
Matrix Matrix_minor(Matrix mat, int i, int j);
Matrix Matrix_adj(Matrix mat);
Matrix Matrix_inv(Matrix mat);
Matrix Matrix_T(Matrix mat);

T Matrix_at(Matrix mat, int i, int j);
ERROR_CODE Matrix_set(Matrix mat, int i, int j, T x);
int Matrix_get_num_rows(Matrix mat);
int Matrix_get_num_cols(Matrix mat);

Matrix Matrix_add(Matrix a, Matrix b);
Matrix Matrix_sub(Matrix a, Matrix b);
Matrix Matrix_mul(Matrix a, Matrix b);
Matrix Matrix_scaler(Matrix mat, T lamda);
bool Matrix_eq(Matrix a, Matrix b, T tolerance);

T Matrix_trace(Matrix mat);
T Matrix_det(Matrix mat);
T Matrix_frob(Matrix mat);
int Matrix_rank(Matrix mat); // TODO

ERROR_CODE Matrix_print(Matrix mat, const char* name);










#endif // MATRIX_H
