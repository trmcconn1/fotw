/* partio.c: Calculate the partition function
 * 
 *
 * 	By Terry R. McConnell
 */

/* 

Theory:

The partition function p(n) equals the number of ways to write n as a sum
of naturnal numbers, or, equivalently, the number of ways to distribute n
indistinguishable balls into indisttinguishable boxes. If the boxes are 
numbered, or the order of summands is tracked, then we obtain distributions 
rather than partitions. These are much simpler to count, and indeed there
are exactly 2^n distributions in total.

We will compute p(n) using a recursion formula that is based on Euler's
pentagonal number theorem. Acccording to this theorem, for most values of n 
there are the same number of partition of n into an odd number of 
distinct parts as there are into an even number of distinct parts. Denote
these two sets of partitions by O(n) and E(n), respectively. The idea for
a bijectiion f:O(n) -> E(n) is due to Franklin and can be described with 
two examples:

******            f                           ******x
****          ---------->                     ****x
xx       

A very small part at the bottom (shown with x's) can be split up and the pieces
added to the ends of the long parts, thus flipping the parity of the number
of parts involved (3 --> 2 here) without changing the fact that they are 
distinct. This works as long as the shortest part is smaller than the number
of parts above it. If not, one uses a different map: remove the ends of the
longest parts and use them to form a new smallest part:

********      f            *******x
*******    <-----          ******x
****                       ****x 
xxx

There are, however, two troublesome configurations:

*****        ******
****    AND  *****
***          ****

Neither form of f will work when either (a) the rightmost diagonal is the same
length as the shortest part, or (b) one less. However, (a) only happens if
n is a pentagonal number of the form j(3j-1)/2, and (b) only happens if n is
a pentagonal number of the form j(3j+1)/2.  (See below for more on pentagonal
numbers). In the example j = 3, an odd number, and the example configurations
provide an extra partition of the n into an add number of distinct parts.
When j is even, for example in the pair with j = 2, n = 5 and n = 7
the offending patterns give an extra partition into an even number of 
parts:

***         ****
**    AND   ***

Thus, we have THEOREM (Euler) |O(n)| = |E(n)| + e(n) where e(n) = 0 if n
is not a pentagonal number, e(n) = -1 if n is pentagonal number with an
even j, and e(n) = +1 if n is a pentagonal number with an odd j. 

It is remarkable that this theorem can be used to compute the partition 
function since it talks only about partitions of a very special type. 
Here is a lovely idea sketched in exercise 39 of Integer Partitions, by 
G.E. Andrews and K. Eriksson.  Introduce a bijection between PxO and PxE
where P is the set of all partitions, O is the set of all partitions into
an odd number of distinct parts, and E is the set of all partitions into
an even number of distinct parts. For a given element (p,o) of PxO, identify
the smallest part in either p or o. If it is in p, move it to o. If it is
in o, move it to p. If there is a tie, move to p. In all cases, some part
gets moved to or from o, so its parity changes. When o is added to, it is only
by adding a part from p that is smaller than any originally in o, so o will
always end up containing distinct parts. Let us name this mapping g. We
must show that it is one-one. Suppose g(p1,o1) = g(p2,o2) = (p3,e). If e > o1
(has more parts) then e = o1,k (k is a part added onto o1) and p1 = p3,k. Here
k is the smallest part of p1 and smaller than any part in p1. If also
e > o2 then the same reasoning immediately leads to (p1,o1) = (p2,o2), so
we may assume e < o2. Then o2 = e,l (say) = o1,k,l where l < k and <= any part
of p2. Also p3 = p2,l. In sum, the situation must be as shown:

p1 o1   p2 o2   p3  e
----------------------
p2 o1   p2 o1   p2  o1
k          k    l   k
l          l    

But this is impossible. Since l < k, l would have been moved from p1 instead
of k.  

This argument shows that p(n-k)x|O(k)| = p(n-k)x|E(k)|.  Subtracting and
summing on k we get that 0 = summation on k of e(k)p(n-k). Since only
terms with k pentagonal are non-zero, we can solve this for p(n), getting
p(n) = p(n-1) + p(n-2) - p(n-5) - p(n-7) + p(n-12) + p(n-15) - ...   

 */

/* compile: cc -o partio partio.c


      Use -D_SHORT_STRINGS if your compiler does not support multiline
          string constants.

      Select the proper choice of N_MAX based on your ULONG_MAX (See below)
	or run catalan -c to see the proper compile line
*/


#include "partio.h"

/* The safe value is a bit conservative and we can get away with one more
on a 32 bit machine: */
#ifndef N_MAX
#define N_MAX 19   /* ULONG_MAX = 2^32 = 4294967295 */
#endif
#if 0
#define N_MAX 5    /* ULONG_MAX = 2^8 = 64 */
#define N_MAX 9    /* ULONG_MAx = 2^16 = 65536 */
#define N_MAX 34   /* ULONG_MAX = 2^64 */
#define N_MAX 68   /* ULONG_MAX = 2^128 */
#endif     

#define VERSION "1.0"
#define USAGE "partition [-c -p -t -h -v] n"
#ifndef _SHORT_STRINGS
#define HELP "\n\npartition [-c -t -h -v ] n\n\n\
Print the number of partitions of n\n\n\
-c: Give compilation advice based on installation limits\n\
-t: Print a table of of the partition function\n\
-p: Apply operation to pentagonal numbers rather than partitions\n\
-v: Print version number and exit. \n\
-h: Print this helpful information. \n\n"
#else
#define HELP USAGE
#endif


/* The pentagonal numbers are the double sequence j(3j-1)/2 and j(3j+1)/2 for
j = 1,2, ...   i.e., 1,2,5,7,12,15,22,26,....   Successive terms in each
sequence are obtained by adding to the previous one numbers that grow by
3 each time. Thus, 4,7,10,..., and 5,8,11,....   This is an extension of
the "figurate number" concept: adding successive natural numbers give the
triagular numbers, successive odds (grow by 2 each time) give the square
numbers. Geometricially, the added sequence form successive layers called
gnomons, like the layers in a tree or hailstone, that wrap around the 
boundary of the previous number. Each additional "corner" in the figure
adds one to the gap between gnomons. */

#define PENTAGONAL(n) ((n)%2?(((n)+1)/2)*(3*(((n)+1)/2)-1)/2:((n)/2)*(3*((n)/2)+1)/2)


/* Recursive routine to calculate the partition function
Uses global array C defined in main.c
   C[0] must be intitialized to 1 before this is called for the first time */


unsigned long partitions(int n)
{

	int j=1;
	unsigned long rval = 0L;

	/* avoid recursive overhead by storing and using values
           computed already */

	if(C[n])return C[n];

	while(PENTAGONAL(j) <= n){
		if((j%4 == 1)||(j%4 == 2))
			rval += partitions(n - PENTAGONAL(j));
		else rval -= partitions(n - PENTAGONAL(j));
		j++;
	}

	C[n] = rval;
	return rval;
}
