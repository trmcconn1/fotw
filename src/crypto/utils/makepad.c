/* makepad.c: create a text file suitable to be used as the one-time pad
   in the onetime cypher.

   Author: Terry R. McConnell

   Prints random characters to stdout, 64 to a line. Resembles makebook in
   operational details, but assumes uniform distribution for the desired
   frequency distribution.

   Program also reports the Chi-square statistic for the actual vs uniform
   character distribution in the file created.
 
*/

#include <stdio.h>
#include <stdlib.h>


#define VERSION "1.0"
#define USAGE "makepad [ -s number -f number  -h -v] "
#ifndef _SHORT_STRINGS
#define HELP "makepad [ -s number -f number -h -v ]\n\n\
Create a file of random text suitable for use as a one time pad.\n\n\
-s: Use next argument as RNG seed. (otherwise use system time as seed.)\n\
-f: Use next argument as output file size (KB). Default = 20KB.\n\
-v: Print version number and exit. \n\
-h: Print this helpful information. \n\n"
#else
#define HELP USAGE
#endif


#ifdef _NO_RANDOM
#define RANDOM rand
#define SRANDOM srand
#else
#ifndef RANDOM
#define RANDOM random
#endif
#ifndef SRANDOM
#define SRANDOM srandom
#endif
#endif


#define MAX_FILE_SIZE 20   /* KB */

#define INITIAL_SEED 3445  /* Had to be something */
#ifndef _MAX_RAND
#define _MAX_RAND RAND_MAX
#endif


/* return a number chosen at random from 0,1...,k-1 */

int random_on(int k){

        double U;   /* U(0,1) random variable */
        int rval;

        U = ((double)RANDOM())/((double)_MAX_RAND);
        rval = (int)((double)k*U);
        return rval;
}


int main(int argc, char **argv)
{

	int i,j=0,k,l,n,m;
	long seed = 0;
	int file_kb = MAX_FILE_SIZE;
	int freq_total = 95;  /* uniform distribution on 95 printables */
	double O,E,chi_square;
	char buffer[1024];  /* Keep size = 1 KB */
	int observed_freq[256];

	/* Process command line */
	while(++j < argc){
		if(argv[j][0] == '-')
			switch(argv[j][1]){ 
				case 's':
				case 'S':
					if(j+1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					seed = atol(argv[j+1]);
					j++;
					continue;
				case 'f':
				case 'F':
					if(j+1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					file_kb = atoi(argv[j+1]);
					j++;
					continue;	
				case 'v':
				case 'V':
					printf("%s\n",VERSION);
					exit(0);
				case '?':
				case 'h':
				case 'H':
					printf("%s\n",HELP);
					exit(0);
				default:
					fprintf(stderr,"makepad: unkown option %s\n",
						argv[j]);
					exit(1);
			}
	}

	if(j > argc){
		fprintf(stderr,"%s\n",USAGE);
		exit(1);
	}
 
	/* If no seed is supplied, then use current system time */
	
	if(!seed)
		if((seed = (long)time(NULL)) == -1){
			seed = INITIAL_SEED; /* if all else fails */
			fprintf(stderr, "Using seed = %d\n",INITIAL_SEED);
		}
		
	SRANDOM((unsigned)seed);

	/* Sum the frequencies */

	/* initialize observed frequencies */

	for(j=0;j<256;j++)observed_freq[j] = 0;
	

	i = 0;
	while(i++ < file_kb){

		/* Fill buffer with random characters */

		for(n=0;n<1024;n){
			m = buffer[n++] = ' ' + random_on(freq_total);
			(observed_freq[m])++;
		}

		/* write buffer out */

		for(n=0;n<1024/64;n++){
			for(k=0;k<64;k++)
				putchar(buffer[k+64*n]);
			putchar('\n');
		}
	}

	fprintf(stderr,"Pad successfully created.\n");

	/* Report distance between actual and desired frequencies */
	
	chi_square = 0.0;
	for(j=32;j<95+32;j++){
		E = ((double)file_kb*1024.0)/95.0;
		O = (double)observed_freq[j];
		chi_square += (O-E)*(O-E)/E;
	} 
	fprintf(stderr,"Chi-Squared Statistic (H0=uniform,df=94) %g\n",chi_square);	

	/* Print some helpful advice */
	fprintf(stderr,"\n\nSet pad file permissions to 400.\n");
	fprintf(stderr,"Keep in a directory not readable or searchable by others.\n");

	return 0;
}
