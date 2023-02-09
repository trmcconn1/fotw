/* test platform for zigzag problem utilities */

#include <stdio.h>
#include "zz.h"


main()
{
	float u[ROWS][COLS];
	int rows;
	
       	rows = rdlist(u,ROWS,COLS);
	dxplus(u);
	dxplus(u);
	prtlist(u,rows,COLS);
}
