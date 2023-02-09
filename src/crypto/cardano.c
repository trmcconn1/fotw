/*  Cardano.c: Implementation of Cardano Grille Cipher

     The Cardano cipher is a transposition cipher that operates on blocks
     of text arranged in a square NxN array.  It is named after medieval
     mathematician Girolamo Cardano.

     The author learned of the cadano cipher by reading Stephen Fratini,
     "Encryption using a variant of the turing-grille method, Mathematics
     Magazine 75(2002), 389-396.

     N should be even. The square array of text to be enciphered is overlain
     with a "Grille", a square array of empty cells, one for each letter of
     text. Exactly 1/4 of the grille cells have been cut out making the
     letters underneath them visible. These visible letters are then read
     off in order from left to right and top to bottom to become the first
     part of the ciphertext. 

     The grille is then rotated 90 degrees counterclockwise and the same
     process is repeated. (The empty cells of the grille need to be arranged
     so that each rotation uncovers a different set of plaintext letters.)

     After two more such rotations, the permutation of the plaintext block is
     complete. The same operation is then applied to subsequent blocks.

     Decryption is done by inserting successive ciphertext letters into the
     the empty grille cells, rotating the grille and repeating, etc, with
     each block of NxN ciphertext letters. 

     In our implementation, a suitable grille is derived from the key. (See
     comments below for how this is done.) 

     N.B.: Whitespace, including newlines, are enciphered along with other
     characters.

*/

#include "global.h"

/* i,j element of nxn matrix macro */
#define E(A,i,j,n) *((*(A))[(n)*(i)+(j)])

/* Here, the declaration of A is int *(*)[]: pointer to an array of pointers 
 * to int */

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

void free_matrix(int *(*A)[],int n)
{
	int i;
	for(i=0;i<n*n;i++)free((*A)[i]);
	free(*A);
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

/* Rotate the square matrix 90 degrees counterclockwise, as if turning the
   Grille. The rotation operation maps the (i,j) element to the
   (n-j,i) position. (In C indexing conventions this comes out as
   (n-1-j,i).) */

void rotate_matrix(int *(*A)[],int n) {

	  int i,j;
          int *(*B)[]; /* for a copy, freed at end. */  

	  B = (int *(*)[])copy_matrix(A,n);
	  for(i = 0; i<n;i++)
		for(j=0; j<n; j++)
			E(A,n-j-1,i,n) = E(B,i,j,n);	
	  free_matrix(B,n);
}

/* These are defined in main.c */
extern char key[];
extern int encrypt_on;
extern void my_error(char *);

static int *(*G)[];  /* The Grille */
static int N;        /* The Grille is NxN */
void setup_grille(char *key, int key_len, int n);

int my_encrypt(char *input, char *output, int n)
{
	int i,j,k=0,l=0,m; 
	int c;
	static int firstcall = 1;

        if(firstcall){
		set_block_size(&i);  /* This sets grille dimension N. Throw
                                        away value in i. */
		setup_grille(key,strlen(key),N);
	        firstcall = 0;
		return 0;
        }

	/* Encryption/decryption part */
	for(m=0;m<4;m++){
		for(i=0;i<N;i++)
			for(j=0;j<N;j++){
				if(E(G,i,j,N)){ /* Grille hole here ? */
					if(encrypt_on){
						if(k<n)
							output[l++] = input[k];
					 }
					 else {  /* decrypting */
						if(l<n && N*i+j < n)
							output[N*i+j] = 
								input[l++];
					}
				}
				k++;
			}
		if(m==3)break; /* No need to rotate matrix last time thru */ 
		rotate_matrix(G,N);
		k = 0;
	}
	return l;
}
	
/* The block size must be even. If k is the length of the key the grille 
dimension will be the smallest integer N such that (N/2)^2 >= k.  
This routine also has the side effect of storing the grille dimension in N.
(The block size is N^2.)
*/

int set_block_size(int *ip)
{
	int k,n=2;

	k = strlen(key);
	while((n/2)*(n/2)<k)n += 2;
	N = n;
	*ip = n*n;
	return 0;
}

/* Set up the Grille. It is NxN, and conceptually divided into 4 quadrants: A
   Northwest (NW), NE, SE and SW quadrant of sizes (N/2) by (N/2) each.   
   One thinks of the entries of each quadrant as being numbered 
   consecutively as 1 2 3 4 ... N/2
                    N/2+1 ..... N
                    .           .
		    .           .
                    .           .
		      ........(N/2)^2

   Initially, all entries are zero. For each character of the key in turn,
   select a quadrant by reducing the character modulo 4, and put a 1 in the
   the corresponding numbered position in that quadrant. (If key letters
   are exhausted, continue by generating the quadrant numbers at random until
   all (n/2)^2 positions have been marked 0 or 1. 
*/

void setup_grille( char *key, int key_len ,int n)
{
		int i,j,m=0,c;


		srandom((long)key_len); /* The seed must depend on the key
                                           only, but perhaps it should depend
                                           on more than just the key length? */

		G = (int *(*)[])malloc(n*n*sizeof(int *));
		if(!G){
			fprintf(stderr,"setup_grille: cannot allocate memory.\n");
			exit(1);
        	}
		for(i=0;i<n*n;i++){
			(*G)[i] = (int *)malloc(sizeof(int));
			if(!(*G)[i]){
				fprintf(stderr,"set_matrix: cannot allocate memory.\n");
				exit(1);
			}
			*((*G)[i])=0;
		}
		for(i=0;i<n/2;i++)
			for(j=0;j<n/2;j++){
				if(m < key_len)
					c = key[m];
				else c = (int) random();
				switch(c%4){
				
					case 0:
						E(G,i,j,n) = 1;
						break;
					case 1:
						E(G,n-1-j,i,n) = 1;
						break;
					case 2:
						E(G,n-1-i,n-1-j,n) = 1;
						break;
					case 3:
						E(G,j,n-1-i,n) = 1;
						break;
				}
				m++;
				
			}
}

#if 0

/* Main routine for debugging */

char key[] = "mary had a little gamb";

int main()
{
	int n;
	int k = strlen(key);
	
	set_block_size(&n);
	setup_grille(key,k,N);
	dump_matrix(G,N);
	printf("\n");
	rotate_matrix(G,N);
	dump_matrix(G,N);
	printf("\n");
	rotate_matrix(G,N);
	dump_matrix(G,N);
	printf("\n");
	rotate_matrix(G,N);
	dump_matrix(G,N);
	
}

#endif
