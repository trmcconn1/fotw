/* find least biconcave majorant of a function v on a domain
defined 
by a membership test (insideQ) */ 
/* The domain is a subset of a bounding box */ 
 
#include <stdio.h> 
#include "zz.h"
 
int zzopt(float arry[][]); 
float max(float , float ); 
char insideQ(int i, int j); 
float v( int i, int j); 
 
main()  /* control iteration of zzopt, i/o */
{ 
     float u[ROWS][COLS]; 
     int i,j; 
     int count;
      
     for(i=0;i<ROWS;i++){ 
          for(j=0;j<COLS;j++)   /* initialize the array */ 
          u[i][j] = v(i,j); 
     } 
	for(count=0;count <ITERATIONS ;count++)
          zzopt(u);
          prtlist(u,ROWS,COLS);
} 
 
/* zzopt - replace array with array of maxima of elts with
vertical 
and horizontal averages. (vert or hor. only on boundaries) */ 
 
/*with membership test: change array only on a subset defined by
a membership 
test given by function insideQ */  
 
int zzopt(float u[ROWS][COLS]) 
{ 
     float hold[ROWS][COLS]; 
     int i = 0, j= 0; 
 
     for(j = 1;j<COLS-1;j++) /* handle top row */ 
          hold[0][j] = max((u[0][j+1]+u[0][j-1])/2.0,u[0][j]); 
     hold[0][0] = u[0][0]; 
     hold[0][COLS-1] = u[0][COLS-1]; /* no change at corners */ 
 
     for(i=1;i<ROWS-1;i++){ 
 
          hold[i][0] = max((u[i+1][0]+u[i-1][0])/2.0,u[i][0]); 
                  /* handle first col */ 
          for(j=1;j<COLS-1;j++) 
                
          hold[i][j] = max((u[i+1][j]+u[i-1][j])/2.0,  
                         max((u[i][j+1]+u[i][j-1])/2.0,u[i][j]));
	  hold[i][COLS-1]=       /* handle last col */
	    max((u[i+1][COLS-1]+u[i-1][COLS-1])/2.0
	    ,u[i][COLS-1]); 
          } 
 
     for(j = 1;j<COLS-1;j++) /* handle bottom row */ 
       hold[ROWS-1][j] =
max((u[ROWS-1][j+1]+u[ROWS-1][j-1])/2.0,u[ROWS-1][j]); 
     hold[ROWS-1][0] = u[ROWS-1][0]; 
     hold[ROWS-1][COLS-1] = u[ROWS-1][COLS-1]; 
 
      /* transfer temporary holder for new array */ 
      for(i=0;i<ROWS;i++){ 
          for(j=0;j<COLS;j++) 
          if(insideQ(i,j))      /* OK, I know this is inefficient
*/        u[i][j] = hold[i][j]; 
     } 
 
 
     return 0; /* put something meaningful here later ? */ 
} 
 
/* max - return the maximum of two floating point arguments */ 
 
float max(float a, float b) 
{ 
     return (a > b) ? a : b; 
} 
 
/* insideQ - return 1 if point (i,j) is inside a subset of array 
*/ 
/* change code to get desired subset */ 
 
char insideQ(int i, int j) 
{ 
     float x,y; 
      
     x = (float) XO + ((float) j)/((float) SCALE); 
     y = (float) YO + ((float)(ROWS - i-1) )/((float) SCALE); 
     if (y + 1.0/(2.0*SCALE)< x+2.0 && x <=1.0 && y >= 1.0) /* change condition */ 
          return 1;                   /* for other domains */    
     else return 0; 
} 
 
/* v - the function whose least biconcave majorant is to be found
*/ 
/* its values determine the initial array */ 
 
float v(int i, int j) 
{ 
     float x,y; 
      
          x = (float) XO + ((float)(j))/((float) SCALE); 
          y = (float) YO + ((float)(ROWS - i-1) )/((float)
SCALE);         
          if (j == COLS -1) 
             return 7.0/4.0 + (3.0/4.0)*y; /* "" */ 
          else {if (i == ROWS-1) 
             return 7.0/4.0 + (3.0/4.0)*x; /* "" */
                else 
            return max((x+y)*(x+y)/4.0,1.0); /* change for other functions */
	    }     				 		       
} 
