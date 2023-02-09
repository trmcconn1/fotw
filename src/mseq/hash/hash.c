/* hash.c:  Program to implement a tree filling scheme based on depth d hash function */

/* By Terry McConnell */

/* Start with a binary tree to depth d with all nodes empty, then proceed to mark
   exactly one node on each turn. Call the node just marked the current
   node, and the tail of that node the current tail. Here tail(abcd) = 
   bcd. The rules are the following:

	1) Keep taking tail until a marked node is obtained.
           
	2) It the mark is 0, mark the 1 child 1. Else mark the 0 child 0.

*/

#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<sys/types.h>
#include<unistd.h>

#define DEPTH 4
#define MAX_TIME 1000  /* Change as desired */
#define LONG_BITS 32  /* Number of bits in a long int */

#define USAGE "usage: filling [-p]: -p: print each generation, rather \n\
than just summary statistics\n"

/* The basic data structure: contains all relevant information about
   the node that is filled at time n */

struct NodeStruct {
	int time;   /* time that the node was last marked  */
	unsigned long place; /* bitmap location of the node. : read
                                   from right to left. */
                                /* 1 = right branch, 0  = left */
                                /* We refer to this as s below */
                                /* Leading 1 is a placeholder */
	int length; /* the length of string representing the place */
	int mark;
	struct NodeStruct *right_child; /* pointer to 1s  node */
        struct NodeStruct *left_child;  /* pointer to 0s node */
} *Nodes;


/* More on place: level 1 of tree is places 2 and 3
                  level 2 of tree is places 4-7
		  level 3 of tree is places 8-15
		  level 4 of tree is places 16-31

                  etc
*/
struct NodeStruct *AllocNode( unsigned long, int);
struct NodeStruct *GetNode(unsigned long);
void fatal(FILE *stream,char *fmt, ...);
char  *ltostr(unsigned long, char *);
unsigned long prepend(int , unsigned long);
unsigned long Tail(unsigned long);
int TieBreaker(int);


struct NodeStruct Root = {0,1UL,0,0,NULL,NULL};

int 
main(int argc, char **argv)
{
	int n = 0;   /* time parameter */
	int printout = 0;
	int X;
	int l;
	int m,k;
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
	srandom(getpid());

/* Initialize node tree to depth n by using the DeBruijn algorithm */
	Nodes = &Root;
	current_node = &Root;
	l = 0;
	while((l=current_node->length) <= DEPTH){

	X = current_node->mark;
printf("current node place: %ld\n",current_node->place);
	current_place = current_node->place;
	current_tail = Tail(current_place);

/* Iterate tail until one is filled */
        while(GetNode(current_tail) == NULL && current_tail > 1)
              current_tail = Tail(current_tail); 
	
/* Look up children of current tail*/

	Right = GetNode(prepend(1,current_tail));
	Left = GetNode(prepend(0,current_tail));

/* If both exist and not at full depth then fill a child of the
   original current node */

   if(Right != NULL && Left != NULL){
        /* fill child of current node */
		X = 1;
                Child = AllocNode(prepend(X,current_place), n);
                if(X) current_node->right_child = Child;
                else current_node->left_child = Child;
                current_node = Child;
		current_node->mark = 1;
                continue; /* back to beginning of main loop */
        }
  	current_node = GetNode(current_tail);
         if (Right != NULL){
             Child = AllocNode(prepend(0,current_tail),n);
             current_node->left_child = Child;
             current_node = Child;
	     current_node->mark = 0;
             continue;  /* back to beginning of main loop */
             }

          /* else */

          if(Left != NULL){
            Child = AllocNode(prepend(1,current_tail),n);
            current_node->right_child = Child;
            current_node = Child;
	    current_node->mark = 1;
            continue;
            }
            else {
                   X = 1;
                   Child  = AllocNode(prepend(X,current_tail), n);
                   if(X) current_node->right_child = Child;
                   else current_node->left_child = Child;
                   current_node = Child;
		   current_node->mark = 1;
                   continue;
	    }
	}  /* end of initialization */

/* Set seed marks on level DEPTH of tree */

	k = 1;m=DEPTH;
	while(m--)k *=2;
/* k = 2^DEPTH */
	for(m=k;m<2*k;m++){
		current_node = GetNode(m);
		if(random()>RAND_MAX/2)
			current_node->mark=0;   
		else current_node->mark=1;
	}
			
	while(n++<MAX_TIME)
	 {
		X = current_node->mark;
		current_place = current_node->place;
		current_tail = Tail(current_place);
		Right = GetNode(prepend(1,current_tail));
		Left = GetNode(prepend(0,current_tail));

		if(X==1){
			current_node=Right;
			current_node->mark = 1 - Right->mark;
		}
		else { 
			current_node=Left;
			current_node->mark = 1- Left->mark;
		}
		printf("%d",X);
		continue;	
	}
	printf("\n");
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
	ANode->mark = 0;
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

/* Tie Breaker function that returns 0 and 1 alternately 

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
