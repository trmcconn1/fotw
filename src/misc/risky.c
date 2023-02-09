/* risky.c: by Vince Fatica */

#include <stdio.h>
#include <time.h>
#include "mersenne.h"

#define TIMES 10000000

#define max(x,y) ((x>y) ? (x) : (y))
#define min(x,y) ((x>y) ? (y) : (x))

int main ( void ) {

	int a[3],b[2],A[2],B[2], hi;

	unsigned long AA = 0, BB = 0, AB = 0, BA = 0, i, j, k, timer;

	sgenrand(time(&timer));

	for (k=0; k<TIMES; k++) {
		a[0]=(genrand()%6);
		a[1]=(genrand()%6);
		a[2]=(genrand()%6);
		b[0]=(genrand()%6);
		b[1]=(genrand()%6);

		for ( hi=-1,i=0; i<3; i++ ) if ( a[i] > hi ) {
			j = i;
			hi = a[i];
		}
		A[0] = a[j];
		A[1] = max( a[(j+1)%3], a[(j+2)%3] );

		B[0] = max(b[0],b[1]);
		B[1] = min(b[0],b[1]);

		if ( A[0] > B[0] ) {
			if ( A[1] > B[1] ) AA++;
			else AB++;
		}
		else {
			if ( A[1] > B[1] ) BA++;
			else BB++;
		}
	}

	printf("AA = %5.5f\nBB = %5.5f\nAB = %5.5f\nBA = %5.5f\n",
	   (float) AA/TIMES, (float) BB/TIMES,
	   (float) AB/TIMES, (float) BA/TIMES);

	return 0;

}

