/* Uniqueness check for ACP problem 15 */

#include<stdio.h>
#include<stdlib.h>


int a[]={-2,4,6};
int b[]={3,5,7};
int f[]={10,11,12};

int main(){

	int i,j,k;
	int E1,E2,E3,E4;
	int A,B,F;

	for(i=0;i<3;i++)
		for(j=0;j<3;j++)
			for(k=0;k<2;k++){
				A = a[i];
				B = b[j];
				F = f[k];
				E1 = -F + (A-B);
				E2 = -(A+B)+F;
				E3 = 1-A+F;
				if((E1==E2)||(E1==E3)||(E2==E3)){
					printf("You lose i,j,k = %d %d %d\n",
							i,j,k);
					return 1;
				}
				if(! ((B-A)*(B-A+F) % (B+A)) ){
					E4 = (B-A)*(B-A+F)/(B+A);
					if((E1==E4)||(E2==E4)||(E3==E4)){
						printf("You lose on part 4 ijk=%d%d%d\n",i,j,k);
						return 1;
					}
							
				}
	}
	printf("You win\n");
	return 0;
}

