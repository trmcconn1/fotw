/* This files contains the main routines for generating tracks and
track segments */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<strings.h>
#include<float.h>
#include<math.h>
#include<time.h>
#include "xml.h"
#include "gpxpoly.h"
#include "constants.h"
#include "parse.h"


void  mkheadend(double xlat,double xlong,
	double azimuth, double dist, double *ylat, double *ylong, 
		int method);

char method = GREAT_CIRCLE;  /* Default connection method */
static double dist_conv = 1.0; /* Default distance conversion factor */

/* Parse input file and write xml to make a track based on the instructions 
therein (See README for information on format of the input file)
Call mktrkseg as needed to make track segments */



void mktrk(FILE *in, FILE *out){

	int k;
	int status;
	int lines = 1;
	int parity = 0;
	int heading_flag = 0;
	char buf[1024];
	char *token;
	double secs,degs;
	double xlat,ylat,xlong,ylong;
	int points = 100;

/* Write opening trk tag */

	fprintf(out,"<trk>\n");

/* Parse the input file and write trk segments as node pairs are encountered */

	while(1){
		token = parse(in,&status);
		if(status == MY_EOF)break;
		if(status == MY_EOL){
			lines++;
			continue;
		}
		buf[0] = '\0';
		if(strcasecmp(token,"NAME") == 0){
			while(1){
				token = parse(in,&status);
				if(status == MY_EOF)break;
				if(status == MY_EOL){
				        lines++;
					break;
				}
				strcat(buf,token);
				strcat(buf," ");
			}

/* print the name of the track */

			fprintf(out,"<name> %s </name>\n",buf);

			if(status == MY_EOF)break;
			continue;
		}
		if(strcasecmp(token,"HEADING")==0){
			heading_flag = 1;
			if(status == MY_EOF)break;
			continue;
		}
		if(strcasecmp(token,"MILES")==0){
			dist_conv = MILE2KM*1000.0;
			if(status == MY_EOF)break;
			continue;
		}
		if(strcasecmp(token,"METERS")==0){
			dist_conv = 1.0;
			if(status == MY_EOF)break;
			continue;
		}
		if(strcasecmp(token,"FEET")==0){
			dist_conv = FT2METER;
			if(status == MY_EOF)break;
			continue;
		}
		if(strcasecmp(token,"POINTS") == 0){
			token = parse(in,&status);
			if(status != MY_NORMAL){
				fprintf(stderr,"Syntax error line %d\n",lines);
				return;
			}
			points = atoi(token);
			if((points < 1)||(points > MAX_POINTS)){
				fprintf(stderr,"Bad points value %d on line %d\n",
					points,lines);
				return;
			}
			while(1){
				token = parse(in,&status);
				if(status == MY_EOF)break;
				if(status == MY_EOL){
					lines++;
					break;
				}
			}
			if(status == MY_EOF)break;
			continue;
		}
		if(strcasecmp(token,"METHOD") == 0){
			token = parse(in,&status);
			if(status != MY_NORMAL){
				fprintf(stderr,"Syntax error line %d\n",lines);
				return;
			}
			switch(token[0]){

				case 'G':
				case 'g':
					method = GREAT_CIRCLE;
					break;
				case 'S':
				case 's':
					method = SIMPLE_INTERPOLATION;
					break;
				case 'l':
				case 'L':
					method = LOXODROME;
					break;
				default:
					fprintf(stderr,"Unknown method line %d\n",lines);
					return;
			}
			while(1){
				token = parse(in,&status);
				if(status == MY_EOF)break;
				if(status == MY_EOL){
					lines++;
					break;
				}
			}
			if(status == MY_EOF)break;
			continue;
		}

/* If we get to here, the next tokens should be a parts of a lat/long 
   string */

		strcat(buf,token);
		strcat(buf," ");
		while(1){
			token = parse(in,&status);
			if(status == MY_EOF)break;
			if(status == MY_EOL){
				lines++;
				break;
			}
			strcat(buf,token);
			strcat(buf," ");
		}
		if(status == MY_EOF)break;

/* Figure out whether this field is a latitude longitude or
   what? */

/* We use the parity variable to track gathering pairs of coordinates
to make a node, and pairs of nodes to mark the start/end of a new
track segment. */

		k = dms2s(buf,&secs);
		if(debug)
			printf("parsed coord. parity = %d k= %d,string = %s\n",parity,k,buf);
		if(parity%4 < 2)
		switch(k){ 
			case NLAT:
				xlat = S2DEGS(secs);
				parity++;
				continue;
			case SLAT:
				xlat = -S2DEGS(secs);
				parity++;
				continue;
			case WLONG:
				xlong = -S2DEGS(secs);
				parity++;
				continue;
			case ELONG:
				xlong = S2DEGS(secs);
				parity++;
				continue;
			case INDEFINITE:
				if(parity%2 == 0)
				xlat = S2DEGS(secs);
				else xlong = S2DEGS(secs);
				parity++;
				continue;
			default:  /* OK, let's try the other format */
				k = dm2s(buf,&secs);
				switch(k){
					case NLAT:
						xlat = S2DEGS(secs);
						parity++;
						continue;
					case SLAT:
						xlat = -S2DEGS(secs);
						parity++;
						continue;
					case WLONG:
						xlong = -S2DEGS(secs);
						parity++;
						continue;
					case ELONG:
						xlong = S2DEGS(secs);
						parity++;
						continue;
					case INDEFINITE:
						if(parity%2 == 0)
						xlat = S2DEGS(secs);
						else xlong = S2DEGS(secs);
						parity++;
						continue;
					default:
						if(parity%2==0)
						xlat = atof(buf);
						else
						xlong = atof(buf);
						parity++;
						continue;
					
				}
			 
		}
		else /* 2nd point of a pair */
		switch(k){ 
			case NLAT:
				ylat = S2DEGS(secs);
				parity++;
				break;
			case SLAT:
				ylat = -S2DEGS(secs);
				parity++;
				break;
			case WLONG:
				ylong = -S2DEGS(secs);
				parity++;
				break;
			case ELONG:
				ylong = S2DEGS(secs);
				parity++;
				break;
			case INDEFINITE:
				if(parity%2 == 0)
				ylat = S2DEGS(secs);
				else
				ylong = S2DEGS(secs);
				parity++;
				break;
			default:  /* OK, let's try the other format */
				k = dm2s(buf,&secs);
				switch(k){
					case NLAT:
						ylat = S2DEGS(secs);
						parity++;
						break;
					case SLAT:
						ylat = -S2DEGS(secs);
						parity++;
						break;
					case WLONG:
						ylong = -S2DEGS(secs);
						parity++;
						break;
					case ELONG:
						ylong = S2DEGS(secs);
						parity++;
						break;
					case INDEFINITE:
						if(parity%2==0)
						ylat = S2DEGS(secs);
						else
						ylong = S2DEGS(secs);
						parity++;
						break;
					default:
						if(parity%2==0)
						ylat = atof(buf);
						else ylong = atof(buf);
						parity++;
						break;
				}
			 
			}
			if(parity%4==0){

/* create track segment from (xlat,xlong) to (ylat,ylong) */

/* If this is a heading, we need to compute y as the far end of it. 
We pass ylat as the azimuth, and ylong as the distance */

			if(heading_flag)
				mkheadend(xlat,xlong,ylat,ylong,
					&ylat,&ylong,method);

			mktrkseg(out,xlat,xlong,ylat,ylong,method,points);

/* Done with current track segment */
                       /* move y to x */		
			xlat = ylat;
			xlong = ylong;
			parity = 2;
			heading_flag = 0;

			} /* end of if parity == 4 */
		} /* end of input file parsing loop */

/* Write closing trk tag */

	fprintf(out,"</trk>\n");
}

/*  Print to file handle out the xml for a track segment of
given number of points from (xlat,xlong) to (ylat,ylong) according
to the given method */

void mktrkseg(FILE *out,double xlat,double xlong,double ylat,
		double ylong,int method, int points)
{

	double t,r,T,delta;
	struct cartesian_coordinate ccx,ccy,ccz;
	struct spherical_coordinate scx,scy,scz;

	delta = 1/((double)points);

/* Fill in the sperical coordinates of x and y */

	scx.type = CELESTIAL;
	scx.phi = RADIANS(xlat);
	scx.theta = RADIANS(xlong);
	scx.r = EARTH_RADIUS;
	scy.type = CELESTIAL;
	scy.phi = RADIANS(ylat);
	scy.theta = RADIANS(ylong);
	scy.r = EARTH_RADIUS;

/* Initialize parts of intermediate point z that don't change */

	ccz.type = CELESTIAL;
	scz.r = EARTH_RADIUS;

/* print the open trkseg tag */
	fprintf(out,"<trkseg>\n");

/* in here we write the successive trkpts */
		
	t = 0.0;
	while(t <= 1.0){
		
/* We elaborate on each method of interpolating track points as that
		method is used */
		
	switch(method){
	
		case GREAT_CIRCLE:
		
/* Connect x and y with a great circle arc. This is actually a lot
simpler than it sounds. First, compute the cartesian coordinates of
x and y. The line segment in space between them is then a chord of the
great circle between them since it lies in the plane determined by x,y, 
and the center of the earth. 
		
Form track points by subdividing the line segment from x to y with 
convex combinations of x and y. Project subdivsion points out onto the surface
of the sphere (earth) by normalizing these points' radius to be the same
as x and y's, i.e. the radius of the Earth. */ 
		
			sphere2cart(&scx,&ccx); /* convert to cartesian */
			sphere2cart(&scy,&ccy);
			ccz.x = (1.0-t)*ccx.x + t*ccy.x; /* z is convex combo */
			ccz.y = (1.0-t)*ccx.y + t*ccy.y; /* x and y */
			ccz.z = (1.0-t)*ccx.z + t*ccy.z; 
		
			/* renormalize z */
			r = sqrt((ccz.x)*(ccz.x) + (ccz.y)*(ccz.y) + 
				(ccz.z)*(ccz.z));
			ccz.x = ccz.x/r;
					ccz.y = ccz.y/r;
			ccz.z = ccz.z/r;
			cart2sphere(&ccz,&scz); /* convert back to spherical */
			break; 
		
		case SIMPLE_INTERPOLATION:
		
/* This method is even more simple-minded than the great circle: form
convex combinations of the spherical coordinates themselves. 
*/
			scz.theta = (1.0-t)*scx.theta + t*scy.theta;
			scz.phi = (1.0-t)*scx.phi + t*scy.phi;
			break;
		
		case LOXODROME: 
		
		/* A loxodrome, or rhumb line, is a curve of constant heading - it makes the
same angle with each meridian it crosses. In this method we interpolate
between x and y by forming the loxodrome between them. Since a loxodrome is
not the arc of a great circle, it cannot be found - except in very special
cases - by using spherical trigonometry. For the following discussion I 
have borrowed heavily from the Wikipedia article on the subject. 
		
Let p0,l0 be the latitude and longitude of x and p,l the latitude of an
arbitrary point on the loxodrome that makes an angle B with the meridian
l = l0. (B is the angle between tangent vectors, of course.) The basic
result (see below for the derivation) is that dl/dp = tan B sec p along
the loxodrome when it is parametrized by the latitude p. Integrating with
respect to p gives 
		
l - l0 = tan B (arctanh(sin p) - arctanh(sin p0))
	
Sketch of the derivation: The radius vector is r(p,l) = cos l cos p i +
sin l cos p j + sin p k. Differentiation wrt l and p gives a pair of orthogonal
vectors that span the tangent plane to the sphere at (p0,l0):
		
r_p = -cos l0 sin p0 i - sin l0 sin p0 j + cos p0 k := w 
		
r_l = -sin l0 cos p0 i + cos l0 cos p0 j
		
The first of these,w,  is a unit vector, but the second is not. It has magnitude
cos p0. Let v be r_l/cos p0. It follows then that the unit tangent to 
the desired loxodrome at (p0,l0) is  u = cos B w + sin B v
	
An increment ds of arclength along the loxodrome produces an increment in r
of dr = u ds. Expanding both sides separately we obtain the vector equation
		
	r_l dl + r_p dp = cos B w ds + sin B v ds
		or
	cos p0 v dl + w dp = cos B w ds + sin B v ds
		
Since v and w are linearly independent (indeed, orthogonal), we obtain two
equations:
		
ds =  (1/cos B) dp  and ds = (cos p0/ sin B) dl. Solving for dl/dp gives the
desired differential relation along the loxodrome */    
		
			/* Find tan B using latitude and longitude of
	                         the endpoints */
		
			T = (scy.theta - scx.theta)/(atanh(sin(scy.phi)) -
					atanh(sin(scx.phi)));
		
		/* Find latitude of point along loxodrome for given
                   t as convex combo of endpoint latitudes */
	
			scz.phi = (1.0-t)*scx.phi + t*scy.phi;
		
		/* Then compute longitude of same point */
		
			scz.theta = scx.theta + T*(atanh(sin(scz.phi)) -
						atanh(sin(scx.phi)));

			break;
		 
		default:
			return; /* should not happen */
		} /* end of method switch */
		
		fprintf(out,"<trkpt lat=\"%10.6f\" lon=\"%10.6f\"></trkpt>\n",
			DEGREES(scz.phi),DEGREES(scz.theta)); 
		t += delta;
	} /* end of create trk segment loop */
		
		
	/* print the close trkseg tag */
	fprintf(out,"</trkseg>\n");
}

/* Compute the far endpoint y of a great circle or loxodrome
(depending on value of method) from x with given azimuth and 
length. Coordinate of the far point are returned through the
pointer arguments */

void  mkheadend(double xlat,double xlong,
	double azimuth, double dist, double *ylat, double *ylong, 
		int method)
{

	double L,p,T,delta = 0.0000001,n;
	double mylong,mylat,mydist;

	mydist = dist*dist_conv;
	if((method == GREAT_CIRCLE)||(method == SIMPLE_INTERPOLATION)){
		
/* The endpoints X and Y of the great circle to be determined, together
with the North Pole N, form the vertices of a spherical triangle. Side y,
opposite Y, connects X to N and is a meridian of longitude with length equal
to the co-latitude of X. The side n, opposite N, has angular length 
determined by the desired distance, dist. The given azimuth is the
spherical angle between sides y and n. Thus, we can first find x, the side
opposite X, using the cosine law. It gives the co-latitude of Y. The
longitude of Y can be inferred from the sperical angle at N (found using the
sine law) and the given latitude of X */   

/* Angle at center of earth subtended by desired great circle: */

		n = mydist/EARTH_RADIUS;

/* The cosine law in action: */

		mylat = asin(cos(n)*sin(RADIANS(xlat)) +
			sin(n)*cos(RADIANS(xlat))*cos(RADIANS(azimuth)));

/* The sine law in action: */

		mylong = RADIANS(xlong) + 
	          asin(n*sin(RADIANS(azimuth))/cos(mylat));

	        *ylat = DEGREES(mylat);
		*ylong = DEGREES(mylong);
		return;
	}
	else {  /* Method == Loxodrome */

		T = tan(RADIANS(azimuth));
		L = RADIANS(xlong); /* parametrize rhumb by longitude */
		p = RADIANS(xlat);
		n = 0.0;  /* running total of rhumb arc-length */

/* See discussion of theory in mktrkseg above. B = azimuth. 
ds =  (1/cos B) dp  and ds = (cos p0/ sin B) dl. Solving for dl/dp gives the
desired differential relation along the loxodrome */    
		
		while(1){	

/* BUG: worry about azimuth = 0 */
/* Total up arc length along rhumb and break when it first exceeds
  the desired distance */

			n+= EARTH_RADIUS*delta/(2.0*PI*cos(RADIANS(azimuth)));
			L += delta;
			if(n >= mydist)break;
		}
		*ylat = DEGREES(xlat + T*(atanh(sin(L))) - 
			atanh(sin(RADIANS(xlong))));
		*ylong = DEGREES(L);
		return;
	}
}
