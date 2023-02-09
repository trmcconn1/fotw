/* Print random unsigned ints, one to a line, to stdout or to a file */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mtwist.h"

#define MYBUFSIZ 1024
#define VERSION "1.0"
#define USAGE "\nrdata [-hvVdx] [-s seed] [-f name] n: create random data set of size n\n"
#define USAGE1 "-h: print this helpful information and exit\n"
#define USAGE2 "-v: print version number and exit\n"
#define USAGE8 "-V: more verbose output\n"
#define USAGE3 "-d: turn on debug mode\n"
#define USAGE4 "-s: provide seed for the RNG\n"
#define USAGE5 "-x: run experimental code and exit (may do nothing)\n"
#define USAGE6 "-f: output to file name rather than to stdout \n"
#define HELP USAGE USAGE1 USAGE2 USAGE8 USAGE3 USAGE4 USAGE5 USAGE6 


static char buf[MYBUFSIZ];

int main(int argc, char **argv)
{
	int n,j;
	unsigned seed = 0xABCDEFAA;
	FILE *output = stdout;
	int file_output = 0;
	int debug = 0;
	int verbose = 0;

	/* Process command line options. argc will always count number of args
            that remain. */
	
	argv++; argc--; /* skip program name */
    while((argc > 0) && (*argv)[0] == '-'){
		  if(strcmp(*argv,"-h")==0){
			printf("%s\n", HELP);
			exit(0);
		  }
		  if(strcmp(*argv,"-v")==0){
			printf("%s\n",VERSION);
			return 0;
		  }
		  if(strcmp(*argv,"-d")==0){
			  debug = 1;
			  verbose = 1;
			  ++argv;
			  argc--;
			  continue;
		  }  
		  if(strcmp(*argv,"-V")==0){
			  verbose = 1;
			  ++argv;
			  argc--;
			  continue;
		  }
		  if(strcmp(*argv,"-s")==0){
			  ++argv;
			  argc--;
			  seed = atoi(*argv++);
			  argc--;
			  continue;
		  }
		  if(strcmp(*argv,"-f")==0){
			  ++argv;
			  argc--;
			  strcpy(buf,*argv++);
			  argc--;
			  output = fopen(buf,"w");
			  if(!output){
				fprintf(stderr,"rdata: unable to open %s for write\n",buf);
				return 1;
			  }
			  file_output = 1;
			  continue;
		  }
		  if(strcmp(*argv,"-x")==0){/* Put any experimental stuff here */
			  printf("This is a test, this is ONLY a test!\n");
			  
			  return 0;
		  }
		  fprintf(stderr, "rdata: Unknown option %s\n", *argv);
		  fprintf(stderr, "%s\n",USAGE);
		  return 1;
	   }
	

	/* There should still be something (i.e. n) left on the command line */

	if(argc == 0){
		fprintf(stderr,"rdata: usage %s\n", USAGE);
		return 1;
	}

	/* Assume what's left on the command line is a number */
	
	n = atoi(*argv);

	if(n <= 0 ){ /* sanity check */
		fprintf(stderr,"rdata: %d is impossible\n",n);
		return 1;
	}


	/* seed the rng */
	mt_seed32new(seed);

	for(j=0;j<n;j++)
		fprintf(output,"%u\n",mt_lrand());
	if(file_output)fclose(output);
	return 0;

}
