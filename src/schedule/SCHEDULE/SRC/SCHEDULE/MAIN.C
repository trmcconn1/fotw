/* main.c: main module of the Mathematics Department scheduling program */

/* Version 1.2, By Terry McConnell, January 1997 */

#ifdef __MSDOS__
#define VERSION "1.2(MS-DOS)"
#else
#define VERSION "1.2(Unix)"
#endif


#include "global.h"
#include "ui.h"
#include <string.h>
#include <time.h>

#ifdef __MSDOS__
#include<dir.h>
#include<dos.h>
#include<io.h>
#include<conio.h>
#include<sys\stat.h>
char DriveLetter[3] = "\0\0";
char StartUpDrive[3];
#else
#include<sys/types.h>
#include<sys/file.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<conio.h>

#endif


char WorkingDirectory[MAXPATH];
char StartUpDirectory[MAXPATH];
char TempDirectory[MAXPATH];
char TempFilePath[MAXPATH];
char CurrentFile[MAXPATH];
char MissingData[LONGNAME] = MISSING_DATA;
char Semester[LONGNAME];
char Version[LONGNAME];
char Date[SHORTNAME];
struct TokenType *Tokens[MAXTOKS];
extern struct JobStruct DefaultJob;



#define INFO  "Schedule: create and maintain department class schedule.\n\
Usage: schedule [ -c -n  -wd <path> -sem <string> -debug -help -version ]\n\
-c: Run in command mode.\n\
-n: Suppress reading .schedulerc\n\
-wd: The following path gives the working directory. (no trailing /)\n\
-sem: The string defines the Semester file extension." 
#define USAGE "schedule [ -c -n -wd <path> -sem <string> -debug -help -version ]\n"

#define MAX_ARG_LEN 80             /* Max length of combined options */

static int find_opt(char *option_string);
#include <stdarg.h>
void myerror(FILE *, char *fmt, ...);
void fatal(FILE *, char *fmt, ...);
char ProgramName[MAXPATH];
extern int shell(FILE *, FILE *, FILE *, FILE *, char *errmsg, void *data);
extern int ParseRcFile(void);
extern void init_defaults();

#ifdef __MSDOS__
extern int menu();
#endif

FILE *errorstream;
FILE *ostream;
FILE *holdstream;
FILE *istream;

int debug = 0;
static char buffer[BUFSIZ];


int main(int argc, char **argv)
{
	int c=-1;                /* numeric option code */
	int cmdshell = 0;
	int source = 1;
#ifdef __MSDOS__
        unsigned driveno;
        unsigned ndrives;
#endif
	int i;
	time_t time_now;
	char *info = INFO;
	char *ptr;
	char opsep = '-';      /* Standard for Unix */
	char argstr[MAX_ARG_LEN];

	strcpy(Version,VERSION);
	strcpy(CurrentFile,"None");

	holdstream = ostream = stdout;


/* Until reset, send error messages on stderr */
	errorstream = stderr;


/* Allocate token memory for parsers that use them. */

	for(i=0;i<MAXTOKS;i++)
		if((Tokens[i]=(struct TokenType *)malloc(sizeof(struct TokenType)))==NULL)
			fatal(errorstream,"Unable to allocate token memory\n");

/* INITIALIZE DEFAULT DATA TYPES */
	

/* initialize remaining default data structures */
	init_defaults();

/* Put the current date in Date */

	time(&time_now);
	strftime(Date,SHORTNAME,"%m/%d %Y",localtime(&time_now));

#ifdef __MSDOS__
/* Record current directory so it can be restored */
	getcwd(StartUpDirectory,MAXPATH);
#else
	getwd(StartUpDirectory);
#endif

/* Set default WorkingDirectory and Semester */
	SAFECPY(WorkingDirectory,StartUpDirectory,MAXPATH);
	strcpy(Semester,"f96");

#ifdef __MSDOS__
	/* Get startup default drive so it can be restored */
	_splitpath(StartUpDirectory,StartUpDrive,NULL,NULL,NULL);
        _dos_getdrive(&driveno);
#endif

	/* Set default temp directory */
	strcpy(TempDirectory,"");

	/* Open and read in Global Setup from rc file */

/* Do a quick and dirty parse of the command line to see if the
option -n is present. More complete command line parsing is done below */

	for(i=0;i<argc;i++)
		if((strcmp(argv[i],"-n")==0)||
			(strcmp(argv[i],"-nc")==0)||
			(strcmp(argv[i],"-cn")==0)) source = 0;

	if(argc>1){
	if(strcmp(argv[1],"-version")==0) source = 0;
	if(strcmp(argv[1],"-help")==0) source = 0;
	}

	  if(source){
#ifdef __MSDOS__
	  if((istream = fopen(searchpath("schedule.rc"),"r"))!=NULL){
		SAFECPY(CurrentFile,searchpath("schedule.rc"),MAXPATH);
		Dialogue(istream,ostream,errorstream,ostream,"",
		NULL,&shell);
		}		
	   else myerror(errorstream,"Unable to open %s\n",
		buffer);
#else
/* In Unix, we assume the rc-file is in the user's home directory. */
	  strcpy(buffer,getenv("HOME"));
	  strcat(buffer,"/.schedulerc");
	  if((istream = fopen(buffer,"r"))!=NULL){
		SAFECPY(CurrentFile,buffer,MAXPATH);
		Dialogue(istream,ostream,errorstream,ostream,"",
		NULL,&shell);
	}	
	  else myerror(errorstream,"Unable to open %s\n",
		buffer);

#endif

	if(istream != NULL)
	fclose(istream);
 

	}


	istream = stdin;

/* See if the user has supplied a prompt. Use default otherwise */

        if(GetShell("PROMPT") == NULL ) set("Prompt","Schedule>",3,NULL);



/* Program name is available in ProgramName */

	SAFECPY(ProgramName,argv[0],MAXPATH);


/* Process Command Line Options */

if(--argc > 0){  /* Anything on command line ? */
		++argv;
		while((argc > 0)&&((*argv[0]) == opsep)){ /* loop while 
							there are flags */
			argstr[0]=opsep;
			argstr[1]='\0';
			strcat(argstr,++*argv); /* glob argstr*/
			argc--;
			while((argc>0)&&(strlen(*argv)<3)&& (strlen(*(argv+1))<3)&&
				((*(argv+1))[0]==opsep)&&((*(argv+1))[1]!=opsep)){
				++argv;
				argc--;
				strcat(argstr,++*argv);
			}
			++argv; /* ok, consecutive flags now globbed into one*/
		       c = find_opt(argstr+1); /* look up flag code */
		switch(c){

/* 
   Make sure to increment argv and decrement argc 
   each time. The code for handling the arguments for a given flag must
   go in the case for that flag. 

*/

			case -1: break;  /* default, e.g no options */

			case 0: printf("%s\n",info);
				return 0;
			case 1: /* -version */
				printf("%s\n",VERSION);
				return 0;
			case 2: /* -c : command shell */
				cmdshell = 1;
				break;
			case 3: strcpy(WorkingDirectory,*argv++);
				argc--;
				break;
			case 4: strcpy(Semester,*argv++);
				argc--;
				break;
			case 5: debug = 1;
				break;
			case -2: ;  /* barf (don't use in your code) */
			default:
				myerror(stderr,"%s: illegal option\n",argstr);
				myerror(stderr,USAGE);
				return 1;
			} /* End flag switch */
	} /* End flag grabbing while loop */
} /* end if: any args ? */

/*   argc now gives a count of remaining arguments on command line,
	and argv points at the first one */

	if(argc > 0) myerror(stderr," Too many command line arguments.\n");

#ifdef __MSDOS__
/* Set appropriate default drive (Dos only) 
 Let's see if the working directory path supplies a drive letter */

	_splitpath(WorkingDirectory,DriveLetter,NULL,NULL,NULL);
	if(DriveLetter[0]!='\0')
		system(DriveLetter);

	/* Else drive is the same as it was when pgm started */
/* change to working directory */

	strcpy(buffer,"cd ");
	strcat(buffer,WorkingDirectory);
	system(buffer);
#else
	if(chdir(WorkingDirectory)!=0){
		myerror(errorstream,"Unable to cd to working directory\n");
		strcpy(WorkingDirectory,StartUpDirectory);
	}
	
#endif

/* Create tempfile */ 
/* First, see if the user has specified a non-default temp directory */
	if((ptr = GetShell("TEMPDIR"))!= NULL)
		SAFECPY(TempDirectory,ptr,MAXPATH);

/* Generate a temporary file name */
	tmpnam(buffer);
	strcpy(TempFilePath,TempDirectory);
	strcat(TempFilePath,buffer);

/* Create a file with this name. It may be opened later as needed */

#ifdef __MSDOS__
	if(creat(TempFilePath,S_IREAD | S_IWRITE) == -1)
		myerror(errorstream,"Unable to creat %s, errno = %d\n",
			TempFilePath,errno);

#else

	if(creat(TempFilePath,0600) == -1)
                myerror(errorstream,"Unable to creat %s, errno = %d\n",
                        TempFilePath,errno);
#endif

/* Command-line interface ? */
	if(cmdshell){
		strcpy(CurrentFile,"stdin");
                fprintf(ostream,"MATHEMATICS DEPARTMENT SCHEDULING PROGRAM -- Version %s --\n",VERSION);
		Dialogue(istream,ostream,errorstream,ostream,"Interactive Command Shell",
		NULL,&shell); 
	}

else {
/* Reached unless user chooses to exit program from shell */
#ifdef __MSDOS__
      menu();  
#endif
}


/* Clean up and exit */
         
          remove(TempFilePath);         

#ifdef __MSDOS__

          _dos_setdrive(driveno,&ndrives);  /* Restore default drive */
          chdir(StartUpDirectory);
 /*         system("cls");     */
#else
      
	  chdir(StartUpDirectory);
	  system("clear");
	  system("echo \"\n\n\""); 
#endif

	return 0;

}

/*  find_opt: return a unique small integer for each possible option string */
/*  There should be a case in the switch statement in main to handle each. */
/*  -2 reserved for arg not found -1 reserved for no options */

/* N.B. word doesn't contain the leading hyphen */

int find_opt(char *word)
{
	if(strcmp(word,"help")==0)return 0;
	if(strcmp(word,"version")==0)return 1;
	if(strcmp(word,"debug")==0)return 5;
	if(strcmp(word,"c") == 0) return 2;
	if(strcmp(word,"nc")==0) return 2;
	if(strcmp(word,"cn")==0) return 2;
	if(strcmp(word,"wd") == 0) return 3;
	if(strcmp(word,"sem") == 0) return 4;
	if(strcmp(word,"n")==0)return -1; /* special case: n gets processed
	      before the usual option processing section, so we don't treat
	      it there. */

/* arg not found */
	return -2;
}

/* myerror: generic error reporting routine */

void myerror(FILE *stream,char *fmt, ...)
{
	va_list args;

	va_start(args,fmt);
	fprintf(stream,"%s line %d: ",CurrentFile,line);
	vfprintf(stream,fmt,args);  
	va_end(args);
	return;
}

/* fatal: generic error reporting routine */

void fatal(FILE *stream,char *fmt, ...)
{
	va_list args;
        int i;

/* clear the screen by printing many blank lines */
        for(i=0;i<26;i++)
                fprintf(stream,"%80c",'\n');

#ifdef __MSDOS__
        if(GetShell("MENU") != NULL){

                _setcursortype(_NORMALCURSOR);
        }
#endif
	va_start(args,fmt);
	fprintf(stream,"%s: Fatal Error.\n",ProgramName);
	fprintf(stream,"%s line %d: ",CurrentFile,line);
	vfprintf(stream,fmt,args);
	fprintf(stream,"\n");
	va_end(args);

/* Clean up and exit */
         
          remove(TempFilePath);         

#ifdef __MSDOS__

	  system(StartUpDrive);  /* Restore default drive */
	  strcpy(buffer,"cd ");
	  strcat(buffer,StartUpDirectory);
	  system(buffer);   /* Restore startup directory */ 
 
#endif

	exit(1);
}
