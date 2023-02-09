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
#include <string.h>

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

	double E,n,theta,e,a,r,r0,NM,CLat,x,CLong,q,M,s,t0,N,v,ksquare;
	struct event *an_event;


	if(!orbit)return NULL;
	if(orbit->center == EARTH_CENTERED)
		ksquare = EARTH_MG;
	else ksquare = SOLAR_MG;

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
		q = a*(1.0-e);
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
	v = sqrt(2.0*ksquare/r);

    }
	/* calculate true anomaly in terms of eccentric anomaly */
	
	if(e < PARABOLIC_LO){ /* elliptic orbit case */

		theta = 2.0*atan( sqrt( (1 + e)/(1 - e))*tan (E/2.0));

	/* calculate the radius from variant polar equation */

		r = a*(1.0 - e*cos(E));

	/* calculate the velocity from equation of the center */

		v = sqrt(2.0*ksquare*(1/r - 1.0/(2.0*a)));
	}
	if(e > PARABOLIC_HI){ /* hyperbolic orbit case */

	/* calculate true anomaly in terms of eccentric anomaly */
        /* In the hyperbolic case the true anomaly is still the geometric
           angle from focus to body. */

		theta = 2.0*atan(sqrt( (1 + e)/(e - 1))*tanh(E/2.0));

	/* calculate the radius from the equation of center */

		r = a*(-1.0 + e*cosh(E));

		v = sqrt(ksquare*(2.0/r + 1.0/a));
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

		v = sqrt(2.0*ksquare*(e + 1.0/r));

		an_event->where->type = ECLIPTIC;
		an_event->where->phi = orbit->omega;
		an_event->where->theta = orbit->i; 
		an_event->where->r = r; 
		an_event->v = v;

		return an_event;

	} /* end of case a = 0 */

	/* The rest is "just" spherical trigonometry and does not depend
           on the type of orbit */

	/* calculate the angular distance along the orbit from the ascending
             node (NM = "Node to Moon") */

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
	an_event->v = v;

	return an_event;
}

/* Dump to stdout a listing of all orbital elements currently defined */

void dump_all_orbits (void)
{

	int i;

	printf("\n\nBuilt-in sets of orbital elements (index order):\n\n");
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

	printf("\n\nOther loaded sets of orbital elements (index order):\n\n");
	for(i=0;i<orbit_lib_size;i++){
		printf("[%d] %s\n",nbuiltins + i+1,orbit_lib[i]->name);
		if(!verbose)continue;
		if(orbit_lib[i]->reference == REFER_TO_ECLIPTIC)
			printf("\tReference plane = ecliptic\n");
		else printf("\tReference plane = celestial equator\n");
		if(orbit_lib[i]->center == EARTH_CENTERED)
			printf("\tOrigin = Earth\n");
		else printf("\tOrigin = Sun\n");
		printf("\tEpoch (JD) = %8.1f\n",orbit_lib[i]->epoch);
		if(orbit_lib[i]->e >= 1.0)
			printf("\tDistance at periapsis = %g m\n",orbit_lib[i]->a);
		else
			printf("\tSemi-major axis = %g m\n",orbit_lib[i]->a);
		printf("\tEccentricity = %g\n",orbit_lib[i]->e);
		printf("\tInclination = %g degrees\n",DEGREES(orbit_lib[i]->i));
		printf("\tLongitude of ascending node = %g degrees\n",DEGREES(orbit_lib[i]->BigOmega));
		printf("\tArgument of perigee = %g degrees\n",DEGREES(orbit_lib[i]->omega));
		printf("\tMean anomaly at the epoch = %g degrees\n",DEGREES(orbit_lib[i]->arg));

		if(orbit_lib[i]->e >= 1.0)
			printf("\tMean motion (n) = %g degrees/day\n",
				orbit_lib[i]->period*180.0/PI);
		else
		printf("\tOrbital (sidereal) period = %g days\n", orbit_lib[i]->period/(3600.0*24.0));

		printf("\n\n");
	}
	printf("\n\nLoaded sets of initial conditions (index order):\n\n");
	for(i=0;i<nics;i++){
		if(!verbose){
			printf("[%d] %s\n",i+1,ic_lib[i]->name);
		}
		else dump_ic(ic_lib[i]);
	}
}

/* List initial conditions in human readable form */

void dump_ic(struct initial_conditions *my_ic)
{

	printf("Initial conditions of %s\n",my_ic->name);
		if(my_ic->reference == REFER_TO_ECLIPTIC)
			printf("\tReference plane = ecliptic\n");
		else printf("\tReference plane = celestial equator\n");
		if(my_ic->center == EARTH_CENTERED)
			printf("\tOrigin = Earth\n");
		else printf("\tOrigin = Sun\n");
	printf("\tEpoch (JD) = %8.1f\n",my_ic->epoch);
	printf("\t x = %g m\n",my_ic->x);
	printf("\t y = %g m\n",my_ic->y);
	printf("\t z = %g m\n",my_ic->z);
	printf("\t vx = %g m/s\n",my_ic->vx);
	printf("\t vy = %g m/s\n",my_ic->vy);
	printf("\t vz = %g m/s\n",my_ic->vz);
	printf("\tMass Ratio = %f\n",my_ic->mratio);
	printf("\n\n");
}

/* List orbit parameters in human readable form */

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
	if(orbit->a!=0.0){
		if(orbit->e < 1.0)
			printf("a\tSemi-major axis\t\t%g km\t %g AU\n",
			orbit->a/1000, orbit->a/AU_METERS);
		else
			printf("q\tperiapsis dist.\t\t%g km\t %g AU\n",
			  orbit->a/1000, orbit->a/AU_METERS);
	}
	else {
		printf("a\tSemi-major axis\t\t0.0 (zero L orbit)\n");
	}
	if(orbit->a != 0.0){
		printf("e\tEccentricity\t\t %f\n",orbit->e);
		printf("i\tInclination\t\t%g deg\n",DEGREES(orbit->i));
		printf("Omega\tLongitude asc. node\t%g deg\n",DEGREES(orbit->BigOmega));
		printf("w\tArgument of perigee\t%g deg\n",DEGREES(orbit->omega));
		printf("M\tMean anomaly\t\t%g deg (at the epoch)\n",DEGREES(orbit->arg));
	}
	else {
		printf("e\tNormalized Energy\t%g\n",orbit->e); 
		printf("theta\tAngle with plus z axis\t%g deg\n",DEGREES(orbit->i));
		printf("phi\tPolar angle\t\t%g deg\n",DEGREES(orbit->omega));
		printf("r0\tInitial radius\t\t%g m (%g AU) \n",orbit->arg,orbit->arg/AU_METERS);
	}

	if(orbit->a != 0.0){
	if(orbit->e < 1.0)
		printf("P\tSidereal period\t\t%g d\t%g yr\n",
		orbit->period/(3600.0*24.0), orbit->period/(3600*24*365.25));
	else
		printf("n\tmean motion\t\t%g deg/day\n",
		n = orbit->period*180/PI);
	}
	if((orbit->a != 0.0)&&(orbit->e >= 1.0))
	printf("t0\tperiapsis passage\t%f (JD)\n",orbit->t0);
	
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

/* Create and fill in orbital elements for an orbit with initial conditions
given relative to given origin.  It is up to the caller to provide the name and epoch */

struct orbital_elements *osculating_orbit( double x, double y,
                   double z, double vx, double vy, double vz, int origin,
			double m_ratio)
 {
	 struct orbital_elements *rval;
	 double h, a, e, i, BigOmega, omega, M0, E0, Lx, Ly, Lz,
		           L,r0,Ex,Ey,Ez,E,theta,period,ksquare,v;
	 double det,dot;   

	 if(!(rval = (struct orbital_elements *)malloc(sizeof(struct 
			orbital_elements)))) return NULL;

	 r0 = sqrt(x*x + y*y + z*z);
	 v = sqrt(vx*vx+vy*vy+vz*vz);

/* handle silly special case to avoid division by zero */

		if(r0 < 0.00000001){ /* He's dead, Jim */
			rval->i = 0.0;
			rval->omega = 0.0;
			rval->arg = 0.0;
			rval->e = 0.0;
			return rval;
		}


/* For earth orbiters other than the moon, the gravity constant
   should just be EARTH_MG, but in the case of the moon, since it
   is a 2-body problem, we must increase earth MG in proportion to the
   the moon-earth mass ratio  = .01230002 */

	 if(origin == EARTH)ksquare = EARTH_MG*(1.0+m_ratio);
	 else ksquare = SOLAR_MG;


	 /* calculate the angular momentum vector per unit mass */

	 Lx = (y*vz - z*vy); 
	 Ly = (z*vx - x*vz); 
 	 Lz = (x*vy - y*vx);

	/* total angular momentum */

	 L = sqrt(Lx*Lx + Ly*Ly + Lz*Lz);

	if((v==0.0)||(L/(r0*v) < ZERO_L)){ /* Free-fall case here */

/* In this case there is no orbital plane, but rather an orbital
line. We can thus choose the orientation of the line of nodes arbitrarily.
*/

		rval->a = 0.0; /* the signal that we are free-fall */
		rval->BigOmega = 0.0; /* +x-axis, arbitrary choice */

/* In place of inclination we just use the initial latitude,
i.e., the initial spherical theta */

		rval->i = PI/2.0 - acos(z/r0);

/* In place of omega, the argument of the periapsis, we just use the
initial longitude as seen from the sun, i.e, the spherical phi */

		if(fabs(rval->i) < .00000001)
			rval->omega = 0.0; /* arbitrary in this case */ 
		else {
			rval->omega = acos(x/sqrt(x*x+y*y));
			if(y < 0.0)rval->omega = 2*PI - rval->omega;
		}

/* If motion is away from the origin we have the convention that the
sign of omega is negative */

		if(x*vx + y*vx + z*vx > 0.0) 
			rval->omega = rval->omega - 2*PI;

/* Since the e parameter is meaningless, our convention is that its value
   conveys a normalized energy value. Specifically the energy divided by
mMG = mk^2 */
		h = -ksquare/r0 + 0.5*(vx*vx + vy*vy + vz*vz);
		rval->e = h/ksquare;	

/* Since the true anomaly is identically equal to omega we recycle its value
to give the initial radius */

		rval->arg = r0;
		return rval;
	} /* End of zero L case */

	/* Now we can calculate the inclination and BigOmega */
	/* In both coordinate systems the vernal 
	   equinnox is the direction of the positive x axis */

	 i = acos(Lz/L); /* Note that i is always between 0 and
                              Pi */

	 /* The direction of the ascending node is given by k x L =
		-Ly i + Lx j. Clearly this vector is perpendicular to both
                L and k, hence points along the line of nodes. If
		the orbit is regrograde (i > 90 0r Lz < 0) then this
                vector must be reversed. This only matters later when
                we find omega */

	/* In case i = 0 kxL is the zero vector, but then BigOmega is
          arbitrary */

	if(!i)BigOmega = 0.0;  /* why not? */
	else {
/* See extended discussion below on angle between vectors */
		BigOmega = acos(-Ly/sqrt(Ly*Ly + Lx*Lx));
		if(Lx < 0.0) BigOmega = 2.0*PI - BigOmega; /* retrograde */ 
	}

		 /* Next find the energy per unit mass, h */

	h = -ksquare/r0 + 0.5*(vx*vx + vy*vy + vz*vz);

	/* eccentricity from angular momentum and energy */

	e = sqrt(1.0 + 2.0*h*L*L/(ksquare*ksquare));

	/* We need to decide whether we really treat this orbit as 
	   parabolic */

	if((e<PARABOLIC_HI)&&(e>PARABOLIC_LO)){
		e = 1.0;
		h = 0.0;
	}

	/* The energy determines the semi-major axis */

	if(h < 0.0)
		a = -ksquare/(2.0*h);
	if(h > 0.0) a = ksquare/(2.0*h);  /* in hyperbolic case, we store a as 
                                            perihelion distance at the end */
	if(h == 0.0) a = L*L/(ksquare); /* the parameter or latus rectum */

		/* calculate LRL vector/mass^2 */

	Ex = (vy*Lz - vz*Ly)- ksquare*x/r0;
	Ey = (vz*Lx - vx*Lz)- ksquare*y/r0;
	Ez = (vx*Ly - vy*Lx)- ksquare*z/r0;

	E = sqrt(Ex*Ex + Ey*Ey + Ez*Ez);

	/* The LRL vector points towards the perigee in the orbital plane  
            for elliptic orbits
              */

	/* Find angle between line of nodes and LRL vector */

	if(E == 0.0){ /* The orbit is circular. We just take perigee to
		be at the x-axis */

		omega = 2*PI - BigOmega ;

		theta =  acos(x/r0); 
		if(y < 0.0)theta = PI + theta;
	}
	else {

/* The problem of finding the angle between vectors in 3 dimensions occurs
3 separate times in this code. Let us settle the issue here once and for
all. The usual normalized dot product formula determines the angle BETWEEN
two vectors, but there are two possible such angles, one the negative
of the other. The two vectors determine a plane, and if this plane is given
an orientation, then it is meaningful to speak of the angle from one vector
to the other. 

In our case there are two such planes involved - the reference plane and
the orbital plane. The reference plane receives its preferred orientation
from the rotation of the earth (celestial case) and the revolution of the
earth (ecliptic case.) We naturally give the orbital plane the same sense 
as the orbiting body revolves - prograde, or retrograde. 

The magnitude of the angle between vectors a and b is arcos(a.b/|a||b|);
The sign of the angle from a to b is positive if the cross product axb has
a positive component in the direction of the normal, and negative
otherwise. Equivalently, if the a,b,N form a right-handed frame.

The determining component will be non-zero, unless one of the vectors
a or b is zero.  */
 

		/* For omega, we use the dot product of (-Ly i + Lx j), which
                  points towards the ascending node, and E, which points
                 towards perihelion. 
                  */

		omega = acos((-Ex*(Ly)+Ey*Lx)/((sqrt(Lx*Lx + Ly*Ly)*E)));
		det = Lx*Ez*Lx + Ly*Ez*Ly - Lz*(Ex*Lx+Ey*Ly);
		if(det < 0.0)omega = 2*PI - omega;


	/* theta, the true anomaly, is the angle from the E vector to
           the radius vector at time 0 
            */
	
		theta = acos((x*Ex + y*Ey + z*Ez)/(E*r0));
		det = Lx*(Ey*z-Ez*y) + Ly*(Ez*x-Ex*z) + Lz*(Ex*y-Ey*x);
		if(det < 0.0)theta = 2*PI - theta;
		
	}

	if(e < 1.0){

	/* Find initial eccentric anomaly 
           */

		E0 = atan(tan(theta/2.0)*sqrt((1.0-e)/(1.0+e)))*2.0;

	/* Find initial mean anomaly from Kepler's Equation. */

		M0 = E0 - e*sin(E0);
		while(M0 >= 2.0*PI)M0+=2.0*PI;
		while(M0 < 0.0)M0+=2.0*PI;

/* We get the period from Kepler's 3rd law */

		period = sqrt(4.0*PI*PI*a*a*a/ksquare);

	}

	/* For hyperbolae the picture is similar but different */

	if(e > 1.0){

		E0 = atanh(tan(theta/2.0)*sqrt((e-1.0)/(1.0+e)))*2.0;
		M0 = e*sinh(E0) - E0;
		/* period is actually "mean motion */
		period = sqrt(ksquare/(a*a*a))*3600.0*24.0;
	}
	if(e == 1.0){ /* Parabolic case */

		M0 = tan(theta/2.0)*tan(theta/2.0)*tan(theta/2.0)/3.0 +
			tan(theta/2.0);
		/* again, the period is actually mean motion */
		period = sqrt(ksquare*4.0/(a*a*a))*3600.0*24.0;
	}

	 /* fill stuff in */

	 rval->BigOmega = BigOmega;
	if(e < 1.0)
		rval->a = a;
	if(e > 1.0)
		rval->a = a*(e-1.0); /* perihelion distance */
	if(e == 1.0)
		rval->a = a/2.0;  /* perihelion distance */
	 rval->i = i;
	 rval->omega = omega;
	 rval->arg = M0;
	 rval->e = e;
	 rval->period = period;
	 return rval;
 }


/* Convert initial conditions orbit specification to elements */

struct orbital_elements *initialconditions2elements(struct initial_conditions 
	*my_ic)
{
	struct orbital_elements *orbit;
	int my_center;
	double M0,epoch,n,ksquare,h,r0,v0;

	if(my_ic->center == EARTH_CENTERED){
		my_center = EARTH;
		ksquare = EARTH_MG;
	}
	else {
		my_center = SUN;
		ksquare = SOLAR_MG;
	}
	if(!(orbit = osculating_orbit(my_ic->x,my_ic->y,my_ic->z,my_ic->vx,
		my_ic->vy, my_ic->vz,my_center,my_ic->mratio))){
		printf("Unable to create osculating orbit\n");
		return NULL;
	}
	orbit->name = strdup(my_ic->name);
	orbit->epoch = my_ic->epoch;
	orbit->center = my_ic->center;
	orbit->reference = my_ic->reference;
	if(orbit->a == 0.0){
		r0 = sqrt((my_ic->x)*(my_ic->x)+(my_ic->y)*(my_ic->y)
			+(my_ic->z)*(my_ic->z));
		v0 = sqrt((my_ic->vx)*(my_ic->vx)+(my_ic->vy)*(my_ic->vy)
			+(my_ic->vz)*(my_ic->vz));
		h = -ksquare/r0 + 0.5*v0*v0; 
		orbit->e = h/ksquare;
		orbit->arg = r0;
		return orbit;
	}
	if(orbit->e >= 1.0){
		M0 = orbit->arg;
		n = orbit->period;
		epoch = my_ic->epoch;
		orbit->t0 = epoch - (M0/n); 
	}
	return orbit;
}

/* convert orbit elements to equivalent initial conditons */

struct initial_conditions *elements2initialconditions(struct orbital_elements
	*orbit)
{

	struct initial_conditions *rval;
	struct cartesian_coordinate c1,c2;
	struct spherical_coordinate s1,s2;
	struct event *eventa,*eventb;
	double t,delta = .00001,v,v1,vx,vy,vz,r0;

	rval = (struct initial_conditions *)malloc(sizeof(struct 
			initial_conditions));
	if(!rval)return NULL;
	t = orbit->epoch;
	eventa = ephemeris_kepler(t,orbit);
	eventb = ephemeris_kepler(t+delta,orbit);
	if(!eventa)return NULL;
	if(!eventb)return NULL;
	v = (eventa->v+eventb->v)/2.0;
	s1.type = orbit->reference;
	s1.theta = eventa->where->theta;
	s1.phi = (eventa->where)->phi;
	s1.r = (eventa->where)->r;
	s2.type = orbit->reference;
	s2.theta = (eventb->where)->theta;
	s2.phi = (eventb->where)->phi;
	s2.r = (eventb->where)->r;
	rval->name = strdup(orbit->name);
	rval->center = orbit->center;
	rval->reference = orbit->reference;
	sphere2cart(&s1,&c1);		
	sphere2cart(&s2,&c2);		
	rval->x = c1.x;
	rval->y = c1.y;
	rval->z = c1.z;
	if(orbit->a == 0.0){ /*zero angular momentum orbit. We need to make sure
		the velocity is exactly parallel to radius */
		r0 = sqrt(c1.x*c1.x + c1.y*c1.y + c1.z*c1.z);
		if(r0==0.0){ /* He's dead, Jim */
			rval->vx = 0.0;
			rval->vy = 0.0;
			rval->vz = 0.0;
		}
		else {
		/* recall that positive arg parameter for this type of
                   orbit means negative radial velocity */
			if(orbit->arg > 0.0) v = -v;
			rval->vx = v*c1.x/r0;
			rval->vy = v*c1.y/r0;
			rval->vz = v*c1.z/r0;
		}
	}
        else {                       
		vx = (c2.x - c1.x)/(delta*SECS_PER_DAY);
		vy = (c2.y - c1.y)/(delta*SECS_PER_DAY);
		vz = (c2.z - c1.z)/(delta*SECS_PER_DAY);
		v1 = sqrt(vx*vx + vy*vy + vz*vz);
		rval->vx = vx*v/v1;
		rval->vy = vy*v/v1;
		rval->vz = vz*v/v1;  /* we should have exact velocity magnitude,
                                 approximate direction */
	}
	rval->epoch = orbit->epoch;
	rval->mratio = 0.0; /* BUG */
	free(eventa);
	free(eventb);
	return rval;
}
	
