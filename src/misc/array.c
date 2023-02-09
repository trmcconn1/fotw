/*
/* Array.c Prints out ROWS by COLS array of form i-j
*/

#include <stdio.h>
#include <stdlib.h>

#define ROWS 92
#define COLS 10

int
main ()
{
	int i=0;
	int j=0;

	do {
		j=0;
		do	{
			 printf("%i-%i\t",i,j);
			} while (++j <= COLS-1);
		printf("\n");
	   } while (++i <= ROWS-1);

	return(EXIT_SUCCESS);
}
