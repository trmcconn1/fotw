/* Example for mat 541: using fermat's theorem to show that 2^19+1 is not
prime */


#include<stdio.h>
#include<stdlib.h>

int main(void)
{

	long long n=2,i,j;
	long long  N = 524289;


	for(i=1;i<=19;i++){
		n *= n;
		n = n % N; 
		printf("i=%lld, 2^2^i = %lld\n",i,n);
	}
	return 0;
}

	
