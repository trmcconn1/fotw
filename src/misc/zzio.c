/* zzio.c - some i/o routines for 2 dimensional arrays */

/* getline: read a line of floats into row, return valid length */
#include <stdio.h>
#include "zz.h"

int getline(float row[], int cols)
{
	int j;
	int flag;

	for (j=0; j <= cols-1 &&(flag = scanf("%f",&row[j])) !=EOF
	   && flag != 0; j++)
	;
	
	return j;
}
/* rdarray - read a sequence of lines from stdin into float array */
/* returns number of valid lines */
/* stops reading when length of line no longer matches previous */

int rdarray(float u[][COLS], int rows, int cols)
{
	int i;
	int flag = 1;
	int temp =1;	

	temp = getline(u[0],cols);
	if(temp == 0)return 0;

	for(i=1;i<rows;i++){
		flag = getline(u[i],cols);
		
		if(flag != temp)
		return i+1;
		temp = flag;
	}
	return i;
}
/* int rdlist- read Mathematica formatted list into float array */

int rdlist(float u[][COLS],int rows, int cols)
{
	int i;
	int j;
	char trash[10];

	/* List begins "{"  */

	scanf("%s",trash);

	/* normal row: "{....},"  */

	for(i=0;i<rows-1;i++){
		scanf("%s",trash);       /* find beginning "{" */

			/* fill row with floats. scan for "number,"
*/
		for(j=0;j<cols-1 ;j++){
			scanf("%f %s",&u[i][j],trash) ;

		}
		 	/* find last No. in row and trailing "}," */

		scanf("%f %s ",&u[i][j],trash); 
		scanf("%s",trash); /* trailing , */
	} /* end of normal row loop */
	  /* Last row is different: ends with "}" only */

		/* fill all but last column */

		for(j=0;j<cols -1;j++){
			scanf("%f %s",&u[i][j],trash);
		}	/* find "last number}" */
		scanf("%f %s",&u[i][j],trash);
		 /* find closing "}" of List. */
		
	scanf("%s ",trash);
	return i;  /* number of validly filled rows */
	
}
 
/* prtlist - print out array of floats in Mathematica input form */ 
 
void prtlist(float arry[ROWS][COLS],int rows, int cols) 
{ 
     int i=0,j=0; 
     
 	printf("{ { ");
     for (i=0;i<ROWS;i++){ 
          for (j=0;j<COLS;j++){
	  if(j < COLS -1) 
              printf("%f ,\n",arry[i][j]);
	   else printf("%f \n",arry[i][j]);
	   }
	   if(i < ROWS -1) 
          printf("} , {");
	  else printf(" } "); 
          }
	  printf(" }\n"); 
} 
		 
		

	
/* prtarray - print out array of floats in matrix form */ 
/* i going down, j across */ 
 
void prtarray(float arry[][COLS],int rows,int cols) 
{ 
     int i=0,j=0; 
 
     for (i=0;i<rows;i++){ 
          for (j=0;j<cols;j++) 
              printf("%4.2f ",arry[i][j]); 
          printf("\n"); 
          } 
} 

/* versions of all of the above  which read or write to files */


/* fgetline: read a line of floats into row, return valid length */
#include <stdio.h>
#include "zz.h"

int fgetline(FILE *fp,float row[], int cols)
{
	int j;
	int flag;

	for (j=0; j <= cols-1 &&(flag = fscanf(fp,"%f",&row[j])) !=EOF
	   && flag != 0; j++)
	;
	
	return j;
}
/* frdarray - read a sequence of lines from stdin into float array */
/* returns number of valid lines */
/* stops reading when length of line no longer matches previous */

int frdarray(FILE *fp, float u[][COLS], int rows, int cols)
{
	int i;
	int flag = 1;
	int temp =1;	

	temp = fgetline(fp,u[0],cols);
	if(temp == 0)return 0;

	for(i=1;i<rows;i++){
		flag = fgetline(fp,u[i],cols);
		
		if(flag != temp)
		return i+1;
		temp = flag;
	}
	return i;
}
/* int frdlist- read Mathematica formatted list into float array */

int frdlist(FILE *fp, float u[][COLS],int rows, int cols)
{
	int i;
	int j;
	char trash[10];

	/* List begins "{"  */

	fscanf(fp,"%s",trash);

	/* normal row: "{....},"  */

	for(i=0;i<rows-1;i++){
		fscanf(fp,"%s",trash);       /* find beginning "{" */

			/* fill row with floats. scan for "number,"
*/
		for(j=0;j<cols-1 ;j++){
			fscanf(fp,"%f %s",&u[i][j],trash) ;

		}
		 	/* find last No. in row and trailing "}," */

		fscanf(fp,"%f %s ",&u[i][j],trash); 
		fscanf(fp,"%s",trash); /* trailing , */
	} /* end of normal row loop */
	  /* Last row is different: ends with "}" only */

		/* fill all but last column */

		for(j=0;j<cols -1;j++){
			fscanf(fp,"%f %s",&u[i][j],trash);
		}	/* find "last number}" */
		fscanf(fp,"%f %s",&u[i][j],trash);
		 /* find closing "}" of List. */
		
	fscanf(fp,"%s ",trash);
	return i;  /* number of validly filled rows */
	
}
 
/* fprtlist - print out array of floats in Mathematica input form */ 
 
void fprtlist(FILE *fp, float arry[ROWS][COLS],int rows, int cols) 
{ 
     int i=0,j=0; 
     
 	fprintf(fp,"{ { ");
     for (i=0;i<ROWS;i++){ 
          for (j=0;j<COLS;j++){
	  if(j < COLS -1) 
              fprintf(fp,"%f ,\n",arry[i][j]);
	   else fprintf(fp,"%f \n",arry[i][j]);
	   }
	   if(i < ROWS -1) 
          fprintf(fp,"} , {");
	  else fprintf(fp," } "); 
          }
	  fprintf(fp," }\n"); 
} 
		 
		

	
/* fprtarray - print out array of floats in matrix form */ 
/* i going down, j across */ 
 
void fprtarray(FILE *fp, float arry[][COLS],int rows,int cols) 
{ 
     int i=0,j=0; 
 
     for (i=0;i<rows;i++){ 
          for (j=0;j<cols;j++) 
              fprintf(fp,"%4.2f ",arry[i][j]); 
          fprintf(fp,"\n"); 
          } 
}
 
