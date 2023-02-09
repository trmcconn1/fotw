/* inttest.c: test platform for numeric integration */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define DERROR .00000001
#define PI 3.141592653458

double a = 0.0, b = PI;

extern double simpson(double(*)(double,double),double,double,double,double);


double f(double x, double t_error)
{
	return sin(x);
}

int main()
{
	printf("Integral is %20.8f\n", simpson(f,1.0,a,b,DERROR));
	return 0;
}
