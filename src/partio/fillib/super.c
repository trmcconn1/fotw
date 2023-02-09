


#include "fillib.h"


int super_distinct_parts( unsigned char (*p)[], int n, int m )
{
	int i;
	for(i=0;i<n-1;i++){
		if((*p)[i]>1) return FALSE;
		if((*p)[i] == 1)
			if((*p)[i++ +1]>0) return FALSE;
	}
	return TRUE;
}
