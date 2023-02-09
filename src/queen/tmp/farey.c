/* farey.c: generate farey sequence
 *
 *  By Terry R. McConnell
 *
 *                              Theory
 *
 *  The Farey sequence of order n is the complete list of all fractions in
 *  lowest terms with denominator at most n and which lie in the closed
 *  unit interval. The fractions are listed in increasing order.
 *
 *  The number of farey fractions is 1 + phi(1) + phi(2) + ... + phi(n).
 *  Is there is a simple formula for this? (The same sum over divisors of n
 *  equals n -- use that both n and the some of phi(d) over d | n are
 *  multiplicative functions that agree on prime powers.)
 *
 *  The key property of the Farey sequence is that consecutive fractions, say
 *  h/k < x/y, satisfy kx - hy = 1. Hardy and Wright give 4 different proofs
 *  of this fact. One of the proofs is constructive in that it constructs the
 *  next farey fraction from the preceeding one. Our implementation uses
 *  this same construction. Given h/k, find a solution of (x,y) of 
 *  kx - hy = 1 with y as large as possible subject to y <= n. One can then
 *  show that x/y is the next farey fraction -- see Hardy and Wright for
 *  details. 
 *
 *  This file contains a standalone program for construction and display of
 *  a Farey sequence, plus an exportable function called farey that returns
 *  the number of Farey fractions of order n and stores the numerators and
 *  denominators in separate integer arrays. (See below for the calling
 *  sequence.)
*/             

/* compile: cc -o farey -DMAIN farey.c euclid.o totient.o

      Use -D_SHORT_STRINGS if your compiler does not support multiline
          string constants.

   Run farey -h for usage information.
 
   Leave off -DMAIN when compiling only to use the exportable function.

*/


#include "global.h"
#include<stdio.h>
#include<stdlib.h>

#define VERSION "1.0"
#define USAGE "farey [ -c <m> -h -v -- ] n"
#ifndef _SHORT_STRINGS
#define HELP "\nfarey [-c <m> -h -v --] n\n\n\
Generate the farey sequence of order n \n\n\
--: Signal end of options.\n\
-c: Set size of display in cols to m (default 80)\n\
-v: Print version number and exit. \n\
-h: Print this helpful information. \n\n"
#else
#define HELP USAGE
#endif


extern integer my_gcd(integer a, integer b, integer *c, integer *d);
extern integer phi(integer);


/* exportable from this file */

integer farey(integer n, integer (**)[], integer (**)[]);

#define MAX_DISPLAY_COLS 1024

#ifdef MAIN
int
main(integer argc, char **argv)
{
	integer n,i,l,j=0,k,m;
	integer (*nums)[], (*dens)[];
	integer display_cols = 80;
	char buf[MAX_DISPLAY_COLS];

	/* Process command line */
	while(++j < argc){
		if(argv[j][0] == '-')
			switch(argv[j][1]){ 
				case '-':
					++j;
					break;
				case 'c':
					++j;
					display_cols = atoi(argv[j]);
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
					fprintf(stderr,"farey: unkown option %s\n",
						argv[j]);
					fprintf(stderr,"%s\n",USAGE);
					exit(1);
			}
		break;
	}

	if(j >= argc){
		fprintf(stderr,"farey: usage error.\n");
		fprintf(stderr,"%s\n",USAGE);
		return 1;
	}
	n = atoi(argv[j++]);
	
	if(n <=0 ) {
		fprintf(stderr,"farey: n must be a natural number.\n");
		return 1;
	}

	l = farey(n,&nums,&dens);
	printf("Farey sequence of order %ld (length = %ld)\n",n,l);
	i=0;
	while(i<l){ /* print in groups compatible with display */
		k=0;
		m = 0;
		while((i+m<l) && (k<display_cols)){
			sprintf(buf,"%ld/%ld ",(*nums)[i+m],(*dens)[i+m]);
			k += strlen(buf);
			m++;
		} 
		if(k < display_cols)m = m+1;
		for(j=1;j<m;j++){
			printf("%ld/%ld ",(*nums)[i],(*dens)[i]);
			i++;
		}
		printf("\n");
	}
	return 0;	
}		

#endif

/* Returns the length of the farey sequence of order n, = 1 + phi(1) + ...
   + phi(n). Stores through the pointer arguments pointers to arrays of
   numerators and denominators, which reside in memory allocated by
   this routine. 

   This routine does no sanity checking on its arguments. It is up to
   the caller to do that.

*/

integer farey(integer n, integer (**num)[], integer (**den)[])
{

	integer i,j,m=1,l,d,x,y,h,k;

	for(j=1;j<=n;j++) m += phi(j);

	*num = (integer (*)[]) malloc(m*sizeof(integer));
	*den = (integer (*)[]) malloc(m*sizeof(integer));

	(**num)[0]=0;
	(**den)[0]=1;

	if(m>1){
	
	h = 1; k = n;

	for(j=1;j<m;j++){


		(**num)[j] = h;
		(**den)[j] = k;

		/* generate the next farey fraction. Call my_gcd
		   to find x and y such that k*x - h*y = 1 */

		d = my_gcd(h,k,&y,&x);

		y = -y;

		l = 1;
		while( y + l*k <= n ){ 
			l++;
			x += h;
		}
		k = y + (l-1)*k;
		h = x;
		
	}
	}
	return m;
}
