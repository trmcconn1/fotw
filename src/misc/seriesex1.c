/* print out numerical table of partial sums of 3 example series */

#define TERMS 20
#include <stdio.h>

float power(float base, int exponent);
main()
{
	int i;
	float s1=0.0,s2=0.0,s3=0.0;

	for (i=1;i<=TERMS;i++){
		s1 +=100.0*power(0.1,i);
		s2 +=power(-1.0,i)*(float)(i/(i+1.0));
		s3 +=1.0/(power((float)i,4) +1.0);
		printf("%d\t%10.6f\t%10.6f\t%10.6f\n", i,s1,s2,s3);
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

