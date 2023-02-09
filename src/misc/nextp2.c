/* nextp2.c: Puzzle solution: find a single expression that will convert
 * an int into the next higher power of 2.
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


int
main(int argc, char **argv)
{
	unsigned x,y;

	if(argc != 2) {
		fprintf(stderr,"Usage: nextp2 <nnn>\n");
		return 1;
	}
	x = atoi(argv[1]);

	if(x > UCHAR_MAX){
		fprintf(stderr,"%u is too big. Max is %u.\n",x,UCHAR_MAX);
		return 1;
	}

	/* The guts of it: */
	y = (x|x>>1|x>>2|x>>3|x>>4|x>>5|x>>6|x>>7) + 1 ;
	
	printf("x = %u, least power of 2 > x = %u\n",x,y);
	return 0;
}
