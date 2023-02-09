/* ephemeris.c: Naive implementation of an ephemeris program */


/* The goal is to calculate the position (normally in celestial latitude/
   	longitude) of an EARTH or SOLAR satellite at a given instant.

A good reference for the Two-body problem solutions we use here is

Forest Ray Moulton, An Introduction to Celestial Mechanics, 2nd Revised Ed.,
Dover, New York, 1970.

*/

#include "ephemeris.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "constants.h"

/* implemented in kepler.c */

extern int kepler(double *E, double M, double e, double derror,
            int method);


/* implemented in radial_kepler.c */

extern double radial_kepler(double r0, double t, double e, double derror);

/* 

ephemeris: creates, fills in and returns a pointer to an event. The event
 describes where and when a body orbiting the earth ot sun will be, based on
information passed in. This routine is a wrapper for the routine
ephemeris_kepler, which does the real work. 

*/


struct event *ephemeris( double epoch, struct orbital_elements *orbit){

	struct event *sun;  /* where sun is at the epoch, if needed */
	struct event *body; 
	struct cartesian_coordinate sunxyz; /* vector from earth to sun */
	struct cartesian_coordinate bodyxyz;/* vector from sun to body */
	struct spherical_coordinate newsc;

	body  = ephemeris_kepler(epoch,orbit);

	if(!body){

		fprintf(stderr,"ephemeris: no body event\n");
		return NULL;
	}

	/* For simplicity the ephemeris_kepler routine assumes the orbit
           is earth centered, so we need to do some conversions if
           the desired ephemeris is sun-centered */

	if(orbit->center == SUN_CENTERED) {

		/* We must find the position of earth. We assume the latest
                   solar orbit elements are stored as builtins[0] */

		sun = ephemeris_kepler(epoch,&(builtins[0]));	

		if(!sun){
			fprintf(stderr,"ephemeris: no solar event\n");
			free(body);
			return NULL;
		}

		sphere2cart(sun->where,&sunxyz);
		sphere2cart(body->where,&bodyxyz);

		/* Find cartesian vector from earth to body by simple
                   vector addition, and store in original bodyxyz */

		bodyxyz.x += sunxyz.x;
		bodyxyz.y += sunxyz.y;
		bodyxyz.z += sunxyz.z;
		
		/* Convert back to spherical coordinate, now earth-centered */
		cart2sphere(&bodyxyz,&newsc);
		(body->where)->type = ECLIPTIC;
		(body->where)->phi = newsc.phi;
		(body->where)->theta = newsc.theta;
		(body->where)->r = newsc.r;
		body->v += sun->v; 
		free(sun);
	}
	return body;
}

/* ephemeris_kepler: creates, fills in and returns a pointer to an event. The 
	event describes where and when a body orbiting the 
	sun or earth will be, based
        upon information passed in the orbital elements struct. The
        coordinates returned are based on ecliptic coordinates. 
*/

struct event *ephemeris_kepler(double epoch, struct orbital_elements *orbit){

	double E,n,theta,e,a,r,r0,NM,CLat,x,CLong,q,M,s,t0,N;
	struct event *an_event;

	/* allocate stuff to return */

	an_event = (struct event *)malloc(sizeof(struct event));
	if(!an_event) return NULL;

	an_event->where = (struct spherical_coordinate *) malloc
			(sizeof(struct spherical_coordinate));

	if(!an_event->where){
		free(an_event);
		return NULL;
	}
	
	an_event->when = epoch;
	e = orbit->e;
	a = orbit->a;

	if(a != 0.0){ /* The somewhat obscure situation of a linear
                       orbit ("free-fall") needs to be handled for
                       completeness */

	if(e >= 1.0) { 

/* in this range of eccentricities the orbit->a
actually contains the perihelion distance, since the semi-major axis
is not defined */

/* The polar equation of an hyperbola is

	 	r = a(e^2-1)/(1 + e cos(v))

   so the perihelion distance is a(e-1)

   so we need to convert q to obtain what plays the role of a:

*/
 
		q = a;
		a = q/(e - 1.0);
/* And for e > 1.0 the period field already contains the mean motion,
      in radians per day */
		n = (orbit->period)/(3600.0*24.0);

	}
	if(e < 1.0){
		n = (2*PI)/orbit->period;
		q = a*(1-e);
	}

	/* calculate eccentric anomaly by solving kepler's equation */
	/* it is a different equation in the hyperbolic case, but the
	   kepler routine detects the difference from the passed value of e */

    if((e < PARABOLIC_LO) || (e > PARABOLIC_HI))

    kepler(&E, n*(epoch - orbit->epoch)*3600.0*24.0 + orbit->arg, 
		e, .0000000001, 1); /* 1 selects Newton's method */ 


    else {  /* We are treating orbit as parabolic */
	    /* See Moulton, p. 107 for the derivation of the replacement
for kepler's equation in the parabolic case. The orbit equation with
e = 1 becomes r = qsec^2(v/s) where v is the true anomaly. From the 
areal equation r^2dv/dt = n  we obtain n dv/dt = sec^4(v/2). This is
a separable ODE which integrates to n(t-t0) = tan(v/2) + tan^3(v/2)/3. 
Moulton gives an elegant solution that involves introducting a new
parametrization in which the mean motion is replaced as follows: */


	if(orbit->center == SUN_CENTERED)
	n = sqrt(0.5*SOLAR_MG/q)/(q);  
	else
	n = 3.0*sqrt(0.5*EARTH_MG/q)/(2.0*q);  
	t0 = (orbit->t0); /* Time (days) since perihelion passage */

/* When called with e = 1 this solves E^3/3 + E = n(t-t0) */
kepler(&E, n*(epoch - t0)*3600.0*24.0, 
		1.0, .0000000001, 1); /* 1 selects Newton's method */ 

	theta = 2.0*atan(E);
	r = 2.0*q/(1.0 + cos(theta));

    }
	/* calculate true anomaly in terms of eccentric anomaly */
	
	if(e < PARABOLIC_LO){ /* elliptic orbit case */

		theta = 2.0*atan( sqrt( (1 + e)/(1 - e))*tan (E/2.0));

	/* calculate the radius from equation of center */

		r = a*(1.0 - e*cos(E));
	}
	if(e > PARABOLIC_HI){ /* hyperbolic orbit case */

	/* calculate true anomaly in terms of eccentric anomaly */
        /* In the hyperbolic case the true anomaly is still the geometric
           angle from focus to body. */

		theta = 2.0*atan(sqrt( (1 + e)/(e - 1))*tanh(E/2.0));

	/* calculate the radius from the equation of center */

		r = a*(-1.0 + e*cosh(E));
	}
	}
	else { /* The special case of a an object falling directly toward
               the center of attraction. Aiiiyeeeee! There are again 3
               distinct cases (elliptic, parabolic, hyperbolic) depending on
               the total energy ( < 0, = 0, > 0 ).  For details 
               consult radial_kepler.c.  We calculate a "mean motion" 
               parameter similarly to the non-zero angular momentum case */

		/* The e parameter is the specific energy (energy/body mass)
                   divided by k^2 = MG */

		r0 = orbit->arg;

		if((e < 0.0)&&(r0 > 1.0/fabs(e))){

			/* The orbital elements are impossible because there
                           is not enough energy to reach such a large value
                           of r0.
			*/

			fprintf(stderr,"Orbital elements are inconsistent\n");
			return NULL;
		}
		if(orbit->center == SUN_CENTERED){
			if(e == 0.0) n =  -3.0*sqrt(0.5*SOLAR_MG);  
		 	else n = -sqrt(2.0*SOLAR_MG);
		}
		else {
			if(e == 0.0) n =  -3.0*sqrt(0.5*EARTH_MG);  
		 	else n = -sqrt(2.0*EARTH_MG);
		}
		if(orbit->omega < 0.0) n = -n;
		theta = 0.0;  /* true anomaly is relative to omega */
		t0 = orbit->epoch;

		r = radial_kepler(r0, n*(epoch - t0)*3600.0*24.0, 
				e, .0000000001);  

	} /* end of case a = 0 */

	/* The rest is "just" spherical trigonometry and does not depend
           on the type of orbit */

	/* calculate the angular distance along the orbit from the ascending
             node (NM = "Node to Moon") */

	 /* NM = theta + ( orbit->omega - orbit->BigOmega ); */
	NM = theta +  orbit->omega; 

	/* Normalize to lie between 0 and 2*PI */

	while(NM >  2*PI)NM -= 2*PI;
	while(NM < 0) NM += 2*PI;

	/* Apply the spherical sine law to get the celestial latitude
         */

	x = sin(NM)*sin(orbit->i);
	CLat = asin(x);

/* In the standard picture of a generic spherical triangle, we have 
      C
   b    a
 A    c    B

In our case, a = NM, B = i, A = 90 degrees, and b = CLat, found above using
the sine law. Now we need to find c = CLong-Omega. For this, the following 
Napier "analogy" is made to order: 
				cos((A+B)/2)	
	tan(c/2)cot((a+b)/2) = --------------     	 
			        cos((A-B)/2)
*/


{ 
		double C, A, x, B, D;
		A = RADIANS(90);
		B = orbit->i;
		D = cos((A+B)/2.0)/cos((A-B)/2.0);
		C = D*tan((NM + CLat)/2.0);
		x = 2.0*atan(C);	
	
		CLong = x + orbit->BigOmega;
		/* Normalize result to lie in 0...2Pi */
		if(CLong > 2.0*PI) CLong -= 2.0*PI;
		if(CLong < 0.0)CLong += 2.0*PI;

}

	an_event->where->type = ECLIPTIC;
	an_event->where->phi = CLong;
	an_event->where->theta = CLat; 
	an_event->where->r = r; 

	return an_event;
}

/* Dump to stdout a listing of all built in orbital elements defined in
   elements.c */

void dump_builtins (void)
{

	int i;

	printf("\n\nPre-defined sets of orbital elements (index order):\n\n");
	for(i=0;i<nbuiltins;i++){
		printf("[%d] %s\n",i+1,builtins[i].name);
		if(!verbose)continue;
		if(builtins[i].reference == REFER_TO_ECLIPTIC)
			printf("\tReference plane = ecliptic\n");
		else printf("\tReference plane = celestial equator\n");
		if(builtins[i].center == EARTH_CENTERED)
			printf("\tOrigin = Earth\n");
		else printf("\tOrigin = Sun\n");
		printf("\tEpoch (JD) = %8.1f\n",builtins[i].epoch);
		if(builtins[i].e >= 1.0)
			printf("\tDistance at periapsis = %g m\n",builtins[i].a);
		else
			printf("\tSemi-major axis = %g m\n",builtins[i].a);
		printf("\tEccentricity = %g\n",builtins[i].e);
		printf("\tInclination = %g degrees\n",DEGREES(builtins[i].i));
		printf("\tLongitude of ascending node = %g degrees\n",DEGREES(builtins[i].BigOmega));
		printf("\tArgument of perigee = %g degrees\n",DEGREES(builtins[i].omega));
		printf("\tMean anomaly at the epoch = %g degrees\n",DEGREES(builtins[i].arg));

		if(builtins[i].e >= 1.0)
			printf("\tMean motion (n) = %g degrees/day\n",
				builtins[i].period*180.0/PI);
		else
		printf("\tOrbital (sidereal) period = %g days\n", builtins[i].period/(3600.0*24.0));

		printf("\n\n");
	}
}

void dump_orbit(struct orbital_elements *orbit)
{

	printf("Elements of %s\n",orbit->name);
		if(orbit->reference == REFER_TO_ECLIPTIC)
			printf("\tReference plane = ecliptic\n");
		else printf("\tReference plane = celestial equator\n");
		if(orbit->center == EARTH_CENTERED)
			printf("\tOrigin = Earth\n");
		else printf("\tOrigin = Sun\n");
	printf("\tEpoch (JD) = %8.1f\n",orbit->epoch);
	printf("\tSemi-major axis = %g m\n",orbit->a);
	printf("\tEccentricity = %f\n",orbit->e);
	printf("\tInclination = %g degrees\n",DEGREES(orbit->i));
	printf("\tLongitude of ascending node = %g degrees\n",DEGREES(orbit->BigOmega));
	printf("\tArgument of perigee = %g degrees\n",DEGREES(orbit->omega));
	printf("\tMean anomaly at the epoch = %g degrees\n",DEGREES(orbit->arg));

	printf("\tOrbital (sidereal) period = %g days\n", orbit->period/(3600.0*24.0));
	printf("\n\n");
}

void info_orbit(struct orbital_elements *orbit)
{

	double a,e,h,p,i,w,M,L,v,c,b,T,n,E,L0,wtilde,k2;

	printf("\nElements of %s\n",orbit->name);
		if(orbit->reference == REFER_TO_ECLIPTIC)
			printf("(Reference plane = ecliptic, ");
		else printf("\tReference plane = celestial equator, ");
		if(orbit->center == EARTH_CENTERED)
			printf("origin = Earth/Moon Barycenter)\n");
		else printf("origin = Sun)\n");
	printf("\n");
	printf("t\tEpoch (JD)\t\t%8.1f\n",orbit->epoch);
	if(orbit->a != 0.0)
	if(orbit->e < 1.0)
	printf("a\tSemi-major axis\t\t%g km\t %g AU\n",orbit->a/1000,
		orbit->a/AU_METERS);
	else
	printf("q\tperiapsis dist.\t\t%g km\t %g AU\n",orbit->a/1000,
		orbit->a/AU_METERS);
	if(orbit->a != 0.0)
	printf("e\tEccentricity\t\t %f\n",orbit->e);
	else
	printf("e\tNormalized Energy\t%g\n",orbit->e); 
	printf("i\tInclination\t\t%g deg\n",DEGREES(orbit->i));
	printf("Omega\tLongitude asc. node\t%g deg\n",DEGREES(orbit->BigOmega));
	printf("w\tArgument of perigee\t%g deg\n",DEGREES(orbit->omega));
	if(orbit->a != 0.0)
	printf("M\tMean anomaly\t\t%g deg (at the epoch)\n",DEGREES(orbit->arg));
	else
	printf("r0\tInitial radius\t\t%g (%g) m, (AU)\n",orbit->arg,orbit->arg/AU_METERS);

	if(orbit->a != 0.0)
	if(orbit->e < 1.0)
	printf("P\tSidereal period\t\t%g d\t%g yr\n",
		orbit->period/(3600.0*24.0),
		orbit->period/(3600*24*365.25));
	else
	printf("n\tmean motion\t\t%g deg/day\n",
		n = orbit->period*180/PI);
	printf("\n\n");
	if(verbose){

	printf("Other Orbital Parameters\n\n");

		/* prepare for some computations */
		a = orbit->a;
		e = orbit->e;
	if(a > 0.0){
		if(e<1.0){
		printf("n\tmean motion\t\t%g deg/day\n"
			,n = 360*3600*24/orbit->period);
		printf("q\tperiapsis dist.\t\t%g km\t %g AU\n",
				a*(1-e)/1000, a*(1-e)/AU_METERS);
		}
		c = a*e;
		if(e<1.0)b = sqrt(a*a - c*c);
		else b = sqrt(c*c - a*a);
		printf("b\tSemi-minor axis\t\t%g km\t %g AU\n",b/1000,
		b/AU_METERS);
		if(e<1.0)
		 	p = (1-e*e)*a;
		else p = (e*e-1)*a;
		printf("p\tParameter\t\t%g km\t %g AU (Semi-latus rectum)\n",
			p/1000,p/AU_METERS);
		M = orbit->arg*180/PI;
		if(e < 1.0){
			if(M<180.0)T = -M/n;
			else T = (360-M)/n;
		}
		else T = M/n;
		printf("T\tPeriapsis Time\t\t%g days (nearest time) \n",T);
 
		h = GAUSSIAN_CONSTANT/(SECS_PER_DAY)*(e*e-1)*SOLAR_MG/p;
		printf("h\tEnergy\t\t\t%g J/kg (ie. per kg of body)\n",h);
		L = sqrt(GAUSSIAN_CONSTANT/(SECS_PER_DAY)*SOLAR_MG*p);
		printf("|L|\tAng. Momentum\t\t%g km^2/sec (per kg)\n",
			L/(1000*1000)); 
		M = orbit->arg;
		if(e < 1.0){
			kepler(&E,M,e,.000000001,0);
			v = 2.0*atan(sqrt((1+e)/(1-e))*tan(E/2));
		}
		else{
			kepler(&E,M,e,.000000001,1);
			v = 2.0*atan(sqrt((1+e)/(e-1))*tanh(E/2));
		}
		printf("v\tTrue Anomaly\t\t%g deg\n",DEGREES(v));
		wtilde = orbit->omega+orbit->BigOmega;
		if(wtilde > 2*PI)wtilde -= 2*PI;
		printf("wtilde\tlong. of periapsis\t%g deg.\n",
			DEGREES(wtilde));
		L0 = M+orbit->omega+orbit->BigOmega;
		while(L0 > 2*PI)L0 -= 2*PI;
		printf("L\tMean long.\t\t%g deg. (at epoch)\n",
 			DEGREES(L0));	
	}
	if(a == 0.0){ /* case of a zero angular momentum orbit */
		if(orbit->center == EARTH_CENTERED)k2 = EARTH_MG;
		else k2 = SOLAR_MG;
		h = e*k2; 
		b = 1/e;
		printf("h\tSpecific Energy\t\t%g J/kg\n",h);
		if(e < 0.0)
		printf("R\tMaximum Radius\t\t%g km\t %g AU\n",-b/1000,-b/AU_METERS);
		else
		printf("V\tv at infinity \t\t%g km/sec\n",sqrt(fabs(orbit->e)*k2*2.0)/1000.0); 
		if(orbit->omega < 0.0)
		printf("v0\tv at time 0\t\t%g km/sec\n",sqrt(fabs(h+k2/orbit->arg))*sqrt(2.0)/1000.0);
		else
		printf("v0\tv at time 0\t\t%g km/sec\n",-sqrt(fabs(h+k2/orbit->arg))*sqrt(2.0)/1000.0);
	}
   }
}

/* Create and fill in orbital elements for an orbit with initial conditions given relative to given origin.
   It is up to the caller to provide the name and epoch */

 struct orbital_elements *osculating_orbit( double mass, double x, double y, double z, double vx, double vy, double vz, int origin)
 {
	 struct orbital_elements *rval;
	 double h, a, e, i, BigOmega, omegabar, M0, E0, Lx, Ly, Lz, L,r0,Ex,Ey,Ez,E,theta,period;
	 double b1;   /* scratch area */

	 if(!(rval = (struct orbital_elements *)malloc(sizeof(struct orbital_elements)))) return NULL;

	 r0 = sqrt(x*x + y*y + z*z);

	 /* calculate the angular momentum vector */
	 Lx = mass*(y*vz - z*vy); Ly = mass*( z*vx - x*vz); Lz = mass*(x*vy - y*vx);

	 L = sqrt(Lx*Lx + Ly*Ly + Lz*Lz);
	 if(L == 0){ printf("Angular Momentum vanishes\n");return NULL;}

	 if(origin == EARTH){

		/* Now we can calculate the inclination and BigOmega */
		 /* We are working in ecliptic coordinates. The vernal equinnox is
		 the direction of the positive x axis */

		 i = acos(Lz/L); /* Note that i is always between 0 and
                                  Pi */

		 /* The direction of the ascending node is given by k x L =
		-Ly i + Lx j. Clearly this vector is perpendicular to both
                L and k, hence points along the line of nodes. */

		 
		b1 = acos(-Ly/sqrt(Ly*Ly + Lx*Lx));
		BigOmega = b1;
#if 0
		 if(Ly == 0)b1 = PI/2;
		 else
		 b1 = atan(-Lx/Ly); /* The direction of ascending node is this angle or
						   pi plus this angle */

		 if(Lz > 0) /* counter-clockwise */
			 if(i < PI/2 ) BigOmega = PI + b1;
			 else BigOmega = b1;
		 else   /* clockwise */
			 if(i < PI/2) BigOmega = b1;
			 else BigOmega = PI + b1;

			 /* BUG: what about extreme values of Lz? */
#endif
		 /* Next find the energy, h */

		h = -EARTH_MG*mass/r0 + 0.5*mass*(vx*vx + vy*vy + vz*vz);

		if(h >= 0){ printf("Energy is positive\n"); return NULL;}

		/* The energy determines the semi-major axis */

		a = -mass*EARTH_MG/(2*h);

		/* and then the eccentricity, together with angular momentum */

		e = sqrt(1.0 + 2.0*h*L*L/(mass*mass*mass*EARTH_MG*EARTH_MG));

		/* calculate LRL vector */
		Ex = mass*(vy*Lz - vz*Ly)- mass*mass*EARTH_MG*x/r0;
		Ey = mass*(vz*Lx - vx*Lz)- mass*mass*EARTH_MG*y/r0;
		Ez = mass*(vx*Ly - vy*Lx)- mass*mass*EARTH_MG*z/r0;

		E = sqrt(Ex*Ex + Ey*Ey + Ez*Ez);


		/* The LRL vector points to the perigee in the orbital plane */

		/* Find angle between line of nodes and LRL vector */

		b1 = acos((Ex*(Ly)-Ey*Lx)/(L*E));

		if(b1 < BigOmega) b1 = b1 - BigOmega + 2*PI;
		omegabar = b1+BigOmega;
		if(omegabar > 2*PI)omegabar -= 2*PI;

		/* Find the true anomoly at time 0 = angle between initial direction and LRL */

		theta = acos((x*Ex + y*Ey + z*Ez)/(E*r0));
		if(Lz < 0) /* retrograde */
			theta = 2*PI - theta;

		/* Find initial eccentric anomoly and mean anomoly */
		E0 = atan(tan(theta/2.0)*sqrt((1-e)/(1+e)))*2;

		/* Find initial mean anomoly */

		M0 = E0 - E0*sin(e);

		period = sqrt(4.0*PI*PI*a*a*a/EARTH_MG);

	 }
	 else { /* SUN centered: TODO */ return NULL;}

	 /* fill stuff in */
	 rval->BigOmega = BigOmega;
	 rval->a = a;
	 rval->i = i;
	 rval->omega = omegabar;
	 rval->arg = M0;
	 rval->e = e;
	 rval->period = period;
	 return rval;
 }

