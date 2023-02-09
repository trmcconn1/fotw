/* gamma.c:
 
   Compute the gamma and digmamma functions of real parameters 
   and a real variable. 

   The gamma function is defined by 

   Gamma(x) = \int_0^{\infty} e^{-t}t^{x-1}dt

   (See about digamma below.)

   When x is a positive integer Gamma(x) = (x-1)! In other cases we reduce
   to y with 0 < y < 1 using the relation Gamma(x)=(x-1)Gamma(x-1). For
   such y we compute the truncated improper integral at upper limit N with
   N large  by breaking it into the integrals from 0 t0 1 and from 1 to N. 
   We do the first by expanding the integrand in a Maclaurin series and 
   integrating term by term. The second we do by numerical integration.

   For background on the gamma function, including 
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

#include<math.h> /* For fabs, pow */
#include<float.h>
#include<stdlib.h>
#include<stdio.h>
#include "global.h"

extern real_number simpson( real_number(*integrand)(real_number,real_number),
	real_number bound, real_number a , real_number b, real_number t_error);

/* my_f: the integrand in the gamma function. Used to pass to simpson */

static real_number my_y; /* global variable used by my_f */
static real_number my_f(real_number t, real_number t_error)
{
	return exp(-t)*pow(t,my_y-1.0);
}

/* my_gamma: Compute the gamma function gamma(x) to within
 * an error < t_error. (Name prevents conflict with existing library function.)
 *
 * We compute the gamma function as explained above. 
 *
 * It is up to the caller to do sanity checking on the argument and
 * parameters. If you feed it bad values, you may be toast.
 */

real_number my_gamma(real_number x, real_number t_error)
{

	real_number rval = 1.0, y=x; /* y congruent to x mod 1, 0 <= y < 1 */
	real_number M = 1.0,N; /* M s.th. Gamma(x) = M*Gamma(y), N is upper
				 limit of integral */
	real_number term = 0,bound,j; 

	/* Reduce x to y modulo 1, building the multiplier M along the way */

	if(x >= 1.0)
		while( y >= 2.0){
			M *= (y-1.0);
			mults++;
			y -= 1.0;
		}
	else    if( x < 0.0) {
			while( y < 0.0){
				M /= y;
				divs++;
				y += 1.0;
			}
			/* If y is zero, then x was at a pole ( 0 or negative 
			 * integer ) of the gamma function. Return 
			 * representation of infinity */
			if(y == 0.0)return DBL_MAX;
		 }


	/* Handle the case 0 < y < 1/2 using the identity 
	 * gamma(x)gamma(1-x) = pi/sin(pi*x) */

	if(y < 0.5) {
		/* Gamma < Gamma(.5) = 1.77, so relative error is at worst 
		 * doubled in the division. */
		rval =  PI/(sin(PI*y)*my_gamma(1.0-y,t_error/2.0));
		mults += 3;
		divs += 1;
		subs += 1;
		if((INEXACT(fabs(M),t_error))||(INEXACT(rval,t_error))||
				(INEXACT(fabs(M*rval),t_error)))
		fprintf(stderr,"gamma: May be loss of precision due to large factors.\n");
		return M*rval;
	}
	if(y < 1.0){
		M /= y;
		divs++;
		y += 1.0;
	}

	/* If y is 1.0 then M has the answer: (x-1)! = M */
	if(y == 1.0)return M;

	/* From now on we can assume 1 < y < 2 */

	/* Gamma(y) is defined by an improper integral. We need to determine 
	 *  the upper limit of integration so that the tail beyond that 
	 *  contributes < t_error/2. For this it suffices that 
	 * (N+1)e^{-N} <= t_error/2. If N >= 4 then (N+1)e^{-N} < e^{-.25N}.
	 * So  N >= -4*ln(t_error/2)/3.0 is sufficient for us.  
	 */

	N = -4.0*log(t_error/2.0)/3.0;
	if(N < 1.0)N = 4.0; /* in case of ridiculuous t_error */

	/* From now on, we have t_error/2 to play with */

	/* To the desired accuracy we can assume the gamma function is
	 * defined as
	 *
	 * \int_0^N e^{-t}t^{y-1}dt.
	 *
	 * For the contribution from 0 to 1 
	 * we get an alternating series when we expand the integrand and 
	 * integrate term by term. 
	 * An upper bound for the absolute value
	 * of the jth term (using y > 1) is easily found to be 1/(j+1)!,
	 * so we sum until this is < t_error/4.
	 */

	bound = 1.0/y;
	term = 1.0;
	rval = 1.0/y;      /* zeroth term of series is 1/y */
	j = 1.0;
	while (bound >= t_error/4.0){

		/* Find jth term of series  (-1)^j /((j+y)j!) */

		term = term*(-1.0)/j;

		/* And add it on */

		rval += (term/(j+y));

		divs += 2;
		adds += 1;

		/* And prepare for next round */

	        bound = bound/(j+1.0);
		j = j + 1.0;
	}

        /* We handle the integral from 1 to N using simpson's rule. */
	/* Simpson needs to be passed a bound on the 4th derivative of
	 * the integrand, e^(-t)t^(y-1), on [1,N]. Here we win big, because
	 * the integrand is a joy. 
	 *  A bound on the 4th derivative turns out to be 1.2 We'll use 2. */

	my_y = y;
        rval += simpson(my_f,2.0,1.0,N,t_error/4.0);
	adds+=1;
	mults += 1;
	if((INEXACT(fabs(M),t_error/4.0))||(INEXACT(fabs(rval),t_error/4.0))||
				(INEXACT(fabs(M*rval),t_error/4.0)))
		fprintf(stderr,"gamma: May be loss of precision due to large factors.\n");
	return M*rval;
}

/* Euler's constant, = minus the digamma at x = 1, GAMMA = -Psi(1) */
#define GAMMA .577215664901532860606512090082
static real_number my_f2(real_number t, real_number t_error)
{
	return exp(-t)*pow(t,my_y-1.0)*log(t);
}

/* The digamma function is the logarithmic derivative of the gamma function.
 * It too is analytic except for poles at 0 and the negative integers. It has
 * an additive recurrence similar to the gamma's multiplicative recurrence. 
 *
 * As for the gamma, we use the recurrence to reduce to 1 <= x < 2. 
 *
 *
 */

real_number digamma(real_number x, real_number t_error)
{
	real_number rval, y=x; /* y congruent to x mod 1, 0 <= y < 1 */
	real_number S = 0.0,N; /* S s.th. Psi(x) = S + Psi(y) */
	real_number term = 0,bound,j; 

	/* Reduce x to y [1,2), building the offset S along the way */

	if(x >= 1.0)
		while( y > 2.0){
			S += 1.0/(y-1.0);
			y -= 1.0;
			adds += 2;
		}
	else    {
	       	if (x <= 0.0){
	        	while( y < 0.0){
				S -= 1.0/y;
				y += 1.0;
				adds += 1;
				subs += 1;
				if(fabs(S) > 1000.0)
					if(INEXACT(fabs(S),t_error))
						fprintf(stderr,"digamma:warning:possible loss of precision due to large S.\n");

			}
	/* If y is zero, then x was at a pole ( 0 or negative integer ) of
	 * the gamma function. Return representation of a non number */
			if(y == 0.0)return DBL_MAX;
		}
		/* 0 < x < 1 Increase to 1 < y < 2 */
		S -= 1.0/y;
		y += 1.0;
		if(fabs(S) > 1000.0)
			if(INEXACT(fabs(S),t_error))
				fprintf(stderr,"digamma:warning:possible loss of precision due to large S.\n");

		adds += 1;
		subs += 1;
	}
	divs += 1;

	/* Better check the absolute size of S. Since it is added or 
	 * subtracted, a huge value could nuke our precision */

	if(fabs(S) > 1000.0)
		if(INEXACT(fabs(S),t_error))
		fprintf(stderr,"digamma:warning:possible loss of precision due to large S.\n");

	/* If y is 1.0 then S - gamma is the answer  */
	
	if(y == 1.0){
		subs += 1;
		return S-GAMMA;
	}

	/* From now on, we can assume 1 < y < 2. We proceed as for
	 * the gamma, but calculate gamma' instead. Then divide by gamma
	 * to get psi. */

	/* The difference is that the integrand now has an extra factor
	 * of log. */

	/* Gamma'(y) is defined by an improper integral. We need to determine
	 * the upper limit of integration so that the tail beyond that 
	 * contributes < t_error/3. For this it suffices that 
	 * (2NlogN+1)e^{-N} <= t_error/3. If N > 4 then (2NlogN+1)e^{-N}
	 * < e^{-3N/4} (easily), so N > 4log(t_error/3)/3 will do */   

	N = -4*log(t_error/3.0)/3.0;
	if(N < 1.0)N = 4.0; /* in case of ridiculuous t_error */

	/* To the desired accuracy we can assume the gamma' function is
	 * defined as
	 *
	 * \int_0^N e^{-t}t^{y-1}log(t)dt.
	 *
	 * For the contribution from 0 to 1 
	 * we get an alternating series when we expand the integrand and 
	 * integrate term by term. 
	 * An upper bound for the absolute value
	 * of the jth term (using y < 1) is easily found to be 1/(j+1)!},
	 * so we sum until this is < t_error/6.
	 */

	bound = 1.0/y;
	term = -1.0;
	rval = -1.0/(y*y);      /* zeroth term of series is -1/y^2 */
	j = 1.0;
	while (bound >= t_error/6.0){

		/* Find jth term of series  -(-1)^j /((j+y)^2j!) */

		term = term*(-1.0)/j;

		/* And add it on */

		rval += (term/((j+y)*(j+y)));

		divs += 1;
		adds += 1;

		/* And prepare for next round */

	        bound = bound/(j+1.0);
		j = j + 1.0;
	}

        /* We handle the integral from 1 to N using simpson's rule. */
	/* Simpson needs to be passed a bound on the 4th derivative of
	 * the integrand, e^(-t)t^(y-1)ln(t), on [1,N]. 
	 * One can check this will never exceed 3 */

	my_y = y;
        rval += simpson(my_f2,3.0,1.0,N,t_error/6.0);
	adds+=1;
	mults += 1;
	return S + rval/my_gamma(y,t_error/3.0);
}




/* This algorithm is obsolete. Don't use it */

#ifdef USE_DIGAMMA_DIFF
real_number digamma(real_number x, real_number t_error)
{
	real_number rval, y=x; /* y congruent to x mod 1, 0 <= y < 1 */
	real_number S = 0.0; /* S s.th. Psi(x) = S + Psi(y) */

	/* Reduce x to y [1,2), building the offset S along the way */

	if(x >= 1.0)
		while( y > 2.0){
			S += 1.0/(y-1.0);
			y -= 1.0;
			adds += 2;
		}
	else    {
	       	if (x <= 0.0){
	        	while( y < 0.0){
				S -= y;
				y += 1.0;
				adds += 1;
				subs += 1;
			}
	/* If y is zero, then x was at a pole ( 0 or negative integer ) of
	 * the gamma function. Return representation of a non number */
			if(y == 0.0)return DBL_MAX + 1.0;
		}
		/* 0 < x < 1 */
		S -= y;
		y += 1.0;
		adds += 1;
		subs += 1;
	}
	divs += 1;

	/* If y is 1.0 then S - gamma is the answer  */
	
	if(y == 1.0){
		subs += 1;
		return S-GAMMA;
	}

	/* From now on, we can assume 1 < y < 2 */
	/* The error in the difference quotient approximation to the
	 * logarithmic derivative is easily seen to be bounded by 2*h. 
	 * (Consider the second derivative.)
	 * The quotient of gammas will double the relative error (and
	 * the gammas are bounded by gamma(2) = 1.) So we call them with
	 * t_error/4 and take h = t_error/4. 
	 */

	/*WRONG! As noted above, the absolute error in the subtraction gets
	 * magnified greatly when dividing by h, so the following is not
	 * called at the right truncation level. We really would need the
	 * t_errors to be squared, which limits the accuracy greatly. */

	h = t_error/4.0;
	rval = S + (my_gamma(y+h,t_error/4.0)/my_gamma(y,t_error/4.0) - 1.0)/h;
	divs +=4; adds += 1; subs += 1;
	return rval;

}
#endif

/* The algorithm below is horrible, and obsolete. Don't use it. */

#ifdef USE_DIGAMMA_SERIES
/* The digamma function is the logarithmic derivative of the gamma function.
 * It too is analytic except for poles at 0 and the negative integers. It has
 * an additive recurrence similar to the gamma's multiplicative recurrence. 
 *
 * As for the gamma, we use the recurrence to reduce to 0 < x < 1. 
 * For 0 < x < 1 we use -gamma + sum of series with nth term 
 *            1/(n+1) - 1/(n+x)
 * n = 0,1,...
 *
 * Because this series converges very slowly, computation of Psi is 
 * sloooooooow!
 */

real_number digamma(real_number x, real_number t_error)
{

	real_number rval = -GAMMA, y=x; /* y congruent to x mod 1, 0 <= y < 1 */
	real_number S = 0.0; /* S s.th. Psi(x) = S + Psi(y) */
	real_number term = 0,j; 

	/* Right now let's just return 1 so we can test gamma. This is too
	 * slow */
	return 1.0;

	/* Reduce x to y modulo 1, building the offset S along the way */

	if(x >= 0.0)
		while( y > 1.0){
			S += 1/(y-1.0);
			y -= 1.0;
		}
	else    while( y < 0.0){
			S -= y;
			y += 1.0;
		}

	/* If y is zero, then x was at a pole ( 0 or negative integer ) of
	 * the gamma function. Return representation of a non number */

	if(y == 0.0)return DBL_MAX + 1.0;

	/* If y is 1.0 then S -gamma is the answer  */
	
	if(y == 1.0)return S-GAMMA;

	/* From now on, we can assume 0 < y < 1 */

	/* A bound on the nth term of the defining series is
	 * 2/(n+1)(n-1) for n = 2,... The tail of this series from N onward
	 * is 1/(N-1) since it telescopes. This is a very poor rate of
	 * convergence, unfortunately.
	 */

	j = 0.0;
	for(j=0.0;j< 1.0 + 1.0/t_error;j++){

		/* Find jth term of series */

		term = 1.0/(j+1.0) - 1.0/(j+y);

		/* And add it on */

		rval += term;
	}
	return S+rval;
}
#endif
