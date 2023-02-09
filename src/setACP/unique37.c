/* Uniqueness check for ACP problem 17 */

#include<stdio.h>
#include<stdlib.h>


int A[]={2,3};
int B[]={-5};
int C[]={-2,2};
int Ds[]={-5,-4,4,5};
int E[]={-1,1};

int main(){

	int ia,ib,ic,id,ie;
	int D;
	int a,b,c,d,e;
	int E1,E2,E3,E4,E5;

	for(ia=0;ia<2;ia++)
	for(ib=0;ib<1;ib++)
	for(ic=0;ic<2;ic++)
	for(id=0;id<4;id++)
	for(ie=0;ie<2;ie++){
		a = A[ia];
		b = B[ib];
		c = C[ic];
		d = Ds[id];
		e = E[ie];
		E1 = 3*a*e*e + 2*b*e + c; 
		E2 = 3*a*e*e + 2*b*e + c + d; 
		E3 = a*e*e*e + b*e*e + c*e + d;
		E4 = a*e*e + b*e + c;
		E5 = 2*a*e*e + b*e + c;
		D = (E1-E3)*(E1-E4)*(E1-E5); /* E1 != E2 */
		if(D == 0){
			printf("You lose 1: %d%d%d%d%d\n",ia,ib,ic,id,ie);
			exit(1);
		}	
		D = (E2-E3)*(E2-E4)*(E2-E5);
		if(D == 0){
			printf("You lose 2: %d%d%d%d%d\n",ia,ib,ic,id,ie);
			exit(1);
		}	
		D = (E3-E4)*(E3-E5);
		if(D == 0){
			printf("You lose 3: %d%d%d%d%d\n",ia,ib,ic,id,ie);
			exit(1);
		}	
		D = (E4-E5);
		if(D == 0){
			printf("You lose 4: %d%d%d%d%d\n",ia,ib,ic,id,ie);
			exit(1);
		}	

	}
	printf("You win\n");
	return 0;
}

