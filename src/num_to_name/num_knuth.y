%{
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


%}

%union {
	int i;
	char *str;
	}

%token  <str> LITERAL
%token <str> EON
%left <str> WHITESPACE
%left <i>  DIGIT
%type <str> tens
%type <str> tens_inc
%type <str> hundreds
%type <str> hundreds_inc
%type <str> myriads
%type <str> myriads_inc
%type <str> myllions
%type <str> myllions_inc
%type <str> byllions
%type <str> byllions_inc
%type <str> tryllions
%type <str> tryllions_inc
%type <str> quadryllions
%type <str> quadryllions_inc

%%

line_array: /* empty */
	|  line_array  line 
	;

line: text_array '\n'   {printf("\n");}
	| '\n' {printf("\n");}
	;

text_array:  text_array  text 
	| text_array  number 
	| text_array filler
	| text
	| number 
	| filler
	;

filler: WHITESPACE { printf("%s",$1);/* pass through whitespace */}
	;

text:   LITERAL {printf("%s",$1);/* pass through non-numbers */}
	;

number: quadryllions EON  {printf("%s",$1);wipe();}
	| quadryllions_inc  {printf("%s",$1);wipe();}
	| tryllions_inc  {printf("%s",$1);wipe();}
	| byllions_inc  {printf("%s",$1);wipe();}
	| myllions_inc  {printf("%s",$1);wipe();}
	| myriads_inc  {printf("%s",$1);wipe();}
	| hundreds_inc  {printf("%s",$1);wipe();}
	| tens_inc   {printf("%s",$1);wipe();}
;



quadryllions: tryllions tryllions { char *ptr = &names[indx][0]; clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,$2);
		if(*ptr != '\0')
		strcat(ptr, " quadryllion");
	        $$ = strcat(ptr,$1);
		indx++;
		}

quadryllions_inc:  tryllions tryllions_inc { char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,$2);
		strcat(ptr, " quadryllion");
	        $$ = strcat(ptr,$1);
		indx++;
		}
	|  tryllions EON { char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
	        $$ = strcpy(ptr,$1);
		indx++;}

	|  tryllions_inc       {/* produces conflict */ }
;
tryllions: byllions byllions { char *ptr = &names[indx][0]; clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,$2);
		if(*ptr != '\0')
		strcat(ptr, " tryllion");
	        $$ = strcat(ptr,$1);
		indx++;
		}

tryllions_inc:  byllions byllions_inc { char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,$2);
		strcat(ptr, " tryllion");
	        $$ = strcat(ptr,$1);
		indx++;
		}
	|  byllions EON { char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
	        $$ = strcpy(ptr,$1);
		indx++;}

	|  byllions_inc       {/* produces conflict */ }
;

byllions: myllions myllions { char *ptr = &names[indx][0]; clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,$2);
		if(*ptr != '\0')
		strcat(ptr, " byllion");
	        $$ = strcat(ptr,$1);
		indx++;
		}

byllions_inc:  myllions myllions_inc { char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,$2);
		strcat(ptr, " byllion");
	        $$ = strcat(ptr,$1);
		indx++;
		}
	|  myllions EON { char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
	        $$ = strcpy(ptr,$1);
		indx++;}

	|  myllions_inc       {/* produces conflict */ }
;

myllions: myriads myriads { char *ptr = &names[indx][0]; clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,$2);
		if(*ptr != '\0')
		strcat(ptr, " myllion");
	        $$ = strcat(ptr,$1);
		indx++;
		}

myllions_inc:  myriads myriads_inc { char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,$2);
		strcat(ptr, " myllion");
	        $$ = strcat(ptr,$1);
		indx++;
		}
	|  myriads EON { char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
	        $$ = strcpy(ptr,$1);
		indx++;}

	|  myriads_inc       {/* produces conflict */ }
;

myriads: hundreds hundreds { char *ptr = &names[indx][0]; clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,$2);
		if(*ptr != '\0')
		strcat(ptr, " myriad");
	        $$ = strcat(ptr,$1);
		indx++;
		}

myriads_inc:  hundreds hundreds_inc { char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,$2);
		strcat(ptr, " myriad");
	        $$ = strcat(ptr,$1);
		indx++;
		}
	|  hundreds EON { char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
	        $$ = strcpy(ptr,$1);
		indx++;}

	|  hundreds_inc       {/* produces conflict */ }
;

hundreds: tens tens { char *ptr = &names[indx][0]; clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,$2);
		if(*ptr != '\0')
		strcat(ptr, " hundred");
	        $$ = strcat(ptr,$1);
		indx++;
		}

hundreds_inc:  tens tens_inc { char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
		strcpy(ptr,$2);
		strcat(ptr, " hundred");
	        $$ = strcat(ptr,$1);
		indx++;
		}
	|  tens EON { char *ptr = &names[indx][0];
		 clearbuf(ptr,BUFSIZE); 
	        $$ = strcpy(ptr,$1);
		indx++;}

	| tens_inc  { /* produces conflict */ }  
;

tens: DIGIT DIGIT {char *ptr = &names[indx][0];clearbuf(ptr,BUFSIZE);
		 strcpy(ptr, name(10*$2 + $1)); $$ = ptr;indx++;}
;
tens_inc: DIGIT EON {char *ptr = &names[indx][0];clearbuf(ptr,BUFSIZE);
		 strcpy(ptr, digit_names[$1]); $$ = ptr;indx++;}
;
%%

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

	
