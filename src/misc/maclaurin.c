/* maclaurin.c print out table of values of first 5 Maclaurin polys
of exp(x) at a selection of points. For MAT397 */

#include <stdio.h>
#include <math.h>


#define INC 0.25
#define MAX 1.75
#define MAXDEG 6

int main()
{
	double x = 0.0;
	int degree = 0;
	double p = 1.0;
	int c;
	int cum = 1;

	for(c=0;c<=(int)(MAX/INC);c++){
		printf("%.2f  %.4f  ",x,exp(x));
		p=1.0;
		cum = 1;
		for(degree = 1;degree <= MAXDEG;degree++){
		p = p + pow(x,(double)degree)/(cum*degree);
		cum = cum*degree;
		printf("%.4f   ",p);
		}
		printf("\n");
		x = x + INC;
	}
	return 0;
}

	
