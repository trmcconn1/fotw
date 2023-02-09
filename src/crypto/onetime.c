/* onetime.c: implementation of encryption routine for a one time pad
   cipher. This requires a secret key consisting of random characters.
   The key must be no shorter than the plain text. The "key" supplied
   by the user is just the filename of the "pad", a file containing the
   actual key characters. 

   Encryption consists of adding each key character to each plain text
   character, modulo the number of characters. To decrypt, just subtract
   the key character.

   If the pad is truly random, and is used only once, then this cypher is
   unbreakable. The drawback, of course, is that both parties must be
   supplied with a secure copy of the pad before any communication. Despite
   this inconvenience, the one time pad cypher has important applications.
   (NB: computer RNGs are not "truly random." If one knows the algorithm --
   -- and it is axiomatic that the enemy ALWAYS knows the algorithm -- then
   it is probably possible to recover the seed by analyzing a large sample
   of RNG output. Indeed, I conjecture that building a `random' RNG is
   essentially equivalent to the problem of building a `secure' cypher.)

   Official bug: some might consider newlines and tab characters to be
   "printable". However, we do not encipher these characters, which occur
   before position 32 in the ascii code.

*/
  
#include "global.h"

#define MAXLINE 256


int my_encrypt(char *plain_text, char *cypher_text, int n)
{
	int i=0,l=0; /* i = pos in plaintext, l = pos in cyphertxt */
	int key_len;
	int nchars=0;
	int printable_counts[256];
	int c,d;
	FILE *pad;
	static int *pad_buf;
	static int firstcall = 1;

	if(firstcall){  /* initialization */

		key_len = strlen(key);

		if(key_len == 0){
			fprintf(stderr, "Key cannot be empty\n");
			return -1;
		}

		/* Try to open the pad */

		if(!(pad = fopen(key,"r"))){
			
			my_error("Cannot open pad file\n");
			return -1;
		}


                /* Do a first pass through the pad to see how
                   big it is. Count char instances as we go. This allows
                   for possible precautionary statistical analysis of the
                   pad file (not implemented) */

                while((c=fgetc(pad))!=EOF){
                        (printable_counts[c])++;
                        nchars++;
                }

                /* Allocate buffer to hold pad in memory */

                pad_buf = (int *)malloc(nchars*sizeof(int));
                if(!pad_buf){
                        my_error("Cannot allocate buffer for pad\n");
                        return -1;
                }

                /* Read pad into memory */

                rewind(pad);
                i = 0;
                while((c=fgetc(pad))!=EOF)
                        pad_buf[i++] = c;


		firstcall = 0;
		return 0;
    	}

	for(i=0;i<n;i++)
		switch(c = plain_text[i]){
			case '\n':
				cypher_text[l++] = '\n';
				break;
			case '\t':
				cypher_text[l++] = '\t';
				break;
			default:
				c -= START_CHAR; /* shift space down to zero */
				d = pad_buf[l]-START_CHAR;
				if(encrypt_on)
					cypher_text[l] = (char)(START_CHAR+((c+d)%NCHARS));
				else
					if(c>d)
						cypher_text[l] = 
                                                  (char)(START_CHAR+((c-d)%NCHARS));
					else
						cypher_text[l] = 
                                                  (char)(START_CHAR+((NCHARS+c-d)%NCHARS));
			
				l++;
				break;
		}
	return l;
}


/*  Block size routine: 
 	Default block size routine */

int set_block_size(int *ip)
{
	*ip = INPUT_BUFFER_SIZE;
	return 0;
}
	
