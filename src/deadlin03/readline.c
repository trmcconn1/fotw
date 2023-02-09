/* ReadLine.c: Implementation of the readline subroutine.
   Reads a line of input from a given stream into a given buffer. Returns 
   the number of characters read.

   In DOS, line editing support involving use of the function keys to
   recall and edit previous lines as well the current one is provided
   by console driver code. Unix tty drivers typically behave quite differently,
   so we must supply the DOS functionality by hand. This involves placing
   the terminal in raw mode (done in init_tty) and then intercepting and
   acting upon special control characters.

   The code in this file uses only C-library calls and so is not system-
   dependent.
*/
   
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "deadlin.h"
#include "dosedit.h"
#include <string.h>


#define BEEP fprintf(stderr,"\a");
#define IND(a) ((a)-(buffer))

extern char **Lines;
extern char *LineBuf;
extern int CurrentLine,LastLine,CutStart,CutEnd;
extern int Status;
#ifndef _MSDOS
int ParseAltNum(FILE *);
extern int DoBackspace(int,int);  /* Defined in unix.c */
extern int RawWrite(int,char);        /* Defined in unix.c */
#endif
extern int init_printer(void);
extern int flush_printer(void);
extern void Unescape(char *);
void Tabulate(char *,char *);

/* Printed form of control chars */

char ControlChars[][3] = { "^@","^A","^B","^C","^D","^E","^F","^G",
"^H","^I","^J","^K","^L","^M","^N","^O","^P","^Q","^R","^S","^T","^U",
"^V","^W","^X","^Y","^Z","^[","^\\","^]","^^","^_"};

int TabSize = 8;
char PreviousLine[MAX_LINE_LENGTH+2];

static char *s;
static char CharSizes[MAX_LINE_LENGTH+1]; /* Records the number
              of characters in the visual representation of the chars
              on the line currently being entered for backspacing
              purposes */

/* Handles Intra-line editing */

int ReadLine(FILE *input, char *buffer)
{

	char *p,*pl;
	char c;
	int i,j;
	int escaped = 0;  /* Flag occurence of ESCAPE char */

	s = buffer;
	for(j=0;j<MAX_LINE_LENGTH+1;j++)CharSizes[j]=0;

	pl = PreviousLine;  /* This advances with s, except when in insert
                                 mode */
#ifndef _MSDOS
	if(input == stdin)
	  while(1){   /* Loop while acquiring characters */

		c = (char)fgetc(input);

	/* Check to make sure we don't go beyond the maximum line length */
	/* If so, beep until the user provides a newline to finish it off
	   or a backspace to get out of the situation. 
           The newline is handled in the switch statement below. */

		if((s-buffer >= MAX_LINE_LENGTH-1)&&(c != '\n')&&(c != BACKSPACE)){
			if((c<32)||(c==0x7F)) DoBackspace(1,2);
			else
				DoBackspace(1,1);
			BEEP
			continue;
		}	

		switch(c){      /* Trap special chars not caught by
                                           tty driver */
				case QUIT:
				  clearerr(input); /* Clear EOF condition */
			          fprintf(stderr,"\n");
				  return 0;
				
				case TAB:
				/* We assume that tabs are set every
                                   TabSize spaces. If this isn't true, we're
                                   kind of screwed. */

					Tabulate(s,buffer);
#ifdef _MINIX
/* Minix insists on echoing TAB as ^I */
				        DoBackspace(1,2);
					printf("\t");
#endif
	/* else we assume the terminal driver has done the tab on the screen */
					*s++=c;
				        if(!FLAGGED(INSERT_MODE))pl++;
					continue;	

				case PRINTER_ECHO:
					DoBackspace(1,2);
					if(FLAGGED(LOG_PRINTER)){
						UNFLAG(LOG_PRINTER);
						flush_printer();
					}
					else{
						if(init_printer()){
							fprintf(stderr,"Unable to open PRN device\n");
							continue;
						}
					FLAG(LOG_PRINTER);
					}
					continue;
				

				case '\n':
					*s++ = '\n';
					*s = '\0';
					strcpy(PreviousLine,buffer);
					Unescape(PreviousLine); /* Fixup  ^V  escapes: see parse.c */
					return strlen(PreviousLine);
				case BACKSPACE:
#ifdef _HARDCH
					RawWrite(1,' ');
#else
					DoBackspace(1,2);
#endif
				  /* We actually have to do >= three backspaces
                                    to accomplish anything, because the 
                                    backspace character has already been 
				    been echoed as two characters ^H */                                    
					if(s-buffer == 0)continue;
				        if(!FLAGGED(INSERT_MODE))pl--;
					else UNFLAG(INSERT_MODE);
					s--;
					DoBackspace(1,CharSizes[IND(s)]);
					continue;
				case ALT:
					DoBackspace(1,2);
					if((i=ParseAltNum(input))){
						if(i<32)
					           CharSizes[IND(s)]=2;
						else CharSizes[IND(s)]=1;
						*s++=i;
					}
					continue;		

				case DEL:
					if(FLAGGED(INSERT_MODE))UNFLAG(INSERT_MODE);
					DoBackspace(1,2);
					if(pl<PreviousLine+strlen(PreviousLine))
						pl++;
					continue;
				case INS:
					DoBackspace(1,2);
					if(FLAGGED(INSERT_MODE))UNFLAG(INSERT_MODE);
					else FLAG(INSERT_MODE);
					continue;
				case ESCAPE:
					DoBackspace(1,2); /* ^[ itself */
					printf("\\\n");
					STARTUNNUMBERED
					s=buffer;
					if(FLAGGED(INSERT_MODE))UNFLAG(INSERT_MODE);
					pl=PreviousLine;
					continue;
				case F1:
					if(FLAGGED(INSERT_MODE))UNFLAG(INSERT_MODE);
					DoBackspace(1,2);
					if(pl-PreviousLine <  strlen(PreviousLine)-1){
						*s = *pl++;
						if(*s<32){
							if(*s=='\t'){
								Tabulate(s,buffer);
								printf("\t");
							}
							else {
								CharSizes[IND(s)]=2;
					                	printf("^%c",ControlChars[*s][1]);
							}
						}
						else {
							CharSizes[IND(s)]=1;
					                printf("%c",*s);
						}
						s++;
					}
					continue;
				case F2: 
					if(FLAGGED(INSERT_MODE))UNFLAG(INSERT_MODE);
					DoBackspace(1,2);
					if((p=strchr(PreviousLine,fgetc(input)))){
						DoBackspace(1,1);
						while(pl<p){
						  *s = *pl++;
						  if(*s<32){
							if(*s=='\t'){
								Tabulate(s,buffer);
								printf("\t");
							}
							else {
								CharSizes[IND(s)]=2;
					                	printf("^%c",ControlChars[*s][1]);
							}
						   }
						   else {
							CharSizes[IND(s)]=1;
					                printf("%c",*s);
						   }
						 s++;
						}
					}
					continue;
				case F3:
					if(FLAGGED(INSERT_MODE))UNFLAG(INSERT_MODE);
					DoBackspace(1,2);
					while(pl-PreviousLine<strlen(PreviousLine)-1){
						*s = *pl++;
						  if(*s<32){
							if(*s=='\t'){
								Tabulate(s,buffer);
								printf("\t");
							}
							else {
								CharSizes[IND(s)]=2;
					                	printf("^%c",ControlChars[*s][1]);
							}
						   }
						   else {
							CharSizes[IND(s)]=1;
					                printf("%c",*s);
						   }
						 s++;
					}
					continue;
				case F4:
					if(FLAGGED(INSERT_MODE))UNFLAG(INSERT_MODE);
					DoBackspace(1,2);
					if((p=strchr(PreviousLine,fgetc(input))))
						pl = p;
					DoBackspace(1,1);
					continue;
				case F5:
					if(FLAGGED(INSERT_MODE))UNFLAG(INSERT_MODE);
					DoBackspace(1,2);
					printf("@\n");
					STARTUNNUMBERED;
					*s++ = '\n';
					*s = '\0';
					strcpy(PreviousLine,buffer);
					s=buffer;
					pl=PreviousLine;
					continue;
				case LITERAL:
					escaped=1;
				default: /* This, of course, is the typical
                                            case: just add an ordinary
                                            char to the line buffer and
                                            continue the while loop. */
					if(c<32)
						CharSizes[IND(s)]=2;
					else
					    CharSizes[IND(s)] = 1;
					*s++ = c;
					if(!FLAGGED(INSERT_MODE))pl++;
					continue;
			}
	  }
	else {  /* Reading from a file: trap EOF char only */
		while(1){
			c = fgetc(input);
			if(c == EOF){
				if( s > buffer ){ 
					*s ='\0';
					return(strlen(buffer));
				}
				else return 0;
			}
				
		        if((s-buffer >= MAX_LINE_LENGTH-1)||(c=='\n')){
				*s++ = c;
				*s = '\0';
				return(strlen(buffer));
			}
			if(c== F6)
			  if(!FLAGGED(IGNORE_EOF)){
				*s = '\0';
				return 0;
			  }
			*s++ = c;
		}
	}
#else 
	/* Essentially trivial routine in DOS since the console driver
		does everything. */
          				
		if(fgets(buffer,MAX_LINE_LENGTH,input) == NULL)
			return 0;
		else
			return strlen(buffer);
#endif
}

/* Track the "size" of a tab character */

void Tabulate(char *s, char *buffer)
{
	int i,j;

	/* Determine how many spaces
           until the next tab stop:*/
/* this will be used if we backspace
              over the tab. */
	j=11; /* till start of line */
	for(i=0;i< IND(s);i++)
		j+=CharSizes[i];
	CharSizes[IND(s)] = TabSize-(j%TabSize);
}

#ifndef _MSDOS

/* Simulates the Alt + numeric key translation done by DOS. Translates
a stream of characters containing only ALTs and digits into a number
in the range 1--255 as soon as  3 digits followed by any character, or
as soon as 1 or more digits followed by some character other than ALT are
obtained. Pushes non alt character following digits back on stream.
Returns number in the range 1-255 if translation was successful, and
0 otherwise. */


int
ParseAltNum(FILE *input)
{
	int result = 0;
	int c;
	int stage = 0; 

	while((c=fgetc(input))){
		if((stage<=2)&&((c==ALT)||isdigit(c))){
			if(c == ALT) {
				DoBackspace(1,2); /* extra ^ */
				continue; /* Throw away ALTS */
			}
		        result = 10*result + (c - '0');
			stage++;
			DoBackspace(1,1);
			continue;
		}
		else {
			if(stage >= 2){ /* Must be complete 3 digit num */
			   if(c == ALT){
				DoBackspace(1,2);
				if(result <= 255){
				   if(result > 31)
			            printf("%c",result);
				   else printf("%c^",ControlChars[result][1]);
				}
				return result <= 255 ? result : 0;
		           }
			   else {
				DoBackspace(1,1);
				if(result<=255){
				    if(result > 31)
				        printf("%c",result);
				    else  printf("%c^",ControlChars[result][1]);
				}
			      	printf("%c",c); 
				ungetc(c,input);
				return result <= 255 ? result : 0;
			   }
			}
			else {   /* Invalid: ignore */
				ungetc(c,input);
				return stage > 0 ? result : 0;
			}
		}
	}
	return result;
}	
#endif
