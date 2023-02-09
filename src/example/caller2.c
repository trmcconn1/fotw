#include<stdio.h>
#include<stdlib.h>

extern void silly(void);

void
caller2(void)
{

	printf("I'm caller2 and I'm calling silly\n");
	silly();
	return;
}
	
