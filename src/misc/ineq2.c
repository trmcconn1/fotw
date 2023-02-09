/* Test Hardy-Littlewood type inequality */

/* Enter a lagrange multiplier lambda that will be used to find the  
	b_i in the inquality

SUM (b_0+...+b_n)^4/(n+1)(n+2)(n+3) <= C(b_0^2 + ...)^2   */

/* The Euler-Lagrange equations say b_k+1 = b_k - (2/lambda)*S_k^3/(k+1)(k+2)(k+3) */

#include<stdlib.h>
#include<stdio.h>
#include<math.h>

#define MAX_COEFFS 100

double rhs(double *,int);
double lhs(double *,int);

int
main(int argc, char **argv)
{
	int n,i;
	double sum = 0.0;
	double b[MAX_COEFFS+1];
	double lambda,l_rhs;
	
	b[0] = (double)atof(argv[1]);
	lambda = (double)atof(argv[2]);

	for(n=0;n<MAX_COEFFS;n++){

		sum = 0.0;
		for(i=0;i<n;i++)
			sum += b[i];
		b[n+1] = b[n] - 2.0*sum*sum*sum/(lambda*(double)(n+1)*(double)(n+2)*(double)(n+3));
		printf("b[%d] = %.5g\n",n+1,b[n+1]);
	}
	printf("rhs = %.5g\n", l_rhs = rhs(b,MAX_COEFFS));
	printf("Left side / Right side = %.5g\n", lhs(b,MAX_COEFFS)/l_rhs);
	return 0;
}


double rhs(double *dptr, int N)
{
	int i;
	double sum = 0.0;
	double b;
	
	for(i=0;i<N;i++){
		b = *(dptr+i);
		sum += b*b;
	}
	return sum;
}

double lhs(double *b, int N)
{
	int n,j,i;
	double sum = 0.0;
	double lhs;
	double inner_sum;
	double last_inner_sum = 0.0;

/* Calculate the left side */

	for(n=0;n<N;n++){
		inner_sum = 0.0;
		for(j=0;j<= n;j++)	
			inner_sum += *(b+j);
		inner_sum = inner_sum*inner_sum;
		last_inner_sum = inner_sum;
		sum += inner_sum*inner_sum/((double)(n+1)*(double)(n+2)*(double)(n+3));
	}
	sum += last_inner_sum*last_inner_sum*0.5/((double)(N+1)*(double)(N+2)); 
	lhs = sum;

	return lhs;
}
