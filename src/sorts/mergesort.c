/* mergesort.c: implementation of a recursive merge sort algorithm.
 *
 *
 * Mergesort is a divide and conquer strategy. First one divides the
 * data in two halves, and then one merge sorts each half. The resulting
 * sorted halves can be merged together in one pass.
 *
 * Merge sort takes NlogN time in the worst case. */

#include "global.h"
#include <string.h>

static int current_depth;
void merge(void *base1, void *base2, size_t n1, size_t n2,size_t size,
		int (*cmp)(const void *, const void *));

void _mergesort(void *base, size_t n, size_t size,
			int (*cmp)(const void *, const void *))
{
	int m;

	calls++;
	current_depth++;
	if(current_depth>depth)depth = current_depth;

	if(n <= 1) return; /* nothing to do */

	m = n/2;
	_mergesort(base,m,size,cmp);
	current_depth--;
	_mergesort(base+m*size,n-m,size,cmp);
	current_depth--;
	merge(base,base+m*size,m,n-m,size,cmp); 
}

/* Merge sorted data of size n (sorted according to cmp comparison) in
 * arrays pointed to by base1 and base2 with given sizes. 
 *
 * NB: The answer is stored in the array pointed to by base1, so it must
 * be big enough to hold the the result. Normally, base1 would point to
 * some big array, and base2 would point somewhere inside it.
 */

void merge(void *base1, void *base2, size_t n1, size_t n2,size_t size,
		int (*cmp)(const void *, const void *))
{

	int i,k,l;
	void *p;

	/* Create temporary array to merge into */
	p = malloc((n1+n2)*size);
	if(!p){
		fprintf(stderr,"merge: We are toast. Cannot allocate temporary array\n");
		exit(1);
	}

	/* Do the merge */
	k = l = i = 0;
	while(i<n1+n2){
		if((k>=n1)||((l<n2)&&(cmp(base1+k*size,base2+l*size) == 1)))
			memmove(p+i*size,base2+(l++*size),size);
		else 
			memmove(p+i*size,base1+(k++*size),size);
		i++;
	}

	/* Move sorted stuff from temporary storage back to original array
	 * and clean up */

	for(i=0;i<n1+n2;i++)
		memmove(base1+i*size,p+i*size,size);
	free(p);
}
