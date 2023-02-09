/* egypt.c: Decompose a fraction as a sum of fractions with unit numerators,
* i.e., egyptian fractions. See comments below for theory.
*
* By Terry R. McConnell
*
*/

/* compile: cc -o egypt egypt.c euclid.o
	Use -D_SHORT_STRINGS if your compiler does not support multiline string constants.

	Use -DMAIN to compile as a stand-alone program
	Run egypt -h for usage information.

*/

#include "global.h"
#include "decl.h"
#include "latex.h"
#include<stdio.h>
#include<stdlib.h>
#define VERSION "1.0"
#define USAGE "egypt [ -t -V -h -v -- ] num den"
#ifndef _SHORT_STRINGS
#define HELP " egypt [ -t -V -h -v --] num den\n\n\
Decompose num/den as a sum of eqyptian fractions (numerators = 1.)\n\n\
-V: Produce verbose ascii output \n\
-t: Produce verbose latex output \n\
--: Signal end of options so that negative num can be input. \n\
-v: Print version number and exit. \n\
-h: Print this helpful information. \n\n"
#else
#define HELP USAGE
#endif


#ifdef MAIN
int
main(integer argc, char **argv)
{
	integer j=0,k;
	int nargs = 2;
	integer x,y; /* These are just memory for gcd call */
	integer m,n;
	integer r,q;
	integer sign = 1;
	integer temp;
	integer int_part,i;
	int verbose = 0;
	int tex = 0;

/* Process command line */

while (++j < argc){
	if (argv[j][0] == '-')
		switch (argv[j][1]){
			case '-':
				++j;
				break;
			case 'v':
				printf("%s ",VERSION);
				exit (0);
			case 'V':
				verbose = 1;
				continue;
			case 't':
				verbose = 1;
				tex = 1;
				continue;
			case '?':
			case 'h':
			case 'H':
				printf ("%s ",HELP);
				exit (0);
			default:
				fprintf (stderr,"egypt: unknown %s\n", argv[j]);
				fprintf(stderr,"%s ",USAGE);
				exit (1);
		} /* switch */
	break;
} /* arg processing */

if(j + nargs != argc){
	fprintf(stderr,"egypt: usage error. ");
	fprintf(stderr,"%s ",USAGE);
	return 1;
}

m = atol(argv[j++]);
n = atol(argv[j++]);

/* Sanity checks and normalization */

if (n == 0){
	fprintf(stderr,"egypt: denominator is zero. Can't have that.\n");
	exit (1);
}
if (n < 0){
n = -n;
sign = -1;
}
if (m < 0){
	m = -m;
	sign *= -1;
}

/* m and n are now both positive */

if (tex){
	printf (LATEX_HEADER);
	printf("$");
	printf ("\\dfrac(%ld}{%ld} = ",sign*m,n);
}
else if(verbose) printf("%ld/%ld = ",sign*m,n);

/* Chop off integer part */

i = (int_part = m/n);
m = m-int_part*n;
if (m == 0){
	printf("0 ");
	exit (0);
}

if (i) printf("%ld",sign*int_part);

/* The algorithm is based on the following inductive  proof that x = m/n in
 lowest terms can be decomposed as a sum of egyptian fractions. (Taken from 
Niven and  Zuckerman.) We assume 0 < x < 1. The induction is on the numerator.
The trick is to divide m+n_by m: m+n = mq + r, so (m+n)/nq = m/n + r/ng, 
i.e., m/nq + 1/q = m/n + r/nq, so m/n = (m-r)/nq + 1/q. By the conditions on r
from the Euclidean algorithm, 0 <= r < m. The case r = 0 cannot occur since m
does not  divide n, so we may apply the inductive hypothesis to the
first term.

This algorithm, sometimes called called Sylvester's method,
can be described in a different, more intuitive form: Take
as the denominator, q, of the first fraction in thedecompostion the smallest
possible that satisfies 1/q <= x < 1/ (q-1).
Then repeat with x replaced by x - 1/q. This is a natural "greedy" algorithm,
but it is not obvious in this form that it terminates. Here is the proof that
the two forms are equivalent: We have from 1/q <= m/n < 1/ (q-1)
that n <= qm < m + n. Adding m to both sides of the left
inequality shows that (q+1)m <m +nfails to hold, hence g is
identified as the quotient when m+n is divided by m.* Also note that 
m/n - 1/q < 1/ (q-1) - 1/q = 1/q(q-1) < 1/q
 so the successive denominators strictly increase, showing that
 there are no repeats in the decomposition.

*/

while (m > 0) {  /* main loop */

/* Reduce to lowest terms */

temp = my_gcd (m,n,&x, &y);


if (temp > 1){
	m = m/temp;
	n = n/temp;
}

/* divide m + n by m */

if(m == 1) { /* Special case */
	q = n;
	r = m;
}
else {
	q =(m + n)/m;
	r = (m + n) - q* m;
}

if(i){ /* not first term */
	if (sign < 0){
		if (tex)printf (" - \\dfrac1{%ld}",q);
		else
			printf (" - 1/%ld",q);
	}
	else {
		if (tex)printf (" + \\dfrac1{%ld}",q);
		else
			printf(" + 1/%ld",q);
	}
}
else { /* handle first term differently */
	if (sign < 0) {
		if(tex)printf("-\\dfrac1{%ld}",q);
	else
		printf("-1/%ld",q);
	}
	else {
		if(tex)printf("\\dfrac1{%ld}",q);
		else
			printf("1/%ld",q);
	}
	i = 1; /* toggle off first term condition */
}
m = m - r;
n = n*q;
} /* while m > 0 */

/* clean up */
if (tex)printf("$");
printf("\n");
if (tex) printf(LATEX_FOOTER);
exit (0);
}  /* main */
#endif

