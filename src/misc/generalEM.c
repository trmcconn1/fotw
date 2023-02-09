/* Generate and print terms of a generalized Ehrenfeucht-Mycielski sequence. 
	Here a match means the two strings hash to same value under some given 
	hash function f. If f is 1-1 we get the usual EM sequence */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define TERMS 100

static char gem[TERMS + 1]; /* holds 0 and 1 terms of the sequence */

int F(char *, int);

int main(void){

	int i,j,m=0,n;
	char new;
	int m_max=0;
	char *p,*q;
	char match[TERMS+1];

	gem[0]='0'; 
	gem[1]='1';  /* seed values */
	gem[2]='0';

	for(n=3;n<TERMS;n++){
		m = n - 1;   /* largest possible match length */
		while(m>0){
			p = gem + n - m; /* m digits at end: to be matched  */
			q = p-1;     /* possible matching string */
			j = F(p,m); /* store compare value */
			while(q >= gem){ 

				if(F(q,m) == j)break;
				q--;
			}
			if(q >= gem)break;  /* match found */
			m--;
		}
		/* if(m > m_max){m_max = m; printf("%d\n",(int)(q-gem));} */
		/*printf("%d",m);*/   
		/* Compute new digit: it should prevent match at end */
		for(i=0;i<=m;i++)
			match[i]=q[i];
		match[i+1]='\0';
		j = F(match,m+1);
		for(i=0;i<m;i++)
			match[i]=p[i];
		match[i]='0';
		if(F(match,m+1)!=j){gem[n]='0'; continue;}
		else {

			match[i]='1';
			if(F(match,m+1)!=j){gem[n]='1'; continue;}
			else {fprintf(stderr,"bad evaluation function n = %d\n",n); exit(1);}
		}
		
	}
	/* gem[n]='\0'; */ 
	printf("%s\n",gem);  
}
	

/* String Evaluation Functions */

/*

/* Count number of 1s */ 
/*
int F(char *p, int n)
{

	int s = 0;
	int i,j;
	if(p==NULL)return 0;
	if(n == 0)return 0;
	for(i=0;i<n;i++)
		if(p[i]=='1')++s;
	return s;
}
*/

/* string and bitwise complement hash equal */
/*
int F(char *p, int n)
{
	int i,s=0,t=0,d;
	if(p==NULL)return 0;
	if(n==0)return 0;
	for(i=0;i<n;i++){
		if(p[i]=='0')d=0;
			else d=1;
		s = 2*s + d;
	}
	for(i=0;i<n;i++){
		if(p[i]=='0')d=1;
			else d=0;
		t = 2*t + d;
	}
	if(t < s) return 1+t;
	return 1+s;
}

*/
/* strings hash equal if they are either equal or equal for an initial segment
and opposite after that */

/*
int F(char *p, int n)
{
	int i,s=0,d;
	int flip = 0;
	if(p==NULL)return 0;
	if(n==0)return 0;
	for(i=0;i<n;i++){
		if(flip)
			if(p[i]=='0')d=1;
			else d=0;
		else if(p[i]=='0'){d=1;flip=1;}
		else d=1;
		s = 2*s + d;
	}
	return 1+s;
}
*/

/* Returns binary value of string modulo K */
#define K 3
int F(char *p, int n){

	int i,s=0,t=0,d;
	int j=0;
	if(p==NULL)return 0;
	if(n==0)return 0;
	if(n>30)j=30;
	for(i=j;i<n;i++){
		if(p[i]=='0')d=0;
			else d=1;
		s = 2*s + d;
	}
	return s % K;
}
