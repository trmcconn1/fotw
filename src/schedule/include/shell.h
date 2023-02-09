#include "db.h"
#include "ui.h"
#include<string.h>

#ifdef __MSDOS__
#include<alloc.h>
extern unsigned _stklen;
#else
#include<unistd.h>
#endif

#define COMMENT '#'
#define MAXVARS 256
#define FLDSEP "        \n"   /* Fieldsep space, tab, newline */
#define MAX_CMDS 50
#define MAX_LVL 5

#define SHELLSTREAMS FILE *shell_istream, FILE *shell_ostream,\
FILE *shell_errorstream, FILE *prompt_stream


extern char COLS[];
extern int set(char *, char *, int, FILE *);

extern int numschedules;

extern char common_buf[];
extern char littlebuf[];

/* Line parsing: cf load.c */
extern int tokenize(char *, struct TokenType**);
extern int numtokens;
extern void init_defaults();

extern char *GetShell(char *); /* return value of shell variable */

