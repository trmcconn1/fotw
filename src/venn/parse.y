%{
	/* Parse a simple 3 set algebra for use with Venn diagram demo
           program */


#include "venn.h"
	
%}

%left OR
%left AND
%nonassoc NOT_PREFIX
%nonassoc NOT_POSTFIX
%token A
%token B
%token C
%token S
%token EMPTY
%token ENDFILE

%%



start: expression ENDFILE { return $1; }


expression: set
	| expression AND expression  { $$ = $1 & $3; }
	| expression OR expression   { $$ = $1 | $3; }
	| '(' expression ')' { $$ = $2;}
	| NOT_PREFIX expression { $$ = 0xFF - $2;}
	| expression NOT_POSTFIX { $$ = 0xFF - $1;}

set: A { $$ = SET_A;}
	| B  { $$ = SET_B; }
	| C  { $$ = SET_C; }
	| S  { $$ = SET_S; }
	| EMPTY { $$ = SET_EMPTY;}

%%

char myinput[BUFSIZ];
char *myinputptr;  /* current position in myinput */
int myinputlim; /* end of data */

int
main( int argc, char **argv)
{
	int result;
	char bindigits[8];
	int i;

	if(argc < 2) {
		fprintf(stderr,"Usage: venn <set algebra expression>\n");	
		return 1;	
	}
	strcpy(myinput,*++argv);
	myinputlim = strlen(myinput);
	myinputptr = myinput;
	result = yyparse();
	printf("The set has value: ");
	i = 0;
	if(result == 0) printf("00000000\n");
	else{
	  while(result){
		if(result % 2)bindigits[i++]='1';
		else bindigits[i++]='0';
		result = result/2;
	  }
	  for(i=7;i>=0;i--)printf("%c",bindigits[i]);
	  printf("\n");
	}
	return 0;
}
