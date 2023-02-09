/* Example showing how to invert a matrix using the Gnu Scientific Libarary (GSL)

compile:
gcc gsl_example.c -lgsl -lgslcblas -lm
*/

#include<stdio.h>
#include<gsl/gsl_linalg.h>
#include<gsl/gsl_matrix.h>

int
main(void)
{
	/* Data for the example matrix to be inverted */
	double my_data[] = {1,0,2,
			    2,-1,3,
			    4,1,8};
	int s; /* holds sign of permutation. We don't use it, but it must exist */

	/* This seems to be the preferred way to create a matrix from data. A view struct lives on the stack
            and so does not need to be freed. The matrix itself resides in the matrix field of this struct */
	gsl_matrix_view m = gsl_matrix_view_array(my_data,3,3);

	/* Allocate a matrix to hold the answer */
	gsl_matrix *inverse_matrix = gsl_matrix_alloc(3,3);

	/* The next 2 commands do an LU decomposition on the matrix in place. The inversion routine assumes this has
            been done first, as do quite a few other library matrix operation routines. */
	gsl_permutation *p = gsl_permutation_alloc(3);
	gsl_linalg_LU_decomp(&m.matrix,p,&s);

	gsl_linalg_LU_invert(&m.matrix,p,inverse_matrix);
	printf("The inverse matrix = \n");

	/* This just dumps the matrix row by row, one element to a line. Nothing fancy */
	gsl_matrix_fprintf(stdout,inverse_matrix,"%f");

	/* Clean up, for form's sake */
	gsl_matrix_free(inverse_matrix);
	gsl_permutation_free(p);
	return 0;
}
