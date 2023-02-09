/* main module of the deadlin edlin clone. This file should be
   kept totally system independent. */

/* A minor annoyance: The array of lines is indexed starting from zero
   according to C custom, but the user references lines starting from 1.
   Thus CurrentLine must always be adjusted. */

#ifndef CPM68
#include <stdio.h>
#include <stdlib.h>
#else
#include <cpm.h>
#endif
#include "deadlin.h"

#ifndef _MSDOS
#include "unix.h"
#endif

#include "dosedit.h"
#ifndef CPM68
#include <string.h>
#endif


#ifdef CPM68
#define USE_FSEEK_FTELL 1
#endif
/* Global Variables */

char **Lines;
char *LineBuf;
int Status;     /* Bitmap */
int LinesToAppend;
#ifdef CPM86
unsigned int BrkVal = DEFAULT_BRK;   /*  malloc area */
unsigned int MemAvail;
unsigned int MemStart;
#else
long BrkVal = DEFAULT_BRK;   /*  malloc area */
long MemAvail;
long MemStart;
#endif
int MaxLines = MAX_LINES;

/* LastLine is the index of the last line present in the file. */

int CurrentLine=0,LastLine=-1,CutStart,CutEnd,DestLine,Count;
extern int TabSize;
extern char FileExt[];
extern char FilePath[];
extern char BaseName[];
#ifndef CPM68
#ifndef _MSDOS
char TempFile[L_tmpnam];
FILE *tempfile;   /* Used for printer output */
#endif
#endif

/* Functions */

void *MyMalloc(int);
long StringFree(void *);
extern int Parse(FILE *);
extern int ReadLine(FILE *,char *);
extern int OpenFile(char *,FILE **);  /* System dependent */ 
extern int init_tty(int);
extern int cleanup(void);
int init_mem(void);

int main(int argc, char **argv)
{
	FILE *istream = stdin;
	FILE *ostream ; 
#ifndef USE_FSEEK_FTELL
	fpos_t fpos;
#else
        LONG fpos;        
#endif
	char *ptr;
	int i=0;


	/* Process Command Line */

	if(argc > 1) {
		while(argc > 1)
		  if((*++argv)[0]==OPSEP)
			switch((*argv)[1]){
				case 'b':
				case 'B':
					FLAG(IGNORE_EOF);
					argc--;
					break;
				case 'T':
				case 't':
					++argv;
					TabSize = atoi(*argv);
					argc = argc -2;
					break;
				case 'm':
				case 'M':
					++argv;
#ifdef CPM68
                                        BrkVal = atoi(*argv); /* BrkVal is unsigned, atoi is signed */
#else
					BrkVal = atol(*argv);
#endif
					argc = argc - 2;
					break;
				default:
					fprintf(stderr,"Invalid Parameter\n");
					exit(1);
			}
		else {
			if(FLAGGED(INPUT_SET)){
				fprintf(stderr,"Invalid Parameter\n");
				exit(1);
			}
			FLAG(INPUT_SET);	
	        	if(!OpenFile(*argv,&istream)){
				FLAG(NEW_FILE);
				fprintf(stderr,"Error Opening %s\n",FilePath);
			}
			argc--;
		}
	}
	else {
		fprintf(stderr,"File name must be specified\n");
		exit(1);
	}
	if(!FLAGGED(INPUT_SET)){
		fprintf(stderr,"File name must be specified\n");
		exit(1);
	}

/* Initialize input mode of tty: basically cbreak mode */

	init_tty(0);

/* Attempt to satisfy memory requested, and malloc global arrays */

	init_mem();
	MemStart = MemAvail;

	if(FLAGGED(NEW_FILE)) 
		fprintf(stderr,"New File\n");	

	else {
#ifndef USE_FSEEK_FTELL
		fgetpos(istream,&fpos);
#else
                fpos = ftell(istream);
#endif
		while(ReadLine(istream,LineBuf)){
                        if(ptr = (char *)MyMalloc(strlen(LineBuf)+1)){
				Lines[i] = ptr;
				strcpy(Lines[i++],
						LineBuf);
                                             
#ifndef USE_FSEEK_FTELL
				fgetpos(istream,&fpos);
#else
                                fpos = ftell(istream);
#endif
				}
/* Note: the istream is left open in case the user needs to append lines */
			else {
#ifndef USE_FSEEK_FTELL
                                fgetpos(istream,&fpos);
#else
                                fpos = ftell(istream);
#endif
				FLAG(NOT_RESIDENT); /* File didn't fit */
				break;
			}
		}
		if(!FLAGGED(NOT_RESIDENT))
		      fprintf(stderr,"End of Input File\n");
		LastLine = i-1;	
	}

	/* Main Loop */

	while(TRUE)
	{
		if(FLAGGED(INPUT_MODE)) {
			STARTLINE(CurrentLine)
			while(ReadLine(stdin,LineBuf)){
			  if(LineBuf[0]==F6)break;
			/* Maybe the user really wanted to begin the
                           line with control D and put a ^VD sequence in: */
			  if((LineBuf[0]==LITERAL)&&(LineBuf[1]=='D')){
				LineBuf[0]=F6;
				LineBuf[1]='\0';
				strcat(LineBuf,LineBuf+2);
			  }
#ifndef CPM68
#ifndef _MSDOS
			  if(FLAGGED(LOG_PRINTER))
				fprintf(tempfile,"%s",LineBuf);
#endif
#endif
                                if(ptr= (char *)MyMalloc(strlen(LineBuf)+1)){

			/* Move Lines below current line down to make room */

					for(i=LastLine++;i>=CurrentLine;i--)
						Lines[i+1] = Lines[i];					

					Lines[CurrentLine] = ptr;
					strcpy(Lines[CurrentLine++],
						LineBuf);
				}
				else {
					FLAG(NOT_RESIDENT); /* Line didn't fit */
					break;
				}
			STARTLINE(CurrentLine)
			}  /* end while: no more input */
		UNFLAG(INPUT_MODE);
		}
		else {  /* Command Mode */

			switch(Parse(stdin)){
				case END_EDIT:

				/* Backup existing file, if necessary */

					if(!FLAGGED(NEW_FILE)){
						rewind(istream);
						strcat(BaseName,".bak");
						ostream = fopen(BaseName,"w");
#ifdef DTZDEBUG
fprintf(stderr,"DtZdebug: Opened an %s file for writing\n",BaseName);
#endif
						while(ReadLine(istream,LineBuf))
							fprintf(ostream,"%s",LineBuf);
						fclose(istream);
						fclose(ostream);
					/* Remove .bak extension */
				                BaseName[strlen(BaseName)-4]
							= '\0';	
					}

				/* Write lines in memory to file */
					
					/* A file by this name will exist only
                                           if the user has used the w command */

					strcat(BaseName,".$$$");
					ostream = fopen(BaseName,"a");
#ifdef DTZDEBUG
/* DtZ */
fprintf(stderr,"DtZdebug: Opened an %s\n",BaseName);
#endif
					for(CurrentLine=0; CurrentLine <= LastLine;
						CurrentLine++)
						fprintf(ostream,"%s",Lines[CurrentLine]);
				/* edlin insists that a file end with a CR/LF and
                                   a ^Z. In Unix we just put a newline, and
                                   the ^Z bit makes no sense, so we omit it. */
						if((LastLine>=0)&&(*(Lines[LastLine] +
							strlen(Lines[LastLine])-1)
							!= '\n'))
							fprintf(ostream,"\n");
#ifdef _MSDOS
					fprintf(ostream,"");
#endif
#ifdef CPM68
					fprintf(ostream,"");    
#endif
					fclose(ostream);
					remove(FilePath);
					rename(BaseName,FilePath);
/* DtZ */
#ifdef DTZDEBUG
fprintf(stderr,"DtZdebug :and renamed %s => %s \n",BaseName,FilePath);
#endif
					BaseName[strlen(BaseName)-4]='\0';
					cleanup();
					return 0;
				
				case APPEND_LINES:
					i=1;
#ifndef USE_FSEEK_FTELL
                                        fgetpos(istream,&fpos);
#else
                                        fpos = ftell(istream);
#endif
				        UNFLAG(NOT_RESIDENT);
					while(ReadLine(istream,LineBuf)){
						if(i>LinesToAppend){
                                                        FLAG(NOT_RESIDENT); /* File didn't fit */
							break;
						}
                                                if(ptr = (char *)MyMalloc(strlen(LineBuf)+1)){
                                		  Lines[LastLine+i] = ptr;
                                		  strcpy(Lines[LastLine + i++],
                                                                    LineBuf);
#ifndef USE_FSEEK_FTELL
                                                  fgetpos(istream,&fpos);
#else
                                                  fpos = ftell(istream);
#endif
                                	        }
                                                else {
#ifndef USE_FSEEK_FTELL
                                                 fgetpos(istream,&fpos);
#else
                                                 fpos = ftell(istream);
#endif

                                                 FLAG(NOT_RESIDENT); /* File didn't fit */
                                                 break;
                                                }
                                        }
                                        if(!FLAGGED(NOT_RESIDENT))
                                           fprintf(stderr,"End of Input File\n");
                                        LastLine += i-1;
					break;
				
				case 0: /* Nothing to do */
				default:
					break;
			}
		} /* Command Mode */
	} /* Main Loop */
}

/* MyMalloc: Allocate memory. Same as usual malloc, except that it
   refuses to allocate more than 75% of the memory available to the
   process. For portability we don't attempt to actually get and limit
   available resources. */

void *MyMalloc(int bytes)
{
	void *ptr;

#ifndef CPM68
        if ( 4*bytes > 3*MemAvail)  return NULL;
#endif
	ptr = malloc(bytes);
	if( ptr == (void *)NULL) {   /* Shouldn't happen */
/* Let it happen in CPM68 */
		fprintf(stderr,"deadlin: Could not alloc %d bytes\n",bytes);
		exit(1);
	}
	MemAvail -= bytes;
	return ptr;
}

/* StringFree: free memory and update MemAvail. The only reason we can get
   away with this is because the only things ever freed are strings! 
   Otherwise there seems to be no reliable way to keep track of the amount
   of memory freed. Why, oh why, can't free return the amount of memory
    freed ?? */

long
StringFree(void *ptr)
{
	long i;
	i = (long)strlen(ptr);
	MemAvail += i+1;
	free(ptr);
	return i;
}

/* Determines whether the requested amount of memory is available.
   If not, resets the request to a safe value. Then allocates as much
   memory as possible for the global array of line pointers.

   This is very crude.
    */

int
init_mem(void)
{
        void *ptr;
#ifndef CPM68
        long MemGot;
#else
        WORD MemGot;
#endif

        /* Try to malloc the requested amount of memory to see if it
           is available. If not, try half as much, ..., until we succeed. */

        MemGot = BrkVal;
#ifndef CPM68
        while((ptr = malloc((size_t)MemGot)) == (void *)NULL){
#else
        while( (ptr = malloc(MemGot)) == NULL ){
#endif

                MemGot = MemGot/2;
                if(MemGot == 0)break;  /* hopeless */
        }
        MemAvail = MemGot;
        free(ptr);

	/* Allocate memory for the global arrays */

#ifndef CPM68
	LineBuf = (char *)MyMalloc((MAX_LINE_LENGTH + 1)*sizeof(char));
#else
	LineBuf = (char *)MyMalloc(  ((int) ((MAX_LINE_LENGTH + 1)*sizeof(char)) ) );
#endif
	/* If this is null, better bail */
	if(LineBuf==NULL){
		fprintf(stderr,"Fatal: unable to allocate enough memory to run!\n" );
		exit(1);
	}

	MemGot = MAX_LINES*sizeof(char *); /* Re-using local variable */

	while((Lines = MyMalloc(MemGot))==NULL){
		MemGot = MemGot/2;
		if(MemGot == 0)break;
	}
	MaxLines = MemGot/sizeof(char *);

	/* We can assume -m was used, so it's ok to print a message not
           present in the original edlin */
	if(BrkVal != DEFAULT_BRK)
		fprintf(stderr,"Requested %ld bytes, Available = 3/4 of %ld bytes, MaxLines = %d\n",
			BrkVal,MemAvail,MaxLines);	

        return TRUE;
}


#ifndef _MSDOS
/* Create a temp file to hold output to be sent to printer by
   flush_printer */

int
init_printer(void)
{
#ifndef CPM68
        remove(TempFile);
        strcpy(TempFile,tmpnam(NULL));
        if((tempfile = fopen(TempFile,"wb+"))==NULL)
                return 1;
        else
         return 0;
#else
 return 1;
#endif
}

int
flush_printer(void)
{
#ifndef CPM68
        char command[L_tmpnam + sizeof(PRINT_COMMAND) + 5];

        fclose(tempfile);
        sprintf(command,"%s %s",PRINT_COMMAND,TempFile);
        system(command);
        return 0;
#else
 return 1;
#endif
}
#endif
/* Clean up before exit routine */
int
cleanup(void)
{
#ifndef CPM68
#ifndef _MSDOS
        if(tempfile)fclose(tempfile);
        remove(TempFile);
#endif
#endif
        strcat(BaseName,".$$$");
        remove(BaseName);
}

