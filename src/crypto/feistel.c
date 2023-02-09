/*  Feistel.c:  Implementation of a (representative) Feistel Cipher.
 *
 *  The Feistel cipher is named after Horst Feisel, who chaired an IBM
 *  group that was convened to develop a "modern" cipher for use in
 *  business. It is not an actual cipher - but the framework for one - whose
 *  actual implementation involves specifying several ingredients:
 *
 *  1) A round function (F): A function that is iterated over parts of
 *  the input block, with additional input in the form of a different
 *  key each round. 
 *
 *  2) A key scheduler: It takes the user's key and constructs the set
 *  of round keys.
 *
 *  The number of rounds is also configurable.
 *
 *  (Block ciphers by convention run in several modes, so there are additional
 *   functions having to do with modes that must be implemented. See below.) 
 *
 *  Conceptually, the input to each round consists of three items: The
 *  left half of the previous block, the right half of the previous block,
 *  and the round key. We then define
 *
 *  	new left = right
 *  	new right = left ^ F(right,key)
 *  
 *  where ^ denote bitwise exclusive or. The last round is followed by
 *  a final swap of left and right.
 *
 *  The repeated swapping and stirring of one half is designed to propagate
 *  a form of chaos rapidly through the block, and was motivated by ideas
 *  from information theory. 
 *
 *  To decrypt, one merely repeats the same algorithm, but with key ordering
 *  reversed. It is a simple exercise to check that this will restore the
 *  original text. 
 *
 *  Note that if F is the constant function 0 then the effect of the algorithm
 *  is to perform ROUNDS + 1 swaps, so any strength in an implementation of
 *  the cipher depends on the design of the round function and key scheduler.
 *  In this file we provide simple (and probably bad) 
 *  default implementations of these based on repeated use of the random
 *  number generator as hash function, but the user could easily customize his
 *  own. 
 *
 *  The des(1) (digital encryption standard algorithm) is just a particular
 *  feistel cipher, so one can study its algorithm as a source of better
 *  examples. 
*/

  
#include "global.h"

#define ROUNDS 16 
#define BLOCK_SIZE 8 
#define BLOCK_UNIT char

/* BUG: We assume a long is 4 bytes many places. This should be fixed to
 * ease porting to 64 (or longer??) bit platforms */
typedef unsigned long blocktype;
typedef blocktype keytype;

#ifndef ROUND_FUNCTION
#define ROUND_FUNCTION default_round_function
#endif

#ifndef KEY_SCHEDULER
#define KEY_SCHEDULER default_key_scheduler
#endif

/* The round function combines half of the encryption block with a round
 * key pointed to by the void pointer. The round function knows what type
 * of data the pointer points to. */

blocktype default_round_function(blocktype, void *);
void default_key_scheduler(char *);

/* Block ciphers can operate in one of 4 modes: */

#ifndef BLOCK
#define ECB 0 /* Electronic Codebook Mode */
#define CBC 1 /* Cipher Block Chaining Mode */
#define CFB 2 /* Cipher Feedback Mode */
#define OFB 3 /* Output Feedback Mode */
#define CCM 4 /* Counter Mode */
#endif

#ifdef USE_ECB
static int mode = ECB;   
#elif USE_CFB
static int mode = CFB;   
#elif USE_OFB
static int mode = OFB;   
#elif USE_CCM
static int mode = CCM;   
#else
static int mode = CBC; /* Default mode is CBC */
#endif

/* We implement an externally callable function (in addition to set_block_size)
 * that sets the mode */

int set_block_mode(int n){
	mode = n;
       return n;
}       

/* ECB mode is the "normal" mode in that we encipher each block passed to
 * us and return ciphertext through the output pointer. No state is retained
 * between calls. 
 *
 * In CBC mode the previous ciphertext is retained in a buffer and xor'd
 * with the next input. This is an easily reversible option that hinders
 * probable text attacks by propagating some chaos from one encryption to
 * the next. We need and initial source of xor text to get the ball rolling,
 * which is provided by the INITIALIZTION VECTOR: */

static blocktype iv_left,iv_right;  
static blocktype left_fwd,right_fwd;

/* CFB (Cipher Feedback Mode) handles input one byte at a time rather than
 * in blocks. The block that is actually encrypted is the initialization
 * vector, with the output byte from the previous stage shifted in from the
 * left. The output byte, in turn, is gotten by xor-ing with the shifted out
 * byte. The advantage of this mode is that it is effectively a stream
 * cipher, so there is no need to pad the final block. 
 *
 * OFB (Output Feedback Mode) is similar to CFB except that instead of
 * shifting in the output byte (after xor) one shifts the previous shift
 * out byte (before the xor). This has the advantage of limiting the
 * damage from corrupted bytes of cyphertext to just a single corrupted
 * recoved plaintext byte.
 * 
 * The following routine handles implementing a shift register in the
 * initialization vector memory */

unsigned char shift(blocktype *, blocktype *, unsigned char );

/* In CCM (counter mode) the initialization vector block is successively
 * incremented and encyphered with the result xor'd with the input to
 * produce the output. The following routine manages the increments */

void increment(blocktype *, blocktype *);

#ifndef GET_IV
#define GET_IV default_get_iv 
#endif

void default_get_iv(char *,blocktype * ,blocktype *);

static void *keys[ROUNDS];

/* These are defined in main.c */
extern char key[];
extern int encrypt_on;
extern void my_error(char *);

blocktype get_left(char *);
blocktype get_right(char *);
void put_left(blocktype, char *);
void put_right(blocktype, char *);

int my_encrypt(char *input, char *output, int n)
{
	blocktype left,right,temp;
	unsigned char c;
	keytype *round_key;
	static int firstcall = 1;
	static int first_decryption = 1;
	int i;

        if(firstcall){
		
		if((mode == CBC)||(mode == CFB)||(mode == OFB) ||
				(mode == CCM)){

			 GET_IV(key,&iv_left,&iv_right);
		}

		KEY_SCHEDULER(key);
	        firstcall = 0;
		return 0;
        }

	if((mode == CBC)||(mode == ECB)){
		left = get_left(input);
		right = get_right(input);
	}
	if((mode == CFB)||(mode == OFB)){
		left = iv_left;
		right = iv_right;
	}
	if(mode == CCM){
		left = iv_left;
		right = iv_right;
		increment(&iv_left,&iv_right);
	}
	if(mode == CBC){

		if(encrypt_on){
			left ^= iv_left;
			right ^= iv_right;
		}
		else { /* decrypting: retain input(cypher)text to xor at end  */

			if(first_decryption)
				first_decryption = 0;
			else {
				iv_left = left_fwd;
				iv_right = right_fwd;
			}
			left_fwd = left;
			right_fwd = right;
		}

	}

	for(i=0;i<ROUNDS;i++){
		temp = right;
		/* CFB mode uses the same key schedule for decryption */
		if(encrypt_on||(mode == CFB)||(mode == OFB)||(mode==CCM))
			round_key = keys[i];
		else round_key = keys[ROUNDS-i-1];/* when decrypting, use
					     the keys in reverse order */
		right =left ^ ROUND_FUNCTION(right,round_key);/* ^ = bitwise 
								 xor */ 
		left = temp;
	}

	/* The final swap */
	temp = left;
	left = right;
	right = temp;

	if(mode == CBC){

		if(encrypt_on){
			/* save for next encryption */
			iv_right = right;
			iv_left = left;
		}
		else { /* decrypting */

			left ^= iv_left;
			right ^= iv_right;
		}

	}
	if(mode == CCM){

		put_right(right^get_right(input),output);
		put_left(left^get_left(input),output);

	}
	if(mode == CFB){
		c = shift(&left,&right,(unsigned char)0);
		/* The only difference between encryption and decryption
		 * in CFB mode is that the input byte is shifted in
		 * _before_ the xor operation on decryption */
		if(!encrypt_on)
			shift(&iv_left,&iv_right,*input);
		c ^= (unsigned char)*input;
		if(encrypt_on)
			shift(&iv_left,&iv_right,c);
		*output = c;
	}
	if(mode == OFB){
		c = shift(&left,&right,(unsigned char)0);
		shift(&iv_left,&iv_right,c);
		c ^= (unsigned char)*input;
		*output = c;
	}
	if((mode == ECB)||(mode == CBC)){
		put_right(right,output);
		put_left(left,output);
	}
	return n;
}
	
/*  
 *  Return 8(bytes) as the block size (or other, depending on units).
*/ 

int set_block_size(int *ip)
{
	if((mode == CFB)||(mode == OFB))
		*ip = 1;
	else
		*ip = BLOCK_SIZE;
	return 0;
}

void default_key_scheduler( char *key)
{
	int n,i,j;
	keytype x;

	srandom(n = strlen(key));
	for(i=0;i<ROUNDS;i++){
		keys[i] = (keytype *)malloc(sizeof(keytype));
		for(j=0;j<key[i%n];j++)x = (keytype)random();
		*((keytype *)keys[i]) = x;
	}
}

blocktype default_round_function(blocktype data, void *key)
{
	long i,j;

	srandom(*(long *)key);
	i = random();
	srandom((long)data);
	j = random();
	srandom(i^j);
	return (blocktype)random();
}


void default_get_iv(char *mykey, blocktype *left, blocktype *right){

	int n,i,j;
	blocktype s = 0;

	srandom(n = strlen(mykey));

	for(i=0;i<n;i++);
	for(j=0;j<mykey[i];j++)s += random();

	*left = s;
	s = 0;

	for(i=0;i<n;i++);
	for(j=0;j<mykey[i];j++)s += random();

	*right = s;
}

/* Get the left half of the array of characters, viewed as a 32 bit integer.
 * I.e., carray[0]...carray[4] become the binary digits (least significant ->
 * most significant) of an unsigned 32 bit int. One shouldn't worry too
 * much about the actual ordering of the bits. All that really matters is
 * that put_left be the inverse of get_left. */

blocktype get_left( char *carray)
{
	blocktype total = 0L,c = 0L;
	int i;

	for(i=3;i>=0;i--){
		/* prevent unwanted sign extension */
		c = (unsigned char)carray[i]; 
		total = 256L * total + c;
	}
	return total;
}

blocktype get_right(char *carray) 
{
	blocktype total = 0L,c = 0L;
	int i;

	for(i=7;i>=4;i--){
		c = (unsigned char)carray[i];
		total = 256L * total + c;
	}
	return total;
}


void put_left( blocktype x, char *carray)
{
	int i;

	for(i=0;i<4;i++){
		carray[i] = (unsigned char)(x % 256L);
		x = x / 256L;
	}
}


void put_right( blocktype x, char *carray)
{
	int i;

	for(i=4;i<8;i++){
		carray[i] = (unsigned char)(x % 256L);
		x = x / 256L;
	}
}

/* Do a bytewise right shift on the 64 bit quantity left|right, shifting
 * in x from the left and returning the most significant byte of right. */

unsigned char shift(blocktype *left, blocktype *right, unsigned char x)
{

		blocktype r,l;
		unsigned char c,d;

		r = *right;
		l = *left;

		c = l/(0xFFFFFF+1L); /* MSB of left */
		*left = (blocktype)x + l*256L;
		d = r/(0xFFFFFF+1L);
		*right = (blocktype)c + r*256L;
		return (unsigned char)d;
}

/* Increment the 64 bit quantity left|right by one. Used in counter
 * mode, in which the iv blocks are incremented by one and encrypted
 * successively */

void increment(blocktype *left, blocktype *right)
{
		blocktype r,l;
		blocktype c=0L;

		r = *right;
		l = *left;

		if(l = 0xFFFFFFFF) {
			c = 1;
			l = 0L;
		}
		else l++;
		r += c;
		*left = l;
		*right = r;
}
		

