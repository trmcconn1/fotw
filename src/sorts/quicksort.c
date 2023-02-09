/* quicksort.c: implementation of C.A.R. Hoare's Quicksort algorithm.
 *
 * This is one of the most analyzed of all algorithms, and a good one
 * in practise, though by no means the best. It is a divide and conquer
 * algorithm. One begins by selecting an element of the array to be
 * sorted called the pivot. In our implementation the pivot is chosen at
 * random, but it could be taken to be some fixed element, perhaps the
 * middle. Swap the pivot with the initial datum. Make a pass through
 * the rest of the data and compare each item with the pivot. If it
 * compares less, move it to the first part of the array, from positions
 * 1 ... last. (One increases the index denoted last during the pass as
 * necessary.) Move those that compare greater above last. Then swap the
 * pivot with position last. Then call quicksort recursively on the two
 * arrays, 1 ... last-1 and last+1 ... n-1. */

#include "global.h"

static int current_depth;

void quicksort(void *base, size_t n, size_t size,
			int (*cmp)(const void *, const void *))
{
	int last=0;
	int i;

	calls++;
	current_depth++;
	if(current_depth>depth)depth = current_depth;

	if(n <= 1) return; /* nothing to do */

	/* Pick a random pivot element and swap it with the zeroth element */

	swap(base, 0, rand() % n,size);

	/* Partition elements into those <= the pivot element
	 * (in positions 1 .. last) and those larger (in positions last+1
	 * ... n-1).
	 */

	for(i=1;i<n;i++) 
		if(cmp(base,base+i*size) > 0)
			swap(base,++last,i,size);

	/* Move pivot into its proper position: last */
	swap(base,0,last,size);

	/* Now recursively sort the two partitions -- those < pivot
	 * in positions 0 .. last, and those > pivot in positions 
	 * last+1 .. n-1. Note that the pivot itself is left alone.
	 * Gotcha: if we included the pivot in either part of the partition
	 * we would loop forever. */

	quicksort(base,last,size,cmp);
	current_depth--;
	quicksort(base+(last+1)*size,n-last-1,size,cmp);
	current_depth--;
}

