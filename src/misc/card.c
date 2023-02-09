
#include<stdio.h>
#include<stdlib.h>

unsigned card(unsigned long);

int
main()
{
	unsigned long x = 55;

	printf("The card of %d is %d.\n",x,card(x));
	return 0;
}

unsigned card (unsigned long x)
{
	unsigned r = 0;
	int i;

	for(i=0;i<8*sizeof(unsigned long);i++){
		r += (x%2);
		if( (x = x >>1)==0 ) break;
	}
	return r;
}

