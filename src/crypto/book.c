/* book.c: implementation of encryption routine for a general book
   cipher. A book cipher encrypts each plain text character into a number
   giving the number of a word in a chosen text (the "book") whose first
   letter is the given plain text character.

   In our implementation we depart from the traditional word-based model,
   since this seems unnatural for text files based upon all printable
   characters. Instead each encryptable character is replaced by its offset
   in a designated book file  (When the character appears more
   than once in the book, as would be typical, an occurence is chosen
   randomly.) 

   The key or "password" obtained from the user is interpreted as the system
   name of a file to be opened that contains the text of the book. For
   example, on unix systems the password could be the fully qualified path
   of the book file. 

   For the purposes of this routine, the normal alphabet consists of all
   printable ascii characters, i.e., characters 32(space)-126(~) of the
   ascii code. If the book does not contain some character of the plain
   text then the encryption fails. Note that case is significant on both
   encryption and decryption. This is in keeping with our design philosophy
   that, whenever possible, the original file should be reproduced exactly.
   This does, however, reduce the strength of the encryption because a given
   character does not occur as often in, say, upper case, as it does if
   case is ignored. Better security would be obtained by using a book that
   contains only lower case (or only upper case) characters, used to encrypt
   a file of the same type. 

   The package includes a utility in the utils directory called makebook
   that can be used to create a "book" file made of randomly selected
   characters. With a randomly created book used only once, the security
   of this cypher could approach that of a one-time pad. For best results,
   the frequency of characters in the book should match (roughly) the 
   probability of occurence of a given character in the plain text.

   You can test this program by using the README as the book to encrypt
   and decrypt the CHANGES file. (See the Appendix of README.)

   Official bug: some might consider newlines and tab characters to be
   "printable". However, we do not encipher these characters, which occur
   before position 32 in the ascii code.

*/
  
#include "global.h"

#define MAXLINE 256

static int nchars;  /* number of chars in the book */
static int max_num_len; /* to how much space does a plaintext char expand? */
static int printable_counts[256];
static char *book_buf;

/* return a number chosen at random from 0,1...,k-1 */

int random_on(int k){

	double U;   /* U(0,1) random variable */
	int rval;

	U = ((double)rand())/((double)RAND_MAX);
	rval = (int)((double)k*U);
	return rval;
} 

int my_encrypt(char *plain_text, char *cypher_text, int n)
{
	int i,j=0,m,k,l=0; /* i = pos in plaintext, l = pos in cyphertxt */
	int key_len;
	int c;
	FILE *book;
	char *p;
	char lbuf[256];    /* scratch buffer */
	static int firstcall = 1;

	/* Used in decryption only: */
	static int pending_word = 0; /* Number of leftover chars */
	static char pword_buf[256];  /* partial word from last round */


	if(firstcall){  /* initialization */

		key_len = strlen(key);

		if(key_len == 0){
			fprintf(stderr, "Key cannot be empty\n");
			return -1;
		}

		/* Try to open the book */

		if(!(book = fopen(key,"r"))){
			
			my_error("Cannot open book file\n");
			return -1;
		}

		/* Do a first pass through the book to see how 
                   big it is. Count char instances as we go. */

		while((c=fgetc(book))!=EOF){
			(printable_counts[c])++;
			nchars++;
		}

		/* Allocate buffer to hold book in memory */

		book_buf = (char *)malloc(nchars*sizeof(char));
		if(!book_buf){
			my_error("Cannot allocate buffer for book\n");
			return -1;
		}

		/* Read book into memory */
		rewind(book);
		i = 0;
		while((c=fgetc(book))!=EOF)
			book_buf[i++] = c;
				

		/* seed random number generator using a check-sum from
                   the key */

		m = 0;
		for(i=0;i<key_len;i++)
			m += (int)key[i];
		srand(m);

		/* Figure out longest number offsets in book can
                   be. Used to choose proper buffer size */

		sprintf(lbuf,"%d ",nchars);
		max_num_len = strlen(lbuf);
		if(max_num_len == 0){  /* we divide by this */
			my_error("Zero length codes\n");
			return -1;
		}

		firstcall = 0;
		fclose(book);
		return 0;
    	}


	if(encrypt_on){  /* We are encrypting a message */
		for(i=0;i<n;i++)
			switch(c = plain_text[i]){
				case '\n':
					cypher_text[l++] = '\n';
					break;
				case '\t':
					cypher_text[l++] = '\t';
					break;
				default:

					/* How many times does current plain
                                           text char occur in book? */

					m = printable_counts[c];
					if(!m){
						sprintf(lbuf,"character %c not in book\n",c);
						my_error(lbuf);
						
						return -1;
					}

					/* Select an occurence at random */
					j = random_on(m);

					/* Look up jth occurence in book */
					p = book_buf-1;
					for(k=0;k<=j;k++){
						p = strchr(p+1,c);
						if(!p){
							my_error("can't find j-th occurence.\n");
							return -1;
						}
					}

					/* Write offset of found occurence
                                           as string to cypher text buffer */

					sprintf(lbuf,"%d ", (p - book_buf));
					j = strlen(lbuf);
					for(k=0;k<j;k++)
						cypher_text[l++]=lbuf[k];
					break;
			}
		}
	else {  /* we're decrypting: cypher_text here = plain text */

		/* Any incomplete word left over from last call? */
		j = pending_word; /* = how many digits are left over */

		i = 0;
		while(i<n)
		   switch(k=plain_text[i++])
				{
				case '\n':
					cypher_text[l++] = '\n';
					break;
				case '\t':
					cypher_text[l++] = '\t';
					break;
				case ' ' :
					pword_buf[j]='\0';
					c = atoi(pword_buf);
					/* make sure c is in bounds */
					if((c<0)||(c >= nchars)){
						my_error("text index too big\n");	
						return -1;
					}

					/* next char is c-th one in book */
					cypher_text[l++] = book_buf[c];
					j=0;
					pending_word = 0;
					break;
				default:
					pending_word++;
					pword_buf[j++]=k;
					break;

			}
	}
	return l;
}


/*  Block size routine: because characters get expanded into multi-character
    numbers we need to accept fewer characters in the input buffer. Dividing
    by the largest number length is probably overkill, but it's safe.  */

int set_block_size(int *ip)
{
	if(encrypt_on){
		*ip = OUTPUT_BUFFER_SIZE/max_num_len;
	}
		else *ip = INPUT_BUFFER_SIZE;
	return 0;
}
	
