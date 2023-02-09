/* Beaufort.c: implementation of encryption routine for 
   the classical Beaufort cipher. It is named for Admiral Francis Beaufort
   who popularized it in England during the latter half of the 19th
   century. Beaufort is perhaps better known for the 12 point scale of
   wind speeds that bears his name.

   The beaufort is a variant of the vigenere cypher that can be implemented
   using the standard vigenere tableau. Instead of choosing the row using
   the next successive key letter, as in the vigenere, the row is selected
   using the next plaintext letter. One then finds the next key letter in
   the given row (whose alphabet is a cyclic permutation of the plain
   alphabet) and locates the cypher text letter at the head of the column.
   (See also vigenere.c for more on using the standard tableau.)

   The cypher was quite popular in Victorian England as a means of quickly
   encyphering a telegram before handing it over to the telegram clerk, thus
   protecting one's secrets from his prying eyes. It is one of the few
   cyphers that is IDEMPOTENT, i.e, the same operation serves for both
   encryption and decryption. (The enigma cypher also has this property.) 

   The relationship amongst the vigenere, the beaufort, and a third cypher
   sometimes called the "variant vignere", is perhaps best understood using
   indices into the plain alphabet. Thus, let
   i  ,i  and i denote the indices in the plain alphabet of the current
    p   k      c	

   plaintext letter, key letter, and cyphertext letter respectively. Then:

                       encyphering                   decyphering
                    -------------------            ----------------

Vigenere            i    =   i   +   i  (mod N)   i   =  i  -  i  (mod N)
                     c        k       p            p      c     k

Variant             i    =   i   -   i  (mod N)   i   =  i  +  i  (mod N)
                     c        p       k            p      c     k

Beaufort            i    =   i   -   i  (mod N)   i   =  i  -  i  (mod N)
                     c        k       p            p      k     c

Here, N denotes the size of the alphabet.

Note that decyphering in the vigenere is the same as encyphering with the
orginal key complemented modulo the alphabet size. (The same holds for
the variant cypher.) 

We do not implement the variant vigenere, since it can be done by interchanging
the roles of encyphering and decyphering in the vigenere.

*/  
  
#include "global.h"

static char cipher[] = PRINTABLE;  /* Holds cipher alphabet. We use it
                                      to implement a vigenere tableau by
                                      creative indexing. */

/* position of char x in cipher array */
#define INDEX(x) ((x) - ' ')
#define NN ( (sizeof PRINTABLE) - 1) 

int my_encrypt(char *plain_text, char *cypher_text, int n)
{
	int i,j,k,l; 
	static int firstcall = 1;
	static int m = 0; /* m is number of current text char counting from
                             beginning of file */
	static int key_len = 0;
		
	if(firstcall){
		key_len = strlen(key);
		firstcall = 0;
		return 0;
	}

	for(k=0;k<n;k++)
		switch(plain_text[k]){
			case '\n':
				cypher_text[k] = '\n';
				break;
			case '\t':
				cypher_text[k] = '\t';
				break;
			default:
                                if(INDEX(plain_text[k]) < 0)return -1;
                                if(INDEX(plain_text[k]) >= sizeof PRINTABLE) return -1;

				m++;
				l = (m-1)%key_len; /* key repeats
                                                     cyclically */
				i = INDEX(key[l]);
				j = (i + NN - INDEX(plain_text[k]))%NN;
				cypher_text[k] =
					    cipher[j];
		}
	return n;
}
	
/* Default block size routine */
int set_block_size(int *ip)
{
	*ip = INPUT_BUFFER_SIZE;
	return 0;
}
	
