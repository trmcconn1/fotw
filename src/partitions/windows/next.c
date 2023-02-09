/* next: generate the "next" partition in a global array. The array holds
N numbers in the range 0 to N which tell how many times to repeat each
of the parts. These reflect the common exponential notation representation
of a partition, as, e.g. 5^33^12^31^6 for 5+5+5+3+2+2+2+1+1+1 */

#include<stdio.h>

#define N 254

/* The array of exponents */

unsigned char E[N+1];

void initializeE(void){
	unsigned char i;
	for(i=0;i<N;i++)E[i]=0;
}

int filter_arg = 0;

int filter(int(*f)(unsigned char (*)[],int,int),int n)
{
	return f(&E,n,filter_arg);
}

/* This is for debugging */

void dumpE(void){
	unsigned char i = 0;
	for(i=0;i<N;i++)printf("%u",E[i]);
	printf("\n");
}

void listE(void){

	unsigned char i = 0,j,n;
	int plustoggle = 0; /* prevents leading + */

/* find most significant digit */

	n = N;
	while(n>0){
		if(E[n])break;
		n--;
	}

	for(i=0;i<=n;i++){
		if(E[i]==0)continue;
		if(plustoggle == 0){
			plustoggle = 1;
			printf("%d",i+1);
		}
		else printf("+%d",i+1);
		for(j=1;j<E[i];j++)printf("+%d",i+1);
	}
	printf("\n");
}

/* The main routine of this module. If we consider E to be the representation
of a number in base N+1, with most significant digit on the right,
then it just adds 1 to number with appropriate carry. Returns the
column number that was increased */

unsigned char next(void){

	unsigned char j=0;

	while(j<N){
		if(E[j] < N){
			E[j]++;
			return j;
		}
		E[j] = 0; /* and carry */
		j++;
	}
	return N;
}	

/* Zero out jth number in E and add a carry digit
  to the next entry. Returns index of number increased */

unsigned char jump(unsigned char j){

	if(j>N)return N+1;
	E[j]=0;
	if(j == N)return N+1;
	j++;
	while(E[j] == N){
		E[j]=0;
		if(j == N)return N+1;
		j++;
	}
	E[j]++;
	return j;
}

/* Returns the number represented by the partition in E */

unsigned long E2Num(){

	unsigned char j;
	unsigned long S = 0L;
	
	for(j=0;j<N;j++)
		S += ((unsigned long)(j+1))*((unsigned long)E[j]);
	return S;
}
