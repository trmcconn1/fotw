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

[%][0-9A-F][0-9A-F] { cgilval.str = strcp(yytext);return __HEX;}

[=] { return __EQUALS; }

[^%=& 	\n]+ { int i; char c; 
		cgilval.str = strcp(yytext);
		/* replace any + signs in text with space */
		for(i=0;i<strlen(yytext);i++)
			if((cgilval.str)[i]=='+')
				(cgilval.str)[i]=' ';	
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

/* Bison requires that these be implemented */

void cgierror(char *msg)
{
	fprintf(stderr,"PROGRAM_NAME: %s\n",msg);
	fprintf(stderr,"near text: %s.\n",yytext);
}

int cgiwrap()
{
	return 1;
}
