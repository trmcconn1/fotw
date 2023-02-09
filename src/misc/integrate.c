/* Integrate.c: numerically integrate the given function to the given
 * accuracy, over a given interval.
 *
 *
 * For the moment, we're just going to implement Simpson's rule. Maybe if
 * ambition allows, we can shoot for more sophisticated methods later. 
 *
 * User must provide pointer to integrand, and bound, an apriori upper bound
 * on the 4th derivative of the integrand.
 *
 * */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define EVEN 0
#define ODD 1

double simpson( double (*f)(double,double), double bound, double a, double b,
		double derror)
{

	double sign = 1.0,t,sum=0.0,x,y,delta,multiplier;
	int parity = EVEN;
	double n;

	/* Normalize interval of integration so that a < b */

	if(b<a){
		sign = -1.0;
		t = a;  /* swap a and b */
		a = b;
		b = t;
	}

	if(a==b)return 0.0; /* Trivial case */


	/* Decide how many subdivisions (2n) are required to meet the accuracy
	 * demand. We use the Simpson error estimate given in calculus texts
	 *
	 *        error < bound*(b-a)^5/(180*16*n^4)
	 *
	 */

	/* To take into account calculating f to +/- A, how small must A
	 * be? Suffices to have A(b-a) < derror/2, if we use derror/2 for
	 * error in simpson error estimate above. Thus A = derror/(2*(b-a))
	 * will do. We pass this to f when called below 
	 * */

	t = bound*(b-a)*(b-a)*(b-a)*(b-a)*(b-a);
	t = t*2.0/(derror*180.0*16.0);
	t = sqrt(sqrt(t));
	n = ceil(t);
	delta = (b-a)/(2.0*n);

	x = a;
	while(x<=b){

		/* The rule is to form y_0 + 4y_1 + 2y_2 + 4y_3 
		 *  + alternating 2 and 4 multiples ... + y_2n. Then
		 *  multiply by 1/3 of the mesh size (delta) */

		if((x==a)||(x == b))multiplier = 1.0;
		else if(parity==EVEN)multiplier = 2.0;
	       		else multiplier = 4.0;	
		y = f(x,derror/(2.0*(b-a)));
		sum += multiplier*y;
		x += delta;
		if(parity == EVEN)parity = ODD;
		else parity = EVEN;
	}
	return sign*sum*delta/3.0;
}
