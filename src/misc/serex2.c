/* serex2.c - give example for mat397 illustrating heirarchy of 
rates of growth: n,n^4, 2^n, n! and n^n */

#include <stdio.h>
#include <math.h>
#define TERMS 10

void rowshow(int, char);
double factorial(int);

main()
{
	int n;

	for (n=1;n<=TERMS;n++)
	rowshow(n,6);

	for (n = 11; n<=20;n++)
	rowshow(n,4);
}
 /* rowshow(n) - display above functions of n in a row */

void rowshow(int n, char cols)
{
	double x;

	x = (double) n;
	switch (cols){
		case 6: printf(" %10.0f %10.0f %10.0f %10.0f %10.0f %10.0f \n",
		x,log(x),pow(x,4),pow(2,x),factorial(n),pow(x,x));
		break;

		case 4: printf(" %10.0f %10.0f %10.0f %10.0f \n",x,log(x),pow(x,4),pow(2,x));
		}
}
/* factorial(int) - as its name implies */

double factorial(int n)
{
	int i;
	double f = 1.0;

	for(i=1;i<=n;i++)
		f = f*i;
	return f;

}
