/* main.c: main program of the monkey random text generator */

/* For background information consult the man page monkey.1 included
in the doc subdirectory. This program must be linked with node.o and
with libmatrix.a in the matrix subdirectory. libmatrix.a is built using
the mkmatrix utility. See its man page in doc also. libmatrix contains
a bunch of repetitious commands which create the large tree-structured
matrix of transition probabilities at run time. For the structure of
each node of the tree see the declaration of nodestruct in global.h. Also
see the comments in mkmatrix.c */

/* The sequence of characters produced by this program X_1, X_2, ...
   is such that (X_k, X_k+1, ... X_k+l) is a markov chain, where
   l = depth -1. If depth = 0 it is just an i.i.d sequence. Given that
   the last state was a string s, the next state is chosen to be a char
   c with probability m/n, where n is the total number of times s occurs
   in the dictionary, and m is the number of times sc occurs. It is
   remarkable how rapidly the text approaches intelligibility with
   increasing depth. */

#include "global.h"
#include "matrix/build.h"
#include <sys/types.h>

#define INFO  "Monkey Random Text Generator, By Terry McConnell\n\
	  Chicken Haven Software\n\
usage: monkey [ -d <n> ] [-s <n> ] [-seed n] [-help] [-version]\n\
-d: base the probability of each new letter on the previous n+1 (default 0)\n\
-seed: use the following int to seed the RNG\n\
-s: print a total of n random characters. (1000)"  

#define USAGE "monkey [-d depth ] [-s size] [-seed n]  [-help] [-version]\n"
#define MAX_ARG_LEN 80             /* Max length of combined options */

int find_opt(char *option_string);
void myerror(char *fmt, ...);
struct NodeStruct *ChooseNode(struct NodeStruct *);


/* The root of the big tree of transition probs */
struct NodeStruct Root = {"",0,0,NULL,NULL,NULL,NULL};

char ProgramName[256];

int main(int argc, char **argv)
{
	int c=-1;                /* numeric option code */
	int seed;
	int i,j,n,m;
	char *info = INFO;
	char opsep = '-';      /* Standard for Unix */
	char argstr[MAX_ARG_LEN];
	struct NodeStruct *ANode,*BNode,*CNode,*DNode;

/* defaults */
	char depth = 0;
	int num_chars = 1000;

	/* default seed comes from our pid */
	seed = (int)getpid();

/* Program name is available in ProgramName */
	strcpy(ProgramName,argv[0]);

/* Process Command Line Options: this is my humble boiler-plate code */

if(--argc > 0){  /* Anything on command line ? */
                ++argv;
                while((argc > 0)&&((*argv[0]) == opsep)){ /* loop while 
							there are flags */
                        argstr[0]=opsep;
                        argstr[1]='\0';
                        strcat(argstr,++*argv); /* glob argstr*/
                        argc--;
                        while((argc>0)&&(strlen(*argv)<3)&&
				((*(argv+1))[0]==opsep)&&((*(argv+1))[1]!=opsep)){
                                ++argv;
                                argc--;
                                strcat(argstr,++*argv);
                        }
                        ++argv; /* ok, consecutive flags now globbed into one*/
                       c = find_opt(argstr+1); /* look up flag code */
                switch(c){

			case -1: break;  /* default, e.g no options */

			case 0: printf("%s\n",info);
				return 0;
			case 1:
				if(argc == 0){
					myerror("%d\n",USAGE);
					return 1;
				}
			        depth = (char) atoi(*(argv++));
				argc--;
				if(depth > DEPTH) {
					myerror("Maximum depth is %d\n",DEPTH);
					return 1;
				}
				break;

			case 2: 
				if(argc == 0){
					myerror("%d\n",USAGE);
					return 1;
				}
			        num_chars =  atoi(*(argv++));
				argc--;
				break;

			case 4: if(argc == 0){
					myerror("%d\n",USAGE);
					return 1;
				}
				seed = atoi(*(argv++));
				argc--;
				break;

			case 3: printf("%s (depth = %d)\n", VERSION,DEPTH);
				return 0;
                        case -2: ;  /* barf (don't use in your code) */
			default:
				fprintf(stderr,"%s: illegal option\n",argstr);
				fprintf(stderr,USAGE);
				return 1;
			} /* End flag switch */
	} /* End flag grabbing while loop */
} /* end if: any args ? */

/*************************************************************************/
/*        End of option processing section. Here begins the big show   */

/* Create the matrix of transition probabilities */
	CreatMatrix1();

	/* seed random number generator */
        srandom(seed);

	/* Print the right number of initial characters depending on depth */

	switch(depth){
	case 0:
		ANode = &Root;
		break;

	case 1:

		ANode = ChooseNode(&Root);
		printf("%c",*(ANode->name + strlen(ANode->name) -1));
		break;

	case 2:

		BNode = ChooseNode(&Root);
		printf("%c",*(BNode->name + strlen(BNode->name) -1));
		ANode = ChooseNode(BNode);
		printf("%c",*(ANode->name + strlen(ANode->name) -1));
		break;

	case 3:

		CNode = ChooseNode(&Root);
		printf("%c",*(CNode->name + strlen(CNode->name) -1));
		BNode = ChooseNode(CNode);
		printf("%c",*(BNode->name + strlen(BNode->name) -1));
		ANode = ChooseNode(BNode);
		printf("%c",*(ANode->name + strlen(ANode->name) -1));
		break;


	case 4:
		DNode = ChooseNode(&Root);
                printf("%c",*(DNode->name + strlen(DNode->name) -1));
                CNode = ChooseNode(DNode);
                printf("%c",*(CNode->name + strlen(CNode->name) -1));
                BNode = ChooseNode(CNode);
                printf("%c",*(BNode->name + strlen(BNode->name) -1));
                ANode = ChooseNode(BNode);
                printf("%c",*(ANode->name + strlen(ANode->name) -1));
                break;


	default:
;
	}

/* Print the remaining characters */

	for(i= depth+1;i<=num_chars;i++){
              /* Print a newline every 80th character */
                if(i % 80 == 0) printf("\n");

        	/* Pick a child at random */

                ANode = ChooseNode(ANode);

        /* Print last char of node's name */

                printf("%c",*(ANode->name + strlen(ANode->name) -1));
		ANode = GetNode(ANode->name+1,&Root);

	/* This should never happen, because if foobar occurs in the
           dictionary, then so does oobar */
		if(ANode == NULL)
			fatal(stderr,"No next node. Tree corrupt!\n");
        }

	/* Give back prompt on a new line */

	printf("\n");

	return 0;
}

/* Select a node to provide the next character by adding up
            the nodal frequencies until they exceed the randomly chosen
            number above.
*/

struct NodeStruct *ChooseNode(struct NodeStruct *parent)
{
	int j;
	long m=0,n;
	struct NodeStruct *ANode;
	
	/* Select a number at random between 0 and the sum of all
           the nodal frequencies. */

        n = (random() % (long)(parent->freq));
	ANode = parent->firstchild;

        for(j=0;j<parent->num_children;j++){
                        m += (long)(ANode->freq);
                        if(m >  n) break;
                        /* else */
                        ANode = ANode->sibling;
                }

	return ANode;
}


/*  find_opt: return a unique small integer for each possible option string */
/*  There should be a case in the switch statement in main to handle each. */
/*  -2 reserved for arg not found -1 reserved for no options */

/* N.B. word doesn't contain the leading hyphen */

int find_opt(char *word)
{
	if(strcmp(word,"help")==0)return 0;
	if(strcmp(word,"d")==0)return 1;
	if(strcmp(word,"s")==0)return 2;
	if(strcmp(word,"version")==0)return 3;
	if(strcmp(word,"seed")==0)return 4;
	

/* arg not found */
	return -2;
}

/* myerror: generic error reporting routine */

void myerror(char *fmt, ...)
{
        va_list args;

        va_start(args,fmt);
        fprintf(stderr,"%s: ",ProgramName);
        vfprintf(stderr,fmt,args);
        fprintf(stderr,"\n");
        va_end(args);
        exit(1);
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

