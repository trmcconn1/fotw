/* The following may be set from a makefile */
#ifndef MAX_LEVEL
#define MAX_LEVEL 16
#endif
#ifndef MAX_STRING
#define MAX_STRING 65536

/* MAX_STRING should be 2^MAX_LEVEL */
#endif

struct node {

	int order;
	int visits;
	int xvisits;
	int yvisits;
	char *name;
	
	struct node *prev0;
	struct node *prev1;
	struct node *next0;
	struct node *next1;
	struct node *up1;
	struct node *up0;

	int xin;
	int xout;
	int yin;
	int yout;

};

#define COMPLEMENT(c) ( (c) == '0' ? '1' : '0' )

/* Declarations of functions defined (mostly) in util.c. The repeat is required
   since this will be included by both .c and .cpp files */

#ifdef CPLUSPLUS
extern "C" struct node *parse(char *,int);
extern "C" void dumppath(char *, int);
extern "C" void dumpantipath(char *, int );
extern "C" int countantipath(char *, int );
extern "C" struct node *allocnode(char *,int);
extern "C" int isdebruijn(char *, int);
extern "C" char *EM(char *,int);
extern "C" struct EMnodestruct *EMstarnodes(char *,int);
extern "C" void EMprintnodeinfo(char *,int);
extern "C" struct node *searchpath(char *name, struct node *, int);
extern "C" void freenodearray(void);
extern "C" int power2(int);
extern "C" void warn(char *);
extern "C" void EMSnodes(char *, int);
extern "C" char *fixedpoint(char*, int);
extern "C" void reverse(char *);
extern "C" void complement(char *);
extern "C" int readfile(char *);
extern "C" void rotate(char *, int,int);
extern "C" char *prefer_same(int);
extern "C" char *prefer_opposite(int);
extern "C" int verbose;
extern "C" char *linear(unsigned,int);
extern "C" char *prefer1(int);
extern "C" char *keyword(char *,int);
extern "C" char *keyseq(char *, char (*)(char *),int);
extern "C" int freenode(struct node *);

#else
extern struct node *parse(char *,int);
extern void dumppath(char *, int);
extern void dumpantipath(char *, int );
extern struct node *allocnode(char *,int);
extern int countantipath(char *, int);
extern struct node *searchpath(char *name, struct node *, int);
extern int isdebruijn(char *, int);
extern char *EM(char *, int);
extern void EMprintnodeinfo(char *,int );
extern struct EMnodestruct *EMstarnodes(char *, int);
extern int power2(int);
void freenodearray(void);
extern void warn(char *);
extern void EMSnodes(char *,int);
extern char *fixedpoint(char *,int);
extern void reverse(char *);
extern void complement(char *);
extern int readfile(char *);
extern void rotate(char *,int,int);
extern char  *prefer_same(int);
extern char *prefer_opposite(int);
extern int verbose;
extern char *linear(unsigned,int);
extern char *prefer1(int);
extern char *keyword(char *,int);
extern char *keyseq(char *, char (*)(char *),int);
extern int freenode(struct node *);
#endif

/* These are essentially all the deBruijn sequences of order 4 
The rest are obtained from these 8 by either bitwise complement,
or by time reversal. If the complementary graph of the path
in the DeBruijn graph has the minimum possible 3 cycles, we
call the sequence a double helix. */
#define TESTSEQ1 "1111000010011010111"   /*double helix*/
#define TESTSEQ2 "1111000011010010111"   /*5 cycles*/
#define TESTSEQ3 "1111001000011010111"   /*double helix*/
#define TESTSEQ4 "1111001101000010111"   /*5 cycles*/
#define TESTSEQ5 "1111010000101100111"   /*5 cycles*/
#define TESTSEQ6 "1111010000110010111"   /*5 cycles*/
#define TESTSEQ7 "1111010010110000111"   /*5 cycles*/
#define TESTSEQ8 "1111010011000010111"   /*5 cycles*/

/* A modest library of octal codes for linear debruijn sequences */
/* From Table III-5, Shift Register Sequences, S. Golomb */
#define DEG1CODES "3"
#define DEG2CODES "7"
#define DEG3CODES "13 15"
#define DEG4CODES "23 31"
#define DEG5CODES "45 51 57 67 73 75"
#define DEG6CODES "103 133 141 147 155 163"
#define DEG7CODES "203 211 217 221 235 247 253 271 277 301 313 323 325 345 357 361 367 375"
#define DEG8CODES "435 453 455 515 537 543 545 551 561 607 615 651 703 717 747 765"
#define DEG9CODES "1021 1033 1041 1055 1063 1137 1151 1157 1167 1175 1207 1225 1243 1245 1257 1267 1275 1317 1321 1333 1365 1371 1423 1425 1437 1443 1461 1473 1517 1533 1541 1553 1555 1563 1577 1605 1617 1665 1671 1707 1713 1715 1725 1731 1743 1751 1773"
/* Dear reader, you are invited to supply more */
/* Make sure to edit the method == 5 section of main.cpp */

#define DEG10CODES ""
#define DEG11CODES ""
#define DEG12CODES ""
#define DEG13CODES ""
#define DEG14CODES ""
#define DEG15CODES ""
#define DEG16CODES ""
