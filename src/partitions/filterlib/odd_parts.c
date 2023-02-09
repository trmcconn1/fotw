


#include "filterlib.h"


int odd_parts( unsigned char (*p)[], int n, int m )
{
	int i;
	for(i=0;i<n;i++){
		if(!((*p)[i]))continue;
		if(!((i+1) % 2))return FALSE;
	}
	return TRUE;
}
