/* radial_kepler.c: used to handle freely falling body "orbits" */

/* Most books that discuss the two body problem omit discussion of
the zero angular momentum case. The orbit in this case is on a line 
that passes through the attractor. We outline the theory here
since it is hard to find elsewhere.  Imagine a body moving along a radius
through the center of attraction. At time t0 we assume the radial 
distance to be r_0. The body may be moving either away from the attactor
or towards it. What happens subsequently depends on the radial velocity
"at infinity" that would be compatible with the initial radius and
velocity. (Strictly speaking, this only makes sense in the hyperbolic and
parabolic case since in the elliptic case there is a maximum radius)  

There are 3 cases depending on the (constant) value of
the total energy h = (1/2)m(dr/dt)^2 - mk^2/r. The simplest case to
analyze is the "parabolic" case h = 0; Here the velocity at infinity is
exactly 0. We have

dr/dt = sqrt(2)k r^(-1/2) or sqrt(2)kdt = sqrt(r)dr.  Integrating this,
we obtain

n(t - t0) = r^(3/2)-r0^(3/2) where n = (+/-)3sqrt(2)k/2. (The sign is +/- 
according as the body is moving away from/towards the attractor at time 0.)

Of course, here we can solve for r in closed form: r = (r0^(3/2) + 
	                                                 n(t-t0))^(2/3). 
 
In the other cases we put w = h/k^2. Then

n(t - t0) = f(r) - f(r0) where

n = (+/-)sqrt(2)h^(3/2)/k^2 and

f(r) = arcsin(sqrt(-wr)) - sqrt(-wr(1+wr)) (h < 0) and
f(r) = sqrt((wr)^2 + wr) - ln(sqrt(wr) + sqrt(1+wr)) (h > 0)

(Thanks to wikipedia for these formulas). They can be derived using
integral formulas 171 and 214 of the CRC 65th Edition, after making
a subsitituion u = sqrt(r). 

Unfortunately, there does not seem to be a good approach to inverting these
functions over the full range of parameter values in this formulation. 
In particular, it ought to be clear how the case of small h (near parabolic)
approaches the parabolic case. In the hyperbolic case (h > 0) let us 
reformulate so that at least the mean motion looks similar to the parabolic
case:

sqrt(2)k (t1 - t0) = +/- int_r0^r1 (a^2 + 1/r)^(-1/2) dr, a^2 = h/k^2. 

Since k^2 is a big number, the a parameter would typically be very small
here, unless the velocity at infinity is very large. One could, for example,
expand in powers of a, with the leading term being r^(1/2) as in the 
parabolic case. 

Our approach then is to numerically integrate the right side for successive
r until the value M on the left side is reached. The 2 integration 
routines, one for hyperbolic and one for elliptic cases, use simpson's
rule (slightly adapted for a singular integrand, at the bottom of the
file.) 

*/ 
 

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<float.h>
#include<math.h>

/* numerical integrator for hyperbolic case */
static double Ih(double r0, double r_1, double a);
/* numerical integrator for elliptic case */
static double Ie(double r0, double r_1, double a);

double
radial_kepler(double r0, double M, double e, double my_derror)
{
	double  r=0.0,delta,M0,a,m,MSAVE,R;

        if( e == 0.0){
		r = exp((3.0/2.0)*log(r0));
		if(r+M > 0.0)return exp((2.0/3.0)*log(r+M));
     /* when M+r = 0 the body has hit the attractor (RIP) */
		else return 0.0;	
	}

	a = sqrt(fabs(e));
	if( e > 0) {  /* hyperbolic case */

		/* We must solve n(t1-t0) = M = Ih(r_0,r,a) for r */ 

		r = r0;

		if(M > 0.0){

/* We are moving away from the sun and will do so faster than the parabolic
  solution. Find a starting value (lower bound) for r by using the parabolic
  solution */
			delta = r0*my_derror;
			r = radial_kepler(r0,3.0*M/2.0,0.0,my_derror);
/* The 3/2 is because the parabolic case uses a different definition of 
mean motion, which we must compensate for here */
			M0 = Ih(r0,r,a);

/* Also find an upper bound R on the radius by using that the
integrand in I is bounded below by 1/a. Thus (R - r0)/a <= M so R = r0 + a*M 
*/

			R = r0 + a*M;  /* solution lies in [r,R] and we can
			home in with a binary search */ 
			r0 = r;
			m = 0.5;
			delta = R-r;

/* binary search for right value of r */

			while(m > my_derror){
				r += m*delta;
				MSAVE = M0;
				M0 += Ih(r0,r,a);
				if(M0>M){
					M0 = MSAVE;
					r = r0;
				}
				else r0 = r;
				m *= 0.5;
			}  

		} 
		if(M < 0.0){

/* We are moving towards the sun and will do so faster than the parabolic
  solution. Find a ballpark starting value for r by using the parabolic
  solution */

			delta = r0*my_derror;
			r = radial_kepler(r0,3.0*M/2.0,0.0,my_derror);
			if(r < delta)return 0.0;
			M0 = -Ih(r,r0,a);  
			r0 = r;
			m = 0.5;
			delta = r;

/* binary search for right value of r */

			while(m > my_derror){
				r -= m*delta;
				MSAVE = M0;
				M0 -= Ih(r,r0,a);
				if(M0<M){
					M0 = MSAVE;
					r = r0;
				}
				else r0 = r;
				m *= 0.5;
			}  
		} 
	}

	if(e < 0.0) {  /* elliptic case */

		/* We must solve n(t1-t0) = M = Ie(r_0,r,a) for r */ 

		if(r0 > -1.0/e) return 0.0 ;  

/* This must have been
     called in error since there is not enough energy in the orbit to reach
     such a large value. We return a defensive value, but this should
     be headed off at a higher level. */

		r = r0;

		if(M > 0.0){

/* We are moving away from the sun and will do so slower than the parabolic
  solution. Find the value of M (M0) needed to reach the turnaround point */

			R = -1.0/e; /* turnaround point */
			delta = r0*my_derror;
			M0 = Ie(r0,R,a);

			if(M<M0){ /* still heading away from the sun */

/* Find the correct r in [r0,R] using a binary search */

			M0 = 0.0; /* re-using M0 */
			r = r0;
			m = 0.5;
			delta = R-r;

			while(m > my_derror){
				r += m*delta;
				MSAVE = M0;
				M0 += Ih(r0,r,a);
				if(M0>M){
					M0 = MSAVE;
					r = r0;
				}
				else r0 = r;
				m *= 0.5;
			}  
			} /* M < M0 */
			else {  /* past turnaround and falling. Proceed
				as in the case M < 0, but with the residual
                                amount of M = M - M0 */
				M =  M0 - M;
				M0 = 0.0; /* re-using M0 */

				r0 = R;
				m = 0.5;
				r = r0;
				delta = r;

/* binary search for right value of r */

			while(m > my_derror){
				r -= m*delta;
				MSAVE = M0;
				M0 -= Ih(r,r0,a);
				if(M0<M){
					M0 = MSAVE;
					r = r0;
				}
				else r0 = r;
				m *= 0.5;
			}  
		} /* else clause: past turnaround and falling */

		} /* M > 0.0 */ 
		if(M < 0.0){

/* We are moving towards the sun and will do so slower than the parabolic
  solution.  */

			delta = r0*my_derror;
			r = r0;
			if(r < delta)return 0.0;
			M0 = 0.0;
			m = 0.5;
			delta = r;

/* binary search for right value of r */

			while(m > my_derror){
				r -= m*delta;
				MSAVE = M0;
				M0 -= Ih(r,r0,a);
				if(M0<M){
					M0 = MSAVE;
					r = r0;
				}
				else r0 = r;
				m *= 0.5;
			}  

		} /* M < 0.0 */
	} /* e < 0.0 */
	return r;
}


/* Calculate the integral from r_0 to r_1 of 1/sqrt(a^2 + 1/r)^(1/2) using
Simpson's rule. Must have r_1 > r_0 */ 

#define NN 1000  /* Number of subdivisions. Increase if you don't like
                 the accuracy, but program will run slower. Should be even  */
#define ND 1000.0 /* Same as N, but to be used as a double */

double Ih(double r_0, double r_1, double a){

	double d,rval,r;
	int j=0;
	
	d = (r_1 - r_0)/ND; /* subdivision size */
	rval = d/sqrt(a*a + 1.0/r_0) + d/sqrt(a*a + 1.0/r_1);
	r = r_0;

	while(j<(NN-2)/2){
		r += d;
		rval += 4.0*d/sqrt(a*a + 1.0/r);
		r += d;
		rval += 2.0*d/sqrt(a*a + 1.0/r);
		j++;
	}

	return rval/3.0;
}	

/* Calculate the integral from r_0 to r_1 of 1/sqrt(-a^2 + 1/r)^(1/2) using
Simpson's rule. Must have  1/a^2 >= r_1 > r_0  */ 

double Ie(double r_0, double r_1, double a){

	double d,rval,r;
	int j=0;
	
	d = (r_1 - r_0)/ND; /* subdivision size */

/* The integrand is singular at r = 1/(a^2), so for r close to this
limit we need to use an approximation. Let d = 1 - (a^2)r. Then as d -> 0
the integral is asymptotic to  (2)sqrt(d) */ 

	if(r_0 > (0.99)/(a*a)) return (2.0*sqrt(1.0 - a*a*r_0) -
				2.0*sqrt(1.0 - a*a*r_1))/(a*a);

	if(r_1 > (0.99)/(a*a)) /* last term is average value */
		rval = d/sqrt(-a*a + 1.0/r_0) + 2.0*d*sqrt(1.0 - a*a*(r_1 - d))/a; 
	else
		rval = d/sqrt(-a*a + 1.0/r_0) + d/sqrt(-a*a + 1.0/r_1);
	r = r_0;

	while(j<(NN-2)/2){
		r += d;
		rval += 4.0*d/sqrt(-a*a + 1.0/r);
		r += d;
		rval += 2.0*d/sqrt(-a*a + 1.0/r);
		j++;
	}

	return rval/3.0;
}	


