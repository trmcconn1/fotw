/* Dialogs with user implemented here */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ephemeris.h"
#include "constants.h"

#define BACK 0
#define HELP 1
#define QUIT 2
#define STRING 3
/* Should be greater than number of steps in any dialog: */
#define DONE 20 

static char buffer[81];
static char do_step(char,struct orbital_elements *);
static char do_ostep(char,struct orbital_elements *);
static void bitch(void);
static char get_user(void);

struct orbital_elements *elements_dialog(void){

	struct orbital_elements *orbit;
	char n=1;

	orbit = (struct orbital_elements *)malloc(sizeof(struct orbital_elements));
	if(!orbit){
		fprintf(stderr,"dialog: Cannot allocate elements structure.\n");
		return NULL;
	}

printf("This dialog allows you to enter the elements of an orbit interactively\n");
printf("The following letters, entered alone on a line, have a special meaning:\n");
printf("\tB: Go back to the previous step\n");
printf("\tH: Print help screen for this item\n");
printf("\tQ or ^D: Quit the dialog\n\n");

while((n=do_step(n,orbit))!=127)if(n==DONE)break;
if(n==127){
	free(orbit);
	return NULL;
}
return orbit;
}

/* do a step of the dialog and return the next step number */

char do_step(char n,struct orbital_elements* orbit){

	int j;

	switch(n){

	case 1:
		printf("Name for this orbit (at most 80 characters):\n");
		if (!fgets(buffer,80,stdin))return 127;
		j = strlen(buffer);
		buffer[j-1]='\0';
		orbit->name = (char *)malloc(sizeof(char)*strlen(buffer)+1);
		if(!orbit->name){
			fprintf(stderr,"Cannot allocate space for name.\n");
			return 127;
		}
		strcpy(orbit->name,buffer); 
		return 2;

	case 2:  
		printf("Orbit Center[EARTH,SUN,B,H,Q]: \n");
		switch(get_user()){
			case HELP:
printf("This program can generate an ephemeris for earth or sun-centered\n");
printf("orbits. If the elements mention longitude then you want a sun \n");
printf("centered orbit. In all other cases you probably want an earth\n");
printf("centered orbit since an orbit as seen from the sun would \n");  
printf("be of interest only to (very hot!) beings living on the sun.\n\n");
				return n;
			case BACK:
				return n-1;
			case QUIT:
				return 127;
			default:
				if(strcmp(buffer,"EARTH")==0){
					orbit->center = EARTH_CENTERED;
					return n+1;
				}
				if(strcmp(buffer,"SUN")==0){
					orbit->center = SUN_CENTERED;
					return n+1;
				}
				bitch();
				return n;
			}
	case 3:
		printf("Reference Plane[ECLIPTIC,EQUATOR,B,H,Q]: \n");
		switch(get_user()){
			case HELP:
printf("The orbit inclination and other things are measured relative\n");
printf("to the reference plane. The ecliptic is the plane containing\n");
printf("the orbits of the planets, and the (celestial) equator is \n");
printf("the plane of the earth's equator, so the latter only makes\n");
printf("sense as reference plane in a coordinate system with the\n");
printf("the center of the earth at rest. Thus, if you picked sun\n");
printf("centered you MUST pick ECLIPTIC here. Both choices are \n");
printf("possible in the earth centered case. Generally, EQUATOR\n");
printf("is more convenient for earth orbiters, and ECLIPTIC is \n");
printf("often used in planetary work.\n\n"); 
				return n;
			case BACK:
				return n-1;
			case QUIT:
				return 127;
			default:
				if(strcmp(buffer,"ECLIPTIC")==0){
					orbit->reference = REFER_TO_ECLIPTIC;
					return n+1;
				}
				if(strcmp(buffer,"EQUATOR")==0){
					orbit->reference = REFER_TO_EQUATOR;
					return n+1;
				}
				bitch();
				return n;
			}
		case 4:
			printf("Big Omega[degrees,B,H,Q]: \n");
			switch(get_user()){
				case HELP:
printf("Called longitude of the ascending node in ecliptic coordinates\n");
printf("and right ascension of the ascending node in equatorial\n");
printf("coordinates. Right ascension is commonly measured in hours,\n");
printf("minutes, and seconds (of time) so you may need to convert to \n");
printf("degrees = 360*(hrs + min/60 + sec/3600)/24. The reference\n");
printf("plane and the orbital plane intersect in a line called the \n");
printf("line of nodes. The orbit meets this line at two points, and \n");
printf("the one where the orbiter crosses from below the reference\n");
printf("is called the ascending node. Big Omega is the angle in the \n");
printf("reference plane at the center from the x-axis to the ray from\n");
printf("the center to the ascending node. The x-axis is the line \n");
printf("from the center to the ascending node of the sun's apparent\n");
printf("orbit, also known as the vernal equinox.\n\n"); 
				return n;
			case BACK:
				return n-1;
			case QUIT:
				return 127;
			default:
				orbit->BigOmega = RADIANS(atof(buffer));
				return n+1;
			}
	case 5:
			printf("inclination[degrees,B,H,Q]: \n");
			switch(get_user()){
				case HELP:
printf("The inclination is the dihedral angle between the reference plane\n");
printf("and the orbital plane. It is measured in degrees from 0 to 180,\n");
printf("with the convention that obtuse angles are used for retrograde\n");
printf("orbits, i.e., ones traced clockwise relative to the axis of \n");
printf("of the object orbited.\n\n");
				return n;
			case BACK:
				return n-1;
			case QUIT:
				return 127;
			default:
				orbit->i = RADIANS(atof(buffer));
				return n+1;
			}

	case 6:
			printf("a[km,B,H,Q]: \n");
			switch(get_user()){
				case HELP:
printf("For elliptical orbits, a is the semi-major axis. For parabolic and\n");
printf("hyperbolic orbits it is the distance at closest approach to the \n");
printf("body orbited. For freefall (zero angular momentum) orbits a should\n");
printf("be entered as 0\n");
				return n;
			case BACK:
				return n-1;
			case QUIT:
				return 127;
			default:
				orbit->a = atof(buffer)*1000.0;
				return n+1;
			}
	case 7:
			printf("eccentricity e [number,B,H,Q]: \n");
			switch(get_user()){
				case HELP:
printf("e is the eccentricity except in the case of a free-fall orbit \n");
printf("where it is taken to be specific energy of the orbit divided by\n");
printf("the gravitational constant MG, M = mass of earth (resp. sun) and\n");
printf("G = universal gravitational constant. It is zero for a parabolic\n");
printf("free-fall and is equal to 0.5v^2, where v is the velocity at\n");
printf("infinity, for a hyperbolic free-fall. In the case of an elliptic \n");
printf("its reciprocal is the maximum radius attainable at the given\n");
printf("energy. Free-fall orbits are more conveniently specified as\n");
printf("osculating orbits with given initial position and velocity\n"); 
printf("This parmeter is negative for elliptic free-falls\n\n");
				return n;
			case BACK:
				return n-1;
			case QUIT:
				return 127;
			default:
				orbit->e = atof(buffer);
				return n+1;
			}
	case 8:
			printf("little omega [degrees,B,H,Q]: \n");
			switch(get_user()){
				case HELP:
printf("Little omega is called the argument of the periapsis (= perigee \n");
printf("or perihelion). It is also called the argument of the perifocus.\n");
printf("It is the angle measured clockwise from the x-axis to the ray \n");
printf("from the center through the primary focus of the orbit. For a \n");
printf("free-fall orbit it is just the constant polar coordinate of the\n");
printf("body, but we adopt the convention that a negative value connotes\n");
printf("that the body is moving away from the attractor initially.\n\n");
				return n;
			case BACK:
				return n-1;
			case QUIT:
				return 127;
			default:
				orbit->omega = RADIANS(atof(buffer));
				return n+1;
			}


	case 9:
			printf("epoch [day number,B,H,Q]: \n");
			switch(get_user()){
				case HELP:
printf("The epoch is the julian day number of the instant when the \n");
printf("elements apply. Elements go out of date due to perturbations of \n");
printf("bodys other than the attractor (e.g., jupiter in the case of sun) \n");
printf("More importantly, the mean anomaly element is tied to a particular\n");
printf("instant. Julian day is a standardized count of days and fractions\n");
printf("thereof. The julian day starts a noon Greenwich mean time (UTC)\n");
printf("For example, 1/2/2000 0:00 UTC has julian day number 2451545.5\n\n");
				return n;
			case BACK:
				return n-1;
			case QUIT:
				return 127;
			default:
				orbit->epoch = atof(buffer);
				return n+1;
			}

	case 10:
			printf("Mean Anomaly [degrees,B,H,Q]: \n");
			switch(get_user()){
				case HELP:
printf("The mean anomaly is the polar angle coordinate of a fictitious \n");
printf("body that increases at constant rate so as to complete one \n");
printf("revolution in the same amount of time as the actual body. It is\n");
printf("measured at the orbit epoch instant. The mean anomaly can be \n");
printf("computed from the actual body position at the epoch using a \n");
printf("suitable version of Kepler's equation, but fortunately most\n");
printf("organizations that report element sets give the mean anomaly\n");
printf("itself. For free fall orbits, we interpret this element instead\n");
printf("as the initial radius in meters.\n\n");
				return n;
			case BACK:
				return n-1;
			case QUIT:
				return 127;
			default:
				if(orbit->a)
					orbit->arg = RADIANS(atof(buffer));
				else
					orbit->arg = atof(buffer);
				return n+1;
			}

	case 11:
			printf("Period [days,B,H,Q]: \n");
			switch(get_user()){
				case HELP:
printf("For elliptic orbits this is the sidereal period in days, but\n");
printf("for parabolic and hyperbolic orbits it is taken to be the mean\n");
printf("motion, i.e., the number of degrees per day the object moves\n");
printf("on average, never mind that this average is actually zero. We\n");
printf("don't currently use this for anything but documentation, so \n");
printf("you could just make something up :-) \n");
				return n;
			case BACK:
				return n-1;
			case QUIT:
				return 127;
			default:
				orbit->period = atof(buffer)*3600.0*24.0;
				return n+1;
			}

	case 12:
			printf("JD of perihelion passage [number,B,H,Q]: \n");
			switch(get_user()){
				case HELP:
printf("This parameter is only used for parabolic and hyperbolic orbits, \n");
printf("where it gives the julian day when the body passed (will pass) \n");
printf("closest to the attractor. In all other cases, you could just\n");
printf("make something up.\n\n");
				return n;
			case BACK:
				return n-1;
			case QUIT:
				return 127;
			default:
				orbit->t0 = atof(buffer);
				return DONE;
			}
	default:
		return 127;
	}
}

/* self-documenting */

void bitch(void){
	printf("Unrecognized input. Please try again.\n\n");
}

/* Get input from the user and remove the newline fgets unhelpfully
puts there. Return something appropriate to caller */

char get_user(void){

	int n;
	if (!fgets(buffer,80,stdin))return QUIT;
	n = strlen(buffer);
	if(n==0)return QUIT;
	buffer[n-1]='\0';
	if(strcmp(buffer,"B")==0)return BACK;
	if(strcmp(buffer,"H")==0)return HELP;
	if(strcmp(buffer,"Q")==0)return QUIT;
	return STRING;
}

struct orbital_elements *osculating_dialog(void){

	struct orbital_elements *orbit;
	char n=1;

	orbit = (struct orbital_elements *)malloc(sizeof(struct orbital_elements));
	if(!orbit){
		fprintf(stderr,"dialog: Cannot allocate elements structure.\n");
		return NULL;
	}

printf("This dialog allows you to enter initial conditions interactively\n");
printf("From these data the program constructs a set of orbit elements\n");
printf("The following letters, entered alone on a line, have a special meaning:\n");
printf("\tB: Go back to the previous step\n");
printf("\tH: Print help screen for this item\n");
printf("\tQ or ^D: Quit the dialog\n\n");

while((n=do_ostep(n,orbit))!=127)if(n==DONE)break;
if(n==127){
	free(orbit->name);
	free(orbit);
	return NULL;
}
return orbit;
}

char do_ostep(char n,struct orbital_elements* orbit){

	int j;
	double m,x0,y0,z0,vx0,vy0,vz0;
	struct orbital_elements *an_orbit;

	switch(n){

	case 1:
		printf("Name for this orbit (at most 80 characters):\n");
		if (!fgets(buffer,80,stdin))return 127;
		j = strlen(buffer);
		buffer[j-1]='\0';
		orbit->name = (char *)malloc(sizeof(char)*strlen(buffer)+1);
		if(!orbit->name){
			fprintf(stderr,"Cannot allocate space for name.\n");
			return 127;
		}
		strcpy(orbit->name,buffer); 
		return 2;

	case 2:  
		printf("Orbit Center[EARTH,SUN,B,H,Q]: \n");
		switch(get_user()){
			case HELP:
printf("This program can generate an ephemeris for earth or sun-centered\n");
printf("orbits. If the elements mention longitude then you want a sun \n");
printf("centered orbit. In all other cases you probably want an earth\n");
printf("centered orbit since an orbit as seen from the sun would \n");  
printf("be of interest only to (very hot!) beings living on the sun.\n\n");
				return n;
			case BACK:
				return n-1;
			case QUIT:
				return 127;
			default:
				if(strcmp(buffer,"EARTH")==0){
					orbit->center = EARTH_CENTERED;
					return n+1;
				}
				if(strcmp(buffer,"SUN")==0){
					orbit->center = SUN_CENTERED;
					return n+1;
				}
				bitch();
				return n;
			}
	case 3:
		printf("Reference Plane[ECLIPTIC,EQUATOR,B,H,Q]: \n");
		switch(get_user()){
			case HELP:
printf("The orbit inclination and other things are measured relative\n");
printf("to the reference plane. The ecliptic is the plane containing\n");
printf("the orbits of the planets, and the (celestial) equator is \n");
printf("the plane of the earth's equator, so the latter only makes\n");
printf("sense as reference plane in a coordinate system with the\n");
printf("the center of the earth at rest. Thus, if you picked sun\n");
printf("centered you MUST pick ECLIPTIC here. Both choices are \n");
printf("possible in the earth centered case. Generally, EQUATOR\n");
printf("is more convenient for earth orbiters, and ECLIPTIC is \n");
printf("often used in planetary work.\n\n"); 
				return n;
			case BACK:
				return n-1;
			case QUIT:
				return 127;
			default:
				if(strcmp(buffer,"ECLIPTIC")==0){
					orbit->reference = REFER_TO_ECLIPTIC;
					return n+1;
				}
				if(strcmp(buffer,"EQUATOR")==0){
					orbit->reference = REFER_TO_EQUATOR;
					return n+1;
				}
				bitch();
				return n;
			}
	case 4:
			printf("x0[m,B,H,Q]: \n");
			switch(get_user()){
				case HELP:
printf("The x coordinate of the body at the epoch, in m\n");
				return n;
			case BACK:
				return n-1;
			case QUIT:
				return 127;
			default:
				x0 = atof(buffer);
				return n+1;
			}

	case 5:
			printf("y0[m,B,H,Q]: \n");
			switch(get_user()){
				case HELP:
printf("The y coordinate of the body at the epoch, in m\n");
				return n;
			case BACK:
				return n-1;
			case QUIT:
				return 127;
			default:
				y0 = atof(buffer);
				return n+1;
			}
	case 6:
			printf("z0 [m,B,H,Q]: \n");
			switch(get_user()){
				case HELP:
printf("The z coordinate of the body at the epoch, in m\n");
				return n;
			case BACK:
				return n-1;
			case QUIT:
				return 127;
			default:
				z0 = atof(buffer);
				return n+1;
			}
	case 7:
			printf("vx0 [m/sec,B,H,Q]: \n");
			switch(get_user()){
				case HELP:
printf("The x component of body velocity at epoch in m/sec\n");
				return n;
			case BACK:
				return n-1;
			case QUIT:
				return 127;
			default:
				vx0 = atof(buffer);
				return n+1;
			}


	case 8:
			printf("vy0 [m/sec number,B,H,Q]: \n");
			switch(get_user()){
				case HELP:
printf("The y component of body velocity at the epoch in m/sec\n");
				return n;
			case BACK:
				return n-1;
			case QUIT:
				return 127;
			default:
				vy0 = atof(buffer);
				return n+1;
			}

	case 9:
			printf("vz0 [m/s,B,H,Q]: \n");
			switch(get_user()){
				case HELP:
printf("The z component of body velocity at the epoch in m/sec\n");
				return n;
			case BACK:
				return n-1;
			case QUIT:
				return 127;
			default:
				vz0 = atof(buffer);
				return n+1;
			}
	case 10:
			printf("epoch [day number,B,H,Q]: \n");
			switch(get_user()){
				case HELP:
printf("The epoch is the julian day number of the instant when the \n");
printf("elements apply. Elements go out of date due to perturbations of \n");
printf("bodys other than the attractor (e.g., jupiter in the case of sun) \n");
printf("More importantly, the mean anomaly element is tied to a particular\n");
printf("instant. Julian day is a standardized count of days and fractions\n");
printf("thereof. The julian day starts a noon Greenwich mean time (UTC)\n");
printf("For example, 1/2/2000 0:00 UTC has julian day number 2451545.5\n\n");
				return n;
			case BACK:
				return n-1;
			case QUIT:
				return 127;
			default:
				orbit->epoch = atof(buffer);
		if(orbit->center = EARTH_CENTERED)
		an_orbit = osculating_orbit(x0,y0,z0,vx0,vy0,vz0,EARTH,0.0); 
		else
		an_orbit = osculating_orbit(x0,y0,z0,vx0,vy0,vz0,SUN,0.0); 
		if(!an_orbit)return 127;
		orbit->BigOmega = an_orbit->BigOmega;
		orbit->i = an_orbit->i;
		orbit->a = an_orbit->a;
		orbit->e = an_orbit->e;
		orbit->omega = an_orbit->omega;
		orbit->arg = an_orbit->arg;
		orbit->period = an_orbit->period;
		orbit->t0 = an_orbit->period;
		free(an_orbit);
				return DONE;
			}

	default:
		return 127;
	}
}

