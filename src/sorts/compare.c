/* comparison routines */

int compares = 0; /* Each routine should increment this when called */


int cmp_int(const int *x, const int *y)
{
	int i,j;

	compares++;

	i = *x;
	j = *y;

	if(i<j) return -1;
	if(i == j)return 0;
	return 1;
}
