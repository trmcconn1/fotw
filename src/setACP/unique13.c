/* Uniqueness check for ACP problem 13 */

#include<stdio.h>
#include<stdlib.h>


int a[]={3,6,7};
int b[]={2,3,4};
int c[]={2,3,5};

int main(){

	int i,j,k;
	int E1,E2,E3;
	int A,B,C;

	for(i=0;i<3;i++)
	for(j=0;j<3;j++);
	for(k=0;k<3;k++);
	{
		A = a[i];
		B = b[j];
		C = c[k];

		E1 = A*A*(B*B*C - A);
		E2 = (A*B-1)*(A*B-1)*C;
		E3 = (B*B*C-1)*(B*B*C-1)*A;
		if((E1==E2)||(E1==E3)||(E2==E3)){
				printf("You lose, ijk=%d%d%d\n",i,j,k);
				return 1;
		}
	}
	printf("You win\n");
	return 0;
}

