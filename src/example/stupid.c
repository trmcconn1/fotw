#include <stdio.h>
#include<stdlib.h>

extern void caller1(void);
extern void caller2(void);

int
main(void)
{
	caller1();
	caller2();
	return 0;
}
