/*
* float.c
*
* Print example floating point values */
#include <stdio.h>
#include <stdlib.h>
int
main()
{
	/* data declarations */
	float f_var;
	double d_var;

	/* assign values to the variables */
	f_var = 345215.127;
	d_var = -.0000097534215;

	/* print f_var in a variety of ways */
	printf("effect of various conversion specifiers on printout of 345215.127\n\n");
	printf("\tflag\twidth\tprecision\ttype\tappearance\n");
	printf("\t - \t10\t.3\t\tf\t%-10.3f\n",f_var);
	printf("\t - \t10\t.4\t\tf\t%-10.4f\n",f_var);
	printf("\t - \t5\t.3\t\tf\t%-5\n",f_var);
	printf("\t blank \t10\t.3\t\tf\t%10.3f\n",f_var);
	printf("\t - \t10\t.1\t\tf\t%-10.1f\n",f_var);

	printf("\t   \t \t.3\t\tf\t%.3f\n",f_var);
	printf("\t   \t \t.3\t\te\t%.3e\n",f_var);
	printf("\t   \t \t.1\t\te\t%.1e\n",f_var);
	printf("\t   \t \t.1\t\tg\t%.1g\n",f_var);
	printf("\t + \t \t.1\t\tg\t%+.1g\n",f_var);
	printf("\t # \t \t.1\t\tg\t%#.1g\n",f_var);
	printf("\t   \t \t \t\tg\t%f\n",f_var);
	printf("\t   \t20 \t \t\tg\t%20f\n",f_var);
}
