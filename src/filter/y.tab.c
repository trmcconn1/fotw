
/*  A Bison parser, made from fgn.y with Bison version GNU Bison version 1.22
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	ENDFILE	258
#define	ENDCOMMANDS	259
#define	OPTION	260
#define	ARG_TAG	261
#define	RIGHTARG	262
#define	LEFTARG	263
#define	LITERAL_BEGIN	264
#define	LITERAL_END	265
#define	CHAR	266
#define	BACKSLASHES	267
#define	SLASH	268
#define	C_ESCAPE	269
#define	BACKDOLLAR	270

#line 1 "fgn.y"

/* fgn.y: generate a lex specification for a filter
	using a command syntax similar to sed 

	Version 1.1   Febuary 1995  Chicken Haven Software, Inc
		by Terry McConnell

This is a preprocessor for lex. It writes its outfut in a file named
ff.out.l in the current directory. Essentially, this program offers
extensions to lex, setting up standard Unix option processing and allowing
parts of the matched patterns to be manipulated in the action code.

usage: fgn inputfile.

inputfile contains literal blocks of c-code, comments and commands. 

In the literal block the user can (and should) #define a string called
INFO which will be displayed whenever the filter is invoked with the
option -help. (There is a default info string which is not helpful.)

A command  has the form:
[+-]option  /regexp/  { c-code }

Where + introduces an option for which the following filter command line argument
is to be used as an argument. It is available in the variable clvar. Only
one option should take an argument, and it must be given last on the
filter command line. 

one command must describe the default action of the filter in case
no options are given on the command line. It has the same format, but
the name of the option is _ (underscore)


The same option may occur on multiple lines, but all lines with
the same option must occur together
The regexp is optional. The regexp may contain sections of the form
\(stuff\). What is matched by the first such is substituted for the
expression \$0 in the c-code section, etc. The rest of regexp is echoed.

Whenever possible, the user should eschew the use of such variables, and
especially when only one is needed. In that case use yytext instead. 

A single command-line argument may be passed to the generated filter after
the end of options. It is available in the string char *clvar.

Usage of the filter generated:

name [options] [option arg] [input file]

If no input file is given, the filter reads from stdin.   

All options are munged before any processing, so if multiple options are
allowed the command file should include commands for all possible orderings of
munged options.

For example, the command line

filter -f -y arg file

gets converted to 

filter -fy arg file

Here arg is an argument taken by the -y option, and filter will read from
file. The command file must contain lines of the form +fy. 

Multiple file names can be given on the command. If - is used as a 
filename it indicates stdin. This may occur anywhere in a file list. 

You may use -- to indicate the end of options ( useful for input files
whose names begin with -.) It also should be used when there are no
options, but a command line argument is expected. 
Lines beginning with # are comments     */

/* 	bugs: won't handle nested variables  */
/*   And probably never will :-) */
/*   There is one shift/reduce conflict */

#include <string.h>
#include <stdio.h>
#include "/usr/include/ansi/stdlib.h"
#include "fgn.h"

#define BUFSIZE 10000 /* must be big to allow for long action code */
#define MAXARGLEN 1000 /* longest possible arg pattern */
#define MAXOPTIONLEN 80 /* longest option name */
#define NVARS 10     /* number of variable tags in code */
#define MAXOPTS 20 /* maximum number of options */

int first_lex =1;    /* switch to put scanner in start state */
int command_ctr =0; /* for error messages */
int option_ctr =1; /* counts how many lines for each option */
int no_opts =0;   /* actual number of options used */
int count;

static char last_option[MAXOPTIONLEN] ="none"; 
static char options[MAXOPTS][MAXOPTIONLEN]; /* names of all options */
static char stringbuf[BUFSIZE];
char *bufptr;
static char argbuf[MAXARGLEN]; /* buffer for argument regexp */
char *argbufptr;

struct option_node {      /* built for each command line */
	char *name;  /* name of option */
	char *pattern; /* the full regexp for this line */
	int vars; /* number of \( \)'s on this line */
	char *varpats[NVARS]; /* what's in each \(\) */
	char *action_code; /* C-code for this line */
	} option_node;

extern FILE *yyin;
FILE   *ffout, *temp, *last;  /* pointers to output and temporary file */
           /* and to file holding anything beyond the %% */
void filecopy(FILE *ifp, FILE *ofp);
void clearbuf(char *, int);
char *strcp(char *);   /* same as strdup, but returns char * */
void start_states( FILE *, struct option_node , int );
void pattern_lines(FILE *, struct option_node, int );
void make_main(FILE *, char a[][MAXOPTIONLEN], int);
void yyerror(char *);

#line 122 "fgn.y"
typedef union {
	char *str;
	char chr;
	int integer;
	} YYSTYPE;

#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		45
#define	YYFLAG		-32768
#define	YYNTBASE	20

#define YYTRANSLATE(x) ((unsigned)(x) <= 270 ? yytranslate[x] : 30)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    19,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,    16,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    17,     2,    18,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     4,     7,    11,    14,    18,    19,    22,    25,    28,
    32,    35,    38,    42,    46,    50,    53,    55,    58,    61,
    64,    65,    68,    71,    74,    77,    80,    83,    86,    89
};

static const short yyrhs[] = {    21,
    29,     3,     0,    21,     3,     0,    22,    23,     4,     0,
    23,     4,     0,     9,    29,    10,     0,     0,    23,    24,
     0,    23,    11,     0,    23,    16,     0,     5,    25,    26,
     0,     5,    26,     0,     5,    11,     0,     5,    25,    11,
     0,    16,    29,    16,     0,    17,    29,    18,     0,    28,
     7,     0,     8,     0,    28,    11,     0,    28,    12,     0,
    28,    13,     0,     0,    29,    11,     0,    29,    27,     0,
    29,     6,     0,    29,    14,     0,    29,    12,     0,    29,
    13,     0,    29,    15,     0,    29,    19,     0,    29,     4,
     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   144,   147,   149,   174,   202,   219,   220,   221,   223,   228,
   254,   281,   283,   287,   294,   301,   309,   310,   311,   313,
   317,   318,   319,   323,   331,   333,   335,   337,   340,   342
};

static const char * const yytname[] = {   "$","error","$illegal.","ENDFILE",
"ENDCOMMANDS","OPTION","ARG_TAG","RIGHTARG","LEFTARG","LITERAL_BEGIN","LITERAL_END",
"CHAR","BACKSLASHES","SLASH","C_ESCAPE","BACKDOLLAR","'/'","'{'","'}'","'\\n'",
"sourcefile","rules","literal_block","command_list","command","regexp","code",
"arg","arg_inc","string",""
};
#endif

static const short yyr1[] = {     0,
    20,    20,    21,    21,    22,    23,    23,    23,    23,    24,
    24,    24,    24,    25,    26,    27,    28,    28,    28,    28,
    29,    29,    29,    29,    29,    29,    29,    29,    29,    29
};

static const short yyr2[] = {     0,
     3,     2,     3,     2,     3,     0,     2,     2,     2,     3,
     2,     2,     3,     3,     3,     2,     1,     2,     2,     2,
     0,     2,     2,     2,     2,     2,     2,     2,     2,     2
};

static const short yydefact[] = {     6,
    21,    21,     6,     0,     0,     2,     0,     0,     4,     0,
     8,     9,     7,    30,    24,    17,     5,    22,    26,    27,
    25,    28,    29,    23,     0,     1,     3,    12,    21,    21,
     0,    11,    16,    18,    19,    20,     0,     0,    13,    10,
    14,    15,     0,     0,     0
};

static const short yydefgoto[] = {    43,
     2,     3,     4,    13,    31,    32,    24,    25,     5
};

static const short yypact[] = {    -8,
-32768,     1,-32768,    44,    11,-32768,    -1,    57,-32768,    55,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,    52,-32768,-32768,-32768,-32768,-32768,
    58,-32768,-32768,-32768,-32768,-32768,    25,    39,-32768,-32768,
-32768,-32768,     6,     8,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,    13,-32768,-32768,   -22,-32768,-32768,    -2
};


#define	YYLAST		75


static const short yytable[] = {     7,
     1,    26,    14,     6,    15,    44,    16,    45,    40,    18,
    19,    20,    21,    22,    14,     8,    15,    23,    16,     0,
    17,    18,    19,    20,    21,    22,    37,    38,    14,    23,
    15,     0,    16,     0,     0,    18,    19,    20,    21,    22,
    41,     0,    14,    23,    15,     0,    16,     9,    10,    18,
    19,    20,    21,    22,    11,     0,    42,    23,    33,    12,
    27,    10,    34,    35,    36,    28,     0,    11,    39,     0,
    29,    30,    12,     0,    30
};

static const short yycheck[] = {     2,
     9,     3,     4,     3,     6,     0,     8,     0,    31,    11,
    12,    13,    14,    15,     4,     3,     6,    19,     8,    -1,
    10,    11,    12,    13,    14,    15,    29,    30,     4,    19,
     6,    -1,     8,    -1,    -1,    11,    12,    13,    14,    15,
    16,    -1,     4,    19,     6,    -1,     8,     4,     5,    11,
    12,    13,    14,    15,    11,    -1,    18,    19,     7,    16,
     4,     5,    11,    12,    13,    11,    -1,    11,    11,    -1,
    16,    17,    16,    -1,    17
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/local/lib/bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Bob Corbett and Richard Stallman

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */


#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#define YYLEX		yylex(&yylval, &yylloc)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_bcopy(FROM,TO,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_bcopy (from, to, count)
     char *from;
     char *to;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_bcopy (char *from, char *to, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 184 "/usr/local/lib/bison.simple"
int
yyparse()
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_bcopy ((char *)yyss1, (char *)yyss, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_bcopy ((char *)yyvs1, (char *)yyvs, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_bcopy ((char *)yyls1, (char *)yyls, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 144 "fgn.y"
{ printf("sourcefile parsed\n");
/* copy everything read in to ffout: */
	 fprintf(ffout,"%s",stringbuf);clearbuf(stringbuf,BUFSIZE);YYACCEPT;;
    break;}
case 2:
#line 147 "fgn.y"
{ YYACCEPT;;
    break;}
case 3:
#line 149 "fgn.y"
{ printf("commands parsed\n");
/* add the default pattern line. */
		fprintf(temp,".|[\\n]\t{ECHO;}\n");
/* add %% */
		fprintf(temp,"%%%%\n"); 
/* Make sure there is a default option line  */
/* Else warn */
	for(count=0;count <= no_opts-1;count++)
		if(strcmp(&options[count][0]+1,"_") == 0)goto checks;
			yyerror("No default option line\n");
			checks: ;

/* Write the main program and accompanying declarations */
	make_main(temp,options,no_opts);
/* Append temp to the end of ffout */

	fclose(ffout);
	ffout = fopen("ff.out.l","a");
	rewind(temp);
	filecopy(temp,ffout);
/* prepare for additional input after command block */
	clearbuf(stringbuf,BUFSIZE);
	bufptr = stringbuf;
	;
    break;}
case 4:
#line 174 "fgn.y"
{
/* add the default pattern line. */
		fprintf(temp,".|[\\n]\t{ECHO;}\n");
/* add %% */
		fprintf(temp,"%%%%\n");
/* Make sure there is a default option line  */
/* Else warn */
	for(count=0;count <= no_opts-1;count++)
		if(strcmp(&options[count][0]+1,"_") == 0)goto checks2;
			yyerror("No default option line\n");
			checks2: ;

/* Write the main program and accompanying declarations */
	make_main(temp,options,no_opts);
/* Append temp to the end of ffout */

	fclose(ffout);
	ffout = fopen("ff.out.l","a");
	rewind(temp);
	filecopy(temp,ffout);
/* prepare for additional input after command block */
	clearbuf(stringbuf,BUFSIZE);
	bufptr = stringbuf;
	;
    break;}
case 5:
#line 202 "fgn.y"
{
                               /* copy to output stream ffout  */
	 fprintf(ffout,"%%{\n");
	 fprintf(ffout,"%s",stringbuf);clearbuf(stringbuf,BUFSIZE);
/* End of user literal block stuff. Put default literal block stuff
		here */
#ifndef __NEXT__
	fprintf(ffout,"#include <string.h>\n");
#endif
	fprintf(ffout,"#define NVARS 10\n");
	fprintf(ffout,"char *var[NVARS];\n");
	fprintf(ffout,"char *clvar;\n");
/* Mark the end of the literal block */
	 fprintf(ffout,"%%}\n");
		bufptr=stringbuf;;
    break;}
case 8:
#line 221 "fgn.y"
{yyerror("missing - before option");
					YYABORT;;
    break;}
case 9:
#line 223 "fgn.y"
{yyerror("missing option tag");
					YYABORT;;
    break;}
case 10:
#line 228 "fgn.y"
{
	option_node.name = strcp(yyvsp[-2].str);
		command_ctr++;
/* Make sure the option lines are together */
	for(count=0;count < no_opts-1;count++){
		if(strcmp(&options[count][0],option_node.name) == 0){
			yyerror("Split option lines.\n");
			YYABORT;
			}
		}
/* Increase option counter if this option has been seen before */
	if( strcmp(option_node.name, last_option) == 0)
		option_ctr++;
	else {
		option_ctr = 1;
		strcpy(&options[no_opts][0],option_node.name);
		no_opts++;
		}
	strcpy(last_option , option_node.name);
/* Make the new start states */
	start_states(ffout,option_node,option_ctr);
/* Write the action code */
	pattern_lines(temp,option_node,option_ctr);
/* Reset for next command line */
	option_node.vars = 0;
;
    break;}
case 11:
#line 254 "fgn.y"
{
	option_node.name = strcp(yyvsp[-1].str)+1;
		command_ctr++;
/* Make sure the option lines are together */
	for(count=0;count < no_opts-1;count++){
		if(strcmp(&options[count][0],option_node.name) == 0){
			yyerror("Split option lines.\n");
			YYABORT;
			}
		}
/* Increase option counter if this option has been seen before */
	if( strcmp(option_node.name, last_option) == 0)
		option_ctr++;
	else {
		option_ctr = 1;
		option_ctr = 1;
		strcpy(&options[no_opts][0],option_node.name);
		no_opts++;
	     }
	strcpy(last_option , option_node.name);
/* Make the new start states */
	start_states(ffout,option_node,option_ctr);
/* Write the action code */
	pattern_lines(temp,option_node,option_ctr);
/* Reset for next command line */
	option_node.vars = 0;
;
    break;}
case 12:
#line 281 "fgn.y"
{yyerror("Missing / or extra characters before /\n");
		YYABORT;;
    break;}
case 13:
#line 283 "fgn.y"
{yyerror("Missing { or extra char befor {\n");
		YYABORT;;
    break;}
case 14:
#line 287 "fgn.y"
{option_node.pattern =
		strcp(stringbuf); /* store regexp string */
		clearbuf(stringbuf,BUFSIZE); /* reset */
		bufptr = stringbuf; ;
    break;}
case 15:
#line 294 "fgn.y"
{ option_node.action_code = 
		strcp(stringbuf); /* copy code */
		clearbuf(stringbuf,BUFSIZE); /* reset */
		bufptr = stringbuf; ;
    break;}
case 16:
#line 301 "fgn.y"
{ yyval.str = strcp(yyvsp[-1].str); /* reset for next arg: */
				argbufptr = argbuf;
				clearbuf(argbuf,MAXARGLEN);
			/* store this arg pattern: */
				option_node.varpats[option_node.vars]=yyval.str;
				option_node.vars++;
			;
    break;}
case 17:
#line 309 "fgn.y"
{ /* point at buffer */ yyval.str = argbuf;;
    break;}
case 18:
#line 310 "fgn.y"
{ *argbufptr++ = yyvsp[0].chr; /* copy a CHAR */;
    break;}
case 19:
#line 311 "fgn.y"
{ /* Literal \, ie \\ in regexp */
				strcat(argbufptr,"\\\\"); argbufptr +=2; ;
    break;}
case 20:
#line 313 "fgn.y"
{ /* Literal /, ie \/ in regexp */
				strcat(argbufptr,"\\/"); argbufptr +=2; ;
    break;}
case 22:
#line 318 "fgn.y"
{*bufptr++ =yyvsp[0].chr;;
    break;}
case 23:
#line 319 "fgn.y"
{ /* append the arg back to regexp string */
				strcat(bufptr,yyvsp[0].str);
				bufptr += strlen(yyvsp[0].str); ;
    break;}
case 24:
#line 323 "fgn.y"
{ int i = yyvsp[0].integer;
				sprintf(bufptr," var[");
				bufptr +=5;
				*bufptr++ = '0' + i;
				*bufptr++ = ']';
				*bufptr++ = ' ';
				 /* only 10 args ok */
			;
    break;}
case 25:
#line 331 "fgn.y"
{ /* C-escape sequence in code */
				strcat(bufptr,yyvsp[0].str); bufptr +=strlen(yyvsp[0].str); ;
    break;}
case 26:
#line 333 "fgn.y"
{ /* Literal \, ie \\ in regexp */
				strcat(bufptr,"\\\\"); bufptr +=2; ;
    break;}
case 27:
#line 335 "fgn.y"
{ /* Literal /, ie \/ in regexp */
				strcat(bufptr,"\\/"); bufptr +=2; ;
    break;}
case 28:
#line 337 "fgn.y"
{ /* A literal \$ in code. Don't laugh, it
				_could_ happen */
				strcat(bufptr,"\\$"); bufptr +=2; ;
    break;}
case 29:
#line 340 "fgn.y"
{yyerror("Newline in string.\n");
		YYABORT;;
    break;}
case 30:
#line 342 "fgn.y"
{yyerror("Unexpected end of file\n");YYABORT;;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 465 "/usr/local/lib/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 346 "fgn.y"

int
main(int argc, char **argv)
{

/* Look to see if there is an input file. Else use stdin  */

	if(argc!=1){ if((yyin = fopen(argv[1],"r"))==NULL){
			printf("filgen: can't open %s\n",argv[1]);
			return 1;}
		}

/* Open stream for output. */

	if((ffout = fopen("ff.out.l", "w"))==NULL){
		printf("filgen: can't open ff.out\n");
		return 1;
		}

/* Open temporary scratch file */

	temp = tmpfile();

	setbuf(temp,NULL); /* These may need to be changed */
	setbuf(ffout,NULL);

/* Put in %% to mark beginning of pattern lines section */
	fprintf(temp,"%%%%\n");

/* initialize pointers */

	bufptr = stringbuf;
	argbufptr = argbuf;
	option_node.vars = 0;

	yyparse();

	fclose(ffout);
	fclose(yyin);

	return 0;
}

/* filecopy: copy file ifp to file ofp */
void filecopy(FILE *ifp, FILE *ofp)
{
	int c;

	while ((c=fgetc(ifp)) != EOF)
		fputc(c, ofp);
}

void clearbuf( char *first, int size)
{
	int i;
	for(i=0;i<size;i++) *(first + i) = '\0';
	return;
}

/* strcp: the same as strdup. Some implementations idiotically return
int instead of char *  */

char *strcp( char *s)
{
	char *p;

	p = (char *) malloc(strlen(s)+1);
	if(p != NULL)
		strcpy(p,s);
	return p;
}

/* start_states: Based on the information in the option_node, i.e., 
from a single command line, set up the required set of start states. For
an option named o with 3 argument tags they are:

%s s_o 
%s o0
%s o1
%s o2

If the same option has a 2nd command line, then 
start numbering the rest from 10.

output goes to file fp 
*/

void
start_states(FILE *fp, struct option_node o, int ctr )
{
	int vs;
	vs = o.vars;
	if(ctr == 1)
		fprintf(fp,"%%s s_%s\n",o.name+1);
	while(vs-- > 0)
		fprintf(fp,"%%s %s%-d\n",o.name+1,vs+10*(ctr-1));
	return;
}

/* pattern_lines: write the pattern lines for each command line. For
the example in the start_state comment above, the pattern lines should be:
<s_o>pattern   { push back matched input and switch to state o1}
<o1>pattern1   { store matched input in var[0] and switch to o2}
<o1>.|[\n]     { ECHO anything before pattern 1 }
etc
where the command line has the form
-o /...\(pattern1\).../    { code }
The last pattern requires a different treatment:
<o2>pattern2  { store in var [2], action code, reset code }
*/

void
pattern_lines(FILE *fp, struct option_node o, int ctr )
{
	int count;
	fprintf(fp,"<s_%s>",o.name+1);
	fprintf(fp,"%s\t",o.pattern);
	if(o.vars != 0){
	fprintf(fp,"{BEGIN %s%-d; yyless(0);}\n",o.name+1,10*(ctr-1));
	for(count =0; count < o.vars -1; count++){
		fprintf(fp,"<%s%-d>",o.name+1,count+10*(ctr-1));
		fprintf(fp,"%s\t",o.varpats[count]);
		fprintf(fp,"{var[%c]=strdup(yytext);BEGIN %s%-d;}\n",'0'+
			count,o.name+1,count+10*(ctr-1)+1);
		fprintf(fp,"<%s%-d>",o.name+1,count+10*(ctr-1));
		fprintf(fp,".|[\\n]\t{ECHO;}\n");
		}
	fprintf(fp,"<%s%-d>",o.name+1,count+10*(ctr-1));
	fprintf(fp,"%s\t",o.varpats[count]);
	fprintf(fp,"{var[%c]=strdup(yytext);",'0'+
		count);
	}
	else fprintf(fp,"{");
	fprintf(fp,"%s",o.action_code);
	if(o.vars != 0)
	fprintf(fp,"BEGIN s_%s;}\n",o.name+1);
	else fprintf(fp,"}\n");
/* echo everything that doesn't match */
	if(o.vars != 0){
	fprintf(fp,"<%s%-d>",o.name+1,count+10*(ctr-1));
	fprintf(fp,".|[\\n]\t{ECHO;}\n");
	}
	else fprintf(fp,"<s_%s>.|[\\n]\t{ECHO;}\n",o.name+1);
	return;
}

/*  make_main: writes struct options, main etc. See example.l file */

void
make_main(FILE *fp, char array[][MAXOPTIONLEN], int n)
{
	int nops = n;
	/* write struct options */
	fprintf(fp,"struct options{\n");
	fprintf(fp,"char *name;\n");
	fprintf(fp,"int code;\n");
	fprintf(fp,"} options[] =\n{ ");
	while(--n >= 0)
		fprintf(fp,"\"%s\",%d,\n",&array[n][0],nops-n);
	fprintf(fp,"\"-\", 0\n};\n");
/* define a default info if the user has not defined one */
	fprintf(fp,"#ifndef INFO\n");
	fprintf(fp,"#define INFO \"Sorry, no information available\"\n");
	fprintf(fp,"#endif\n");
/* Stuff for file chaining */
	fprintf(fp,"char **fileList;\n");
	fprintf(fp," unsigned nFiles;\n");
	fprintf(fp," unsigned currentFile = 0;\n");
/* declare find_opt */
	fprintf(fp,"int find_opt(char *);\n");
	fprintf(fp,"int cmdlinearg=0;\n");
/* write main */
	fprintf(fp,"int main(int argc, char **argv)\n");
	fprintf(fp,"{\nint c;\n");
	fprintf(fp,"char *info = INFO;\n");
	fprintf(fp,"char argstr[80] = \"-\";\n");
	fprintf(fp,"if(--argc > 0){\n");
	fprintf(fp,"if((*++argv)[0]==\'-\'){\n");
	fprintf(fp,"argc--;\n");
	fprintf(fp,"if((*argv)[1]=='-'){c=find_opt(\"-_\");++argv;}\n");
	fprintf(fp,"else{ strcat(argstr,++*argv);\n");
	fprintf(fp,"while((argc>0)&&((*++argv)[0]==\'-\')&&((*argv)[1]!=\'-\')){\n");
	fprintf(fp,"argc--;\n");
	fprintf(fp,"strcat(argstr,++*argv);}\n");
	fprintf(fp,"c=find_opt(argstr);}\n");
	fprintf(fp,"switch(c){\n");
	fprintf(fp,"case 0: printf(\"%%s\\n\",info);\n");
	fprintf(fp,"return 0;\n");

	n = nops;
	while(--n>=0){
		fprintf(fp,"case %d: \n",nops-n);
		fprintf(fp,"BEGIN s_%s;\n",&array[n][0]+1);
		fprintf(fp,"break;\n");
	}
	fprintf(fp,"default:\n");
	fprintf(fp,"fprintf(stderr,\"%%s: illegal option\\n\",argstr);\n");
	fprintf(fp,"return 1;\n}\n");
	fprintf(fp,"}else BEGIN s__;}\n");
	fprintf(fp,"else BEGIN s__;\n");
	fprintf(fp,"if(cmdlinearg==1){\n");
	fprintf(fp,"if(argc>0){clvar=*argv;argc--;\n");
	fprintf(fp,"++argv;}\n");
	fprintf(fp,"else {fprintf(stderr,\"Option argument expected\\n\");\n");
	fprintf(fp,"exit(1);}}\n");
	fprintf(fp,"if(argc>0){\n");
	fprintf(fp,"argc--;\n");
	fprintf(fp,"if(strcmp(*argv,\"-\")!=0){\n");
	fprintf(fp,"if((yyin=fopen(*argv,\"r\"))==NULL){\n");
	fprintf(fp,"fprintf(stderr,\"Cannot open %%s\\n\",*argv);\n");
	fprintf(fp,"return 1;}}}\n");
	fprintf(fp,"nFiles=argc;\n");
	fprintf(fp,"fileList=argv+1;\n");
	fprintf(fp,"yylex();\n");
	fprintf(fp,"return 0;\n}\n");
/* write function find_opt */
	fprintf(fp,"int find_opt(char *word)\n{\n");
	fprintf(fp,"int count=0;\n");
	fprintf(fp,"if(strcmp(word,\"-help\")==0)return 0;\n");
	fprintf(fp,"while(strcmp(options[count].name,\"-\")!=0){\n");
	fprintf(fp,"if(strcmp(options[count].name+1,word+1)==0){\n");
	fprintf(fp,"if(options[count].name[0]==\'+\'){\n");
	fprintf(fp,"cmdlinearg=1;\n");
	fprintf(fp,"}\n");
	fprintf(fp,"return options[count].code;}\n");
	fprintf(fp,"count++;\n}\nreturn -1;\n}\n");
/* Write yywrap() */
fprintf(fp,"yywrap()\n");
fprintf(fp,"{\n");
fprintf(fp,"FILE *file;\n");
fprintf(fp,"if((nFiles>=1)&&(currentFile < nFiles))\n");
fprintf(fp,"fclose(yyin);\n");
fprintf(fp,"else{ fclose(yyin);\n");
fprintf(fp,"return 1;\n");
fprintf(fp,"}\n");
fprintf(fp,"while(fileList[currentFile]!=(char *)0){\n");
fprintf(fp,"if(strcmp(fileList[currentFile],\"-\")==0){\n");
fprintf(fp,"currentFile++;\n");
fprintf(fp,"file=stdin;\n");
fprintf(fp,"yyin=file;\n");
fprintf(fp,"break;\n");
fprintf(fp,"}\n");
fprintf(fp,"file=fopen(fileList[currentFile++],\"r\");\n");
fprintf(fp,"if(file != NULL ){\n");
fprintf(fp,"yyin=file;\n");
fprintf(fp,"break;\n");
fprintf(fp,"}\n");
fprintf(fp,"fprintf(stderr,\"Cannot open %%s \\n\",fileList[currentFile-1]);\n");
fprintf(fp,"}\n");
fprintf(fp,"return(file ? 0 : 1);\n");
fprintf(fp,"}\n");
}
