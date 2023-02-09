/* Uniqueness check for ACP problem 39 */

#include<stdio.h>
#include<stdlib.h>


int D[]={-2,4};
int E[]={1,7};

int main(){

	int i,j,k;
	int a,b,d,e;
	int E1,E2,E3;

	for(i=0;i<2;i++)
		for(j=0;j<2;j++)
			for(k=0;k<5;k++){
				d = D[i];
				e = E[j];
				switch(k){
					case 0:
						a = 0,b=1;
						break;
					case 1:
						a = -1,b=0;
						break;
					case 2: 
						a = -1,b=1;
						break;
					case 3:
						a = -2,b=0;
						break;
					case 4:
						a = 0, b = 2;
						break;
					case 5:
						a = -2, b = 2;

				}
				E1 = 2*(b*b*b-a*a*a) + 3*d*(b*b-a*a)/2 
					+ 3*e*(b-a);
				E2 = 2*(b-a)*(b-a)*(b-a) + 3*d*(b-a)*(b-a)
					+ 3*e*(b-a);
				if(E1 == E2){
					printf("You lose 1 i,j,k = %d %d %d\n", i,j,k);
					exit(1);
				}

				E1 = 4*(b-a)+d;
				E2 = 2*(b*b-a*a)+d*(b-a)+e;
				E3 = (b*b*b-a*a*a) + d*(b*b-a*a) + e*(b-a);

				if((E1==E3)||(E2==E3)){
					printf("You lose 2 i,j,k = %d %d %d\n", i,j,k);
					return 1;
				}
			}
	printf("You win!\n");
	return 0;
}

