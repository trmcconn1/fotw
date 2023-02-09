/* uebchi.c: implementation of encryption routine for 
   the uebchi cipher. 

   The uebchi cypher is a double columnar transposition used by the Germans
   in WWI. One begins by writing the plain text block in a tableau below
   the key. Each key letter is translated into a number as follows: The
   first a is number 1, the second a is number 2, and so on until there
   are no more a's. The next consecutive number is assigned to the first b,
   etc. For example, the setup to encipher "Bad Luck Charlie" with key
   madder is:

	m(5)  a(1)  d(2) d(3)  e(4)  r(6)
        B     a     d    L     u     c
        k     C     h    a     r     l
        i     e

   In the next step, one takes the columns off one after another in numerical
   order and writes the resulting letters down consecutively. Thus,
   aCedhLaurBkicl.  This is the first columnar transposition. One then repeats
   the whole thing a second time, using as text the result of the first
   transposition. 

   Between the two transpositions the encoder adds as many null characters
   as there are words in the key. (This rule is not necessary for the
   method to work properly. It merely adds security. The number of words
   in the key is a convenient rule available to both encoder and decoder.)

   As in the other programs of this package, we use an alphabet consisting
   of all 95 printable ascii characters instead of the classical 26 or
   27 letter alphabet. We implement tableaux as one-dimensional arrays
   with appropriate indexing to create a two-dimensional effect. 

*/  
  
#include "global.h"

static int col_order[MAX_KEY];    /* The numbers next to successive 
                                       keyletters, as above */ 
static int inv_col_order[MAX_KEY];  /* The inverse of this association */


static char plain[] = PRINTABLE;  /* Holds plaintext alphabet */
#define NN (( sizeof PRINTABLE) - 1)   /* size of plaintext alphabet */

static int key_words;
static int key_len = 0;

char random_char(void);   /* Used to generate Null characters */

int my_encrypt(char *plain_text, char *cypher_text, int n)
{
	int i,j,k,l,m; 
	static int firstcall = 1;
	int rows;
	char scratch[INPUT_BUFFER_SIZE+MAX_KEY];
		
	if(firstcall){
		key_len = strlen(key);

		if(key_len == 0){
			fprintf(stderr, "Key cannot be empty\n");
			return -1;
		}

		/* Fill col_order array from key */
		for(i=0;i<key_len;i++){
			k=0;
			for(j=0;j<key_len;j++)
				if(key[j] <  key[i])k++;
				else if((j<i)&&(key[j]==key[i]))k++;
			col_order[i] = k;
			inv_col_order[k] = i;
		}

#ifdef DEBUG
		fprintf(stderr,"debug: key letters ( col number )\n");
		for(i=0;i<key_len;i++)
			fprintf(stderr,"%c (%d) ",key[i],col_order[i]);
		fprintf(stderr,"\ni=j: ith col is number j\n");
		for(i=0;i<key_len;i++)
			fprintf(stderr,"%d=%d ",i,inv_col_order[i]);
		fprintf(stderr,"\n");
#endif

		/* seed random number generator using a check-sum from
                   the key */
		m = 0;
		for(i=0;i<key_len;i++)
			m += (int)key[i];

		srand(m);
		firstcall = 0;
		return 0;
	}


		if(encrypt_on){   /* Encrypting */

			/* copy columns (using rows of length key_len)
                            to scratch buffer in inv_col_order */
	
			l=k=0;
			while(k<key_len){
				j = inv_col_order[k];
				i = 0;
				while(i*key_len+j < n){
					scratch[l++] = plain_text[i*key_len
                                                                     +j];
					i++;
				}
				k++;
			}


			/* Add as many null characters as there are words
				in key */

			for(i=0;i<key_words;i++)
				scratch[l++] = random_char();

			/* do the transposition a second time with input
                           from the scratch buffer and output to the
                           cypher_text buffer */

			l=k=0;
			while(k<key_len){
				j = inv_col_order[k];
				i = 0;
				while(i*key_len+j < n+key_words){
					cypher_text[l++] = scratch[i*key_len
                                                                     +j];
					i++;
				}
				k++;
			}
		}
		else {   /* Decrypting */

			rows = n/key_len; /* number of complete rows, or
                                             one less if tableau is complete
                                          */
			if(rows*key_len == n)rows--;

			m = k = 0;

			while(k < key_len){

				/* What column does this stuff go in ? */

				j = inv_col_order[k];

				/* How long (l) was that column ? */

				l = ((j+ key_len*rows) < n ? rows+1 : rows );

				/* Copy next l chars into col j */

				for(i=0; i<l; i++)
					scratch[i*key_len+j] = plain_text[m++];
				k++;
			}

			/* Do the same thing again, with plain_text -->
                           scratch and scratch --> cypher_text */

			/* Note that n is reduced by key_words this time,
                           effectively throwing away the nulls introduced
                           during encryption */

			rows = (n-key_words)/key_len; 
			if(rows*key_len == (n-key_words))rows--;
			m = k = 0;
			while(k < key_len){

				/* What column does this stuff go in ? */

				j = inv_col_order[k];

				/* How long (l) was that column ? */

				l = 
				  ((j+key_len*rows) <  n-key_words ? rows+1 : rows );

				/* Copy next l chars into col j */

				for(i=0; i<l; i++)
					cypher_text[i*key_len+j] = scratch[m++];
				k++;
			}
		}
	if(encrypt_on)
		return n+key_words;
	else return n-key_words;
}
	

char
random_char(void)
{
	long n;

	n = rand();
	return plain[(int)(n%NN)];
}
	

int set_block_size(int *ip){

	static int firstcall = 1;
	int in_word = 0;
	int i;

	if(firstcall){
		/* Count words in key */
		for(i=0;i<key_len;i++)
			if((key[i]!=' ')&&(key[i]!='\t')&&
		            (key[i]!='\n')){
				if(!in_word){
					in_word = 1;
					key_words++;
				}
			}
			else in_word = 0;
		firstcall = 0;
	}

	if(encrypt_on)
		*ip = INPUT_BUFFER_SIZE - key_words;  /* Leave room for nulls */
	else
		*ip = INPUT_BUFFER_SIZE;
	return 0;
}
