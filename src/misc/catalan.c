/* catalan.c: Calculate the Catalan numbers
 * 
 *
 * 	By Terry R. McConnell
 */

/* 

Theory:

The Catalan numbers are named after Eugene Charles Catalan (1814-1894), a 
Belgian mathematician. The nth Catalan number C_n has a number of important
combinatatorial interpretations. For example, C_n counts

(a) the number of well-formed algebraic expressions using only n sets of
parentheses. For example, () (()()), but not () (((()). 

(b) the number of simple random walk paths starting from 0 that return to 0
after 2n steps (and possibly before) without ever having become negative in
the meantime.

(c) the number of Ferrers diagrams (rows of dots of non-increasing length)
that will fit under a staircase of length n. (See, e.g., Integer Partitions,
George E. Andrews and Kimmo Eriksson, Cambridge University Press, Cambridge,
2004, Exercise 34.)  

They are sequence number A000108 in the OEIS encyclopedia.

The C_n satisfy an interesting recurrence: C_0 = 1, C_n+1 = C_nC_0 + 
C_n-1C_1 + ... + C_0C_n.  They also have a simple direct formula in terms of
binomial coefficient, C_n = C(2n,n)/(n+1),  but this is surprisingly difficult
to derive from any of the usual interpretations. (Try a proof by induction. It
will probably not go well.) 

The recurrence is most easily derived from interpretation (a) since we
can write any expression with n+1 pairs = (expression with k pairs) expression
with n-k pairs, for some k = 0,1, ... n. Summing on k and the number of
choices for the expressions on the right side gives the result. 

It is easy to generate random parenthesis expressions recursively (see, e.g.,
our lisper.c code): With probability p (say), print "(" and push, or, with
probability 1-p, print ")" and pop. To connect with (b), associate each 
positive step of the walk with a push and each negative step with a pop. Since
it is not possible to pop from level 0, we must restrict to paths that never
become negative. 

To derive the explicit formula, begin by computing the generating function
E_x(s^T) for the random time T until a simple symmetric random walk started 
from x will reach -1 for the first time. Here 0 <= s < 1. Denoting this
function g(x), the strong Markov property implies that g satisfies the
difference equation (1/2)g(x+1) + (1/2)g(x-1) = g(x)/s, x an integer >= 0, 
with boundary condition g(-1) = 1. To solve, assume an exponential form
g(x) = aexp(bx). Subsituting, we find that y = e^b satisfies the quadratic
y^2 + 1 = 2y/s, so y = (1 - sqrt(1 - s^2))/s. (The other root gives y > 1,
which would make g -> infinity as x -> infinity.) The boundary condition
determines the choice of the constant a and yields 

g(x) = [(1 - sqrt(1 - s^2))/s]^(x+1),

so

g(0) = (1 - sqrt(1 - s^2))/s.

On the other hand, by interpretation (b),

g(0) = (1/2) summation C_n (s/2)^n, where the summation is from 0 to infinity,
since each path occurs with probability (1/2)^(n+1). Replacing s by 2s,
we obtain as the generating function for the Catalan numbers 
(1 - sqrt(1 - 4s))/(2s). 

The rest is straightforward. By the binomial expansion,
sqrt(1-4s) = summation C(1/2,j)(-4s)^j, j = 0 to infinity. Thus,

(1 - sqrt(1 - 4s))/(2s) = (-1/2)summation C(1/2,j)(-4)^j s^(j-1) , j = 1 ...

= summation (-1/2)C(1/2,j+1)(-4)^(j+1) s^j, j = 0, ...
  
We need only check that (-1/2)C(1/2,j+1)(-4)^(j+1) = C(2j,j)/(j+1), since the
generating function determines the sequence uniquely. Now
(1/2)(1/2-1)...(1/2-j) (j+1 factors) = (1/2)^(j+1) (2j-1)(2j-3)...(1)(-1)^j
= -(1/2)^(j+1)(-1)^(j+1) (2j)!/(j!2^j), and the result follows at once. 

 */

/* compile: cc -o catalan catalan.c

      Use -D_SHORT_STRINGS if your compiler does not support multiline
          string constants.

      Select the proper choice of N_MAX based on your ULONG_MAX (See below)
	or run catalan -c to see the proper compile line
*/


#include<stdio.h>
#include<stdlib.h>
#include<limits.h>


/* It is easy to check by induction that C_n <= 4^n/(n+1). This bound is
asymptotically exact by Stirling's formula.  Let 2^2k be the largest unsigned
int representable on the machine. Then the largest value of n allowable
must satisfy n <= k + 1/2 log(n+1), so k + 1/2log(k) is a safe value of
n. For example, on a normal 32 bit machine k = 16, so a safe n
would be 18. Uncomment the appropriate choice. If you don't know, make
a guess, and run catalan -c for compilation advice */

/* The safe value is a bit conservative and we can get away with one more
on a 32 bit machine: */
#ifndef N_MAX
#define N_MAX 19   /* ULONG_MAX = 2^32 = 4294967295 */
#endif
#if 0
#define N_MAX 5    /* ULONG_MAX = 2^8 = 64 */
#define N_MAX 9    /* ULONG_MAx = 2^16 = 65536 */
#define N_MAX 34   /* ULONG_MAX = 2^64 */
#define N_MAX 68   /* ULONG_MAX = 2^128 */
#endif     

#define VERSION "1.0"
#define USAGE "catalan [-c -h -v] n"
#ifndef _SHORT_STRINGS
#define HELP "\n\ncatalan [-c -t -h -v ] n\n\n\
Print the nth Catalan number \n\n\
-c: Give compilation advice based on installation limits\n\
-t: Print a table of Catalan numbers\n\
-v: Print version number and exit. \n\
-h: Print this helpful information. \n\n"
#else
#define HELP USAGE
#endif

/* Array to hold accessible values of the Catalan numbers */
static unsigned long C[N_MAX];   

int
main(int argc, char **argv)
{
	int j=0;
	unsigned long n = N_MAX,m,k;
	int do_table = 0;

	/* Process command line */

	*argv++;
	argc--;
	while(argc){
		if((*argv)[0] == '-'){
			switch((*argv)[1]){ 
				case 'v':
				case 'V':
					printf("%s\n",VERSION);
					exit(0);
				case '?':
				case 'h':
				case 'H':
					printf("%s\n",HELP);
					exit(0);
				case 'c':
					m = ULONG_MAX;
					k = 0L;
					while(m){
						m = m>>1;	
						k++;
					} /* k = log(ULONG_MAX) */
					printf("ULONG_MAX = %lu = 2^%lu\n",ULONG_MAX,k);
					n = m = k/2;
					k=0L;
					while(m){
						m = m>>1;
						k++;
					}
					printf("cc -o catalan -DN_MAX=%lu catalan.c\n",n+k/2);
					exit(0);
				case 't':
					do_table = 1;
					break;
				default:
					fprintf(stderr,"catalan: unkown option %s\n",
						*argv);
					exit(1);
			}
			*argv++;
			argc--;
		}
		else break;
	}

	if(argc)
		n = atol(*argv); /* Undocumented "feature": n = N_MAX by
                                    default */

	if(n > N_MAX){
		fprintf(stderr,"catalan: n = %lu is too big for the installation\n",
				n);
		fprintf(stderr,"Upgrade your machine and recompile with bigger limit\n");
		return 1;
	}
	

	C[0] = 1L;
	m = 0L;
	while(m++ < n) /* Compute C[m] from the recursion formula */
		for(k=0L;k<m;k++)
			C[m] += C[k]*C[m-k-1];	

	if(do_table){
		printf("\n\tTable of Catalan Numbers C(n)\n\n");
		printf("n\tC(n)\n");
		printf("-----------------------\n");
		for(m=0L;m<=n;m++)
			printf("%lu\t%lu\n",m,C[m]);
	}
	else printf("%lu\n",C[n]);
	return 0;
}
	 
