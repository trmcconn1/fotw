/* multiletter.c: calculate probabilities of the possible truth functions in
   N letter sentential calculus statements generated at random */

#include<stdio.h>
#include<stdlib.h>

#define N 2  /* Number of letters */
#define iterations 10

double p_and = 1/3.0;
double p_not = 1/3.0;

double p_l[N] = {1/2.0, 1/2.0};




int main(int argc, char **argv)
{

	long n,m,mask;
	int a,b;
	int letter_masks[N];
	double *x, *x_old, sum;
	int i,j,k,l;

	/* compute 2^(2^n) = number of truth functions */
	/* set up mask for first m = 2^n binary digits */

	n = 1L;
	m = 1L;
	for(i=0;i<N;i++) m *= 2L;
	mask = 0L;
	for(i=0;i<m;i++) {
		n *= 2L;
		mask = mask<<1;
		mask = mask + 1L;
	}


	/* Allocate memory for truth function variables */

	x = (double *)malloc(n*sizeof(double));
	x_old = (double *)malloc(n*sizeof(double));
	if(x == NULL || x_old == NULL){
		fprintf(stderr,"Cannot allocate memory for variables.\n");
		return 1;
	}

	
	/* Initialize letter masks */

	for(i=0;i<N;i++)letter_masks[i]=0;
	a = 1;
	for(j=0;j<N;j++){
		for(k=0;k<m;k++){
			b = 1;
			for(l=1;l<(k|a);l++)b = b<<1;
			letter_masks[j] |= b;
		}
		a = a<<1;
	}	
	
	/* Initialize variables to equal probabilities */


	for(i=0;i<n;i++)
		x[i] = 1.0/((double)n);


		
	/* iteration loop to find fixed point */

	for(l=0;l<iterations;l++){

		for(i=0;i<n;i++)x_old[i] = x[i];

		/* Update variables */
		for(i=0;i<n;i++){

			x[i]=0.0;
			for(j=0;j<n;j++)
				for(k=0;k<n;k++)
					if((j&k) == i)
						x[i] += x_old[j]*x_old[k]*p_and;
					
			j = (~i)&mask;
			x[i] += p_not*x_old[j];
					
			for(j=0;j<N;j++){
				if(i == letter_masks[j]) 
					x[i] += (1.0-p_and-p_not)*p_l[j];
			}
				
		}
		/* Print stuff out */
		printf("Iteration %d:\n\n",l);
		for(i=0;i<n;i++)
			printf("Variable %d = %8.5f\n",i,x_old[i]);
		sum = 0.0;
		for(j=0;j<n;j++) sum += x_old[j];
		printf("Sum = %8.5f\n",sum);
		printf("\n\n");
	}

	return 0;
	
}
