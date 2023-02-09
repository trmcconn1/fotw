%{

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdarg.h>
#include "global.h"
#include "stack.h"
#include "macro.h"

#define RESERVED "RL"
extern char current_token[];        /* see lex.l */
extern int tape[];
extern int tindxmax;
extern int tindx;
extern int line;
extern int macroline;
extern int state;
extern struct stackstruct *pending_inputs;
extern struct stackstruct *pending_buffers;
extern struct stackstruct *pending_prefixes;
extern void new_input(FILE *);
extern char input_filename[];
char *tokens[MAXTOKENS];
int preprocess = 0;
static int ntokens = 0;
static char current_directive[MAXTOKEN];
static int directive_flag = 0;
static int error_flag = 0;
static int exit_flag = 0;
static int if_lvl = 0;
static unsigned toggles = 0xFF;  /* Supports ifdef, etc */
static char state_prefix[MAXTOKEN]="";
char alphabet[MAX_ALPHABET] = "0123456789";
int alphabet_size = 10;
int nstates = 0;
struct statestruct *states[MAX_STATES]; /* initialized in lex.l */
int i;

void handle_directive(void);
void handle_instruction(void);
static int warn(char *,...);
int error(char *, ...);

%}


%token TOKEN KEYWORD HARDENDFILE SOFTENDFILE

%%

	
start: tokenlist terminator {
					if( directive_flag )
					  handle_directive();
					  else 
						if(toggles == 0xFF)
							if(!preprocess)
					  			handle_instruction();
							else { /* print instruction */
							      if(ntokens){
								printf(".file %s\n",input_filename);
								printf(".line %d\n", macroline);
								}
								for(i=0;i<ntokens;i++){
								printf("%s ",tokens[i]);
							}
							if(ntokens)
							  printf("\n\n\n");
						}

					/* clean up */ 

					  for(i=0;i<ntokens;i++)
						free(tokens[i]);
				        ntokens = 0;
					if(error_flag){
						error_flag = 0;
						directive_flag = 0;
						return ERROR;
					}
					if(exit_flag){
						if(exit_flag==2){
							exit_flag=0;
							return HARDEOF;
						}
						exit_flag = 0;
						return SOFTEOF;
					}
					else if(directive_flag){
						directive_flag = 0;
						return DIRECTIVE;
					      }
					      else return INSTRUCTION;
			     }
;

terminator: '\n'
	| SOFTENDFILE { exit_flag = 1; }
	| HARDENDFILE { exit_flag = 2; } 
;
tokenlist:   
	| KEYWORD { 
	        	strcpy(current_directive,current_token+1);
		        directive_flag = 1; 
		}
		
	| tokenlist token  

;
token: TOKEN  {tokens[ntokens++] = (char *)strdup(current_token);}
;

%%

int compare_states(struct statestruct *a, struct statestruct *b)
{
	if(!a)return -1;
	if((!b) && (!a)) return 0;
	return strcmp(a->name,b->name);
}

void handle_instruction(void)
{
	int i,j,n,found;
	char c;
	char buffer[MAX_PREFIX+ MAXTOKEN+1];
	struct statestruct field0;
	char *field1;
	char *field2;
	struct statestruct field3;
	struct instruction *an_instruction,*ip;
	struct statestruct *p;

	/* Check to make sure there is room for the instruction. No point
           in going further if not */

	if(ninstr >= MAX_INSTRUCTIONS){

		fprintf(stderr,"Fatal error at line %d\n",line);
		fprintf(stderr,"Too many instructions. (Precompliled limit is %d)\n",MAX_INSTRUCTIONS);
		fprintf(stderr,"Reduce source, or recompile with larger limit.\n");
		exit(1);

	}

	/* Make sure the number of tokens is divisisble by 4 */

	n = ntokens/4;  /* number of instructions */

	if(ntokens % 4) {

			error("incomplete instruction, %d leftover token(s) on line.",ntokens%4);
			error_flag = 1;
			return;
	}

	for(i=0;i<n;i++){ /* loop over instructions (groups of 4 tokens) */

		field0.name = (char *)malloc(MAXTOKEN+1);
		sprintf(field0.name,"%s%s",state_prefix,tokens[i*4]);
		field1 = (char *) strdup(tokens[i*4+1]);
		field2 = (char *) strdup(tokens[i*4+2]);
		field3.name = (char *)malloc(MAXTOKEN+1);
		sprintf(field3.name,"%s%s",state_prefix,tokens[i*4+3]);

		an_instruction = malloc(sizeof(struct instruction));

		if(!an_instruction){
			error("Internal error: can't allocate instruction");
			exit(1);
		}

		/* set action field */

		if(strcmp(field2,"R")==0)an_instruction->action = RIGHT;
		else
			if(strcmp(field2,"L")==0)an_instruction->action = LEFT;
				else an_instruction->action = WRITE;

	        /* See if the first arg is a state, create new one if
                       necessary  */

		if(!(p = binsearch((void *)&field0,(void **)states,nstates,
				(int(*)(void *,void *))compare_states))){
			if(nstates >= MAX_STATES){
				error("Fatal. Too many states. %d is max.\n",MAX_STATES);
				exit(1);
			}
			p = malloc(sizeof(struct statestruct));
			if(!p){
				error("Fatal. Cannot malloc state struct.\n");
				exit(1);
			}
			p->name = (char *)malloc(strlen(field0.name)+1);
			if(!p->name){
				error("Fatal. Cannot malloc state name.\n");
				exit(1);
			}
			strcpy(p->name,field0.name);
			p->indx = nstates;
			states[nstates]=p;
			nstates++;
			krqsort((void **)states,0,nstates-1,
				(int(*)(void *, void *))compare_states);
		}

		an_instruction->state = -(p->indx+1);

		/* Similarly for 4th arg */

		if(!(p = binsearch((void *)&field3,(void **)states,nstates,
				(int(*)(void *,void *))compare_states))){
			if(nstates >= MAX_STATES){
				error("Fatal. Too many states. %d is max.\n",MAX_STATES);
				exit(1);
			}
			p = malloc(sizeof(struct statestruct));
			if(!p){
				error("Fatal. Cannot malloc state struct.\n");
				exit(1);
			}
			p->name = (char *)malloc(strlen(field3.name)+1);
			if(!p->name){
				error("Fatal. Cannot malloc state name.\n");
				exit(1);
			}
			strcpy(p->name,field3.name);
			p->indx = nstates;
			states[nstates]=p;
			nstates++;
			krqsort((void **)states,0,nstates-1,
				(int(*)(void *, void *))compare_states);
		}
		an_instruction->newstate = -(p->indx+1);
		
		/* no longer needed */
		free(field0.name);
		free(field3.name);

		i=0;
		found=0;
		while(alphabet[i]!='\0'){ 
			if(alphabet[i] == field1[0]){
				found = 1;
				break;
			}
			i++;
		}
		if(!found){
			error("%c not in alphabet", field1[0]); 
			error_flag = 1;
		}
		an_instruction->input = i; 

		if(an_instruction->action == WRITE){
			i=0;
			found=0;
			while(alphabet[i]!='\0'){ 
				if(alphabet[i] == field2[0]){
					found = 1;
					break;
				}
				i++;
			}
			if(!found){
				error("%c not in alphabet",field2[0]); 
				error_flag = 1;
			}
			an_instruction->output = i; 
		}

		/* no longer needed */
		free(field1);
		free(field2);

		/* See whether the instruction exists. It should not. */

		ip = binsearch((void *)an_instruction,(void **)instructions,ninstr,
				(int(*)(void *,void *))compare_inst);
		if(ip != NULL)
			{
				error("matching state/input");
				error_flag = 1;
				free(an_instruction);
			}
		else {  /* All OK. install instruction */	

			instructions[ninstr++] = an_instruction;
			krqsort((void **)instructions,0,ninstr-1,
				(int(*)(void *,void *))compare_inst);

		}

	}
}



static char macro_text[MAX_MACRO_TEXT];
static char arglist[MAX_MACRO_TEXT];

void handle_directive(void)
{
	int i,c,n,found=0;

	/* Note to self: when you add a directive, don't forget to
           update is_directive in lex.l */

	if(strcmp(current_directive,"endif")==0){

		if(!if_lvl)error("unmatched .ifdef or .ifndef\n");
		else {
		if_lvl--;
		toggles |= (1<<if_lvl);
		}
		return;
		
	}


	if(strcmp(current_directive,"ifdef")==0){

		if(!getmacro(tokens[0]))
			toggles &= ~(1<<if_lvl);
		if_lvl++;
		return;
	}

	if(strcmp(current_directive,"ifndef")==0){

		if(getmacro(tokens[0]))
		        toggles &= ~(1<<if_lvl);
		if_lvl++;
		return;
	}

        /* Remaining are ignored when there is a pending false ifdef or
           ifndef as indicated by toggles */

	if(toggles < 0xFF)return;

	/* undef directive */
	if(strcmp(current_directive,"undef")==0){
		if(rmmacro(tokens[0]))
			warn("undef: no such macro %s\n",tokens[0]);
		return;
	}

	/* status directive */
	if(strcmp(current_directive,"status")==0){
		found=1;
		printf("\nCurrent compilation status:\n\n");
		printf("Alphabet: %s\n",alphabet);
		printf("%d instructions compiled.\n",ninstr); 
		dump_macros();
		printf("States (n=%d):\n",nstates);
		for(i=0;i<nstates;i++)
			printf("%d. %s\n",states[i]->indx,states[i]->name);
		return;
	}

	/* line directive */

	if(strcmp(current_directive,"line")==0){
			if(ntokens == 1)
				line = atoi(tokens[0]);
				macroline = line;
			found = 1;
	}

	/* file directive */

	if(strcmp(current_directive,"file")==0){
			if(ntokens == 1)
				strcpy(input_filename,tokens[0]);
			found = 1;
	}

	/* echo directive */
	if(strcmp(current_directive,"echo")==0){
		found = 1;
		if(ntokens)
			for(i=0;i<ntokens;i++)
				printf("%s ",tokens[i]);
		printf("\n");
	}

	/* define directive */
	if(strcmp(current_directive,"define")==0){

		struct macro_struct *my_macro;
		char *p,*q;
		
		found = 1;	

		/* See if a macro by this name is already defined */

		my_macro = getmacro(tokens[0]);
		if(my_macro){
			warn("macro %s already defined",tokens[0]);
			return;
		}

		macro_text[0] = '\0';
			for(i=1;i<ntokens;i++){
				strcat(macro_text,tokens[i]);	
				strcat(macro_text," ");
			}

				/* See if macro_text starts with a (. If so
                                then we need to get at the argmuments.) */
				i=0;	
				if(macro_text[0]=='('){

					/* search for closing ')', putting
                                           everything except whitespace 
                                           between into a buffer */
					
					n=0;
					for(i=1;i<strlen(macro_text);i++){
						if(macro_text[i]==')'){
							i++;
							break;
						}
						if(macro_text[i]==' ')continue;
						arglist[n++]=macro_text[i];
					}
					arglist[n] = '\0';
					if(i==strlen(macro_text)){
						error("Bad macro syntax for %s\n",tokens[0]);
						return;
					}
					
				}	
				my_macro = (struct macro_struct *)
					malloc(sizeof(struct 
							macro_struct));
				my_macro->value = (char *) strdup(macro_text+i);
				my_macro->name = (char *) strdup(tokens[0]);
				if(n){ /* non-empty arg list */

					my_macro->argc=0;
					my_macro->argv = (char **)malloc(n*sizeof(char *));
					p = strtok(arglist,",");
					if(p){
						*(my_macro->argc+my_macro->argv)
							= (char *)strdup(p);
						my_macro->argc++;
					}
					while(p=strtok(NULL,",")){
						*(my_macro->argc+my_macro->argv)
							= (char *)strdup(p);
						(my_macro->argc)++;
					}
				}
				addmacro(my_macro);
			return;
	}

	/* include directive */
	if(strcmp(current_directive,"include")==0){
		FILE *afile;

		found=1;
		afile = fopen(tokens[0],"r");
		if(!afile){
			error("cannot open include file %s", tokens[0]);
			error_flag = 1;
			return;
		}

		new_input(afile);
		strcpy(input_filename,tokens[0]);
		
	}

	/* prepend directive */
	if(strcmp(current_directive,"prepend")==0){
		char *p,*q,*r;
		found=1;
		if(ntokens<=0){
			p = pop_stack(pending_prefixes);
			if(p){
			/* wipe out last occurence of string p in 
                                  state_prefix */
				r = state_prefix;
				while((q = strstr(r+1,p)))r=q;
				*r = '\0';
				free(p);
			}
		}
		else {
			push_stack(pending_prefixes,(void *)strdup(tokens[0]));
			strcat(state_prefix,tokens[0]);
		}
	}

	/* error directive */

	if(strcmp(current_directive,"error")==0){

		error(tokens[0]);
		error_flag = 1;
		return;
	}

	if(strcmp(current_directive,"warn")==0){

		warn(tokens[0]);
		return;
	}
	/* alphabet directive */
	if(strcmp(current_directive,"alphabet")==0){

		found=1;

		/* check the proposed new alphabet: any reserved letters ? */

		if(strpbrk(tokens[0],RESERVED)!=NULL){
			error("reserved letter in alphabet");
			error_flag = 1;
			return;
		}
		
		strcpy(alphabet,tokens[0]);
		alphabet_size = strlen(alphabet);
		if(ntokens > 1)warn("Extra tokens on line");
	}

	/* tape directive */
	if(strcmp(current_directive,"tape")==0){

		char *p;
		int head = 0; /* flag to indicate non default head */
		found = 1;

		n = strlen(tokens[0]);
		for(i=0;i<n;i++){
			if(tokens[0][i]=='R'){
				tape[tindxmax++]=-1;
				tindx = tindxmax;
				head = 1;
			}
			else {
			  p = strchr(alphabet,tokens[0][i]);
			  if(p)tape[tindxmax++]=(int)(p-alphabet);
			  else {
				error("tape directive has invalid (non-alphabet?) character %c",
				tokens[0][i]);
				error_flag = 1;
				break;
			  }
			}
			
		}
		if(head)tape[0]=0;
	}
	if(!found){
			
		error("unknown directive %s", current_directive);
		error_flag = 1;
		
	}

}

int warn ( char *format, ...){

	va_list ap;
	
	va_start(ap,format);
	fprintf(stderr,"%s: %d(warning): ",input_filename,macroline);
	vfprintf(stderr,format,ap);
	fprintf(stderr,"\n");
	va_end(ap);
	return 0;
}

int error( char *format, ...) {

	va_list ap;

	va_start(ap, format);
	fprintf(stderr,"%s:%d: ",input_filename,macroline);
       	vfprintf(stderr,format,ap);
	fprintf(stderr,"\n");
	for(i=0;i<ntokens;i++)free(tokens[i]);
	ntokens = 0;
	va_end(ap);
	return 0;
}
 
int yyerror( char *msg ){
	return error("%s",msg);
}
