/*  Hill.c:  Implementation of the Hill Cypher.
 *
 *  The Hill Cypher, invented by mathematician Lester Hill in the 1920s,
 *  uses matrix multiplication modulo the alphabet size (M) to encrypt blocks
 *  of k successive plaintext characters. One simply treats each character
 *  as a number class mod M and multiplies the column vector of
 *  k such characters by a fixed matrix A determined from the key. The 
 *  resulting column vector is read off as the next k cyphertext characters.
 *  To decode, merely replace A by its inverse.   
 *
 *  This code is interesting because it is one of the more mathematically
 *  sophisticated of the classical (pre WWII) cyphers. It was probably not 
 *  widely
 *  used since it is tedious and error prone to encode and decode by hand.
 *  Since the advent of computers there have been even more sophisticated
 *  methods available. Another point is that the code, while seemingly
 *  rather secure, is actually quite vulnerable to known or probable plaintext
 *  attacks. See Stalling's book for further discussion. 
 *
 *  Other substitution programs in this package use an alphabet of size
 *  95 by not encyphering the tab and newline characters. Here it is far
 *  easier to encrypt those as well since we handle input blocks of a fixed
 *  size and also because 97 is prime, which makes it more likely a key
 *  matrix will be invertible. Accordingly, we also encrypt newline and
 *  tab characters. 
 *
 *  If the final block to be encrypted or decrypted is smaller than the
 *  normal block size (which is typical,) we make new matrices of the
 *  appropriate smaller size using the initial part of the key. 
 *
 *  To construct a matrix from a key, we view the characters of the key
 *  as numbers modulo 97. We fill a square matrix with these numbers by
 *  writing them across the first row and continuing with subsequent
 *  rows until there are no more characters in the key. If this does not
 *  result in a square matrix, we pad with zeros. Since this may well 
 *  produce a singular matrix, we repeat if needed with a modified key. 
 *  Any reasonable method of modifying the key would do, so long as it
 *  is systematic and bound to succeed sooner or later. 
 *  We merely add 1 to the initial key matrix mod 97 until the first time
 *  we obtain a nonsingular matrix. (For this addition the matrix is viewed
 *  as a single integer base 97 with length = the number of entries.) 
 *
 *  There are a number of supporting routines included here to implement
 *  the matrix inversion. 
 *
*/

  
#include "global.h"

/* i,j element of nxn matrix macro */
#define E(A,i,j,n) *((*(A))[(n)*(i)+(j)])
/* Here, the declaration of A is int *(*)[]: pointer to an array of pointers 
 * to int */

static int N; /* We read and encrypt in NxN chunks */


/* my_gcd: return the greatest common divisor of a and b, or -1 if it
 * is not defined. Return through the pointer arguments the integers such
 * that gcd(a,b) = c*a + b*d. (Many libraries implement a gcd, hence the choice
 * of name.)
 */

int my_gcd(int a, int b, int *c, int *d)
{
	int q,r,t,C,D,rval;
	int sgn_a = 1,sgn_b = 1, swap = 0;

	/* Normalize so that 0 < a <= b */
	if((a == 0)||(b == 0)) return -1;
	if(a < 0){
	       	a = -a;
		sgn_a = -1;
	}
	if(b < 0){
	       	b = -b;
		sgn_b = -1;
	}
	if(b < a){
		t = b;
		b = a;
		a = t;
		swap = 1;
	}

	/* Now a <= b and both >= 1. */

	q = b/a;
	r = b - a*q;
	if(r == 0) {
		if(swap){
			*d = 1;
			*c = 0;
		}
		else {
			*c = 1;
			*d = 0;
		}
		*c = sgn_a*(*c);
		*d = sgn_b*(*d);
		return a;
	}

	rval =  my_gcd(a,r,&C,&D);
	if(swap){
		*d = (C-D*q);
		*c = D;
	}
	else {
		*d = D;
		*c = (C-D*q);
	}
	*c = sgn_a*(*c);
	*d = sgn_b*(*d);
	return rval;
}
/* Calculate the inverse mod (M) of the integer passed. Return 0
 * if there is no inverse, which is appropriate since 0 cannot _be_ an
 * inverse. */

int inv(int n,int M){

	int x,y;

	if(my_gcd(n,M,&x,&y) != 1)return 0;
	x = x % M;
	if(x < 0) x += M; 
	return x;
}

void dump_matrix(int *(*A)[],int n)
{
	int i,j;

	for(i=0;i<n;i++){
		for(j=0;j<n;j++)
			printf("%d ",E(A,i,j,n));
		printf("\n");
	}
	printf("\n");
}



/* Pivot nx2n augmented matrix A|B in place around its (k,k) element. We assume
 * things have been set up properly, for example that the (k,k) element
 * itself is 1. Arithmetic is modulo M */

void pivot(int *(*A)[], int *(*B)[],int k,int n,int M)
{
	int i,j,m,a,b;

	for(i=0;i<n;i++){
		if(i==k)continue; /* skip kth row */
		m = E(A,i,k,n);
		for(j=0;j<n;j++){
			a  = (E(A,i,j,n) -= (m*E(A,k,j,n) % M));
			b =  (E(B,i,j,n) -= (m*E(B,k,j,n) % M));
			if(a < 0) a += M;
			if(b < 0) b += M;
			E(A,i,j,n) = a;
			E(B,i,j,n) = b;
		}
	}
}

int **copy_matrix(int *(*A)[],int n)
{
	int *(*p)[], i,  j;

	p = (int *(*)[])malloc(n*n*sizeof(int *));
	if(!p){
		fprintf(stderr,"copy_matrix: cannot allocate memory.\n");
		exit(1);
	}
	for(i=0;i<n*n;i++){
		(*p)[i] = (int *)malloc(sizeof(int));
		if(!(*p)[i]){
			fprintf(stderr,"copy_matrix: cannot allocate memory.\n");
			exit(1);
		}
	}
	for(i=0;i<n;i++)
		for(j=0;j<n;j++)
			E(p,i,j,n) = E(A,i,j,n);
	return (int **)p;
}


/* Return pointer to array of integers representing the inverse
 * of the nxn matrix passed as B mod (M), or NULL if it does not
 * exist. Use the Gauss Jordan procedure */

int **mat_inv(int *(*B)[], int n, int M)
{
	int *(*p)[],i,j,k,l;
	int *(*A)[];
	int temp;

	if(!B) return NULL;
	if(M <= 1)return NULL;

	A = (int *(*)[])copy_matrix(B,n);

	p = (int *((*)[]))malloc(n*n*sizeof(int *));
	if(!p){
		fprintf(stderr,"mat_inv: cannot allocate memory.\n");
		exit(1);
	}
	for(i=0;i<n*n;i++){
		(*p)[i] = (int *)malloc(sizeof(int));
		if(!(*p)[i]){
			fprintf(stderr,"mat_inv: cannot allocate memory.\n");
			exit(1);
		}
	}

	/* Initialize p matrix, which will end up being the inverse */
	for(i=0;i<n;i++)
		for(j=0;j<n;j++)
			if(i != j) E(p,i,j,n) = 0;
			else E(p,i,j,n) = 1;

	/* loop over columns */
	for(j=0;j<n;j++){
		/* Find first invertible entry at or below jth row */
		k = 0;
		for(i=j;i<n;i++)
			if(k=inv(E(A,i,j,n),M))break;
		if(!k)return NULL; /* matrix not invertible */
		if(i != j) { /* Swap rows i and j  and multiply by k */
			for(l=0;l<n;l++){
				temp = E(A,j,l,n);
				E(A,j,l,n) = E(A,i,l,n)*k%M;
				E(A,i,l,n) = temp;
				temp = E(p,j,l,n);
				E(p,j,l,n) = E(p,i,l,n)*k%M;
				E(p,i,l,n) = temp;
			}
		}
		else 
			for(l=0;l<n;l++){
				E(A,i,l,n) = E(A,i,l,n)*k % M;
				E(p,i,l,n) = E(p,i,l,n)*k % M;
			}
		pivot(A,p,j,n,M);
	}/* End main loop over columns */	

	/* Normalize the matrix p so that all its elements lie between 0
	 * and M-1 */

	for(i=0;i<n;i++)
		for(j=0;j<n;j++)
			if(E(p,i,j,n) < 0) E(p,i,j,n) += M;

	for(i=0;i<n*n;i++)free((*A)[i]);
	free(A);
	return (int **)p;
}

/* returns pointer to matrix product of nxn matrices A and B, computed
 * modulo M */

int **mat_prod(int *(*A)[], int *(*B)[],int n,int M)
{
	int *(*p)[],i,j,k;

	if(!A) return NULL;
	if(!B) return NULL;
	if(M <= 1)return NULL;

	p = (int *(*)[])malloc(n*n*sizeof(int *));
	if(!p){
		fprintf(stderr,"mat_prod: cannot allocate memory.\n");
		exit(1);
	}
	for(i=0;i<n*n;i++){
		(*p)[i] = (int *)malloc(sizeof(int));
		if(!(*p)[i]){
			fprintf(stderr,"mat_prod: cannot allocate memory.\n");
			exit(1);
		}
	}
	for(i=0;i<n;i++)
		for(j=0;j<n;j++){
			E(p,i,j,n) = 0;
			for(k=0;k<n;k++)E(p,i,j,n) += 
				E(A,i,k,n) * E(B,k,j,n);
			E(p,i,j,n) = E(p,i,j,n) % M;
		}
	return (int **)p;

}



#undef PRINTABLE 
#define PRINTABLE "\n\t !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"

char printable[] = PRINTABLE;

/* These are defined in main.c */
extern char key[];
extern int encrypt_on;
extern void my_error(char *);


static int N;
static int M = sizeof(printable)-1;
static int *(*K)[], *(*K_inv)[];
void setup_matrix(char *key, int key_len, int modulus);

int my_encrypt(char *input, char *output, int n)
{
	int i,j,k,l,m; 
	int c;
	static int firstcall = 1;

        if(firstcall){
		K = NULL;
		setup_matrix(key,strlen(key),97);
	        firstcall = 0;
		return 0;
        }

	/* Encryption/decryption part. Multiply column vector of input
	 * characters by K/K_inv and return resulting column as output */

	if(n<N)  /* partial block. We need to build new nxn matrices
		     for this one encryption */
		setup_matrix(key,n*n,97);

	for(i=0;i<n;i++){
		m = 0;
		for(j=0;j<n;j++){
			switch(input[j]){
				case '\n':
					c = 0;
					break;
				case '\t':
					c = 1;
					break;
				default:
					c = (int)(input[j]) - ' ' + 2;
					break;
			} /* c is index of input character */
			if(encrypt_on)
				m += E(K,i,j,N)*c;
			else
				m += E(K_inv,i,j,N)*c;
		}
		m = m % M;
		output[i] = printable[m];
	}	
	return n;
}
	
/* Slurp input in n length blocks, where n is the least integer whose 
 * square is greater than or equal to the key length */

int set_block_size(int *ip)
{
	int n,i;

	n = strlen(key);
	for(i=0;i*i<n;i++);
	*ip = i;
	return 0;
}

/* Set up the matrices K and K inverse on which the Hill method is based.
 * We take K to be the letters of the Key arranged in a square, padded
 * with zeros to the size of the next larger perfect square. This may
 * produce a singular matrix. If so, we repeat by adding one to the matrix
 * until a nonsingular matrix is produced */

void setup_matrix( char *key, int key_len, int modulus)
{
		int i,j,m,c;


		for(i=0;i*i < key_len; i++);

		N = i;

		do {

		if(K){ 

			/* build a new matrix by adding one to the old
			 * one modulo modulus */

			c = 1;
			for(j=N*N-1;j>=0;j--){
				*(*K)[j] = *(*K)[j]+c;
				c = *(*K)[j]/modulus;
				*(*K)[j] = *(*K)[j]%modulus;
			}
					
		}
		else {
			K = (int *(*)[])malloc(N*N*sizeof(int *));
			if(!K){
				fprintf(stderr,"set_matrix: cannot allocate memory.\n");
				exit(1);
        		}
			for(i=0;i<N*N;i++){
				(*K)[i] = (int *)malloc(sizeof(int));
				if(!(*K)[i]){
					fprintf(stderr,"set_matrix: cannot allocate memory.\n");
					exit(1);
				}
			}
			for(i=0;i<N;i++)
				for(j=0;j<N;j++)
					if(i*N+j < key_len)
						E(K,i,j,N)=key[i*N+j]%modulus;
					else E(K,i,j,N) = 0;
		}
		K_inv = (int *(*)[])mat_inv(K,N,M);
		} while (K_inv == NULL);
}

/* Main routine for debugging */
#if 0
#define DIM 2
int TestData[] = {0,1,2,3};
int *TestA[DIM*DIM];

int main()
{
	int i,j,k;
	int *(*p)[],*(*q)[],*(*r)[];

	for(i=0;i<DIM*DIM;i++)
		TestA[i] = &(TestData[i]);
	p = (int *(*)[])copy_matrix(&TestA,DIM);
	printf("The matrix\n");
	dump_matrix(p,DIM);
	printf("\nhas inverse mod 97 given by\n");
	q = (int *(*)[])mat_inv(p,DIM,97);
	if(q)dump_matrix(q,DIM);
	printf("check: \n");
	p = (int *(*)[])copy_matrix(&TestA,DIM);
	r = (int *(*)[])mat_prod(p,q,DIM,97);
	if(r) dump_matrix(r,DIM);
	else 
		printf("No inverse exists\n");

}

#endif
