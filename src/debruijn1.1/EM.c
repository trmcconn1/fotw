#define _CRT_SECURE_NO_WARNINGS

#include "debruijn.h"
#include <stdlib.h>
#include <stdio.h>

#include <string.h>

static char buffer[MAX_STRING];

struct EMnodestruct {
	int m;  /* net time, does not include seed */
	int n;  /* gross time, does include seed */
	char y; /* next digit */
	char *sigma; /* shortest unique tail */
	struct EMnodestruct *next;
};

struct EMnodestruct *EMallocnode(char *sigma)
{
	struct EMnodestruct *rval;
	rval = malloc(sizeof(struct EMnodestruct));
	if(!rval)return NULL;
	rval->next = NULL;
	if(sigma == NULL)
		rval->sigma = NULL;
	else
		rval->sigma = strdup(sigma);
	return rval;
}

void EMnodedestroy(struct EMnodestruct *p){
	if(p){
		if(p->sigma)free(p->sigma);
		free(p);
	}
}

/* Return pointer to node having the given string in a linked
   list of nodes, else return NULL */

struct EMnodestruct *EMfindnode(char *astring, struct EMnodestruct *p)
{
	char *s;

	while(p !=NULL){
		if(astring == NULL)NULL;
		if((s = p->sigma) == NULL){
			return NULL;
		}
		else if(strcmp(s,astring)==0)return p;
		p = p->next;
	}
	return NULL;
}

/* return pointer to string = seed + n terms of seeded Ehrenfeucht-Mycielski sequence */

char *EM(char *seed, int n)
{
	int i, m;
	char *rval;
	char c,*p,*q,*r;

	/* Allocate memory */
	if(seed == NULL)m = n;
	else m = n+(int)strlen(seed);
	rval = (char *)malloc((m+1)*sizeof(char));
	if(rval == NULL){fprintf(stderr,"EM: alloc memory failed\n");return NULL;}

	/* Handle trivial cases */
	if((seed==NULL)||(strlen(seed)==0)){
		if(n < 0)return NULL;
		if(n==0){rval[0]='\0'; return rval;}
		rval[0]='0';
		rval[1]='\0';
		i=1;
	}
	else {strcpy(rval,seed); i =(int) strlen(seed);}

	/* Insert next digit in return string at index i */

	while(i < m){
		p=rval+i;
		while((q=strstr(rval,p))&&(q<p)&&(p>rval))p--;
		p++;  /* p points at longest tail that occured before */

		/* find pointer q to latest such earlier occurrence */
		q = strstr(rval,p);
		while((r=strstr(q+1,p))<p)q=r;

		/* get char following occurrence at q */	
		c = *(q+strlen(p));
		rval[i++] = COMPLEMENT( c );
		rval[i]='\0';
	}
	return rval;	
}
/* return pointer to head of node linked list (length n and
     given seed ) representing an EM sequence */

struct EMnodestruct  *EMnodes(char *seed, int n)
{
	int i,j, m;
	char *em;
	struct EMnodestruct *rval,*r,*prev;
	char c,*p,*q,*s;

	/* Allocate memory */
	if(seed == NULL)m = n;
	else m = n+(int)strlen(seed);
	em = (char *)malloc((m+1)*sizeof(char));
	if(em == NULL){fprintf(stderr,"EM: alloc memory failed\n");return NULL;}
	rval=malloc(n*sizeof(struct EMnodestruct *));
	if(rval == NULL){fprintf(stderr,"EM: alloc memory failed\n");
		free(em);
		return NULL;
	}

	/* Handle trivial cases */
	if(seed==NULL){
		if(n < 0)return NULL;
		rval=EMallocnode("\0");
		rval->m = 0;
		rval->n = 0;
		rval->y='0';
		if(n==0)return rval;
		em[0]='0';
		em[1]='\0';
		i=1;
	}
	else {strcpy(em,seed); i =(int) strlen(seed);}
	j=1;
	prev = rval;
	/* Insert next digit in em string at index i */

	while(i < m){
		p=em+i;
		while((q=strstr(em,p))&&(q<p)&&(p>em))p--;
		p++;  /* p points at longest tail that occured before */
		/* find shortest tail of p not yet acquired */
		q = p+strlen(p);
		while(EMfindnode(q,rval) != NULL)q--;
		r=EMallocnode(q);
		prev->next = r;
		r->m = j;
		r->n = i;

		/* Generate next digit of EM sequence */
		/* find pointer q to latest earlier occurrence of p */

		q = strstr(em,p);
		while((s=strstr(q+1,p))<p)q=s;

		/* get char following occurrence at q */

		c = *(q+strlen(p));
		em[i++] = COMPLEMENT( c );
		em[i]='\0';
		r->y = COMPLEMENT(c); /* finish filling in node data */
		prev=r; /* and link up */
		j++;
	}
	free(em);
	return rval;	
}

void EMprintnodeinfo(char *seed,int n)
{
	struct EMnodestruct *nodes,*p,*q;
	int i;

	nodes = EMnodes(seed,n);
	p = nodes;
	if(nodes == NULL)return;
	for(i=0;i<n;i++){
		printf("%d:\t%c (%s)\n",i,p->y,p->sigma);
		p = p->next;
	}
	p = nodes;
	while((p) != NULL){
		q=p->next;
		EMnodedestroy(p);
		p=q;
	}
}

int hasbothdigits(char *s)
{
	char c;
	int n,i;

	if(s == NULL)return 0;
	if((n = (int)strlen(s)) <= 1) return 0;
	c = s[0];
	for(i=1;i<n;i++)if(s[i] != c)break;
	return (i == n) ? 0 : 1;
}
	/* Experimental stuff: */


/* EMstar is the evil twin of EM. Roughly speaking, it does the
opposite of what EM does. */

struct EMnodestruct *EMstarnodes(char *seed, int n)
{
	int i,j, m;
	char *p;
	struct EMnodestruct *rval,*r,*current;
	struct EMnodestruct *w,*v;
	char cbuf[256];

	/* Allocate memory */
	m=n;
	rval=malloc(sizeof(struct EMnodestruct *));
	if(rval == NULL){fprintf(stderr,"EMstar: alloc memory failed\n");}
	w = v = EMnodes(seed,10*n); /* change w, not v */
	if(w == NULL)fprintf(stderr,"He's dead, Jim. Failed to create EM nodes\n"); 

	rval = EMallocnode("1");
	rval->y = '0';
	i=0;current = rval;

	/* Insert next digit in return string at index i */

	while(i++ < m){

		strcpy(cbuf,current->sigma);
		j=(int)strlen(cbuf);
		cbuf[j++]=current->y;
		cbuf[j++]='\0';
	
		p = cbuf+strlen(cbuf)-1;
		while(EMfindnode(p,rval)||!hasbothdigits(p))p--;
		/* p is the shortest unique tail of EMstar */

		/* 
		if(!hasbothdigits(p)){
			r = EMallocnode(p);
			printf("%s\n",p);
			getchar();
			current->next = r;
			current = r;
			current->y = COMPLEMENT( p[0] );
			continue;
		} */
		r = EMallocnode(p);
		printf("%s\n",p);
		getchar();
		current->next = r;
		current=r;
		r=EMfindnode(p,v);
		current->y=COMPLEMENT( r->y );
	}	

	free(v);
	return rval;
}

/* A "symmetric" approach that generates a sequence and its evil twin
together. The first to acquire a node leaves a pointer to it's own copy
in the other's marker field. Each tries to do the EM rule as much as
possible, but must not mimic the exit from a marked node */

void EMSnodes(char *seed, int n)
{
	int i,j, m;
	char *p;
	struct EMnodestruct *r,*s,*u;
	struct EMnodestruct *x0,*y0;
	struct EMnodestruct *x,*y;
	char cbuf[256];

	/* Allocate memory */
	m=n;
	 
	x= EMallocnode("\0");
	x->y = '0';
	y= EMallocnode("\0");
	y->y ='1';
	x0=x;
	y0=y;
	i=0;

	/* Insert next digit in return string at index i */

	while(i++ < m){

		strcpy(cbuf,x->sigma);
		j=(int)strlen(cbuf);
		cbuf[j++]=x->y;
		cbuf[j++]='\0';
	
		p = cbuf+strlen(cbuf)-1;
		while(EMfindnode(p,x0))p--;
		/* p is the shortest unique tail of x0 */

		r = EMallocnode(p);
		
		x->next = r;
		x = r;
		if((u=EMfindnode(p,y0)))
			x->y = COMPLEMENT( u->y ); /* Don't do same as y did from here */
		else { /* Use EM rule */
			printf("*");
			u = EMfindnode(p+1,x0);
			x->y = COMPLEMENT( u->y );
		}
		printf("x: %s\n",p);
		getchar();

		/* repeat for y */
		
		strcpy(cbuf,y->sigma);
		j=(int)strlen(cbuf);
		cbuf[j++]=y->y;
		cbuf[j++]='\0';
	
		p = cbuf+strlen(cbuf)-1;
		while(EMfindnode(p,y0)
			)p--;
		/* p is the shortest unique tail of y0 */

		s = EMallocnode(p);
		
		y->next = s;
		y=s;
		if((u=EMfindnode(p,x0)))
			y->y = COMPLEMENT( u->y );
		else {
			printf("*");
			u = EMfindnode(p+1,y0);
			y->y = COMPLEMENT( u->y );
		}
		printf("y: %s\n",p);
		getchar();
	}	
}