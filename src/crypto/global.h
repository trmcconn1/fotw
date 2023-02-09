/* global.h: common definitions for all modules */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERSION "1.03"

#define PRINTABLE " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"

/* The following give default values for various parameters that can be
   set on the compiler command line, depending on which target is being
   built. 
*/

#ifndef MAX_KEY
#define MAX_KEY 255
#endif

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "crypt"
#endif

#ifndef INPUT_BUFFER_SIZE 
#define INPUT_BUFFER_SIZE 256
#endif

/* This should be at least twice the size of the input buffer to allow for
   ciphers like the playfair that insert special symbols to break up double
   letter pairs, or for the addition of nulls. */
#ifndef OUTPUT_BUFFER_SIZE
#define OUTPUT_BUFFER_SIZE 512
#endif

#ifndef NCHARS
#define NCHARS 95
#endif

#ifndef START_CHAR
#define START_CHAR 32
#endif

#ifdef BLOCK
#define ECB 0 /* Electronic Codebook Mode */
#define CBC 1 /* Cipher Block Chaining Mode */
#define CFB 2 /* Cipher Feedback Mode */
#define OFB 3 /* Output Feedback Mode */
#define CCM 4 /* Counter Mode */
extern int set_block_mode(int);
#endif

#define USAGE "[-d] [passwd] <plaintext_file >cyphertext_file"

/* These are defined in main.c */
extern char key[];
extern int encrypt_on;
extern void my_error(char *);

/* This is defined in bin2pc.c: */
extern int bin2pc(unsigned char *, char *, int);
extern int pc2bin(char *, unsigned char *, int);
