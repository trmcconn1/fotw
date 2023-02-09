/* data.c: implement routines that generate test data sets */

#include "global.h"

/* Return pointer to the first element of an array of random integers of
 * length n. (Or null if it cannot be constructed)
 */

static int seeded = FALSE;

int *random_int_data(int n, int low, int hi, int res, long seed)
{
	int *rval;
	int i;

	if(seeded != TRUE){
		srandom(seed);
		seeded = TRUE;
	}

	if(low >= hi) return NULL;
	if(res < 1)return NULL;
	rval = (int *)malloc(n*sizeof(int));
	if(!rval) return NULL;

	for(i=0;i<n;i++)
		*(rval + i) = low + ((random()%(hi-low))/res)*res;

	return rval;
}	
