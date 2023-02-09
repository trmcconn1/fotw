
#include <stdlib.h>
#include <stdio.h>

extern int kepler(double *E, double ,double,double,int);

int
main()
{

	double M,E,e;
	int n,method;

	e = .99;

	for(method = 0; method < 3; method++){
	printf("\n\nMethod %d\n\n",method);
	for(M=0;M<1.0;M += .1){
		if((n=kepler(&E,M,e,.000000001,method))==-1){
			printf("failed at M = %.7f\n",M);
			exit(1);
		}
		printf("M = %.7f, E = %.7f (count = %d)\n",M,E,n);
	}
	}
	return 0;
}
