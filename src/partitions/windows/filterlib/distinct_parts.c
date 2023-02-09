
/* No two parts the same */


#include "filterlib.h"


int distinct_parts( unsigned char (*p)[], int n ,int m)
{
	int i;
	for(i=0;i<n;i++)
		if((*p)[i] > 1)return FALSE;
	return TRUE;
}
