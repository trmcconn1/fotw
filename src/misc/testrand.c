/* Print 100 random numbers on [0,1] to stdout using the lousy rand()
   RNG
*/


#include<stdlib.h>
#include<stdio.h>

int
main()
{
	int i,n = 100;

	srand(3146); /* seed RNG */

	for(i=1;i<=n;i++) {
		printf("%g\n", (double)rand()/((double) RAND_MAX));
	}
	return 0;
}
