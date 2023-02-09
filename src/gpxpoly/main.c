/* 

   Main program of gpxpoly: it will create a gpx trail connecting
   designated coordinates  

   Author: Terry R. McConnell
	   trmcconn@syr.edu

*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "xml.h"
#include "gpxpoly.h"

/* Data for testing */
#define CAMP_LAT "43 35' 40.7\" N"
#define CAMP_LONG "75 35' 13.5\" W"
#define DECLOT_LAT "43 36' 03.1\" N"
#define DECLOT_LONG "75 33' 01.0\" W" 
#define CAMPLAT "43 35.678' N"
#define CAMPLONG "75 35.225' W"
#define DECLOTLAT "43 36.052' N"
#define DECLOTLONG "75 33.017' W" 

#define VERSION "1.0"
#define BANNER "\n			GPXPOLY (0.1)\n\n"	 

#define USAGE "gpxpoly [-hvdi] [-I file] [-f file] if1 [if2 ...]"

#ifdef _SHORT_STRINGS
#define HELP USAGE
#else
#define HELP "\n"USAGE"\n\
-h: print this helpful message\n\
-v: print version number and exit\n\
-d: turn on debug mode\n\
-f: write gpx to file instead of stdout \n\
-i: start interactive dialog to enter track information\n\
-I: same as i but dialog result also saved in file \n\
Create gpx file containing a polygonal track\n\
according to specification in input file(s)\n\n"
#endif

int debug = 0;
extern char method;
static char tempfile_name[256] = "/tmp/gpxpolyXXXXXXX";

int
main(int argc, char **argv)
{
	FILE *out,*in;
	int iflag = 0; /* interactive option flag */

	out = stdout;

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
		  if(strcmp(*argv,"-i")==0){
			mkstemp(tempfile_name);
			out = fopen(tempfile_name,"w");
			if(!out){
				fprintf(stderr,"Could not open temp file\n");
				return 1;
			}
			track_dialog(out);
			fclose(out);
			iflag = 1;
			argv++; argc--;
			continue;
		  }
		  if(strcmp(*argv,"-I")==0){
			argv++;
			argc--;
			if(!argc){
				fprintf(stderr,"%s\n",USAGE);
				return 1;
			}
			out = fopen(*argv,"w");
			if(!out){
				fprintf(stderr,"Could not open %s\n",*argv);
				return 1;
			}
			track_dialog(out);
			fclose(out);
			strcpy(tempfile_name,*argv);
			iflag = 1;
			argv++; argc--;
			continue;
		  }
		  if(strcmp(*argv,"-d")==0){
			debug = 1;
			argv++;
			argc--;
			continue;
		  }
		  if(strcmp(*argv,"-f")==0){
			argv++;argc--;
			if(!(out = fopen(*argv,"w"))){
				fprintf(stderr,"Unable to open %s\n",
				    *argv);
				return 1;
			}
			argv++;argc--;
			continue;
		  }
		  fprintf(stderr, "gpxpoly: Unknown option %s\n",*argv);
		  fprintf(stderr, "%s\n",USAGE);
		  return 1;
	   }
	}
	
	if((argc==0)&&(iflag==0))return 0; /* Nothing to do */

/* Write boilerplate xml shtuff at the top of the output */

	fprintf(out,"%s\n",HEADER);

/* print the open gpx tag */

	fprintf(out,"<gpx %s>\n",GPX_ATTRIBUTE);
	fprintf(out,"%s\n",METADATA);

/* If the user invoked the -i option to generate an input file
interactively, use that first to generate a track */

	if(iflag){
		in = fopen(tempfile_name,"r");
		if(!in){
			fprintf(stdout,"Cannot open %s\n",tempfile_name);
			return 1;
		}
		mktrk(in,out);
		fclose(in);
	}
	

/* Loop over any input files listed on command line. Each one can
define a track */

   	while(argc){
		argc--;
		in = fopen(*argv,"r");
		if(!in){
			fprintf(stdout,"Cannot open %s\n",*argv);
			argv++;
			continue;
	}
	
/* parse input file for instructions on how to make a polygonal
  tracks, and write the xml to make it */

		mktrk(in,out);
		fclose(in);
		argv++;
}

/* close the main pgx tag */

	fprintf(out,"</gpx>\n");

/* clean up */

	fclose(out);
	return 0;
}
