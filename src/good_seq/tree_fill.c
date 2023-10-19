/* tree_fill.c: Generate filled nodes of a tree filling sequence */
/* See T.R. McConnell, Laws of Large Numbers for Some Non-repetitive
   Sequences, unpublished manuscript  section 2 for background on
   tree-filling sequences. 
*/

/* By Terry R. McConnell */

/* This program does essentially the same thing as mseq/misc/filling.c,
   but I think this one has a better approach. Important difference: 
filling.c creates the shortest successor of a filled tail of the current
filled string, while this program creates the shortest successor of ANY tail
of the current filled string. The successive filled strings are minimal
tail sequences in both cases, but of different binary sequences in general. 

The following is taken
   from filling.c:

   Start with a binary tree with all nodes empty, then proceed to fill
   exactly one node on each turn. Call the node just filled the current
   node, and the tail of that node the current tail. Here tail(abcd) = 
   bcd. The rules are the following:

	1) If both children of the current tail are filled, choose a child
           of the current node according to the tie-breaking function, and
           fill it.

	2) If exactly one child of the current tail is filled, fill the
           other child.

	3) If both children of the current tail are unfilled, choose one
           according to the tie-breaking function, and fill it.

    Note that, in 1 it is a child of the current node that is filled, while
    in 2 and 3 it is a child of the current tail.

    In order to prevent "holes" from developing in the filled set, we
    adopt a special rule if the current tail itself is unfilled: keep
    taking tails of the currrent tail until a filled node is obtained. Then
    apply the above rules with that node in place of current tail.

    IMPORTANT: there is a small collection of built-in tiebreaker rules
    that can be selected from the command line. The section below were
    the functions implementing the rules and related data structures are
    defined is marked clearly. This section will need to be edited if new
    tie breaker rules are needed. This is done as follows:

	1) Increase the value of N_TIEBREAKER_RULES by 1
	2) Add a string to the array tb_desc that describes the action of
		each rule. It will be displayed at the command line with
   		the -f -1 option and should be informative enough to allow
		the user to select an appropriate rule.
	3) For a rule named foo, implement a function int foo(int n) that
           returns the value of the nth tiebreaker (values returned must
           be 0 or 1.

	4) Add a pointer to the function foo in mytbfpointerarray

	5) Recompile and reinstall

*/ 

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

extern unsigned char is_square(unsigned long);

#define MAX_N 4096   /* Maximum number of terms to generate */

#define MAX_NODE_LENGTH 20      /*  == L */
#define MAX_TREE 2097152       /* should be 2^L X 2 */
static unsigned MARKS[MAX_TREE];
static int terms[MAX_N];
static unsigned mask;

/* The binary digits of x reference a unique node on the binary tree. To
use these as an address in MARKS array, we need to prefix on the left with
a '1', else leading zeros won't count */

#define PROGNAME "tf"
#define VERSION "1.3"
#define USAGE "usage: tf [-s <seed> -f <tb indx>  -d -h -q -v] n"

#define HELP "\nPrint n terms and filled nodes of tree filling sequence\n\
usage: tf [-s <seed> -f <tb indx> -q  -h -v] n \n\
-f: next arg is -1 to list tie breaker functions, or 0,1,2,... to select\n\
-s: seed random number generator\n\
-d: print only sequence digits followed by newline\n\
-v: print version information and exit\n\
-q: be less verbose. print only summary information \n\
-h: print this helpful information.\n\
\n"


#define MARK(x) (MARKS[mask+1+(x)]++)
#define MARKED(x) ((MARKS[mask+1+(x)])!=0)

/****************************************************/
/* Stuff to define and select built-in tiebreaker functions (rules) */

#define N_TIEBREAKER_RULES 6 

/* Informative descriptions */

char *tb_desc[N_TIEBREAKER_RULES] = {
"0. Always use 1\n",
"1. Random Bernoulli\n",
"2. Autoregressive - use nth term of sequence as nth tiebreaker\n",
"3. Alternating - 1,0,1,0, ...\n",
"4. Interactive - enter 0 or 1 at prompt\n",
"5. Vince - see source code for this one\n"
};  

/* Library of tiebreaker macros */
/* These defines are obsolete but keep for documentation */
#if 0
#define TIEBREAKER(N) (0)              /* Always use zero */
#define TIEBREAKER(N) (random() > RAND_MAX/2 ? 1 : 0) /* random */
#define TIEBREAKER(N) (terms[(N)])     /* "Autoregressive" */
#define TIEBREAKER(N) ((N)%2 ? 1 : 0)  /* Alternating ones and zeros */ 
#endif

typedef int (*tbfptr)(int); /* pointer to tiebreaker function */
tbfptr tbf;


/* Implementation of tiebreaker functions */
int allone(int n){
	return 1;
}

int bernoulli(int n){
	return (random() > RAND_MAX/2 ? 1 : 0); /* random */
}

int autoregress(int n){
	return terms[n];
}

int alternate(int n){
	return n % 2 ? 0 : 1;
}

int interactive(int n){
	char c;
	printf("? ");
	c = getchar();
	if(c == '0')return 0;
	return 1;
}

/* Vince Fatica suggested this example. The tie breakers
are 1101001000100001... where there is one more zero in each group. 
The kth one is at position k + (k-1)(k-2)/2 and one can check that
a natural number n is of this form if and only if 8n - 7 is a
perfect square. Vince also helped me develop my is_square routine
used here. See our email correspondence for details. 
*/

int vince(int n){

unsigned long k;

k = 8*n+1;

if(is_square(k))return 1;
return 0;

}

/* Jump table for tiebreaker functions */

tbfptr mytbfptrarray[N_TIEBREAKER_RULES] =
{
	&allone, /* default rule */
	&bernoulli,
	&autoregress,
	&alternate,
	&interactive,
	&vince
};

/*******************************************************/

/* Helper routines */

/* Toggle least significant digit */
#define TOGGLE(N) ((N) = (N)&1 ? ((N)-1):((N)+1))


/* Print the sequence of binary digits in x in reverse order. msk gives number 
of digits to print */

void 
binprint(unsigned int x, unsigned msk)
{
	char buf[MAX_NODE_LENGTH];
	int n;

	n = 0;
	while(msk){
		if(x%2)buf[n++]='1';
		else buf[n++]='0';	
		x = x/2;
		msk = msk/2;
	}
	while(--n>=0)putchar(buf[n]);
}

/* returns the number of one bits */

int onbits(unsigned x)
{
	int n = 0;
	while(x){
		n += (x%2); 
		x = x >> 1;
	}
	return n;
}

int 
main(int argc, char **argv){

	int j=0,tb,i=0;
	int n_max = MAX_N;
	int sum = 0;
	unsigned y,z;
        unsigned x = 0;  /* This tracks the just filled string */
	unsigned msk;
	int n=0,m=0; /* m counts tiebreakers used */
	int k = 0; /* k counts uses of rule 1 */
	int l = 0; /* l counts decreases in fill length */
	int w = 0; /* counts dereases > 1 */
	int tb_flag = 1;
	int verbose = 1;
	int dump = 0;


	/* Process command line */
	if(argc <= 1){
		fprintf(stderr,"%s\n",USAGE);
		exit(1);
	}
	while(++j < argc){
		if(argv[j][0] == '-')
			switch(argv[j][1]){ 
				case 's':
				case 'S':
					if(j+1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					srandom(atoi(argv[j+1]));
					j++;
					if(j + 1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					continue;
				case 'q':
				case 'Q':
					verbose = 0;
					if(j + 1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					continue;
				case 'd':
				case 'D':
					verbose = 0;
					dump = 1;
					if(j + 1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					continue;
					
				case 'f':
					if(j+1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					i = atoi(argv[j+1]);
					if(i == -1){
						printf("%d Tiebreaker rules\n",N_TIEBREAKER_RULES);
						printf("Use index with -f to select. \n\n");
						for(i=0;i<N_TIEBREAKER_RULES;i++)
							printf("%s",tb_desc[i]);
						return 0;
					}
					if((i<0)||(i+1 > N_TIEBREAKER_RULES)){
						fprintf(stderr,"Bad index\n");
						return 1;
					}
					j++;
					if(j + 1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
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
					fprintf(stderr,"%s: unkown option %s\n",
PROGNAME,argv[j]);
					exit(1);
			}
	}

	if(j != argc){
		fprintf(stderr,"%s\n",USAGE);
		exit(1);
	}
	n_max = atoi(argv[j-1]);
	if((n_max<0)||(n_max>MAX_N)){
		fprintf(stderr,"%s: terms value out of range\n", PROGNAME);
		exit(1);
	}

	tbf = mytbfptrarray[i]; /* selected tie-breaker rule */
 

	MARKS[1] = 1; /* Mark the root node */
	tb = (*tbf)(m++);
	terms[0]=tb;
	x = tb;
	mask = 1;  

	/* Heading */
	if(verbose){
	printf("x[n]\tn\tfilled node (<-- root) (* denotes tiebreaker)\n");
	printf("------------------------------------------------------------\n");
	}
	while(n++ < n_max){

		/* Mark and print stuff from previous round */

		MARK(x);
		if(tb_flag){
			if(verbose)
				printf("%d*\t%d\t",terms[n-1],n);  
			tb_flag = 0;
		}
		else
			if(verbose)
			printf("%d\t%d\t",terms[n-1],n);  
		sum += terms[n-1];
		if(verbose)
			binprint(x,mask);

/* FOR DEBUG UNCOMMENT */
/*
for(j=0;j<15;j++)
printf(" %d ",MARKS[j]);
printf("x,msk= %d,%d",x,mask);
*/

		if(verbose)
			printf("\n");

		z = x; /* save */

		/* Throw away most signicant digit of filled string */
		/* Normally we fill a successor of this one */

		x = (x<<1 & mask) >>1 ;

		if(MARKED(2*x+1) && MARKED(2*x)){ /* Both already filled */

                /* This is the case where we need to up the fill length
                   and use a tie-breaker */

			x = z; /* restore original filled string */
			tb = (*tbf)(m++);
			tb_flag = 1; /* flag for printout */
			x = x*2 + tb;
			terms[n] = tb;
			k++;
			mask = 2*mask+1;
			continue;
		}

		/* ELSE */

		/* Find the shortest filled tail such that at least one
                 successor is unmarked, if possible */

		msk = 1;
		while(1){
			y = (x & msk);
			msk = 2*msk + 1;
			if(MARKS[2*y+msk+1] == 0)break;
			if(MARKS[2*y+msk+2] == 0)break;
		}

		if(msk < mask){ /* Fill length decreases */
			l++;
			if(mask  > 2*msk + 1)w++;
			x = y;
			mask = msk;
		}

		/* Else fill length stays the same */

		if(!MARKED(2*y)){
			if(!MARKED(2*y+1)){ /* tie breaker needed  */
				tb = (*tbf)(m++);
				tb_flag = 1;
				x = x*2 + tb;
				terms[n] = tb;
				continue;
			}
			else { /* non-tb with +1 tail marked already */ 
				terms[n] = 0;
				x = 2*x;
			}
		}
		else { /* non-tb with +0 tail marked already */
			terms[n] = 1;
			x = 2*x+1;
		}
	}
	if(verbose)
	printf("------------------------------------------------------------\n");
	if(dump==1){
		for(j=0;j<n;j++)printf("%d",terms[j]);
		printf("\n");
		return 0;
	}
	printf("Summary statistics:\n");
	printf("x-bar = %f\n",(double)sum/(double)n);
	printf("tie-breakers used = %d\n",m);
	printf("fill length increases = %d\n",k);
	printf("fill length decreases = %d\n",l);
	printf("Large fill length decreases = %d\n",w);
	return 0;
}
