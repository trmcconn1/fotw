/* linear.c: Solve the general linear congruence  
 *
 *  By Terry R. McConnell
 *
*/             


/* compile: cc -o linear linear.c

      Use -D_SHORT_STRINGS if your compiler does not support multiline
          string constants.

   Run linear -h for usage information.

*/


#include "global.h"
#include "latex.h"
#include<stdio.h>
#include<stdlib.h>

#define VERSION "1.0"
#define USAGE "linear [ -m -t -V -h -v -- ] a b c n"
#ifndef _SHORT_STRINGS
#define HELP "\nlinear [ -m -t -V -h -v --] a b c n\n\n\
Solve the linear congruence ax + b = c (mod n) (n > 0)\n\n\
-m: Give the minimal solution only\n\
-V: Produce verbose ascii output\n\
-t: Produce verbose latex output\n\
--: Signal end of options so that negative a and or b can be input.\n\
-v: Print version number and exit. \n\
-h: Print this helpful information. \n\n"
#else
#define HELP USAGE
#endif

extern integer my_gcd(integer a, integer b, integer *c, integer *d);
integer minimal_linear(integer a, integer c, integer n);

#ifdef MAIN
int
main(integer argc, char **argv)
{
	integer a,b,c,n,h,s,d,x,y;
	integer m = -1;  /* minimal solution: -1 means no solution */
	integer j=0,k;
	int verbose = 0;
	int minimal = 0;
	int tex = 0;

	/* Process command line */
	while(++j < argc){
		if(argv[j][0] == '-')
			switch(argv[j][1]){ 
				case '-':
					++j;
					break;
				case 'v':
					printf("%s\n",VERSION);
					exit(0);
				case 'V':
					verbose = 1;
					continue;
				case 'm':
					minimal = 1;
					continue;
				case 't':
					verbose = 1;
					tex = 1;
					continue;
				case '?':
				case 'h':
				case 'H':
					printf("%s\n",HELP);
					exit(0);
				default:
					fprintf(stderr,"euclid: unkown option %s\n",
						argv[j]);
					fprintf(stderr,"%s\n",USAGE);
					exit(1);
			}
		break;
	}

	if(j >= argc){
		fprintf(stderr,"linear: usage error.\n");
		fprintf(stderr,"%s\n",USAGE);
		return 1;
	}
	a = atoi(argv[j++]);
	if(j >= argc){
		fprintf(stderr,"linear: usage error.\n");
		fprintf(stderr,"%s\n",USAGE);
		return 1;
	}
	b = atoi(argv[j++]);
	if(j >= argc){
		fprintf(stderr,"linear: usage error.\n");
		fprintf(stderr,"%s\n",USAGE);
		return 1;
	}
	c = atoi(argv[j++]);
	if(j >= argc){
		fprintf(stderr,"linear: usage error.\n");
		fprintf(stderr,"%s\n",USAGE);
		return 1;
	}
	n = atoi(argv[j++]);
	
	if(n==0){
		fprintf(stderr,"linear: modulus must be nonzero.\n");
		return 1;
	}

	if(verbose){
		if(tex == 1)printf(LATEX_HEADER);
		if(minimal == 1){
			if(tex == 1)printf("The minimal nonnegative of $%ldx + %ld \\equiv %ld \\pmod{%ld}$ is ",a,b,c,n);
			else printf("The minimal nonnegative solution of %ldx + %ld = %ld (mod %ld) is ",
				a,b,c,n);
		}
		else
			if(tex == 1)printf("The solution set of $%ldx + %ld \\equiv %ld \\pmod{%ld}$ is\n$$\n \\{ ",a,b,c,n);
			else printf("The solution set of %ldx + %ld = %ld (mod %ld) is\n{ ",a,b,c,n);

	}

	if(n<0) n = -1*n;

	/* normalize a >= 0 */

	if(a < 0) {
			a = -a;
			b = -b;
			c = -c;
	}

	/* Normalize b and c */

	if(b < 0) {
		s = -1;
	 	b = -b;
	}
		
	k = b/n;
	b = b - k*n;
	if( s == -1) b = n - b;

	if(c < 0) {

		s = -1;
	 	c = -b;

	}
		
	k = c/n;
	c = c - k*n;
	if( s == -1) c = n - c;

	/* Find the minimal positive solution */

	if( a == 0 ) {

		if( b == c ) m = c;

	}
	else {

		c = c - b;
		if(c < 0) c = n - c;
		m = minimal_linear(a,c,n);

	}
	
	if(m == -1){
		if(verbose){
			if(tex == 1)printf("\\nullset\n");
			else printf("The empty set.\n");
		}
		return 0;
	}
	printf("%ld",m);
	if(verbose == 1){
		if(minimal==0){

			d = my_gcd(a,n,&x,&y);

			j = 1;
			k = n/d;
			while( j < d){
				printf(", %ld",(m+j*k)%n);
				j++;
			}
			if(tex == 1)printf(" \\}\n$$\n");
			else printf(" }\n");
		}
		else
			printf("\n");
		}
	else {
		if(minimal == 0){

			d = my_gcd(a,n,&x,&y);

			j = 1;
			k = n/d;
			while( j < d){
				printf(" %ld",(m+j*k)%n);
				j++;
			}
		}
		printf("\n");
	}
	if(tex == 1)printf(LATEX_FOOTER);
	return 0;

}
#endif


/* minimal_linear: return the minimal nonnegative solution of the congruence

	ax = c (mod n)

or -1 if there is no solution. We assume 0 < a < n and 0 <= c < n */


integer minimal_linear( integer a, integer c, integer n)
{
	integer x,y,d,m;

	d = my_gcd(a,n,&x,&y);

	if( c % d ) return -1; /* no solution if not d|n */

	m = n/d;
	a = a/d;
	c = c/d;

	/* ax = c (mod n) iff (a/d)x = (c/d) (mod n/d) */

	/* Get the inverse of a/d mod n/d in x. */

	d = my_gcd(a,m,&x,&y); /* don't need d or y here, only x */
	if(x < 0) x = m + x;
	y = x*c;  /* y now holds solution */
	
	return y % m;
}
