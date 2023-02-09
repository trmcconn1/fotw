
/* Reject the partition unless parts and distinct and each even part
is > 2#(odd parts)  */


#include "filterlib.h"


int bressoud( unsigned char (*p)[], int n ,int m)
{
	int i;
	int odd_parts = 0;
	int min_evenp = n+1;

	for(i=0;i<n;i++){
		if((*p)[i] > 1)return FALSE;
		if((*p)[i]){
			if((i+1)%2)odd_parts++;
			else if(i+1 < min_evenp)min_evenp = i+1;
		}
	}
	if(min_evenp <= 2*odd_parts)return FALSE;
	return TRUE;
}
