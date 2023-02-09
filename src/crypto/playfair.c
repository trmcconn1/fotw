/* playfair.c:  Implementation of the enciphering routine for the playfair
cypher. Named after Lord Playfair, the cypher was actually invented by
Charles Wheatstone. 

To encipher with, e.g., key foobar, the unique letters of the key followed
by the remaining letters of the standard alphabet were arranged in a 5x5
tableau, thus:

	f   o   b   a   r
	c   d   e   g   h
	ij  k   l   m   n
        p   q   s   t   u
        v   w   x   y   z

(Note that i and j are merged into a single letter.) The plaintext was divided
into pairs of letters, each of which was enciphered into a pair of letters
as follows: If the plain text pair are in the same row, each letter is
replaced by the next to the right (with wraparound at the end of the row.)
Similarly, if the letters lie in the same column then each letter is
replaced by the one below it. Otherwise, each letter is replaced by the
one in its row and in the column of the other letter of the pair, and
in the same column but in the row of the other letter.

Double letters cannot be encyphered by these rules. The usual method was
to interpose another letter, e.g., "x" between the letters of the pair. 

There are a number of difficulties adapting these rules in our setting. 
First, as in all of the programs of this suite, we use an alphabet
consisting of all 95 printable ascii characters. Rather than merge enough
characters to obtain a perfect square (81), it seems more reasonable to
add 5 additional letters so that the alphabet contains 100 letters.
Unfortunately, this would require the use of non-printable characters. 
We have elected, instead, to use a 5x19 (rectangular) tableau rather than
a square one. (Wheatstone originally allowed the use of non-rectangular
talbeaux.)

Rather than insist that the user eschew double letters, we have the program
insert the ~ character between such pairs. (~ can be replaced by anything
else by defining the DOUBLE_MARK macro.) Of course, the ~ then shows up
when encyphered text is deciphered again, so this program will not
necessarily reproduce the original plain text. (Actually, the original
plaintext will already not be reproduced for another reason: since we
don't encipher newlines, any digraph that is split by a newline character
will migrate to the next line in the deciphered text.) 

Another problem is what to do about an odd letter (i.e. unpaired) left over
at the end of the plaintext. Among the possibities are simply leaving
the letter unecyphered, or adding some additional character (perhaps ~
again.) Both of these would seem to provide clues to the cryptanalyst. For
example, if the letter is left unencyphered, the cryptanalyst could note
that there are an odd number of letters in the cyphertext and deduce how
the plaintext ends. Since we are already butchering the plaintext (cf the
previous paragraph), we have elected to simply drop such letters.

We do not actually use a 2 dimensional tableau. Rather, we implement 
a virtual tableau by indexing into a one dimensional array.

Editorial comment: The playfair cypher is generally considered to be 
important in the history of cryptography, perhaps because it is one of the
first examples of a digraphic substitution. One should bear in mind, however,
that it is merely a monalpabetic substitution on the alphabet of digraphs.

*/

  
#include "global.h"

static char plain[] = PRINTABLE;  /* Holds plaintext alphabet */



/* position of char x in plain array */
#define INDEX(x) ((x) - ' ')
#define NN (( sizeof PRINTABLE) - 1)   /* size of plaintext alphabet */
static int found[NN]; /* Records letters seen in key */
static int row[NN];  /* Records row of letter in scrambled tableau */
static int col[NN];  /* Records col " " " " " */
static unsigned char cipher[NN]; 
#ifndef DOUBLE_MARK
#define DOUBLE_MARK '~'   /* Separate doubled letters with this */
#endif

int my_encrypt(char *plain_text, char *cypher_text, int n)
{
	int i,j,k,m; 
	int i1,i2,j1,j2;
	static int firstcall = 1;
	static unsigned digraph[2]; /* Holds digraph we're currently encoding.
                                       May carry over to next call. */
	static int l = 0; /* index into digraph */


        /* Build cipher tableau from key. Use unique letters from key
           first, then complete with remaining unused letters in order.
           For efficiency, this is done only the first time the routine
           is called. */

        if(firstcall){
		k = 0;
                m = strlen(key);
                for(i=0;i<m;i++){ /* Place key letters in cipher alphabet */

                        if(!found[key[i] - plain[0]]){
                                found[key[i] - plain[0]] = 1;
                                cipher[k++] = key[i];
                        }
                }
                for(i=0;i<NN;i++)              /* Place remaining unused */
                        if(!found[i])           /* letters in alphabet */
                                cipher[k++] = plain[i];

		/* Record the "row" and "column"  of each letter of the 
		   scrambled alphabet in the virtual 10x10 array.
                 */

		for(i=0;i<NN;i++){
			row[(unsigned)(cipher[i] - plain[0])] = i/5;
			col[(unsigned)(cipher[i] - plain[0])] = i%5;
		}

                firstcall = 0;
#ifdef DEBUG
	/* Dump tableau */
	fprintf(stderr,"Debug: dumping tableau:\n");
	for(i=0;i<19;i++){
		for(j=0;j<5;j++)
			fprintf(stderr,"%c ",cipher[i*5+j]);
		fprintf(stderr,"\n");
	}
#endif  /* DEBUG */
		return 0;
        }


	/* Loop done for each block of text to be encrypted/decrypted */

	m = 0; /* Count of letters enciphered, for return to caller */
	for(k=0;k<n;)
	  if(encrypt_on)
		switch(plain_text[k]){
			case '\n':
				cypher_text[m++] = '\n';
				k++;
				break;
			case '\t':
				cypher_text[m++] = '\t';
				k++;
				break;
			default:
                                if(INDEX(plain_text[k]) < 0)return -1;
                                if(INDEX(plain_text[k]) >= sizeof PRINTABLE) return -1;
				if(l==0) { /* first letter of digraph */
					digraph[0] = plain_text[k++];
					l++;
					break; /* go 'round for next letter */
				}
				/* else */
				if(plain_text[k] == digraph[0]){ /* double 
								   letter ? */
					/* If the DOUBLE_MARK itself is
                                           doubled, then we're hosed */
					if(plain_text[k] == DOUBLE_MARK){
						fprintf(stderr,"Can't have doubled %s in plain text\n",DOUBLE_MARK);
						return -1;
					}
					digraph[1] = DOUBLE_MARK;
					/* Don't advance k here */
				}
				else 
					digraph[1] = plain_text[k++];

                                i1 = row[(unsigned)(digraph[0]-plain[0])];
                                j1 = col[(unsigned)(digraph[0]-plain[0])];
				i2 = row[(unsigned)(digraph[1]-plain[0])];
				j2 = col[(unsigned)(digraph[1]-plain[0])];

				if(i1==i2){ /* letters in same row */
					cypher_text[m] = 
				           cipher[(i1*5 + ((j1+1)%5))%NN];
					cypher_text[m+1] = 
				           cipher[(i2*5 + ((j2+1)%5))%NN];
					m += 2;
				}
				else if(j1==j2){ /* letters in same col */
					cypher_text[m] = 
				           cipher[(((i1*5+5)%NN) + j1)%NN];
					cypher_text[m+1] = 
				           cipher[(((i2*5+5)%NN) + j2)%NN];
					m += 2;
				}
				else{  /* letters in general position */
					cypher_text[m] =
						cipher[i1*5 + j2]; 
					cypher_text[m+1] =
						cipher[i2*5 + j1]; 
					m += 2;
				}
				l = 0; /* reset for next digraph */
		}
	  else    /* decrypting */
		switch(plain_text[k]){
			case '\n':
				cypher_text[m++] = '\n';
				k++;
				break;
			case '\t':
				cypher_text[m++] = '\t';
				k++;
				break;
			default:
				if(l==0) { /* first letter of digraph */
					digraph[0] = plain_text[k++];
					l++;
					break; /* go 'round for next letter */
				}
				/* else */
				digraph[1] = plain_text[k++];

                                i1 = row[(unsigned)(digraph[0]-plain[0])];
                                j1 = col[(unsigned)(digraph[0]-plain[0])];
				i2 = row[(unsigned)(digraph[1]-plain[0])];
				j2 = col[(unsigned)(digraph[1]-plain[0])];

				if(i1==i2){ /* letters in same row */
					cypher_text[m] = 
				           cipher[(i1*5 + ((4+j1)%5))%NN];
					cypher_text[m+1] = 
				           cipher[(i2*5 + ((4+j2)%5))%NN];
					m += 2;
				}
				else if(j1==j2){ /* letters in same col */
					cypher_text[m] = 
				           cipher[(((90+i1*5)%NN) + j1)%NN];
					cypher_text[m+1] = 
				           cipher[(((90+i2*5)%NN) + j2)%NN];
					m += 2;
				}
				else{  /* letters in general position */
					cypher_text[m] =
						cipher[i1*5 + j2]; 
					cypher_text[m+1] =
						cipher[i2*5 + j1]; 
					m += 2;
				}
				l = 0; /* reset for next digraph */
		}
	return m;
}
	
/* Default block size routine */
int set_block_size(int *ip)
{
	*ip = INPUT_BUFFER_SIZE;
	return 0;
}
	
