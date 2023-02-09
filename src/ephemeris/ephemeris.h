
/* ephemeris.h: common includes for ephemeris */


/* The goal is to calculate the position (normally in celestial latitude/
   	longitude) of the Moon or other EARTH satellite at a given instant.
*/

/* disables some annoying compiler warnings: */
#define _CRT_SECURE_NO_WARNINGS

#define SUN_CENTERED 0
#define EARTH_CENTERED 1
#define REFER_TO_ECLIPTIC 0
#define REFER_TO_EQUATOR 1

/* Macro for converting to Degrees from radians */
#define DEGREES(r)  (180.0*(r)/(PI))
#define RADIANS(d)  ((PI)*(d)/180.0)

/* These determine where the eccentricity is so close to 1 that
   we treat the orbit as parabolic, necessary since the methods
   for elliptic and hyperbolic orbits break down at e = 1.  */

#define PARABOLIC_HI 1.0000005
#define PARABOLIC_LO .99999995

/* structure for standard description of orbital elements */

struct orbital_elements {
	char *name;       /* (optional) name for orbit */
	char  center;     /* select earth or sun-centered */
	char reference;   /* reference plane selector */
	double BigOmega;  /* longitude(or R.A.)of ascending node */
	double i;         /* polar angle of inclination */
	double a;        /*  semi-major axis */
	double e;        /*  eccentricity */
	double omega;    /*  argument of the perigee: angle measured from node
			     to perigee in plane of oribit */
	double arg;       /* Mean anomaly: angle in plane of orbit from
			     perigee to mean-motion position at the epoch */ 

	double epoch;     /* Julian date on which elements apply */ 
	double period;    /* This is not an orbital element, per se, but is
                             included for greater accuracy modelling the moon. 
                             For small satellites of earth, we can compute
                             the period using EARTH_MG as the force constant,
                             but we should use the reduced mass in the
                             case of the moon. This is given in seconds. */ 
	double t0;        /* JD of perihelion passage. This only really
                             gets used when the eccentricity = 1 or is
                             sufficiently close to 1 as determined by the
                             settings of PARABOLIC_HI and PARABOLIC_LO, so
			     a nonsense value (say 0.0) could be entered  
                             in all other cases */
};


/* define coordinate types */

#define CELESTIAL 0  /* declination and right-ascension */
#define GALACTIC  1  /* for Mr. Sulu :-) */
#define ECLIPTIC  2  /* celestial latitude and longitude */

#define EARTH 0 
#define SUN 1

struct spherical_coordinate {
	int type; /* one of the above */	
	double phi;  /* azimuthal angle */
	double theta; /* polar angle = latitude */
	double r;  /* radius */
};

struct cartesian_coordinate {
	int type;
	double x;
	double y;
	double z;
};


struct event {
	struct spherical_coordinate *where;
	double when;
	double v;  /* cartesian velocity */ 
};

#define KEPLER_METHOD 0 /* Binary search */    

extern int verbose;

/* Builtin sets of orbital elements defined in elements.c: */

extern int nbuiltins;
extern struct orbital_elements builtins[];

extern struct orbital_elements *osculating_orbit( double m, double x, double y, double z, double vx, double vy, double vz, int origin);

extern struct spherical_coordinate *convert_to(struct spherical_coordinate *source,
	int target_type);
extern int sphere2cart(struct spherical_coordinate *, struct cartesian_coordinate *);
extern int cart2sphere(struct cartesian_coordinate *, struct spherical_coordinate *);

extern struct event *ephemeris_kepler( double epoch, struct orbital_elements *orbit);
extern struct event *ephemeris( double epoch, struct orbital_elements *orbit);
extern void dump_builtins(void);
extern void dump_orbit(struct orbital_elements *);
extern void info_orbit(struct orbital_elements *);

