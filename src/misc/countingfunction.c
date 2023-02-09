/*
*  countingfunction.c main pgm version
*  locate position in an increasing sequence
*/

#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define TIME 10.5
#define NSTEPS 10
#define ARRIVALTIMES {1.1,4,3,7.2,8,10,20.2,25,53,100}

int monotone(float[NSTEPS]);

int
main () {
	int n = NSTEPS;
	float arts[NSTEPS] = ARRIVALTIMES;
	int ctr = 0;
	float t = TIME;

	/*  idiotproofing tests */
	if (t < 0.0) { 
		fprintf(stderr, "time out of range\n");
                }
	if ( monotone(arts)== FALSE )
		fprintf(stderr, "warning: time list not increasing\n");

	/*  counting loop */

	while ( (arts[ctr] <= t) && (ctr < NSTEPS) )
		ctr++;

	printf("%i",ctr);

	return EXIT_SUCCESS;
}

int
monotone(float list[NSTEPS]) /* checks to see if list is increasing */
{
	int count = 0;
	int test = TRUE;

	while (count < NSTEPS-1) { 
		if (!(list[count] < list[count + 1])){
			test = FALSE;
			break;}
		count++;
		}
	return (test);
}
			
