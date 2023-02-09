/* selectsort.c: implementation of a selection sort algorithm.
 *
 * Selection sort is the following simple-minded algorithm: Find the
 * smallest element in the array and move it (if necessary) to position 0. Find
 * the smallest among the rest and move it to position 1, etc.  
 *
 * */

#include "global.h"

void selectsort(void *base, size_t n, size_t size,
			int (*cmp)(const void *, const void *))
{
	int i,j,k=0;

	if(n <= 1) return; /* Nothing to do */

	for(i=0;i<n;i++){ /* successive passes through the data */
		k = i;
		for(j=i;j<n;j++)
			if(cmp(base+j*size,base+k*size)==-1) k = j;
		/* k is now the index of smallest among ith...n-1th */
		if(k!=i)swap(base,i,k,size);
	}
}

