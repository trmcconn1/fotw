/* lookat.c - look at a screen-sized piece of a big 2-d array 
stored in the form of a Mathematica list */

#include "zz.h"   /* header for zig-zag problem */
#include <stdio.h>

float u[ROWS][COLS];
int wdw(float,float,int,int);

int main(int argc,char *argv[])
{
	float x,y;
	FILE *fp;

	if(argc == 1){  		/* take input from stdin */
	rdlist(u,ROWS,COLS);

	while(getchar() != EOF){
		printf("NW corner x = ?\n");
		scanf("%f",&x);
		printf(" y = ? \n");
		scanf("%f",&y);
		wdw(x,y,ROWS,COLS);
	}
	}
	else {
	fp = fopen(argv[1],"r");
	if(fp == NULL){ printf("can't open\n");
	return 1;
	}
	frdlist(fp,u,ROWS,COLS); /* get array from file */
	printf("array read from %s :strike any key \n",argv[1]);

	while(getchar() != EOF){                /* using stdin */
		printf("NW corner x = ?\n");
		scanf("%f",&x);
		printf(" y = ? \n");
		scanf("%f",&y);
		wdw(x,y,ROWS,COLS);
	}
        }/* end else */	
	return 0;
}
