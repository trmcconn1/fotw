%{
/* fgn.y: generate a lex specification for a filter
	using a command syntax similar to sed 

	Version 1.1   Febuary 1995  Chicken Haven Software, Inc
		by Terry McConnell

This is a preprocessor for lex. It writes its outfut in a file named
ff.out.l in the current directory. Essentially, this program offers
extensions to lex, setting up standard Unix option processing and allowing
parts of the matched patterns to be manipulated in the action code.

usage: fgn inputfile.

inputfile contains literal blocks of c-code, comments and commands. 

In the literal block the user can (and should) #define a string called
INFO which will be displayed whenever the filter is invoked with the
option -help. (There is a default info string which is not helpful.)

A command  has the form:
[+-]option  /regexp/  { c-code }

Where + introduces an option for which the following filter command line argument
is to be used as an argument. It is available in the variable clvar. Only
one option should take an argument, and it must be given last on the
filter command line. 

one command must describe the default action of the filter in case
no options are given on the command line. It has the same format, but
the name of the option is _ (underscore)


The same option may occur on multiple lines, but all lines with
the same option must occur together
The regexp is optional. The regexp may contain sections of the form
\(stuff\). What is matched by the first such is substituted for the
expression \$0 in the c-code section, etc. The rest of regexp is echoed.

Whenever possible, the user should eschew the use of such variables, and
especially when only one is needed. In that case use yytext instead. 

A single command-line argument may be passed to the generated filter after
the end of options. It is available in the string char *clvar.

Usage of the filter generated:

name [options] [option arg] [input file]

If no input file is given, the filter reads from stdin.   

All options are munged before any processing, so if multiple options are
allowed the command file should include commands for all possible orderings of
munged options.

For example, the command line

filter -f -y arg file

gets converted to 

filter -fy arg file

Here arg is an argument taken by the -y option, and filter will read from
file. The command file must contain lines of the form +fy. 

Multiple file names can be given on the command. If - is used as a 
filename it indicates stdin. This may occur anywhere in a file list. 

You may use -- to indicate the end of options ( useful for input files
whose names begin with -.) It also should be used when there are no
options, but a command line argument is expected. 
Lines beginning with # are comments     */

/* 	bugs: won't handle nested variables  */
/*   And probably never will :-) */
/*   There is one shift/reduce conflict */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "fgn.h"

#define BUFSIZE 10000 /* must be big to allow for long action code */
#define MAXARGLEN 1000 /* longest possible arg pattern */
#define MAXOPTIONLEN 80 /* longest option name */
#define NVARS 10     /* number of variable tags in code */
#define MAXOPTS 20 /* maximum number of options */

int first_lex =1;    /* switch to put scanner in start state */
int command_ctr =0; /* for error messages */
int option_ctr =1; /* counts how many lines for each option */
int no_opts =0;   /* actual number of options used */
int count;

static char last_option[MAXOPTIONLEN] ="none"; 
static char options[MAXOPTS][MAXOPTIONLEN]; /* names of all options */
static char stringbuf[BUFSIZE];
char *bufptr;
static char argbuf[MAXARGLEN]; /* buffer for argument regexp */
char *argbufptr;

struct option_node {      /* built for each command line */
	char *name;  /* name of option */
	char *pattern; /* the full regexp for this line */
	int vars; /* number of \( \)'s on this line */
	char *varpats[NVARS]; /* what's in each \(\) */
	char *action_code; /* C-code for this line */
	} option_node;

extern FILE *yyin;
FILE   *ffout, *temp, *last;  /* pointers to output and temporary file */
           /* and to file holding anything beyond the %% */
void filecopy(FILE *ifp, FILE *ofp);
void clearbuf(char *, int);
char *strcp(char *);   /* same as strdup, but returns char * */
void start_states( FILE *, struct option_node , int );
void pattern_lines(FILE *, struct option_node, int );
void make_main(FILE *, char a[][MAXOPTIONLEN], int);
void yyerror(char *);
%}
%union {
	char *str;
	char chr;
	int integer;
	}

%token ENDFILE 0
%token ENDCOMMANDS 
%token <str> OPTION
%token <integer> ARG_TAG
%token RIGHTARG LEFTARG LITERAL_BEGIN LITERAL_END 
%token <chr> CHAR
%token <str> BACKSLASHES
%token <str> SLASH
%token <str> C_ESCAPE
%token <str> BACKDOLLAR

%type <str> arg
%type <str> arg_inc

%%

sourcefile: rules string ENDFILE { printf("sourcefile parsed\n");
/* copy everything read in to ffout: */
	 fprintf(ffout,"%s",stringbuf);clearbuf(stringbuf,BUFSIZE);YYACCEPT;}
	| rules ENDFILE { YYACCEPT;}

rules: literal_block  command_list ENDCOMMANDS { printf("commands parsed\n");
/* add the default pattern line. */
		fprintf(temp,".|[\\n]\t{ECHO;}\n");
/* add %% */
		fprintf(temp,"%%%%\n"); 
/* Make sure there is a default option line  */
/* Else warn */
	for(count=0;count <= no_opts-1;count++)
		if(strcmp(&options[count][0]+1,"_") == 0)goto checks;
			yyerror("No default option line\n");
			checks: ;

/* Write the main program and accompanying declarations */
	make_main(temp,options,no_opts);
/* Append temp to the end of ffout */

	fclose(ffout);
	ffout = fopen("ff.out.l","a");
	rewind(temp);
	filecopy(temp,ffout);
/* prepare for additional input after command block */
	clearbuf(stringbuf,BUFSIZE);
	bufptr = stringbuf;
	}

	| command_list  ENDCOMMANDS {
/* add the default pattern line. */
		fprintf(temp,".|[\\n]\t{ECHO;}\n");
/* add %% */
		fprintf(temp,"%%%%\n");
/* Make sure there is a default option line  */
/* Else warn */
	for(count=0;count <= no_opts-1;count++)
		if(strcmp(&options[count][0]+1,"_") == 0)goto checks2;
			yyerror("No default option line\n");
			checks2: ;

/* Write the main program and accompanying declarations */
	make_main(temp,options,no_opts);
/* Append temp to the end of ffout */

	fclose(ffout);
	ffout = fopen("ff.out.l","a");
	rewind(temp);
	filecopy(temp,ffout);
/* prepare for additional input after command block */
	clearbuf(stringbuf,BUFSIZE);
	bufptr = stringbuf;
	}

;


literal_block: LITERAL_BEGIN  string	LITERAL_END  {
                               /* copy to output stream ffout  */
	 fprintf(ffout,"%%{\n");
	 fprintf(ffout,"%s",stringbuf);clearbuf(stringbuf,BUFSIZE);
/* End of user literal block stuff. Put default literal block stuff
		here */
#ifndef __NEXT__
	fprintf(ffout,"#include <string.h>\n");
#endif
	fprintf(ffout,"#define NVARS 10\n");
	fprintf(ffout,"char *var[NVARS];\n");
	fprintf(ffout,"char *clvar;\n");
/* Mark the end of the literal block */
	 fprintf(ffout,"%%}\n");
		bufptr=stringbuf;}

;
command_list: /* empty */
	|	command_list command
	| 	command_list CHAR {yyerror("missing - before option");
					YYABORT;}
	| 	command_list '/' {yyerror("missing option tag");
					YYABORT;}
;


command: OPTION	 regexp  code {
	option_node.name = strcp($1);
		command_ctr++;
/* Make sure the option lines are together */
	for(count=0;count < no_opts-1;count++){
		if(strcmp(&options[count][0],option_node.name) == 0){
			yyerror("Split option lines.\n");
			YYABORT;
			}
		}
/* Increase option counter if this option has been seen before */
	if( strcmp(option_node.name, last_option) == 0)
		option_ctr++;
	else {
		option_ctr = 1;
		strcpy(&options[no_opts][0],option_node.name);
		no_opts++;
		}
	strcpy(last_option , option_node.name);
/* Make the new start states */
	start_states(ffout,option_node,option_ctr);
/* Write the action code */
	pattern_lines(temp,option_node,option_ctr);
/* Reset for next command line */
	option_node.vars = 0;
}
	| OPTION  code {
	option_node.name = strcp($1)+1;
		command_ctr++;
/* Make sure the option lines are together */
	for(count=0;count < no_opts-1;count++){
		if(strcmp(&options[count][0],option_node.name) == 0){
			yyerror("Split option lines.\n");
			YYABORT;
			}
		}
/* Increase option counter if this option has been seen before */
	if( strcmp(option_node.name, last_option) == 0)
		option_ctr++;
	else {
		option_ctr = 1;
		option_ctr = 1;
		strcpy(&options[no_opts][0],option_node.name);
		no_opts++;
	     }
	strcpy(last_option , option_node.name);
/* Make the new start states */
	start_states(ffout,option_node,option_ctr);
/* Write the action code */
	pattern_lines(temp,option_node,option_ctr);
/* Reset for next command line */
	option_node.vars = 0;
}
	| OPTION CHAR {yyerror("Missing / or extra characters before /\n");
		YYABORT;}
	| OPTION regexp CHAR {yyerror("Missing { or extra char befor {\n");
		YYABORT;}
;

regexp:	'/'	string  	'/' {option_node.pattern =
		strcp(stringbuf); /* store regexp string */
		clearbuf(stringbuf,BUFSIZE); /* reset */
		bufptr = stringbuf; }
;


code:	'{'	string  	'}' { option_node.action_code = 
		strcp(stringbuf); /* copy code */
		clearbuf(stringbuf,BUFSIZE); /* reset */
		bufptr = stringbuf; }
;


arg:	arg_inc	RIGHTARG { $$ = strcp($1); /* reset for next arg: */
				argbufptr = argbuf;
				clearbuf(argbuf,MAXARGLEN);
			/* store this arg pattern: */
				option_node.varpats[option_node.vars]=$$;
				option_node.vars++;
			}
;
arg_inc: LEFTARG { /* point at buffer */ $$ = argbuf;}
	| arg_inc CHAR { *argbufptr++ = $2; /* copy a CHAR */}
	| arg_inc BACKSLASHES { /* Literal \, ie \\ in regexp */
				strcat(argbufptr,"\\\\"); argbufptr +=2; }
	| arg_inc SLASH { /* Literal /, ie \/ in regexp */
				strcat(argbufptr,"\\/"); argbufptr +=2; }
;

string: /* empty */
	| string CHAR {*bufptr++ =$2;}
	| string arg    { /* append the arg back to regexp string */
				strcat(bufptr,$2);
				bufptr += strlen($2); }

	| string ARG_TAG { int i = $2;
				sprintf(bufptr," var[");
				bufptr +=5;
				*bufptr++ = '0' + i;
				*bufptr++ = ']';
				*bufptr++ = ' ';
				 /* only 10 args ok */
			}
	| string C_ESCAPE { /* C-escape sequence in code */
				strcat(bufptr,$2); bufptr +=strlen($2); }
	| string BACKSLASHES { /* Literal \, ie \\ in regexp */
				strcat(bufptr,"\\\\"); bufptr +=2; }
	| string SLASH { /* Literal /, ie \/ in regexp */
				strcat(bufptr,"\\/"); bufptr +=2; }
	| string BACKDOLLAR  { /* A literal \$ in code. Don't laugh, it
				_could_ happen */
				strcat(bufptr,"\\$"); bufptr +=2; }
	| string  '\n' {yyerror("Newline in string.\n");
		YYABORT;}
	| string ENDCOMMANDS {yyerror("Unexpected end of file\n");YYABORT;}
;


%%
int
main(int argc, char **argv)
{

/* Look to see if there is an input file. Else use stdin  */

	if(argc!=1){ if((yyin = fopen(argv[1],"r"))==NULL){
			printf("filgen: can't open %s\n",argv[1]);
			return 1;}
		}

/* Open stream for output. */

	if((ffout = fopen("ff.out.l", "w"))==NULL){
		printf("filgen: can't open ff.out\n");
		return 1;
		}

/* Open temporary scratch file */

	temp = tmpfile();

	setbuf(temp,NULL); /* These may need to be changed */
	setbuf(ffout,NULL);

/* Put in %% to mark beginning of pattern lines section */
	fprintf(temp,"%%%%\n");

/* initialize pointers */

	bufptr = stringbuf;
	argbufptr = argbuf;
	option_node.vars = 0;

	yyparse();

	fclose(ffout);
	fclose(yyin);

	return 0;
}

/* filecopy: copy file ifp to file ofp */
void filecopy(FILE *ifp, FILE *ofp)
{
	int c;

	while ((c=fgetc(ifp)) != EOF)
		fputc(c, ofp);
}

void clearbuf( char *first, int size)
{
	int i;
	for(i=0;i<size;i++) *(first + i) = '\0';
	return;
}

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

/* start_states: Based on the information in the option_node, i.e., 
from a single command line, set up the required set of start states. For
an option named o with 3 argument tags they are:

%s s_o 
%s o0
%s o1
%s o2

If the same option has a 2nd command line, then 
start numbering the rest from 10.

output goes to file fp 
*/

void
start_states(FILE *fp, struct option_node o, int ctr )
{
	int vs;
	vs = o.vars;
	if(ctr == 1)
		fprintf(fp,"%%s s_%s\n",o.name+1);
	while(vs-- > 0)
		fprintf(fp,"%%s %s%-d\n",o.name+1,vs+10*(ctr-1));
	return;
}

/* pattern_lines: write the pattern lines for each command line. For
the example in the start_state comment above, the pattern lines should be:
<s_o>pattern   { push back matched input and switch to state o1}
<o1>pattern1   { store matched input in var[0] and switch to o2}
<o1>.|[\n]     { ECHO anything before pattern 1 }
etc
where the command line has the form
-o /...\(pattern1\).../    { code }
The last pattern requires a different treatment:
<o2>pattern2  { store in var [2], action code, reset code }
*/

void
pattern_lines(FILE *fp, struct option_node o, int ctr )
{
	int count;
	fprintf(fp,"<s_%s>",o.name+1);
	fprintf(fp,"%s\t",o.pattern);
	if(o.vars != 0){
	fprintf(fp,"{BEGIN %s%-d; yyless(0);}\n",o.name+1,10*(ctr-1));
	for(count =0; count < o.vars -1; count++){
		fprintf(fp,"<%s%-d>",o.name+1,count+10*(ctr-1));
		fprintf(fp,"%s\t",o.varpats[count]);
		fprintf(fp,"{var[%c]=strdup(yytext);BEGIN %s%-d;}\n",'0'+
			count,o.name+1,count+10*(ctr-1)+1);
		fprintf(fp,"<%s%-d>",o.name+1,count+10*(ctr-1));
		fprintf(fp,".|[\\n]\t{ECHO;}\n");
		}
	fprintf(fp,"<%s%-d>",o.name+1,count+10*(ctr-1));
	fprintf(fp,"%s\t",o.varpats[count]);
	fprintf(fp,"{var[%c]=strdup(yytext);",'0'+
		count);
	}
	else fprintf(fp,"{");
	fprintf(fp,"%s",o.action_code);
	if(o.vars != 0)
	fprintf(fp,"BEGIN s_%s;}\n",o.name+1);
	else fprintf(fp,"}\n");
/* echo everything that doesn't match */
	if(o.vars != 0){
	fprintf(fp,"<%s%-d>",o.name+1,count+10*(ctr-1));
	fprintf(fp,".|[\\n]\t{ECHO;}\n");
	}
	else fprintf(fp,"<s_%s>.|[\\n]\t{ECHO;}\n",o.name+1);
	return;
}

/*  make_main: writes struct options, main etc. See example.l file */

void
make_main(FILE *fp, char array[][MAXOPTIONLEN], int n)
{
	int nops = n;
	/* write struct options */
	fprintf(fp,"struct options{\n");
	fprintf(fp,"char *name;\n");
	fprintf(fp,"int code;\n");
	fprintf(fp,"} options[] =\n{ ");
	while(--n >= 0)
		fprintf(fp,"\"%s\",%d,\n",&array[n][0],nops-n);
	fprintf(fp,"\"-\", 0\n};\n");
/* define a default info if the user has not defined one */
	fprintf(fp,"#ifndef INFO\n");
	fprintf(fp,"#define INFO \"Sorry, no information available\"\n");
	fprintf(fp,"#endif\n");
/* Stuff for file chaining */
	fprintf(fp,"char **fileList;\n");
	fprintf(fp," unsigned nFiles;\n");
	fprintf(fp," unsigned currentFile = 0;\n");
/* declare find_opt */
	fprintf(fp,"int find_opt(char *);\n");
	fprintf(fp,"int cmdlinearg=0;\n");
/* write main */
	fprintf(fp,"int main(int argc, char **argv)\n");
	fprintf(fp,"{\nint c;\n");
	fprintf(fp,"char *info = INFO;\n");
	fprintf(fp,"char argstr[80] = \"-\";\n");
	fprintf(fp,"if(--argc > 0){\n");
	fprintf(fp,"if((*++argv)[0]==\'-\'){\n");
	fprintf(fp,"argc--;\n");
	fprintf(fp,"if((*argv)[1]=='-'){c=find_opt(\"-_\");++argv;}\n");
	fprintf(fp,"else{ strcat(argstr,++*argv);\n");
	fprintf(fp,"while((argc>0)&&((*++argv)[0]==\'-\')&&((*argv)[1]!=\'-\')){\n");
	fprintf(fp,"argc--;\n");
	fprintf(fp,"strcat(argstr,++*argv);}\n");
	fprintf(fp,"c=find_opt(argstr);}\n");
	fprintf(fp,"switch(c){\n");
	fprintf(fp,"case 0: printf(\"%%s\\n\",info);\n");
	fprintf(fp,"return 0;\n");

	n = nops;
	while(--n>=0){
		fprintf(fp,"case %d: \n",nops-n);
		fprintf(fp,"BEGIN s_%s;\n",&array[n][0]+1);
		fprintf(fp,"break;\n");
	}
	fprintf(fp,"default:\n");
	fprintf(fp,"fprintf(stderr,\"%%s: illegal option\\n\",argstr);\n");
	fprintf(fp,"return 1;\n}\n");
	fprintf(fp,"}else BEGIN s__;}\n");
	fprintf(fp,"else BEGIN s__;\n");
	fprintf(fp,"if(cmdlinearg==1){\n");
	fprintf(fp,"if(argc>0){clvar=*argv;argc--;\n");
	fprintf(fp,"++argv;}\n");
	fprintf(fp,"else {fprintf(stderr,\"Option argument expected\\n\");\n");
	fprintf(fp,"exit(1);}}\n");
	fprintf(fp,"if(argc>0){\n");
	fprintf(fp,"argc--;\n");
	fprintf(fp,"if(strcmp(*argv,\"-\")!=0){\n");
	fprintf(fp,"if((yyin=fopen(*argv,\"r\"))==NULL){\n");
	fprintf(fp,"fprintf(stderr,\"Cannot open %%s\\n\",*argv);\n");
	fprintf(fp,"return 1;}}}\n");
	fprintf(fp,"nFiles=argc;\n");
	fprintf(fp,"fileList=argv+1;\n");
	fprintf(fp,"yylex();\n");
	fprintf(fp,"return 0;\n}\n");
/* write function find_opt */
	fprintf(fp,"int find_opt(char *word)\n{\n");
	fprintf(fp,"int count=0;\n");
	fprintf(fp,"if(strcmp(word,\"-help\")==0)return 0;\n");
	fprintf(fp,"while(strcmp(options[count].name,\"-\")!=0){\n");
	fprintf(fp,"if(strcmp(options[count].name+1,word+1)==0){\n");
	fprintf(fp,"if(options[count].name[0]==\'+\'){\n");
	fprintf(fp,"cmdlinearg=1;\n");
	fprintf(fp,"}\n");
	fprintf(fp,"return options[count].code;}\n");
	fprintf(fp,"count++;\n}\nreturn -1;\n}\n");
/* Write yywrap() */
fprintf(fp,"yywrap()\n");
fprintf(fp,"{\n");
fprintf(fp,"FILE *file;\n");
fprintf(fp,"if((nFiles>=1)&&(currentFile < nFiles))\n");
fprintf(fp,"fclose(yyin);\n");
fprintf(fp,"else{ fclose(yyin);\n");
fprintf(fp,"return 1;\n");
fprintf(fp,"}\n");
fprintf(fp,"while(fileList[currentFile]!=(char *)0){\n");
fprintf(fp,"if(strcmp(fileList[currentFile],\"-\")==0){\n");
fprintf(fp,"currentFile++;\n");
fprintf(fp,"file=stdin;\n");
fprintf(fp,"yyin=file;\n");
fprintf(fp,"break;\n");
fprintf(fp,"}\n");
fprintf(fp,"file=fopen(fileList[currentFile++],\"r\");\n");
fprintf(fp,"if(file != NULL ){\n");
fprintf(fp,"yyin=file;\n");
fprintf(fp,"break;\n");
fprintf(fp,"}\n");
fprintf(fp,"fprintf(stderr,\"Cannot open %%s \\n\",fileList[currentFile-1]);\n");
fprintf(fp,"}\n");
fprintf(fp,"return(file ? 0 : 1);\n");
fprintf(fp,"}\n");
}
