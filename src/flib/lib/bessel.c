/* bessel.c: Implements Bessel functions of real and imaginary argument 

   Cylinder functions of order nu are solutions of the ode
    
   	y'' + (1/x)y' + (1 - nu^2/x^2)y = 0.

   When nu = n, an integer, a series solution that is well-behaved at 0
   is called the Bessel function of order n, J_n(x). A companion solution,
   N_n, the Neumann function of order n (or Bessel function of 2nd type) is
   singular at the origin.  Together, J_n and N_n span the solution set of
   Bessel's equation (above.) 

   For background on the Bessel and other cylinder functions, including 
   proofs of all the above statements, see, e.g., Special Functions and Their
   Applications, by N.N. Lebedev and Richard R. Silverman, Dover, NY, 1972, pp.    238-250. Note that they use Y_n in place of N_n. 

   This file implements the following functions, which are intended to be
   added to a library of standalone callable routines.

	extern double besselJ(double nu, double x, double t_error);
	extern double besselN(double nu, double x, double t_error);
	extern double besselI(double nu, double x, double t_error);
	extern double besselK(double nu, double x, double t_error);

   Compile this file with cc -c 

   Bugs:
		- We only support real number computations.

   Author: Terry R. McConnell
	   trmcconn@syr.edu
*/

#include<math.h> /* For fabs, pow */
#include<float.h>
#include<stdlib.h>
#include<stdio.h>
#include "global.h"

extern real_number my_gamma(real_number,real_number);
real_number besselJasymptotic(real_number nu, real_number x, real_number *t_error);

/* The following routine calculates the Bessel function of the first kind,
 * J_nu. For an integer index, vu = n, it has a series representation given by


                      __                     2j
	              \        j        (x/2)
J (x) = 1/n! (x/2)^n       (-1)   __________________
 n	              /__          j!(n+1)...(n+j)
		      j=0

When vu is not an integer, we replace factorials with gamma functions. 
In that case, the function is not defined and should not be called with x <= 0. 

The series solution produces very poor accuracy when x is large because the
terms become very large before they become small, and there may be a loss
of precision. If the requested truncation error is not met by summing the
series, we try an asymptotic expansion instead. 

If in the end the requested truncation error cannot be met, we report an error.

*/

real_number besselJ(real_number nu, real_number x, real_number t_error)
{
	real_number dsum=0.0,dterm,s_j,d_n,d_j,cfact=1.0,t,s,t_error_orig;
	int bad_precision = 0;
	int j,n,nn;

	t_error_orig = t_error;

	n = (int)nu;
	
	nn = n >= 0 ? n : -n;  /* Absolute value of n. Use the relation
                                  J  (x) = (-1)^n J  (x) for negative n 
		                   -n              n    */

	d_n = fabs(nu);       /* Absolute value of nu. Use in place of nu. */

	/* Attach appropriate sign for return */

	if(n < 0)
		s_j = nn%2 ? -1.0 : 1.0;

	/* The order non-integer case should always be called off the
	 * branch cut, x < 0. The order integer case is an even function
	 * for even n and odd otherwise */

	if(x < 0.0){
		s_j *= (nn%2 ? -1.0 : 1.0);
	        x = fabs(x);	
	}

	/* Calculate the common factor (x/2)^n/n! so it only has to be
           done once. */
	
	
	if(nu == floor(nu)){ /* nu = n, an integer */
		for(j=1;j<=nn;j++){
			d_j = (real_number)j;
			cfact *= x/(2.0*d_j);
		}
		mults += 1; divs += 1;
	}
	else  { /* We need to call our gamma with the appropriate t_error. */
		/* Our Bessel function is the product CD, where C is cfact,
		 * the part that is subject to library error, and D is an
		 * alternating series that we can compute "exactly." If C is
		 * subject to error d, then CD is subject to error dD. Thus,
		 * make a quick and dirty calculation of D to within +/- 1 */

		dsum = dterm = 1.0,j=1;
		do {
			d_j = (real_number)j;
			s_j = j%2 ? -1.0: 1.0;
			dterm *= x*x/(d_j*4.0*(d_n + d_j));
			dsum += s_j*dterm;
			j++;

		} while (dterm > 1.0);
		t_error /= (fabs(dsum)+1.0); /* Now the multiplicative error is
					        handled */

		/* Next, the gamma we need to compute is in the denominator.
		 * With b non-negative, |a/(b+d) - a/b| <= |a/b|(1/b)d. It
		 * is sufficient to call my_gamma with t_error divided by any
		 * upper bound of |a/b|(1/b). */

		t = my_gamma(d_n+1,0.1);  /* t >= 0.9, clearly.*/
		t = pow(x/2.0,d_n)/((t - 0.1)*(t - 0.1)); 
		if(t < 1.0)t =  1.0;
		cfact = pow(x/2.0,d_n)/my_gamma(d_n+1,t_error/t);
		divs += 1;
	}

	/* j = 0 term: */
	dsum = dterm = cfact;

	j = 1;

	do {
		mults += 2; divs += 2; 
		if(INEXACT(dterm,t_error)) bad_precision = 1;
		d_j = (real_number)j;
		s_j = j%2 ? -1.0: 1.0;
		dterm *= (x/(2.0*d_j))*(x/(2.0*(d_n + d_j)));
		dsum += s_j*dterm;
		adds += 1;
		j++;
	} while( dterm > t_error ); /* alternating series error estimate */
		

	if(bad_precision){

		/* Try to do the calculation using the asymptotic series */
			reset_roundoff();
			t = besselJasymptotic(nu,x,&s);
		        if(s < t_error_orig)return s_j*t;
			else	
			fprintf(stderr,"Bessel:Insufficient precision at requested truncation error.\n");
	}
	return  n >= 0 ? dsum : s_j*dsum;
}

/* An asymptotic expansion is more accurate for large x, though for fixed
 * x it is a divergent formal series. There is an optimum number of terms for
 * a given x. Thus, it makes no sense to ask for a given truncation error.
 * Rather, the function returns the truncation error it is able to achieve
 * through the t_error argument. 
 *
 * It can be shown that sqrt(pi*x/2)J_n(x) has an expansion in the form
 *  A_n cos(x - nu*pi/2 - pi/4) - B_n sin(x - nu*pi/2 - pi/4)
 *  where A_n and B_n are nth partial sums of asymptotic series. A_n
 *  contains only even negative powers of (2x) and B_n contains only
 *  odd negative powers.  The coefficient of (2x)^-(2k) in A_n is 
 *  (-1)^k V(nu,2k) and the coefficient of (2x)^(-2k-1) in B_n is
 *  (-1)^k V(nu,2k+1). The definition ov V(nu,2k) is (-1)^k (1/2 - nu)_k
 *  (1/2 + nu)_k/k! or [(4nu^2-1)(4nu^2 - 9)...(4nu^2 - (2k-1)^2)]/(2^(2k))
 *  (k!)](-1)^k.
 *
 *  This should be called only for positive x.
 */ 

real_number besselJasymptotic(real_number nu, real_number x, real_number *t_error){
	real_number An,Bn,an,bn,minte,n,y;

	/* ak and bk hold the values of the odd and even terms in the
	 * asymptotic series A_n and B_n resp. */

	/* Normalize the angle to lie between -pi and pi. We're not sure
	 * what method the libs use to calculate trig functions but the
	 * docs explicitly say to be wary of roundoff error for large x. */

	y = x - nu*PI/2.0 - PI/4.0;
	while(y <= -PI){
		y+=2.0*PI;
		adds += 1;
	}
	while(y > PI){
		y -= 2.0*PI;
		subs += 1;
	}
	mults += 1;
	An = an = sqrt(2.0/(PI*x))*cos(y);
	Bn = bn = -sqrt(2.0/(PI*x))*sin(y)*
		        (0.5-nu)*(0.5+nu)/(2.0*x);
	mults += 5; divs += 3; subs += 3; adds += 1;

	n = 1.0;
	minte = fabs(an);
	if(fabs(bn)<minte)minte = fabs(bn);

	while(1){

		an *= (-1.0)*(0.5-nu+2.0*n-2.0)*(0.5+nu+2.0*n-2.0)*
			       (0.5-nu+2.0*n-1.0)*(0.5+nu+2.0*n-1.0)/
			       ((2.0*n-1.0)*n*8.0*x*x); 
		bn *= (-1.0)*(0.5-nu+2.0*n-1.0)*(0.5+nu+2.0*n-1.0)*
			       (0.5-nu+2.0*n)*(0.5+nu+2.0*n)/
			       (n*(2.0*n+1.0)*8.0*x*x); 
		if(minte == 0.0)break;  /* This will happen for half integer
					    order */

		if(fabs(an) > minte)break;
		minte = fabs(an);
		An += an;
		if(fabs(bn) > minte)break;
		minte = fabs(bn);
		Bn += bn;
		divs += 1; mults += 3;
		adds += 1;
		subs += 1;
		n += 1.0; 
	}

	*t_error = minte;
	return An - Bn;
}


