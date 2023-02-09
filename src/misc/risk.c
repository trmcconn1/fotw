/* Risk.c: simulation of the dice rolls in the game of risk */

/* 
Written by T.McConnell in response to a question by Amy Storfer-Isser.

	In the game of "Risk" there are two players, the defender (D) and
	the attacker (A.) On each play, A rolls 3 fair dice and D rolls
	2 fair dice. A's and D's numbers are separately arranged in
        decreasing order and then matched against each other in pairs. The
   	higher die wins in each pair, but ties go to D. Thus, there are
	3 possible outcomes:

	1) D loses twice (e.g, A: 6,2,1; D: 5,1)
	2) D wins twice  (A: 4,2,1; D: 5,2)
	3) Each wins once (A: 6,2,1; D:5,3)

	In this program, we simulate many rolls in order to estimate
	the probabilities of 1,2, and 3.

	Note: computations using Maple give the following exact values:

	P(D wins twice) = 2275/7776, P(D loses twice) = 1445/3888,
	P(split) = 2611/7776. This program, run with 10,000,000 trials
	confirms the first 3 decimal points of these values.

*/

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
#define USAGE "risk [ -s number -t number -h -v]"
#ifndef _SHORT_STRINGS
#define HELP "risk [ -s number -t number -h -v ]\n\n\
Print information on simulations of risk die tosses.\n\n\
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


/* Simulate one roll of a fair die, and return a value in {1,2,3,4,5,6} */
int roll(void){

	double U;   /* U(0,1) random variable */
	int rval;

	U = ((double)RANDOM())/((double)_MAX_RAND);
	rval = (int)(6.0*U);
	return rval + 1;
} 



int
main(int argc, char **argv)
{
	int trials = TRIALS;
	int i=0,j=0;
	int d_2win_count = 0;
	int d_2loss_count = 0;
	int a1,a2,a3,d1,d2,temp;
                                      
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
					fprintf(stderr,"parrondo: unkown option %s\n",
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
	i=0;
	printf("Simulating %d trials ...\n",trials);
	while(i<trials){   /* Loop over trials */

		/* reseed */
		seed = RANDOM();
		SRANDOM((int)seed);

		/* generate die rolls and order in decreasing order */

		a1 = roll();
		a2 = roll();
		if(a1 < a2){ temp = a1; a1 = a2; a2 = temp; } 
		a3 = roll();
		if(a1 < a3){ temp = a1; a1 = a3; a3 = temp; temp = a2;
				a2 = a3; a3 = temp; }
			else if ( a2 < a3 ){ temp = a2; a2 = a3; a3 = temp; } 

		d1 = roll();
		d2 = roll();
		if(d1 < d2){ temp = d1; d1 = d2; d2 = temp; } 

		/* figure out who wins what, and increment tallies */

		if( a1 > d1){
			if(a2 > d2)d_2loss_count++;
		}
		else
			if(a2 <= d2)d_2win_count++;
		
		i++;
	}

	/* Print stuff out */

	printf("%d D 2 wins, %d D 2 losses, %d splits\n",d_2win_count,
			d_2loss_count, temp = i-(d_2win_count+d_2loss_count));
	printf("Proportions: D 2 wins: %g%%, D 2 losses: %g%%, splits: %g%%\n",
		100.0*((double)d_2win_count/(double)i),
		100.0*((double)d_2loss_count/(double)i),
		100.0*((double)temp/(double)i)
	);
	return 0;
}
	


