#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int
main(int argc, char **argv)
{
	int i,N;
	double num=0.0,den=0.0;

	N = atoi(argv[1]);

	for(i=0;i<=N;i++){
		num += sqrt(1.0/(double)(i+1));
	}
	den = (double)(N+3);
	num = num*num;
	printf("num = %g, den = %g, ratio = %g\n", num,den,num/den);
	return 0;
}

	
	
