
# line 2 "num_knuth.y"
/* num_knuth.y: Yacc grammer for Knuth's number nomenclature. */
          /* ( Programming exercise, 1/94 ) */
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
	There are shift/reduce conflicts. Is there a way to do this
	without them? The default shift resolution works here because
	of the ordering of rules.

To compile: yacc -d num_knuth.y
	    lex num_knuth.l
	    cc y.tab.c lex.yy.c -o num_knuth -ly -ll

usage: num_knuth [ < input_file ] [ > outputfile ]
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define TRUE 0
#define FALSE 1
#define BUFSIZE 1000
#define DENOMINATIONS 10 /*  tens, hundreds, myriads, ... */

char buf[BUFSIZE];          /* buffer */
char names[DENOMINATIONS * DENOMINATIONS][BUFSIZE];
int indx = 0;

char *name( int diad );  /* Names numbers in range 00-99 */
void clearbuf( char *first, int size ); /* does what it says */
void wipe(void); /* reinitialize the big buffer */

static char *digit_names[] = { "", " one", " two", " three",
	" four", " five", " six", " seven", " eight", " nine"};

static char *teen_names[] = { " ten", " eleven", " twelve", 
	" thirteen", " fourteen", " fifteen", " sixteen", 
	" seventeen", " eighteen", " nineteen"};

static char *tens_names[] = { " twenty", " thirty", " forty",
	" fifty", " sixty", " seventy", " eighty", " ninety"};



# line 62 "num_knuth.y"
typedef union  {
	int i;
	char *str;
	} YYSTYPE;
# define LITERAL 257
# define EON 258
# define WHITESPACE 259
# define DIGIT 260
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern short yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yylval, yyval;
# define YYERRCODE 256

# line 265 "num_knuth.y"


char *name( int diad)
/* returns pointer to the English phrase for a number from 01 - 99 */
{
	int ones,tens; /* digits */
	char *localbuf;

	clearbuf(buf,BUFSIZE);
	
	tens = diad/10;
	ones = (diad - tens*10);

	if( tens == 0) sprintf(buf,"%s",digit_names[ones]);
	else 
	{    if( tens == 1)
		sprintf(buf,"%s",teen_names[ones]);
		else {
			sprintf(buf,"%s",
			tens_names[tens-2]);
			strcat(buf,digit_names[ones]);
		     	}
	}
	return buf;
}

void clearbuf( char *first, int size )
{
	int i;
	for(i=0;i<size;i++) *(first + i) = '\0';
	return ;
} 

/* wipe: clear the big names buffer */
void wipe(void)
{	
	int count;

	for(count=0;count< DENOMINATIONS * DENOMINATIONS; count++)
		clearbuf(names[count],BUFSIZE);
	return;
}

	
short yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
	};
# define YYNPROD 47
# define YYLAST 255
short yyact[]={

  25,  54,  29,  53,   4,  52,  49,  24,  24,  46,
  43,  24,  24,  40,  32,  24,  24,  37,  23,  16,
  36,  16,  15,  35,  15,  14,  19,  14,  13,  22,
  20,  21,   7,   3,   2,  12,  28,  11,  18,   1,
   6,  51,  50,  48,  27,  45,  38,  34,  34,  42,
  10,  41,  47,  44,  33,  39,  31,  30,   5,   9,
   0,   0,  26,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   8,   0,  17,
  24,   8,   0,  17,  24 };
short yypact[]={

-1000,  -6,-1000, -10,-1000,-1000,-1000,-1000,-1000,-256,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-244,-245,
-248,-249,-252,-253,-257,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000 };
short yypgo[]={

   0,  18,  17,  29,  20,  31,  23,  30,  28,  26,
  35,  38,  37,  59,  50,  39,  34,  33,  58,  40,
  32 };
short yyr1[]={

   0,  15,  15,  16,  16,  17,  17,  17,  17,  17,
  17,  20,  18,  19,  19,  19,  19,  19,  19,  19,
  19,  13,  14,  14,  14,  11,  12,  12,  12,   9,
  10,  10,  10,   7,   8,   8,   8,   5,   6,   6,
   6,   3,   4,   4,   4,   1,   2 };
short yyr2[]={

   0,   0,   2,   2,   1,   2,   2,   2,   1,   1,
   1,   1,   1,   2,   1,   1,   1,   1,   1,   1,
   1,   2,   2,   2,   1,   2,   2,   2,   1,   2,
   2,   2,   1,   2,   2,   2,   1,   2,   2,   2,
   1,   2,   2,   2,   1,   2,   2 };
short yychk[]={

-1000, -15, -16, -17,  10, -18, -19, -20, 257, -13,
 -14, -12, -10,  -8,  -6,  -4,  -2, 259, -11,  -9,
  -7,  -5,  -3,  -1, 260,  10, -18, -19, -20, 258,
 -11, -12, 258, -10,  -8,  -6,  -4,  -2,  -9, -10,
 258,  -7,  -8, 258,  -5,  -6, 258,  -3,  -4, 258,
  -1,  -2, 258, 260, 258 };
short yydef[]={

   1,  -2,   2,   0,   4,   8,   9,  10,  12,   0,
  14,  15,  16,  17,  18,  19,  20,  11,   0,   0,
   0,   0,   0,   0,   0,   3,   5,   6,   7,  13,
  21,  22,  23,  28,  32,  36,  40,  44,  25,  26,
  27,  29,  30,  31,  33,  34,  35,  37,  38,  39,
  41,  42,  43,  45,  46 };
# line 1 "/usr/lib/yaccpar"
#ifndef lint
static char yaccpar_sccsid[] = "@(#)yaccpar	4.1	(Berkeley)	2/11/83";
#endif not lint

# define YYFLAG -1000
# define YYERROR goto yyerrlab
# define YYACCEPT return(0)
# define YYABORT return(1)

/*	parser for yacc output	*/

#ifdef YYDEBUG
int yydebug = 0; /* 1 for debugging */
#endif
YYSTYPE yyv[YYMAXDEPTH]; /* where the values are stored */
int yychar = -1; /* current input token number */
int yynerrs = 0;  /* number of errors */
short yyerrflag = 0;  /* error recovery flag */

yyparse() {

	short yys[YYMAXDEPTH];
	short yyj, yym;
	register YYSTYPE *yypvt;
	register short yystate, *yyps, yyn;
	register YYSTYPE *yypv;
	register short *yyxi;

	yystate = 0;
	yychar = -1;
	yynerrs = 0;
	yyerrflag = 0;
	yyps= &yys[-1];
	yypv= &yyv[-1];

 yystack:    /* put a state and value onto the stack */

#ifdef YYDEBUG
	if( yydebug  ) printf( "state %d, char 0%o\n", yystate, yychar );
#endif
		if( ++yyps>= &yys[YYMAXDEPTH] ) { yyerror( "yacc stack overflow" ); return(1); }
		*yyps = yystate;
		++yypv;
		*yypv = yyval;

 yynewstate:

	yyn = yypact[yystate];

	if( yyn<= YYFLAG ) goto yydefault; /* simple state */

	if( yychar<0 ) if( (yychar=yylex())<0 ) yychar=0;
	if( (yyn += yychar)<0 || yyn >= YYLAST ) goto yydefault;

	if( yychk[ yyn=yyact[ yyn ] ] == yychar ){ /* valid shift */
		yychar = -1;
		yyval = yylval;
		yystate = yyn;
		if( yyerrflag > 0 ) --yyerrflag;
		goto yystack;
		}

 yydefault:
	/* default state action */

	if( (yyn=yydef[yystate]) == -2 ) {
		if( yychar<0 ) if( (yychar=yylex())<0 ) yychar = 0;
		/* look through exception table */

		for( yyxi=yyexca; (*yyxi!= (-1)) || (yyxi[1]!=yystate) ; yyxi += 2 ) ; /* VOID */

		while( *(yyxi+=2) >= 0 ){
			if( *yyxi == yychar ) break;
			}
		if( (yyn = yyxi[1]) < 0 ) return(0);   /* accept */
		}

	if( yyn == 0 ){ /* error */
		/* error ... attempt to resume parsing */

		switch( yyerrflag ){

		case 0:   /* brand new error */

			yyerror( "syntax error" );
		yyerrlab:
			++yynerrs;

		case 1:
		case 2: /* incompletely recovered error ... try again */

			yyerrflag = 3;

			/* find a state where "error" is a legal shift action */

			while ( yyps >= yys ) {
			   yyn = yypact[*yyps] + YYERRCODE;
			   if( yyn>= 0 && yyn < YYLAST && yychk[yyact[yyn]] == YYERRCODE ){
			      yystate = yyact[yyn];  /* simulate a shift of "error" */
			      goto yystack;
			      }
			   yyn = yypact[*yyps];

			   /* the current yyps has no shift onn "error", pop stack */

#ifdef YYDEBUG
			   if( yydebug ) printf( "error recovery pops state %d, uncovers %d\n", *yyps, yyps[-1] );
#endif
			   --yyps;
			   --yypv;
			   }

			/* there is no state on the stack with an error shift ... abort */

	yyabort:
			return(1);


		case 3:  /* no shift yet; clobber input char */

#ifdef YYDEBUG
			if( yydebug ) printf( "error recovery discards char %d\n", yychar );
#endif

			if( yychar == 0 ) goto yyabort; /* don't discard EOF, quit */
			yychar = -1;
			goto yynewstate;   /* try again in the same state */

			}

		}

	/* reduction by production yyn */

#ifdef YYDEBUG
		if( yydebug ) printf("reduce %d\n",yyn);
#endif
		yyps -= yyr2[yyn];
		yypvt = yypv;
		yypv -= yyr2[yyn];
		yyval = yypv[1];
		yym=yyn;
			/* consult goto table to find next state */
		yyn = yyr1[yyn];
		yyj = yypgo[yyn] + *yyps + 1;
		if( yyj>=YYLAST || yychk[ yystate = yyact[yyj] ] != -yyn ) yystate = yyact[yypgo[yyn]];
		switch(yym){
			
case 3:
# line 92 "num_knuth.y"
{printf("\n");} break;
case 4:
# line 93 "num_knuth.y"
{printf("\n");} break;
case 11:
# line 104 "num_knuth.y"
{ printf("%s",yypvt[-0].str);/* pass through whitespace */} break;
case 12:
# line 107 "num_knuth.y"
{printf("%s",yypvt[-0].str);/* pass through non-numbers */} break;
case 13:
# line 110 "num_knuth.y"
{printf("%s",yypvt[-1].str);wipe();} break;
case 14:
# line 111 "num_knuth.y"
{printf("%s",yypvt[-0].str);wipe();} break;
case 15:
# line 112 "num_knuth.y"
{printf("%s",yypvt[-0].str);wipe();} break;
case 16:
# line 113 "num_knuth.y"
{printf("%s",yypvt[-0].str);wipe();} break;
case 17:
# line 114 "num_knuth.y"
{printf("%s",yypvt[-0].str);wipe();} break;
case 18:
# line 115 "num_knuth.y"
{printf("%s",yypvt[-0].str);wipe();} break;
case 19:
# line 116 "num_knuth.y"
{printf("%s",yypvt[-0].str);wipe();} break;
case 20:
# line 117 "num_knuth.y"
{printf("%s",yypvt[-0].str);wipe();} break;
case 21:
# line 122 "num_knuth.y"
{ char *ptr = &names[indx][0]; clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,yypvt[-0].str);
		if(*ptr != '\0')
		strcat(ptr, " quadryllion");
	        yyval.str = strcat(ptr,yypvt[-1].str);
		indx++;
		} break;
case 22:
# line 130 "num_knuth.y"
{ char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,yypvt[-0].str);
		strcat(ptr, " quadryllion");
	        yyval.str = strcat(ptr,yypvt[-1].str);
		indx++;
		} break;
case 23:
# line 137 "num_knuth.y"
{ char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
	        yyval.str = strcpy(ptr,yypvt[-1].str);
		indx++;} break;
case 24:
# line 142 "num_knuth.y"
{/* produces conflict */ } break;
case 25:
# line 144 "num_knuth.y"
{ char *ptr = &names[indx][0]; clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,yypvt[-0].str);
		if(*ptr != '\0')
		strcat(ptr, " tryllion");
	        yyval.str = strcat(ptr,yypvt[-1].str);
		indx++;
		} break;
case 26:
# line 152 "num_knuth.y"
{ char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,yypvt[-0].str);
		strcat(ptr, " tryllion");
	        yyval.str = strcat(ptr,yypvt[-1].str);
		indx++;
		} break;
case 27:
# line 159 "num_knuth.y"
{ char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
	        yyval.str = strcpy(ptr,yypvt[-1].str);
		indx++;} break;
case 28:
# line 164 "num_knuth.y"
{/* produces conflict */ } break;
case 29:
# line 167 "num_knuth.y"
{ char *ptr = &names[indx][0]; clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,yypvt[-0].str);
		if(*ptr != '\0')
		strcat(ptr, " byllion");
	        yyval.str = strcat(ptr,yypvt[-1].str);
		indx++;
		} break;
case 30:
# line 175 "num_knuth.y"
{ char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,yypvt[-0].str);
		strcat(ptr, " byllion");
	        yyval.str = strcat(ptr,yypvt[-1].str);
		indx++;
		} break;
case 31:
# line 182 "num_knuth.y"
{ char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
	        yyval.str = strcpy(ptr,yypvt[-1].str);
		indx++;} break;
case 32:
# line 187 "num_knuth.y"
{/* produces conflict */ } break;
case 33:
# line 190 "num_knuth.y"
{ char *ptr = &names[indx][0]; clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,yypvt[-0].str);
		if(*ptr != '\0')
		strcat(ptr, " myllion");
	        yyval.str = strcat(ptr,yypvt[-1].str);
		indx++;
		} break;
case 34:
# line 198 "num_knuth.y"
{ char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,yypvt[-0].str);
		strcat(ptr, " myllion");
	        yyval.str = strcat(ptr,yypvt[-1].str);
		indx++;
		} break;
case 35:
# line 205 "num_knuth.y"
{ char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
	        yyval.str = strcpy(ptr,yypvt[-1].str);
		indx++;} break;
case 36:
# line 210 "num_knuth.y"
{/* produces conflict */ } break;
case 37:
# line 213 "num_knuth.y"
{ char *ptr = &names[indx][0]; clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,yypvt[-0].str);
		if(*ptr != '\0')
		strcat(ptr, " myriad");
	        yyval.str = strcat(ptr,yypvt[-1].str);
		indx++;
		} break;
case 38:
# line 221 "num_knuth.y"
{ char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,yypvt[-0].str);
		strcat(ptr, " myriad");
	        yyval.str = strcat(ptr,yypvt[-1].str);
		indx++;
		} break;
case 39:
# line 228 "num_knuth.y"
{ char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
	        yyval.str = strcpy(ptr,yypvt[-1].str);
		indx++;} break;
case 40:
# line 233 "num_knuth.y"
{/* produces conflict */ } break;
case 41:
# line 236 "num_knuth.y"
{ char *ptr = &names[indx][0]; clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,yypvt[-0].str);
		if(*ptr != '\0')
		strcat(ptr, " hundred");
	        yyval.str = strcat(ptr,yypvt[-1].str);
		indx++;
		} break;
case 42:
# line 244 "num_knuth.y"
{ char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,yypvt[-0].str);
		strcat(ptr, " hundred");
	        yyval.str = strcat(ptr,yypvt[-1].str);
		indx++;
		} break;
case 43:
# line 251 "num_knuth.y"
{ char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
	        yyval.str = strcpy(ptr,yypvt[-1].str);
		indx++;} break;
case 44:
# line 256 "num_knuth.y"
{ /* produces conflict */ } break;
case 45:
# line 259 "num_knuth.y"
{char *ptr = &names[indx][0];clearbuf(ptr,BUFSIZE);
		 strcpy(ptr, name(10*yypvt[-0].i + yypvt[-1].i)); yyval.str = ptr;indx++;} break;
case 46:
# line 262 "num_knuth.y"
{char *ptr = &names[indx][0];clearbuf(ptr,BUFSIZE);
		 strcpy(ptr, digit_names[yypvt[-1].i]); yyval.str = ptr;indx++;} break;
# line 148 "/usr/lib/yaccpar"

		}
		goto yystack;  /* stack new state and value */

	}
