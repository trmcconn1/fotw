#include<stdio.h>
#include<stdlib.h>



int main()
{
	int c;

	while((c=getchar())!=EOF){
		if(c=='\n')
			putchar('\r');
		putchar(c);
	}
	return 0;
}

