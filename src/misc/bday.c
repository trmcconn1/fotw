/* bday.c: simulation of birthday problem involving triple matches */

/* 

Written by T.McConnell in response to a student question in MAT 521

	The well-known birthday problem asks for the probability that
        some pair of people in a group of size k will have the same birth-
        day. We assume there are 365 days in a year and that each person's
        birthday is equally likely to be any of them, independent of the
        others. As soon as k >= 23 the odds are better than 50:50 that
        some pair will share a birthday. Most people find this small group
        size surprising.

	How large a group is needed before the odds are 50:50 that some
        set of 3 people will share a birthday?  It turns out that a group
        of size >= 88 works. (See my notes and maple notebook for details
        of the exact calculation.)

	This program simulates assigning birthdays at random to groups of
        size k. It reports the approximate probability that NO set of
	3 or more people in a group of the given size will share a birthday.
        (Thus, it is the complementary probability to the one above.)

	You give it k on the command line, and optionally other things. 
        

*/

/* compile: cc -o bday bday.c 

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
#define USAGE "bday [ -s number -t number -h -v] group_size"
#ifndef _SHORT_STRINGS
#define HELP "bday [ -s number -t number -h -v ] group_size\n\n\
Print information on simulations of triple birthday problem.\n\n\
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


/* return a number chosen at random from 0,1...,k-1 */

int random_on(int k){

	double U;   /* U(0,1) random variable */
	int rval;

	U = ((double)RANDOM())/((double)_MAX_RAND);
	rval = (int)((double)k*U);
	return rval;
} 

#define NN 365 /* birthdays */
#define TRIPLE_SEEN 1
#define TRIPLE_NOT_SEEN 0

int do_trial(int k){

	int i,j;
	int marks[NN+1]; /* mark number of times each bday seen */

	/* initialize marks to zero */
	for(i=0;i<NN;i++)marks[i] = 0;

	/* Choose k "birthdays," incrementing marks as we go */

	i = 0;
	while(i++ < k){
		j = random_on(NN);
		 marks[j]++;
	}

	/* scan for triples, and return accordingly */

	i=0;
	while(i < NN)
		if(marks[i++] >= 3) return TRIPLE_SEEN;
	return TRIPLE_NOT_SEEN;	
}

int
main(int argc, char **argv)
{
	int trials = TRIALS;
	int i=0,j=0,k;
        int freq=0;                               
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
					fprintf(stderr,"bday: unkown option %s\n",
						argv[j]);
					exit(1);
			}
	}

	if(j > argc){
		fprintf(stderr,"%s\n",USAGE);
		exit(1);
	}

	k = atoi(argv[j-1]);  /* The group size */
 
	/* If no seed is supplied, then use current system time */
	
	if(!seed)
		if((seed = (long)time(NULL)) == -1){
			seed = INITIAL_SEED; /* if all else fails */
			fprintf(stderr, "Using seed = %d\n",INITIAL_SEED);
		}
		
	SRANDOM((unsigned)seed);
	i=0;
	printf("Simulating %d trials ...\n",trials);
	while(i++ < trials)   /* Loop over trials */
		if(do_trial(k)==TRIPLE_SEEN)freq++;
		

	/* Print stuff out */

	printf("k = %d prob = %g\n", k, 1.0-(double)freq/((double)trials));
	return 0;

}
