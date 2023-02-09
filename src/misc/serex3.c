/* serex3.c - print out partial sums of \sum \frac1{i^2}  */

#include <stdio.h>

main()
{
	int n;
	float sum = 0.0;

	for (n=1; n<=100;n++){
	sum += 1.0/((float) n*n);
	printf(" %d\t %f \n",n,sum);
	}
}

