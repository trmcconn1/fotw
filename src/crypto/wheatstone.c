/* Wheatstone.c: implementation of encryption routine for 
   the classical wheatstone cipher. Charles Wheatstone first exhibited
   a machine for encryption that uses plain and cypher alphabets of
   different lengths at the Exposition Universelle at Paris in
   1867. Apparently unknown to him, an American named Decius Wadsworth
   had essentially the same idea in 1817. 

   Wheatstone's device looked like a clock with the plaintext alphabet
   (26 letters plus space) printed around the outside of the face and
   a scrambled alphabet of 26 letters (w/o space) printed around the face
   just inside the plaintext alphabet. The "minute hand" was moved clockwise
   to point at successive letters of the plaintext and the cypher text was
   read off from the direction of the "hour hand". The two hands were
   connected by gears with a ratio of 27/26, so that the two hands always
   pointed towards letters. To decipher, the hour hand was moved and the
   plain text read off from the position of the minute hand. 

   In our implementation, the plaintext alphabet consists of all 95 printable
   ascii characters, and the cypher alphabet is the same without the space
   character.  

   As it stands, the mapping from plain to cypher text cannot be 1-1, and
   so cannot be invertible. One can think of the wheatstone mapping as
   assigning an interval between successive  plaintext letters to an 
   interval between successive cypher text letters. Since there is one
   more plaintext letter, and hence one more possible interval, one of
   the possible intervals must be dropped to make the function 1-1. Wheatstone
   suggested disallowing doubled plaintext letters (i.e., the interval of
   0 is dropped, ) by substituting some "unused" letter for the second letter
   in a doubled pair. This is a problem for us, since in principle any
   printable ascii character can occur in text. We have chosen the seldom
   used tilde "~" character as our unused character, but this can be
   changed at compile time by defining the UNUSED macro. If the UNUSED
   character actually occurs in the plaintext, an error message is printed and
   the encryption fails. 

   We use a key to produce the scrambled alphabet by the same method as
   in the caesar cypher (see caesar.c) */ 
  
#include "global.h"

static char plain[] = PRINTABLE;  /* Holds plaintext alphabet */

/* The cypher alphabet contains one fewer character: the space is missing,
so that, in particular, there are no word divisions. This also makes the
plain and cypher alphabet lengths relatively prime. 

The cypher alphabet is built from normal as in caesar.c. We initialize
cipher the same as normal to reserve space, but it will ultimately be
scrambled */

static char normal[] = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

static char cipher[] = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";


/* position of char x in plain array */
#define INDEX(x) ((x) - ' ')
/* position of char x in scrambled array */
#define CINDEX(x) (found[(x) - (normal[0])])
#define NN (( sizeof PRINTABLE) - 1)   /* size of plaintext alphabet */
#define MM (( sizeof cipher ) - 1 )     /* size of cypher alphabet */
#ifndef UNUSED
#define UNUSED '~'   /* Replace doubled letter with this */
#endif
static int found[MM]; /* Records letters seen in key */
		      /* Also used to record the index of a given letter
                          in the scrambled alphabet */

int my_encrypt(char *plain_text, char *cypher_text, int n)
{
	int i,k,m,temp; 
	static int firstcall = 1;
	static int j = 0; /* Current total motion of big hand */
	static int prev = 0; /* Index of previous letter */
	static int prev_j = 0; /* Previous value of j. Used in decryption. */ 


        /* Build cipher alphabet from key. Use unique letters from key
           first, then complete with remaining unused letters in order.
           For efficiency, this is done only the first time the routine
           is called. */

        if(firstcall){
		k = 0;
                m = strlen(key);
                for(i=0;i<m;i++){ /* Place key letters in cipher alphabet */

			/* Check for space character in key. The space is not
                           a valid cypher character. Warn user. */

			if(key[i] == ' ') {
				fprintf(stderr,"wheatstone: Space character in key ignored.\n");
				continue;
			}

                        if(!found[key[i] - normal[0]]){
                                found[key[i] -normal[0]] = 1;
                                cipher[k++] = key[i];
                        }
                }
                for(i=0;i<MM;i++)              /* Place remaining unused */
                        if(!found[i])           /* letters in alphabet */
                                cipher[k++] = normal[i];

		/* Record the index of each letter in the scrambled alphabet
                   in the position in the found array that corresponds with
                   its position in the normal array. This is for rapid
                   lookup.  */

		for(i=0;i<MM;i++)
			found[cipher[i]-normal[0]] = i;

                firstcall = 0;
		return 0;
        }

	/* Loop done for each block of text to be encrypted/decrypted */

	for(k=0;k<n;k++)
		if(encrypt_on){  /* We are encrypting a message */
			switch(plain_text[k]){
				case '\n':
					cypher_text[k] = '\n';
					break;
				case '\t':
					cypher_text[k] = '\t';
					break;
				case UNUSED:
					fprintf(stderr,"Double letter marker %c found in plaintext. \n",UNUSED); 
					return -1;
				default:
                                        if(INDEX(plain_text[k]) < 0)return -1;
                                        if(INDEX(plain_text[k]) >= sizeof PRINTABLE) return -1;

					if((temp = INDEX(plain_text[k]))==prev)
						temp = INDEX(UNUSED);

					/* Advance big hand to next plain text
                                           letter */
					j += (temp >  prev ?
						temp  - prev :
						(NN  - prev + temp));

					/* Record index of current letter
						for next round */
					prev = temp;

					cypher_text[k] = 
						cipher[j%MM];
/*                                                      ^^^
                                                  Reducing total motion of
                                                  big hand by modulus equal
                                                  to number of cypher letters
                                                  gives position of little
                                                  hand.
*/
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
					j += (CINDEX(plain_text[k]) >  prev ?
						CINDEX(plain_text[k]) - prev :
						(MM - prev + CINDEX(plain_text[k])));
					/* Check for double letter */
					if(plain[j%NN]==UNUSED)
						cypher_text[k] = plain[prev_j%NN];
					else
						cypher_text[k] = plain[j%NN];
					prev = CINDEX(plain_text[k]);
					prev_j = j;
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
	
