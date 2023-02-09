/* physical_constants.h: purpose is obvious. Also define other constants */
/* We'll work in mks units throughout */ 
#ifndef PI 
#define PI 3.14159265 
#endif 
/* J2000 Julian Day. It is 1/1/2000 at 12h UTC */
#define J2000 2451545 
#define EARTH_EQUATORIAL_RADIUS 6.378E06 
#define EARTH_POLAR_RADIUS 6.357E06 
#define EARTH_RADIUS ((EARTH_POLAR_RADIUS + EARTH_EQUATORIAL_RADIUS)/2.0) 
/* The value of little G is hard to pin down, since it varies
from 9.780 at the equator to 9.832 at the poles. The conventional average
value is 9.80665. We arrived at the following compromise experimentally
by comparing the Lunar Mean orbit with one that osculates with it at 
Y2K */
#define LITTLE_G 9.804371
/* #define LITTLE_G 9.80665 *//* m/sec**2 */ 
#define EARTH_MG 3.986004419E14
/* #define EARTH_MG (LITTLE_G*EARTH_RADIUS*EARTH_RADIUS) */ 
#define SOLAR_MG 1.32712440042E20  /* m^3/sec^2 */
#define GAUSSIAN_CONSTANT 0.01720209895 /* radians/day */
#define EARTH_MASS 5.9742E24 
#define MOON_MASS (EARTH_MASS * 0.01230002) 
/* Obliquity of the ecliptic at epoch 2000 */ 
#define EPSILON_2K (23.4392911*PI/180.0)
#define SECS_PER_DAY (60*60*24)
#define AU_METERS 149597870700  /* Now defined exactly */
