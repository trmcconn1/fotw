/* heapsort.c: implementation of a basic heap sort algorithm.
 *
 * Heap sort is similar to insertion sort in that there is an initial
 * pass through the data in which each datum is placed in a position in
 * a certain sorted structure. The difference is that the structure is
 * not a linear list, but rather a
 * a binary tree with the property that the value of each node is at least as
 * large as the values of its two children. Once the pass is completed, the
 * tree can be quickly flattened into a sorted list.
 *
 * Call a tree a max heap if each node's value is greater than or equal to
 * the values of its two children.
 *
 * */

#include "global.h"
#include <string.h>  /* For memmove */

#define LEFT(i) (2*(i))
#define RIGHT(i) (2*(i)+1)

/* The left and right children are assumed to be roots of max heaps, but
 * the root node may not be in the right position. Move it down into the
 * tree into the correct position. */

void max_heapify(void *base, int n, int i, int size, 
		int (*cmp)(const void *,const void *))
{
	int r,l,max;

	r = RIGHT(i);
	l = LEFT(i);

	if((l < n)&&(cmp(base+l*size,base+i*size)==1))
		max = l;
	else max = i;
	if((r < n)&&(cmp(base+r*size,base+max*size)==1))
		max = r;
	if(max != i) { 
		swap(base,max,i,size);
		max_heapify(base,n,max,size,cmp);
	}
}

void _heapsort(void *base, size_t n, size_t size,
			int (*cmp)(const void *, const void *))
{
	int i;

	if(n <= 1) return; /* Nothing to do */


	/* Make our data into a max heap */
	for(i=n/2;i>=0;i--)
		max_heapify(base,n,i,size,cmp);
	

	for(i=n;i>1;i--){
		/* peel top off tree and store at end coming down */
		/* Re-max the tree, but one fewer data left each time. */
		swap(base,0,i-1,size);
		max_heapify(base,i-1,0,size,cmp);
	}
	return;

}

