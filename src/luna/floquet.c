/* Calculate Floquet Monodromy matrix BA^-1 Using Gnu Scientific Libarary (GSL)

compile:
gcc floquet.c -lgsl -lgslcblas -lm
*/

#include<stdio.h>
#include<gsl/gsl_blas.h>
#include<gsl/gsl_linalg.h>
#include<gsl/gsl_matrix.h>
#include<gsl/gsl_eigen.h>
#include "lunadata.c"

double C_data[] = {0.0,0.0,0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0,0.0,0.0};

double V_data[] = {0.0,0.0,0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0,0.0,0.0};

double S_data[] = {0.0,0.0,0.0,0.0,0.0,0.0};
double Work_data[] = {0.0,0.0,0.0,0.0,0.0,0.0};

int
main(void)
{
	int s; /* holds sign of permutation. We don't use it, but it must exist */

	/* This seems to be the preferred way to create a matrix from data. A view struct lives on the stack
            and so does not need to be freed. The matrix itself resides in the matrix field of this struct */
	gsl_matrix_view A = gsl_matrix_view_array(A_data,6,6);
	gsl_matrix_view B = gsl_matrix_view_array(B_data,6,6);
	gsl_matrix_view C = gsl_matrix_view_array(C_data,6,6);
	gsl_vector_view S = gsl_vector_view_array(S_data,6);
	gsl_matrix_view V = gsl_matrix_view_array(V_data,6,6);
	gsl_vector_view Work = gsl_vector_view_array(Work_data,6);

	/* Allocate a matrix to hold the answer */
	gsl_matrix *inverse_matrix = gsl_matrix_alloc(6,6);
	gsl_eigen_nonsymm_workspace *myworkspace;
	gsl_vector_complex *myeval = gsl_vector_complex_alloc(6);

	/* The next 2 commands do an LU decomposition on the matrix in place. The inversion routine assumes this has
            been done first, as do quite a few other library matrix operation routines. */
	gsl_permutation *p = gsl_permutation_alloc(6);
	gsl_linalg_LU_decomp(&A.matrix,p,&s);

	gsl_linalg_LU_invert(&A.matrix,p,inverse_matrix);
	gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,
		1.0, inverse_matrix,&B.matrix,
		0.0, &C.matrix);
	myworkspace = gsl_eigen_nonsymm_alloc(6);
	gsl_eigen_nonsymm(&C.matrix,myeval,myworkspace);
	gsl_vector_view R = gsl_vector_complex_real(myeval);
	gsl_vector_view I = gsl_vector_complex_imag(myeval);

/*
	gsl_linalg_SV_decomp(&C.matrix,&V.matrix,&S.vector,&Work.vector);
*/
	/* printf("Monodromy matrix = \n");*/

	/* This just dumps the matrix row by row, one element to a line. Nothing fancy */
	gsl_vector_fprintf(stdout,&R.vector,"%g");
	gsl_vector_fprintf(stdout,&I.vector,"%g");

	/* Clean up, for form's sake */
	gsl_matrix_free(inverse_matrix);
	gsl_permutation_free(p);
	return 0;
}
