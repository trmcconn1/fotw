


#include "filterlib.h"


int fewerthan( unsigned char (*p)[], int n, int m )
{
	int i,c=0;
	for(i=0;i<n;i++){
		c += (*p)[i];
		if(c >= m)return FALSE;
	}
	return TRUE;
}
