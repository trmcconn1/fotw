#include<stdlib.h>
#include<stdio.h>


int
main()
{
        unsigned long int m =1;
        unsigned long int p2max;
        char *p;

        while((p = (char *)malloc(m))!= (char *)NULL){
                free(p);
                m *= 2;
        }
        p2max = m = m/2;  /* Highest power of 2 we could grab */

	for(;p2max > 1; p2max /= 2)
		if((p = (char *)malloc(m))!= NULL){
			free(p);
			m += p2max;
	}
        printf("Able to malloc at most %ul chars\n",m);
        return 1;
}

