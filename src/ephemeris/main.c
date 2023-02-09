/* 

   Main program of ephemeris package. Handles command line options, 
   writes lines of an ephemeris for an object orbiting the earth. 

   Author: Terry R. McConnell
	   trmcconn@syr.edu

*/

#include "ephemeris.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<float.h>
#include<math.h>
#include<time.h>
#include "constants.h"

#define VERSION "1.0"
#define BANNER "\n			Ephemeris (1.0)\n\n"	 

#define USAGE "ephemeris [-d -h -v -V -i -u <nn>] date time [ date time [ interval ]]"

#ifdef _SHORT_STRINGS
#define HELP USAGE
#else
#define HELP "\n"USAGE"\n\n\
-h: print this helpful message\n\
-v: print version number and exit\n\
-V: more verbose operation\n\
-d: information listing for selected (default all) orbit(s)\n\
-u: select orbit number nn \n\
-i: interactively enter the orbital elements to be used.\n\
Print ephemeris of selected (default = sun) for given date and time (UTC).\n\
Additional optional arguments give the last date and time to include and \n\
the time between entries. Default is to give lines for 30 days, 1 day apart.\n\
All time entries should be in hh:mm:ss.s format.\n\n"
#endif

extern int secs(char *,double *);
extern int ParseDate(char *,double *);
extern void facts(double);
extern void load_builtins(void);
extern struct orbital_elements *dialog(void);
extern time_t jd2timet(double ajd);
int verbose = 0;

int
main(int argc, char **argv)
{
	int n = 1,i=1,j;
	int m=1;
	double secs_after_midnight_utc, epoch,start_epoch,end_epoch,delta_epoch;
	char buf[1024];
	struct event *ecl_event;
	int dump_flag = 0;
	struct spherical_coordinate *cel_coord;
	struct orbital_elements *an_orbit = NULL ;
	time_t atime;
	char timebuf[13] = "Local Date  "; /* After header is printed, this gets overwritten with date strings */
	
#if 0
	/* This is an experimental section */

	/* Find the initial cartesian coordinates of the moon at 2000 */
	{
		struct cartesian_coordinate c1,c2;
		struct spherical_coordinate s1,s2;
		struct orbital_elements *osc;

		s1.type = ECLIPTIC;
		s1.theta = RADIANS(5.18);
		s1.phi = RADIANS(205.08);
		s1.r = 62.262*EARTH_RADIUS;
		s2.type = ECLIPTIC;
		s2.theta = RADIANS(5.23);
		s2.phi = RADIANS(217.28);
		s2.r = 62.860*EARTH_RADIUS;
		sphere2cart(&s1,&c1);
		sphere2cart(&s2,&c2);
	/*	printf("r=%f,x=%f,y=%f,z=%f\n",s1.r,c1.x,c1.y,c1.z);
		printf("dx=%f,dy=%f,dz=%f\n",c2.x - c1.x,c2.y-c1.y,c2.z - c1.x);
		printf("vx=%f,vy=%f,vz=%f",(c2.x-c1.x)/SECS_PER_DAY,(c2.y-c1.y)/SECS_PER_DAY,(c2.z-c1.z)/SECS_PER_DAY);
		*/
		osc = osculating_orbit(MOON_MASS,c1.x,c1.y,c1.z, (c2.x-c1.x)/SECS_PER_DAY, 
			(c2.y-c1.y)/SECS_PER_DAY, (c2.z-c1.z)/SECS_PER_DAY,EARTH);
		if(osc == NULL){
			printf("Failed to create osculating orbit.\n");
			return 1;
		}
		osc->name = strdup("Lunar osculating orbit at 2000");
		osc->epoch = 2451543.5;
		dump_orbit(osc);
		return 0;
		
	}

#endif

	/* end experimental section */

	/* Load in predefined sets of orbital elements from elements.c */

	load_builtins();

	/* Process command line options. argc will count number of args
            that remain. */

	if(argc > 1){
	   argv++; argc--; /* skip program name */
	   while(argc && ((*argv)[0] == '-')){
		  if(strcmp(*argv,"-h")==0){
			printf("%s\n", HELP);
			exit(0);
		  }
		  if(strcmp(*argv,"-v")==0){
			printf("%s\n",VERSION);
			exit(0);
		  }
		  if(strcmp(*argv,"-d")==0){
			dump_flag = 1;
			argv++;argc--;
			continue;
		  }
		  if(strcmp(*argv,"-u")==0){
			argv++;
			j = atoi(*argv++);
			argc -=2;	
			if((j<1)||(j>nbuiltins)){
				fprintf(stderr,"ephemeris: argument of -u, %d, is out of range 1..%d\n",j,nbuiltins);
				exit(1);
			}
			an_orbit = &(builtins[j-1]);
			continue;
		  }
		  if(strcmp(*argv,"-V")==0){
			verbose = 1;
			argv++;argc--;
			continue;
		  }
		  if(strcmp(*argv,"-i")==0){
				argv++;argc--;
				an_orbit = dialog();
				if(!an_orbit)exit(1);
				continue;
		  }
		  fprintf(stderr, "ephemeris: Unknown option %s\n", argv[i]);
		  fprintf(stderr, "%s\n",USAGE);
		  return 1;
	   }
	}
	else {
		  fprintf(stderr, "%s\n",USAGE);
		  return 1;
	}

	if(dump_flag){
		if(an_orbit)
			info_orbit(an_orbit);
		 else
			dump_builtins();
		exit(0);
	}
	
/* If we got here there should be at least two more arguments - the starting
   date and time. If not present, the user needs to instructed on how
   to use the program */
	if(argc < 2){
		fprintf(stderr, "%s\n",USAGE);
		return 1;
	}

	/* assume what's left on the command is a date followed by a time */

	if(!an_orbit)an_orbit = &(builtins[0]); /* default orbit */

	strcpy(buf,*argv++);
	argc--;
	if(ParseDate(buf,&start_epoch)) {
		fprintf(stderr, "ephemeris: cannot parse %s as date\n",
			buf);
		return 1;
	}

	if(argc == 0){
		fprintf(stderr,"ephemeris: time of day missing.\n");
		return 1;
	}
	strcpy(buf,*argv++);
	argc--;
	if(secs(buf,&secs_after_midnight_utc)){
		fprintf(stderr, "ephemeris: couldn't interpret %s as time xx:xx:xx.x\n", buf);
		return 1;
	}
        /* Adjust for actual utc time of day. Recall that the julian day 
            began at the previous noon, UTC */

	start_epoch += (secs_after_midnight_utc)/(24.0*60.0*60.0);  

	/* preset defaults for these */
	delta_epoch = 1.0;
	end_epoch = start_epoch + 30.0;

	/* If there is anything else on the command line it must be
           the optional end_epoch, etc. Repeat above exercise, if so */

	if(argc){
		strcpy(buf,*argv++);
		argc--;
		if(ParseDate(buf,&end_epoch)) {
			fprintf(stderr, "ephemeris: cannot parse %s as date\n",
				buf);
			return 1;
		}
		strcpy(buf,*argv++);
		argc--;
		if(secs(buf,&secs_after_midnight_utc)){
			fprintf(stderr, "ephemeris: couldn't interpret %s as time\n",
				buf);
			return 1;
		}


		end_epoch += (secs_after_midnight_utc)/(24.0*60.0*60.0);  

		/* Finally, if there is an optional delta, fetch and process */

		if(argc){
			strcpy(buf,*argv++);
			argc--;
			if(secs(buf,&delta_epoch)){
				fprintf(stderr, "ephemeris: couldn't interpret %s as time\n",
					buf);
				return 1;
			}

			/* convert from seconds to days */
			delta_epoch = delta_epoch/(3600.0*24.0);

			/* Do a sanity check: if it's <= 0 we'll loop
                           forever */
			if(delta_epoch < .0001){
				fprintf(stderr,"ephemeris: crazy delta %g\n",
					delta_epoch);
				return 1;
			}
		}
	}


	printf(BANNER);
	printf("Ephemeris for %s\n\n", an_orbit->name);
	if(verbose)
	printf("%sJD       C.Long  C. Lat    R.A.(h m s)    Declination   r\n",timebuf);
	else
	printf("%sJD       C.Long  C. Lat    R.A.(h m s)    Declination\n",timebuf);
	printf("-----------------------------------------------------------------\n");
	for(epoch = start_epoch; epoch  <= end_epoch; epoch += delta_epoch){
		int hr,min,deg,mm,sign;
		double sec,temp,ss;

		ecl_event = ephemeris(epoch,an_orbit);
		if(!ecl_event){
			fprintf(stderr,"main: ephemeris returned NULL\n");
			exit(1);
		}
		cel_coord = convert_to(ecl_event->where,CELESTIAL);
		if(!cel_coord){
			fprintf(stderr,"ephemeris: error converting to ra/decl\n");
			exit(1);
		}

		/* Express Right Ascension in hours, minutes, seconds */

		temp = DEGREES(cel_coord->phi);
		hr = (int) (temp/15.0);
		temp -= ((double)hr)*15.0;
		temp = temp*4.0;  /* 4 = 60/15: degrees --> minutes of time */
		min = (int)temp;
		temp -= min;
		sec = 60*temp;

		/* Express Declination  in degrees, minutes, seconds */

		temp = DEGREES(cel_coord->theta);
		if(temp<0)sign = -1;
		else sign = 1;
		temp = fabs(temp);
		deg = (int)temp;
		temp -= deg;
		deg = sign*deg;
		mm = (int)(60.0*temp);
		ss = 60.0*(60.0*temp - mm);
		
		/* Figure out the local date corresponding to JD of event */
	    atime = jd2timet(ecl_event->when);
		strftime(timebuf,12,"%x", localtime(&atime));
		printf("%s ",timebuf);
		if(verbose){
		printf("%-12.1f%-8.2f%-8.2f  %2d %2d %5.2f   %3d %2d %5.2f%6.2f \n",
		  ecl_event->when, DEGREES(ecl_event->where->phi),
                  DEGREES(ecl_event->where->theta),hr,min,sec,deg,mm,ss,
			ecl_event->where->r/AU_METERS); 
		}
		else {
		printf("%-12.1f%-8.2f%-8.2f  %2d %2d %5.2f   %3d %2d %5.2f \n",
		  ecl_event->when, DEGREES(ecl_event->where->phi),
                  DEGREES(ecl_event->where->theta),hr,min,sec,deg,mm,ss); 
		}
		free(ecl_event->where);
		free(ecl_event);
		free(cel_coord);
	}
	return 0;
}
