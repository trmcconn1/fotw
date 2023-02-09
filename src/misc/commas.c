/* put_in_commas.c: group digits in threes and put in commas */ 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *commas_in( char *input);
char buf[100];

main()
{
	printf("%s\n", commas_in("1123342187590"));
}


char *commas_in( char *input)
{
	int len, ntriples, i;
	char *ptr;

	ptr = input;
	len = strlen(input);
	ntriples = len/3;

/* handle odd digits at beginning */

	strncpy(buf,input,len % 3);
	ptr = ptr + ( len % 3);

/* Put in a comma, if there is a reason to */

	if (len > 3) {
		 strcat(buf,",");
		 for(i=1;i<ntriples;i++) {
			 strncat(buf,ptr,3);
			 ptr = ptr + 3;
			 strcat(buf,",");
			}
		  strncat(buf,ptr,3);
		 }
		
	return buf;
}	
		
		 
