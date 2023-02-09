/*
* hbars.c Print a set of horizontal bars of varying lenght
*/
  
#include <stdio.h>
#include <stdlib.h>

#define STRING(n,c) while (n-- >0) putchar(c);

#define NLINES 10

int
main (void)
{	int n;
	int m;

	for (n=1; n<=NLINES;++n) {

		m = n;
		STRING(m, '*')
		putchar('\n');
	}
	return EXIT_SUCCESS;
}
		
