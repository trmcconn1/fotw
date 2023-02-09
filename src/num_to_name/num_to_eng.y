%{
/* num_to_name.y: Yacc grammer for number-to-English filter. */
          /* ( Programming exercise, 1/94 ) */
/* Parsing assumes the number has commas, e.g., 1,102,001 */
/* The lexer (num_to_name.l) ensures this is the case */
/* This is a filter: it passes non-numbers on through, and
	changes numbers to english, e.g
 I saw 121 girls -> I saw one hundred twenty one girls

bugs:	Won't handle decimals.
	Won't handle numbers at the end of a sentence or with
		other punctuation right after them as numbers.
	Won't handle ordinals, e.g 2nd -> second
	"Only" goes up to decillions. No undecillions.
	Expansion on numbers may make lines very long

To compile: yacc -d num_to_name.y
	    lex num_to_name.l
	    cc y.tab.c lex.yy.c -o num_to_name -ly -ll

usage: num_to_name [ < input_file ] [ > outputfile ]
*/

#include <string.h>
#include <stdio.h>

#define TRUE 0
#define BUFSIZE 256

char buf[BUFSIZE];          /* number buffer */

char *prepend(const char *, char *); /* supplement to string.h */
char *name( int triad );  /* Names numbers in range 00-99 */
char localbuf[BUFSIZE]; /* used by prepend */
void clearbuf( char *first, int size ); /* does what it says */

static char *digit_names[] = { "", " one", " two", " three",
	" four", " five", " six", " seven", " eight", " nine"};

static char *teen_names[] = { " ten", " eleven", " twelve", 
	" thirteen", " fourteen", " fifteen", " sixteen", 
	" seventeen", " eighteen", " nineteen"};

static char *tens_names[] = { " twenty", " thirty", " forty",
	" fifty", " sixty", " seventy", " eighty", " ninety"};


%}

%union {int i;
	char *str;
	 }
%token COMMA
%token <str> LITERAL
%left <str> WHITESPACE
%right <i> DIGIT
%type <i> triad


%%
start: line_array { return;} 
	;

line_array: line_array  line 
	| line
	;

line: text_array '\n'   {printf("\n");}
	| '\n' {printf("\n");}
	;

text_array:  text_array  text 
	| text_array  number  {clearbuf(buf,sizeof(buf));/* prepare for
					next  number */}
	| text_array filler
	| text
	| number {clearbuf(buf,sizeof(buf));}
	| filler
	;

filler: WHITESPACE { printf("%s",$1);/* pass through whitespace */}
	;

text:   LITERAL {printf("%s",$1);/* pass through non-numbers */}
	;

number:  hundreds  {buf[ (int) strlen(buf)-1 ] = '\0'; /* kill last ,*/
			 printf("%s",buf+1);/* eat up leading space*/}
	| thousands {buf[ (int) strlen(buf)-1 ] = '\0'; printf("%s",buf+1);}  
	| millions   {buf[ (int) strlen(buf)-1 ] = '\0'; printf("%s",buf+1);}
	| billions  {buf[ (int) strlen(buf)-1 ] = '\0';printf("%s",buf+1);}
	| trillions {buf[ (int) strlen(buf)-1 ] = '\0';printf("%s",buf+1);}
	| quadrillions  {buf[ (int) strlen(buf)-1 ] = '\0';printf("%s",buf+1);}
	| quintillions {buf[ (int) strlen(buf)-1 ] = '\0';printf("%s",buf+1);}
	| sextillions {buf[ (int) strlen(buf)-1 ] = '\0';printf("%s",buf+1);}
	| septillions {buf[ (int) strlen(buf)-1 ] = '\0';printf("%s",buf+1);}
	| octillions {buf[ (int) strlen(buf)-1 ] = '\0';printf("%s",buf+1);}
	| decillions {buf[ (int) strlen(buf)-1 ] = '\0';printf("%s",buf+1);}
	;

decillions: triad COMMA nonillions { prepend( " decillion,", buf);
			    prepend( name($1), buf); }
	;
nonillions: triad COMMA octillions {  if( strlen(name($1)) > 0)/*not 000*/
			prepend( " nonillion,", buf);
			    prepend( name($1), buf); }
	;
octillions: triad COMMA septillions {  if( strlen(name($1)) > 0)
			prepend( " octillion,", buf);
			    prepend( name($1), buf); }
	;
septillions: triad COMMA sextillions {  if( strlen(name($1)) > 0)
			prepend( " septillion,", buf);
			    prepend( name($1), buf); }
	;
sextillions: triad COMMA quintillions {  if( strlen(name($1)) > 0)
			prepend( " sextillion,", buf);
			    prepend( name($1), buf); }
	;
quintillions: triad COMMA quadrillions {  if( strlen(name($1)) > 0)
			prepend( " quintillion,", buf);
			    prepend( name($1), buf); }
	;
quadrillions: triad COMMA trillions {  if( strlen(name($1)) > 0)
			prepend( " quadrillion,", buf);
			    prepend( name($1), buf); }
	;

trillions: triad COMMA billions {  if( strlen(name($1)) > 0)
			prepend( " trillion,", buf);
			    prepend( name($1), buf); }
	;

billions: triad COMMA millions {  if( strlen(name($1)) > 0)
			prepend( " billion,", buf);
			    prepend( name($1), buf); }
	;

millions: triad COMMA thousands {  if( strlen(name($1)) > 0)
			prepend( " million,", buf);
			    prepend( name($1), buf); }
	;

thousands: triad COMMA hundreds {  if( strlen(name($1)) > 0)
				prepend( " thousand,", buf);
			    prepend( name($1), buf); }
	;

hundreds: triad { if( strlen(name($1)) > 0)
	sprintf( buf, "%s,", name($1)); }
	;

triad: DIGIT DIGIT DIGIT { $$ = 100*$1 + 10*$2 + $3;}
	| DIGIT DIGIT {$$ = 10*$1 + $2;}
	| DIGIT {$$ = $1;}
	;
%%


char *prepend(const char *first, char *second)

/* Stick first on front of second, return second */
{
	char temp[BUFSIZE];

	strcpy(temp,first);  /* store first in temp */
	strcat(temp,second); /* tack on second */
	strcpy(second,temp);
	return second;
}

char *name( int triad)
/* returns pointer to the English phrase for a number from 001 - 999 */
{
	int ones,tens,hundreds; /* digits */

	clearbuf(localbuf,sizeof(localbuf));
	
	hundreds = triad/100;
	tens = (triad - hundreds*100)/10;
	ones = (triad - hundreds*100 - tens*10);

	if( tens == 0) sprintf(localbuf,"%s",digit_names[ones]);
	else 
	{    if( tens == 1)
		sprintf(localbuf,"%s",teen_names[ones]);
		else {
			sprintf(localbuf,"%s",digit_names[ones]);
			prepend(tens_names[tens-2],localbuf);
		     }  /* starts with "twenty" ^  */
	}
	if(hundreds !=0) {
		prepend(" hundred", localbuf);
		prepend(digit_names[hundreds],localbuf);
	}
	return localbuf;
}

void clearbuf( char *first, int size )
{
	int i;
	for(i=0;i<size;i++) *(first + i) = '\0';
	return ;
}
