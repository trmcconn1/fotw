

#include<stdio.h>
#include<stdlib.h>
#include<limits.h>

extern int verbose; /* defined in main.c */
extern int n_filters; /*defined in manager.c */
extern int filter_arg; 

/* Handy array */
extern unsigned long C[];

extern void initializeE(void);
extern void dumpE(void);
extern void listE(void);
extern unsigned char next(void);
extern unsigned char jump(unsigned char);
extern unsigned long E2Num();
extern unsigned long partitions(int);
extern int filter(int(*)(unsigned char (*)[],int,int),int);

/* These are filters that reside in filterlib.a */

extern int distinct_parts(unsigned char (*)[], int,int);
extern int odd_parts(unsigned char (*)[], int,int);
extern int super_distinct_parts(unsigned char (*)[], int,int);
extern int fewerthan(unsigned char (*)[], int,int);
extern int lessthan(unsigned char (*)[], int,int);
extern int bressoud(unsigned char (*)[], int,int);
extern int _1or4mod5(unsigned char (*)[], int,int);


struct filter_entry {

	char *name;
	char *info;
	int (*point)(unsigned char (*)[],int,int);

}; 

extern struct filter_entry my_filters[]; /* defined in manager.c */

extern void dump_filter_names(void);


/* This should be set from the makefile, included here for reference */

#if 0
#define N_MAX 16    /* ULONG_MAX = 2^8 = 64 */
#define N_MAX 43    /* ULONG_MAX = 2^16 = 65536 */
#define N_MAX 127   /* ULONG_MAX = 2^32 = 4294967295 */
#define N_MAX 416   /* ULONG_MAX = 2^64 */
#endif     

/* The pentagonal numbers are the double sequence j(3j-1)/2 and j(3j+1)/2 for
j = 1,2, ...   i.e., 1,2,5,7,12,15,22,26,....   Successive terms in each
sequence are obtained by adding to the previous one numbers that grow by
3 each time. Thus, 4,7,10,..., and 5,8,11,....   This is an extension of
the "figurate number" concept: adding successive natural numbers give the
triagular numbers, successive odds (grow by 2 each time) give the square
numbers. Geometricially, the added sequence form successive layers called
gnomons, like the layers in a tree or hailstone, that wrap around the 
boundary of the previous number. Each additional "corner" in the figure
adds one to the gap between gnomons. */

#define PENTAGONAL(n) ((n)%2?(((n)+1)/2)*(3*(((n)+1)/2)-1)/2:((n)/2)*(3*((n)/2)+1)/2)

