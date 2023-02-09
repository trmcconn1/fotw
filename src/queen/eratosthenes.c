/* eratotshtnes.c: Find the prime numbers up to n by the sieve of
 * eratosthenes.
 *
 *  By Terry R. McConnell
 *
*/             


/* compile: cc -o eratosthenes  eratosthenes.c

      Use -D_SHORT_STRINGS if your compiler does not support multiline
          string constants.

   Run eratosthenes -h for usage information.

*/


#include "global.h"
#include<stdio.h>
#include<stdlib.h>

#define VERSION "1.0"
#define USAGE "eratosthenes [ -h -v -V -c -n -t <m> ] n"
#ifndef _SHORT_STRINGS
#define HELP "\neratosthenes [ -h -v -V -c -n -t <m>] n\n\n\
Print all primes in the range 1...n. Uses eponymous sieve. \n\n\
-t: Print in ascii m column tabular format\n\
-n: Give the number of primes only.\n\
-V: More verbose output.\n\
-c: Print a c language array containing the primes.\n\
-v: Print version number and exit. \n\
-h: Print this helpful information. \n\n"
#else
#define HELP USAGE
#endif


#ifdef MAIN
int
main(integer argc, char **argv)
{
	integer n,i,k;
	integer  *array;
	int do_c = 0;
	int cols;
	int do_table = 0;
	int verbose = 0;
	int do_number = 0;
	integer j=0,m=2;

	/* Process command line */
	while(++j < argc){
		if(argv[j][0] == '-')
			switch(argv[j][1]){ 
				case 'v':
					printf("%s\n",VERSION);
					exit(0);
				case '?':
				case 'h':
				case 'H':
					printf("%s\n",HELP);
					exit(0);
				case 'V':
					verbose = 1;
					continue;
				case 'c':
					do_c = 1;
					continue;
				case 'n':
					do_number = 1;
					continue;
				case 't':
					do_table = 1;
					++j;
					cols = atoi(argv[j]);
					continue;
				default:
					fprintf(stderr,"eratosthenes: unkown option %s\n",
						argv[j]);
					fprintf(stderr,"%s\n",USAGE);
					exit(1);
			}
		break;
	}

	if(j >= argc){
		fprintf(stderr,"eratosthenes: usage error.\n");
		fprintf(stderr,"%s\n",USAGE);
		return 1;
	}
	n = atoi(argv[j++]);

	if(n<=0) {
		fprintf(stderr,"eratosthenes: n must be postive integer.\n");
		return 1;
	}
	array = (integer *) calloc((n+1),sizeof(integer));
	if(array == NULL){
		fprintf(stderr,"eratosthenes: failed to calloc memory.\n");
		return 1;
	}

	/* Do the sieve: mark non primes with 1 */

	while(m*2 <= n){
		i = 2;
		while(i*m <= n) {
			array[i*m] = 1;
			i++;
		}
		m++;
		while(array[m]==1)m++; /* skip looking at multiples of
                                          non primes */
	}

	/* Count up how many primes */

	k=0;
	for(i=2;i<=n;i++)
		if(array[i] == 0)k++;

	/* Print number of primes only */
	if(do_number){
		if(verbose)printf("The number of primes less than or equal to %ld is %ld.\n",n,k);
		else
		printf("%ld\n",k);
		return 0;
	}

	if(do_c){
		printf("integer primes[%ld] = {",k);
		for(i=2;i<=n;i++)
			if(array[i]==0)printf("%ld,\n",i);
		printf("};\n");
		return 0;
	}

	if(verbose)printf("The %ld prime numbers less than or equal to %ld:\n\n",k,n);

	if(do_table){
		j = 1;
		i = 2;
		while(i<=n){
			if(array[i]==0){
				printf("%ld",i);
				if(j >= cols){
					j = 0;
					printf("\n");
				}	
				else printf("\t");
				j++;
			}
			i++;
		}
		return 0;
	}	
	for(i=2;i<=n;i++)
		if(array[i] == 0)printf("%ld\n",i);
			
	printf("\n");
	return 0;

}
#endif
