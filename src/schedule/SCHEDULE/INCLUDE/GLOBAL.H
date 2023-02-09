
#ifndef GLOBALH
#define GLOBALH

#include<stdio.h>
#include<stdlib.h>




extern FILE *errorstream;  /* defined in main.c */
extern FILE *holdstream;  /* place to store while changeing */

#ifndef __MENU__
extern FILE *ostream;           /* menu is written in C++ */
extern FILE *istream;
#endif

#define PROMPT "Schedule>"    /* Default */
#define SECONDARYPROMPT ">"
#define PROMPTLEN 30

extern char WorkingDirectory[];
extern char Semester[];
extern char TempDirectory[];
extern char TempFilePath[];
extern char COLS[];
extern char Date[];
extern char Version[];
extern int numtokens;
extern struct TokenType *Tokens[];

#define TRUE 1
#define FALSE 0
#define MAXLINE 256
#define MAXTOKEN 80       /* size of a token */
#define MAXTOKS 50       /* number of tokens */
#define LONGNAME 80
#define SHORTNAME 20
#define NUMBERLEN 10
#define MAXPATH 256
#define COMMENT '#'
#define METACHARS "\"\n$\\"
#define MISSING_DATA "TBA"

/* Be careful with this macro. It is anything but safe if the capacity
argument exceeds the actual length of the buffer, since strncpy will pad
with nuls beyond the end of the buffer. Also, remember that this
macro will evaluate string more than once. */

#define SAFECPY(buf,string,capacity)  {if(strlen((string))>=(capacity))\
	myerror(errorstream,"String %s too long. Must be < %d\n",\
		(string),(capacity));\
	strncpy((buf),(string),(capacity)-1);}

struct TokenType {
char text[MAXTOKEN];
int position;
};

struct ShellVar {
int indx;
char *name;
char *value;             /* Associative array of shell variables */
};


extern void myerror(FILE *, char *fmt, ...);
extern void fatal(FILE *, char *fmt, ...);
extern void krqsort ( void *arrayname[], int, int,
         int (*functionname)(void *, void *));
extern void *binsearch(void *key,void *v[],int n, int (*comp) (void *, void *));
extern void *FindNameIn(void **,int,char *,int);
extern char MissingData[];
extern int line;
extern int debug;
extern char *GetShell(char *); /* return value of shell variable */
extern char CurrentFile[];


#endif
