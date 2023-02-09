%{
/* nplparse.l: scanner for nplparse.y */

#include "cgi.tab.h"
#include<stdlib.h> 
#include<string.h>

char *strcp(char *);

%}


%%

[&] { return __AMPERSAND; }

[ \t\n] { /* ignore whitespace -- there won't be any */ ;}

[%][0-9A-F][0-9A-F] { yylval.str = strcp(yytext);return __HEX;}

[=] { return __EQUALS; }

[^%=& 	\n]+ { yylval.str = strcp(yytext);
		return __TEXT;}

<<EOF>> {return __ENDFILE;}

%%
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

/* Bison requires that this be implemented */

void yyerror(char *msg)
{
	fprintf(stderr,"PROGRAM_NAME: %s\n",msg);
	fprintf(stderr,"near text: %s.\n",yytext);
}
