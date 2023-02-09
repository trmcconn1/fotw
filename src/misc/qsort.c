/* This is the implementation of C.A.R. Hoare's quicksort algorthm
	taken from K&R. 

usage: if array[] is an array of some kind of thingies,
       and comp(thingie *a, thingie *b) returns -1, 0, or 1 according
       as a < b, a==b, or a > b in some ordering it imposes on
       thingies, then

       krqsort( array, int left, int right, &comp ) 

       sorts array in order, using initial choices left and right
       in the algorithm.

	Declare this guy in your program as

	void krqsort ( void *arrayname[], int, int,
	 int (*functionname)(void *, void *));

	see sortargs.c for an example using this function

*/

void krqsort( void *v[],int left, int right, int (*comp)(void *,void *))
/*                                                ^
                                                  |
                                            pointer to a function of
                                            two generic pointers,
                                            returning int. */
{
	int i,last;
	void swap(void *v[], int, int );  /* private swap function */

	if( left >= right ) /* do nothing if array contains fewer
                               than 2 elements */	
	return;
	swap(v, left, (left + right)/2);
	last = left;
	for ( i= left +1;i <= right; i++)
		if((*comp)(v[i],v[left]) < 0)
			swap(v,++last,i);
	swap(v,left,last);
	krqsort(v, left, last -1,comp);
	krqsort(v,last+1,right,comp);
}

void swap(void *v[],int i, int j)
{
	void *temp;

	temp = v[i];
	v[i] = v[j];
	v[j] = temp;
}


