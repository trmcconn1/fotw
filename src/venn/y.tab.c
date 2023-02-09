
# line 2 "parse.y"
	/* Parse a simple 3 set algebra for use with Venn diagram demo
           program */


#include "venn.h"
	
# define OR 257
# define AND 258
# define NOT_PREFIX 259
# define NOT_POSTFIX 260
# define A 261
# define B 262
# define C 263
# define S 264
# define EMPTY 265
# define ENDFILE 266
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern short yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
#ifndef YYSTYPE
#define YYSTYPE int
#endif
YYSTYPE yylval, yyval;
# define YYERRCODE 256

# line 41 "parse.y"


char myinput[BUFSIZ];
char *myinputptr;  /* current position in myinput */
int myinputlim; /* end of data */

int
main( int argc, char **argv)
{
	int result;
	char bindigits[8];
	int i;

	if(argc < 2) {
		fprintf(stderr,"Usage: venn <set algebra expression>\n");	
		return 1;	
	}
	strcpy(myinput,*++argv);
	myinputlim = strlen(myinput);
	myinputptr = myinput;
	result = yyparse();
	printf("The set has value: ");
	i = 0;
	if(result == 0) printf("00000000\n");
	else{
	  while(result){
		if(result % 2)bindigits[i++]='1';
		else bindigits[i++]='0';
		result = result/2;
	  }
	  for(i=7;i>=0;i--)printf("%c",bindigits[i]);
	  printf("\n");
	}
	return 0;
}
short yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
	};
# define YYNPROD 13
# define YYLAST 231
short yyact[]={

   4,  13,  12,  12,  14,  14,  14,   2,   3,   1,
  11,  19,  15,  16,   0,   0,   0,   0,   0,   0,
  17,  18,   0,   0,   0,   0,   0,   0,   0,   0,
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
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   5,
   0,   6,   7,   8,   9,  10,   0,  13,  12,   0,
  14 };
short yypact[]={

 -40,-1000,-256,-1000, -40, -40,-1000,-1000,-1000,-1000,
-1000,-1000, -40, -40,-1000, -30,-254,-254,-255,-1000 };
short yypgo[]={

   0,   9,   7,   8 };
short yyr1[]={

   0,   1,   2,   2,   2,   2,   2,   2,   3,   3,
   3,   3,   3 };
short yyr2[]={

   0,   2,   1,   3,   3,   3,   2,   2,   1,   1,
   1,   1,   1 };
short yychk[]={

-1000,  -1,  -2,  -3,  40, 259, 261, 262, 263, 264,
 265, 266, 258, 257, 260,  -2,  -2,  -2,  -2,  41 };
short yydef[]={

   0,  -2,   0,   2,   0,   0,   8,   9,  10,  11,
  12,   1,   0,   0,   7,   0,   6,   3,   4,   5 };
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
			
case 1:
# line 25 "parse.y"
{ return yypvt[-1]; } break;
case 3:
# line 29 "parse.y"
{ yyval = yypvt[-2] & yypvt[-0]; } break;
case 4:
# line 30 "parse.y"
{ yyval = yypvt[-2] | yypvt[-0]; } break;
case 5:
# line 31 "parse.y"
{ yyval = yypvt[-1];} break;
case 6:
# line 32 "parse.y"
{ yyval = 0xFF - yypvt[-0];} break;
case 7:
# line 33 "parse.y"
{ yyval = 0xFF - yypvt[-1];} break;
case 8:
# line 35 "parse.y"
{ yyval = SET_A;} break;
case 9:
# line 36 "parse.y"
{ yyval = SET_B; } break;
case 10:
# line 37 "parse.y"
{ yyval = SET_C; } break;
case 11:
# line 38 "parse.y"
{ yyval = SET_S; } break;
case 12:
# line 39 "parse.y"
{ yyval = SET_EMPTY;} break;
# line 148 "/usr/lib/yaccpar"

		}
		goto yystack;  /* stack new state and value */

	}
