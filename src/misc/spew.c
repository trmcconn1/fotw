/* spew.c: throw 1 copy of each printable ascii character (32-126) into a file.
   Usage: spew filename
*/

/* Quick and dirty: no error or arg testing. Beware. */

#include<stdlib.h>
#include<stdio.h>

int
main(int argc, char **argv)
{
	int i =0;
	FILE *ostream;

	ostream = fopen(argv[1],"wb");
	for(i=32;i<127;i++)fputc(i,ostream);
	fclose(ostream);
	return 0;
}

	
