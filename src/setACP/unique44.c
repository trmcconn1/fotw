/* Uniqueness check for ACP problem 44 */

#include<stdio.h>
#include<stdlib.h>


int A[]={16,16,16};
int B[]={1,3,4,5};
int C[]={11,13,14,15,17,19,20};

int main(){

	int i,j,k;
	int a,b,c;
	int E1,E2,E3,E4,E5;

	for(i=0;i<3;i++)
		for(j=0;j<4;j++)
			for(k=0;k<7;k++){
				a = A[i];
				b = B[j];
				c = C[k];
				E1 = c + a*b*b;
				E2 = c;
				E3 = a*b*b;
				E4 = 2*b*a + c;
				E5 = b;
				if((E1==E2)||(E1==E3)||(E1==E4)||(E1==E5)||
						(E2==E3)||(E2==E4)||(E2==E5)||
						(E3==E4)||(E3==E5)||
						(E4==E5)){
					printf("You lose 1 i,j,k = %d %d %d\n", i,j,k);
					exit(1);
				}

			}
	printf("You win!\n");
	return 0;
}

