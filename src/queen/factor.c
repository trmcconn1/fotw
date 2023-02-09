/* factor.c: factor an integer
 *
 *  By Terry R. McConnell
 *
*/             


/* compile: cc -o factor factor.c

      Use -D_SHORT_STRINGS if your compiler does not support multiline
          string constants.

   Run factor -h for usage information.

*/


#include "global.h"
#include "factor.h"
#include "latex.h"
#include<stdio.h>
#include<stdlib.h>

#define VERSION "1.0"
#define USAGE "factor [ -t -h -v -- ]  n"
#ifndef _SHORT_STRINGS
#define HELP "\nfactor [ -t -h -v --] n\n\n\
Give the factorization into primes of the integer n\n\n\
-t: Produce latex output\n\
--: Signal end of options so that negative n can be input.\n\
-v: Print version number and exit. \n\
-h: Print this helpful information. \n\n"
#else
#define HELP USAGE
#endif

int
main(integer argc, char **argv)
{
	integer n;
	integer j=0,k;
	int verbose = 0;
	int tex = 0;

	/* Process command line */
	while(++j < argc){
		if(argv[j][0] == '-')
			switch(argv[j][1]){ 
				case '-':
					++j;
					break;
				case 'v':
					printf("%s\n",VERSION);
					exit(0);
				case 't':
					tex = 1;
					continue;
				case '?':
				case 'h':
				case 'H':
					printf("%s\n",HELP);
					exit(0);
				default:
					fprintf(stderr,"factor: unkown option %s\n",
						argv[j]);
					fprintf(stderr,"%s\n",USAGE);
					exit(1);
			}
		break;
	}

	if(j >= argc){
		fprintf(stderr,"factor: usage error.\n");
		fprintf(stderr,"%s\n",USAGE);
		return 1;
	}

	n = atoi(argv[j++]);
	pprint_factored(factor_integer(n,BRUTE_FORCE));
	printf("\n");
	return 0;

}

