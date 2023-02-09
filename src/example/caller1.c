#include<stdio.h>
#include<stdlib.h>

extern void silly(void);

void
caller1(void)
{

	printf("I'm caller1 and I'm calling silly\n");
	silly();
	return;
}
	
