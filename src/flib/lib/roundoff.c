/* Roundoff.c: variables and routines having to do with modelling roundoff
 * error */

#include "global.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


/* Our library routines are supposed to increment these whenever they
 * perform an inexact operation. It is hard to predict when an operation
 * will be inexact, so our policy is to assume the worst. 
 *  */

unsigned long mults = 0L;
unsigned long divs = 0L;
unsigned long adds = 0L;
unsigned long subs = 0L;

double z_alpha = 0.0;  /* Only find this once */

/* units_error_roundoff: return, in units of the least significant decimal
 * place, a confidence bound on the size of roundoff error incurred in a
 * computation using the above numbers of operations of each type.
 *
 * The effect on roundoff error of multiplications and divisions is quite
 * different from that of additions and subtractions. In the former case it
 * is quite natural to consider relative roundoff error, since this affects
 * only the mantissas. Assume there are n significant digits of accuracy
 * available (e.g., DBL_DIGS in double precision calculations), and let
 * u be 5x10^(-n). Then a careful analysis (see, e.g., F.B. Hildebrand,
 * Introduction to Numerical Analysis, McGraw-Hill, 1956, pp. 8-12), shows
 * that the relative roundoff error in m muliplications or divisions,
 * measured in units of 10^(-n) is bounded by
 *
 *             (5/u)[(1+u)^(m-1)(1+u/10) -1]
 *
 * For example, when m = 2 this is 5.5 + .5u < 6 for any reasonable u. 
 * If u < 1/10 and m > 2, Taylor's theorem gives 5[m - 0.9 + 
 * 11(m-1)(m-2)/200] as an upper bound. 
 *
 * For u < 1/1000 we can replace 200 by 20000.
 *
 * In the case of repeated additions and subtractions it is more natural to
 * consider bounding the absolute size of the roundoff error. Here, the
 * exponents as well as the mantissas would need to be accounted for. We
 * will assume the individual calculation routines keep track of the order
 * of magnitude of all quantities involved in a calculation. We assume here
 * the worst case, i.e., all summands have the same order of magnitude. Then
 * the roundoff error in each calculation is +/- 0.5 (scaled to
 * appropriate order of magnitude.) To keep from being hurt too much by our
 * caution, we will suppose the +/- signs occur at random. Then the effect
 * of m additions can be well approximated as having a Normal distribution
 * with variance m/4. 
 *
 */

int units_error_roundoff(void)
{
	double a,b;
	static int first_call = 1;
	int m = m;

	m  = mults + divs;
	a = (double)m;
	if(m<=2) b = 6.0;
	else
	 if( DIGITS_PRECISION <= 3) /* Probably not true */
	 	b = 5.0*(a - 0.9 + 11.0*(a-1.0)*(a-2.0)/200.0); 
	 else
	 	b = 5.0*(a - 0.9 + 11.0*(a-1.0)*(a-2.0)/20000.0); 

	if(first_call) { /* calculate the upper alpha point of the
			    standard normal */ 
		a = 0.0;
		/* erf(x/sqrt(2))/2 = P(0<Z<x) */ 
		while(.5 - erf(a/sqrt(2.0)/2.0) > ALPHA)a+=0.1;
		z_alpha = a;
		first_call = 0;
	}

	a = (double)(adds+subs);
	return (int)ceil(b + 0.5*z_alpha*sqrt(a/4.0));

}

void reset_roundoff(void){

	mults = divs = adds = subs = 0L;

}

void report_roundoff(void){
	printf("%lu multiplications, %lu divisions, %lu additions, %lu subtractions\n",mults,divs,adds,subs);
	printf("The roundoff is bounded by %d times 10^(-%d) at alpha = %7.6f\n",units_error_roundoff(),DIGITS_PRECISION,ALPHA);
	printf("Z alpha = %8.6f\n",z_alpha);
}

void roundoff_check(real_number t_error){
	
	real_number a;
	int i=0;

	a = (real_number)units_error_roundoff();

	while(i<DIGITS_PRECISION){
		a /= 10.0;
		i++;
	}
	if(a >= t_error){

		fprintf(stderr,"warning: uncertain precision due to roundoff error.\n");	
	        fprintf(stderr,"Roundoff error bounded by %d times 10^(-%d) at alpha = %7.6f\n",units_error_roundoff(),DIGITS_PRECISION,ALPHA);
	}
}
