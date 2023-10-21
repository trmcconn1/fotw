/* Test whether a number is a perfect square using only integer arithmetic */

/* unsigned char is_square(unsigned long n): returns 1 if n is a perfect
square, otherwise 0 */

/* Compile with -D__MAIN__ to include main testing routine below */



#include<stdio.h>
#include<stdlib.h>


#define TRUE 1
#define FALSE 0


unsigned char is_square(unsigned long n)
{

	unsigned long lo,hi = 1,x =1;

	if(n == 1)return TRUE;

/* Bracket the square root of n with a quick and dirty doubling search */

	while (x <= n){ x *= 4; hi *= 2;}
	lo = hi >> 1;

/* Do a binary search for the square root of n >=  lo and  < hi */

	while(TRUE){

		if(lo*lo == n) return TRUE;
		if(hi - lo <= 1)return FALSE;

		if( lo*lo + 2*lo*hi + hi*hi > 4*n ) hi = (hi+lo)/2;
		else lo = (hi + lo)/2;
	}
}

#ifdef __MAIN__

int main(int argc, char **argv){

	unsigned long n;

	for(n=1;n<=100;n++)
		if(is_square(n)) printf("%lu\n",n);
}

#endif
