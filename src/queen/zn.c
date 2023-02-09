/* zn.c: routines specific to arithmetic modulo a natural number 
 *
 *  By Terry R. McConnell
 *
*/             



#include "global.h"
#include "decl.h"
#include<stdio.h>
#include<stdlib.h>


/* associate: return the inverse of a modulo n (called the associate of
 * a in Hardy and Wright. Returned is the smallest positive representative,
 * or -1 if the number was not invertible */

integer associate (integer a, integer n)
{
	integer x,y,r;

	r = my_gcd(a,n,&x,&y);
	if(r != 1) return -1;
	return normalize(x,n);
}

/* normalize: return the number congruent to m and lying in the range
   0 ... (n-1). Return -1 to signal an error.  */

integer normalize( integer m, integer n)
{
	if(n < 0) n = -n;
	if(n == 0) return -1;

	while(m < 0) m+=n;
	return (m%n);
}

/* Chinese: return the integer x that is congruent to each given 
 * residue modulo the n given moduli. I.e., this embodies the Chinese
 * Remainder Theorem. The moduli must be relatively prime in pairs, but
 * it is up to the caller to ensure that this is the case. The value
 * returned is the least nonnegative solution, which theory ensures is
 * unique modulo the least common multiple of the moduli. 
 * 
 * Theory: To find x such that simultaneously the following congruences
 * hold:
 *
 * 	x = r_1 (mod m_1), ..., x = r_n (mod m_n)           (*)
 *
 * where = stands for congruent and m_1, m_2, ..., m_n are relatively
 * prime in pairs. Let for i = 1,2,...n N_i be the product of all the
 * moduli _except_ the ith one, and let A_i be its inverse modulo m_i.
 * (This exists by the hypothesis.) A_i may be reduced modulo m_i but
 * N_i may not. Then a solution x is given by
 *
 * 	x = (A_1 N_1)r_1 + (A_2 N_2)r_2 + ... + (A_n N_n)r_n
 *
 * For, modulo m_i, all terms but the ith are divisible by m_i. Moreover,
 * the factor (A_i N_i) is congruent to 1 modulo m_i. 
 *
 * It is simple to check that any two solutions of (*) differ by a multiple
 * of the lcm of the m_i. Conversely, adding any multiple of the lcm 
 * clearly produces another solution. 
 *
 * Returns -1 if there is no solution (e.g., when the necessary hypotheses
 * on the moduli is not met.) Returns the value of the lcm through the
 * last pointer argument, if not null.
 *
 */

integer chinese(integer *residues, integer *moduli, int n, integer *lcm)
{
	int i,j;
	integer rval=0, pp,a,b,x,y,my_lcm=1;  /* rval = solution x */

	/* Sanity checks */
	if(n<1)return -1;
	if(residues == NULL)return -1;
	if(moduli == NULL)return -1;

	if(n == 1) 
		return normalize(residues[0],moduli[0]);

	/* if n > 1: */	
	for(i=0;i<n;i++){

		pp = 1;  /* initialize a partial product */

		/* Compute N_i (see above) */
		for(j=0;j<n;j++)
			if(j!=i)pp *= moduli[j];

		/* Compute A_i */
		a = associate(pp,moduli[i]);
		if(a == -1)return -1;

		/* Gather up solution x */
		rval += a*residues[i]*pp;

		/* Gather up lcm of the moduli as we go */
		b = my_gcd(pp,moduli[i],&x,&y);
		if(b==-1)return -1; /* probably cannot happen */
		my_lcm *= moduli[i]/b;

	}

	if(lcm)*lcm = my_lcm;
	return normalize(rval,my_lcm);
}
