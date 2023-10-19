/* I think this is an obsolete earlier verstion of the program implemented
in tree_fill.c. No harm in keeping it.
*/

/* Head/tail terminolgy motivated by basic biology:  Digits come in at the 
head and out at the tail. */ 

#include <stdio.h>
#include <stdlib.h>

#define MAX 256   /* Maximum number of terms to generate */
#define MAXMARKS 1048576

static unsigned MARKS[MAXMARKS];
static unsigned char terms[MAX];
static unsigned mask = 1;

/* The binary digits of x reference a unique node on the binary tree. To
use these as an address in MARKS array, we need to prefix on the left with
a '1', else leading zeros won't count */

#define MARK(x) (MARKS[mask+1+(x)]++)
#define MARKED(x) ((MARKS[mask+1+(x)])!=0)

/* Library of tiebreaker macros */
/* Comment out ones you don't want, write new ones as needed. */
#if 0
#define TIEBREAKER(N) (terms[(N)])     /* "Autoregressive" */
#define TIEBREAKER(N) ((N)%2 ? 0 : 1)  /* Alternating ones and zeros */ 
#endif
#define TIEBREAKER(N) (0)              /* Always use zero */

/* Toggle least significant digit */
#define TOGGLE(N) ((N) = (N)&1 ? ((N)-1):((N)+1))


/* Print the sequence of binary digits in x. msk gives number of digits to 
print */

void 
binprint(unsigned int x, unsigned msk)
{
	while(msk){
		if(x%2)putchar('1');
		else putchar('0');	
		x = x/2;
		msk = msk/2;
	}
}

int 
main(int argc, char **argv){

	unsigned x=0,y,digits,start=0;
	int n,m=0,k; /* m counts tiebreakers used */
	int in_cycle, cycle_no;

	/* get the number of digits from command line */

	digits = atoi(argv[1]);

	/* heading */
	printf("Cycles for level %d with lower levels filled.\n All possible starting strings.\n\n",
		digits);
	k = 1;
	n = digits;
	while(n--)
		k = 2*k;

	/* k = 2^d, where d is number of digits */
	/* fill in all nodes on next lower layer on tree than start */
	x = 0;
	while(x < k)
		MARKS[x++] = 1;

	while(start < k){

		x = k;
		while(x < MAXMARKS)
			MARKS[x++] = 0;

		mask = k - 1;
		x = start++;

		printf("Start = %d:\n\n",x);   
		binprint(x,mask); printf("\t");
		in_cycle = 1; /* start first cycle */
		cycle_no = 0;

		n = 0; /* counts marked at level n */
		while(n < k){

			MARK(x);
			if(in_cycle)n++;

			/* Find the longest unmarked tail */
			y = ((x << 1) & mask)>>1; /* take mask-sized tail */
			while(MARKS[(mask+1)/2 + y] == 0) {
				mask = (mask+1)/2 - 1; /* decrease the mask */
				y = ((y << 1) & mask)>>1; /* take mask-sized tail */
			} 

			y = 2*y + TIEBREAKER(m);

			/* did we drop back to the bottom? */

			if((!in_cycle) && (mask == (k-1))){
				in_cycle = 1;
				binprint(y,mask); printf("\t");
			}

			if(MARKED(y)){
				if(MARKED(TOGGLE(y))){
					if(mask == (k-1)){
						/* turn off cycle flag, etc */
						in_cycle = 0;
						binprint(x,mask);printf("\n"); 
						cycle_no++;
					}
					x = x*2 + TIEBREAKER(m);
					mask = (2*mask)+1; /* increase the mask */
					terms[n] = TIEBREAKER(m);
					m++;
					continue;
				}
				else terms[n] = 1 - TIEBREAKER(m);
			}
			else {
				terms[n] = TIEBREAKER(m);
				m++;
			}
			x = y;
		}
		printf("\n%d cycles with start (%d) ",cycle_no,start-1);
		binprint(start-1,k-1); printf("\n\n");
	}
}
