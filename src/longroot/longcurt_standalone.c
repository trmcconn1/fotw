/* longcurt.c: program to print out examples of the by-hand cube root
*  algorithm.
*
* By Terry R. McConnell 12/2003
*
* Usage: longcurt <radicand> <digits>
* where radicand is a decimal number and digits is a natural number
*
* Arguments can be arbitrarily long, subject only to the size of
* ARG_MAX on your system. POSIX.1 ensures that this is at least 4096 bytes.
*
*/

#include<stdio.h>
#include<stdlib.h>

#define USAGE "longcurt [-hv --] <radicand> <digits>"
#define PROGNAME "longcurt"
#define VERSION "1.0"

#define HELP "\n\nlongcurt [-hv --] <radicand> <digits>\n\n\
-h: print this helpful information and exit.\n\
-v: print version number and exit.\n\
--: signal end of options. Needed to give negative radicand.\n\n\
Print listing of by-hand calculation of cube root(radicand) to number of digits."

extern int longcurt(char *,char *);

int 
main(int argc, char **argv)
{

	int i=1,options_on=1;

	/* Process any options */
	while(options_on && i < argc && argv[i][0]=='-'){
		switch(argv[i][1]){
			case '-':
				options_on = 0;
				break;
			case 'v':
				printf("%s\n",VERSION);
				return 0;
			case 'h':
				printf("%s\n",HELP);
				return 0;
				break;
			default:
				fprintf(stderr,"%s:%s\n",PROGNAME,USAGE);
				fprintf(stderr,"No such option: %s.\n",
						argv[i]+1);
				return 1;

		}
		i++;
	}

	/* If we got here we should have two more arguments */

	if( argc - i < 2 ) {

		fprintf(stderr,"%s: Usage: %s\n\n",PROGNAME,USAGE);
		exit(1);

	}

	return longcurt(argv[i],argv[i+1]);
}
