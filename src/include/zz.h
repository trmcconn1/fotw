/* zz.h header file for zig-zag martingale problem */
 
#define XO -1    /*abscissa of lower left corner of bounding box
*/ 
#define YO 1     /* ordinate of lower left corner of box */ 
#define X1 1      /* "  " upper right " " " " */ 
#define Y1 3      /* etc */ 
#define SCALE 100 /*pgm units (i,j) per true unit */ 
#define ROWS (X1 - XO)*SCALE + 1 
#define COLS  (Y1 - YO)*SCALE + 1   
#define ITERATIONS 100000

void prtarray(float u[][],int rows, int cols);
void prtlist(float u[][], int, int);
int rdarray(float u[][],int, int);
int rdlist(float u[][],int,int);

