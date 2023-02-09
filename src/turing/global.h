/* global.h: declarations and defines that must be in visible in more than
   one source file */

#define VERSION "1.2"

#define MAXTOKEN 0xFFFF
#define MAXTOKENS 256
#define MAX_PATH 256
#define MAX_PREFIX 256

/* Turing machines that do anything substantial require large numbers of
   states and instructions. These may need to be increased: */
#define MAX_STATES 0xFFFF
#define MAX_INSTRUCTIONS 0xFFFF

#define MAX_ALPHABET 256

/* Result of a parse */
#define DIRECTIVE 0
#define INSTRUCTION 1
#define ERROR 2
#define HARDEOF 3
#define SOFTEOF 4

/* possible actions */
#define HALT 0
#define RIGHT 1
#define LEFT 2
#define WRITE 3

#define TRUE 1
#define FALSE 0

/* Internal representation of an instruction */

struct instruction {

	int state;   /* the state in which the instruction applies */
	int input;  /* the input that must be seen by the read head in order
                       for the instruction to apply */

	int action; /* what to do: see defines */
	int output;
	int newstate;
};

struct statestruct {
	int indx;
	char *name;
};

extern struct instruction *instructions[];
extern char alphabet[];
extern struct statestruct *states[];
extern int nstates;
extern int preprocess;
extern int alphabet_size;
extern int tape_size;
extern int ninstr;
extern void krqsort( void *v[],int left, int right, int (*comp)(void *,void *));
extern void *binsearch(void *key,void *v[],int n, int (*comp) (void *, void *));
extern int *alloc_tape(int);
extern void free_tape();
extern int grow_tape();
extern int compare_inst( struct instruction *, struct instruction *);
extern int line;
extern FILE *yyin;
