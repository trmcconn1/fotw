/* mseq: generate terms of the m-sequence */
/*        This is an interesting psuedo-random sequence. 
		For a description of the alogorithm see

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NTERMS 200000
#define MAXLEN 200001    /* reserved storage for sequence */

char buf[MAXLEN];

/* The sequence is reversed in order to use library functions */

char *preface( char c, char *s);

main()
{
	char m[MAXLEN];  /* The m-sequence */
	char ms[MAXLEN];  /* Initial string to be matched */
	char nxt;
	int ml = 0;  /* Length of longest example string */
	int lkbk = 0;
	int count, j;
	char *found;  /* pointer to example string */

	m[0] = '0';    /* Initial value of m-sequence */
	m[1] = '\0';

	printf("n\tm[n]\tmlen\tlkbk\n"); /* Title line */

	for(count = 0; count < NTERMS; count++)
	{
		printf("%d\t%c\t%d\t%d\n",count+1, m[0],ml,lkbk);
		
	/* Search for longest example string. Strings longer than ml + 1
	   need not be sought */

		for( j = ml+1;j > 0; j--)
		{
			ms[j]='\0';
			strncpy(ms,m,j); /* initial string into ms */

	/* See whether stored initial string occurs earlier */
	/* Store pointer to latest such occurence */

			if((found=strstr(&m[1],ms))!= NULL)
				break;
		}
		if(j==0)   /* use special startup rule  */
			nxt = ( m[0] == '0' ? '1' : '0' );
		else

	/* Do opposite of what followed found example string */
	
			nxt = ( *--found == '0' ? '1' : '0' );
		ml = j;
		preface(nxt, m); 
		lkbk = ( found == NULL ? 0 : found - m + 1) ;
	}
} 


/* preface: preface a string with a given character */

char *preface( char c, char *s)
{
	int i = 0;
	char *ptr;

	ptr = s;
	buf[0] = c;
	while((buf[i+1] = *s++) != '\0')
		i++;

	return strcpy(ptr,buf);
}
