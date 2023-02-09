/* vigenere.c: implementation of encryption routine for 
   the classical vigenere cipher. This is one of the earliest 
   polyalphabetic substitution methods. It is attributed to Blaise
   de Vigenere (1523-1596), although the basic idea of encrpyting successive
   plaintext letters with different alphabets seems to have originated with
   Johannes Trithemius (1462-1516). Trithemius, however, did not use a
   key to select the alphabets. */

/* People who encrypted messages by hand used a vigenere tableau which, for
   a standard aphabet, would look like this:
	
    	abcdef ...z  (plain text alphabet)
a	abcdefg...z  ( cypher alphabets )
b	bcdefg...za
c	cdefg...zab
.	.
.	.
.	.
z	zabc...   y

The letters in the key were used in succession to select one the alphabets.
Thus, if the keyword is bac, then the second alphabet is used to encrypt the
first plaintext letter (the second alphabet is the one corresponding to
the key letter "b"), the first alphabet to encrypt the second plaintext letter,
and the 3rd alphabet to encrypt the 3rd plaintext letter. The process then
starts over with the first letter of the keyword. 

The alphabet in our implementation is the set of printable characters, i.e.,
those having ascii codes 32-126. (Newlines and tabs are not encrypted.) Also,
we don't create a physical tableau in memory, but rather achieve the same
effect by indexing.

Repeated characters are a great threat to the security of the Vigenere. For
example, a file that starts with many repeated space characters will
be encrypted with the key in plain view! (Try this with our README file.)

See the file beaufort.c for a discussion of some variants of the vigenere.

*/  
  
#include "global.h"

static char cipher[] = PRINTABLE;  /* Holds cipher alphabet. We use it
                                      to implement a vigenere tableau by
                                      creative indexing. */

/* position of char x in cipher array */
#define INDEX(x) ((x) - ' ')
#define NN ((sizeof PRINTABLE) - 1)    

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
		if(encrypt_on){  /* We are encrypting a message */
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
					j = INDEX(plain_text[k]);
					cypher_text[k] = 
						cipher[(i+j)%NN];
			}
		}
		else {  /* we're decrypting: cypher_text here = plain text */
			switch(plain_text[k]){
				case '\n':
					cypher_text[k] = '\n';
					break;
				case '\t':
					cypher_text[k] = '\t';
					break;
				default:
					m++;
					l = (m-1)%key_len;
					i = INDEX(key[l]);
					j = (INDEX(plain_text[k])+NN-i)%NN;
					cypher_text[k] =
					    cipher[j];
			}
		}
	return n;
}
	
/* Default block size routine */
int set_block_size(int *ip)
{
	*ip = INPUT_BUFFER_SIZE;
	return 0;
}
	
