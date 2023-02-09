%{

/* Scanner for tableau program */

#include "global.h"
#include "parse.h"


static int escape_on = 0;
static int eof_flag = 0;
static int firstcall = 1;
extern char input_filename[];
int line = 0;
char current_token[MAXTOKEN];
int i;
void reset(void);


%}

%%

 { 

	if(eof_flag)return END_OF_INPUT;

	if(firstcall) { /* Do this when yylex is first called */
		firstcall = 0;
	}
 }
   


#  { if(escape_on){
		strcat(current_token,yytext);
		escape_on = 0;
	}
	else {  /* throw the rest of the line on the floor */
		char c;
		while(1){
			c = input();
			if(c == '\n') { 
				line++;
				break;
			}
		}
	}
    }

^[ \t]*\n { 
			line++; /* skip blank lines */ 
	}

[\n] {

	line++;
	if(escape_on){
		escape_on = 0;
		if(strlen(current_token) > 0)
			strcat(current_token,yytext);
	}
	else return NEWLINE;

}

[ \t]+ { /* terminating whitespace */ 

	if(escape_on){ 
		strcat(current_token,yytext);
		escape_on = 0;
	  } 
	if(strlen(current_token)>0) return VARIABLE;

      } 

\\ { if(escape_on){
		strcat(current_token,yytext);
		escape_on = 0;
	}
	else { 
		escape_on = 1;
	}
     }

=[fFtT] {if(escape_on){

		strcat(current_token,yytext);
		escape_on = 0;
	}
else
	{
			if(strlen(current_token)>0) {
				unput(yytext[0]);
				unput(yytext[1]);
				return VARIABLE;
			}
			if((yytext[1]=='T')||(yytext[1]=='t'))
				return TRUE_SIGN;
			if((yytext[1]=='F')||(yytext[1]=='f'))
				return FALSE_SIGN;
	}
}

[(] { if(escape_on){
		strcat(current_token,yytext);
		escape_on = 0;
	}
	else 
	{ 
		if(strlen(current_token)>0) {
			unput(yytext[0]);
			return VARIABLE;
		}
		strcpy(current_token,"("); return LEFT_DELIMITER;} }

[)] { if(escape_on) {
		strcat(current_token,yytext);
		escape_on = 0;
	}
	else
	{
		if(strlen(current_token)>0) {
			unput(yytext[0]);
			return VARIABLE;
		}
	 strcpy(current_token,")"); return RIGHT_DELIMITER; } }

[~!-] { if(escape_on) {
		strcat(current_token,yytext);
		escape_on = 0;
	}
	else {
		if(strlen(current_token)>0) {
			unput(yytext[0]);
			return VARIABLE;
		}
	strcpy(current_token,"~"); return NOT;} }

[\^&*.] { if(escape_on) {
	 	strcat(current_token,yytext);
		escape_on = 0;
	}
	else {
		if(strlen(current_token)>0) {
			unput(yytext[0]);
			return VARIABLE;
		}
	strcpy(current_token,"^"); return AND;
	}
 }

[|+] { if(escape_on) {
		strcat(current_token,yytext);
		escape_on = 0;
	}
	else {
		if(strlen(current_token)>0) {
			unput(yytext[0]);
			return VARIABLE;
		}
	 strcpy(current_token,"|"); return OR;} }

[^()!-&=*.+|^~# \t\n]+ { 

	  int j;

               /* build name or operation name */

          strcat(current_token,yytext);
		
	  if(yytext[j = (strlen(yytext)-1)]=='\\'){
		/* backup to first one, if more than one */
		while(j && (yytext[j-1]=='\\')){
			unput('\\');
			j--;
		}
		escape_on = 1;
		current_token[j]='\0';
	  }
	  else {

	  if((strcasecmp(current_token,"NOT")==0) ||
		   (strcasecmp(current_token,"~")==0) ||
		   (strcasecmp(current_token,"!")==0) ||
		   (strcasecmp(current_token,"-")==0) ) return NOT;

		if((strcasecmp(current_token,"AND")==0) ||
		   (strcasecmp(current_token,"^")==0) ||
		   (strcasecmp(current_token,"&")==0) ||
		   (strcasecmp(current_token,".")==0) ||
		   (strcasecmp(current_token,"*")==0) ) return AND;

		if((strcasecmp(current_token,"OR")==0) ||
		   (strcasecmp(current_token,"|")==0) ||
		   (strcasecmp(current_token,"+")==0)) return OR;

		if((strcasecmp(current_token,"IMPLIES")==0) ||
		   (strcasecmp(current_token,"->")==0)) return IMPLIES;

		if((strcasecmp(current_token,"EQUIVALENT")==0) ||
		   (strcasecmp(current_token,"<->")==0)) return EQUIVALENCE;
 
		return VARIABLE;
	}
}
		
<<EOF>> { eof_flag = 1; return END_OF_INPUT;
	}

%%

/* This is called whenever the scanner encounters EOF. Returning 0 from
   it indicates we should continue scanning, returning 1 indicates this
   is really the end */

#ifdef yywrap  /* old versions of flex declared this as a macro */
#undef yywrap
#endif

/* This returns 1 to tell the scanner to return 0 token
   If it returns 0 the scanner will continue scanning.  */

int yywrap()
{
	int c; /* Use later ? */
	return 1;
}

void myunput(void)
{

	int i,n;

	n = strlen(current_token);
	for(i=0;i<n;i++)unput(current_token[i]);
}

void discard(void)
{
	current_token[0] = 0;
}

void reset(void){

	eof_flag = 0;
	discard();

}
