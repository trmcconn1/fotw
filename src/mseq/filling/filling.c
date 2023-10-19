/* filling.c:  Program to implement a tree filling scheme. */

/* By Terry McConnell */

/* Start with a binary tree with all nodes empty, then proceed to fill
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

#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>

#define MAX_TIME 1000  /* Change as desired */
#define LONG_BITS 32  /* Number of bits in a long int */

#define USAGE "usage: filling [-p]: -p: print each generation, rather \n\
than just summary statistics\n"

/* The basic data structure: contains all relevant information about
   the node that is filled at time n */

struct NodeStruct {
	int time;   /* time that the node first fills */
	unsigned long place; /* bitmap location of the node. : read
                                   from right to left. */
                                /* 1 = right branch, 0  = left */
                                /* We refer to this as s below */
                                /* Leading 1 is a placeholder */
	int length; /* the length of string representing the place */
	struct NodeStruct *right_child; /* pointer to 1s  node */
        struct NodeStruct *left_child;  /* pointer to 0s node */
} *Nodes;



struct NodeStruct *AllocNode( unsigned long, int);
struct NodeStruct *GetNode(unsigned long);
void fatal(FILE *stream,char *fmt, ...);
char  *ltostr(unsigned long, char *);
unsigned long prepend(int , unsigned long);
unsigned long Tail(unsigned long);
int TieBreaker(int);


struct NodeStruct Root = {0,1UL,0,NULL,NULL};

int 
main(int argc, char **argv)
{
	int n = 0;   /* time parameter */
	int k = 0;   /* number of tie-breakers used */
	int printout = 0;
	int X;
	int running_sum = 0;
	int max_length = 0;
	int max_gap = 0;
	int gap;
	unsigned long current_place = 1UL;
	unsigned long current_tail;
	struct NodeStruct *current_node;
	struct NodeStruct *Right;
	struct NodeStruct *Left;
	struct NodeStruct *Child;
	char buffer[BUFSIZ];

	/* process command line */
	if(argc > 2){
		fprintf(stderr,USAGE);
		return 1;
	}

	if(argc == 2){
		if(strcmp(argv[1],"-p")==0)printout=1;
		else if((strcmp(argv[1],"-?")==0)||
			(strcmp(argv[1],"-help")==0)){
				fprintf(stderr,USAGE);
				return 0;
		     }
		     else {
			fprintf(stderr,USAGE);
			return 1;
		     }
	}

/* Initialize random number generator */
	srandom(1);
/* Initialize tree */

	Nodes = &Root;
	current_node = &Root;

/* Print heading */
	if(printout)
	   printf("n x(n) (tie-breakers used): tail filled at time n.\n\n");


/* Main loop: given the node just filled, find,
              alloc, and fill the new one */

	while(n++ < MAX_TIME){

/* Keep track of current record values */

	if(current_node->length > max_length)max_length = current_node->length;
	gap = max_length - current_node->length;
	if(gap > max_gap)max_gap = gap;
	running_sum += buffer[strlen(buffer)-1] - '0';

/* Print out stuff */
	ltostr(current_node->place,buffer);
	if(n>1)
	  if(printout)
	    printf("%d %c (%d):\t\t%s\n",n-1,buffer[strlen(buffer)-1],k,buffer);

	  
/* Use when printing out sequence only */
	/* printf("%c",buffer[strlen(buffer)-1]); 
	if(n%80 == 0)printf("\n"); */
	current_place = current_node->place;
	current_tail = Tail(current_place);


/* Iterate tail until one is filled */
        while(GetNode(current_tail) == NULL && current_tail > 1)
              current_tail = Tail(current_tail); 
        
	
/* Look up children of current tail, to see whether they are filled */

	Right = GetNode(prepend(1,current_tail));
	Left = GetNode(prepend(0,current_tail));

	if(Right != NULL && Left != NULL){

        /* fill child of current node */

		X = TieBreaker(k++);
		Child = AllocNode(prepend(X,current_place), n);
	 	if(X) current_node->right_child = Child;
		else current_node->left_child = Child;
		current_node = Child;
		continue; /* back to beginning of main loop */
	}
		
	/* else */

	 current_node = GetNode(current_tail);
	 if (Right != NULL){
	     Child = AllocNode(prepend(0,current_tail),n);
	     current_node->left_child = Child;
             current_node = Child; 
	     continue;  /* back to beginning of main loop */ 
	     }

	  /* else */

	  if(Left != NULL){
            Child = AllocNode(prepend(1,current_tail),n);
	    current_node->right_child = Child;
            current_node = Child;
	    continue;
	    }
	    else {
		   X = TieBreaker(k++);
		   Child  = AllocNode(prepend(X,current_tail), n);
                   if(X) current_node->right_child = Child;
                   else current_node->left_child = Child;
                   current_node = Child;
		   continue;
	     }

	}  /* end main loop */
			
	/* Print summary statistics */
	fprintf(stderr,"Max Length = %d, Max Gap = %d, Avg %g \n",max_length, max_gap,
		(float)running_sum/(float)n);
	return 0;
}


/* ltostr: converts long argument to a string of 0s and 1s that gives the
   binary representation of the argument. Returns pointer to string */
/* Recall that the long argument always starts with a 1, which serves only
   as a placeholder */

char *ltostr(unsigned long number, char *binary)
{
	unsigned long quotient;
	char buffer[LONG_BITS];
	int i = 0,j;

	buffer[0] = 0;

	for(quotient = number; quotient > 1UL; i++){
		if(quotient % 2UL) buffer[i]='1';
		else buffer[i]='0';
		quotient = quotient >> 1;
	}

	/* Copy buffer and properly terminate */
	for(j=0;j<i;j++)
		*(binary + j) = buffer[j];
	*(binary + j) = '\0';
	return binary;
} 

/* GetNode: returns a pointer to the node  whose place component
   matches the argument, or NULL if there is no such node. */

struct NodeStruct *GetNode(unsigned long pattern)
{
	struct NodeStruct *ANode = Nodes;
	unsigned long quotient;
	
	quotient = pattern;
	while(quotient > 1UL){
		if(quotient % 2UL) 
		  if(ANode->right_child != NULL)
			ANode = ANode->right_child;
	          else return NULL;
		else if(ANode->left_child != NULL)
			ANode = ANode->left_child;
                     else return NULL;
		quotient = quotient >> 1;
	}
	return ANode;
}

/* prepend: attaches digit at beginning of long argument, just before
   its leading 1 */

unsigned long prepend(int digit, unsigned long pattern)
{
	unsigned long p = 1UL;
	unsigned long quotient;

	/* Find the number 2^(i+1), of binary digits in pattern,
           where i is the number of digits in pattern */

	quotient = pattern;
	while(quotient > 0UL){
		p *= 2UL;
		quotient = quotient >> 1;
	}

	/* stick a new leading one on the front */
	pattern = pattern + p;

	/* replace original leading 1 with digit */
	if(!digit) pattern -= p/2;

	return pattern;
}
		

/* AllocNode: allocate memory and initialize a new node */

struct NodeStruct *AllocNode(unsigned long place, int time)
{
	struct NodeStruct *ANode;
	char buffer[256];

	if((ANode = (struct NodeStruct *)malloc(sizeof(struct NodeStruct)))
             == NULL)
		fatal(stderr,"Unable to alloc new node\n");

	ANode->place = place;
	ANode->time = time;
	ltostr(ANode->place,buffer);
	ANode->length = strlen(buffer);
	ANode->left_child = NULL;
	ANode->right_child = NULL;

	return ANode;
}


/* Generic error routine */

void fatal(FILE *stream,char *fmt, ...)
{
        va_list args;

        va_start(args,fmt);
        fprintf(stream,"%s: Fatal Error.\n","mseq");
        vfprintf(stream,fmt,args);
        fprintf(stream,"\n");
        va_end(args);

        exit(1);
}


/* Tail: returns the unsigned long corresponding to the tail of its argument */

unsigned long Tail(unsigned long pattern)
{
	if(pattern > 1UL)
          return pattern/2;
	else return pattern;
}


/* Library of Tie Breaker sequences. Only one should be uncommented at a
     time */

/* "Trivial one that always returns 1 */
/*
int TieBreaker(int index)
{
	return 1;
}
*/
/* Tie Breaker function that returns 1 and 0 alternately 

*/
int TieBreaker(int index)
{
	static parity = 0;

	if(parity) parity = 0;
	else parity = 1;
	return parity;
}
/* Returns 110 repeatedly */

/*
int TieBreaker(int index)
{
	static state = 0;
	switch(state){
		case 0:
			state=1;
			return 1;
		case 1:
			state=2;
			return 1;
		case 2:
			state=0;
			return 0;
	}
}
*/

/* Returns 1100 repeatedly */

/*
int TieBreaker(int index)
{
	static state = 0;
	switch(state){
		case 0:
			state=1;
			return 1;
		case 1:
			state=2;
			return 1;
		case 2:
			state=3;
			return 0;
		case 3:
			state=0;
			return 0;
	}
}
*/

/* General periodic tie-breaker: gets info from the user on first
   call
*/

#if 0
#define MAX_STATES 256

int TieBreaker(int index)
{
	static int first_call = 1;
	static int nstates;
	static int tbreakers[MAX_STATES];
	int state;
	char input_buf[32];
	int i;
	char c;

	if(first_call){
		fprintf(stderr,"Using generic periodic tie-breaker sequence\n");
		fprintf(stderr,"Enter period (0 < period < %d): ",MAX_STATES);
		fgets(input_buf,32,stdin);
		input_buf[strlen(input_buf)-1]='\0';
		nstates = atoi(input_buf);
		fprintf(stderr,"Using %d states\n",nstates);
		fprintf(stderr,"At each prompt, enter 1 or 0 for tiebreakers\n");
		for(i=0;i<nstates;i++){
			fprintf(stderr,"\nEnter> ");
			c = fgetc(stdin);
			tbreakers[i] = ( c == '1' ? 1 : 0);
			c = fgetc(stdin);  /* discard \n */
		}
		fprintf(stderr,"Using ");
		for(i = 0;i<nstates;i++) fprintf(stderr,"%d ",tbreakers[i]);
		fprintf(stderr,"\n");
		fprintf(stderr,"Correct? [yn] ");
		c = fgetc(stdin);
		if(c == 'n')exit(1);
		fprintf(stderr,"\n");
		first_call = 0;
	}
	state = index % nstates;
	return tbreakers[state];
}		
#endif
		
		
/*  Returns random Bernoulli sequence 

int TieBreaker(int index)
{
	return (random() % 2);
} */


/*  A lacunary sequence:110100100001000000001...  nth one is in pos 2^(n-2)+n-1 
for n >= 2. */
#ifdef FOO

int TieBreaker(int index)
{

	int n=0;  
	int i;
	int m = 1;  /* power of 2 */

	i = index + 1; /* Tie breakers are called for in the main program
                           starting with index 0 rather than 1 */

	if(i==1) return 1; /* special case */

/* figure out whether i is of the form 2^n + n + 1 */

	while(i - m -n - 1 > 0){
		n++;
		m *= 2;   /* m = 2^n */
	} 
	return ( i - m -n - 1 == 0 ? 1 : 0 ); 
}
#endif 

/* The sequence 1101001000100001... nth one in position n + (n-1)(n-2)/2 */

/*
int TieBreaker(int index)
{
	int n = 2;
	int m;
	int i;


	i = index + 1;
	if(i==1) return 1;
	while((m=n + (n-1)*(n-2)/2 )< i)n++;
	return ( m == i ? 1 : 0);
}
*/
