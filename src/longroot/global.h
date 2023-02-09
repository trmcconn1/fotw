/* global.h: Main header for by-hand arithmetic programs
*
* By Terry R. McConnell 12/2003
*
*/

#define VERSION "1.0"

#define DIGITS "0123456789"
#define NONZERO_DIGITS "123456789"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include<ctype.h>

#define TRUE 1
#define FALSE 0
#define RADIX 10 

#define BORROW 1
#define NO_BORROW 0
#define ARG_MAX 65536

/* implemented in lib.c: */

int sub( char x, char y, char *res);
int isgte( char *A, char *B);
char *sbc(char *mend, char *shend);
char *times_digit(char *mcand, char dgt);
char *splice(char *front, char *back);
char *cut_off(char *source, int n);
char *shift_left(char *);
char *multiply(char *, char *, char ***);
char *add(char *,char *);
#ifndef HAVE_STRDUP
#define strdup mystrdup
#endif
char *mystrdup(const char *);
