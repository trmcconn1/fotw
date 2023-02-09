/*
/* Example1.c  Prints solution of difference equation
*  in example1.text  in ~/mat295
*/

#include <stdio.h>
#include <stdlib.h>

#define NSTEPS 20
#define INITIAL 100.0

int
main ()
{
	int i=1;
        float s=INITIAL;

        printf("\tmonth\tamount\n");

	do {
             s = 0.25*s +100;
             printf("\t %d\t%f\n",i,s);
	   } while (++i <= NSTEPS-1);


	return(EXIT_SUCCESS);
}
