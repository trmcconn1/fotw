/* Spew psuedo-random 0s and 1s to stdout */

#include<stdio.h>
#include<stdlib.h>


#define TERMS 100

int 
main()
{
	int seed;
	int i=0;
	char buffer[35];
	

	fprintf(stderr,"Input a positive integer to use as seed.\n");
	scanf("%d",&seed);
	srandom(seed);
	while(i<TERMS){
		sprintf(buffer,"%ld",random());
		switch(buffer[strlen(buffer)-1]){
			case '0':
			case '2':
			case '4':
			case '6':
			case '8':
		  		printf("0");
				break;
			case '1':
			case '3':
			case '5':
			case '7':
			case '9':
	          		printf("1");
		}
		i++;
		if(i%80 == 0)printf("\n");
	}
	return 0;
}
