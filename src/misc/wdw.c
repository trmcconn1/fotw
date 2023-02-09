/* wdw(x,y,r,c) - print out a terminal screen size piece of array 
starting with x,y in NW corner. /*
/* include axes scales across top and lhs */

#define min(A,B) ((A) < (B) ? (A) : (B))

#include <stdio.h>
#include "zz.h"

#define SCREENWIDTH 15
#define SCREENLENGTH 20


int wdw(float x, float y, int rows, int cols)
{
     int i,j,localrows,localcols;
     int i0,j0;
     extern float u[ROWS][COLS];

     /* find indices of (x,y) in the big array u */
     j0 = (int)(SCALE*x - SCALE*XO);
     i0 = (int)(-SCALE*y + SCALE*YO -1 + rows);

     if((j0<0)||(j0>cols-1)||(i0<0)||(i0>rows-1)){
          printf("point out of range \n");
          return 1;
     }

     localcols = min(cols-j0,SCREENWIDTH);
     localrows = min(rows - i0,SCREENLENGTH);

     printf("    "); /* leave 1st col blank for y - axis */
     for(j=0;j < localcols; j++)  /* print x -axis across top */
          printf("%3.1f ",x + ((float) j/(float) SCALE));
	  printf("\n");
     printf("**************************************************************\n");

     /* print out the main part of the table */
     for(i=0;i<localrows;i++){
          printf("%3.1f|",y - ((float)i/(float)SCALE));
          for(j=0;j<localcols;j++)
               printf("%3.1f ",u[i0+i][j0+j]);
          printf("\n");

     }
     return 0;
}              
