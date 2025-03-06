
#include "Matrix.h"

struct Matrix_t{
	int rows;
	int cols;
	T* data;
};

T rand_T(T appToAbs)
{
	if(appToAbs <= 0) return (T)0;
	T res = (T)rand() / (T)RAND_MAX;
	int sign = 1;
	if(rand() % 2 == 0) sign = -1;
	return res * appToAbs * sign;

}

Matrix Matrix_create(int rows, int cols, T* data){
	if(rows <= 0 || cols <= 0){
		return NULL;
	}
	Matrix res = malloc(sizeof(*res));
	if(!res){
		return NULL;
	}
	T* new_data = malloc(sizeof(*new_data)*cols*rows);
	if(!new_data){
		free(res);
		res = NULL;
		return NULL;
	}
	if(data == NULL){
		for(int i = 0; i < cols*rows; i++){
			new_data[i] = (T)0;
		}
	}
	else{
		for(int i = 0; i < cols*rows; i++) new_data[i] = data[i];
	}

	res->data = new_data;
	res->rows = rows;
	res->cols = cols;
	return res;
}

ERROR_CODE Matrix_destroy(Matrix mat){
	if(!mat) return NO_MAT;
	free(mat->data);
	mat->data = NULL;
	free(mat);
	mat = NULL;
	return OK;
}

T Matrix_at(Matrix mat, int i, int j){
	if(i <= 0 || j <= 0) return (T)0;
	if(!mat) return (T)0;
	return mat->data[(i-1)*mat->cols + (j-1)];
}

ERROR_CODE Matrix_print(Matrix mat, const char* name){

	if(name != NULL){
		printf("%s = ");
	}
	if(!mat){
		printf("NULL\n");
		return NO_MAT;
	}
	printf("[\n");
	for(int i = 1; i <= mat->rows; i++){
		for(int j = 1; j <= mat->cols; j++){
			printf("  %.2f", Matrix_at(mat, i, j));
		}
		printf("\n");
	}
	printf("]\n");

	return OK;
}

ERROR_CODE Matrix_set(Matrix mat, int i, int j, T x){
	if(!mat) return NO_MAT;
	if(i <= 0 || j <= 0 || i > mat->rows || j > mat->cols) return BAD_INDEX;
	mat->data[(i-1)*mat->cols + (j-1)] = x;
	return OK;
}


Matrix Matrix_id(int size){
	if(size <= 0) return NULL;
	Matrix res = Matrix_create(size, size, NULL);
	if(!res) return NULL;
	for(int i = 1; i <= res->rows; i++){
		for(int j = 1; j <= res->cols; j++){
			if(i == j) Matrix_set(res, i, j, (T)1);
		}
	}
	return res;
}

Matrix Matrix_T(Matrix mat){
	if(!mat) return NULL;
	Matrix res = Matrix_copy(mat);
	if(!res) return NULL;
	int tmp = res->cols;
	res->cols = res->rows;
	res->rows = tmp;
	for(int i = 1; i <= res->rows; i++){
		for(int j = 1; j <= res->cols; j++){
			Matrix_set(res, i, j, Matrix_at(mat, j, i));
		}
	}
	return res;
}

Matrix Matrix_copy(Matrix mat){
	if(!mat) return NULL;
	Matrix res = Matrix_create(mat->rows, mat->cols, mat->data);
	if(!res) return NULL;
	for(int i = 1; i <= res->rows; i++){
		for(int j = 1; j <= res->cols; j++){
			Matrix_set(res, i, j, Matrix_at(mat, i, j));
		}
	}
	return res;
}

Matrix Matrix_scaler(Matrix mat, T lamda){
	if(!mat) return NULL;
	Matrix res = Matrix_copy(mat);
	if(!res) return NULL;
	for(int i = 1; i <= res->rows; i++){
		for(int j = 1; j <= res->cols; j++){
			Matrix_set(res, i, j, lamda * Matrix_at(mat, i, j));
		}
	}
	return res;
}

T Matrix_trace(Matrix mat){
	T res = (T)0;
	if(!mat) return res;
	if(mat->rows != mat->cols) return res;
	for(int i = 1; i <= mat->rows; i++){
		for(int j = 1; j <= mat->cols; j++){
			if(i ==j) res += Matrix_at(mat, i, j);
		}
	}
	return res;
}

Matrix Matrix_add(Matrix a, Matrix b){

	if(!a || !b) return NULL;
	if(a->rows != b->rows || a->cols != b->cols) return NULL;
	Matrix a_copy = Matrix_copy(a);
	for(int i = 1; i <= a_copy->rows; i++){
		for(int j = 1; j <= a_copy->cols; j++){
			Matrix_set(a_copy, i, j, Matrix_at(a, i, j) + Matrix_at(b, i, j));
		}
	}
	return a_copy;
}

Matrix Matrix_sub(Matrix a, Matrix b){
	return Matrix_add(a, Matrix_scaler(b, (T)(-1)));
}

Matrix Matrix_rand(int rows, int cols, T appToAbs)
{
	if(rows <= 0 || cols <= 0 || appToAbs <= (T)0) return NULL;
	Matrix res = Matrix_create(rows, cols, NULL);
	if(!res) return NULL;
	for(int i = 1; i <= rows; i++){
		for(int j = 1; j <= cols; j++){
			Matrix_set(res, i, j, rand_T(appToAbs));
		}
	}
	return res;

}

Matrix Matrix_get_row(Matrix mat, int row)
{
	if(!mat || row <= 0 || row > mat->rows) return NULL;
	Matrix res = Matrix_create(1, mat->cols, NULL);
	for(int j = 1; j <= mat->cols; j++){
		Matrix_set(res, 1, j, Matrix_at(mat, row, j));
	}
	return res;
}

Matrix Matrix_get_col(Matrix mat, int col)
{
	if(!mat || col <= 0 || col > mat->cols) return NULL;
	Matrix res = Matrix_create(mat->rows, 1, NULL);
	for(int i = 1; i <= mat->rows; i++){
		Matrix_set(res, i, 1, Matrix_at(mat, i, col));
	}
	return res;
}

Matrix Matrix_minor(Matrix mat, int i, int j)
{
	int set = 0;
	if(!mat || i <= 0 || j <= 0 || i > mat->rows || j > mat->cols || mat->cols != mat->rows) return NULL;
	Matrix res = Matrix_create(mat->rows - 1, mat->cols - 1, NULL);
	if(!res) return NULL;
	for(int i_in = 1; i_in <= mat->rows; i_in++){
		for(int j_in = 1; j_in <= mat->cols; j_in++){
			 if(!(i_in == i || j_in == j)){
				res->data[set] = Matrix_at(mat, i_in, j_in);
				set++;
			 }
		}
	}
	return res;
}


T Matrix_det(Matrix mat)
{
	if(!mat) return (T)0;
	if(mat->cols != mat->rows) return (T)0;

	if(mat->rows == 1) return mat->data[0];

	T res = (T)0;
	for(int j = 1; j <= mat->cols; j++){
		res += pow(-1, 1+j) * Matrix_at(mat, 1, j) * Matrix_det(Matrix_minor(mat, 1, j));
	}
	return res;
}

int Matrix_get_num_rows(Matrix mat)
{
	if(!mat) return 0;
	return mat->rows;
}

int Matrix_get_num_cols(Matrix mat)
{
	if(!mat) return 0;
	return mat->cols;
}

T Matrix_frob(Matrix mat)
{
	if(!mat) return (T)0;
	T res = (T)0;
	for(int i = 1; i <= mat->rows; i++){
		for(int j = 1; j <= mat->cols; j++){
			 res += pow(Matrix_at(mat, i, j), 2);
		}
	}
	res = sqrt(res);
	return res;
}

Matrix Matrix_mul(Matrix a, Matrix b)
{
	if(!a || !b) return NULL;
	if(a->cols != b->rows) return NULL;
	Matrix res = Matrix_create(a->rows, b->cols, NULL);
	int m = a->cols;
	for(int i = 1; i <= res->rows; i++){
		for(int j = 1; j <= res->cols; j++){
			 T element = (T)0;
			 for(int k = 1; k <= m; k++)
				element += Matrix_at(a, i, k)*Matrix_at(b, k, j);
			 Matrix_set(res, i, j, element);
		}
	}
	return res;
}

Matrix Matrix_inv(Matrix mat)
{
	if(!mat) return NULL;
	if(mat->cols != mat->rows) return NULL;
	if(Matrix_det(mat) == (T)0) return NULL;

	Matrix res = Matrix_copy(mat);
	if(!res) return NULL;
	for(int i = 1; i <= res->rows; i++){
		for(int j = 1; j <= res->cols; j++){
			 Matrix_set(res, i, j, pow(-1, i+j)*Matrix_det(Matrix_minor(mat, j, i)) / Matrix_det(mat));
		}
	}
	return res;

}

Matrix Matrix_adj(Matrix mat)
{
	if(!mat) return NULL;
	if(mat->cols != mat->rows) return NULL;

	Matrix res = Matrix_copy(mat);
	if(!res) return NULL;
	for(int i = 1; i <= res->rows; i++){
		for(int j = 1; j <= res->cols; j++){
			 Matrix_set(res, i, j, pow(-1, i+j)*Matrix_det(Matrix_minor(Matrix_T(mat), i, j)));
		}
	}
	return res;



}

bool Matrix_eq(Matrix a, Matrix b, T tolerance)
{
	if(!a || !b) return false;
	if(a->cols != b->cols || a->rows != b->rows) return false;
	for(int i = 1; i <= a->rows; i++){
		for(int j = 1; j <= a->cols; j++){
			 if(ABS(Matrix_at(a, i, j) - Matrix_at(b, i, j)) > tolerance)
				return false;
		}
	}
	return true;
}



