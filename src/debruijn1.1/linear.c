/* Linear: generate deBruijn strings from linear recurrences

It is remarkable that deBruijn strings can be generated from certain linear 
recurrences. To generate an order n deBruijn string, begin with an initial string
x_1...x_n = 100...0. For k > n let x_k = c_1 x_(k-1) + c_2 x_(k-2) + ... 
+ c_n x_(k-n), where the operations are done modulo 2. The coefficients c_1, ...
c_n need to be chosen so that the characteristic polynomial f(x) = 1 + c_1 x + 
... + c_n x^n is irreducible in Z2[x] and so that the smallest exponent p such that
f(x) divides ( 1 - x^p) is p = 2^n - 1. The resulting sequence x_n will then have 
period p. Even if p is maximal, it is not quite a deBruijn string, since the all zero sequence is 
omitted, but every other string of order n appears. The fix is just to insert an 
extra zero in the string of n-1 zeros. 

See pp 30-43 of Golomb for the theory. Also of interest are the tables on pp 62-65
that list all suitable characterstic polynomials through order n = 11. The 
coefficients of f are given as an octal bitmap. Thus, for example, x^5 + x^3 +
x^2 + x + 1 is binary 101,111 or 57 in octal.

*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debruijn.h"

static char buffer[MAX_STRING];


/* code is an integer in octal that specifies the coefficients
   in the linear recurrence. See above for references.
   BUG: this routine does not check to make sure that the code will really
   produce a maximal period. */

char *linear(unsigned code, int level){

	int n,i,j,m,s;
	static int c[MAX_LEVEL]; /* coefficients extracted from code */

	n = level;
	m = power2(n);

	if((n > MAX_LEVEL)|| ( n < 1)){
		warn("linear called with level out of range\n");
			return NULL;
	}

	/* extract coefficients from code */

	for(i=0;i<=n;i++){
		c[i] = ( code & 1 ) ? 1 : 0;
		code>>=1;
		
	}
	
	/* prepare initial state */

	buffer[0] = '1';
	for(i=1;i<n;i++)
		buffer[i] = '0';

	/* main loop: Generate successive terms */

	while(i < m + n - 1){
		 
		s = 0;

		for(j=1;j<=n;j++)
			if(buffer[i-j]=='1')s += c[j];
		if(s%2)buffer[i] = '1';
		else buffer[i] = '0';

		i++;
	}
	/* properly string-terminate buffer */
	buffer[i] = '\0';

	/* Change the initial 1. This is needed because
	linear recursion does not produce the all zero string. The string
	generated so far begins and ends with 100...0. To provide the
	missing all zero string, we simply replace the leading 1 with a 0. */

	buffer[0] = '0';
	return (char *)buffer;

} 