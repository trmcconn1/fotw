/* longsqrt.c: main program to print out examples of the by-hand square root
*  algorithm. (The guts is in longsqrt.c)
*
* By Terry R. McConnell 12/2003
*
* Usage: longsqrt <radicand> <digits>
* where radicand is a decimal number and digits is a natural number
*
* Arguments can be arbitrarily long, subject only to the size of
* ARG_MAX on your system. POSIX.1 ensures that this is at least 4096 bytes.
*
*/

#include<stdlib.h>
#include<stdio.h>

#define USAGE "longsqrt [-hv] <radicand> <no_digits>"
#define PROGNAME "longsqrt"
#define VERSION "1.0"

#define HELP "\n\nlongsqrt [-hv] <radicand> <no_digits>\n\n\
-h: print this helpful information and exit.\n\
-v: print version number and exit.\n\n\
Print listing of by-hand calculation of sqrt(radicand) to number of digits."

extern int longsqrt( char *,  char *);

int 
main(int argc, char **argv)
{

	int i=1;

	/* Process any options */
	while(i < argc && argv[i][0]=='-'){
		switch(argv[i][1]){
			case 'v':
				printf("%s\n",VERSION);
				return 0;
			case 'h':
				printf("%s\n",HELP);
				return 0;
				break;
			default:
				fprintf(stderr,"%s:%s\n",PROGNAME,USAGE);
				fprintf(stderr,"radicand must be nonnegative.\n");
				return 1;

		}
		i++;
	}

	/* If we got here we should have two more arguments */

	if( argc - i < 2 ) {

		fprintf(stderr,"%s: Usage: %s\n\n",PROGNAME,USAGE);
		exit(1);

	}

	return longsqrt(argv[i],argv[i+1]);
}
