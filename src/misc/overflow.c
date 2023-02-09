/* Question from Vince Fatica: how to tell ahead of time, in a portable way,
 whether an addition will give "overflow" */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#ifndef __STDC__ 
#error Compiler appears not to be ANSI compliant!
#endif

#define LO_MASK ((UINT_MAX)>>1)  /* binary: 011..1 */  
#define HI_MASK (~LO_MASK)       /* binary: 100..0 */

/* The following definitions are to handle overflow in unsigned arithmetic */
/* N.B.: x,y below must be unsigned types to prevent compiler conversion */
/* We rely on the fact that overflow will happen in x+y if and only if at least
   two of the following conditions occur:
	- highest bit of x is set
	- highest bit of y is set
	- there is a carry into the highest bit position
*/

#define HI_BIT_SET(x) (((x)&HI_MASK)?1:0)
#define HI_CARRY(x,y) (HI_BIT_SET(((x)&LO_MASK) + ((y)&LO_MASK)))

#define WILL_OVERFLOW(x,y) (HI_BIT_SET((x)) + HI_BIT_SET((y)) + \
HI_CARRY((x),(y))) >= 2


/* The next routine returns 1 iff the operation on signed quantities
   would overflow */

#define ABS(x) (x) > 0 ? (x) : -(x)

int
will_overflow(int x, int y)
{
	unsigned a,b,c;
	
	if((x >= 0) && (y <= 0)) return 0; /* Can't overflow here */
	if((x <= 0) && (y >= 0)) return 0; /* Can't overflow here */ 

	a = ABS(x);
	b = ABS(y);
	c = a + b;

	if((x>0)&&(y>0))
		if( c > INT_MAX ) return 1;
	if((x<0)&&(y<0))
		if( c > (unsigned)INT_MIN) return 1;

	return 0;
}


int
main(void){

	unsigned i;
	int x,y,z;
	
	printf("Testing overflow in unsigned addition\n");
	for(i=0;i<10;i++){
		if(WILL_OVERFLOW((unsigned)UINT_MAX - 5U,i))
			printf("UINT_MAX - 5 + %d will overflow\n",i);
		else
			printf("UINT_MAX - 5 + %d will not overflow\n",i);
	}

	printf("Testing overflow in signed addition\n");
	printf("Testing overflow above INT_MAX\n");
	for( z = 0; z < 10; z++)
		if(will_overflow(INT_MAX-5,z))
			printf("INT_MAX -5 + %d will overflow\n",z);
		else
			printf("INT_MAX -5 + %d will not overflow\n",z);
	printf("Testing overflow under INT_MIN\n");
	for( z = 0; z > -10; z--)
		if(will_overflow(INT_MIN+5,z))
			printf("INT_MIN + 5 + %d will overflow\n",z);
		else
			printf("INT_MIN + 5 + %d will not overflow\n",z);
			
	return 0;
}
