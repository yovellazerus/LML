

#include "Matrix.h"
#include <time.h>


int main(int argc, char** argv)
{
	char* end;


	if(argc < 4){
		fprintf(stderr, "\n\033[31musege: <scanMatrix> <rows> <cols> <a11 a12 a13 ... am1 am2 am3 ... amn>m\n");
		return 1;
	}
	if(strtod(argv[1], &end)*strtod(argv[2], &end) != argc - 3){
		fprintf(stderr, "\n\033[31merror: number of elemnts is not roes*colsm\n");
		return 1;
	}

	double* arr = malloc(sizeof(*arr) * (argc - 3));
	if(!arr){
		fprintf(stderr, "\n\033[31mno memorym\n");
		return 1;
	}

	for(int i = 3; i < argc; i++){
		arr[i-3] = strtod(argv[i], &end);
	}

	Matrix mat = Matrix_create(strtod(argv[1], &end), strtod(argv[2], &end), arr);
	if(!mat){
		fprintf(stderr, "\n\033[31mno memorym\n");
		return 1;
	}

	Matrix_print(mat, "mat");

	return 0;


}
