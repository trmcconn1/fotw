/* zp.c: routines specific to arithmetic modulo a prime
 *
 *  By Terry R. McConnell
 *
*/             



#include "global.h"
#include<stdio.h>
#include<stdlib.h>


/* Calculate the legendre symbol of n modulo p. The caller must insure that
   p is actually prime, otherwise the return value will have no meaning.
   If n is divisible by p the return value is 0. It is also 0 if p is 0.

   Recall that the legendre symbol is +1 if n is a perfect square modulo
   p and -1 if not. We use Gauss's Lemma: Count how many reduced residues
	{ n, 2n, 3n, ... , (p-1)n/2 } are greater than p/2. If this number is
   even then the legendre symbol is 1. If it is odd the legendre symbol l is
   is -1.  Reason: Denote by x the product of the set of reduced residues 
   above. Then l = n^x (mod p). Negating those residues that fall above p/2
   gives exactly the missing ones from the set that fell below. (one must
   check this.) Thus n^x ((p-1)/2)! = (-1)^c((p-1)/2)! where c is the count
   of the negations. */

integer legendre_symbol(integer n, integer p)
{

	integer l = 1;
	integer j,m;
	integer h = (p-1)/2;

	if(p == 0) return 0;
	n = n % p;
	if(n == 0) return 0;
	for(j = 1; j<=h; j++){

		m = j*n % p;
		if( m > h) l = -l; /* toggle */
	}
	return l;
}	

/* Calculate the inverse modulo p. The caller must ensure that p is prime 
   and that x is prime to p.
   Otherwise use associate routine in zn.c */

integer inverse( integer a, integer p )
{
	integer x,y,r;

	r = my_gcd(a,p,&x,&y);
	return x;
}	

	
/* Calculate the square root of an integer modulo p.

	For this we use Shank's algorithm, see Number Theory with Computer
Applications, R. Kumanduri and C. Romero, Section 9.2. 

	Let a be the number whose square root is to be found. The number of
iterations depends on how close p is to 1 in the 2-adic numbers. We write
 
	(p-1) = s 2^k

where s is odd. We need to first find a quadratic nonresidue, z. Any one will
do -- we use the smallest positive one. Then we form two sequences:

x_0, x_1, ... and b_0, b_1, ... . 

Let x_0 = a^(s+1)/2, b_0 = a^s. If p is congruent to 3 mod 4 we're already
done, since

	x_0 ^2 = a^(s+1) = a * a^[(p-1)/2] = a,

by Euler's criterion, since a is a residue. (We can assume this).

Otherwise define inductively

b_m = b_(m-1) * (z^s) ^ 2^(k-r)

x_m = x_(m-1) * (z^s) ^ 2^(k-r-1)


where 2^r is the order of b_(m-1).  One can check by induction that

	x_m ^ 2 = a * b_m (*)

It turns out that the orders of the b's
strictly decrease, reaching 1 eventually. When they do, the desired square
root is x_m.

The theory is quite simple: Let G be the multiplicative subgroup of the
multiplicative group of invertible elements mod p consisting of all 
elements whose order is a power of 2. One checks:

 b_0 = a^s belongs to G and z^s generates  G. 

Hence all b_m belong to G, and so r as above exists. To see that the
order of b_m is strictly smaller than order of b_(m-1) = 2^r, raise b_m to
the power 2^(r-1): We get the product of two factors, each congruent to -1:

b_(m-1)^[2^(r-1)] and (z^s)^[2^(k-1)],

the latter using Euler's criterion. 

This returns a square root of a mod p in the range 0 <= x < p, or -1
if there is no square root */ 


integer sqrtp(integer a, integer p)
{

	integer s, z, x, b, c, k, y;

	integer j,r,m=0;

	if( p <= 1) return -1;

	while( a < 0 ) a += p; 
	a = a % p;
	if( a == 0) return 0;

	if( p == 2) return 1;

	/* From now on, p is an odd prime and 0 < a < p */

	if(legendre_symbol(a,p)==-1) return -1;

	/* Find the factorization of p-1 = s*2^k. */
	k = 0;
	s = p-1;

	if( s % 2 ) return -1;  /* should not happen */

	
	while( ( s % 2 ) == 0) {
		s = s/2;
		k++;
	}	

	/* Now find the smallest positive quadratic non_residue mod p */

	z = 2;
	while(legendre_symbol(z,p)==1)z++;

	/* compute initial b = a^s, y = z*s */

	b = a;
	y = z;

	for(j=1;j<s;j++){
		 b *= a;
		 b = b%p;
		 y *= z;
		 y = y%p;
	}
		
	/* compute x = a^(s+1)/2 */

	x = a;
	j = (s+1)/2;

	while(j>1){

		x *= a;
		x = x % p;
		j--;
	}

	while( b != 1 ) {  /* Main loop */

	/* Find the order 2^r of b */

		r = 0;
		c = b;
		while(c != 1){
			c *= c;  /* square c */
			c = c % p;
			r++;
		}

	/* Find new x = old x * y^(2^(k-r-1)), new b = old b * y^(2^(k-r)) */

		/* First update y^(2^(k-r-1)) by squaring the old y
		   the requisite additional number of times. The total number
                   of squarings of y done so far (k - old r) is maintained in 
		   m */

		for(j=0;j<k-r-m-1;j++) { 

				y *= y;
				y = y%p;
		}

		m = k - r; /* store for next iteration */

		x *= y;
		x = x%p;

	 	y *= y;
		y = y%p;

		b *= y;
		b = b%p;
	
	}

	return x;
}
