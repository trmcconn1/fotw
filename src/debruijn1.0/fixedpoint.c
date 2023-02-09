/* Create a seeded deBruijn sequence  of order n+1 by the Ehrenfeucht-Mycielski 
mechanism in a fixed-length buffer of size n. This is the default method in this
package for generating debruijn strings. For the theory see my pre-print
DeBruijn Strings, Double Helices, and Ehrenfeucht-Mycielski mechanism. */

#define _CRT_SECURE_NO_WARNINGS 

#include "debruijn.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Return pointer to longest suffix of p that is a prefix of q */
char *overlap(char *q, char *p)
{
	int n;
	char *r;

	if(!q)return NULL;
	if(!p)return NULL;
	n = (int)strlen(p);
	if(n==0)return p;
	r = p;
	while((r - p)<n)
		if(strstr(q,r)==q)break;
		else r++;
	return r;
}

extern int verbose;

/* Copy seed into the middle of a big buffer and grow EM_n(seed) to the
right. Whenever an n-string at the end has not occurred before, we prepend
it to the beginning of the seed. Thus the seed may grow to the left during
the algorithm. See paper for the theory on why this will always produce
a debruijn string of order n+1 beginning with 0...01 (n+1 string). We pad the
seed at the end with 000...01 to make sure we begin properly */

char *fixedpoint(char *seed, int n)
{
	int i, m,k;
	char *buf,*rval;
	char c,*p,*q,*left,*r;

	/* Handle trivialities */
	if(n < 0)return NULL;
	
	/* Allocate memory */

	m = power2(n)*n+power2(n+1)+n;
	if(seed) m += (int)strlen(seed);
	buf = (char *)malloc((m+1)*sizeof(char));
	if(buf == NULL){fprintf(stderr,"fixed point: alloc memory failed\n");return NULL;}

	/* copy the seed into the middle of the buffer */

	left = buf+power2(n)*n;
	rval = left;
	if(seed){strcpy(left,seed);rval+=strlen(seed);}

	/* put n zeros and a 1 at the end of the seed: this is the beginning
	   of the result string */

	for(i=0;i<n;i++)rval[i]='0';
	rval[i++]='1';
	rval[i]='\0'; /* properly terminate string */

	/* Do EM_n(seed) for 2^(n+1) steps */

	while(i < power2(n+1)+n){
		p=rval+i-n; /* point p at tail n-string */
		if((q=strstr(left,p))==p){  /* No previous occurrence. Prepend tail pointed to by p
				to left end of buffer, thus lengthening the seed. For efficiency, and also for
				theoretical interest, we overlap the tail as much as possible with the beginning of the existing
				seed. */
			r = overlap(left,p);
			k=(int)strlen(r);
			strncpy(left-(n-k),p,n-k);
			left = left-(n-k);
			q = left;
		}

		/* find pointer q to latest earlier occurrence */
		while((r=strstr(q+1,p))<p)q=r;

		/* get char following tail occurrence at q */	
		c = *(q+strlen(p)); /* n = strlen(p) */
		rval[i++] = COMPLEMENT( c ); /* set next digit of return */
		rval[i]='\0';
	}
	if(verbose){
		printf("Final Seed: ");
		r = left;
		while(r<rval){
			printf("%c",*r);
			r++;
		}
		printf("\n");
	}
	return rval;	/* Caller may free this */
}