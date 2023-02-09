/* Hyper.c:
 
   Compute the hypergeometric function of real parameters and a real variable. 

   The hypergeometric function, F(a,b,c;x), is really a 3 parameter (a,b,c) 
   family of functions of x. It is analytic in its parameters except for poles
   in c a 0, -1, -2, ... . For fixed (a,b,c) it is analytic in x in the slit
   complex plane with cut from 1 to infinity along the real axis. In the unit
   disk, it has an elegant expansion

  	F(a,b,c;x) = 1 + abx/c + ... + [(a)_j(b)_j/(j!(c)_j)]x^j + ...

   where (a)_j = a(a+1)(a+2)... (j factors.)	

   The hypergeometric function satisfies many identities, some of which allow
   the series definition to be extended beyond the disk. Of particular interest
   is the identity

  	F(a,b,c;x) = (1-x)^(-a) F(a,c-b,c,x/(x-1)).
   
   Since the transformation x -> x/(x-1) maps the half plane Re(x) < 1/2 onto
   the unit disc, mapping 0 to 0, the cases below allow us to calculate F for 
   all x from minus infinity to 1 by ultimately summing a series:

  	case: x <= 0: Apply x -> x/(x-1) and then sum.
        case: x > 0:  Sum	

   For background on the hypergeometric function, including proofs of all the
   above statements, see, e.g., Special Functions and Their Applications, by
   N.N. Lebedev and Richard R. Silverman, Dover, NY, 1972, pp. 238-250.


   This file implements the following function, which is intended to be
   added to a library of standalone callable routines.

	extern double hyper(double a, double a, double c, double x, 
		double t_error);

   Compile this file with cc -c 

   Bugs:
		- We only support real number computations.

   Author: Terry R. McConnell
	   trmcconn@syr.edu
*/

#include<math.h> /* For pow */
#include "global.h"


real_number hypersum(real_number a, real_number b, real_number c, real_number x, real_number t_error);

/* hyper: Compute the hypergeometric function F(a,b,c;x) to within
 * an error < t_error.
 *
 * As explained in the header, we use an identity to handle x <= 0
 * and 0 < x < 1 separately. The hypersum routine does the real work.
 *
 * Return the result as a real_number. 
 */

real_number hyper(real_number a, real_number b, real_number c, real_number x, real_number t_error)
{
	real_number xx=x,rval = 0;

	if(x <= 0){
		x = x/(x-1.0);
		b = c - b;
	}
	rval = hypersum(a,b,c,x,t_error);
	if(xx <= 0)
		rval *= pow(1.0-xx,-a); /* use original x */

	return rval;
}

/* hypersum: return the value of F(a,b,c;x) to within error < t_error.
 *
 * We compute the hypergeometric function by summing the hypergeometric
 * series. This will only converge when |x| < 1. The caller might use
 * any known transformation of argument identities to reduce computation
 * of the analytically continued function to the case |x| < 1.
 *
 * It is up to the caller to do sanity checking on the argument and
 * parameters. If fed bad values, this routine may churn away forever
 * or cause an exception.
 */

real_number hypersum(real_number a, real_number b, real_number c, real_number x, real_number t_error)
{

	real_number rval = 1.0;
	real_number n = 1.0;
	real_number term = a*b*x/c;
	real_number K,C;

	/* find max(|a|,|b|,|c|) for use below */
	K = fabs(a) ? fabs(b) : fabs(a) > fabs(b);
	K = K ? fabs(c) : K > fabs(c);

	do {
		/* Decide whether we are accurate enough yet, i.e., if
		 * tail of hypergeometric series < t_error. Let An be nth term
		 * of series, and Bn = (a+n)(b+n)/[(c+n)(1+n)]. Then
		 * An+1/An = Bnx. Bn --> 1, which gives radius of convergence
		 * 1. Let Cn be a monotone nonincreasing upper bound for |Bn|
		 * Then by comparison with geometric series, |An|/(1-Cnx) < 
		 * t_error is sufficient to ensure accuracy. If we let K be
		 * the max of |a|,|b|,|c|, then it is easy to check that
		 * Cn = 1 + 6K/n + 2(K/n)^2 is such a monotone upper bound when
		 * n > 2K.  
		 */

		C = 1.0 + 6.0*K/n + 2.0*(K/n)*(K/n);
		if((n>2.0*K) && (C*x < 1) && (fabs(term)/(1 - C*x) < t_error))break;

		rval += term;
		a += 1.0;
		b += 1.0;
		c += 1.0;
		n += 1.0;
		term = term*a*b*x/(n*c);

	} while(1);   
	return rval;
}
