%{
/*   filgen.l: lexer for filgen.y */
/* 	bugs: won't handle nested variables  */

	static int block_ctr = 0; /* for C code */
	extern int first_lex ;
	int command_no = 0;
#include <stdio.h>
#include "y.tab.h"
char *strdup( char *s);

%}

%s normal regexp code comment strng copyall 


%%
%{
	if(first_lex) {
		BEGIN normal;
		first_lex = 0;
	}
%}

<normal>^#.* {/* ignore: comment line */}

<normal>^[%]\{ {BEGIN copyall; return LITERAL_BEGIN;}
<normal>^[%][%]  {BEGIN  copyall; /* copy everything after %% */ 
                   return ENDCOMMANDS;}
<normal>[ \t\n]+	{;/* ignore whitespace */}
<normal>\/	{BEGIN regexp; return '/';}
<normal>^[+-][_0-9a-zA-Z]+  {yylval.str = strdup(yytext); return OPTION;}
<normal>^. {yyerror("Command-line does not start with -");exit(1);}

<copyall>^[%]\}	{BEGIN normal; return LITERAL_END;}
<copyall>.|[\n]	{yylval.chr=yytext[0]; return CHAR;}
<copyall><<EOF>> {return ENDFILE;}
<regexp>\\\\	{ /* two \\'s */ yylval.str = strdup(yytext); return BACKSLASHES;}
<regexp>\\\(	{ return LEFTARG; }
<regexp>\\\) { return RIGHTARG; }
<regexp>\\\/  { yylval.str=strdup(yytext);return SLASH;}
<regexp>\/    {BEGIN normal; return yytext[0];}
<regexp>.  { yylval.chr=yytext[0]; return CHAR;}
<regexp>\\[\n] {/* ignore escaped newline */;}
<regexp>[\n] {return yytext[0]; /* this will cause an error */}

<normal>\{	{BEGIN code; block_ctr =1;return yytext[0];}
<normal>. { yylval.chr=yytext[0]; return CHAR;}

<code>\{	{yylval.chr=yytext[0]; block_ctr++;return CHAR;}
<code>\}	{if(block_ctr > 1) {
			block_ctr--;
			yylval.chr=yytext[0];
			return CHAR;
			}
		  else  {
			block_ctr--;
			BEGIN normal;
			command_no++;
			return yytext[0];
			}
		}
<code>\/\*	{ BEGIN comment; }

<comment>\*\/	{ BEGIN code; }
<comment>.|[\n] { /* skip everything in a comment */ }

<code>\"	{ BEGIN strng; yylval.chr=yytext[0];return CHAR;}

<strng>\\\$[0-9]  {int i = yytext[2]-'1'+1;yylval.integer = i;return ARG_TAG;}
<strng>\\\\\$[0-9]  {/* Literal \$ */ yylval.str="\\$"; return BACKDOLLAR;}
<strng>\\[ntvbrfa\\?'"]     { yylval.str=yytext;return C_ESCAPE;}
<strng>\\[0-7]{1,3}     {/* octal */ yylval.str=strdup(yytext);return C_ESCAPE;}
<strng>\\x[0-9a-fA-F]+     {/* hex */ yylval.str=strdup(yytext);return C_ESCAPE;}
<strng>\"     { BEGIN code; yylval.chr=yytext[0];return CHAR;}
<strng>.       { yylval.chr=yytext[0];return CHAR;}

<code>\\\$[0-9]  {int i = yytext[2]-'1'+1;yylval.integer = i;return ARG_TAG;}
<code>.|[\n]    {yylval.chr=yytext[0];return CHAR;}
%%

yyerror( const char *msg)
{
	fprintf(stderr,"barfed on line %d near token %s\n", command_no,yytext);
	fprintf(stderr, "%s\n",msg);
}
