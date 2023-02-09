/* bin2pc: convert binary data to printable characters in a reversible way */

#include "global.h"

/* bin2pc: Binary to printable. Expand groups of 3 bytes to 4 printable
 * characters. We view the 3 bytes as 4 groups of 6 bits which
 * are taken to be bits of 4 characters. We then add 32 = character space
 * to make the characters printable.   This is the method used by uuencode(1).
 * Return number of output characters. The number of bytes to encode
 * is given by n. If it is not divisible by 3, then there will be 1 or
 * 2 garbage characters at the end of the returned output, so the caller
 * must be able to handle this. 
 *
 * Space characters (32) are treated as a special case: they are converted
 * to character 96 (the first beyond the normal printable range) so the
 * uudecoded file body does not contain any space characters.
 *
 * This could probably be done more elegantly with bit-fields, but I 
 * couldn't figure out how to do it portably. */

/* The reason for encoding 3 binaries as 4 printables is that 4:3 is 
 * a convenient ratio having that property that 96^(4/3) > 256. This 
 * condition ensures that some encoding is possible */

/* Masks to split the "low" 24 bits of a long into disjoint blocks of
 * 6 bits */

static unsigned long c4mask=0x3F;
static unsigned long c1mask=(0x3F << 18);
static unsigned long c2mask=(0x3F << 12);
static unsigned long c3mask=(0x3F << 6);

int bin2pc(unsigned char *input, char *output, int n)
{

	unsigned long temp[3];
	unsigned long tt=0L;
	int i=0,m=0;

	if(!input) return 0;
	if(!output)return 0;

	while(i < n){

		if(i<n) tt = ((unsigned long)input[i++]) << 16;
		if(i<n) tt |= ((unsigned long)input[i++]) << 8;
		if(i<n) tt |= (unsigned long)input[i++];

		output[m]=(unsigned char)(((tt&c1mask)>>18)+32L);
		if(output[m]==32)output[m]+= 64;
		m++;
		output[m]=(unsigned char)(((tt&c2mask)>>12)+32L);
		if(output[m]==32)output[m]+= 64;
		m++;
		output[m]=(unsigned char)(((tt&c3mask)>>6)+32L);
		if(output[m]==32)output[m]+= 64;
		m++;
		output[m]=(unsigned char)((tt&c4mask)+32L);
		if(output[m]==32)output[m]+= 64;
		m++;
	}
	return m;
}

/* Masks to split the "low" 24 bits of an unsigned long into 3 unsigned
 * chars */

static unsigned long lomask = 0xFF;
static unsigned long midmask = (0xFF << 8);
static unsigned long himask = (0xFF << 16);

/* This does the opposite of bin2pc. Note that it is important to use
 * unsigned characters to prevent the compiler from mucking around with
 * the high bit. */

int pc2bin(char *input, unsigned char *output, int n)
{

	unsigned long in[4];
	unsigned long tt=0L;
	int i=0,m=0;

	if(!input) return 0;
	if(!output)return 0;

	while(i < n){
		if(i<n)in[0] = ((unsigned long)input[i++])-32L;
		if(in[0]==64L) in[0] -= 64L;
		if(i<n)in[1] = ((unsigned long)input[i++])-32L;
		if(in[1]==64L) in[1] -= 64L;
		if(i<n)in[2] = ((unsigned long)input[i++])-32L;
		if(in[2]==64L) in[2] -= 64L;
		if(i<n)in[3] = ((unsigned long)input[i++])-32L;
		if(in[3]==64L) in[3] -= 64L;

		tt = (in[0]<<18) | (in[1]<<12) | (in[2]<<6) | in[3];

		output[m++]= (unsigned char)((tt & himask) >> 16);
		output[m++]= (unsigned char)((tt & midmask) >> 8);
		output[m++]= (unsigned char)(tt & lomask);
	}
	return m;
}

