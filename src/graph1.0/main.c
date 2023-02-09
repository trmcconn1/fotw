/* To allow monitor to be assembled directly by nasm while still allowing
   for external commands to be written in c, this file sets up the stack
   the way monitor is expecting it. Any use of the standard C library requires
   linking the c run-time startup code. Among other things this takes the
   stack as set up by the OS  - i.e. exactly what monitor expects -
   and prepares for the call of main. Thus, our main has to undo what
   runtime did. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZ 512
extern void monitor(void);
extern char linebuf1[];
extern char rcscript[];
extern char alone;
extern char copy_env;
static char *s;  
static int i,n;
static char **myargv;
static int myargc;
static int rval;
static int verbose = 1;
int debug =0;
int quiet = 0;
char directed = 0;
 

#define USAGE "graph [-v -h -d -D -q] file"
#define HELP "interactive graph analyzer with startup file"
#define HELP1 "-v: print version number and exit"
#define HELP2 "-h: print this helpful information and exit"
#define HELP3 "-q: quiet(er) operation"
#define HELP4 "-d: treated graphs as directed graphs"
#define HELP5 "-D turn on debugging"

int main(int argc, char **argv, char **envp) {

	FILE *rcfile;
	char *dest = rcscript;
	


	alone = 0;  /* monitor is not running stand-alone */

	myargv = argv;
	myargc = argc;

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
				printf("%s\n",HELP5);
				return 0;
			case 'q':
				quiet = 1;	
				break;
			case 'd': 
				directed = 1;
				break;	
		        case 'D':
				debug = 1;
				break;
			default:
				fprintf(stderr,"graph: -%c unknown option\n", 
				      (*myargv)[1]);
				fprintf(stderr,"%s\n",USAGE);
				return 1;
		}
		--myargc;myargv++;
	}
	if(myargc > 0){
		rcfile = fopen(*myargv,"r");
		if(!rcfile){
			fprintf(stderr,"Unable to open %s\n",myargv[1]);
				return 1;
		}
		while(fgets(linebuf1,BUF_SIZ,rcfile)){
			strcpy(dest,linebuf1);
			dest += strlen(linebuf1);
		}
		*dest = '\0';
		fclose(rcfile);
	}

/**************************************************************
*
*  From this point you should leave the code as-is 
*
***************************************************************

	/* Put a null word on the stack to indicate top of environment
           string pointers */

	asm volatile (
		"xor %%eax, %%eax\n\t"
		"pushl %%eax\n\t"
		:
		:
		:
	);
	n++;

	/* Stack up the successive environment string pointers ending
           with a NULL word that will mark end of args */

	do
	{
		s = *envp;
		asm volatile (
			"pushl %0\n\t"
			:
			: "r" (s)
			:
		);	
		envp++;
		n++;
	}
	while(*envp != 0);

/* Put a Null word on stack to indicate end of argv pointers */
/* Not sure why this needs to be done, because I'd think the last time
   through the do ... while loop would already push a null pointer. Maybe
   the compiler optimization is playing games with us ? */

	asm volatile (
		"xor %%eax, %%eax\n\t"
		"pushl %%eax\n\t"
		:
		:
		:
	);

	/* Stack up the successive argument vector string pointers */ 

	for(i=1;i<argc;i++)argv++; /* so they wont be backwards */
	for(i=0;i<argc;i++)
	 {
		s = *argv;
		asm volatile (
			"pushl %0\n\t"
			:
			: "r" (s)
			:
		);	
		argv--;
		n++;
	}

	/* Finally, stack up the argc */

	asm volatile (
		"pushl %0\n\t"
		 :
		 : "r" (argc)
		 :
	);
	n++;

	/* Call the entry point of monitor */

	monitor();

    /* recall that gas syntax moves stuff from left to right */

	asm volatile (
		"lea %0,%%eax\n\t" 
		"mov %%ecx,(%%eax)\n\t"
		 : "+m" (rval)
		 : 
		 :
	);

	if(rval == 2){
		printf("exec failed\n");
		return 1;
	}
	if(verbose)
		printf("main: monitor returned %d\n",rval);

	/* Unstack everything that was put there */

	for(i=0;i<n;i++)
		asm volatile (
			"popl %%eax\n\t"  
		 	:
		 	: 
		 	:
		);
	return rval;
}
