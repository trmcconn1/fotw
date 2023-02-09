/* Reject partition unless all parts are congruent to either 1 or 4 modulo 5*/


#include "fillib.h"


int _1or4mod5( unsigned char (*p)[], int n, int m )
{
	int i,k;
	for(i=0;i<n;i++){
		if(!((*p)[i]))continue;
		k = (i+1)%5;
		k = (k*k)%5;
		if(k != 1)return FALSE;
	}
	return TRUE;
}
