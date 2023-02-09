/* dotproduct : form dot product of first field in two files
 usage: dotproduct file1 file2
 prints dot product on stdout if valid
 returns 1 if dimension error
*/

#include <stdio.h>
#include <stdlib.h>
#define PRINTFORMAT "%.2f\n"

int main( int argc, char *argv[] )
{
	FILE *file1, *file2;
	float a,b;
	float sum = 0.0;

	file1 = fopen( argv[1], "r");
	file2 = fopen( argv[2], "r");

	while (  fscanf( file1, "%f", &a) != EOF ) {
		if ( fscanf (file2, "%f", &b) == EOF )	
			exit(1);
		else {
			sum = sum + a*b;
		}
		
	}  /* make sure files are the same length */
	if ( fscanf( file2, "%f", &b) != EOF )
		exit(1);
	else {
		printf(PRINTFORMAT, sum);  /* print answer */
		fclose(file1);
		fclose(file2);
		exit(0);
		}
}
	
