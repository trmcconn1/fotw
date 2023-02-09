%{
/* num_to_name.y: Yacc grammer for number-to-English filter. */
          /* ( Programming exercise, 1/94 ) */
/*
 
	usage: num_to_name [-k] [< input] [> output]

	options: k: use Knuth's nomenclature instead
			of standard English.
Default:  (English)
*/
/* Parsing assumes the number has commas, e.g., 1,102,001 */
/* The lexer (num_to_name.l) ensures this is the case */
/* This is a filter: it passes non-numbers on through, and
	changes numbers to english, e.g
 I saw 121 girls -> I saw one hundred twenty one girls

/* With the -k option:
*/
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
	"Only" goes up to decillions. No undecillions.
	In -k option:
	There are reduce/reduce conflicts. Is there a way to do this
	without them? The default shift resolution works here because
	of the ordering of rules.

*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define TRUE 0
#define TRUE 0
#define FALSE 1
#define BUFSIZE 1000
#define DENOMINATIONS 10 /*  tens, hundreds, myriads, ... */

char names[DENOMINATIONS * DENOMINATIONS][BUFSIZE];
/* Used to store names of each denomination. For example, with
12344321, "forty three hundred twenty one" is the name of the
hundreds denomination, and "twelve hundred thirty four" is the name
of the myriads denomination. These must be stored in separate
locations because names of lower denominations get used to build names
of higher ones */
int indx = 0;
char *name( int triad );  /* Names numbers in range 00-999 */
char *kname( int diad );  /* Names numbers in range 00-99 */
/* used with -k option */
void clearbuf( char *first, int size ); /* does what it says */
void wipe(void); /* reinitialize the big names buffer */
int start_state = 1; /* default. 1 means English, 2 means Knuthian */
char buf[BUFSIZE];          /* number buffer */
char *prepend(const char *, char *); /* supplement to string.h */
char localbuf[BUFSIZE]; /* used by prepend */

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
%right <i> EDIGIT
%type <i> triad
%token <str> EON
%type <str> tens
%type <str> tens_inc
%type <str> hundreds
%type <str> ehundreds
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
 	| line_array  line 
	;

line: text_array '\n'   {printf("\n");}
	| '\n' {printf("\n");}
	;

text_array:  text_array  text 
	| text_array  englishnumber  {clearbuf(buf,sizeof(buf));/* prepare for
					next  number */}
	| text_array knuthnumber
	| text_array filler
	| text
	| englishnumber {clearbuf(buf,sizeof(buf));}
	| knuthnumber
	| filler
	;

filler: WHITESPACE { printf("%s",$1);/* pass through whitespace */}
	;

text:   LITERAL {printf("%s",$1);/* pass through non-numbers */}
	;

englishnumber:  ehundreds  {buf[ (int) strlen(buf)-1 ] = '\0'; /* kill last ,*/
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

thousands: triad COMMA ehundreds {  if( strlen(name($1)) > 0)
				prepend( " thousand,", buf);
			    prepend( name($1), buf); }
	;

ehundreds: triad { if( strlen(name($1)) > 0)
	sprintf( buf, "%s,", name($1)); }
	;

triad: EDIGIT EDIGIT EDIGIT { $$ = 100*$1 + 10*$2 + $3;}
	| EDIGIT EDIGIT {$$ = 10*$1 + $2;}
	| EDIGIT {$$ = $1;}
	;

knuthnumber: quadryllions EON  {printf("%s",$1);wipe();}
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
		 strcpy(ptr, kname(10*$2 + $1)); $$ = ptr;indx++;}
;
tens_inc: DIGIT EON {char *ptr = &names[indx][0];clearbuf(ptr,BUFSIZE);
		 strcpy(ptr, digit_names[$1]); $$ = ptr;indx++;}
;

%%

int main(int argc, char *argv[]) 
{
	char c;
	while (--argc > 0 && (*++argv)[0] == '-')
		while ( c = *++argv[0])
		switch (c) {
		case 'k':
			 start_state = 2;       /* handle option(s) */
			break;
		default:
			printf("num_to_name: illegal option %c\n", c);
			return 1;
		}
			
	yyparse();
	return 0;
}


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

char *kname( int diad)
/* returns pointer to the English phrase for a number from 01 - 99 */
{
	int ones,tens; /* digits */
		clearbuf(localbuf,BUFSIZE);
	
	tens = diad/10;
	ones = (diad - tens*10);

	if( tens == 0) sprintf(localbuf,"%s",digit_names[ones]);
	else 
	{    if( tens == 1)
		sprintf(localbuf,"%s",teen_names[ones]);
		else {
			sprintf(localbuf,"%s",
			tens_names[tens-2]);
			strcat(localbuf,digit_names[ones]);
		     	}
	}
	return localbuf;
}

/* wipe: clear the big names buffer */
void wipe(void)
{	
	int count;

	for(count=0;count< DENOMINATIONS * DENOMINATIONS; count++)
		clearbuf(names[count],BUFSIZE);
	return;
}


