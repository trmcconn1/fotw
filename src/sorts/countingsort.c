/* Counting sort.c: counting sort relies on having data in a fixed
 * range which is automatic in this test platform setting. Beware, though,
 * that if this method is adapted to data "in the wild" the data will
 * need to be preconditioned with a hash function that maps the actual
 * date to integers in the range [lo...hi] in steps of size res. 
 *
 * The idea of the algorithm is simple: For each value k in the range
 * we maintain a counter C[k]. During an initial pass through the data
 * we record in C[k] how many data values are less than or equal to k. A
 * second (and final) pass uses the counter as an index into the output
 * array. Thus, the largest value goes at the largest index, as it should.
 * The only tricky thing is that we must handle ties. 
 *
 * We follow here the algorithm outline from Cormen, Leisersen, Rivest, and
 * Stein's book.
 *
 */

#include "global.h"

/* BUG: We are ignoring res */

extern int lo,hi,res;  /* these are defined in testsort.c. */ 

/* Routine that does the work, called by the wrapper below */

void mycsort(int *b, size_t n)
{

	int *c,*a,i=0,k,K;

	/* allocate memory for the counter array and recorded input */

	K = (hi-lo + 1);
	c = calloc(K,sizeof(int)); /* zero-ed memory */
	if(!c)return;
	a = (int *)malloc(n*sizeof(int));
	if(!a)return;

	while(i<n){
		k = *(b + i) - lo;
		c[k]++; 
		*(a + i) = k+lo; /* remember original values */
		i++;
	}

	/* c[k] now contains the count of items equal to k+lo */

/* Accumulate partial sums of the c array so it gets the number less
 * than or equal to each k */

	for(i=1;i<=K;i++)c[i] += c[i-1];

/* Write stored values back to the input in order of how many items were
 * less than or equal to them. Decrementing the counter along the way
 * takes care of ties, i.e., we'll get a block of equal values. */

	for(i=n-1;i>=0;i--){ /* Back from the end of b, in decr. order */
		k = *(a+i) - lo;
		*(b+c[k]-1) = k+lo;
		if(i!=(c[k]-1))swaps++; /* for profiling */
		c[k]--;
	}

	/* This seemingly more intuitive approach gives a sort that
	 *  is unstable: it reverses data that hashes equal */
#if 0
	for(i=0;i<n;i++){ /* Back from the end of b, in decr. order */
		k = *(a+i) - lo;
		*(b+c[k]-1) = k+lo;
		if(i!=(c[k]-1))swaps++; /* for profiling */
		c[k]--;
	}
#endif
}


/* Basically just a "forgetful functor"  */

void countingsort(void *b, size_t n, size_t s, int (*cmp)(const void *,const void *))
{
	mycsort((int *)b, n);
	
}

