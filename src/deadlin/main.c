/* main module of the deadlin edlin clone. This file should be
   kept totally system independent. */

/* A minor annoyance: The array of lines is indexed starting from zero
   according to C custom, but the user references lines starting from 1.
   Thus CurrentLine must always be adjusted. */

#include <stdio.h>
#include <stdlib.h>
#include "deadlin.h"
#ifndef _MSDOS
#include "unix.h"
#include <sys/types.h>
#include <fcntl.h>
#endif
#include "dosedit.h"
#include <string.h>

/* Global Variables */

char **Lines;
char *LineBuf;
int Status;     /* Bitmap */
int LinesToAppend;
long BrkVal = DEFAULT_BRK;   /*  malloc area */
long MemAvail;
long MemStart;
int MaxLines = MAX_LINES;

/* LastLine is the index of the last line present in the file. */

int CurrentLine=0,LastLine=-1,CutStart,CutEnd,DestLine,Count;
extern int TabSize;
extern char FileExt[];
extern char FilePath[];
extern char BaseName[];
#ifndef _MSDOS
#define TEMPFILENAMELENGTH 20
char TempFile[] = "/tmp/deadlin.XXXXXX";
FILE *tempfile = NULL;   /* Used for printer output */
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
	fpos_t fpos;
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
					BrkVal = atol(*argv);
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
		fgetpos(istream,&fpos);
		while(ReadLine(istream,LineBuf)){
                        if((ptr = (char *)MyMalloc(strlen(LineBuf)+1))){
				Lines[i] = ptr;
				strcpy(Lines[i++],
						LineBuf);
				fgetpos(istream,&fpos);
				}
/* Note: the istream is left open in case the user needs to append lines */
			else {
			        fsetpos(istream,&fpos);	
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
#ifndef _MSDOS
			  if(FLAGGED(LOG_PRINTER))
				fprintf(tempfile,"%s",LineBuf);
#endif
                                if((ptr= (char *)MyMalloc(strlen(LineBuf)+1))){

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
					fclose(ostream);
					remove(FilePath);
					rename(BaseName,FilePath);
					BaseName[strlen(BaseName)-4]='\0';
					cleanup();
					return 0;
				
				case APPEND_LINES:
					i=1;
					fgetpos(istream,&fpos);
				        UNFLAG(NOT_RESIDENT);
					while(ReadLine(istream,LineBuf)){
						if(i>LinesToAppend){
                                                        FLAG(NOT_RESIDENT); /* File didn't fit */
							break;
						}
                                                if((ptr = (char *)MyMalloc(strlen(LineBuf)+1))){
                                		  Lines[LastLine+i] = ptr;
                                		  strcpy(Lines[LastLine + i++],
                                                                    LineBuf);
						  fgetpos(istream,&fpos);
                                	        }
                                                else {
						 fsetpos(istream,&fpos);
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

	if(4*bytes > 3*MemAvail)return NULL;
	ptr = malloc(bytes);
	if( ptr == (void *)NULL) {   /* Shouldn't happen */
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
        long MemGot;

        /* Try to malloc the requested amount of memory to see if it
           is available. If not, try half as much, ..., until we succeed. */

        MemGot = BrkVal;
        while((ptr = malloc((size_t)MemGot)) == (void *)NULL){
                MemGot = MemGot/2;
                if(MemGot == 0)break;  /* hopeless */
        }
        MemAvail = MemGot;
        free(ptr);

	/* Allocate memory for the global arrays */

	LineBuf = (char *)MyMalloc((MAX_LINE_LENGTH + 1)*sizeof(char));

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
	int fd;
	fd = mkostemp(TempFile,O_APPEND);
	if(fd == -1)return 1;
	tempfile = fdopen(fd,"w+");
	if(tempfile)return 0;
        else return 1;
}

int
flush_printer(void)
{
        char command[TEMPFILENAMELENGTH + sizeof(PRINT_COMMAND) + 5];

        fclose(tempfile);
        sprintf(command,"%s %s",PRINT_COMMAND,TempFile);
        return system(command);
}
#endif

/* Clean up before exit routine */
int
cleanup(void)
{
#ifndef _MSDOS
        if(tempfile)fclose(tempfile);
        remove(TempFile);
#endif
        strcat(BaseName,".$$$");
        remove(BaseName);
	return 0;
}

