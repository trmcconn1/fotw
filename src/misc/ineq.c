/* Test conjectured inequality Lifetime < (4ln(2)-2)*P^2/4pi^2 */

/* Enter a list of integers, which will be used as the coefficients
   of the square root of the derivative of the mapping function */

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
	double inner_sum;
	double inner2_sum;
	double even_tail = 0.0;
	double odd_tail = 0.0;
	double last_even_sum = 0.0;
	double last_odd_sum = 0.0;
	double b[MAX_COEFFS];

	N = argc-1; /* Number of nonzero coefficients. */

   /* Read in the coefficients */

	for(i=0;i<N;i++)
		b[i] = (double)atoi(argv[i+1]);
	for(i=N;i<=2*N;i++)
		b[i] = 0.0;

/* Calculate the left side */

	for(n=0;n<2*N-1;n++){
		inner_sum = 0.0;
		for(j=0;j<= n/2;j++){	
			inner2_sum = 0.0;
			for(i=0;i<= n - 2*j;i++)
				inner2_sum += b[i]*b[n-2*j-i];
			inner_sum += inner2_sum;
		}
		
		if(n%2) last_odd_sum = inner_sum;
		else last_even_sum = inner_sum;
		sum += inner_sum*inner_sum/((double)(n+1)*(double)(n+2)*(double)(n+3));
	}
	for(i=2*N-1;i<500;i++)
		if(i%2)
			odd_tail += 1.0/((double)(i+1)*(double)(i+2)*(double)(i+3));
		else	
			even_tail += 1.0/((double)(i+1)*(double)(i+2)*(double)(i+3));
	sum += last_odd_sum*last_odd_sum*odd_tail 
		+ last_even_sum*last_even_sum*even_tail; 
	sum *= 4;
	printf("Left side is %.5g\n",sum);

/* Calculate the right-hand side */

	sum = 0.0;
	for(n=0;n<N;n++)
		sum += b[n]*b[n];
	sum = sum*sum;
	sum *= 4*log(2.0) - 2.0;
	printf("Right side is %.5g\n",sum);
	return 0;
}
