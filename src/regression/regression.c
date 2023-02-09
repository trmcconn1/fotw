/* Implementation of the various regression methods. For details on the
algorithms for orthogonal regression, see my paper

Complements on Simple Linear Regression
Preprint 2015

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define PI 3.14159265

/* Definition of the arrays of X and Y values. These
   are intialized in main */

double *X;
double *Y;

extern double dotproduct( double *, double *, int dim);
extern double normsquare( double *, int dim);
extern double summation( double *, int noterms);
extern double residuals(double *,double *, double m, double b, int dim);
extern double orthogonal_residuals(double *,double *, double m, double b, int dim);


/* Fit a line y = mx + b to (x,y) pairs using classical least squares
   regression. Return m and b through pointer argument. Return 0
   if successful, 1 if there was an error 
*/

int vertical(double *x, double *y, double n, double *m, double *b, double *ssr){

	double xbar,ybar,cov,var;

	xbar = summation( x, (int) n )/n;
	ybar = summation( y, (int) n)/n;
	cov = dotproduct (x, y, (int)n )/n;
	var = normsquare(x,(int) n)/n;

	if(var == xbar*xbar){
		fprintf(stderr,"vertical: divide by zero\n");
		return 1;
	}

	/* compute slope (m) and intercept (b) */

	*b = (ybar*var - cov*xbar)/(var - xbar*xbar);
	*m = (cov - xbar*ybar)/(var - xbar*xbar);
	*ssr = residuals(x,y,*m,*b, (int)n);
	return 0;
}
	
int orthogonal(double *x, double *y, double n, double *m, double *b, double *ssr){

	double xbar,ybar;
	double ssxy, ssx, ssy, theta, lambda;

	xbar = summation( x, (int) n )/n;
	ybar = summation( y, (int) n)/n;
	ssxy = dotproduct (x, y, (int)n ) - n*xbar*ybar;
	ssx = normsquare(x,(int) n) - n*xbar*xbar;
	ssy = normsquare(y,(int) n) - n*ybar*ybar;

	if(ssxy == 0.0){
		if(ssx == ssy){
			fprintf(stderr,"warning: no unique best line\n");
			theta = 0.0;
		}
		else {
			if(ssx > ssy)theta = PI/2;
			else theta = 0.0;
		}
	}
	else {
		if(ssx == ssy) theta = 0.0;
		theta = 0.5*atan(2.0*ssxy/(ssx - ssy)); 
		if(ssx > ssy) theta = PI/2.0 - theta;
	}

	/* compute slope (m) and intercept (b) */

	if(theta == 0){
		fprintf(stderr,"orthogonal: infinite slope\n");
		return 1;
	}

	*m = -cos(theta)/sin(theta);
	*b = ybar - (*m)*xbar;
	lambda = -cos(theta)*xbar - sin(theta)*ybar;
	*ssr = orthogonal_residuals(x,y,theta,lambda,(int)n);

	return 0;
}
