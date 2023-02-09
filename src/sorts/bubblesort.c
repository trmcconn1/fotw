/* bubblesort.c: implementation of a bubble sort algorithm.
 *
 * Bubblesort works by allowing each datum to "bubble up" to its proper
 * position in the data set. We make one large pass over the data. On the
 * ith pass, we compare the bottom datum to each datum above it in turn 
 * until it compares less. We can quit when we bubble up to i down from
 * the top, because we know i data have already bubbled past the current
 * one.
 *
 * It's an n^2 algorithm, but it is very simple to implement and 
 * understand.
 *
 * */

#include "global.h"

void bubblesort(void *base, size_t n, size_t size,
			int (*cmp)(const void *, const void *))
{
	int i,j;

	if(n <= 1) return; /* Nothing to do */

	for(i=0;i<n;i++)
		for(j=0;j<n-i-1;j++)
			if(cmp(base+j*size,base+(j+1)*size)==1)
				swap(base,j,j+1,size);
}

