/* Imported from my Ephemeris program so there is some inappropriate stuff */

#define NLAT 2
#define SLAT -2
#define ELONG 3
#define WLONG -3
#define INDEFINITE 0
#define NOTWFF -1
#define ARG_SIZE 32 
#define MAX_POINTS 1048526 /* That oughta hold the little bastards */

/* methods for connecting vertices */
#define GREAT_CIRCLE 0
#define SIMPLE_INTERPOLATION 1
#define LOXODROME 2

struct spherical_coordinate {
	int type; 	
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

extern int debug;
extern int dms2s(char *,double *);
extern int dm2s(char *,double *);
extern int s2degs(double,double *);
extern int secs(char*, double *);
extern struct spherical_coordinate *convert_to(struct spherical_coordinate *source,
	int target_type);
extern int sphere2cart(struct spherical_coordinate *, struct cartesian_coordinate *);
extern int cart2sphere(struct cartesian_coordinate *, struct spherical_coordinate *);

extern void mktrk(FILE *in,FILE *out);
extern void mktrkseg(FILE *out,double xlat,double xlong,double ylat,
		double ylong,int method, int points);
extern int track_dialog(FILE *);

/* Macro for converting to Degrees from radians */
#define DEGREES(r)  (180.0*(r)/(PI))
#define RADIANS(d)  ((PI)*(d)/180.0)
#define S2DEGS(s)    ((s)/(3600.0))
/* define coordinate types */

#define CELESTIAL 0  /* declination and right-ascension */
#define GALACTIC  1  /* for Mr. Sulu :-) */
#define ECLIPTIC  2  /* celestial latitude and longitude */

#define EARTH 0 
#define SUN 1
