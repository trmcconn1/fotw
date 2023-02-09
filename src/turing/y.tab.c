/* A Bison parser, made from parse.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

# define	TOKEN	257
# define	KEYWORD	258
# define	HARDENDFILE	259
# define	SOFTENDFILE	260

#line 1 "parse.y"


#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdarg.h>
#include "global.h"
#include "stack.h"
#include "macro.h"

#define RESERVED "RL"
extern char current_token[];        /* see lex.l */
extern int tape[];
extern int tindxmax;
extern int tindx;
extern int line;
extern int macroline;
extern int state;
extern struct stackstruct *pending_inputs;
extern struct stackstruct *pending_buffers;
extern struct stackstruct *pending_prefixes;
extern void new_input(FILE *);
extern char input_filename[];
char *tokens[MAXTOKENS];
int preprocess = 0;
static int ntokens = 0;
static char current_directive[MAXTOKEN];
static int directive_flag = 0;
static int error_flag = 0;
static int exit_flag = 0;
static int if_lvl = 0;
static unsigned toggles = 0xFF;  /* Supports ifdef, etc */
static char state_prefix[MAXTOKEN]="";
char alphabet[MAX_ALPHABET] = "0123456789";
int alphabet_size = 10;
int nstates = 0;
struct statestruct *states[MAX_STATES]; /* initialized in lex.l */
int i;

void handle_directive(void);
void handle_instruction(void);
static int warn(char *,...);
int error(char *, ...);

#ifndef YYSTYPE
# define YYSTYPE int
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 0
#endif



#define	YYFINAL		11
#define	YYFLAG		-32768
#define	YYNTBASE	8

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 260 ? yytranslate[x] : 12)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       7,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    10,    12,    15
};
static const short yyrhs[] =
{
      10,     9,     0,     7,     0,     6,     0,     5,     0,     0,
       4,     0,    10,    11,     0,     3,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,    53,    98,    99,   100,   102,   103,   108,   111
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "TOKEN", "KEYWORD", "HARDENDFILE", 
  "SOFTENDFILE", "'\\n'", "start", "terminator", "tokenlist", "token", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,     8,     9,     9,     9,    10,    10,    10,    11
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     2,     1,     1,     1,     0,     1,     2,     1
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       5,     6,     0,     8,     4,     3,     2,     1,     7,     0,
       0,     0
};

static const short yydefgoto[] =
{
       9,     7,     2,     8
};

static const short yypact[] =
{
       1,-32768,    -3,-32768,-32768,-32768,-32768,-32768,-32768,     6,
       7,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,-32768,-32768
};


#define	YYLAST		7


static const short yytable[] =
{
       3,     0,     4,     5,     6,     1,    10,    11
};

static const short yycheck[] =
{
       3,    -1,     5,     6,     7,     4,     0,     0
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison/bison.simple"

/* Skeleton output parser for bison,

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software
   Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser when
   the %semantic_parser declaration is not specified in the grammar.
   It was written by Richard Stallman by simplifying the hairy parser
   used when %semantic_parser is specified.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

#if ! defined (yyoverflow) || defined (YYERROR_VERBOSE)

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || defined (YYERROR_VERBOSE) */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
# if YYLSP_NEEDED
  YYLTYPE yyls;
# endif
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# if YYLSP_NEEDED
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAX)
# else
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)
# endif

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif


#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).

   When YYLLOC_DEFAULT is run, CURRENT is set the location of the
   first token.  By default, to implement support for ranges, extend
   its range to the last symbol.  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)       	\
   Current.last_line   = Rhs[N].last_line;	\
   Current.last_column = Rhs[N].last_column;
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#if YYPURE
# if YYLSP_NEEDED
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, &yylloc, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval, &yylloc)
#  endif
# else /* !YYLSP_NEEDED */
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval)
#  endif
# endif /* !YYLSP_NEEDED */
#else /* !YYPURE */
# define YYLEX			yylex ()
#endif /* !YYPURE */


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

#ifdef YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif
#endif

#line 315 "/usr/share/bison/bison.simple"


/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif

/* YY_DECL_VARIABLES -- depending whether we use a pure parser,
   variables are global, or local to YYPARSE.  */

#define YY_DECL_NON_LSP_VARIABLES			\
/* The lookahead symbol.  */				\
int yychar;						\
							\
/* The semantic value of the lookahead symbol. */	\
YYSTYPE yylval;						\
							\
/* Number of parse errors so far.  */			\
int yynerrs;

#if YYLSP_NEEDED
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES			\
						\
/* Location data for the lookahead symbol.  */	\
YYLTYPE yylloc;
#else
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES
#endif


/* If nonreentrant, generate the variables here. */

#if !YYPURE
YY_DECL_VARIABLES
#endif  /* !YYPURE */

int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  /* If reentrant, generate the variables here. */
#if YYPURE
  YY_DECL_VARIABLES
#endif  /* !YYPURE */

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack. */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

#if YYLSP_NEEDED
  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
#endif

#if YYLSP_NEEDED
# define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
# define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  YYSIZE_T yystacksize = YYINITDEPTH;


  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
#if YYLSP_NEEDED
  YYLTYPE yyloc;
#endif

  /* When reducing, the number of symbols on the RHS of the reduced
     rule. */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
#if YYLSP_NEEDED
  yylsp = yyls;
#endif
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  */
# if YYLSP_NEEDED
	YYLTYPE *yyls1 = yyls;
	/* This used to be a conditional around just the two extra args,
	   but that might be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
# else
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);
# endif
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
# if YYLSP_NEEDED
	YYSTACK_RELOCATE (yyls);
# endif
# undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
#if YYLSP_NEEDED
      yylsp = yyls + yysize - 1;
#endif

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
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
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

#if YYDEBUG
     /* We have to keep this `#if YYDEBUG', since we use variables
	which are defined only if `YYDEBUG' is set.  */
      if (yydebug)
	{
	  YYFPRINTF (stderr, "Next token is %d (%s",
		     yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise
	     meaning of a token, for further debugging info.  */
# ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
# endif
	  YYFPRINTF (stderr, ")\n");
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
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to the semantic value of
     the lookahead token.  This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

#if YYLSP_NEEDED
  /* Similarly for the default location.  Let the user run additional
     commands if for instance locations are ranges.  */
  yyloc = yylsp[1-yylen];
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
#endif

#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] > 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif

  switch (yyn) {

case 1:
#line 53 "parse.y"
{
					if( directive_flag )
					  handle_directive();
					  else 
						if(toggles == 0xFF)
							if(!preprocess)
					  			handle_instruction();
							else { /* print instruction */
							      if(ntokens){
								printf(".file %s\n",input_filename);
								printf(".line %d\n", macroline);
								}
								for(i=0;i<ntokens;i++){
								printf("%s ",tokens[i]);
							}
							if(ntokens)
							  printf("\n\n\n");
						}

					/* clean up */ 

					  for(i=0;i<ntokens;i++)
						free(tokens[i]);
				        ntokens = 0;
					if(error_flag){
						error_flag = 0;
						directive_flag = 0;
						return ERROR;
					}
					if(exit_flag){
						if(exit_flag==2){
							exit_flag=0;
							return HARDEOF;
						}
						exit_flag = 0;
						return SOFTEOF;
					}
					else if(directive_flag){
						directive_flag = 0;
						return DIRECTIVE;
					      }
					      else return INSTRUCTION;
			     }
    break;
case 3:
#line 99 "parse.y"
{ exit_flag = 1; }
    break;
case 4:
#line 100 "parse.y"
{ exit_flag = 2; }
    break;
case 6:
#line 103 "parse.y"
{ 
	        	strcpy(current_directive,current_token+1);
		        directive_flag = 1; 
		}
    break;
case 8:
#line 111 "parse.y"
{tokens[ntokens++] = (char *)strdup(current_token);}
    break;
}

#line 705 "/usr/share/bison/bison.simple"


  yyvsp -= yylen;
  yyssp -= yylen;
#if YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;
#if YYLSP_NEEDED
  *++yylsp = yyloc;
#endif

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[YYTRANSLATE (yychar)]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[YYTRANSLATE (yychar)]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* defined (YYERROR_VERBOSE) */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*--------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action |
`--------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;
      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;


/*-------------------------------------------------------------------.
| yyerrdefault -- current state does not do anything special for the |
| error token.                                                       |
`-------------------------------------------------------------------*/
yyerrdefault:
#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */

  /* If its default is to accept any token, ok.  Otherwise pop it.  */
  yyn = yydefact[yystate];
  if (yyn)
    goto yydefault;
#endif


/*---------------------------------------------------------------.
| yyerrpop -- pop the current state because it cannot handle the |
| error token                                                    |
`---------------------------------------------------------------*/
yyerrpop:
  if (yyssp == yyss)
    YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#if YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "Error: state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

/*--------------.
| yyerrhandle.  |
`--------------*/
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

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

/*---------------------------------------------.
| yyoverflowab -- parser overflow comes here.  |
`---------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}
#line 114 "parse.y"


int compare_states(struct statestruct *a, struct statestruct *b)
{
	if(!a)return -1;
	if((!b) && (!a)) return 0;
	return strcmp(a->name,b->name);
}

void handle_instruction(void)
{
	int i,j,n,found;
	char c;
	char buffer[MAX_PREFIX+ MAXTOKEN+1];
	struct statestruct field0;
	char *field1;
	char *field2;
	struct statestruct field3;
	struct instruction *an_instruction,*ip;
	struct statestruct *p;

	/* Check to make sure there is room for the instruction. No point
           in going further if not */

	if(ninstr >= MAX_INSTRUCTIONS){

		fprintf(stderr,"Fatal error at line %d\n",line);
		fprintf(stderr,"Too many instructions. (Precompliled limit is %d)\n",MAX_INSTRUCTIONS);
		fprintf(stderr,"Reduce source, or recompile with larger limit.\n");
		exit(1);

	}

	/* Make sure the number of tokens is divisisble by 4 */

	n = ntokens/4;  /* number of instructions */

	if(ntokens % 4) {

			error("incomplete instruction, %d leftover token(s) on line.",ntokens%4);
			error_flag = 1;
			return;
	}

	for(i=0;i<n;i++){ /* loop over instructions (groups of 4 tokens) */

		field0.name = (char *)malloc(MAXTOKEN+1);
		sprintf(field0.name,"%s%s",state_prefix,tokens[i*4]);
		field1 = (char *) strdup(tokens[i*4+1]);
		field2 = (char *) strdup(tokens[i*4+2]);
		field3.name = (char *)malloc(MAXTOKEN+1);
		sprintf(field3.name,"%s%s",state_prefix,tokens[i*4+3]);

		an_instruction = malloc(sizeof(struct instruction));

		if(!an_instruction){
			error("Internal error: can't allocate instruction");
			exit(1);
		}

		/* set action field */

		if(strcmp(field2,"R")==0)an_instruction->action = RIGHT;
		else
			if(strcmp(field2,"L")==0)an_instruction->action = LEFT;
				else an_instruction->action = WRITE;

	        /* See if the first arg is a state, create new one if
                       necessary  */

		if(!(p = binsearch((void *)&field0,(void **)states,nstates,
				(int(*)(void *,void *))compare_states))){
			if(nstates >= MAX_STATES){
				error("Fatal. Too many states. %d is max.\n",MAX_STATES);
				exit(1);
			}
			p = malloc(sizeof(struct statestruct));
			if(!p){
				error("Fatal. Cannot malloc state struct.\n");
				exit(1);
			}
			p->name = (char *)malloc(strlen(field0.name)+1);
			if(!p->name){
				error("Fatal. Cannot malloc state name.\n");
				exit(1);
			}
			strcpy(p->name,field0.name);
			p->indx = nstates;
			states[nstates]=p;
			nstates++;
			krqsort((void **)states,0,nstates-1,
				(int(*)(void *, void *))compare_states);
		}

		an_instruction->state = -(p->indx+1);

		/* Similarly for 4th arg */

		if(!(p = binsearch((void *)&field3,(void **)states,nstates,
				(int(*)(void *,void *))compare_states))){
			if(nstates >= MAX_STATES){
				error("Fatal. Too many states. %d is max.\n",MAX_STATES);
				exit(1);
			}
			p = malloc(sizeof(struct statestruct));
			if(!p){
				error("Fatal. Cannot malloc state struct.\n");
				exit(1);
			}
			p->name = (char *)malloc(strlen(field3.name)+1);
			if(!p->name){
				error("Fatal. Cannot malloc state name.\n");
				exit(1);
			}
			strcpy(p->name,field3.name);
			p->indx = nstates;
			states[nstates]=p;
			nstates++;
			krqsort((void **)states,0,nstates-1,
				(int(*)(void *, void *))compare_states);
		}
		an_instruction->newstate = -(p->indx+1);
		
		/* no longer needed */
		free(field0.name);
		free(field3.name);

		i=0;
		found=0;
		while(alphabet[i]!='\0'){ 
			if(alphabet[i] == field1[0]){
				found = 1;
				break;
			}
			i++;
		}
		if(!found){
			error("%c not in alphabet", field1[0]); 
			error_flag = 1;
		}
		an_instruction->input = i; 

		if(an_instruction->action == WRITE){
			i=0;
			found=0;
			while(alphabet[i]!='\0'){ 
				if(alphabet[i] == field2[0]){
					found = 1;
					break;
				}
				i++;
			}
			if(!found){
				error("%c not in alphabet",field2[0]); 
				error_flag = 1;
			}
			an_instruction->output = i; 
		}

		/* no longer needed */
		free(field1);
		free(field2);

		/* See whether the instruction exists. It should not. */

		ip = binsearch((void *)an_instruction,(void **)instructions,ninstr,
				(int(*)(void *,void *))compare_inst);
		if(ip != NULL)
			{
				error("matching state/input");
				error_flag = 1;
				free(an_instruction);
			}
		else {  /* All OK. install instruction */	

			instructions[ninstr++] = an_instruction;
			krqsort((void **)instructions,0,ninstr-1,
				(int(*)(void *,void *))compare_inst);

		}

	}
}



static char macro_text[MAX_MACRO_TEXT];
static char arglist[MAX_MACRO_TEXT];

void handle_directive(void)
{
	int i,c,n,found=0;

	/* Note to self: when you add a directive, don't forget to
           update is_directive in lex.l */

	if(strcmp(current_directive,"endif")==0){

		if(!if_lvl)error("unmatched .ifdef or .ifndef\n");
		else {
		if_lvl--;
		toggles |= (1<<if_lvl);
		}
		return;
		
	}


	if(strcmp(current_directive,"ifdef")==0){

		if(!getmacro(tokens[0]))
			toggles &= ~(1<<if_lvl);
		if_lvl++;
		return;
	}

	if(strcmp(current_directive,"ifndef")==0){

		if(getmacro(tokens[0]))
		        toggles &= ~(1<<if_lvl);
		if_lvl++;
		return;
	}

        /* Remaining are ignored when there is a pending false ifdef or
           ifndef as indicated by toggles */

	if(toggles < 0xFF)return;

	/* undef directive */
	if(strcmp(current_directive,"undef")==0){
		if(rmmacro(tokens[0]))
			warn("undef: no such macro %s\n",tokens[0]);
		return;
	}

	/* status directive */
	if(strcmp(current_directive,"status")==0){
		found=1;
		printf("\nCurrent compilation status:\n\n");
		printf("Alphabet: %s\n",alphabet);
		printf("%d instructions compiled.\n",ninstr); 
		dump_macros();
		printf("States (n=%d):\n",nstates);
		for(i=0;i<nstates;i++)
			printf("%d. %s\n",states[i]->indx,states[i]->name);
		return;
	}

	/* line directive */

	if(strcmp(current_directive,"line")==0){
			if(ntokens == 1)
				line = atoi(tokens[0]);
				macroline = line;
			found = 1;
	}

	/* file directive */

	if(strcmp(current_directive,"file")==0){
			if(ntokens == 1)
				strcpy(input_filename,tokens[0]);
			found = 1;
	}

	/* echo directive */
	if(strcmp(current_directive,"echo")==0){
		found = 1;
		if(ntokens)
			for(i=0;i<ntokens;i++)
				printf("%s ",tokens[i]);
		printf("\n");
	}

	/* define directive */
	if(strcmp(current_directive,"define")==0){

		struct macro_struct *my_macro;
		char *p,*q;
		
		found = 1;	

		/* See if a macro by this name is already defined */

		my_macro = getmacro(tokens[0]);
		if(my_macro){
			warn("macro %s already defined",tokens[0]);
			return;
		}

		macro_text[0] = '\0';
			for(i=1;i<ntokens;i++){
				strcat(macro_text,tokens[i]);	
				strcat(macro_text," ");
			}

				/* See if macro_text starts with a (. If so
                                then we need to get at the argmuments.) */
				i=0;	
				if(macro_text[0]=='('){

					/* search for closing ')', putting
                                           everything except whitespace 
                                           between into a buffer */
					
					n=0;
					for(i=1;i<strlen(macro_text);i++){
						if(macro_text[i]==')'){
							i++;
							break;
						}
						if(macro_text[i]==' ')continue;
						arglist[n++]=macro_text[i];
					}
					arglist[n] = '\0';
					if(i==strlen(macro_text)){
						error("Bad macro syntax for %s\n",tokens[0]);
						return;
					}
					
				}	
				my_macro = (struct macro_struct *)
					malloc(sizeof(struct 
							macro_struct));
				my_macro->value = (char *) strdup(macro_text+i);
				my_macro->name = (char *) strdup(tokens[0]);
				if(n){ /* non-empty arg list */

					my_macro->argc=0;
					my_macro->argv = (char **)malloc(n*sizeof(char *));
					p = strtok(arglist,",");
					if(p){
						*(my_macro->argc+my_macro->argv)
							= (char *)strdup(p);
						my_macro->argc++;
					}
					while(p=strtok(NULL,",")){
						*(my_macro->argc+my_macro->argv)
							= (char *)strdup(p);
						(my_macro->argc)++;
					}
				}
				addmacro(my_macro);
			return;
	}

	/* include directive */
	if(strcmp(current_directive,"include")==0){
		FILE *afile;

		found=1;
		afile = fopen(tokens[0],"r");
		if(!afile){
			error("cannot open include file %s", tokens[0]);
			error_flag = 1;
			return;
		}

		new_input(afile);
		strcpy(input_filename,tokens[0]);
		
	}

	/* prepend directive */
	if(strcmp(current_directive,"prepend")==0){
		char *p,*q,*r;
		found=1;
		if(ntokens<=0){
			p = pop_stack(pending_prefixes);
			if(p){
			/* wipe out last occurence of string p in 
                                  state_prefix */
				r = state_prefix;
				while((q = strstr(r+1,p)))r=q;
				*r = '\0';
				free(p);
			}
		}
		else {
			push_stack(pending_prefixes,(void *)strdup(tokens[0]));
			strcat(state_prefix,tokens[0]);
		}
	}

	/* error directive */

	if(strcmp(current_directive,"error")==0){

		error(tokens[0]);
		error_flag = 1;
		return;
	}

	if(strcmp(current_directive,"warn")==0){

		warn(tokens[0]);
		return;
	}
	/* alphabet directive */
	if(strcmp(current_directive,"alphabet")==0){

		found=1;

		/* check the proposed new alphabet: any reserved letters ? */

		if(strpbrk(tokens[0],RESERVED)!=NULL){
			error("reserved letter in alphabet");
			error_flag = 1;
			return;
		}
		
		strcpy(alphabet,tokens[0]);
		alphabet_size = strlen(alphabet);
		if(ntokens > 1)warn("Extra tokens on line");
	}

	/* tape directive */
	if(strcmp(current_directive,"tape")==0){

		char *p;
		int head = 0; /* flag to indicate non default head */
		found = 1;

		n = strlen(tokens[0]);
		for(i=0;i<n;i++){
			if(tokens[0][i]=='R'){
				tape[tindxmax++]=-1;
				tindx = tindxmax;
				head = 1;
			}
			else {
			  p = strchr(alphabet,tokens[0][i]);
			  if(p)tape[tindxmax++]=(int)(p-alphabet);
			  else {
				error("tape directive has invalid (non-alphabet?) character %c",
				tokens[0][i]);
				error_flag = 1;
				break;
			  }
			}
			
		}
		if(head)tape[0]=0;
	}
	if(!found){
			
		error("unknown directive %s", current_directive);
		error_flag = 1;
		
	}

}

int warn ( char *format, ...){

	va_list ap;
	
	va_start(ap,format);
	fprintf(stderr,"%s: %d(warning): ",input_filename,macroline);
	vfprintf(stderr,format,ap);
	fprintf(stderr,"\n");
	va_end(ap);
	return 0;
}

int error( char *format, ...) {

	va_list ap;

	va_start(ap, format);
	fprintf(stderr,"%s:%d: ",input_filename,macroline);
       	vfprintf(stderr,format,ap);
	fprintf(stderr,"\n");
	for(i=0;i<ntokens;i++)free(tokens[i]);
	ntokens = 0;
	va_end(ap);
	return 0;
}
 
int yyerror( char *msg ){
	return error("%s",msg);
}
