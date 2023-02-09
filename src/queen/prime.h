/* prime.h: header for prime related stuff */


/* This defines N_PRIMES */
#include "libprimes/libprimes.h" 

struct prime {
	integer p;   /* The prime */
	/* Add further fields as needed */
};

extern struct prime primes[N_PRIMES];
