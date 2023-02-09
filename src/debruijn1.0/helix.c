#define _CRT_SECURE_NO_WARNINGS
#include "debruijn.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern  char *prefer1(int);
extern char *keyword(char *, int); /*e.g.  01000 gives a double helix */
extern char *keyseq(char *, char (*)(char *),int);


/* These are essentially all the deBruijn sequences of order 4 
The rest are obtained from these 8 by either bitwise complement,
or by time reversal. If the complementary graph of the path
in the DeBruijn graph has the minimum possible 3 cycles, we
call the sequence a double helix. */

#define TESTSEQ1 "1111000010011010111"   /*double helix*/
#define TESTSEQ2 "1111000011010010111"   /*5 cycles*/
#define TESTSEQ3 "1111001000011010111"   /*double helix*/
#define TESTSEQ4 "1111001101000010111"   /*5 cycles*/
#define TESTSEQ5 "1111010000101100111"   /*5 cycles*/
#define TESTSEQ6 "1111010000110010111"   /*5 cycles*/
#define TESTSEQ7 "1111010010110000111"   /*5 cycles*/
#define TESTSEQ8 "1111010011000010111"   /*5 cycles*/

static unsigned int prefselect; /* 0-2^8 */
static char mypref(char *argstr){
	
	unsigned int i,k=0,m;
	/* Find binary value of argument string of length 3 */
	m = 1;
	for(i=0;i<3;i++){if(argstr[i]=='1')k+=m;m = 2*m;}/* k in range 0-7 */

	/* get the kth binary digit of prefselect to see what to return */
	m = prefselect;
	for(i=0;i<k;i++)m = m/2;
	if(m%2)return '1';
	return '0';
}

/* Search for key sequence double helices. Print any keys found that work */

void doublehelixkeysearch(int n){

	char buf[MAX_STRING];
	static char keybuf[MAX_LEVEL];
	char *p;
	int i,k;

/* We need to generate all possible keys of length n. Quickest way is
just to generate a deBruijn string */

	if(n > MAX_LEVEL){fprintf(stderr,"dbhsearch: level too big\n"); return;}
	p = prefer1(n);
	if(p==NULL){fprintf(stderr,"dbhsearch: prefer1 croaked\n"); return;}

	strcpy(buf,p);
	k = (int) strlen(buf);
	
	printf("Keys of order %d double helices:\n",n);

	/* loop over keys */
	for(i=0;i<k-n-1;i++){
		strncpy(keybuf,buf+i,n);
		if(countantipath(keyword(keybuf,n),n)==3)printf("%s\n",keybuf);
	}
	free(p);
	return; /* Hopefully having provided good news */

}

/* At present, this only finds all double helix cycles of order 5 */

void doublehelixsearch ( void ){
	
	unsigned int i,j;
	char *p;
	int count = 0;

	char *keylib[8] = {TESTSEQ1,TESTSEQ2,TESTSEQ3,TESTSEQ4,TESTSEQ5,TESTSEQ6,TESTSEQ7,TESTSEQ8};
/* We are using only half of the debruijn cycles of order 4, but we count
   double because only half the preference functions give distinct strings */
	for(i=0;i<8;i++)
		for(j=0;j<256;j++){
			prefselect  = j;
			p = keyseq(keylib[i],&mypref,5);
			if(p==NULL){fprintf(stderr,"dhsearch: got null pointer from keyseq i=%d,j=%d\n",i,j);return;}
			if(countantipath(p,5)==3)count++;
		}
		
	printf("Found %d order 5 double helices out of %d deBruijn sequences\n",count,8*256);
}