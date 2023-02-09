/* This file is to test what seems to be a bug in Borland's implementation
   of dup2 in the run time library under Win32. We change stdin to a file
   that we know exists, list the file to stdout, restore the original stdin
   and try to read from it.

Compile: under unix: cc -o testfd -DUNIX testfd.c
*/

#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#ifndef UNIX
#include<io.h>
#else
#include<unistd.h>
#endif


#define EXISTINGFILE "./testfd.c"
int
main()
{
	FILE *istream;
	char buffer[200];

	/* Dup stdin handle on fd 10 */

	dup2(0,10);
	close(0);

       /* Open a file we know exists and print everything out */

	open(EXISTINGFILE,O_RDONLY);
	istream = fdopen(0,"r");
	while(fgets(buffer,199,istream)!=NULL)
		printf("%s",buffer);

	/* Restore original file descriptor */

	fclose(istream);
	close(0);
	dup2(10,0);

	/* Now make sure we can read from stdin: echo every line
           entered until EOF */
	
	printf("Please enter anything\n");
	while(fgets(buffer,199,stdin)!=NULL)
		printf("%s",buffer);
	return 0;
}

