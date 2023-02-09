/* Generate deBruijn sequences by various algorithms. The comments refer to two references:
   S.W. Golomb, Shift Register Sequences, Aegean Park Press, Willow Creek, California, 1982.
   H. Fredrickson, A Survey of Non-linear Full Length Shift Register Cycle Algorithms, SIAM Review 24(1982),
   195-221. 
   The default algorithm is implemented in fixedpoint.c */

#define _CRT_SECURE_NO_WARNINGS
#include "debruijn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct node *nodearray[MAX_STRING];
static char buffer[MAX_LEVEL]; /* scratch area */

/* shift contents of character buffer one left, with c 
   entering from the right. Returns character lost on left,
   or -1 if there is an error */

int shiftin(char *buf, int level, char c){

	int i;
	char cc;

	if((buf==NULL)||(level <= 0)||(level > MAX_LEVEL))return -1;
	cc = buf[0];
	for(i=0;i<level-1;i++)buf[i]=buf[i+1];
	buf[level-1]=c;
	buf[level]='\0';
	return cc;
}

/* Create a debruijn sequence of the given order by the prefer 1 algorithm. Start with
000...01 (n digits). The next digit is a 1 unless that would cause a repeated n string.
Otherwise, it is a zero. */

char *prefer1(int level){

	int ncount = 0;
	int n; /* length of the deBruijn string */
	char *rval;
	int i,j=0,found;
	struct node *anode;
	char temp[MAX_LEVEL];

	if((level <= 0)||(level > MAX_LEVEL))return NULL;

	/* allocate memory for result string */
	n = power2(level) + level - 1;
	rval = (char *)malloc((n+1)*sizeof(char));
	if(rval==NULL)return rval;

	/* initialize buffer to 000...1 */
	for(i=0;i<level-1;i++)buffer[i]='0';
	buffer[i++]='1';
	buffer[i]='\0';
	strcpy(rval,buffer); /* i indexes next digit to be found in rval */

	/* Main loop */
	while(i<n){
		anode = (nodearray[ncount++] = allocnode(buffer,level));
		if(anode==NULL){fprintf(stderr,"prefer1: allocnode failed\n");break;}
		strcpy(temp,buffer);
		/* see if we can shift in a 1 */
		shiftin(temp,level,'1');
		found=0;
		for(j=0;j<ncount;j++)if(strcmp(nodearray[j]->name,temp)==0)found = 1;
		/* BUG: replace this linear search with a binary one */
		if(found){shiftin(buffer,level,'0');rval[i++]='0';}
		else {shiftin(buffer,level,'1');rval[i++]='1';}
	}
	/* properly terminate for return, and clean up */
	rval[i]='\0';
	for(j=0;j<ncount;j++)freenode(nodearray[j]);
	for(j=0;j<MAX_LEVEL;j++)buffer[j]='\0';
	return rval;
}

/* Create a DeBruijn sequence using the Key Word Theorem (in Golomb's
   terminology. See p133 of Shift Register Sequences) The idea is to start from 
   a sequence that differs from the key in the last digit, and then try to stay away
   from the key for as long as possible. That is, find the longest suffix of the current
   sequence that is a prefix of the key, and then spoil the party by taking as next digit
   the opposite of the next digit in the key. Provided, of course, this conforms with
   the PRIME DIRECTIVE not to repeat any n string. The prefer 1 algorithm is just key word
   with an all zero key. */

char *keyword(char *key,int level){

	int ncount = 0;
	int n; /* length of the deBruijn string */
	char *rval,c;
	int i,j=0,found;
	struct node *anode;
	char temp[MAX_LEVEL]; 

	if((level <= 0)||(level > MAX_LEVEL)||(key == NULL))return NULL;
	if(strlen(key) != level){fprintf(stderr,"keyword: key length mismatch\n");return NULL;}

	/* allocate memory for result string */
	n = power2(level) + level - 1;
	rval = (char *)malloc((n+1)*sizeof(char));
	if(rval==NULL)return rval;

	/* initialize buffer using key */
	for(j=0;j<level-1;j++)buffer[j]=key[j];
	buffer[j++] = COMPLEMENT( key[level-1] );
	buffer[j] = '\0';
	i = (int)strlen(buffer);
	strcpy(rval,buffer); /* i indexes next digit to be entered in rval */

	/* Main loop */
	while(i<n){
		anode = (nodearray[ncount++] = allocnode(buffer,level));
		if(anode==NULL){fprintf(stderr,"keyword: allocnode failed\n");break;}
		strcpy(temp,buffer);

		/* Here is the crux: we try to shift in the digit that keeps us 
		   from making progress towards repeating the key. KWT ensures that
		   this will succeed */

		c = COMPLEMENT( key[0] );
		for(j=1;j<level;j++)
			if(strstr(key,temp+j)==key){c = COMPLEMENT( key[level-j] );break;}

		/* see if we can shift in a c */

		shiftin(temp,level,c);
		found=0;
		for(j=0;j<ncount;j++)if(strcmp(nodearray[j]->name,temp)==0)found = 1;
		/* BUG: replace this linear search with a binary one */
		if(found){shiftin(buffer,level,COMPLEMENT(c));rval[i++]=COMPLEMENT(c);}
		else {shiftin(buffer,level,c);rval[i++]=c;}
	}
	/* properly terminate for return, and clean up */
	rval[i]='\0';
	for(j=0;j<ncount;j++)freenode(nodearray[j]);
	for(j=0;j<MAX_LEVEL;j++)buffer[j]='\0';
	return rval;
}


/* Create a debruijn string using the key sequence theorem. See 
   p. 134 of Golomb for the theory. It needs to be passed a pointer to
   a debruijn string of the next lower order, and a pointer to a preference
   function that returns '1' or '0' given a string of length = level-2. The 
   preference function is used 1/4 of the time, and the debruijn string output is
   uniquely determined by a key string and a preference function. This determines the
   number of possible debruijn strings recursively (De Bruijn's Theorem). 
   
   Here is more on the algorithm: call an n-1 string s = xt, where t is an n-2 string
   and x is a binary digit, EARLY if s occurs before (1-x)t in the key sequence. Otherwise
   s is LATE. Then there are 4 cases:

	  First occurrence of s:
		subcase i:  s is early
			choose the next digit arbitrarily (here: subject to the passed preference function )
		subcase ii: s is late
			choose the next digit opposite the follower of the unique occurence of s in the key sequence.
			
	  Second occurrence of s:
		conform with the PRIME DIRECTIVE 
		
	Personally, I find the description of proof of this algorithm in Golomb almost impenetrable. It is 
	simpler, I think, to view it as a variant of Algorithm 1 from Fredrickson's paper based on rooted 
	trees: The key sequence induces a "tree" in the debruijn graph of order n that is all "trunk" and no branches. For
	each node not on the trunk (i.e, n string that does not occur in the key), arbitrarily attach it to the trunk or to
	its other follower that is not on the trunk. Stop when you reach the trunk. Then follow Fredrickson's algorithm
	for the resulting rooted tree. */

char *keyseq( char *key, char (*pref)( char * ), int level){

	int m,i,j,k;
	char c,*rval,*p,*q;
	char buf[MAX_LEVEL];
	char buf2[MAX_LEVEL];
	char temp[MAX_LEVEL];

	/* sanity tests */
	if(key == NULL)return NULL;
	if(!isdebruijn(key,level-1)){fprintf(stderr,"keyseq:passed non debruijn key\n");return NULL;}
	if(level > MAX_LEVEL){fprintf(stderr,"keyseq: level too big\n"); return NULL;}

	/* Allocate memory for the return value */
	m = power2(level)+level-1;
	rval = (char *)calloc((m+1),sizeof(char));
	if(rval == NULL){fprintf(stderr,"keyseq: unable to malloc\n");return NULL;}

	/* To get the ball rolling, copy the first level-1 characters from the key 
	    string to the result string */

	for(i=0;i<level-1;i++){rval[i]=key[i];}

	/* Main loop: generate char at index position i in return string */
	while(i<m){
		/* copy the level-2 tail of the return string to the first buffer */
		k=0;
		for(j=i-(level-2);j<i;j++)buf[k++]=rval[j];
		buf[k]='\0';

		/* copy the level-1 tail of the return string to the 2nd buffer */
		k=0;
		for(j=i+1-level;j<i;j++)buf2[k++]=rval[j];
		buf2[k]='\0';

		/* Is this the first appearance of the first buffer string in the key, 
		   or the second? If first, we determine the digit to try using the
		   passed-in preference function. If 2nd, we must prefer the opposite
		   of the next key character after the (unique) appearance of the string
		   in the second buffer */

		p = strstr(key,buf);
		q = strstr(key,buf2);
		if(p==q+1){c = pref(buf);}
		else { 
			if(q+level-1 - key == strlen(key)) c = COMPLEMENT(key[level-2]); /* wrap-around */
			else c = COMPLEMENT(*(q + level - 1));
		}
		strcpy(temp,buf2);
		temp[level-1]=c;
		temp[level]='\0';
		if(strstr(rval,temp)){
			rval[i++]= COMPLEMENT( c );
		}
		else {
			rval[i++] = c;
		}
	}
	rval[i] = '\0';
	return rval;
}


/* Create a debruijn string by the "prefer same" algorithm. A sketch of the proof
of the algorithm is given in Fredrickson's paper. The algorithm is greedy, in that, at each moment 
it seeks to create the longest possible run of 0s (or 1s), with zeros preferred
before ones - subject, as always, to non-violation of the prime directive.  */

char *prefer_same(int level)
{
	int maxruns[MAX_LEVEL];/* There must be exactly 2^(n-i-2) runs of each type of
						   length i. There must be 1 of length n, and none of n-1. The
						   reason is that a run of i 0s can be tagged as t00...001 
						   where t can vary over all binary strings of length n-i-1 
						   ending in 1 */
	int runs0[MAX_LEVEL];
	int runs1[MAX_LEVEL]; /* runs of each length accumulated so far */
	char last = '1'; /* preferred choice of digit, if possible */
	int currentrun=0; /* length of run ongoing */
	char *rval;
	int i,j,n;
	char buffer[MAX_LEVEL+1];

	if((level <= 0)|(level > MAX_LEVEL))return NULL;

	/* allocate memory for result string */
	n = power2(level) + level - 1;
	rval = (char *)malloc((n+1)*sizeof(char));
	if(rval==NULL)return rval;

	/* initialize result to 000...01....1 */
	for(i=0;i<level;i++)buffer[i]='0';
	buffer[i]='\0';
	strcpy(rval,buffer);
	for(i=0;i<level;i++)buffer[i]='1';
	buffer[i]='\0';
	strcat(rval,buffer); /* i now indexes next digit in rval to be found */

	/* Initialize run count machinary */
	
	for(i=1;i<=level-2;i++){
		runs0[i]=0; /*runs of length i */
		runs1[i]=0;
		maxruns[i]=power2(level-2-i);
	}
	runs0[level-1]=0; 
	runs1[level-1]=0;
	maxruns[level-1]=0;
	runs0[level]=1;
	runs1[level]=0;
	maxruns[level]=1;
	currentrun = level;
	last = '1';
	/* Main loop */
	i = 2*level;
	while(i<n-level+1){

		/* Fill the buffer with current level-1-string tail + last */
		for(j=i-level+1;j<i;j++)buffer[j-(i-level+1)]=rval[j];
		buffer[level-1]=last;
		buffer[level]='\0';
		if(strstr(rval,buffer)!=NULL){ /* duplicating earlier level-string */
			rval[i++] = COMPLEMENT( last );
			rval[i]='\0';
			if(last == '0')runs0[currentrun]++;
			else runs1[currentrun]++;
			if(last == '0')last = '1';
			else last = '0';
			currentrun=1;
			continue;
		}
		if(last == '0'){ /* try to extend run, unless all runs at next 
						 longer length are filled up */
			if(runs0[currentrun+1]==maxruns[currentrun+1]){
				rval[i++]='1';
				rval[i]='\0';
				last = '1';
				runs0[currentrun]++;
				currentrun = 1;
				continue;
			}
			else{
				rval[i++]='0';
				last = '0';
				rval[i]='\0';
				currentrun++;
				continue;
			}
		}
		else { /* last = '1' */
			if(runs1[currentrun+1]==maxruns[currentrun+1]){
				rval[i++]='0';
				rval[i]='\0';
				last = '0';
				runs1[currentrun]++;
				currentrun = 1;
				continue;
			}
			else{
				rval[i++]='1';
				last = '1';
				rval[i]='\0';
				currentrun++;
				continue;
			}
		}

	}
	for(j=0;j<level-1;j++)rval[i++]='0';
	rval[i]='\0';
	return rval;
}


/* Create a debruijn string by the prefer opposite method. This remarkably simple
algorithm was discovered by Abbas Alhakim (Clarkson University). See his preprint
entitled "A simple combinatorial algorithm for DeBruijn Sequences."  Start with a string
of n zeros. Then try the opposite of the most recently added digit, unless that would 
violate the prime directive. Alhakim shows that this will produce a depleted debruijn string
that visits all n-strings except the all-1 string. He also shows that the all-1 string of length n-1 is
the last string of 1's produced, so it is simple to finish the sequence with n 1's followed
by n-1 zeros. That produces a genuine debruijn string */

char *prefer_opposite(int level)
{
	char *rval;
	char last;
	int i,j,n;
	char buffer[MAX_LEVEL+1];

	if((level <= 0)|(level > MAX_LEVEL))return NULL;

	/* allocate memory for result string */
	n = power2(level) + level - 1;
	rval = (char *)malloc((n+1)*sizeof(char));
	if(rval==NULL)return rval;

	/* initialize result to 000...0 */
	for(i=0;i<level;i++)buffer[i]='0';
	buffer[i]='\0';
	strcpy(rval,buffer);
	/* i now indexes next digit in rval to be found */
	
	last = '0';

	/* Main loop */

	while(i<n-level){

		/* Fill the buffer with current level-1-string tail +  opposite of last */
		for(j=i-level+1;j<i;j++)buffer[j-(i-level+1)]=rval[j];
		buffer[level-1]=COMPLEMENT( last );
		buffer[level]='\0';
		if(strstr(rval,buffer)!=NULL){ /* duplicating earlier level-string */
			rval[i++] = last;
			rval[i]='\0';
			continue;
		}
		else {
			rval[i++] = COMPLEMENT( last );
			last = COMPLEMENT( last );
			rval[i]='\0';
			continue;
		}
		
	}
	/* The algorithm will always end with a string of ones. We need to finish
	by adding an extra 1, and then n-1 zeros */
	rval[i++] = '1';
	for(j=0;j<level-1;j++)rval[i++]='0';
	rval[i]='\0';
	return rval;
}