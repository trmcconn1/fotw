/* Test Hardy-Littlewood type inequality */

/* Enter a list of integers, which will be used as the 
	b_i in the inquality

SUM (b_0+...+b_n)^4/(n+1)(n+2)(n+3) <= C(b_0^2 + ...)^2   */

#include<stdlib.h>
#include<stdio.h>
#include<math.h>

#define MAX_COEFFS 100

int
main(int argc, char **argv)
{

	int N;
	int n,j,i;
	double sum = 0.0;
	double lhs;
	double inner_sum;
	double last_inner_sum = 0.0;
	double b[MAX_COEFFS];

	N = argc-1; /* Number of nonzero coefficients. */

   /* Read in the coefficients */

	for(i=0;i<N;i++)
		b[i] = (double)atoi(argv[i+1]);

/* Calculate the left side */

	for(n=0;n<N;n++){
		inner_sum = 0.0;
		for(j=0;j<= n;j++)	
			inner_sum += b[j];
		inner_sum = inner_sum*inner_sum;
		last_inner_sum = inner_sum;
		sum += inner_sum*inner_sum/((double)(n+1)*(double)(n+2)*(double)(n+3));
	}
	sum += last_inner_sum*last_inner_sum*0.5/((double)(N+1)*(double)(N+2)); 
	lhs = sum;
	printf("Left side is %.5g\n",sum);

/* Calculate the right-hand side */

	sum = 0.0;
	for(n=0;n<N;n++)
		sum += b[n]*b[n];
	sum = sum*sum;
	printf("Right side is %.5g\n",sum);
	printf("Left side / Right side = %.5g\n", lhs/sum);
	return 0;
}
