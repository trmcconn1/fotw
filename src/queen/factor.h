/* Factor.h: routines related to factorization into primes and management of
 * prime factorization information 
 *
*/             

struct factorization {
	int n;         /* Number of primes. = -1 for 0, 0 for 1 */
	integer *exponents;
	integer *primes;
};


/* Supported factorization methods */
#define BRUTE_FORCE 0


/* Pretty print factorization */
extern int pprint_factored( struct factorization *ff);
extern struct factorization *factor_integer( integer z, int ); 



