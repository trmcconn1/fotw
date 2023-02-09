/*
* four.c
*
* This program implements a very simple four-function
* calculator. It uses reverse polish notation (RPN).
*/
#include <stdio.h>
#include <stdlib.h>

#define INBUFSIZE 15

int main (void)
{
	double num1, num2;
	int op;
	char inbuf1[INBUFSIZE+1], inbuf2[INBUFSIZE+1];

	/* get numbers and operator from user */
	printf(" First number : ");
	gets(inbuf1);
	num1 = atof(inbuf1);

	printf(" Second number : ");
	gets(inbuf2);
	num2 = atof(inbuf2);

	printf(" Operation : ");
	op = getchar();

	/* do calculation and print remark */
	switch (op) {
	case '+':
		printf("%lf\n",num1 + num2 );
		break;
	case '-':
		printf("%lf\n",num1 - num2 );
		break;
	case '*':
		printf("%lf\n",num1 * num2);
		break;
	case '/':
		if (num2 != (float) 0)
			printf("%lf\n",num1 / num2 );
		else {
			fprintf(stderr,"cannot divide by zero\n");
			exit(EXIT_FAILURE);
		     }
		break;
	default:
		fprintf(stderr, "Unknown operator: %c\n",op);
		exit(EXIT_FAILURE);
	}
	return EXIT_SUCCESS;
}
			
