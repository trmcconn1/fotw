/* Turing.c: main program for a turing machine simulator. 

	By Terry R. McConnell (trmcconn@syr.edu)

	In normal operation this program parses one or more machine
description files listed on the command line and then runs the resulting
machine, reporting on the tape contents after each cycle. The machine 
description parser is built using bison (yacc) and flex (lex) from the
source files parse.y and lex.l. 

	This main program parses the command line, calls yyparse on each 
successive file named on the command line (or on stdin if none are named),
and then calls do_turing (in this file) to run the resulting machine. 

	See the documentation for information on the machine description
language and on how to build and install the package.

*/

#include<stdio.h>
#include<stdlib.h>
#include "global.h"
#include "macro.h"
#include<string.h>

#define PROGNAME "turing"
#define USAGE "turing [-d <n> -i <digits> -D<name[=value]>  -n <n> \n\
		-t <n> -lcEvh] [ source files ... ]"

#define HELP "-h: print this helpful message\n\
-v: print version number and exit\n\
-l: print a listing of the compiled program\n\
-c: compile only. don't run the machine\n\
-E: process only macros and directives. Print instructions and line number\n\
    directives to stdout.\n\
-i: take the following string as the initial tape state (default alphabet)\n\
    (A ~ character in the string denotes that the initial read head is\n\
     scanning the next character to the right. By default it is positioned \n\
    at the left end of the string.)\n\
-d: take n as number of seconds to delay between cycles\n\
-n: print tape update every nth cycle (default n=1.)\n\
-t: print tape dump every nth cycle (default is not to print dump till end.)\n\
-D: define a macro, and optionally give it a value\n\n\
Simulates a turing machine based on description in files (or stdin). \n\n"

/* Tape size grows as necessary. Probably won't be necessary. */
#define INITIAL_TAPE_SIZE 0xFFFF

static int state;
static int delay = 0;
static int step;
static int dump_cycles=0;
static int mark_cycles=1;
int *tape;
int tindx;          /* current tape index of the "read head" */
int tindxmax;       /* largest tape index with any content */
int ninstr = 0;
char input_filename[MAX_PATH] = "stdin";

struct instruction *instructions[MAX_INSTRUCTIONS];

/* Banner: print name of program etc */

void banner()
{			
	printf("\n\t\t\t%s( %s )\n\n",PROGNAME,VERSION);
}

/* Dump tape routine */

void dump(void);

/* error routine */

void internal_error(char *msg){

	fprintf(stderr,"%s",msg);
	exit(1);
}

/* mark_cycle: prints out current tape information */

#define TAPE_DUMP_WIDTH 16

void mark_cycle(void)
{
	int i;

	/* Make sure the tape is big enough */
	while(tindx + TAPE_DUMP_WIDTH >= tape_size - 2){
		if(grow_tape() == FALSE){
			fprintf(stderr,"%s: unable to grow tape.\n",PROGNAME);
			exit(1);
		}
	}
	printf("%d.\t",step);
	if(tindx < TAPE_DUMP_WIDTH/2) 
		for(i=0;i<TAPE_DUMP_WIDTH+1;i++)
			if(i==tindx-1)
				printf("-->"); 
			else
				printf("  %c",alphabet[tape[i]]);
	else
		for(i=tindx-TAPE_DUMP_WIDTH/2;i<tindx + TAPE_DUMP_WIDTH/2 +1;i++)
			if(i==tindx-1)
				printf("-->"); 
			else
				printf("  %c",alphabet[tape[i]]);
	printf("\t<%s>\t%d\n",states[(-state)-1]->name,tindx);
	
}

/* compare_inst: comparison routine for sorting array of instructions. 
   Compares instructions using state as primary key and input as secondary
   key */

int compare_inst(struct instruction *a, struct instruction *b)
{

	int a_text,b_text;

	if(!a){
		if(!b)return 0;
		else return -1;
	} 
	if(!b)return 1;
	if(a->state < b->state)return -1;
	if(a->state > b->state)return 1;


	if(a->input < b->input)return -1;
	if(a->input > b->input)return 1;
	return 0;
}

/* compare_states_by_indx */
int compare_states_by_indx(struct statestruct *a, struct statestruct *b)
{

	if (a->indx < b->indx) return -1;
	if( a->indx > b-> indx) return 1;
	return 0;
}

/* find_instruction: see if any current instruction matches. Return pointer to
   one that does, NULL otherwise. 
   */

struct instruction *find_instruction(int state, int input)
{

	int i=0;
	struct instruction inst;
	
	inst.state = state;
	inst.input = input;

	return binsearch((void *)&inst,(void **)instructions,ninstr,
		(int (*)(void *,void *))compare_inst);

}

/* Runs the machine: returns 1 if machine halts because no matching instruction
                     could be found. Returns 0 if machine enters halt state */

int do_turing()

{

	struct instruction *current_instruction;
	int save,i;
	int input,action,output,newstate;

	/* main loop */

	while(1){


	/* do whatever you do at the beginning of each cycle */

		step++;
		if(mark_cycles && !( step % mark_cycles)) mark_cycle();
		if(dump_cycles && !( step % dump_cycles)) dump();
		if(delay)sleep(delay);
		input = tape[tindx];

	/* See whether any instruction matches */

		current_instruction = find_instruction(state,input);

		if(!current_instruction) return 1;

		action = current_instruction->action;
		output = current_instruction->output;
		newstate = current_instruction->newstate;

		switch(action){
		
			case WRITE: /* change state and update tape contents */

				if(tindx<=0) internal_error("bad index\n");
				state = tape[tindx-1] = newstate; 
				tape[tindx] = output;
				break;

			case RIGHT:

				/* move the read head right */

				if(tindx == tindxmax){
					
					while(tindxmax >= tape_size-1){
						if(grow_tape() == FALSE)
							fprintf(stderr,"%s: Cannot increase tape size\n",PROGNAME);
						exit(1);
					}
					tape[tindxmax+1]=0;
					save = tape[tindxmax];
					state = tape[tindxmax] = newstate;	
					tape[tindxmax-1]=save;	
					tindxmax++;
				}
				else {
					save = tape[tindx];
					state = tape[tindx] = newstate; 
					tape[tindx-1] = save;
				}
				tindx++;
				break;
				
			case LEFT:

				/* move the read head left */

				if(tindx==1){

					/* Going to have to copy entire
                                           tape one to right and create blank
                                           at beginning */

					while(tindxmax >= tape_size-1){
						if(grow_tape() == FALSE)
							fprintf(stderr,"%s: Cannot increase tape size\n",PROGNAME);
						exit(1);
					}

					tindxmax++;
					for(i=tindxmax;i>1;i--)
						tape[i]=tape[i-1];
					tape[1]=0;
					tape[0] = state = newstate;

				}
				else {

					tape[tindx-1]=tape[tindx-2];
					tape[tindx-2] = state = newstate;
					tindx--;

				}
				break;

			case HALT:   /* This is not in use */
				return 0;
			default:
				internal_error("unknown instruction action\n");
				exit(1);
				
		}
	}
	
}


/* The main program */

int main(int argc, char **argv)
{

	int rval;
	char *p,*q;
	struct macro_struct *amacro;
	int verbosity = 0;
	int r;
	int nfiles = 0;
	int err_flag = 0;
	int dump_tape = 1;
	int do_list = 0;
	int do_run = 1;
	int i=1,j,k;
	FILE *current_input = stdin;

	/* initialize */

	tape = alloc_tape(INITIAL_TAPE_SIZE);
	if(!tape){
		fprintf(stderr,"%s:cannot allocate tape.\n",PROGNAME);
		return 1;
	}
	ninstr = 0;
	tindx=1;
	tindxmax=1;
	tape[0]=-1;
	state = -1;

	/* Process command line */

	while((i<argc) && (argv[i][0]=='-')){
		switch(argv[i][1]){
		case 'v':
			printf("%s\n",VERSION);
			return 0;
		case 'h':
			printf("\n%s\n\n",USAGE);
			printf("%s\n",HELP);
			return 0;
		case 'l':
			do_list = 1;
			verbosity = 0; /* we set this again below in case */
			break;
		case 'c':
			do_run = 0;
			verbosity = 1;
			break;
		case 'n': 
			mark_cycles = atoi(argv[i+1]);
			i++;
			break;
		case 't': 
			dump_cycles = atoi(argv[i+1]);
			i++;
			break;
		case 'E': do_run = 0;
			  preprocess = 1;
			  break;
		case 'd':
			delay = atoi(argv[i+1]);
			i++;
			break;
		case 'D': /* defining a macro */
			/* point p at option argument */
			if(strlen(argv[i])==2){ /* space before option arg */
				p = argv[i+1];
				i++;
			}
			else p = argv[i]+2;
			amacro = (struct macro_struct *)malloc(sizeof(struct
						macro_struct));
			/* see if there is an = */
			q = strchr(p,'=');
			if(!q)
				amacro->value = (char *)strdup("");
			else {
				*q = '\0';
				amacro->value = (char *)strdup(q+1);
			}
			amacro->name = (char *)strdup(p);
			addmacro(amacro);
			break;	
			
		case 'i':
			i++;
			j=0;
			while(argv[i][j] != '\0'){
				if(argv[i][j]=='~'){
					for(k=0;k<j;k++)
						tape[k]=tape[k+1];
					tape[j]=state;
					tindx+=j;
					j++;
			        }
				else
					tape[tindxmax++]=argv[i][j++]-'0';
		        }
			break;
		default:
			fprintf(stderr,"%s:%s\n",PROGNAME,USAGE);
			return 1;
		}
		i++;
	}



	if(do_list)verbosity = 0;
	if(verbosity)banner();
	if(i < argc)
		while(i <  argc){  /* process the remaining files as successive source files */

			current_input = fopen(argv[i],"r");
			if(!current_input){
				fprintf(stderr,"%s: cannot open %s\n",PROGNAME,argv[i]);
				return 1;
			}

		strcpy(input_filename,argv[i]);
		nfiles++;
		if(verbosity)printf("Compiling %s...\n",input_filename);
		/* read in the instructions */
		yyin = current_input;
		line = 0;
		while(1){
			r = yyparse();
			if(r == ERROR)err_flag = 1;
			if(r == HARDEOF)break;
		}
		i++;
	}
	else {
		/* reading from stdin */
		line = 0;
		while(1){
			r = yyparse();
			if(r == ERROR)err_flag = 1;
			if(r == HARDEOF)break;
		}
	}

	/* print a listing, if one is asked for */
	if(do_list){

		for(i=0;i<ninstr;i++)
			switch(instructions[i]->action){
				case WRITE:
					printf("%d;%d;%d;%d\n",
					   instructions[i]->state,
					   instructions[i]->input,
					   instructions[i]->output,
					   instructions[i]->newstate);
					break;
				case RIGHT:
					printf("%d;%d;R;%d\n",
					   instructions[i]->state,
					   instructions[i]->input,
					   instructions[i]->newstate);
					break;
				case LEFT:
					printf("%d;%d;L;%d\n",
					   instructions[i]->state,
					   instructions[i]->input,
					   instructions[i]->newstate);
					break;
				case HALT: /* This is obsolete */
					printf("HALT\n");
					break;
				default:
					fprintf(stderr,"internal error: Unknown action code number %d \n",instructions[i]->action);
			}
	}

	/* resort the state array by index */
	krqsort((void *)states,0,nstates-1,(int(*)(void *,void *))
		compare_states_by_indx);

	/* run the turing machine if we should */

	if((!err_flag) && do_run)
		rval = do_turing();
	else {
		if(do_run && err_flag){
			printf("Turing: did not run due to compilation error(s).\n");
	        	return 1;	
		}
		if(!do_run){ /* Print some compilation statistics so that the
			        user gets something for his money */
			if(!preprocess){
			  unsigned long count = 0L;
			  for(i=0;i<ninstr;i++)
				count += (unsigned long)sizeof(instructions[i]);
			  if(verbosity){
				printf("Compiled %d instructions (%lu bytes) in %d file(s)\n",
					ninstr,count,nfiles);
			        printf("[%d states, alphabet size = %d]\n",
						nstates,alphabet_size);

			  }
			}
		}
		return 0;
	}
	/* Print final tape */
	if(dump_tape)dump();
	return 0;

}

void dump(void){

		int i,j;
		i=0;
		j=0;
		printf("\n\nTape[%d]: (-->head)\n",step);
			while(i<tindxmax){	
				if(!j%80)printf("\n");
				/* Bug: will not handle --> split over line */
				if(tape[i]<0){
					printf("-->");
					j+= 2;
					i++;
				}
				else{
					printf("%c",alphabet[tape[i]]);
					i++;
				}
				j++;
			}
			printf("\n\n");
}

