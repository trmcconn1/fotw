/* Contains the implementation of routines called from debruijn.cpp


Data type for implementing double linked list */
#define _CRT_SECURE_NO_WARNINGS
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "debruijn.h"

int verbose = 0;

static struct node *nodearray[MAX_STRING];

void warn(char *msg){
	fprintf(stderr,"debruijn:%s",msg);
	return;
}

int power2(int n){

	if(n<0)return -1;
	if(n==0)return 1;
	return 2*power2(n-1);
}

/* reverse a string in place */

void reverse(char *p)
{
	int n,i;
	char *q;

	if(!p)return;
	n = strlen(p);
	if(n==0)return;
	q = malloc((n+1)*sizeof(char));
	if(!q){warn("reverse: could not allocate memory");return;}
	for(i=0;i<n;i++)q[i]=p[n-i-1];
	q[i]='\0';
	strcpy(p,q);
	free(q);
	return;
}

/* complement all chars in place */

void complement(char *p)
{
	int i,n;
	if(!p)return;
	n = (int)strlen(p);
	if(n==0)return;
	for(i=0;i<n;i++)
		p[i] = COMPLEMENT( p[i] );
	return; 
}

/* Warning: this rotates a debruijn string, not a debruijn cycle. The
cycle part of it is rotated k units counter-clockwise */

void rotate(char *p,int k,int level)
{
	int i,n,m,j;
	char *q;

	if(!p)return;
	m = (int) strlen(p);
	n = m-level+1;
	if(n<=0)return;
	if(k==0)return;
	q = malloc((m+1)*sizeof(char));
	if(!q){warn("rotate: could not allocate memory");return;}
	for(i=0;i<n;i++)q[i]=p[(i+k) % n ];
	for(j=0;j<level-1;j++)q[i++]=q[j]; /* copy head of initial string to end */
		q[i]='\0';
		strcpy(p,q);
		free(q);
		return;
	}

/* This rotates an arbitrary string k units clockwise */

void rotate_string(char *p,int k)
{
	int i,n;
	char *q;

	if(!p)return;
	n = (int) strlen(p);
	if(n<=0)return;
	if(k==0)return;
	if(k<0)k = n + k;
	q = malloc((n+1)*sizeof(char));
	if(!q){warn("rotate: could not allocate memory");return;}
	for(i=0;i<n;i++)q[(i+k)%n]=p[i];
	q[n]='\0';
	strcpy(p,q);
	free(q);
	return;
}

/* psi: partial summing operator: p[i] is the sum mod 2 of all
	binary digits before it */

void psi(char *p)
{
	char s=0,c;
	int i,n;

	if(!p)return;
	n = strlen(p);
	for(i=0;i<n;i++){
		c = p[i];
		if(s)p[i] = ( c == '0' ) ? '1' : '0';
		else p[i] = ( c == '0' ) ? '0' : '1';
		if(c == '1') s = (s + 1)%2;
	}
	return;
}

/* Add 1 to the longest string of 1's and remove 0 from the longest
string of 0's . Does nothing if string does not contain at least one 0*/

void add1drop0( char *p )
{
	int i=0,j=0,L=-1,m=-1,n,k,r=1;
	int state; /* 0 for within run of zeros, 1 for within run of 1's */
	char c;
		
	if(!p)return;
	/* i will track the length of the longest run of 0's and j same for
			1's. L,m track where longest run ends */
	/* r will track the length of the current run */
		
	n = strlen(p);
	if(n==0)return;
	c = p[0];
	if(c=='0'){state = 0; i = 1; L = 0;}
	if(c=='1'){state = 1; j = 1; m = 0;}
	for(k=1;k<=n;k++){
		c = p[k];
		if((c == '\0')||(c == '0')){
			if(state == 1){
				if(r > j){j = r; m = k-1;}
				state = 0;
				r = 1;
				continue;
			}
			else {
				if(c == '\0'); /* catch in state=0 below */
				else {
					r++;
					continue;
				}
			}

				
		}
		if((c=='\0')||(c == '1')){ 
			if(state == 0){
				if(r > i){i = r; L = k-1;}
				state = 1;
				r = 1;
				continue;
			}
			else r++;
			continue;
		}
	}
	if(L==-1) return;  /* no can do No zeros at all*/
	if(m==-1){
		p[0] = '1'; /* string is all zeros, so just change first to 1 */
		return;
	}
/* Copy everything after 0000 run down 1 place, obliterating last 0 */
	for(k=L;k<n-1;k++)p[k] = p[k+1];
	if(m > L)m--; /* update position of 1 block if past zero block */
/* Copy everthing after 1111 run right one place and stick extra 1 at start */
	for(k=n-1;k>m+1;k--)
		p[k] = p[k-1];
	p[m+1]='1';
	return;
}

void add0drop1(char *p)
{
	if(!p)return;
	complement(p);
	add1drop0(p);
	complement(p);
	return;
}



	struct node *allocnode(char *name, int size)
	{
		struct node *aa;
		size_t m;

		aa = (struct node *)malloc(sizeof(struct node));
		if(aa == NULL){ warn("allocnode: could not allocate node memory\n");return (struct node *)0;}
		aa->prev0 = (struct node *)0;
	aa->prev1 = (struct node *)0;
	aa->next0 = (struct node *)0;
	aa->next1 = (struct node *)0;
	aa->up0 = (struct node *)0;
	aa->up1 = (struct node *)1;
	if(size < 0){warn("allocnode: called with negative size arugment\n");free(aa);return (struct node *)0;}
	if(size == 0){aa->order = 0; aa->name = (char *)0; return aa;}
	if(name == NULL){ warn("allocnode: called with null name pointer\n");free(aa); return (struct node *)0;}
	m = strlen(name);
	if(m != size)warn("allocnode:  mismatch between size and strlen(name)\n");
	aa->order = size;
	aa->visits = 0;
	aa->xvisits = 0;
	aa->yvisits = 0;
	aa->name = (char *)malloc((m+1)*sizeof(char));
	if(aa->name == (char *)0){warn("allocnode: cannot allocate node name memory\n");free(aa);return (struct node *)0;}
	strcpy(aa->name,name);
	return aa;
}

int freenode(struct node *anode){
	if(anode == NULL) return 0; /* nothing to do */
	if(anode->name != NULL)free(anode->name);
	free(anode);
	return 0;
}

void freenodearray(void)
{
	struct node *p;
	int i=0;
	p = nodearray[i++];
	while(p!=NULL){
		freenode(p);
		p = nodearray[i++];
	}
}

/* The following parses a binary string into a doubly linked list of 
   nodes. Unique named nodes are placed in an array. 
   It returns a pointer to the first node in the list */

struct node *parse( char *seq, int level)
{
	int n,i,j,nnodes = 0,found;
	char *c;
	char ch;
	char buf[MAX_LEVEL+1];
	struct node *old,*current,*root;

	old = (struct node *)NULL;

	if(seq == NULL){warn("parse: passed null string\n"); return NULL;}
	if(level > MAX_LEVEL){warn("parse: called with level too large\n");return NULL;}
	n = strlen(seq);
	if(n < level){warn("parse: string passed too short\n"); return NULL;}
	if(n > MAX_STRING){warn("parse: string passed too long\n");return NULL;}

	/* Examine substrings of length = level one at a time starting from i = 0
	position in seq until the last full string that will fit */

	i=0;
	while(level + i <= n){ 
		c = seq + i++;
		for(j=0;j<level;j++){buf[j] =*c;c++;} /* copy current substring */
		buf[level]='\0';

		/* see if a node with this name already exists. Otherwise create a new one and add to
		   the array */

		found=0;
		for(j=0;j<nnodes;j++)if(strcmp(buf,nodearray[j]->name)==0){found = 1;break;}
		if(!found){
				current = allocnode(buf,level);
				if(current == NULL){warn("parse: failed to alloc node\n"); return NULL;}
				current->visits = 1;
				nodearray[nnodes++] = current;
		}
		else { current = nodearray[j]; (current->visits)++;}
		if(old != NULL){ /* set pointers in current and last node */
			ch = *(old->name+level-1);
			if(buf[level-1]=='0')old->next0 = current;
			else old->next1 = current;
			switch(ch){
				case '0':
					current->prev0 = old;
					break;
				case '1':
					current->prev1 = old;
					break;
				default: 
					warn("parse:not binary string char");fprintf(stderr,"(%c)\n",ch);
fprintf(stderr," i = %d, name = %s\n", i,old->name);
			}
		}
		else root = current;
		old = current;
	}

	return root;
}

/* Create node list and path using parse and report on it 
BUG: if the parsed string is not deBruijn, this will crash */


void dumppath(char *astring, int level){

	struct node *anode,*head;
	int ncount=0,j;

	if(astring==NULL){warn("dumppath: called with null string ptr\n");return;}
	if(!isdebruijn(astring,level)){warn("dumppath: not a debruijn string");return;}

	head = parse(astring,level);

	if(head == NULL){warn("dumppath: parse returned null pointer\n");return;}
	anode = head;
	printf("dumppath (level %d):\n",level);
	while(anode != NULL){
		ncount++;
		printf("\t%s[%d]",anode->name,anode->visits);
		if(anode->next0){anode = anode->next0; printf(" -- [0] -->\n");}
		else { anode = anode->next1; printf(" -- [1] -->\n");}
	}
	printf("Number of nodes = %d\n",ncount);
	/* clean up */
	for(j=0;j<ncount;j++)freenode(nodearray[j]);
	return;
}

/* Search node list ("path") created by parse for a node having the given name. Return pointer to node 
   if found, otherwise null */

struct node *searchpath(char *name, struct node *path, int level){

	while((path != NULL)&&(strcmp(path->name,name)!=0))
		if(path->next0)path = path->next0;
		else path = path->next1;
	return path;
}

/* Parse string and report all loops in the DeBruijn graph with edges of the path produced
   by parse removed. ONLY CALL WITH A DEBRUIJN STRING */

void dumpantipath(char *astring, int level){

	struct node *anode,*path;
        char c;
	char buf[MAX_LEVEL+1];
	int i,j,ncount=0,loops=0;;
	
	if(astring==NULL){warn("dumpantipath: called with null string ptr\n");return ;}
	if(!isdebruijn(astring,level)){warn("dumpantipath: not a debruijn string");return;}  
	path = parse(astring,level);
	if(path == NULL){warn("dumpantipath: parse returned null pointer\n");return;}

	/* The last node created by parse does not have its out pointers
	   set, and the first does not have its inpointers set. This needs to be
	   made into a loop */

	c = *(path->name+level-1);
	i = 0;
	while((anode = nodearray[i++])!= NULL)ncount++;
	anode = nodearray[ncount-1];
	if(c=='0'){anode->next0 = path;path->prev0 = anode;}
	else {anode->next1 = path;path->prev1 = anode;}

	if(verbose)printf("Node count = %d\n",ncount);
	while(1){
	i=0;
	while((i < ncount)&&((anode = nodearray[i])->visits) == 2)i++;
/* move to the next node that has not been included in a loop */
	if(i==ncount)break;
	loops++;
	if(verbose)printf("------------------ loop %d ------------------\n",loops);
	else printf("(%s\b",anode->name);
	j=0;
	while(1){
		if(verbose)printf("%s\n",anode->name);
		else printf("%c",anode->name[level-1]);
		if(anode->visits == 2){
			if(verbose)printf("----------------- end loop size %d -------\n",j);
			else printf(")");
			break;
		}
		j++;
		anode->visits++;

	/* build name of next node to visit */

		for(i=0; i<level-1;i++)buf[i] = *(anode->name+i+1);
		if(anode->next0)buf[i++]='1';
		else buf[i++]='0';
		buf[i]='\0';
		anode = searchpath(buf,path,level);
		if(anode == NULL){warn("antipath: can't find next node\n");return;}
	}
	}
	if(verbose)printf("Total loops = %d\n",loops);
	else printf("\n");
	/* clean up */
	for(i=0;i<ncount;i++)freenode(nodearray[i]);
	return;
}

/* Same as dumpantipath, only return the number of loops. ONLY CALL WITH A DEBRUIJN STRING */

int countantipath(char *astring, int level){

	struct node *anode,*path;
    char c;
	char buf[MAX_LEVEL+1];
	int i,j,ncount=0,loops=0;;
	
	if(astring==NULL){warn("countantipath: called with null string ptr\n");return -1;}
	if(!isdebruijn(astring,level)){warn("countantipath: not a debruijn string");return -1;}
	path = parse(astring,level);
	if(path == NULL){warn("countantipath: parse returned null pointer\n");return -1;}

	/* The last node created by parse does not have its out pointers
	   set, and the first does not have its inpointers set. This needs to be
	   made into a loop */

	c = *(path->name+level-1);
	i = 0;
	while((anode = nodearray[i++])!= NULL)ncount++;
	anode = nodearray[ncount-1];
	if(c=='0'){anode->next0 = path;path->prev0 = anode;}
	else {anode->next1 = path;path->prev1 = anode;}

	while(1){
	i=0;
	while((i < ncount)&&((anode = nodearray[i])->visits) == 2)i++;
/* move to the next node that has not been included in a loop */
	if(i==ncount)break;
	loops++;
	j=0;
	while(1){
		if(anode->visits == 2){
	
			break;
		}
		j++;
		anode->visits++;

	/* build name of next node to visit */

		for(i=0; i<level-1;i++)buf[i] = *(anode->name+i+1);
		if(anode->next0)buf[i++]='1';
		else buf[i++]='0';
		buf[i]='\0';
		anode = searchpath(buf,path,level);
		if(anode == NULL){warn("countantipath: can't find next node\n");return -1;}
	}
	}
	/* clean up */
	for(i=0;i<ncount;i++)freenode(nodearray[i]);
	return loops;
}

/* Returns 1 if seq is a deBruijn string of order level, 0 otherwise */

int isdebruijn( char *seq, int level)
{
	int n,i,j,nnodes = 0,found;
	char buf[MAX_LEVEL+1];
	char *c;
	struct node *old,*current;
	int rval = 1;

	old = (struct node *)NULL;

	if(seq == NULL){warn("isdebruijn: passed null string\n"); return 0;}
	if(level > MAX_LEVEL){warn("isdebruijn: called with level too large\n");return 0;}
	n =(int) strlen(seq);
	if(n < level){warn("isdebruijn: string passed too short\n"); return 0;}
	if(n > MAX_STRING){warn("isdebruijn: string passed too long\n");return 0;}

	/* Is the durn thing even of the correct length ? */

	if(n != power2(level) + level - 1)return 0;

	/* Examine substrings of length = level one at a time starting from i = 0
	position in seq until the last full string that will fit */

	i=0;
	while(level + i <= n){ 
		c = (seq + i++);
		for(j=0;j<level;j++){buf[j] =*c;c++;} /* copy current substring */
		buf[level]='\0';

		/* see if a node with this name already exists. Otherwise create a new one and add to
		   the array */

		found=0;
		for(j=0;j<nnodes;j++)if(strcmp(buf,nodearray[j]->name)==0){found = 1;break;}

		if(!found){
				current = allocnode(buf,level);
				if(current == NULL){warn("isdebruijn: failed to alloc node\n"); rval=0;break;}
				current->visits = 1;
				nodearray[nnodes++] = current;
		}
		else { rval = 0; break; }
	}
	for(i=0;i<nnodes;i++)freenode(nodearray[i]);
	return rval;
}


/*  Works like the library routine (which it calls) but allows the searched
for string (q) to wrap around to the beginning of p */

char *cyclic_strstr(char *p, char *q)
{
	int i,k,n;
	char *ep,*s;

	if(!q)return NULL;
	if(!p)return NULL;
	k = strlen(q);
	n = strlen(p);
	if(k > n)return NULL;
	ep = (char *)malloc((n+k)*sizeof(char *));
	if(!ep)return NULL;
	strcpy(ep,p);
	for(i=0;i<k-1;i++)ep[n+i]=ep[i];
	ep[n+i]='\0';
	s = strstr(ep,q);
	if(!s) {
		free(ep);
		return NULL;
	}
	k = s - ep;
	free(ep);
	return k + p;
}
	
	
