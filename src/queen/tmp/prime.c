
/* prime.c: code to create and support library of information on primes */

#include "global.h"
#include <stdlib.h>
#include <stdio.h>
#include "prime.h"
#include "factor.h"

static int primality_test_brute_force(integer);

int primality_test(integer p, int type){

	if(p<0)p=-p;
	if(p==0)return FALSE;
	if(p==1)return TRUE;
	switch(type){

		case BRUTE_FORCE:
			return primality_test_brute_force(p);

		default:
			fprintf(stderr,"primality_test: unsupported type %d\n",type);
	}
	return FALSE;
}

static int primality_test_brute_force(integer n)
{

	int i=0;
	integer p,k;

	while(i < N_PRIMES )  { /* Test p for divisibility by known primes */
		p = primes[i].p;
		if(p == n) return TRUE;
	        if(p*p > n) break;	
	        k = n/p;
	        if(p*k == n) return FALSE;
	        i++;
	}
	if( i == N_PRIMES ) { /* Ran out of known primes ? */
		i = primes[N_PRIMES-1].p + 1;
		if(i==n)return TRUE;
		while (i*i <= n ) {
			k = n/i;
			if(k*i==n) return FALSE;
			i++;
		}
	}
	return TRUE;
}

	
