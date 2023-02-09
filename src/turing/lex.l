%{

/* Scanner for turing machine language compiler */

#include <stdlib.h>
#include "stack.h"
#include "global.h"
#include <string.h>
#include "y.tab.h"
#include "macro.h"


extern int nstates;
extern struct statestruct *states[];
static int escape_on = 0;
static int firstcall = 1;
struct stackstruct *pending_inputs;
struct stackstruct *pending_buffers;
struct stackstruct *pending_inputs_filenames;
struct stackstruct *pending_inputs_lines;
struct stackstruct *pending_inputs_macroline;
struct stackstruct *pending_prefixes;
extern char input_filename[];
int line = 0;
int macroline=0;  /* line + macroline is source line */
char current_token[MAXTOKEN];
int i;
int is_directive(char *);

static int hard_eof=1;  /* used to signal the difference between end of a
                           stacked input, or top-level input */


%}

%%

 { 


	char buf[2];

 	for(i = 0; i<MAXTOKEN; i++)current_token[i]='\0';

	if(firstcall) { /* Do this when yylex is first called */


		nstates = 10;    /* set states to default: digits 0-9 */
		for(i=0;i<10;i++){
			states[i]=malloc(sizeof(struct statestruct));
			sprintf(buf,"%d",i);
			states[i]->name = (char *)strdup(buf);
			states[i]->indx = i;
		}

		/* create stacks for nested input files */
	
		pending_inputs = alloc_stack(256);
		pending_buffers = alloc_stack(256);
		pending_inputs_filenames = alloc_stack(256);
		pending_inputs_lines = alloc_stack(256);
		pending_inputs_macroline = alloc_stack(256);
		pending_prefixes = alloc_stack(256);
				
		firstcall = 0;
	}
 }
   


#  { if(escape_on){
		strcat(current_token,yytext);
		escape_on = 0;
	}
	else {  /* throw the rest of the line */
		char c;
		while(1){
			c = input();
			if(c == '\n') { 
				line++;
				if(line > macroline)macroline = line;
				return '\n';
			}
		}
	}
    }

^[ \t]*\n { 
			line++; /* skip blank lines */ 
			if(line > macroline)macroline = line; 
	}
[ \t\n] { if(yytext[0]=='\n'){
			line++;
			if(line > macroline)macroline = line;
	} 
	if(escape_on){
		strcat(current_token,yytext);
		escape_on = 0;
	  } 
	  else 
		if(yytext[0]=='\n')
			return '\n';
		 /* else throw ws on floor */
      } 

[\\] { if(escape_on){
		strcat(current_token,yytext);
		escape_on = 0;
	}
	else escape_on = 1;
     }

\.[^\\ \t\n]* {   /* hmm.... it looks like this might be a macro */

		struct macro_struct *amacro;
		int i = 0;
		char *p;
		char buf[256];
		
		if(!escape_on && (strcmp(yytext+1,"LINE")==0)){
			sprintf(buf,"%d",line+1);
			i = strlen(buf);
			p = buf;
			while(i--)unput(*(p+i));
			BEGIN 0;
		}
		else
		if(!escape_on && (strcmp(yytext+1,"FILE")==0)){
			sprintf(buf,"%s",input_filename);
			i = strlen(buf);
			p = buf;
			while(i--)unput(*(p+i));
			BEGIN 0;
		}
		else	
		if(!escape_on && (strcmp(yytext+1,"VERSION")==0)){
			sprintf(buf,"%s",VERSION);
			i = strlen(buf);
			p = buf;
			while(i--)unput(*(p+i));
			BEGIN 0;
		}
		else	
		if((amacro = getmacro(yytext+1))&&(!escape_on)){
			p = expand_macro(amacro,yytext+1);
			if(p){
				i = strlen(p);
				while(i--){	
					if(*(p+i)=='\n'){
						if(line >= macroline)
							macroline = line;
						line--;
					}
					unput(*(p+i));
				}
				free(p);
			}
			BEGIN 0; 
		}
		else {
			strcat(current_token,yytext); /* not a macro */
			if(escape_on){
				escape_on = 0; 
				return TOKEN;
			}
			else 
				if(is_directive(yytext))return KEYWORD;
				else return TOKEN;
		}
    }

[^.#\\ \t\n][^\\ \t\n]* { strcat(current_token,yytext);
		     return TOKEN;
	}
		

<<EOF>> { if(hard_eof){
			return HARDENDFILE; 
		}
		else { hard_eof = 1; return SOFTENDFILE; }
	}

%%

/* This is called whenever the scanner encounters EOF. Returning 0 from
   it indicates we should continue scanning, returning 1 indicates this
   is really the end */

#ifdef yywrap  /* old versions of flex declared this as a macro */
#undef yywrap
#endif

int yywrap()
{
int c;
	FILE *afile;
	YY_BUFFER_STATE abuf;
	char *p;
	int *ip;
	int *mp;

	if(pending_inputs->size == 0){ 
		hard_eof=1;
		return 1;
	}

	fclose(yyin); 
	yy_delete_buffer(YY_CURRENT_BUFFER); 

	yyin = (FILE *)pop_stack(pending_inputs); 
	abuf =  (YY_BUFFER_STATE )pop_stack(pending_buffers); 
	p = (char *)pop_stack(pending_inputs_filenames);
	strcpy(input_filename,p);
	free(p);
	ip = (int *)pop_stack(pending_inputs_lines);
	line = *ip;
	ip = (int *)pop_stack(pending_inputs_macroline);
	macroline = *ip;
	free(ip);
		
	yy_switch_to_buffer(abuf);
	hard_eof=0;
	return 0;
}

void new_input(FILE *afile)
{

		int *a_line_number, *a_macro_line_number;
		push_stack(pending_inputs,(void *)yyin);
		push_stack(pending_buffers,(void *)YY_CURRENT_BUFFER);
		push_stack(pending_inputs_filenames,(void *)strdup(input_filename));
		a_line_number = (int *)malloc(sizeof(int));
		*a_line_number = line;
		a_macro_line_number = (int *)malloc(sizeof(int));
		*a_macro_line_number = macroline;
		line = 0;
		macroline = 0;
		push_stack(pending_inputs_lines,(void *)a_line_number);
		push_stack(pending_inputs_macroline,(void *)a_macro_line_number);
		yyin = afile;
		yy_switch_to_buffer(yy_create_buffer(afile,YY_BUF_SIZE));
}

int is_directive(char *w)
{
	if(strcmp(w,".include")==0)return 1;
	if(strcmp(w,".ifdef")==0)return 1;
	if(strcmp(w,".undef")==0)return 1;
	if(strcmp(w,".endif")==0)return 1;
	if(strcmp(w,".ifndef")==0)return 1;
	if(strcmp(w,".status")==0)return 1;
	if(strcmp(w,".define")==0)return 1;
	if(strcmp(w,".prepend")==0)return 1;
	if(strcmp(w,".echo")==0)return 1;
	if(strcmp(w,".tape")==0)return 1;
	if(strcmp(w,".line")==0)return 1;
	if(strcmp(w,".file")==0)return 1;
	if(strcmp(w,".error")==0)return 1;
	if(strcmp(w,".warn")==0)return 1;
	if(strcmp(w,".alphabet")==0)return 1;
	return 0;
}
