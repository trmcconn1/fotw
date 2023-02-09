/* dxplus - right difference op. on 2-d array. Equals left diff
on right boundary */ 
 
#include "zz.h"

int dxplus(float u[][COLS]) 
{ 
     float hold[ROWS][COLS]; 
     int i = 0, j= 0; 

     if(ROWS <= 1) return 1; /* ROWS out of range error */
 
 
     for(i=0;i<=ROWS-1;i++){ 
 
          for(j=1;j<COLS-1;j++) 
                
          hold[i][j] = u[i][j+1]-u[i][j];

	  hold[i][COLS-1]=       /* handle last col */
	  u[i][COLS-1]-u[i][COLS-2];
          } 
 
 
      /* transfer temporary holder for new array */ 
      for(i=0;i<ROWS;i++){ 
          for(j=0;j<COLS;j++) 
           u[i][j] = hold[i][j]*SCALE; 
     } 
 
 
     return 0; 
}
/* dyplus - upward difference op. on arrays. Equals down diff at top
row */
 
 
int dyplus(float u[][ COLS]) 
{ 
     float hold[ROWS][COLS]; 
     int i = 0, j= 0; 
 
     if(COLS <= 1) return 1; /* columns out of range error */

     for(j = 0;j<=COLS-1;j++) /* handle top row */ 
          hold[0][j] = u[0][j] - u[1][j];
 
     for(i=1;i<=ROWS-1;i++){ 
 
          for(j=0;j<=COLS-1;j++) 
                
          hold[i][j] = u[i-1][j]-u[i][j];
          } 
 
 
      /* transfer temporary holder for new array */ 
      for(i=0;i<ROWS;i++){ 
          for(j=0;j<COLS;j++) 
          u[i][j] = hold[i][j]*SCALE; 
     } 
 
 
     return 0; 
}
