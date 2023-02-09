%{
/* num_to_name.l: lexer for num_to_name.y  */
 
#include "y.tab.h"
#include <stdlib.h>
#define BUFSIZE 100

int commas_in( char *input);
char numbuf[BUFSIZE]; /* number with commas buffer */

%}

%s ENGLISH
%s KNUTH
%s ENGLISHNUMBER
%s KNUTHNUMBER

%%
%{
/* Code to pick the appropriate lexer */ 
	static int first_lex = 1;
	extern int start_state;

/* first_lex = 1 for English nomenclature, 2 for Knuthian */

	if(first_lex){
		if( start_state == 1 )
			BEGIN ENGLISH;
		else
			BEGIN KNUTH;
		first_lex = 0;  
	}
	

%}


<KNUTH>[0-9]+[^@0-9] {/*It's a number not already processed: @ is processed
		number marker */
	 int i; 
	/* restore character that followed number */ 
	unput(yytext[yyleng-1]);
	/* put in end of number marker */  unput('@');
	/* push back rest of number in reverse order */
	 for(i=0;i<strlen(yytext)-1;i++)unput(yytext[i]);
		BEGIN KNUTHNUMBER;}

<KNUTHNUMBER>[@] {  return EON;
	/* @: special marker for end of number */}

<KNUTH>([0-9][0-9,]*@)?[^0-9, \t\n]+[^ \t\n]* {/* not a number  */
		yylval.str = yytext;
		 return LITERAL;}

<KNUTH>[ \t]+ {  yylval.str = yytext; return WHITESPACE;}
<KNUTHNUMBER>[ \t]+ { BEGIN KNUTH;  yylval.str = yytext; return WHITESPACE;}

<KNUTH>[\n] { return yytext[0];  /* end of line */}
<KNUTHNUMBER>[\n] {BEGIN KNUTH; return yytext[0];  /* end of line */}


<KNUTHNUMBER>[0-9]    { yylval.i = atoi(yytext); return DIGIT;}

<ENGLISH>[0-9][0-9][0-9][0-9]+ {/*It's a number without commas: put 'em in */
	 int len; len = commas_in(yytext); 
	/* and push back onto input stream */
	 for(;len>0;len--)unput(numbuf[len-1]); BEGIN ENGLISHNUMBER;}

<ENGLISH>([0-9][0-9,]*)?[^0-9, \t\n]+[^ \t\n]* {/* not a number  */
		 yylval.str
		= yytext; return LITERAL;}

<ENGLISH>[ \t]+ {yylval.str = yytext; return WHITESPACE;}
<ENGLISHNUMBER>[ \t]+ {yylval.str = yytext; BEGIN ENGLISH; return WHITESPACE;}

<ENGLISH>[\n] {return yytext[0];  /* end of line */}
<ENGLISHNUMBER>[\n] {BEGIN ENGLISH; return yytext[0];  /* end of line */}

<ENGLISHNUMBER>[0-9]    { yylval.i = atoi(yytext);return EDIGIT;}

<ENGLISH>[0-9]    {/* It must be a number. Begin processing as such */
	 unput(yytext[0]); BEGIN ENGLISHNUMBER;} 


<ENGLISHNUMBER>[,] {return COMMA;}

<ENGLISH>"$" {return 0;}

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

void yyerror(char *msg)
{
	fprintf(stderr,"num_to_name: %s\n",msg);
}
