%{
/* cgi.y: yacc source for parser of a named parameter list.
*/

/* Adapted from a template for cgi-scripts written in C using the POST
method.  The Value function is provided for retrieving the
Value associated with a given name */

/* Make sure you use flex to generate the scanner, since we must be
   to match an <<EOF>> token. The scanner source is cgi.l  */

#define _POSIX_SOURCE
#include<sys/types.h>
#undef _POSIX_SOURCE

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<unistd.h>


#define MAXARGSIZE 80
#define MAXPAIRS 50

int NumberPairs = 0;
int InputLength;    /* Gotten from the evironment */
extern char* strcp(char *); /* Same as strdup */
extern int longsqrt(char *, char *);
extern int longcurt(char *, char *);
extern int yylex();
extern void yyerror(char *);
int HexToA(char *);
extern FILE *yyin;
pid_t pid;

char buffer1[256];
char buffer2[256];

struct AssocArray {     /* Really, it should be called
assocarrayelement   */
char Name[MAXARGSIZE];
char Value[MAXARGSIZE];
};

struct AssocArray Pairs[MAXPAIRS];

/* Used in sorted the associative array */
int CompareNames(struct AssocArray *, struct AssocArray *);

char *Value( char *);  /* Returns the value of a name in Assoc Array
*/
%}

%union {
	char *str;
	}

%token __AMPERSAND
%token __EQUALS
%token <str> __TEXT
%token <str> __HEX
%token __ENDFILE

%type <str> repaired_string

%%

finish: parameter_list __ENDFILE {YYACCEPT; /* YYACCEPT prevents a
syntax error at EOF due to the scanner's lookahead. Actually, in 
practice this rule should never reduce -- rather we return when
a named_pair is closed by an EOF. See below */}
;

parameter_list: /* empty */
	| parameter_list named_pair  {;}
;

named_pair: __TEXT __EQUALS repaired_string __AMPERSAND {
		strcpy(Pairs[NumberPairs].Name,
			$1); strcpy(Pairs[NumberPairs++].Value,$3);}
 	| __TEXT __EQUALS repaired_string __ENDFILE {
		strcpy(Pairs[NumberPairs].Name,
			$1); strcpy(Pairs[NumberPairs++].Value,$3);
			YYACCEPT;	}
	| __TEXT __EQUALS __TEXT __AMPERSAND {
		strcpy(Pairs[NumberPairs].Name,
			$1); strcpy(Pairs[NumberPairs++].Value,$3);
			}
	| __TEXT __EQUALS __TEXT __ENDFILE {
		strcpy(Pairs[NumberPairs].Name,
			$1); strcpy(Pairs[NumberPairs++].Value,$3);
			YYACCEPT;}
;
repaired_string: __TEXT  __HEX { 

/* The server sends any non-alpanumeric to us in the form %AB, where
AB is the hex ascii code. We need to substitute the actual characters
for the the codes, thus "repairing" the string. Potential Bug: we do
this for Values only, not names */

				sprintf(buffer1,"%s",$1);
				sprintf(buffer2,"%c",HexToA($2));
				strcat(buffer1,buffer2);
				$$ = strcp(buffer1);}
	     | repaired_string __TEXT  { strcpy(buffer1,$1);
				strcat(buffer1,$2);
				$$ = strcp(buffer1); }
	     | repaired_string  __HEX {
				sprintf(buffer1,"%s",$1);
				sprintf(buffer2,"%c",HexToA($2));
				strcat(buffer1,buffer2);
				$$ = strcp(buffer1);}
				
;


%%
int
main()
{
	char buffer[256];
	FILE *tempfile;
	char *ptr;

/* With POST method the Content may not end with an EOF. Thus we
   get the length of the content from the evironment, and compare it
   with a character count maintained by the scanner whenever a named
   pair is reduced. */

/* Actually, rather than muck around counting characters, I'm just
going to write what the server sends us to a temp file and then
redirect the parser's input from there */

       printf("Content-type: text/plain\n\n"); /* The browser will
          expect this header -- change as needed */

	pid = getpid();
	if((ptr = getenv("CONTENT_LENGTH")) != NULL){

/* OK, I guess we're getting our input from the server -- otherwise
   we must be running in debug mode */

		InputLength = atoi(ptr);
		if((tempfile = tmpfile())==NULL){
			printf("NPLPARSE:Unable to open tempfile\n");
			return 1;
		}
		setvbuf(tempfile,(char *)NULL,_IOFBF,BUFSIZ);

		fgets(buffer,InputLength+1,stdin);
		fputs(buffer,tempfile);
		rewind(tempfile);
		yyin = tempfile;
		if(!(ptr = getenv("REQUEST_METHOD"))){
			printf("No REQUEST_METHOD from client\n");
			return 1;
		}
		if(strcmp(ptr,"POST")){
			printf("Only POST method supported.\n");
			return 1;
		}
	}
	else{
			printf("No CONTENT_LENGTH from client.\n");
			return 1;
	}

	yyparse();
	fclose(yyin); 

/* Sort the Pairs array alphabetically on the Name field */
	qsort(Pairs,NumberPairs,sizeof(struct AssocArray),
		(int (*)(const void *, const void *)) &CompareNames);

	/* Head off a few potential show stoppers */

	if(Value("ROOT")==NULL){
		printf("Client did not specify which type of root\n");
		return 1;
	}
	if(Value("RADICAND")==NULL){
		printf("Client did not specify a radicand\n");
		return 1;
	}
	if(Value("DIGITS")==NULL){
		printf("Client did not specify number of digits\n");
		return 1;
	}

	stderr = stdout; /* The root taking routines print their error
                            messages to stderr, but some web servers ignore
			    this */
 
	if(strcmp(Value("ROOT"),"2")==0)
		longsqrt(Value("RADICAND"),Value("DIGITS"));
	else if(strcmp(Value("ROOT"),"3")==0)
		longcurt(Value("RADICAND"),Value("DIGITS"));
	else printf("Unknown client value, %s, for ROOT type", Value("ROOT"));

	return 0;
}

/* HexToA:  Takes any sequence of the form %AB,
where AB are hexadecimal digits and returns the corresponding ascii character.
Returns 21 (!) on invalid input.
*/

int HexToA(char *input)
{
	char buffer[4];
	char a,b;

	strcpy(buffer,input);
	if(buffer[0] != '%')return 0x21;
	if(buffer[3] != '\0')return 0x21;
	a = isdigit(buffer[1]) ? buffer[1] - '0' : buffer[1]-'A'+10; 
	b = isdigit(buffer[2]) ? buffer[2] - '0' : buffer[2]-'A'+10; 
	return a*16+b;
}	


/* Return -1, 0, 1 according as A->Name is < = or > B->Name in 
alphabetic order */

int CompareNames(struct AssocArray *A, struct AssocArray *B)
{
	return strcmp(A->Name,B->Name);
}


/* Value: search the associative array for a given name and return a
pointer to the corresponding value string */

char *Value( char *Name)
{ 	
	struct AssocArray *ptr;
	struct AssocArray key;

	strcpy(key.Name,Name);
	strcpy(key.Value, "foobar");  /* doesn't get used */

	if((ptr = bsearch(&key,Pairs,NumberPairs, sizeof(struct
		AssocArray),
		(int (*)(const void *, const void *)) &CompareNames))==NULL)
		return (char *)NULL;
	return ptr->Value;
}

