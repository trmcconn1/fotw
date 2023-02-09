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
extern int monitor(int,char **, char **);
extern void load_builtins(void);
FILE* rcfile;
extern char copy_env;
static char *s;  
static int i,n;
int debug =0;
int verbose = 0;
static int myargc;
static char **myargv;
static char linebuf[256];

#define USAGE "ephemeris [-v -h -D -q -e] [file]"
#define HELP "generates ephemerides and related things [startup file]"
#define HELP1 "-v: print version number and exit"
#define HELP2 "-h: print this helpful information and exit"
#define HELP3 "-q: quiet(er) operation"
#define HELP4 "-D turn on debugging"
#define HELP5 "-e: include os environment at startup"
#define HELP6 "-x[X]: run experimental code and continue[exit]"

int main(int argc, char **argv, char **envp) {

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
		myargc--;
		*myargv++;
	}
	else {
/* try to open .ephemeris2.rc in the home directory */
/* BUG: A really long path to the home directory could make finding
 * the rc file fail silently, but at least we won't hackers in this door */
		if(getenv("HOME"))strncpy(linebuf,getenv("HOME"),230);
		strcat(linebuf,"/.ephemeris2.rc");
		rcfile = fopen(linebuf,"r");
		/* no error message if rcfile is null because an
		 *  rc file is not required */
	}

/* load all built-in orbits */
	 load_builtins(); 

	/* Call the entry point of monitor */

	rval = monitor(argc,argv,envp);

	if(verbose)
		printf("main: monitor returned %d\n",rval);
	return rval;
}
