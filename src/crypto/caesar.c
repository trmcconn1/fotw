/* caesar.c: implementation of encryption routine for a general monalphabetic
   substitution cipher. Thus, "caesar" is a misnomer, since the actual caesar
   cipher only used cipher alphabets which are cyclic permutations of the
   normal alphabet.

   The key or "password" obtained from the user is used as follows to 
   construct the cipher alphabet: The first letters of the cipher alphabet
   are the letters of the key, in order, with repeated letters dropped. 
   Then the unused letters of the normal alphabet are used in order. For
   the purposes of this routine, the normal alphabet consists of all
   printable ascii characters, i.e., characters 32(space)-126(~) of the
   ascii code. Thus, since the last 3 printable characters are |}~, using
   |}~ as key would produce a substitution in which a stands for c,b stands
   for d, etc. 

   Best results are obtained by taking for the key letters that include some
   suffix of the PRINTABLE array (as in the example cited.) The reason for
   this is that letters following the last key letter will not be enciphered.
   (See the description for how the cipher alphabet is constructed above.) 
   Thus, ~foobar would be a reasonable choice of key, but foobar itself would
   not -- all letters in the ascii code from s onward would not be 
   enciphered. 

   The method of this program is primitive even by the standards of ancient
   Rome. In practise, substitution ciphers were strengthened by various
   devices such as inclusion of nulls (special characters with no meaning
   at all) and homophones (multiple symbols to stand for the same plain text
   character. These were most often used for vowels and tended to lower
   the relative frequency count for each cypher text vowel equivalent to
   make them less obvious.) This program uses none of these devices, but
   they would not be difficult to implement.

   Official bug: some might consider newlines and tab characters to be
   "printable". However, we do not encipher these characters, which occur
   before position 32 in the ascii code.

*/
  
#include "global.h"

static char normal[] = PRINTABLE;  /* A copy of all the printable chars */
static char cipher[sizeof PRINTABLE ];  /* will hold cipher alphabet */
static int found[sizeof PRINTABLE ];    /* Keeps track of which letters are
                                            used */

/* position of char x in normal array */
#define INDEX(x) ((x) - ' ')

int my_encrypt(char *plain_text, char *cypher_text, int n)
{
	int i,j=0,m;
	static int firstcall = 1;

	/* Build cipher alphabet from key. Use unique letters from key
           first, then complete with remaining unused letters in order. 
	   For efficiency, this is done only the first time the routine
           is called. */

	if(firstcall){
		m = strlen(key);
		for(i=0;i<m;i++){ /* Place key letters in cipher alphabet */
			if(!found[INDEX(key[i])]){
				found[INDEX(key[i])] = 1;
				cipher[j++] = key[i];
			}
		}
		for(i=0;i<sizeof PRINTABLE;i++) /* Place remaining unused */
			if(!found[i])           /* letters in alphabet */
				cipher[j++] = normal[i];
		firstcall = 0;
		return 0;
	}
		
	for(i=0;i<n;i++)
		if(encrypt_on){  /* We are encrypting a message */
			switch(plain_text[i]){
				case '\n':
					cypher_text[i] = '\n';
					break;
				case '\t':
					cypher_text[i] = '\t';
					break;
				default:
					if(INDEX(plain_text[i]) < 0)return -1;
					if(INDEX(plain_text[i]) >= sizeof PRINTABLE) return -1;
					cypher_text[i] = cipher[INDEX(plain_text[i])];
			}
		}
		else {  /* we're decrypting: cypher_text here = plain text */
			switch(plain_text[i]){
				case '\n':
					cypher_text[i] = '\n';
					break;
				case '\t':
					cypher_text[i] = '\t';
					break;
				default:
					if(INDEX(plain_text[i]) < 0)return -1;
					if(INDEX(plain_text[i]) >= sizeof PRINTABLE) return -1;
					for(j=0;j<sizeof PRINTABLE;j++)
						if(cipher[j] == plain_text[i])break;
							cypher_text[i] = normal[j];
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
	
