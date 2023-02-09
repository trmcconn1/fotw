/* convert.c: Implementations of routines to convert from one coordinate
   system to another or other conversions
*/
#include "ephemeris.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "constants.h"
#include <time.h>

/* Convert a Julian date to a time_t so we can use C time functions */

static int jdcalibrate = 0; /* Flag for stuff to do first time only */
static time_t time0;
static time_t timetperday = 60*60*24; /* seconds in a day. BUG: we assume time_t measures seconds
									   since the unix epoch - always true? */
static struct tm time0tm;
static double jdtime0 = J2000; /* J2000 */

time_t jd2timet(double ajd)
{
	double dtjd; /* time since J2000 in days */
	time_t dt; /* time since J2000 in time_t units */

	if(!jdcalibrate){
		/* Load up a time struct with J2000 numbers */
		time0tm.tm_year = 100;  /* years since 1900 */
		time0tm.tm_mday = 1; /* must be in range 1...31 */
		time0tm.tm_hour = 12;  /* A julian day starts at noon UTC */
		time0tm.tm_yday = 0;
		time0tm.tm_mon = 0;
		time0tm.tm_sec = 0;  /*  */
		time0tm.tm_min = 0;  /*  */
		time0tm.tm_isdst = 0;  /*  */
		time0 = mktime(&time0tm);
		jdcalibrate = 1;  /* Don't need to do this block on subsequent calls */
	}
	dtjd = ajd - jdtime0; /* days and fractions thereof since time 0 */
	dt = dtjd * timetperday;
	if(dt < 0) return time0 - (time_t)fabs(dt);
	return time0 + (time_t)dt;
}


/* Convert a spherical coordinate to cartesian */

int sphere2cart( struct spherical_coordinate *s, struct cartesian_coordinate *c)
{
	double x,y,z,phi,theta,r;
	int type;

	phi = s->phi;
	theta = s->theta;
	type = s->type;
	r = s->r;
	x = r*cos(theta)*cos(phi);
	y = r*cos(theta)*sin(phi);
	z = r*sin(theta);
	c->type = type;
	c->x = x;
	c->y = y;
	c->z = z;

	return 0;
}

int cart2sphere(struct cartesian_coordinate *c, struct spherical_coordinate *s)
{
	double x,y,z,phi,theta,r,rho,phi0;
	int type;

	type = c->type;
	x = c->x;
	y = c->y;
	z = c->z;

	rho = sqrt(x*x+y*y);
	if(rho == 0)
		if(z > 0)theta = PI/2.0;
		else theta = -PI/2.0;
	else theta = atan(z/rho);
	if( x == 0)
		if(y > 0)phi = PI/2.0;
		else phi = 3.0*PI/2.0;
	else {
		phi = atan(fabs(y/x));
		if( (y>0)&&(x<0)) phi = PI - phi;
		if( (y<0)&&(x<0)) phi += PI;
		if( (y<0)&&(x>0) ) phi = 2.0*PI - phi;
	}	
	r = sqrt(rho*rho + z*z);
	s->type = type;
	s->r = r;
	s->phi = phi;
	s->theta = theta;
	return 0;
}


/* 

convert_to: Convert a geocentric spherical coordinate from one coordinate
system to another. E.g, convert from celestial latitude/longitude to
declination/right ascension, and vice-versa. Returns pointer to a
spherical coordinate structure containing the result of the conversion, or
NULL if something goes wrong. (The return value should always be tested,
and should be free'd after use.) 

We use the conversion formulae as derived on pp. 39-40 of W. Smart,
Textbook of Spherical Astronomy, 6th Ed., Cambridge University Press.

 */

struct spherical_coordinate *convert_to(struct spherical_coordinate *old,
	int type){

	double e = EPSILON_2K, L,D,l,d,x,y,phi,theta;
	struct spherical_coordinate *my_scptr;

	switch(old->type){
		case ECLIPTIC:
			switch(type){

				case CELESTIAL:  /* Celestial latitude (D) 
							longitude (L) to 
						    Declination (d) and Right
                                                    Ascension (l) */ 
						
						L = old->phi;
						D = old->theta;

						theta = d = asin(sin(D)*cos(e) +
							  cos(D)*sin(e)*sin(L));                                                x = /* cos(d)cos(l) */
							cos(D)*cos(L);
						y = /* cos(d)sin(l) */
						  cos(D)*cos(e)*sin(L) -
						    sin(D)*sin(e);

						l = atan(y/x);
						/* convert to branch 0->pi */
					        if(l < 0)
							l += PI;		
						/* adjust for 3rd and 4th
                                                   quadrant */
						if(y < 0)
							l += PI;
						phi = l;
						break;
							
				case ECLIPTIC: /* noting to do */
						theta = old->theta;
						phi = old->phi;
						break;

				default: 	
					return NULL;
			}
			break;
		case CELESTIAL:
			switch(type){

				case ECLIPTIC:  /* Declination (d) and Right
                                                   Ascension (l) to Celestial
                                                   Latitude (D) and Longitude
                                                   (L) */

					d = old->theta;
					l = old->phi;
					

					theta = D = asin(sin(d)*cos(e) -
					  cos(d)*sin(e)*sin(l));

                                        x = /* cos(D)cos(L) */
					  cos(d)*cos(l);

					y = /* cos(D)sin(L) */
						  cos(d)*cos(e)*sin(l) +
						    sin(d)*sin(e);

					L = atan(y/x);
					/* convert to branch 0->pi */
					if(L < 0)
						L += PI;		
					/* adjust for 3rd and 4th
                                                   quadrant */
					if(y < 0)
						L += PI;
					phi = L;
					break;

				case CELESTIAL: /* nothing to do */
					theta = old->theta;
					phi = old->phi;
					break;
				default:
					return NULL;
			}
			break;
		default:
			return NULL; /* not implemented */
	}
	my_scptr = (struct spherical_coordinate *)malloc(sizeof(struct
		spherical_coordinate));
	if(!my_scptr)return NULL;
	my_scptr->type = type;
	my_scptr->theta = theta;
	my_scptr->phi = phi;
	return my_scptr;
}


