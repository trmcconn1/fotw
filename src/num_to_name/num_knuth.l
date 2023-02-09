%{
/* num_knuth.l: lexer for num_knuth.y  */

#include "y.tab.h"
#include <stdlib.h>
#define BUFSIZE 100

%}

%s NUMBER

%%

[0-9]+[^@0-9] {/*It's a number not already processed: @ is processed
		number marker */
	 int i; 
	/* restore character that followed number */ 
	unput(yytext[yyleng-1]);
	/* put in end of number marker */  unput('@');
	/* push back rest of number in reverse order */
	 for(i=0;i<strlen(yytext)-1;i++)unput(yytext[i]);
		BEGIN NUMBER;}

<NUMBER>[@] {  return EON;
	/* @: special marker for end of number */}

([0-9][0-9,]*@)?[^0-9, \t\n]+[^ \t\n]* {/* not a number  */
		yylval.str = yytext;
		 return LITERAL;}

[ \t]+ { BEGIN 0;  yylval.str = yytext; return WHITESPACE;}

[\n] {BEGIN 0; return yytext[0];  /* end of line */}

<NUMBER>[0-9]    { yylval.i = atoi(yytext); return DIGIT;}

. {return yytext[0];}
%%
