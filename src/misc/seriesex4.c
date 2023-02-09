/* print out numerical table of partial sums of 3 example series */

#define TERMS 100
#include <stdio.h>
#include <math.h>

float power(float base, int exponent);
main()
{
	int i;
	float s1=0.0,s2=0.0,s3=0.0;

	for (i=1;i<=TERMS;i++){
		s3 +=power(-1.0,i+1)*sin((double)i)*sin((double)i);
		s2 +=power(-1.0,i+1)*((float)(i+1.0)/(float )i);
		s1 +=power(-1.0,i+1)*1/((float)(2*i-1));
		printf(" %d  %10.6f  %10.6f  %10.6f  %10.6f    \n"
			 , i,1.0/((float)(2*i-1)),s1,s2,s3);
		} 
}
/* power function */

float power(float base, int m)
{
	float p = 1.0;
	int i;

	for(i=1;i <=m; ++i)
		p = p*base;
	return p;
}

