#include<stdio.h>
#include<stdlib.h>
#include<math.h> 


extern double simpson(double(*)(double,double),double,double,double,double);

double f(double x, double derror){

	return sin(x)*sin(x)*sin(x)*cos(x)*sin(1.1*x)*cos(1.1*x);
}

int main()
{

	printf("%10.7f\n",simpson(f,100.0,0.0,100.0,.0001)/100.0);
	return 0;
}
