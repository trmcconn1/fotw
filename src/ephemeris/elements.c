/* Define built in sets of orbital elements here */

#include "ephemeris.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "constants.h"

extern int secs(char *,double *);
extern int ParseDate(char *,double *);


#define NBUILTINS 17
int nbuiltins = NBUILTINS;

/* Orbital Elements of the moon */
/* These are epoch J2000 data from the Astronomical Almanac 2000 */
/* The refernce plane is the ecliptic */

struct orbital_elements luna2k = {
		"Mean Lunar Orbit at the epoch 0h UTC 1/0/2000",   /* name field */
		EARTH_CENTERED,                    /* selector */
		REFER_TO_ECLIPTIC,                 /* selector */
		125.123953*PI/180.0,               /* longitude of node */
		5.1453964*PI/180.0,                /* polar angle of incl. */
		3.84400E08,                        /* semi-major axis in m */
		0.054900489,                       /* eccentricity */
#if 0  
/* We originally used these strange, traditional, elements, which are not
   really longitudes at all. More sensible are the arguments, measured
   in the orbit plane from the node. One adds the longitude of the node to
   those to get
   these "longitudes", which are really the sum of two angles measured in 
   different planes. It is the difference of these  = Mean Anomaly that is 
mainly used, so it doesn't matter very much. */

		83.186346*PI/180,                  /* longitude of perigee */
		197.95*PI/180,                 /* longitude at epoch */
		/* 198.551838*PI/180, */                 /* longitude at epoch */
#endif

#if 1

/* Unfortunately, the Nautical Almanac gives longitudes rather than 
   angles measured along the orbit. The argument of the perigee is
   side c in the usual diagram of a spherical triangle

          B
       a     c
    C     b    A

where A = i is at the node, C = 90 degrees, and b = 83.18346 - 125.123953
= 318.059507.  To find c, we use the "Analog Formula" 14, p. 10 Smart:
sin a cos C = cos c sin b - sin c cos c cos A.  Since cos C = 0 we
have cot c = cot b cos i, or tan c = tan b sec i. Since  i = 5.1453964    
we have tan c = tan 318.059507 * sec 5.1453964 = -0.9021606099, we have
c = -42.05553344 = 317.9444666 */

		317.9444666*PI/180.0,               /* argument of perigee */ 

/* For the argument of the body at the epoch we have a similar
   calculation: tan c = tan(198.551838 - 125.123953) * sec(5.1453964)
= 3.374002178 so c = 73.49102331. Subtract omega to get mean anomaly */

		115.5465567*PI/180.0,              /* Mean anomaly  */

#endif
		2451543.5,                         /* epoch (julian day ) */
		27.321662*60.0*60.0*24.0,       /* sidereal period in seconds */
		0.0                           /* perihelion passage JD: not
                                                  used since orbit not
                                                  parabolic */
};	

struct orbital_elements sun2k = {
		"Solar Orbit at the epoch 0h UTC 1/0/2000",
		EARTH_CENTERED,
		REFER_TO_ECLIPTIC,
		0.0,
		-0.00014*PI/180.0,
		1.00000011*AU_METERS,
		0.01670832,
		282.938275*PI/180.0,
		356.049323*PI/180.0,
		2451543.5,
		365.256363*3600.0*24.0, 
		0.0
};

/* More up to date */

struct orbital_elements sun = {
		"Solar Orbit at the epoch 0h UTC 1/1/2018",
		EARTH_CENTERED,
		REFER_TO_ECLIPTIC,
		0.0,
		-0.00014*PI/180.0,
		1.00000011*AU_METERS,
		0.0167845,
		282.7685*PI/180.0,
		357.6205*PI/180.0,
		2458119.5,
		365.256363*3600.0*24.0,
		0.0     
};

struct orbital_elements pallas = {
	        "2 Pallas Orbit at epoch 0h UTC 3/23/2018",
		SUN_CENTERED,
		REFER_TO_ECLIPTIC,
		173.08*PI/180.0, /* longitude of the node */
		34.837*PI/180.0, /* inclination */
		2.7728*AU_METERS, /* semi-major axis */
		0.2305, /* eccentricity */
		310.01*PI/180.0, /* argument of perihelion */
		334.32*PI/180.0, /* mean anomaly at epoch */
		2458200.5,      /* epoch (julian day) */
		1684.869*3600*24.0,   /* siderial period (secs) */
		0.0
};

/* A good source of elements is JPL's HORIZONS web interface. A search 
gets you there easily */

struct orbital_elements mars = {
	        "Mars orbit epoch 0h UTC 12/25/2019",
		SUN_CENTERED,
		REFER_TO_ECLIPTIC,
		49.50118*PI/180.0, /* longitude of the node */
		1.848064*PI/180.0, /* inclination */
		1.52362237079*AU_METERS, /* semi-major axis */
		0.093501397, /* eccentricity */
		286.679269*PI/180.0, /* argument of perifocus */
		243.3990292*PI/180.0, /* mean anomaly at epoch */
		2458842.5,      /* epoch (julian day) */
		686.93298*3600*24.0,   /* siderial period (secs) */
		0.0
};

struct orbital_elements venus = {
	        "Venus orbit epoch 0h UTC 2/01/2020",
		SUN_CENTERED,
		REFER_TO_ECLIPTIC,
		76.6247507*PI/180.0, /* longitude of the node */
		3.394578*PI/180.0, /* inclination */
		.7233259603791730*AU_METERS, /* semi-major axis */
		0.00674579, /* eccentricity */
		54.8715295*PI/180.0, /* argument of perifocus */
		282.91359088*PI/180.0, /* mean anomaly at epoch */
		2458880.5,      /* epoch (julian day) */
		224.697879436*3600*24.0,   /* siderial period (secs) */
		0.0
};

struct orbital_elements mercury = {
	        "Mercury orbit epoch 0h UTC 2/01/2020",
		SUN_CENTERED,
		REFER_TO_ECLIPTIC,
		48.30638873669*PI/180.0, /* longitude of the node */
		7.0037863556*PI/180.0, /* inclination */
		.3870969944881191*AU_METERS, /* semi-major axis */
		.205651134119, /* eccentricity */
		29.18333085*PI/180.0, /* argument of perifocus */
		314.113089623*PI/180.0, /* mean anomaly at epoch */
		2458880.5,      /* epoch (julian day) */
		87.96868295520237*3600*24.0,   /* siderial period (secs) */
		0.0
};

struct orbital_elements jupiter = {
	        "Jupiter orbit epoch 0h UTC 2/01/2020",
		SUN_CENTERED,
		REFER_TO_ECLIPTIC,
		100.51674444*PI/180.0, /* longitude of the node */
		1.303634821*PI/180.0, /* inclination */
		5.203266100185078*AU_METERS, /* semi-major axis */
		.0487105056, /* eccentricity */
		273.53506052*PI/180.0, /* argument of perifocus */
		269.84972064577*PI/180.0, /* mean anomaly at epoch */
		2458880.5,      /* epoch (julian day) */
		4333.165665644521*3600*24.0,   /* siderial period (secs) */
		0.0
};

struct orbital_elements saturn = {
	        "Saturn orbit epoch 0h UTC 2/01/2020",
		SUN_CENTERED,
		REFER_TO_ECLIPTIC,
		113.5946551307333*PI/180.0, /* longitude of the node */
		2.48951313666079*PI/180.0, /* inclination */
		9.582321187871299*AU_METERS, /* semi-major axis */
		.05089701346182615, /* eccentricity */
		337.6598543256854*PI/180.0, /* argument of perifocus */
		204.1755160487530*PI/180.0, /* mean anomaly at epoch */
		2458880.5,      /* epoch (julian day) */
		10832.84321232275*3600*24.0,   /* siderial period (secs) */
		0.0
};


struct orbital_elements uranus = {
	        "Uranus orbit epoch 0h UTC 2/01/2020",
		SUN_CENTERED,
		REFER_TO_ECLIPTIC,
		74.09826836653*PI/180.0, /* longitude of the node */
		.770299236407*PI/180.0, /* inclination */
		19.1765481700291*AU_METERS, /* semi-major axis */
		.046525648029347, /* eccentricity */
		98.7872995621*PI/180.0, /* argument of perifocus */
		226.190164356*PI/180.0, /* mean anomaly at epoch */
		2458880.5,      /* epoch (julian day) */
		30672.17690836454*3600*24.0,   /* siderial period (secs) */
		0.0
};

struct orbital_elements neptune = {
	        "Neptune orbit epoch 0h UTC 2/01/2020",
		SUN_CENTERED,
		REFER_TO_ECLIPTIC,
		131.6525127313519*PI/180.0, /* longitude of the node */
		1.765025495643*PI/180.0, /* inclination */
		30.19585511912170*AU_METERS, /* semi-major axis */
		.01013724577619288, /* eccentricity */
		247.1680621609*PI/180.0, /* argument of perifocus */
		329.68890202468*PI/180.0, /* mean anomaly at epoch */
		2458880.5,      /* epoch (julian day) */
		60604.9703116934*3600*24.0,   /* siderial period (secs) */
		0.0
};

struct orbital_elements pluto = {
	        "Pluto orbit epoch 0h UTC 2/01/2020",
		SUN_CENTERED,
		REFER_TO_ECLIPTIC,
		110.2849003444*PI/180.0, /* longitude of the node */
		17.01684886610828*PI/180.0, /* inclination */
		39.43448087563598*AU_METERS, /* semi-major axis */
		.2491782686498017, /* eccentricity */
		113.3164454082509*PI/180.0, /* argument of perifocus */
		44.35257944493404*PI/180.0, /* mean anomaly at epoch */
		2458880.5,      /* epoch (julian day) */
		90450.84272746998*3600*24.0,   /* siderial period (secs) */
		0.0
};

/* An example of a retrograde asteroid. 
20461 Dioretsa (1999 LD31)
For stress-testing 
*/
struct orbital_elements dioretsa = {
	        "20461 Dioretsa orbit epoch 0h UTC 2/01/2020",
		SUN_CENTERED,
		REFER_TO_ECLIPTIC,
		297.838689890*PI/180.0, /* longitude of the node */
		160.4302985238535*PI/180.0, /* inclination */
		23.90411518586046*AU_METERS, /* semi-major axis */
		.8999731324605316, /* eccentricity */
		103.1487603939937*PI/180.0, /* argument of perifocus */
		62.19185168893080*PI/180.0, /* mean anomaly at epoch */
		2458880.5,      /* epoch (julian day) */
		42688.16004623155*3600*24.0,   /* siderial period (secs) */
		0.0
};

/* Halley's comet. Here we give the real JD of perihelion passage even 
though it would be a poor approximation to treat this orbit as 
parabolic.  

*/

struct orbital_elements halley = {
	        "1P/Halley orbit epoch 0h UTC 2/01/2020",
		SUN_CENTERED,
		REFER_TO_ECLIPTIC,
		58.168964424*PI/180.0, /* longitude of the node */
		162.3166380231*PI/180.0, /* inclination */
		17.86837370486040*AU_METERS, /* semi-major axis */
		.9662857013350251, /* eccentricity */
		111.1063918105520*PI/180.0, /* argument of perifocus */
		162.2502542216505*PI/180.0, /* mean anomaly at epoch */
		2458880.5,      /* epoch (julian day) */
		27793.63935870046, /* sidereal period (days) */
		2446446.55756634732   /* perihelion passage (JD) */
};


/*
Omuamua: interloper from interstellar space discovered in 2017. An 
example of a hyperbolic. There was speculation for a time
that this might be an artificial object. (Rama, anyone?)
 N.B. a is replaced by perihelion distance and period is replaced
by mean motion for hyperbolic objects */

struct orbital_elements oumuamua = {
	        "1I/'Oumuamua (A/2017 U1) orbit epoch 0h UTC 11/22/2017",
		SUN_CENTERED,
		REFER_TO_ECLIPTIC,
		24.596909555232*PI/180.0, /* longitude of the node */
		122.74170628*PI/180.0, /* inclination */
		0.25591158*AU_METERS, /* perihelion distance */
		1.20113380, /* eccentricity */
		241.81053603049*PI/180.0, /* argument of perifocus */
		51.157619793825*PI/180.0, /* mean anomaly at epoch */
		2458080.5,      /* epoch (julian day) */
		0.68674695*PI/180.0,   /* N.B. mean motion rad/day*/
		2458006.0073   /* JD of perihelion passage */	
};


/* A very parabolic, relatively recent named comet. We use this to
test the parabolic case */

struct orbital_elements seki_lines = {
	        "C/1962 C1 (Seki-Lines) epoch 0h UTC 1/2/00",
		SUN_CENTERED,
		REFER_TO_ECLIPTIC,
		304.609*PI/180.0, /* longitude of the node */
		64.3684*PI/180.0, /* inclination */
		.030387096*AU_METERS, /* perihelion distance */
		1.000000, /* eccentricity (actually 1.000003) */
		11.481354*PI/180.0, /* argument of perifocus */
		208.13652204*PI/180.0, /* mean anomaly at epoch */
		2451545.5,      /* epoch (julian day) */
		.0000015117115*PI/180.0,   /* mean motion rads/day  */
		2437777.2299686  /* perihelion passage JD */
};

/* The last special type of orbit is the straight line orbit that
results when a body falls straight "down" (or up) onto the attractor. These 
orbits have zero angular momentum and we signal their presence by having
the semi-major axis/perihelion distance parameter equal to 0. The
argument of the perifocus is then equal to the polar angle at all times
so only the radius needs to calculated. One needs to know two other
values to completely determine the "orbit": the total energy and the initial
value of the radius. We use the eccentricity and mean anomaly parameters
for these

We adopt the converntion that if omega > 0 then motion is toward
the center of force (down) while if omega < 0 then motion is away */


struct orbital_elements freefall = {
	        "Elliptic fall from 2 Pallas position epoch 0h UTC 3/23/2018",
		SUN_CENTERED,
		REFER_TO_ECLIPTIC,
		173.08*PI/180.0, /* longitude of the node */
		34.837*PI/180.0, /* inclination */
		0.0, /* signals degenerate orbit  */
		-0.000000000001, /* Energy parameter: < 0 elliptic = 0 
                                    parabolic > 0
				hyperbolic. This is specific kinetic energy at
 				"infinity" divided by k^2 = MG*/
		(310.01)*PI/180.0,  /* +/- argument of perihelion */
		2.57*AU_METERS, /* radius at the epoch  */
		2458200.5,      /* epoch (julian day) */
		1684.869*3600*24.0,   /* meaningless for this type of orbit */
		0.0
};

	
struct orbital_elements builtins[NBUILTINS];

void load_builtins(void){

/* keep the latest solar elements first as they are used to handle
sun centered orbits */

	builtins[0] = sun2k;
	builtins[1] = luna2k;
	builtins[2] = sun;
	builtins[3] = pallas;
	builtins[4] = mars;
	builtins[5] = venus;
	builtins[6] = mercury;
	builtins[7] = jupiter;
	builtins[8] = saturn;
	builtins[9] = uranus;
	builtins[10] = neptune;
	builtins[11] = pluto;
	builtins[12] = oumuamua;
	builtins[13] = dioretsa;
	builtins[14] = halley;
	builtins[15] = seki_lines;
	builtins[16] = freefall;
}

extern double period(struct orbital_elements *);
extern void dump_orbit(struct orbital_elements *);


/* replace the newline in a string gotten by fgets with a null character */

void strip(char *buf){
	char *p;

	p = strchr(buf,'\n');
	if(p) *p = '\0';
}


struct orbital_elements *dialog(void){

	struct orbital_elements *orbit;
	char buffer[80];
	double t;

	orbit = (struct orbital_elements *)malloc(sizeof(struct orbital_elements));
	if(!orbit){
		fprintf(stderr,"Cannot allocate elements structure.\n");
		return NULL;
	}

while(1){

	printf("\n\nAt the prompt, enter indicated element of orbit.\n");
	printf("Elements are relative to the ecliptic.\n\n");

	printf("Name for this orbit: ");
	if(!fgets(buffer,80,stdin))return NULL;
	strip(buffer);
	orbit->name = (char *)malloc(sizeof(char)*strlen(buffer)+1);
	if(!orbit->name){
		fprintf(stderr,"Cannot allocate space for name.\n");
		return NULL;
	}
	strcpy(orbit->name,buffer); 

	printf("Longitude of ascending node[degrees]: ");
	if(!fgets(buffer,80,stdin))return NULL;
	strip(buffer);
	orbit->BigOmega = atof(buffer)*PI/180.0;

	printf("Polar angle of inclination[degrees]: ");
	if(!fgets(buffer,80,stdin))return NULL;
	strip(buffer);
	orbit->i = atof(buffer)*PI/180.0;

	printf("Semi-major axis[km]: ");
	if(!fgets(buffer,80,stdin))return NULL;
	strip(buffer);
	orbit->a = atof(buffer)*1000.0;

	printf("Eccentricity[dimensionless]: ");
	if(!fgets(buffer,80,stdin))return NULL;
	strip(buffer);
	orbit->e = atof(buffer);

	printf("Argument of perigee[degrees]: ");
	if(!fgets(buffer,80,stdin))return NULL;
	strip(buffer);
	orbit->omega = atof(buffer)*PI/180.0;

	printf("Mean argument of body at epoch[degrees]: ");
	if(!fgets(buffer,80,stdin))return NULL;
	strip(buffer);
	orbit->arg = atof(buffer)*PI/180.0;

	while(1){
		printf("Date of epoch[mm/dd/yyyy]: ");
		if(!fgets(buffer,80,stdin))return NULL;
		strip(buffer);
		if(ParseDate(buffer,&t)){
			printf("Not in required format. Try again.\n");
			continue;
		}
		break;
	}

	orbit->epoch = t;

	while(1){
		printf("Time(UTC) of epoch[hh:mm:ss.s]: ");
		if(!fgets(buffer,80,stdin))return NULL;
		strip(buffer);
		if(secs(buffer,&t)){
			printf("Not in required format. Try again.\n");
			continue;
		}
		break;
	}
	orbit->epoch += t/(3600.0*24.0);

	printf("Orbital period[seconds]: ");
	if(!fgets(buffer,80,stdin))return NULL;
	strip(buffer);
	orbit->period = atof(buffer);

	printf("\n\n");
	dump_orbit(orbit);
	printf("\n\nAre these correct[yn]? ");
	if(!fgets(buffer,80,stdin))return NULL;
	strip(buffer);
	if(strcmp(buffer,"y")==0)break;
}
	return orbit;
}
