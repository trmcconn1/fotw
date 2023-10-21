/* This is the simplest possible useful monitor-based application. A little
shell.
*/

/* To allow monitor to be assembled directly by nasm while still allowing
   for external commands to be written in c, this file sets up the stack
   the way monitor is expecting it. Any use of the standard C library requires
   linking the c run-time startup code. Among other things this takes the
   stack as set up by the OS  - i.e. exactly what monitor expects -
   and prepares for the call of main. Thus, our main has to undo what
   runtime did. */

/* When you incorporate monitor as the command line interface of an 
application, you should start with a copy of this file as the main.c 
of the application, and tailor the top part to the application (help,
option processing, etc ). Leave the bottom part (clearly marked) as is. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZ 512
FILE *rcfile = NULL;
extern int monitor(int, char **, char **);
extern int envc;
extern char linebuf1[];
extern char rcscript[];
extern char alone;
extern char copy_env;
static char *s;  
static int i,n;
static char **myargv;
static char **myenvp;
static int myargc;
static int rval;
static int verbose = 1;
int debug = 1;
 

#define USAGE "wordle [-v -h -e -q] [file]"
#define HELP "command monitor for wordle gaming"
#define HELP1 "-v: print version number and exit"
#define HELP2 "-h: print this helpful information and exit"
#define HELP3 "-e: include OS environment in program environment"
#define HELP4 "-q: quiet(er) operation"

int main(int argc, char **argv, char **envp) {

	char *p;

	alone = 0;  /* monitor is not running stand-alone */

	myargv = argv;
	myargc = argc;
	myenvp = envp;

	myargv++; /* skip program name */
	myargc--; /* and reduce arg count by one */

	while(myargc && ((*myargv)[0]=='-')){
		switch((*myargv)[1]){
			case 'v':
				printf("%s\n",VERSION);
				return 0;
			case 'h':
				printf("%s\n",USAGE);
				printf("%s\n",HELP);
				printf("%s\n",HELP1);
				printf("%s\n",HELP2);
				printf("%s\n",HELP3);
				printf("%s\n",HELP4);
				return 0;
			case 'e':
				copy_env = 1;	
				break;
			case 'q':
				verbose = 0;
				break;
			default:
				fprintf(stderr,"lsh: -%c unknown option\n", 
				      (*myargv)[1]);
				fprintf(stderr,"%s\n",USAGE);
				return 1;
		}
		--myargc;myargv++;
	}
	if(myargc > 0){
		rcfile = fopen(*myargv,"r");
		if(!rcfile){
			fprintf(stderr,"Unable to open %s\n",*myargv);
		}
	}
	
	/* Call the entry point of monitor */

	rval = monitor(argc,argv,envp);

	if(verbose)
		printf("main: monitor returned %d\n",rval);

	return rval;
}
