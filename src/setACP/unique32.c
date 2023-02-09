/* Uniqueness check for ACP problem 32 */

#include<stdio.h>
#include<stdlib.h>


int a[]={-2,1,2};
int b[]={-3,7,4};
int c[]={-7,8,5};
int d[]={-1,-5,6};

int main(){

	int i,j,k,l;
	int E1,E2,E3,E4;
	int A,B,C,D;
	int E,F;

	for(i=0;i<3;i++)
		for(j=0;j<3;j++)
			for(k=0;k<3;k++){
				for(l=0;l<3;l++);
				A = a[i];
				B = b[j];
				C = c[k];
				D = d[l];
				E1 = (C-A)*(C-A) + (D-B)*(D-B);
				E2 = (B-A)*(B-A) + (D-C)*(D-C);
				E3 = (C-B)*(C-B) + (D-A)*(D-A);
				E = C > A ? C-A : A-C;
				F = D > B ? D-B : B-D;
				E4 = E + F;
				if((E1==E2)||(E1==E3)||(E1==E4)
				  ||(E2==E3)||(E2==E4)||(E3==E4)){
					printf("You lose i,j,k,l = %d %d %d %d\n", i,j,k,l);
					return 1;
				}
			}
	printf("You win!\n");
	return 0;
}

