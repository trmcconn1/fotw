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
#include "ephemeris.h"

#define BUF_SIZ 512
extern void monitor(void);
extern char linebuf1[];
extern char rcscript[];
extern char copy_env;
static char *s;  
static int i,n;
int debug =0;
int verbose = 0;
static int myargc;
static char **myargv;

#define USAGE "ephemeris [-v -h -D -q -e] [file]"
#define HELP "generates ephemerides and related things [startup file]"
#define HELP1 "-v: print version number and exit"
#define HELP2 "-h: print this helpful information and exit"
#define HELP3 "-q: quiet(er) operation"
#define HELP4 "-D turn on debugging"
#define HELP5 "-e: include os environment at startup"
#define HELP6 "-x[X]: run experimental code and continue[exit]"

int main(int argc, char **argv, char **envp) {

	FILE *rcfile;
	char *dest = rcscript;
	int rval = 0;	

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
				printf("%s\n",HELP6);
				return 0;
			case 'q':
				verbose = 0;	
				break;
		        case 'D':
				debug = 1;
				break;
			case 'e':
				copy_env = 1;
				break;
			case 'x':
			case 'X':
/* Put yer experimental code here */
{


/* Make sure any variables declared in this block are static. Otherwise
it seems to screw up the stack somehow */

/* This constructs the orbit that osculates with the motion
of the moon on its mean orbit at Y2K. Note that this is not the same
as the osculating orbit at the epoch, which would involve knowing the
actual lunar position and velocity at that instant. I was puzzled at first
that this does not exactly reproduce the mean lunar orbit, but then I realized
that the earth's gravity is not right for an object on the mean lunar orbit
to continue on that exact same orbit. The centrifugal force from the 
revolution of the earth-moon system pretty much exactly accounts for the
difference. 
*/
	static struct orbital_elements *orbit,*orbit2;
	static struct initial_conditions *my_ic;
	
	load_builtins();
	my_ic = ic_lib[0];
	dump_ic(my_ic);
	orbit = initialconditions2elements(my_ic);
	info_orbit(orbit);
	if(my_ic = elements2initialconditions(orbit));
		dump_ic(my_ic);
}
				if((*myargv)[1] =='X')return 0;
				break;
			default:
				fprintf(stderr,"ephemeris: -%c unknown option\n", 
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
				return 1;
		}
		while(fgets(linebuf1,BUF_SIZ,rcfile)){
			strcpy(dest,linebuf1);
			dest += strlen(linebuf1);
		}
		*dest = '\0';
		fclose(rcfile);
		myargc--;
		*myargv++;
	}
	else {
/* try to open .ephemeris2.rc in the home directory */
		linebuf1[0]='\0';
		if(getenv("HOME"))strcpy(linebuf1,getenv("HOME"));
		strcat(linebuf1,"/.ephemeris2.rc");
		rcfile = fopen(linebuf1,"r");
		if(rcfile){
			while(fgets(linebuf1,BUF_SIZ,rcfile)){
				strcpy(dest,linebuf1);
				dest += strlen(linebuf1);
			}
			*dest = '\0';
			fclose(rcfile);
		}
	}

/* load all built-in orbits */
	 load_builtins(); 

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
