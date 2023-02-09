/* shellsort.c: implementation of shell sort algorithm.
 *
 * Shell sort is an interesting variant of insertion sort.
 *
 * Call an array k-sorted if all subarrays having indices in arithmetic 
 * progression k apart are sorted. 
 *
 * One chooses a decreasing sequence of k values, n > k_1 > k_2 > ...
 * > k_m = 1;
 *
 * Do insertion sort on all k_1 subarrays, then repeat with k_2, etc. 
 * We are done after m such stages, since a 1-sorted array is sorted.
 *
 * The idea is that the (expensive) inner loop in insertsort won't have
 * to run more than about the previous k number of times. 
 *
 * In practise the k's are chosen so as to decrease exponentially. The most
 * common choice among programmers appears to be (in ascending order)
 *
 * 1, 4, 13, 40, ... 
 *
 * where the j+1st term is 3k_j + 1.
 *
 */

#include "global.h"
#include <string.h>  /* For memmove */

void shellsort(void *base, size_t n, size_t size,
			int (*cmp)(const void *, const void *))
{
	int j,k,i,m;

	if(n <= 1) return; /* Nothing to do */

	/* figure out the biggest k_n to use */
	k = 1;
	while(k < n)
       		k = 3*k+1;	

	while(k > 1){ /* loop over different k values, from large to small */
		k = k/3;
		/* Loop over the k different subarrays in which indices are
		 * congruent to i (mod k) */
		for(i=0;i<k;i++){
			j=1;
			while(i+j*k <n){ /* Do an insertion sort on the ith
					   subarray */
				m = i+(j-1)*k;
				while((m>=0)&&(cmp(base+m*size,
				    base+(m+k)*size)>0)){
				    	swap(base,m,m+k,size);
				    	m -=k;
				}
				j++;
			 } /* Done insertion sort of ith sub-array */
		}/* Done loop over subarrays */
	}/* Done loop over k */
	return;
}

