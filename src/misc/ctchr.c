/*
* ctchr.c    count chars entered before ^D 
*/
 
#include <stdio.h>
#include <stdlib.h>

int
main (void)
{
        int n;
	char c;
	
	printf("beginning char count, q to quit\n"); /* freindly ms */

	 
	

	n=0; /* initialize count */

	while (1) {      /* begin polling loop */

		  c = getchar();

		  if (c !='') {
			n +=1;  /* increment count if char input */
                        continue;}
		  else  break;
		
		 
                   } /* end while */
	    
	printf("\n Character count = %i\n",n);

	return(EXIT_SUCCESS);
}
