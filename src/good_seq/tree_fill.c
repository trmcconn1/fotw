/* tree_fill.c: Generate filled nodes of a tree filling sequence */

/* By Terry R. McConnell */

/* This program does essentially the same thing as mseq/misc/filling.c,
   but I think this one has a better approach. The following is taken
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

*/ 

#include <stdio.h>
#include <stdlib.h>

#define MAX_N 4096   /* Maximum number of terms to generate */

#define MAX_NODE_LENGTH 20      /*  == L */
#define MAX_TREE 2097152       /* should be 2^L X 2 */
static unsigned MARKS[MAX_TREE];
static unsigned char terms[MAX_N];
static unsigned mask = 1;

/* The binary digits of x reference a unique node on the binary tree. To
use these as an address in MARKS array, we need to prefix on the left with
a '1', else leading zeros won't count */

#define PROGNAME "tree_fill"
#define VERSION "1.01"
#define USAGE "usage: tree_fill [-n <terms> -s <seed> -h -v]"

#define HELP "Print terms and filled nodes of tree filling sequence\n\
usage: tree_fill [-n <terms> -s <seed> -h -v]\n\
-n: print number of terms specified\n\
-s: seed random number generator\n\
-v: print version information and exit\n\
-h: print this helpful information."


#define MARK(x) (MARKS[mask+1+(x)]++)
#define MARKED(x) ((MARKS[mask+1+(x)])!=0)

/* Library of tiebreaker macros */
/* Comment out ones you don't want, write new ones as needed. */
#define TIEBREAKER(N) (random() > RAND_MAX/2 ? 1 : 0) /* random */
#if 0
#define TIEBREAKER(N) (terms[(N)])     /* "Autoregressive" */
#define TIEBREAKER(N) (0)              /* Always use zero */
#define TIEBREAKER(N) ((N)%2 ? 1 : 0)  /* Alternating ones and zeros */ 
#endif

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

int 
main(int argc, char **argv){

	int j=0,tb;
	int n_max = MAX_N;
	unsigned x=0,y;
	int n=0,m=0; /* m counts tiebreakers used */
	int tb_flag = 1;


	/* Process command line */
	while(++j < argc){
		if(argv[j][0] == '-')
			switch(argv[j][1]){ 
				case 'n':
				case 'N':
					if(j+1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					n_max = atoi(argv[j+1]);
					if((n_max<0)||(n_max>MAX_N)){
						fprintf(stderr,"%s: terms value out of range\n", PROGNAME);
						exit(1);
					}
					j++;
					continue;
				case 's':
				case 'S':
					if(j+1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					srandom(atoi(argv[j+1]));
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
					fprintf(stderr,"%s: unkown option %s\n",
PROGNAME,argv[j]);
					exit(1);
			}
	}

	if(j > argc){
		fprintf(stderr,"%s\n",USAGE);
		exit(1);
	}
 
	MARKS[1] = 1; /* Mark root node */
	x = TIEBREAKER(m);
	terms[0]=x;
	m++;

	/* Heading */
	printf("x[n]\tn\tfilled node (<-- root) (* denotes tiebreaker)\n");
	printf("------------------------------------------------------------\n");
	while(n++ < n_max){

		MARK(x);
		if(tb_flag){
			printf("%d*\t%d\t",terms[n-1],n);  
			tb_flag = 0;
		}
		else
			printf("%d\t%d\t",terms[n-1],n);  
		binprint(x,mask);
		printf("\n");

		/* Throw away most significant digit */
		y = ((x << 1) & mask)>>1; 
		while(MARKS[(mask+1)/2 + y] == 0) {
			mask = (mask+1)/2 - 1; /* decrease the mask */
			y = (y>>1);
		} 
		tb = TIEBREAKER(m);
		if(MARKED(y = 2*y + tb)){
			if(MARKED(TOGGLE(y))){
				x = x*2 + tb;
				mask = (2*mask)+1; /* increase the mask */
				terms[n] = tb;
				m++;
				tb_flag = 1;
				continue;
			}
			else {
				terms[n] = 1 - tb;
				x = y;
			}
		}
		else {
			x = y;
			terms[n] = tb;
			if(!MARKED(TOGGLE(y))){
				m++;
				tb_flag = 1;
			}
		}
	}
}
