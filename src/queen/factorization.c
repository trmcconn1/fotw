/* Factorization.c: routines related to factorization into primes and 
 * management of prime factorization information 
 *
 * This has no main routine
 *
 *  By Terry R. McConnell
 *
*/             


/* compile: cc -c factor.c

      Use -D_SHORT_STRINGS if your compiler does not support multiline
          string constants.

*/


#include "global.h"
#include<stdio.h>
#include<stdlib.h>
#include "factor.h"
#include "prime.h"
#include "math.h"

extern int primality_test(integer p, int type);  
extern struct prime primes[N_PRIMES];
static struct factorization *alloc_factorization(int n);
static void free_factorization(struct factorization *);


int pprint_factored( struct factorization *ff)
{
	int i,j,k,n,e;

	if(!ff) return FALSE;
	n = ff->n;
	if(n<0){
		printf("0");
		return TRUE;
	}
	if(n==0){
		printf("1");
		return TRUE;
	}
	for(i=0;i<n;i++){
		e = ff->exponents[i];
		if(e>1)
			printf("(%ld^%ld)",ff->primes[i],e);
		else
			printf("(%ld)",ff->primes[i]);
	}
	return TRUE;	
}

/* This is the only factorization method supported at this time */

/* Brute force factorization. For each prime less than or equal to 
 * z, divide it in as many times as it will go, keeping
 * record of the number in a local array. Then alloc and populate a
 * factorization struct accordingly  
 *
 * Beyond the largest prime in lib_primes it becomes very laborious. */

static integer exponents[N_PRIMES];

static struct factorization *factor_brute(integer z){

	int i,j;
	integer n;
	int m=0,c=0; /* c counts distinct prime factors */
	integer p = 2,k,e;
	struct factorization *my_return = NULL;
	struct factorization *tmp;

	if(z < 0) z = -z;
	n = z;

	if(z == 0) n = -1;
	if(z == 1) n = 0;


	if(n > primes[N_PRIMES-1].p) m = N_PRIMES;
	else while(primes[m].p <= n)m++;

	/* m now has the index of the largest prime that could occur */

	/* Make sure exponents array is initialized */
	for(i=0;i<m+1;i++)exponents[i]=0;

	/* Do test divisions */

	for(i=0;i<m;i++){

		p = primes[i].p;
		k = z / p;
		while(k*p == z){
			z = k;
			exponents[i]++;
			k = z / p;
		}
		if(exponents[i])c++;
		if(z==1)break;
	}  

	/* alloc and populate factorization struct */
	my_return = alloc_factorization(c);
	if(!my_return)return NULL;

	my_return->n = n;
	if(n<1)return my_return;
	my_return->n = c;

	j = 0;
	for(i=0;i<c;i++){
		while(exponents[j]==0)j++;
		my_return->primes[i] = primes[j].p;
		my_return->exponents[i]=exponents[j];
		j++;
	}

	p = primes[N_PRIMES-1].p;
	while(p < z){ /* We ran out of primes in lib */

		if(my_return) tmp = my_return; ;  /* Save for a rainy day */
		p++;

	/* Sigh ... we have to search for next prime 
	 * ourselves... stay tuned  */

		while(primality_test(p,BRUTE_FORCE)==FALSE)p++;

		/* Redo trial division */
		k = z/p;
		e = 0;
		while(k*p==z){
			z = k;
			e++;
		}
		if(e > 0){
			c++;
			tmp = my_return;
			my_return = alloc_factorization(c);
		        if(!my_return)return NULL;
	       		my_return->n = c;
	 		for(j=0;j<c-1;j++){ 
				my_return->primes[j]
						= tmp->primes[j];
				my_return->exponents[j]=tmp->exponents[j];
			}	
			my_return->primes[j] = p;
			my_return->exponents[j]=e;
			free_factorization(tmp);
		}
		if(z==1)break;
	}
	return my_return;
}


static void free_factorization(struct factorization *p)
{
	int n;

	if(!p)return;
	if(n<1){
		free(p);
		return;
	}
	if(p->exponents) free(p->exponents);
	if(p->primes) free(p->primes);
	
	free(p);
	return;
}
		

struct factorization *alloc_factorization(int n)
{
	struct factorization *r;
	integer *e,*p;

	r = (struct factorization *)malloc(sizeof(struct factorization));
	if(!r) return NULL;
	e = (integer *)malloc(n*sizeof(integer));
	p = (integer *)malloc(n*sizeof(integer));
	if(!p || !e){
		free(r);
		return NULL;
	}
	r->n = n;
	r->exponents = e;
	r->primes = p;
	return r;
}


struct factorization *factor_integer( integer z, int method ){

	if(z < 0) z = -z;

	switch(method){

		case BRUTE_FORCE:
			return factor_brute(z);

		default: 
			fprintf(stderr,"factor_integer: unsupported method %d\n",method);
			return NULL;
	}
}	

