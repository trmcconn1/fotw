/* sortargs: sort command line args in numeric order */

/* This illustrates the usage of qsort.c */

#include <stdio.h>
#include <stdlib.h>

extern void krqsort( void *v[], int, int, int (*comp) (void *, void *));
int my_comp( char *, char * );

int
main (int argc, char *argv[])
{
	int i;
 
 for (i = 1; i < argc; i++)
                printf("%d\n",atoi(argv[i]));

	krqsort( (void **)argv, 1, argc -1, (int (*)(void *,void *))my_comp);

	for (i = 1; i < argc; i++)
		printf("%d\n",atoi(argv[i]));
	return 0;
}	


int
my_comp( char *A, char *B)
{
	int a,b;
	a = atoi(A);
	b = atoi(B);
	if ( a == b ) return 0;
	return (a < b ? -1 : 1);
}

