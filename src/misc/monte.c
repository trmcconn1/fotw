/* monte.c: Illustration of Monte Carlo integration */


/* compile: cc -o risk risk.c 

      Use -D_NO_RANDOM if your library doesn't have random/srandom. Most do,
       	but the only truly portable RNG is rand/srand. Unfortunately it has
        very poor performance, so you should use random if possible.

      Use -D_MAX_RAND=  to set the size of the maximum value returned by
         random(). The portable RNG rand() always returns a maximum of 
         RAND_MAX (defined in stdlib.h), but some implementations of random
         do not use this value. Read the man page for random to be sure. A
	 common value is 2^31-1 = 2147483647. In so, and this is not the
         value of RAND_MAX on your system, you would compile with
         -D_MAX_RAND=214748367.

      Use -D_SHORT_STRINGS if your compiler does not support multiline
          string constants.
*/


#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#define VERSION "1.0"
#define USAGE "monte [ -s number -t number -h -v]"
#ifndef _SHORT_STRINGS
#define HELP "monte [ -s number -t number -h -v ]\n\n\
Print information on Monte Carlo integration of x^2\n\n\
-s: Use next argument as RNG seed. (otherwise use system time as seed.)\n\
-t: Use next argument as number of trials. Default 10000.\n\
-v: Print version number and exit. \n\
-h: Print this helpful information. \n\\n"
#else
#define HELP USAGE
#endif


#ifdef _NO_RANDOM
#define RANDOM rand
#define SRANDOM srand
#else
#define RANDOM random
#define SRANDOM srandom
#endif

/* Default values */
#define TRIALS 10000
#define INITIAL_SEED 3445
#ifndef _MAX_RAND
#define _MAX_RAND RAND_MAX
#endif

int
main(int argc, char **argv)
{
	int trials = TRIALS;
	int i=0,j=0;
	double x,sum;
                                      
	long seed=0;

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
				case 't':
				case 'T':
					if(j+1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					trials = atoi(argv[j+1]);
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
					fprintf(stderr,"monte: unkown option %s\n",
						argv[j]);
					exit(1);
			}
		else {
			fprintf(stderr,"%s\n",USAGE);
			exit(1);
		}
	}
 
	/* If no seed is supplied, then use current system time */
	
	if(!seed)
		if((seed = (long)time(NULL)) == -1){
			seed = INITIAL_SEED; /* if all else fails */
			fprintf(stderr, "Using seed = %d\n",INITIAL_SEED);
		}
		
	SRANDOM((int)seed);
	for(j=0;j<10;j++){
		/* reseed */
		seed = RANDOM();
		SRANDOM((int)seed);
		i=0;
		sum = 0.0;
		while(i<trials){   /* Loop over trials */


			x = (double)RANDOM()/(double)(RAND_MAX);

			sum += x*x;

			i++;
		}
		printf("%6.4f ",sum/(double)trials);
	}
	printf("\n");

	return 0;
}
	


