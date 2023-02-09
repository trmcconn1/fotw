/* bord.c: calls assembly program that uses BIOS services to set
	border color 
	
	usage: bord [0-9]
	
	If no command-line argument is given then the user can try
	various border colors interactively
	
	bugs: You should be able to use color values in the range
		0 -15
		
		*/

#include <stdio.h>
#include <stdlib.h>

/*      For reference:    */
#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define YELLOW  6
#define WHITE 7

extern void border(char);  /* Assembly language routine */

int
main(int argc, char **argv)
{
	int color;
	if (argc >= 2) {
		color =(*++argv)[0];     /* get color value from command line*/
		border(color - '0');   /* and set color */
		return 0;
	}

	/*  else go interactive */

	printf("Input a color value, ^Z to quit.\n");
	printf("0-7: black,blue,green,cyan,red,magenta,yellow,white\n");
	printf("Add 8 for a brighter version of the same color. \n");
	while((color=getchar())!=EOF){
	if( color != '\012'){ /* Throw away the newline */
		border(color-'0');
		printf("Try another color? ^Z to quit.\n");
		}
	}
	return 0;
}
