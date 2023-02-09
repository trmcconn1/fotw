/*
* hexswap.c
*
* prompt for short integer,swap bytes ,print out */
#include <stdio.h>
#include <stdlib.h>
#define LENGTH 65536 /*Largest unsigned short int */
#define NBYTES 5
int
main()
{
	/* data declarations */
	unsigned short int k1, k2;
	char c[NBYTES+1];

	/* get a short integer from user */
	printf("enter an integer between 0 and 65536 \n");
	gets(c);
	k1=atoi(c);

	/* print hexadecimal equivalent of number */
	printf("hexadecimal equivalent of number is %x\n", k1);

	/*swap msbytes with lsbytes*/
	k2 = k1 >>8;
	k2 = (k1 << 8)|k2;
 
	printf("hexadecimal equivalent of swapped number is %x\n", k2);

	return EXIT_SUCCESS;
}
