


#include "fillib.h"


int lessthan( unsigned char (*p)[], int n, int m )
{
	int i;
	if(m<=0)return FALSE;
	for(i=m-1;i<n;i++){
		if((*p)[i])return FALSE;
	}
	return TRUE;
}
