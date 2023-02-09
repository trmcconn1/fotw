
/*  A Bison parser, made from num_to_name.y with Bison version GNU Bison version 1.22
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	COMMA	258
#define	LITERAL	259
#define	WHITESPACE	260
#define	DIGIT	261
#define	EDIGIT	262
#define	EON	263

#line 1 "num_to_name.y"

/* num_to_name.y: Yacc grammer for number-to-English filter. */
          /* ( Programming exercise, 1/94 ) */
/*
 
	usage: num_to_name [-k] [< input] [> output]

	options: k: use Knuth's nomenclature instead
			of standard English.
Default:  (English)
*/
/* Parsing assumes the number has commas, e.g., 1,102,001 */
/* The lexer (num_to_name.l) ensures this is the case */
/* This is a filter: it passes non-numbers on through, and
	changes numbers to english, e.g
 I saw 121 girls -> I saw one hundred twenty one girls

/* With the -k option:
*/
/* Parsing assumes numbers come in in reverse */
/* The lexer (num_knuth.l) ensures this is the case */
/* This is a filter: it passes non-numbers on through, and
	changes numbers to Knuth nomenclature, e.g
 I saw 121344 girls -> I saw twelve myriad, thirteen hundred 
 forty four girls

For a description of Knuth's nomenclature,

D.E.Knuth,"Supernatural Numbers," The Mathematical Gardner,
Wadsworth International, 1981, 310-323.

bugs:	Won't handle decimals.
	Won't handle numbers at the end of a sentence or with
		other punctuation right after them as numbers.
	Won't handle ordinals, e.g 2nd -> second
	Expansion on numbers may make lines very long
	"Only" goes up to decillions. No undecillions.
	In -k option:
	There are reduce/reduce conflicts. Is there a way to do this
	without them? The default shift resolution works here because
	of the ordering of rules.

*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define TRUE 0
#define TRUE 0
#define FALSE 1
#define BUFSIZE 1000
#define DENOMINATIONS 10 /*  tens, hundreds, myriads, ... */

char names[DENOMINATIONS * DENOMINATIONS][BUFSIZE];
/* Used to store names of each denomination. For example, with
12344321, "forty three hundred twenty one" is the name of the
hundreds denomination, and "twelve hundred thirty four" is the name
of the myriads denomination. These must be stored in separate
locations because names of lower denominations get used to build names
of higher ones */
int indx = 0;
char *name( int triad );  /* Names numbers in range 00-999 */
char *kname( int diad );  /* Names numbers in range 00-99 */
/* used with -k option */
void clearbuf( char *first, int size ); /* does what it says */
void wipe(void); /* reinitialize the big names buffer */
int start_state = 1; /* default. 1 means English, 2 means Knuthian */
char buf[BUFSIZE];          /* number buffer */
char *prepend(const char *, char *); /* supplement to string.h */
char localbuf[BUFSIZE]; /* used by prepend */

static char *digit_names[] = { "", " one", " two", " three",
	" four", " five", " six", " seven", " eight", " nine"};

static char *teen_names[] = { " ten", " eleven", " twelve", 
	" thirteen", " fourteen", " fifteen", " sixteen", 
	" seventeen", " eighteen", " nineteen"};

static char *tens_names[] = { " twenty", " thirty", " forty",
	" fifty", " sixty", " seventy", " eighty", " ninety"};



#line 86 "num_to_name.y"
typedef union {int i;
	char *str;
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



#define	YYFINAL		106
#define	YYFLAG		-32768
#define	YYNTBASE	10

#define YYTRANSLATE(x) ((unsigned)(x) <= 263 ? yytranslate[x] : 44)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     9,
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
     6,     7,     8
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     4,     7,     9,    12,    15,    18,    21,    23,
    25,    27,    29,    31,    33,    35,    37,    39,    41,    43,
    45,    47,    49,    51,    53,    55,    59,    63,    67,    71,
    75,    79,    83,    87,    91,    95,    99,   101,   105,   108,
   110,   113,   115,   117,   119,   121,   123,   125,   127,   130,
   133,   136,   138,   141,   144,   147,   149,   152,   155,   158,
   160,   163,   166,   169,   171,   174,   177,   180,   182,   185,
   188,   191,   193,   196
};

static const short yyrhs[] = {    -1,
    10,    11,     0,    12,     9,     0,     9,     0,    12,    14,
     0,    12,    15,     0,    12,    29,     0,    12,    13,     0,
    14,     0,    15,     0,    29,     0,    13,     0,     5,     0,
     4,     0,    27,     0,    26,     0,    25,     0,    24,     0,
    23,     0,    22,     0,    21,     0,    20,     0,    19,     0,
    18,     0,    16,     0,    28,     3,    17,     0,    28,     3,
    18,     0,    28,     3,    19,     0,    28,     3,    20,     0,
    28,     3,    21,     0,    28,     3,    22,     0,    28,     3,
    23,     0,    28,     3,    24,     0,    28,     3,    25,     0,
    28,     3,    26,     0,    28,     3,    27,     0,    28,     0,
     7,     7,     7,     0,     7,     7,     0,     7,     0,    30,
     8,     0,    31,     0,    33,     0,    35,     0,    37,     0,
    39,     0,    41,     0,    43,     0,    32,    32,     0,    32,
    33,     0,    32,     8,     0,    33,     0,    34,    34,     0,
    34,    35,     0,    34,     8,     0,    35,     0,    36,    36,
     0,    36,    37,     0,    36,     8,     0,    37,     0,    38,
    38,     0,    38,    39,     0,    38,     8,     0,    39,     0,
    40,    40,     0,    40,    41,     0,    40,     8,     0,    41,
     0,    42,    42,     0,    42,    43,     0,    42,     8,     0,
    43,     0,     6,     6,     0,     6,     8,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   112,   113,   116,   117,   120,   121,   123,   124,   125,   126,
   127,   128,   131,   134,   137,   139,   140,   141,   142,   143,
   144,   145,   146,   147,   148,   151,   154,   158,   162,   166,
   170,   174,   179,   184,   189,   194,   199,   203,   204,   205,
   208,   209,   210,   211,   212,   213,   214,   215,   220,   228,
   235,   240,   242,   250,   257,   262,   265,   273,   280,   285,
   288,   296,   303,   308,   311,   319,   326,   331,   334,   342,
   349,   354,   357,   360
};

static const char * const yytname[] = {   "$","error","$illegal.","COMMA","LITERAL",
"WHITESPACE","DIGIT","EDIGIT","EON","'\\n'","line_array","line","text_array",
"filler","text","englishnumber","decillions","nonillions","octillions","septillions",
"sextillions","quintillions","quadrillions","trillions","billions","millions",
"thousands","ehundreds","triad","knuthnumber","quadryllions","quadryllions_inc",
"tryllions","tryllions_inc","byllions","byllions_inc","myllions","myllions_inc",
"myriads","myriads_inc","hundreds","hundreds_inc","tens","tens_inc",""
};
#endif

static const short yyr1[] = {     0,
    10,    10,    11,    11,    12,    12,    12,    12,    12,    12,
    12,    12,    13,    14,    15,    15,    15,    15,    15,    15,
    15,    15,    15,    15,    15,    16,    17,    18,    19,    20,
    21,    22,    23,    24,    25,    26,    27,    28,    28,    28,
    29,    29,    29,    29,    29,    29,    29,    29,    30,    31,
    31,    31,    32,    33,    33,    33,    34,    35,    35,    35,
    36,    37,    37,    37,    38,    39,    39,    39,    40,    41,
    41,    41,    42,    43
};

static const short yyr2[] = {     0,
     0,     2,     2,     1,     2,     2,     2,     2,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     1,     3,     2,     1,
     2,     1,     1,     1,     1,     1,     1,     1,     2,     2,
     2,     1,     2,     2,     2,     1,     2,     2,     2,     1,
     2,     2,     2,     1,     2,     2,     2,     1,     2,     2,
     2,     1,     2,     2
};

static const short yydefact[] = {     1,
     0,    14,    13,     0,    40,     4,     2,     0,    12,     9,
    10,    25,    24,    23,    22,    21,    20,    19,    18,    17,
    16,    15,    37,    11,     0,    42,     0,    43,     0,    44,
     0,    45,     0,    46,     0,    47,     0,    48,    73,    74,
    39,     3,     8,     5,     6,     7,     0,    41,    51,    49,
    50,    56,    60,    64,    68,    72,    55,    53,    54,    59,
    57,    58,    63,    61,    62,    67,    65,    66,    71,    69,
    70,    38,    26,    28,    29,    30,    31,    32,    33,    34,
    35,    36,    37,     0,    27,    37,     0,    37,     0,    37,
     0,    37,     0,    37,     0,    37,     0,    37,     0,    37,
     0,     0,     0,    37,     0,     0
};

static const short yydefgoto[] = {     1,
     7,     8,     9,    10,    11,    12,    73,    13,    14,    75,
    76,    77,    78,    79,    80,    81,    82,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    54,    35,
    55,    37,    56
};

static const short yypact[] = {-32768,
    11,-32768,-32768,    28,    -3,-32768,-32768,    53,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    30,-32768,     0,-32768,    43,-32768,    47,-32768,
    55,-32768,    58,-32768,    59,-32768,    62,-32768,-32768,-32768,
    65,-32768,-32768,-32768,-32768,-32768,    66,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    71,    66,-32768,    72,    66,    73,    66,    74,
    66,    75,    66,    76,    66,    77,    66,    78,    66,    79,
    66,    80,    66,-32768,    27,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,    81,    82,    83,-32768,-32768,     3,   -46,     5,
     6,    18,    20,    21,    22,    23,    24,   -47,    84,-32768,
-32768,    57,    61,    56,    42,    63,    16,    60,     2,    51,
     4,    64,     1
};


#define	YYLAST		101


static const short yytable[] = {    83,
    74,    38,    34,    41,    36,    15,    16,    48,    38,    34,
   105,    36,    15,    16,     2,     3,     4,     5,    17,     6,
    18,    19,    20,    21,    22,    17,   106,    18,    19,    20,
    21,    22,    47,    39,    65,    40,    86,    71,    68,    88,
    74,    90,    53,    92,    53,    94,    62,    96,     4,    98,
    49,   100,     4,   102,    57,   104,     2,     3,     4,     5,
     4,    42,    60,     4,     4,    63,    66,     4,    52,    69,
    59,    72,     5,    84,    87,    89,    91,    93,    95,    97,
    99,   101,   103,    50,    58,    67,    85,    51,    43,    44,
    45,    46,    64,    61,     0,     0,     0,     0,     0,     0,
    70
};

static const short yycheck[] = {    47,
    47,     1,     1,     7,     1,     1,     1,     8,     8,     8,
     0,     8,     8,     8,     4,     5,     6,     7,     1,     9,
     1,     1,     1,     1,     1,     8,     0,     8,     8,     8,
     8,     8,     3,     6,    33,     8,    84,    37,    35,    87,
    87,    89,    27,    91,    29,    93,    31,    95,     6,    97,
     8,    99,     6,   101,     8,   103,     4,     5,     6,     7,
     6,     9,     8,     6,     6,     8,     8,     6,    27,     8,
    29,     7,     7,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,    27,    29,    35,    84,    27,     8,     8,
     8,     8,    33,    31,    -1,    -1,    -1,    -1,    -1,    -1,
    37
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

case 3:
#line 116 "num_to_name.y"
{printf("\n");;
    break;}
case 4:
#line 117 "num_to_name.y"
{printf("\n");;
    break;}
case 6:
#line 121 "num_to_name.y"
{clearbuf(buf,sizeof(buf));/* prepare for
					next  number */;
    break;}
case 10:
#line 126 "num_to_name.y"
{clearbuf(buf,sizeof(buf));;
    break;}
case 13:
#line 131 "num_to_name.y"
{ printf("%s",yyvsp[0].str);/* pass through whitespace */;
    break;}
case 14:
#line 134 "num_to_name.y"
{printf("%s",yyvsp[0].str);/* pass through non-numbers */;
    break;}
case 15:
#line 137 "num_to_name.y"
{buf[ (int) strlen(buf)-1 ] = '\0'; /* kill last ,*/
			 printf("%s",buf+1);/* eat up leading space*/;
    break;}
case 16:
#line 139 "num_to_name.y"
{buf[ (int) strlen(buf)-1 ] = '\0'; printf("%s",buf+1);;
    break;}
case 17:
#line 140 "num_to_name.y"
{buf[ (int) strlen(buf)-1 ] = '\0'; printf("%s",buf+1);;
    break;}
case 18:
#line 141 "num_to_name.y"
{buf[ (int) strlen(buf)-1 ] = '\0';printf("%s",buf+1);;
    break;}
case 19:
#line 142 "num_to_name.y"
{buf[ (int) strlen(buf)-1 ] = '\0';printf("%s",buf+1);;
    break;}
case 20:
#line 143 "num_to_name.y"
{buf[ (int) strlen(buf)-1 ] = '\0';printf("%s",buf+1);;
    break;}
case 21:
#line 144 "num_to_name.y"
{buf[ (int) strlen(buf)-1 ] = '\0';printf("%s",buf+1);;
    break;}
case 22:
#line 145 "num_to_name.y"
{buf[ (int) strlen(buf)-1 ] = '\0';printf("%s",buf+1);;
    break;}
case 23:
#line 146 "num_to_name.y"
{buf[ (int) strlen(buf)-1 ] = '\0';printf("%s",buf+1);;
    break;}
case 24:
#line 147 "num_to_name.y"
{buf[ (int) strlen(buf)-1 ] = '\0';printf("%s",buf+1);;
    break;}
case 25:
#line 148 "num_to_name.y"
{buf[ (int) strlen(buf)-1 ] = '\0';printf("%s",buf+1);;
    break;}
case 26:
#line 151 "num_to_name.y"
{ prepend( " decillion,", buf);
			    prepend( name(yyvsp[-2].i), buf); ;
    break;}
case 27:
#line 154 "num_to_name.y"
{  if( strlen(name(yyvsp[-2].i)) > 0)/*not 000*/
			prepend( " nonillion,", buf);
			    prepend( name(yyvsp[-2].i), buf); ;
    break;}
case 28:
#line 158 "num_to_name.y"
{  if( strlen(name(yyvsp[-2].i)) > 0)
			prepend( " octillion,", buf);
			    prepend( name(yyvsp[-2].i), buf); ;
    break;}
case 29:
#line 162 "num_to_name.y"
{  if( strlen(name(yyvsp[-2].i)) > 0)
			prepend( " septillion,", buf);
			    prepend( name(yyvsp[-2].i), buf); ;
    break;}
case 30:
#line 166 "num_to_name.y"
{  if( strlen(name(yyvsp[-2].i)) > 0)
			prepend( " sextillion,", buf);
			    prepend( name(yyvsp[-2].i), buf); ;
    break;}
case 31:
#line 170 "num_to_name.y"
{  if( strlen(name(yyvsp[-2].i)) > 0)
			prepend( " quintillion,", buf);
			    prepend( name(yyvsp[-2].i), buf); ;
    break;}
case 32:
#line 174 "num_to_name.y"
{  if( strlen(name(yyvsp[-2].i)) > 0)
			prepend( " quadrillion,", buf);
			    prepend( name(yyvsp[-2].i), buf); ;
    break;}
case 33:
#line 179 "num_to_name.y"
{  if( strlen(name(yyvsp[-2].i)) > 0)
			prepend( " trillion,", buf);
			    prepend( name(yyvsp[-2].i), buf); ;
    break;}
case 34:
#line 184 "num_to_name.y"
{  if( strlen(name(yyvsp[-2].i)) > 0)
			prepend( " billion,", buf);
			    prepend( name(yyvsp[-2].i), buf); ;
    break;}
case 35:
#line 189 "num_to_name.y"
{  if( strlen(name(yyvsp[-2].i)) > 0)
			prepend( " million,", buf);
			    prepend( name(yyvsp[-2].i), buf); ;
    break;}
case 36:
#line 194 "num_to_name.y"
{  if( strlen(name(yyvsp[-2].i)) > 0)
				prepend( " thousand,", buf);
			    prepend( name(yyvsp[-2].i), buf); ;
    break;}
case 37:
#line 199 "num_to_name.y"
{ if( strlen(name(yyvsp[0].i)) > 0)
	sprintf( buf, "%s,", name(yyvsp[0].i)); ;
    break;}
case 38:
#line 203 "num_to_name.y"
{ yyval.i = 100*yyvsp[-2].i + 10*yyvsp[-1].i + yyvsp[0].i;;
    break;}
case 39:
#line 204 "num_to_name.y"
{yyval.i = 10*yyvsp[-1].i + yyvsp[0].i;;
    break;}
case 40:
#line 205 "num_to_name.y"
{yyval.i = yyvsp[0].i;;
    break;}
case 41:
#line 208 "num_to_name.y"
{printf("%s",yyvsp[-1].str);wipe();;
    break;}
case 42:
#line 209 "num_to_name.y"
{printf("%s",yyvsp[0].str);wipe();;
    break;}
case 43:
#line 210 "num_to_name.y"
{printf("%s",yyvsp[0].str);wipe();;
    break;}
case 44:
#line 211 "num_to_name.y"
{printf("%s",yyvsp[0].str);wipe();;
    break;}
case 45:
#line 212 "num_to_name.y"
{printf("%s",yyvsp[0].str);wipe();;
    break;}
case 46:
#line 213 "num_to_name.y"
{printf("%s",yyvsp[0].str);wipe();;
    break;}
case 47:
#line 214 "num_to_name.y"
{printf("%s",yyvsp[0].str);wipe();;
    break;}
case 48:
#line 215 "num_to_name.y"
{printf("%s",yyvsp[0].str);wipe();;
    break;}
case 49:
#line 220 "num_to_name.y"
{ char *ptr = &names[indx][0]; clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,yyvsp[0].str);
		if(*ptr != '\0')
		strcat(ptr, " quadryllion");
	        yyval.str = strcat(ptr,yyvsp[-1].str);
		indx++;
		;
    break;}
case 50:
#line 228 "num_to_name.y"
{ char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,yyvsp[0].str);
		strcat(ptr, " quadryllion");
	        yyval.str = strcat(ptr,yyvsp[-1].str);
		indx++;
		;
    break;}
case 51:
#line 235 "num_to_name.y"
{ char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
	        yyval.str = strcpy(ptr,yyvsp[-1].str);
		indx++;;
    break;}
case 52:
#line 240 "num_to_name.y"
{/* produces conflict */ ;
    break;}
case 53:
#line 242 "num_to_name.y"
{ char *ptr = &names[indx][0]; clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,yyvsp[0].str);
		if(*ptr != '\0')
		strcat(ptr, " tryllion");
	        yyval.str = strcat(ptr,yyvsp[-1].str);
		indx++;
		;
    break;}
case 54:
#line 250 "num_to_name.y"
{ char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,yyvsp[0].str);
		strcat(ptr, " tryllion");
	        yyval.str = strcat(ptr,yyvsp[-1].str);
		indx++;
		;
    break;}
case 55:
#line 257 "num_to_name.y"
{ char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
	        yyval.str = strcpy(ptr,yyvsp[-1].str);
		indx++;;
    break;}
case 56:
#line 262 "num_to_name.y"
{/* produces conflict */ ;
    break;}
case 57:
#line 265 "num_to_name.y"
{ char *ptr = &names[indx][0]; clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,yyvsp[0].str);
		if(*ptr != '\0')
		strcat(ptr, " byllion");
	        yyval.str = strcat(ptr,yyvsp[-1].str);
		indx++;
		;
    break;}
case 58:
#line 273 "num_to_name.y"
{ char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,yyvsp[0].str);
		strcat(ptr, " byllion");
	        yyval.str = strcat(ptr,yyvsp[-1].str);
		indx++;
		;
    break;}
case 59:
#line 280 "num_to_name.y"
{ char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
	        yyval.str = strcpy(ptr,yyvsp[-1].str);
		indx++;;
    break;}
case 60:
#line 285 "num_to_name.y"
{/* produces conflict */ ;
    break;}
case 61:
#line 288 "num_to_name.y"
{ char *ptr = &names[indx][0]; clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,yyvsp[0].str);
		if(*ptr != '\0')
		strcat(ptr, " myllion");
	        yyval.str = strcat(ptr,yyvsp[-1].str);
		indx++;
		;
    break;}
case 62:
#line 296 "num_to_name.y"
{ char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,yyvsp[0].str);
		strcat(ptr, " myllion");
	        yyval.str = strcat(ptr,yyvsp[-1].str);
		indx++;
		;
    break;}
case 63:
#line 303 "num_to_name.y"
{ char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
	        yyval.str = strcpy(ptr,yyvsp[-1].str);
		indx++;;
    break;}
case 64:
#line 308 "num_to_name.y"
{/* produces conflict */ ;
    break;}
case 65:
#line 311 "num_to_name.y"
{ char *ptr = &names[indx][0]; clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,yyvsp[0].str);
		if(*ptr != '\0')
		strcat(ptr, " myriad");
	        yyval.str = strcat(ptr,yyvsp[-1].str);
		indx++;
		;
    break;}
case 66:
#line 319 "num_to_name.y"
{ char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,yyvsp[0].str);
		strcat(ptr, " myriad");
	        yyval.str = strcat(ptr,yyvsp[-1].str);
		indx++;
		;
    break;}
case 67:
#line 326 "num_to_name.y"
{ char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
	        yyval.str = strcpy(ptr,yyvsp[-1].str);
		indx++;;
    break;}
case 68:
#line 331 "num_to_name.y"
{/* produces conflict */ ;
    break;}
case 69:
#line 334 "num_to_name.y"
{ char *ptr = &names[indx][0]; clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,yyvsp[0].str);
		if(*ptr != '\0')
		strcat(ptr, " hundred");
	        yyval.str = strcat(ptr,yyvsp[-1].str);
		indx++;
		;
    break;}
case 70:
#line 342 "num_to_name.y"
{ char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,yyvsp[0].str);
		strcat(ptr, " hundred");
	        yyval.str = strcat(ptr,yyvsp[-1].str);
		indx++;
		;
    break;}
case 71:
#line 349 "num_to_name.y"
{ char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
	        yyval.str = strcpy(ptr,yyvsp[-1].str);
		indx++;;
    break;}
case 72:
#line 354 "num_to_name.y"
{ /* produces conflict */ ;
    break;}
case 73:
#line 357 "num_to_name.y"
{char *ptr = &names[indx][0];clearbuf(ptr,BUFSIZE);
		 strcpy(ptr, kname(10*yyvsp[0].i + yyvsp[-1].i)); yyval.str = ptr;indx++;;
    break;}
case 74:
#line 360 "num_to_name.y"
{char *ptr = &names[indx][0];clearbuf(ptr,BUFSIZE);
		 strcpy(ptr, digit_names[yyvsp[-1].i]); yyval.str = ptr;indx++;;
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
#line 364 "num_to_name.y"


int main(int argc, char *argv[]) 
{
	char c;
	while (--argc > 0 && (*++argv)[0] == '-')
		while ( c = *++argv[0])
		switch (c) {
		case 'k':
			 start_state = 2;       /* handle option(s) */
			break;
		default:
			printf("num_to_name: illegal option %c\n", c);
			return 1;
		}
			
	yyparse();
	return 0;
}


char *prepend(const char *first, char *second)

/* Stick first on front of second, return second */
{
	char temp[BUFSIZE];

	strcpy(temp,first);  /* store first in temp */
	strcat(temp,second); /* tack on second */
	strcpy(second,temp);
	return second;
}

char *name( int triad)
/* returns pointer to the English phrase for a number from 001 - 999 */
{
	int ones,tens,hundreds; /* digits */

	clearbuf(localbuf,sizeof(localbuf));
	
	hundreds = triad/100;
	tens = (triad - hundreds*100)/10;
	ones = (triad - hundreds*100 - tens*10);

	if( tens == 0) sprintf(localbuf,"%s",digit_names[ones]);
	else 
	{    if( tens == 1)
		sprintf(localbuf,"%s",teen_names[ones]);
		else {
			sprintf(localbuf,"%s",digit_names[ones]);
			prepend(tens_names[tens-2],localbuf);
		     }  /* starts with "twenty" ^  */
	}
	if(hundreds !=0) {
		prepend(" hundred", localbuf);
		prepend(digit_names[hundreds],localbuf);
	}
	return localbuf;
}

void clearbuf( char *first, int size )
{
	int i;
	for(i=0;i<size;i++) *(first + i) = '\0';
	return ;
}

char *kname( int diad)
/* returns pointer to the English phrase for a number from 01 - 99 */
{
	int ones,tens; /* digits */
		clearbuf(localbuf,BUFSIZE);
	
	tens = diad/10;
	ones = (diad - tens*10);

	if( tens == 0) sprintf(localbuf,"%s",digit_names[ones]);
	else 
	{    if( tens == 1)
		sprintf(localbuf,"%s",teen_names[ones]);
		else {
			sprintf(localbuf,"%s",
			tens_names[tens-2]);
			strcat(localbuf,digit_names[ones]);
		     	}
	}
	return localbuf;
}

/* wipe: clear the big names buffer */
void wipe(void)
{	
	int count;

	for(count=0;count< DENOMINATIONS * DENOMINATIONS; count++)
		clearbuf(names[count],BUFSIZE);
	return;
}


