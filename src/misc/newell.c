/* newell.c: Studys a question raised in her Usenet report by Shanna Newell,
MAT 397 honors, Fall 1997: Reduce a number to one digit by repeatedly
multiplying all decimal digits. What is the resulting frequency distribution ?
 */

/* C source code by Terry R. McConnell, September 1997 */

/* Compile: cc -o newell newell.c */

#include <stdio.h>
#include <stdlib.h>

#define BASE 10
#define USAGE "Usage: newell <positive integer>\n"

int scrunch(int);
void fatal(char *);

int
main(int argc, char **argv)
{

	int i,n;
	static int Freq[BASE];

	/* Sanity checks */
	if(argc != 2) fatal(USAGE);
	n = atoi(argv[1]);
	if(n < 0) fatal("Duh! Integer must be positive.\n");

        /* "Scrunch" each integer from 0 to n and count how many times
            each digit appears. */

	for(i=0;i<=n;i++) Freq[scrunch(i)]++;

	/* Print report of the results */
	printf("%-10s%10s ( n = %d )\n\n","Digit","Frequency", n);
	for(i=0;i<BASE;i++)
		printf("%-10d%10d\n",i,Freq[i]);

	return 0;
}

/* scrunch: perform the "multiplying out digits" operation recursively.
     For example: 3442 --> 96 --> 54 --> 20 --> 0:  0 = scrunch(3442) */

/* Assumes argument is positive ! */

int
scrunch( int ANumber)
{

	int intermediate = 1;  /* stack cumulative product */

	if(ANumber <= BASE - 1) return ANumber;

	while(ANumber > 0){
		intermediate *= ANumber % BASE; /* Multiply by next digit */
		ANumber /= BASE;
	}
	return scrunch(intermediate);
}


/* Trivial error routine */

void
fatal(char *msg)
{
	fprintf(stderr, "newell: %s", msg);
	exit(1);
}	
