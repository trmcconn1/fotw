/* Hyper.c:
 
   Compute the confluent hypergeometric function of real parameters 
   and a real variable. 
   
   (We also compute the confluent hypergeometric function of the second
   kind -- see below. These functions are sometimes called Kummer
   functions. )

   The confluent hypergeometric function, Phi(a,c;x), is really a 2 
   parameter (a,c) family of functions of x. 

   It is a variant of the hypergeometric function, which depends upon
   an additional parameter (b). Unlike the hypergeometric function, the
   confluent function is an entire function of its variable for valid values
   of the parameters. (As a function of its parameters it is analytic except 
   for poles in c at 0, -1, -2, ...) 

   As an entire function it can always be calculated from its power series
   expansion, which is given by

  	Phi(a,c;x) = 1 + ax/c + ... + (a)_j/(j!(c)_j)x^j + ...

   where (a)_j = a(a+1)(a+2)... (j factors.)	


   For background on the confluent hypergeometric function, including 
   proofs of all the above statements, see, e.g., Special Functions and Their
   Applications, by N.N. Lebedev and Richard R. Silverman, Dover, NY, 1972, pp.    238-250.


   This file implements the following function, which is intended to be
   added to a library of standalone callable routines.

	extern double confluent(double a, double c, double x, 
		double t_error);

   Compile this file with cc -c 

   Bugs:
		- We only support real number computations.

   Author: Terry R. McConnell
	   trmcconn@syr.edu
*/

#include "global.h"
#include<stdio.h>
#include <math.h>


/* confluent: Compute the confluent hypergeometric function Phi(a,c;x) to within
 * an error < t_error.
 *
 * We compute the confluent hypergeometric function by summing the defining 
 * series. This will always converge.
 *
 * It is up to the caller to do sanity checking on the argument and
 * parameters. If you feed it bad values, you may be toast.
 */

real_number confluent(real_number a, real_number c, real_number x, real_number t_error)
{

	real_number rval = 1.0;
	real_number n = 1.0;
	real_number term = a*x/c; 
	real_number K,C;


	/* find max(|a|,|c|) for use below */

	K = fabs(a) ? fabs(c) : fabs(a) > fabs(c); 

	do {
		/* Decide whether we are accurate enough yet, i.e., if
		 * tail of series < t_error. Let An be nth term
		 * of series, and Bn = (a+n)/[(c+n)(1+n)]. Then
		 * An+1/An = Bnx. Bn --> 0, which gives radius of convergence
		 * 1. Let Cn be a monotone nonincreasing upper bound for |Bn|
		 * Then by comparison with geometric series, |An|/(1-Cnx) < 
		 * t_error is sufficient to ensure accuracy. If we let K be
		 * the max of |a|,|c|, then it is easy to check that
		 * Cn = 3/n is such a monotone upper bound when
		 * n > 2K.  
		 */

		C =  3.0/n;
		if((n>2.0*K) && (C*x < 1) && (fabs(term)/(1 - C*x) < t_error))break;
		rval += term;
		a += 1.0;
		c += 1.0;
		n += 1.0;
		term = term*a*x/(n*c);

	} while(1);   
	return rval;
}

/* The confluent hypergeometric function of the 2nd kind. This is defined
 *
 * as Psi(a,c;x) = (Gamma(1-c)/Gamma(1+a-c))Phi(a,c,x)
 *                 + (Gamma(c-1)/Gamma(a))Phi(1+a-c,2-c;x)x^(1-c)
 *
 * for positive x, if c is not an integer. 
 *
 * When c is an integer, it can be shown that lim b->c Psi(a,b;x) exists,
 * and that the function so defined is an entire function of the parameters
 * and in x away from the negative x axis. 
 *
 * Together, Psi and Phi span the space of solutions of the confluent
 * hypergeometric equation, xy'' + (c-x)y' - ay = 0.
 *
 * Accurate calculation of Psi from the definition seems problematic, esp.
 * for integer values of c. Instead we use recurrences to reduce to the
 * case a > 5, where we can use the integral representation,
 *
 *  Psi(a,c;x)*Gamma(a) = int_0^{\infty} e^{-xt)t^(a-1)(1+t)^(c-a-1)
 *
 *  (Actually, this holds as soon as a > 0, but we need a > 5 for
 *  an estimate of the 4th derivative needed in Simpson's rule.)
 */ 


extern real_number simpson(real_number(*)(real_number,real_number),real_number,real_number,real_number,real_number);
extern real_number my_gamma(real_number,real_number);
static real_number X,aa,cc;
real_number integrand(real_number t,real_number t_error){

	return exp(-X*t)*pow(t,aa-1.0)*pow(1+t,cc-aa-1.0);
}

real_number confluent2(real_number a, real_number c, real_number x, real_number t_error)
{

	real_number B,t,s,bound,partI,partII;
	int i,j;

	if( a < 5 ) return (2.0*a + 2.0 - c + x)*confluent2(a+1,c,x,t_error/2.0)
		- (a+1.0)*(a-c+2.0)*confluent2(a+2.0,c,x,t_error/2.0);

	/* For now, we assume a >= 5 */

	/* To determine how to apportion t_error between the integral factor
	 * in the numerator and the Gamma function in the denominator,
	 * consider that |X/Y - (X+d)/(Y+d)| < d[X/Y^2 + 1/Y] for nonnegative
	 * A and B. Apply this with X the integral and Y = Gamma(a). We need
	 * an upper bound on the integral. Divide the range of integration
	 * into [0,1] and [1, infinity). The first integrand is bounded 
	 * above by t^(a-1)(1+t)^(c-a-1) < max(1,2^(c-a-1))t^(a-1), which
	 * gives an upper bound of (1/a)max(1,2^(c-a-1)). The second 
	 * integrand is bounded by 4exp(-xt)(1+t)^(c-2). If c < 2 the integral
	 * is bounded by 4/x. If c >= 2, after bounding 1+t by 2t and making
	 * the change of variable s = xt, we get an upper bound of
	 * 4 2^(c-2)x^(1-c)Gamma(c-1). Combining these bounds gives a suitable
	 * value for X. For Y we use a lower bound on Gamma(a): Dropping
	 * the range of integration from 1 to infinity, lower bounding the
	 * exponential by 1/3, we get Gamma(a) > 1/(3a). */

	s = pow(2.0,c-a-1);
	if(s<1.0)s = 1.0;
	if(c < 2.0) t = 4.0/x;
	else t = 4.0*pow(2.0,c-2)*pow(x,1.0-c)*(0.1 + my_gamma(c-1,0.1));

	/* apportion error equally between factors by merely changing the
	 * value of t_error used from now on: */

	t_error = t_error/( 9.0*a*a*(s+t) + 3.0*a );

	/* We need to restrict the integral to [0,B]. We choose B
	 * so that the tail of the integral is < t_error/2. 
	 *
	 * If B >= 1 the integrand is bounded by exp(-xt)(1+t)^(c-1). 
	 * Split off one factor of exp(-xt/2) and then note that the
	 * other factor is < 1 if B > 5 and B > 4(c-1)^2/x^2 (This
	 * uses that ln(1+t) < sqrt(t) if t > 5. Then the tail integral
	 * with integrand exp(-xt/2)  is (2/x)exp(-xt/2). This is < d/2
	 * if B > (2/x)ln(4/(dx)).
	 */

	s = 4.0*(c-1.0)*(c-1.0)/(x*x);
	if(s < 5.0)s = 5.0;	
	B = (2.0/x)*log(4.0/(t_error*x));
	if(B < s)B = s;

	/* load parameters for simpson integrand */
	X = x;
	aa = a;
	cc = c;

	/* We need to pass the simpson's routine a bound on the 4th derivative
	 * of the integrand. To get a reasonable size bound it is helpful
	 * to break the integration into two parts: I over [0,1] and II over
	 * [1,B]. On the first range we use as upper bound on the 4th
	 * derivative of (fgh) the expression 125*M(f)*M(g)*M(h) where M is an
	 * upper bound on the function and its first 4 derivatives. For
	 * the exponential factor f we have max(1,x^4) >= M(f). For t^(a-1),
	 * since a>=5 an upper bound is (a-1)*(a-2)*(a-3)*(a-4). For the
	 * factor (1+t)^(c-a-1), observe this is > 1 only when the exponent
	 * is positive and the the worst case is t=1. */

        s = x*x*x*x;
	if(s < 1.0)s = 1.0;
	t = pow(2.0,c-a-1);
	if(t < 1.0)t=1.0;
	bound = 125.0*fabs((a-1)*(a-2)*(a-3)*(a-4))*s*t;

	partI = simpson(integrand,bound,0.0,1.0,t_error/4); 

	/* For the integral over [1,B], note that |f^(i)g^(j)h^(k)| <=
	 * x^i (a-1)^j (max(|c-a-1|,|c-a-5|)^k exp(-xt)t^(a-1)(1+t)^(c-a-1),
	 * i.e, since t>1 the worst case is exponents as large as possible. 
	 * The factors independent of i,j, and k are bounded by 1 if c<2
	 * and by exp(-xt)(1+t)^(c-2) if c > 2. To bound the last case
	 * change variable to u = xt, getting an upper bound of 
	 * x^(c-2) exp(-u)(x+u)^(c-2). The exp(-u)(x+u)^(c-2) <= 
	 * 2^(c-2)[x^(c-2) + max exp(-u)u^(c-2)] <= 2^(c-2)[x^(c-2) +
	 * e^(c-2)(c-2)^(c-2). */

	t = 0.0;
	for(i=0;i<= 4;i++)
		for(j=i;j<=4;j++)
			t += pow(x,i)*pow(a-1,j)*pow(fabs(c-a-1),4-i-j);
	if(c > 2) bound = t*pow(2,c-2)*(pow(x,c-2)+pow(2.8,c-2)*pow(c-2,c-2));
        else bound = t;

	partII = simpson(integrand,bound,1.0,B,t_error/4.0);

	return (partI+partII)/my_gamma(a,t_error);
}	
