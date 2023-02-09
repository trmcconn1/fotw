/* Solve the equations of the one letter random expression model by
   iteration */


#include <stdio.h>
#include <stdlib.h>


double f1(double x,double y,double z,double w) {
	return x*x/3.0 + y/3.0;
}

double f2(double x,double y,double z,double w) {
	return 2.0*z*w/3.0 + (2.0*y - y*y)/3.0 + x/3.0;
}

double f3(double x,double y,double z,double w) {
	return z*z/3.0 + 2.0*x*z/3.0 + w/3.0 + 1/3.0;
}

double f4(double x,double y,double z,double w) {
	return w*w/3.0 + 2.0*x*w/3.0 + z/3.0;
}


int main(void)
{
	double x=.70,y=.2,z=.05,w=.05;  /* initial values */
	double x1,y1,z1,w1;
	int i;
	int n = 50;

	for(i=0;i<n;i++){
		x1  = f1(x,y,z,w);
		y1  = f2(x,y,z,w);
		z1  = f3(x,y,z,w);
		w1  = f4(x,y,z,w);
		printf("pass %d: x = %g, y = %g, z = %g, w = %g\n",
			i+1,x1,y1,z1,w1);
		x = x1;
		y = y1;
		z = z1;
		w = w1;
	}
	return 0;
}
		
