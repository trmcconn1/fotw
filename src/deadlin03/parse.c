/* Deadlin edlin clone: Parser for command mode and supporting routines */
/* Also implements all commands */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "deadlin.h"
#include "dosedit.h"
#ifndef _MSDOS
#include "unix.h"
#endif
#include <string.h>

/* Macros */

#define FLOOR(x)  ( (x) > 0 ? x : 0 ) 
#define CEIL(x)   ( (x) <= LastLine + 1 ? x : LastLine + 1)
#define LOCEIL(x)   ( (x) < LastLine  ? x : LastLine )
#define EAT_SPACE(p)  while((*(p) == ' ')||(*(p) == '\t') )(p)++ 

/* Return values for GetRange function */
#define NOFIELDS 0x00
#define FIELD1 0x01
#define FIELD2 0x02
#define FIELD3 0x04
#define FIELD4 0x08
#define ERROR  0xFF

/* Global Variables */

extern char **Lines;
extern char *LineBuf;
extern int Status;     /* Bitmap */
extern int MaxLines;
extern int MemAvail;
extern int MemStart;
extern int LinesToAppend;
extern char ControlChars[][3];

/* LastLine is the index of the last line present in the file.
   CurrentLine is index of the next line that will be entered.  */

extern int CurrentLine,LastLine,CutStart,CutEnd,DestLine,Count;
extern char FileExt[];
extern char FilePath[];
extern char BaseName[];
extern FILE *tempfile;   /* Used for printer output */
extern char PreviousLine[];

/* Functions */

extern void* MyMalloc(int);
extern int StringFree(void *);
extern int ReadLine(FILE *,char *);
extern int cleanup(void);
extern int DoBackspace(int,int);
void PrintLine(char *);
int GetNextCmd(char *, char *);
int GetCmdLetter(char *,int *);
int GetRange(char *command, char letter);
int myatoi(char *);
void InputError(void);
void Unescape(char *);
char *ReplaceString(char *,char *, char *,int);


/* Parse: Use to read and parse input while in command mode. The caller
   decides what action to take based upon what Parse returns to it.
   If it returns 0, no action needs to be taken by caller.
   If the internal buffer is non empty, the next command will be read
   from it. Otherwise the next command will be read from the user.
*/

/* Valid Command Letters */

#define LETTERS "aAcCdDeEiIlLmMpPqQrRsStTwW?"

/* States of the parser */

#define BAD_PARSE 0
#define BEGIN 1
#define EDIT_LINE 2
#define NUMBER 3
#define INTERACTIVE_REPLACE 4
#define INTERACTIVE_SEARCH 5
#define NOP 6

/* These shouldn't go beyond 10 to avoid conflicting with the return values
   from parse defined in main.c */

int Parse(FILE *input)
{
	int number = 0;
	int cmdlen = 0;
	int lastlen = 0;
	int rval = 0;
	int charpos;
	char c,*ptr;
	int i,j,k,l;
	static char PrevLine[MAX_LINE_LENGTH+2];
	static char buffer[MAX_LINE_LENGTH+2];
	static char Search[MAX_LINE_LENGTH+2];
	static char Replace[MAX_LINE_LENGTH+2];
	FILE *sourcefile;
	FILE *ostream;


    if(PrevLine[0] == '\0'){  /* Need to collect anything in buffer ? */
       /* Prompt user for input */
       fprintf(stderr,"%s","*");
       if(!ReadLine(input,LineBuf)) /* LineBuf filled from user with DOS
                                          Editing enabled */
		return 0;
	else
		strcpy(PrevLine,LineBuf);
    }

    /* Now PrevLine is either filled with new commands or with the remains of
       old ones.  Note that PrevLine always has length >= 1 */

    lastlen = strlen(PrevLine);

    if((cmdlen = GetNextCmd(PrevLine,buffer))){/* anything valid to do ?*/
	  switch( c = GetCmdLetter(buffer,&charpos) ) {
		case NOP:   /* e.g., a bare return */
			break;

		/* Implementation of individual line editing commands */

	        case 'A':
		case 'a':  /* Append Lines */
  			switch(GetRange(buffer,c)){
                                case FIELD1:
                                        LinesToAppend = CutStart+1;
                                        break;
                                case NOFIELDS:
					LinesToAppend = MaxLines;
                                        break;
                                case ERROR:
                                default:
                                        InputError();
                                        goto reset;
                        }


			/* We pass the number of lines to append back to
                           main in the LinesToAppend variable. */

			rval =  APPEND_LINES;
			break;

		case 'w':   /* Write Lines */
		case 'W':
			switch(GetRange(buffer,c)){
                                case FIELD1:
					k = CutStart+1; /* number lines to write */ 
                                        break;
                                case NOFIELDS:
                                        k = LastLine+1;
                                        break;
                                case ERROR:
                                default:
                                        InputError();
                                        goto reset;
                        }

			strcat(BaseName,".$$$"); /* name of temp output file */
			ostream = fopen(BaseName,"a");
			for(i=0;i<k;i++){
				if(4*j >= 3*MemStart)break;
				fprintf(ostream,"%s",Lines[i]);
				j += StringFree(Lines[i]);
			}
			fclose(ostream);
			BaseName[strlen(BaseName)-4]='\0';
			/* Move remaining lines down */
			for(j=i;j<=LastLine;j++)
				Lines[j-i] = Lines[j];
			LastLine -=i;
			break;

		case 'I':
		case 'i':
			switch(GetRange(buffer,c)){
				case FIELD1:
	    				CurrentLine = CutStart;
					break;
				case ERROR:
					goto reset;
				case NOFIELDS:
					break;
				default:
					InputError();
					goto reset;
			}
					
			FLAG(INPUT_MODE);
			break;
		case 'Q':
		case 'q':
			printf("Abort edit(Y/N)? ");
			j = fgetc(input); 
			printf("\n");
			if((j=='y')||(j=='Y')){
			     cleanup();
			     exit(0);
			}
			fflush(input);
			PrevLine[0] = '\0';
			return 0;
		case 'e':
		case 'E':
			return END_EDIT;
		case EDIT_LINE: 
			number = myatoi(strtok(buffer," \t\n;"));
		        if(strchr(buffer,'+'))
		          CurrentLine += number;
		        else if(strchr(buffer,'-'))
				CurrentLine += number;
			      else if(number) CurrentLine = number -1;
				else CurrentLine++;
			if(CurrentLine < 0)CurrentLine = 0;
			if(CurrentLine > LastLine){
				 CurrentLine = LastLine+1;
				 break;
			}
			STARTLINE(CurrentLine)
			PrintLine(Lines[CurrentLine]);
			STARTLINE(CurrentLine)
			strcpy(PreviousLine,Lines[CurrentLine]);
			j = ReadLine(stdin,LineBuf);

			if((j==MAX_LINE_LENGTH) &&
				(LineBuf[MAX_LINE_LENGTH]!='\n'))
				{
				  LineBuf[MAX_LINE_LENGTH] =
					'\n';
				  fprintf(stderr,"\a");
				}
			if(j>1) /* quirk: j==0 makes more sense*/
			  if((ptr = (char *)MyMalloc(strlen(LineBuf)+1))){
			   StringFree(Lines[CurrentLine]);
			   strcpy(ptr, LineBuf);
			   Lines[CurrentLine] = ptr;
		          }

		break;

		case 'l':     /* List Lines command */
		case 'L':
			switch(GetRange(buffer,c)){
				case FIELD1:
	    				CutEnd = CutStart + 22;
					break;
				case NOFIELDS:
				case FIELD2:
				case FIELD1|FIELD2:
					break;
				case ERROR:
					CutStart = FLOOR(CurrentLine - 11);
					CutEnd  = CurrentLine + 11;
					break;
				default: 
					InputError();
					goto reset;
			}	
			CutEnd = LOCEIL(CutEnd);
			for(i=CutStart;i<= CutEnd;i++){
				STARTLINE(i)
		        	PrintLine(Lines[i]);
			}
			break;

		case 'p':    /* Page command */
		case 'P':
			switch(GetRange(buffer,c)){
				case FIELD2:   /* first field omitted */
 					CutStart = CurrentLine + 1;
					break;
				case FIELD1:  /* second field omitted */
					CutEnd = CutStart + 22;
					break;
				case NOFIELDS:  /* both fields omitted */
					CutStart = CurrentLine + 1;
					CutEnd = CutStart + 22;
					break;
				case FIELD1|FIELD2:
					break;
				case ERROR:
					goto reset;
				default:
					InputError();
					goto reset;
			}
			CutEnd = LOCEIL(CutEnd);
			CurrentLine = CutEnd;
			for(i=CutStart;i<= CutEnd;i++){
				STARTLINE(i)
		        	PrintLine(Lines[i]);
			}
			break;

		case 'd':      /* Delete Lines Command */
		case 'D':
			switch(GetRange(buffer,c)){
				case FIELD2: /* Omitted first field in range */
					CutStart = CurrentLine;
					break;
				case FIELD1: /* Omitted second field */
					CutEnd = CutStart;
					break;
				case NOFIELDS: /* Both fields omitted */
					CutEnd = CutStart = CurrentLine;
					break;
				case FIELD1|FIELD2:
					break;
				case ERROR:
					goto reset;
				default:
					InputError();
					goto reset; 
			}
			/* Remove lines */
			for(i=CutStart;i<=CutEnd;i++)
				StringFree(Lines[i]);
			/* Move remaining lines down */
			for(i=CutEnd+1;i<=LastLine;i++)
				Lines[CutStart + i - CutEnd-1] =
					Lines[i];
			/* Zero out lines vacated at end of file */
			for(i=CutStart+LastLine-CutEnd;i<=LastLine;i++)
				Lines[i]= NULL;
			LastLine -= (CutEnd - CutStart+1);
			CurrentLine = CutStart;	
			break;		

		case 'c':     /* Copy Lines command */
		case 'C':     /* This one is pretty horrendous :-( */
			i=GetRange(buffer,c);
			if(i==ERROR)goto reset;
			if(!(i & FIELD1)) CutStart = CurrentLine;
			if(!(i & FIELD2)) CutEnd = CurrentLine;
			if((CutEnd >= DestLine)&&(CutStart <= DestLine)){
				InputError();
				break;
			}
			/* Copy blocks of lines to the end of available
                            memory */
			/* The number of lines to copy is Count*(CutEnd-CutStart+1).
	                   First make sure we have enough lines at end of
                           file to hold the copied lines temporarily */ 

#define BLOCK (CutEnd-CutStart+1)
			k = BLOCK*Count;
			k = ( k < MaxLines - LastLine - 1 ? k :
					MaxLines - LastLine -1);

			/* k is total number of lines we CAN copy taking
				into account only the size of the Lines
                                pointer array */

			for(i=0;i< k/BLOCK;i++)
				for(j=0;j< BLOCK;j++){
					if(!(Lines[MaxLines-k + i*BLOCK + j]
						= (char *)MyMalloc(strlen(Lines[CutStart
				                    +j])+1)))goto failed;
					strcpy(Lines[MaxLines-k+i*BLOCK+j],
						Lines[CutStart+j]);
				}
			for(j=0;j<(k%BLOCK);j++){
				if(!(Lines[MaxLines-k + i*BLOCK + j]
					= (char *)MyMalloc(strlen(Lines[CutStart
			                    +j])+1)))goto failed;
				strcpy(Lines[MaxLines-k+i*BLOCK+j],
					Lines[CutStart+j]);
			}
failed: 		k = i*BLOCK + j ;

			     /* k is the number of lines actually copied,
                                taking into account the need to allocate
                                memory for each string */

/* The situation is now:
  [Dest ....   Last] ... free ... [ Copied Lines ... MAX_LINES ]
where free is possibly empty. Now move each pointer in the copied line block
back to the block starting at Dest, moving everything from Dest to Last up
one position each time. These shenanigans are needed to make the most of
a limited memory environment. */

				for(i=0;i<k;i++){
					/* Need to store this in case the
                                         free area above really is empty! */
					ptr = Lines[MaxLines - k + i];

					/* Move everything up one */
					for(j=LastLine+i;j>=DestLine+i;j--)
						Lines[j+1]=Lines[j];

					Lines[DestLine+i] = ptr;
				}
			CurrentLine = DestLine;
			LastLine += k;
			break;   /* phew! */

		case 'm':    /* Move Lines Command */
		case 'M':
			i=GetRange(buffer,c);
			if(i==ERROR)goto reset;
			if(!(i & FIELD1)) CutStart = CurrentLine;
			if(!(i & FIELD2)) CutEnd = CurrentLine;
			if((CutEnd >= DestLine)&&(CutStart <= DestLine)){
				InputError();
				break;
			}
		/* It makes a difference whether a block is being moved
                     up or down. */
		      if(DestLine > CutEnd){   /* Moving Up */
			for(i=CutEnd;i>=CutStart;i--){ /* moving line i */
				ptr = Lines[i];
					/* Move everything down one */
					/* to make room for moved line */
				for(j=i;j<=DestLine-(CutEnd-i)-2;j++)
					Lines[j]=Lines[j+1];

				Lines[DestLine-(CutEnd-i)-1] = ptr;
				}
			CurrentLine = DestLine-CutEnd+CutStart-1;
		      }
		      else {    /* Moving Down */
			for(i=CutStart;i<=CutEnd;i++){ /* moving line i */
				ptr = Lines[i];
					/* Move everything up one */
					/* to make room for moved line */
				for(j=i;j>=DestLine+(i-CutStart)+1;j--)
					Lines[j]=Lines[j-1];

				Lines[DestLine+(i-CutStart)] = ptr;
				}
			CurrentLine = DestLine;
		      }
			break;  

		case 't':   /* Transfer Lines Command */
		case 'T':
			switch(GetRange(buffer,c)){
				case FIELD1:
	    				CurrentLine = CutStart;
					break;
				case ERROR:
					goto reset;
				case NOFIELDS:
					break;
				default:
					InputError();
					goto reset;
			}
					
			/* Attempt to open file in current directory
                            for read . Recall that the string following
			 the command letter begins at index charpos */
			i=charpos+1;
			if((sourcefile =
			      fopen(strtok(buffer+i," \t\n;"),"r"))==NULL){
				fprintf(stderr,"File not found\n");
				goto reset;
			}
			while(fgets(LineBuf,MAX_LINE_LENGTH,sourcefile)){
				ptr = (char *)MyMalloc(strlen(LineBuf)+1);
				if(!ptr)break;
				strcpy(ptr,LineBuf);
				/* Make room for new line */
				for(j=LastLine;j>=CurrentLine;j--)
					Lines[j+1]=Lines[j];
				Lines[CurrentLine++]=ptr;
				LastLine++;
			}
			fclose(sourcefile);
			break;

		case INTERACTIVE_SEARCH:
		case 's':  /* Search Text Command */
		case 'S':
			switch(GetRange(buffer,c)){
				case FIELD1:
					CutEnd = LastLine;
					break;
				case FIELD2:
					CutStart = CurrentLine+1;
					CutEnd = LOCEIL(CutEnd);
					break;
				case NOFIELDS:
					CutStart = CurrentLine+1;
					CutEnd = LastLine;
					break;
				case FIELD1|FIELD2:	
					CutEnd = LOCEIL(CutEnd);
					break;
				case ERROR:
					goto reset;
				default:
					InputError();
					goto reset;
			}
			i=charpos+1;
			if(strlen(buffer+i) > 1){
				strcpy(Search,strtok(buffer+i,"\n"));
				/* Examine search string for ^VD and fixup
                                    if necessary */
				Unescape(Search);
			}
			while(1){
			  ptr = 0;
			  for(j=CutStart;j<=CutEnd;j++)
				if((ptr=strstr(Lines[j],Search)))break;
			  if(ptr){
				STARTLINE(j);
				PrintLine(Lines[j]);
				if(c==INTERACTIVE_SEARCH){
					printf("O.K.? ");
					if(((i=fgetc(stdin))=='Y')||(i=='y')||(i
						=='\n')){
							CurrentLine = j;
							printf("\n");
							break;
					}
					/* else */ CutStart = j+1;
					printf("\n");
				}
				else {  /* Not an interactive search, so quit */
					CurrentLine = j;
					break;
				}
			  }
			  else {
				printf("Not found\n");
				break;
		          }
			}
			break;
					
		case 'R':
		case 'r':
		case INTERACTIVE_REPLACE:
			switch(GetRange(buffer,c)){
				case FIELD1:
					CutEnd = LastLine;
					break;
				case FIELD2:
					CutStart = CurrentLine+1;
					CutEnd = LOCEIL(CutEnd);
					break;
				case NOFIELDS:
					CutStart = CurrentLine+1;
					CutEnd = LastLine;
					break;
				case FIELD1|FIELD2:	
					CutEnd = LOCEIL(CutEnd);
					break;
				case ERROR:
					goto reset;
		        	default:
					InputError();
					goto reset;
			}

			i=charpos+1;
			/* See if a replacement string is provided. If so
                           copy it to the Replace buffer, and adjust cmdlen
                           so that the reset code below will remove all of
                           the current command from the Previous Line buffer.
			   Note that the presence of a replacement string
                           is signalled by the presence of an EOT character.
			*/
			k=0;
			if(buffer[cmdlen-1]==F6){
				while(((j=PrevLine[cmdlen++])!='\n')
                                         &&(j!=F6))Replace[k++]=j;
			                  Replace[k] = '\0';
					  cmdlen++;
				Unescape(Replace);
			}
			if(strlen(buffer+i) > 1){
				strcpy(Search,strtok(buffer+i,"\n"));
				/* If a search string, but not a replacement
                                   string is provided, we use the null string
                                   as replacement. */
				if(!k)Replace[0]='\0';
				Unescape(Search);
			}

			/* How many times does the replace string have
                           the search string embedded in it?. This must
                           be added as an increment to the k below 
			   after each replacement in order
                           to skip over the newly introduced cases of the
                           search string -- otherwise we get an infinite
                           loop. */

			ptr = Replace;
			l = 0;
			while((ptr = strstr(ptr,Search))!=0){
				l++;
				ptr += strlen(Search);
			}

		        for(j=CutStart;j<=CutEnd;j++){
			  k=1;  /* Start out seaching for first occurence */
			  while((ptr=ReplaceString(Lines[j],Search,Replace,k))!=0){
				STARTLINE(j);
				printf("%s",ptr);
				if(c==INTERACTIVE_REPLACE){
					printf("O.K.? ");
					if(((i=fgetc(stdin))=='Y')||(i=='y')||(i
						=='\n')){
							StringFree(Lines[j]);
							Lines[j]=ptr;
							k+=l;
					}
					else k++;
					printf("\n");
				}
				else {  /* Not an interactive search */
					StringFree(Lines[j]);
					Lines[j]=ptr;
					k+=l;
					break;
				}
			  }
			}
			break;	
		case BAD_PARSE:
		default:
			InputError();


	  }   /* End which command letter switch */

	/* Do reset stuff before processing next command here */
	/* Delete command just processed from the PrevLine buffer */
reset:
    for(i=0;i<=lastlen-cmdlen;i++)
	PrevLine[i] = PrevLine[cmdlen+i];
    PrevLine[i]='\0';
    }    /* End if there was anything valid to do. */
    else 
	PrevLine[0]='\0';   /* There was nothing valid to do */
    return rval;
}

/* GetNextCommand: copy the next command from instr into
   buffer. Does not copy beyond the command letter in
   all but r,s,t. Do not copy beyond the F6 in the R  command
   since this requires further processing. After an rRsS we ignore
   ; as delimiter.
   Returns the number of characters copied. The delimiter is always
   included if one is present, and the count includes it too.
 */

int
GetNextCmd(char *instr, char *buffer)
{

	char *ptr;
	int c,d;
	char delimit[5] = "\n";


	c = GetCmdLetter(instr,&d);  /* d is a throw-away */
	switch(c){
		case 'r':
		case 'R':
		case INTERACTIVE_REPLACE:
		case INTERACTIVE_SEARCH:
		case 'S':
		case 's':
			break;
		case 't':
		case 'T':
		case EDIT_LINE:
			delimit[2]=';';
			delimit[3]='\0';
			break;
		case NOP:   
			delimit[2]=';';
			delimit[3]='\0';
			break;
		default:
			delimit[2]=';';
			delimit[3]=c;
			delimit[4] = '\0';
	}
		
	strcpy(buffer, instr);
	ptr = strpbrk(buffer,delimit);
	if(ptr) {
	/* If ptr points to a command letter, skip over any following whitespace
           followed by newline or semicolon */
	   if((*ptr != ';')&&(*ptr != '\n')&&(*ptr != '')){
                ptr++;
		EAT_SPACE(ptr);
		if((*ptr != '\n')&&(*ptr != ';')&&(*ptr != ''))ptr--;
	   }
	   *(ptr+1) = '\0';
	    return strlen(buffer);
	}
	return 0;
}

/* Searches the command and returns command letter or appropriate
   #defined substitute. The index of the command letter in 
   the command string is left in charpos. Returns NOP if no valid
    letter can be found. */

int
GetCmdLetter(char *command, int *charpos)
{
	int state = NUMBER;
	int i;
	char *ptr = command; 

	/* If the entire command is a number, ., or # 
		return an indicator of that */

	EAT_SPACE(ptr);
	if((*ptr == '\n')||(*ptr == ';')||(*ptr == '')) return EDIT_LINE;
	if((*ptr=='.')||(*ptr=='#')){
		ptr++;
		EAT_SPACE(ptr);
		if((*ptr == '\n')||(*ptr == ';'))
		        return EDIT_LINE;
		else state = BEGIN;
	}

	if(state == NUMBER){
	while((ptr - command) < strlen(command))
		if(!isdigit(*ptr)) {
		  switch(*ptr){
			case '\n':  /* These 3 should only occur at the end */
			case ';':
			case '':
				break;
			case '+':
			case '-':
				ptr++;
				continue;
			default:
			state = BEGIN;
			break;
		  }
		break;
		} 
		else ptr++;
	}
	if(state == NUMBER) return EDIT_LINE;

	/* Otherwise, the command letter will be the first valid one
           found, if any */	

	ptr = strpbrk(command,LETTERS);
	if(ptr){ 
	   if(*ptr == '?'){
		/* Eat up white space between ? and command letter */
		ptr++;
		EAT_SPACE(ptr);
		*charpos = ptr - command;
		if((*ptr == 'r') || (*ptr == 'R'))
			return INTERACTIVE_REPLACE;
		if((*ptr == 's') || (*ptr == 'S'))	
			return INTERACTIVE_SEARCH;
		return BAD_PARSE; /* Nothing else is valid */
	   }
	   *charpos = ptr - command;
	   return *ptr;
	}
	return BAD_PARSE;
}

/* GetRange: Parses an expression of the form [n,][m,][k,][l] where n-l
are natural numbers preceded by an optional + or - sign. The value of n
is assigned to the global variable CutStart and m is assigned to CutEnd.
k is assigned to DestLine and l is assigned to Count. Commas are not optional
when needed to show that a given field is empty. No sign can be attached to l.
If a sign is included these are offsets from the current line. Otherwise
they are absolute line indices.  This is natural for the list lines
(l) command. Unfortunately, other commands do not use ranges in consistent
ways. Thus the caller is informed which fields were omitted through
the return value. The return values are given by bitmaps defined at
the top of this file. */ 

int
GetRange(char *command, char letter)
{
	int sign;
	int fields = 0;  /* Bitmap of fields filled */
	int currentfieldbit=1;
	int j;
	char buffer[255];
	char delimit[5] = ",?";
	char *ptr,c;

	/* Copy command to buffer getting rid of whitespace 
	and everything after the command letter */
	ptr = buffer;
	if((letter == INTERACTIVE_REPLACE)||(letter == INTERACTIVE_SEARCH))
		letter = '?';
	while(*command != '\0'){
		if((*command == ' ')||(*command == '\t')){
			command++;
			continue;
		}
		*ptr = *command++;
		if(*ptr++ == letter)break;
	}
	*ptr = '\0';

	ptr = buffer;


/* Set up default values */

	CutStart = FLOOR(CurrentLine - 11);	
	CutEnd =  CEIL(22 + CutStart); 
	/* DestLine has no default value */
	Count = 1;

	if((*ptr == letter)||(strlen(ptr) == 0))return fields;

/* Add command letter to the set of token delimiters */

	     delimit[2] = letter;
	     delimit[3]='\0';

/* Does the Range start with a ,? -- case of omitted first field */
	if(*ptr == ','){
		currentfieldbit = currentfieldbit << 1;
		ptr++;
		if(*ptr == ',')  /* handle ,, at beginning */
			currentfieldbit = currentfieldbit << 1;
	}

	ptr = strtok(buffer,delimit);
	if(strlen(ptr)) fields |= currentfieldbit;

	while(1){
	if(strlen(ptr))  {   /* See if field starts with plus or minus */
		switch(c = *ptr){
			
			case '+':
				if(currentfieldbit==FIELD4){
					InputError();
					return ERROR;
				}
				sign = 1;
				ptr++;
				break;
			case '-':
				if(currentfieldbit==FIELD4){
					InputError();
					return ERROR;
				}
				sign = -1;
				ptr++;
				break;
			default:
				sign = 0;
		}
		j = myatoi(ptr)-1;
		if(sign)
			j = CurrentLine + sign*(j+1);

		switch(currentfieldbit){
			case FIELD1:
				CutStart = CEIL(FLOOR(j));
				break;
			case FIELD2:
				if(j<CutStart){
			/*		InputError(); */
					return ERROR;
				}
				CutEnd = CEIL(j);
				break;
			case FIELD3:
				DestLine = CEIL(FLOOR(j));
				break;
			case FIELD4:
				Count = j+1;
				break;
			default:
				InputError();
			        return ERROR;
		}
	}

	/* Test whether the next char is a comma: this 
           indicates an empty field */

	if(*(ptr+strlen(ptr)+1)==',') currentfieldbit = currentfieldbit << 1;

	ptr = strtok(NULL,delimit); /* Get the next token */

	if(!ptr) return fields; /* Only way out is to run out of tokens */

	currentfieldbit = currentfieldbit << 1;
	if(strlen(ptr))
	      fields |= currentfieldbit; /* Mark current field as filled */
	}
}


/* myatoi: handles translating # as last line + 1 and . as 
current line in line number translations. Otherwise, works the same
as atoi. The addition of 1 is to compensate for the fact that user supplied
line numbers are always converted to indices by subtracting 1. */

int
myatoi(char *astring) { 
	if(astring == NULL) return 0;
	if(strcmp(astring,"#") == 0)return LastLine + 2;
	if(strcmp(astring,".") == 0)return CurrentLine + 1;
	return atoi(astring);
}

void
InputError(void)
{
	fprintf(stderr,"Entry error\n");
	return;
}

/* Replace the nth occurence of old in dest with new. Return pointer to
newly malloced string that replaces dest, or NULL if not possible (e.g.,
not enough memory.) */

char 
*ReplaceString(char *dest,char *old, char *new,int n){

	char *ptr = dest,*newdest;
	int k;

	/* Do we need to do anything at all ? */
	for(k=0;k<n;k++){
	  if((ptr = strstr(ptr,old))==NULL)return NULL;
	  if (k == n-1)break;
	  ptr += strlen(old);
	}

        
	/* malloc a big enough string if possible */
	if((k= strlen(dest) + strlen(new)-strlen(old))>MAX_LINE_LENGTH)
		return NULL;
	if((newdest = (char *)MyMalloc(k + 1))
		==NULL)return NULL;

        /* foo*old*foo... --> foo*new*foo... */

		/* copy first foo block up to beginning of old block */
		for(k=0;k<(ptr-dest);k++)*(newdest+k)=*(dest+k);

		/* tack on the new block */
		*(newdest+k)='\0';
		strcat(newdest,new);

		/* tack on final foo block */
		strcat(newdest,ptr+strlen(old));
	
		return newdest;
}

/* Unescape: searches the string for the sequence ^VD and replaces it with
   ^D. Also handles other ^V escapes */

void
Unescape(char *strng)
{
	int i;
	int j=0;
	int k;
	int flag = 0;

	for(i=0;i<strlen(strng);i++){ /* N.B. i >= j is invariant */
		if(strng[i]==LITERAL){
			flag = 1 - flag;
			if(flag)continue;
		}
		if(flag){
		  if(isupper(strng[i]))
			strng[j++]  = (char)((int)strng[i] - 0x40); /* Yes,
	Professor, I know this is a potential bug */
        	 else strng[j++] = strng[i];
		 flag = 0;
		}
		else strng[j++] = strng[i];
	}	
	strng[j] = '\0';
	return;
}

/* PrintLine: Prints Line to output representing control character
   ^X as X^. Assumes line ends with newline */

void
PrintLine(char *strng)
{
	int j;
	for(j=0;j<strlen(strng)-1;j++)
		if(*(strng+j)<32){
			if(*(strng+j)=='\t')
				printf("\t");
			else printf("%c^",ControlChars[(int)*(strng+j)][1]);
		}
		else printf("%c",*(strng+j));
	printf("\n");
	return;
}
