/* insertsort.c: implementation of an insertion sort algorithm.
 *
 * The insert sort is the most common sort routine used by humans to
 * sort small data sets. (I use it every time I sort a bunch of test
 * papers.) The method is as follows: you maintain a sorted set (empty at
 * first) and make one pass through the data. The first datum goes into
 * the sorted set. After that, each new datum is removed from the data
 * set and placed in the proper position in the sorted set. Its chief
 * virtue is that it can implemented so as to sort the data in place.
 * (It is very slow: n^2 time.)
 *
 * See shellsort.c for an interesting, and much more efficient, variant */

#include "global.h"

void insertsort(void *base, size_t n, size_t size,
			int (*cmp)(const void *, const void *))
{
	int j,sorted = 1;

	if(n <= 1) return; /* Nothing to do */

	while(sorted<n) { /* sorted is the number in the sorted part of the
			   array, from base...base+sorted-1 */

		/* Swap next item to be sorted with the first one (going down)
		 * in the already sorted array that exceeds it */

		j = sorted-1;
		while((j>=0)&&(cmp(base+j*size,base+(j+1)*size)>0)){
			swap(base,j,j+1,size);
			j--;
		}
		sorted++;
	}
	return;

}

