/* An example program from Vince that compiles incorrectly under Microsoft
   VC 6 with optimizations turned on. (It compiles correctly w/o optimization.)
   It also seems to compile correctly under gcc with all optimizations turned
   on. 
*/
#include<stdio.h>

int main()
{
	unsigned short d = 0xABCD;
	int jx;
	unsigned char x[2];
	int pos = 0;

	printf("%X\n",d); /* Displays ABCD */

	/* This SHOULD produce the same output */
	for(jx=1;jx>=0;jx--)
		x[pos++] = (d >> (jx*8))&0xFF;

	printf("%02X%02X\n",x[0],x[1]);
	return 0;
}
