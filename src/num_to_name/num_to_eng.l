%{
/* num_to_name.l: lexer for num_to_name.y  */

#include "y.tab.h"
#include <stdlib.h>
#define BUFSIZE 100

int commas_in( char *input);
char numbuf[BUFSIZE]; /* number with commas buffer */

%}

%s NUMBER

%%

[0-9][0-9][0-9][0-9]+ {/*It's a number without commas: put 'em in */
	 int len; len = commas_in(yytext); 
	/* and push back onto input stream */
	 for(;len>0;len--)unput(numbuf[len-1]); BEGIN NUMBER;}

([0-9][0-9,]*)?[^0-9, \t\n]+[^ \t\n]* {/* not a number  */
		 yylval.str
		= yytext; return LITERAL;}

[ \t]+ {yylval.str = yytext; BEGIN 0; return WHITESPACE;}

[\n] {BEGIN 0; return yytext[0];  /* end of line */}

<NUMBER>[0-9]    { yylval.i = atoi(yytext);return DIGIT;}

[0-9]    {/* It must be a number. Begin processing as such */
	 unput(yytext[0]); BEGIN NUMBER;} 


<NUMBER>[,] {return COMMA;}

"$" {return 0;}

. {return yytext[0];}
%%

/* commas_in: group digits in groups of 3 and punctuate with commas */
/* return string length of resulting number */

int commas_in( char *input)
{
	int len, ntriples, i;
	char *ptr;
	int count;

	ptr = input;
	len = strlen(input);
	ntriples = len/3;

/* initialize buffer */
	for(count=0;count<BUFSIZE;count++)numbuf[count]='\0';

/* handle odd digits at beginning, if necessary */

	if( len % 3 > 0 ) {
		strncpy(numbuf,input,len % 3);
		/* Put in a comma */
		strcat(numbuf,",");
	        ptr = ptr + ( len % 3) ;
		}

/* Put commas after each group of 3 digits */

	if (len > 3) {
		 for(i=1;i<ntriples;i++) {
			 strncat(numbuf,ptr,3);
			 ptr = ptr + 3;
			 strcat(numbuf,",");
			}

/* except the last group of 3 */

		  strncat(numbuf,ptr,3);
		 }
		
	return strlen(numbuf);
}	
