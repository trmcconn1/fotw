%{
 /* Scanner for a simple set-algebra grammer to be used in a venn
    diagram demo program */

/* This file should be processed by flex. The following is needed in
   order to read data from the command line rather than from stdin. See
   p157 of the lex/yacc book */

#include "y.tab.h"
#undef YY_INPUT
#define YY_INPUT(b,r,ms) (r=my_yyinput(b,ms))

%}


%%
[ \t]+ {; /* ignore whitespace */ }
[&]+|[Aa][Nn][Dd]|[Ii][Nn][Tt][eE][rR][cC][Tt]([iI][Oo][Nn])? { return AND ;}
[|]+|[Oo][Rr]|[Uu][Nn][iI][Oo][nN] { return OR;}
[Aa] { return A ;}
[Bb] { return B ;}
[Cc] { return C; }
[Ss] { return S; }
[Nn][Uu][Ll][Ll]([Ss][Ee][Tt])?|[Ee][Mm][Pp][Tt][Yy]([Ss][Ee][Tt])?|0 {return EMPTY;}
!|[Nn][oO][Tt] { return NOT_PREFIX; }
' { return NOT_POSTFIX; }
.    { return yytext[0]; }
<<EOF>> { return ENDFILE;}

%%

/* These are defined in the main program */
extern char myinput[];
extern char *myinputptr;  /* current position in myinput */
extern int myinputlim; /* end of data */

int
my_yyinput(char *buf, int max_size)
{
	int n = ( max_size < myinputlim - (myinputptr - myinput) ?
			max_size : myinputlim - (myinputptr - myinput));

	if(n == 0) return 0;  /* EOF */
	memcpy(buf,myinputptr,n);
	myinputptr += n;
	return n;
}
