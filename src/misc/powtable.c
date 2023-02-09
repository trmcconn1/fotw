#include <stdio.h>
#include <stdlib.h>
#include <math.h>


int
main()
{
	int i;
	double x=0.0;


	for (i=1;i<=100;i++){
		x+=1.0;
			printf("%f\n",pow(2.0,x));
	}
}
