/* supporting routines for regression.c */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double dotproduct( double *X, double *Y, int n)
{
	double a,b;
	double sum = 0.0;
	int i=0;


	while (i<n) {
		a = *(X+i);
		b = *(Y+i);
		sum = sum + a*b;
		i++;
	}  
	return sum; 
}

double residuals( double *X, double *Y, double m, double b, int n)
{
	double x,y,sum = 0.0;
	int i=0;

	
	while ( i < n ) {
		x = *(X+i);
		y = *(Y+i);
		sum = sum + (y - m*x - b)*(y - m*x - b);
		i++;
	}
		
	return sum;
}

double orthogonal_residuals( double *X, double *Y, double theta, double lambda, int n)
{
	double x,y,sum = 0.0;
	int i=0;

	
	while ( i < n ) {
		x = *(X+i);
		y = *(Y+i);
		sum = sum + (lambda + x*cos(theta) + y*sin(theta))*(lambda + x*cos(theta) + y*sin(theta));
		i++;
	}
		
	return sum;
}
	
double summation( double *X, int n )
{
	double sum = 0.0;
	double term;
	int i = 0;

	while ( i < n ) {
		term = *(X+i);
		sum = sum + term;
		i++;
	}
	return sum;
}

double normsquare( double *X, int n)
{
	double a;
	double sum = 0.0;
	int i =  0;


	while ( i < n) {
		a = *(X+i);
		sum = sum + a*a;
		i++;
	}
		
	return sum;  
		
}
