/* newsmstr.c: main module of the  program to initialize data
   for  a new semester in a reasonable way. */

/* Version 1.0, By Terry McConnell, June 1996 */


/* Note: Much of the first half of main is probably irrelevant, since it was
   copied verbatim from the main schedule program. It manages the working
   directory and sets up a bunch of stuff which might be useful some day */

/* Basically, this program just conducts a dialog with the user, copies
    and updates certain files accordingly. It's the sort of thing that
    should be done with a shell script. Unfortunately, that won't help
    us with DOS, which is where the program must eventually live :-(
    The prospect of doing all of this in a DOS batch file is upleasant
    in the extreme. */

#ifdef __MSDOS__
#define VERSION "1.0(MS-DOS)"
#define PATHSEP "\\"
#else
#define PATHSEP "/"
#define VERSION "1.0(Unix)"
#endif



#include "global.h"
#include "shell.h"
#include <string.h>
#include <time.h>

#ifdef __MSDOS__
#include<dir.h>
#include<dirent.h>
#include<dos.h>
#include<io.h>
#include<sys\stat.h>
char DriveLetter[3] = "\0\0";
char StartUpDrive[3];
#else
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
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

char buffer[BUFSIZ];
char buffer2[BUFSIZ];
char AcademicYear[SHORTNAME];
char SemesterName[SHORTNAME];
char TemplateName[SHORTNAME];
char SemStart[SHORTNAME];
char EndClasses[SHORTNAME];
char EndSemester[SHORTNAME];
char NoData[SHORTNAME];


#define INFO  "Newsmstr: get started creating a new schedule from scratch.\n\
Usage: newsmstr [ -q -b -wd <path>  -help -version ]\n\
-q: Quit printing the reminders.\n\
-b: Create only a blank schedule \n\
-wd: The following path gives the working directory. (no trailing /)\n"
#define USAGE "newsmstr [ -q  -b -wd <path>  -help -version ]\n"

#define MAX_ARG_LEN 80             /* Max length of combined options */
#define SCRIPT_DIRECTORY "data"

static int find_opt(char *option_string);
#include <stdarg.h>
void myerror(FILE *, char *fmt, ...);
void fatal(FILE *, char *fmt, ...);
char ProgramName[MAXPATH];
extern int shell(FILE *, FILE *, FILE *, FILE *, char *errmsg, void *data);

FILE *errorstream;
FILE *ostream;
FILE *holdstream;
FILE *istream;

int quiet = 0;        /* Assume we run in Verbose mode */

/* Structure for exchanging data with the GetUser dialog. Cf dialogs.c */
/* Dialogs are run by the dialogue routine defined in ui.c */

struct UserDialogData {
int verbose;
char longmsg[BUFSIZ];
char shortmsg[BUFSIZ];
char usrinpt[BUFSIZ];
char prompt[LONGNAME];
char quitchar;
int (*test)(char *);   /* function to test validity of user input */
};

/* structure for exchanging data with the holidays dialog */

struct HolidayData {
int number;
char *name[100];
char *starts[100];
char *ends[100];
} HolidayData;


/* These routines are defined in dialogs.c */

extern int GetUser(FILE *istream, FILE *ostream, FILE *, FILE *,char *errmsg,
        void *data);
extern int HolidayDialog(FILE *istream, FILE *ostream, FILE *, FILE *,char *errmsg, void *);


int CopyFile(char *source,char *dest);
int CopyTimesFile(char *, char *);
int CopyScheduleFile(char *, char *);

/* These functions test validity of user input for the two dialogs */

int SemesterExists(char *);
int VerifyDate(char *);

int main(int argc, char **argv)
{
	int c=-1;                /* numeric option code */
	int i;
	int script_only = 0;  /* flag to say we only want to make blank sched */
	time_t time_now;
	char *info = INFO;
	char *ptr;
	char opsep = '-';      /* Standard for Unix */
	char argstr[MAX_ARG_LEN];
	static struct UserDialogData DialogData;

	strcpy(Version,VERSION);
	strcpy(CurrentFile,"None");

	holdstream = ostream = stdout;


/* Until reset, send error messages on stderr */
	errorstream = stderr;


/* Allocate token memory for parsers that use them. */

for(i=0;i<MAXTOKS;i++)
if((Tokens[i]=(struct TokenType *)malloc(sizeof(struct TokenType)))==NULL)
		fatal(errorstream,"Unable to allocate token memory\n");

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
	strcpy(Semester,"f97");

#ifdef __MSDOS__
	/* Get startup default drive so it can be restored */
	_splitpath(StartUpDirectory,StartUpDrive,NULL,NULL,NULL);
#endif

	/* Set default temp directory */
	strcpy(TempDirectory,"");

	/* Open and read in Global Setup from rc file */


	  
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


	istream = stdin;



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
			case 2: /* -q : quiet!!! */
				quiet = 1;
				break;
			case 3: strcpy(WorkingDirectory,*argv++);
				argc--;
				break;
			case 4: 
				script_only = 1;
				quiet = 1;
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

/***********************************/
/* Meat of the program begins here */
/********************************/


/* Ask user for the last two digits of the academic year */

#define QUITCHAR 'q'

#define MSG1 "\n\n\nThis program begins the process of creating a new schedule.\n\
It will create a directory to hold the new semester's data files.\n\
Then it will copy the data from an example semester you name \n\
to the newly created directory. Some of these will be edited using\n\
information you supply at the prompts. Although the program\n\
will attempt to make reasonable choices, you will have to edit\n\
all the data files and the schedule file to finish the process.\n\n\
At any stage you may enter q to abort.  \n\n\n"

dialog1:
DialogData.test = NULL;
strcpy(DialogData.longmsg,MSG1);
strcpy(DialogData.shortmsg,"Enter last two digits of the current academic year.\nFor example enter 97 for Fall 96 and for Spring 97.\n");
strcpy(DialogData.prompt,"newsmstr> ");
DialogData.quitchar = QUITCHAR;
if(!quiet)DialogData.verbose = 1;
else DialogData.verbose = 0;


			
	if(Dialogue(istream,ostream,errorstream,ostream,"",
		&DialogData,&GetUser))return 1; 

/* DialogData.usrinpt now contains input from user */

	strcpy(AcademicYear,DialogData.usrinpt);
	DialogData.verbose = 0;
	strcpy(DialogData.shortmsg,"");	
	if(!quiet){fprintf(ostream,"\nIs this correct?[yn]\n");
		fprintf(ostream,"Academic year = %s\n",AcademicYear);
		Dialogue(istream,ostream,errorstream,ostream,"",
			&DialogData,&GetUser);
		if(DialogData.usrinpt[0]=='n') goto dialog1;
	}

#define MSG2 "\n\nThe program will now create a directory to hold the data for\
 the new semster.\n\
You will be asked to supply a name for the new semester, which will also\n\
be used for the name of this directory. Use a 3 letter designation,\n\
e.g f97, in which case the directory created will be the subdirectory \n\
of the current directory named data/f97.\n\n\n"

dialog2:
strcpy(DialogData.longmsg,MSG2);
if(!script_only)
  strcpy(DialogData.shortmsg,"Enter the name of the new semester (e.g f97.)\n");
else
  strcpy(DialogData.shortmsg,"Enter a name for the schedule file (e.g blank) \n");
if(!quiet)DialogData.verbose = 1;
else DialogData.verbose = 0;


			
	if(Dialogue(istream,ostream,errorstream,ostream,"",
		&DialogData,&GetUser))return 1; 

/* DialogData.usrinpt now contains input from user */

	strcpy(SemesterName,DialogData.usrinpt);
	DialogData.verbose = 0;
	strcpy(DialogData.shortmsg,"");	
	if(!quiet){fprintf(ostream,"\nIs this correct?[yn]\n");
		fprintf(ostream,"Semester Name  = %s\n",SemesterName);
		Dialogue(istream,ostream,errorstream,ostream,"",
			&DialogData,&GetUser);
		if(DialogData.usrinpt[0]=='n') goto dialog2;
	}

/* Create the directory. We'll do it the lazy way (using system) since
   this must work in both MSDOS and Unix. */

if(!script_only){
	strcpy(buffer,"mkdir ");
	strcat(buffer,WorkingDirectory);
        strcat(buffer,PATHSEP);
        strcat(buffer,"data");
        strcat(buffer,PATHSEP);
	strcat(buffer,SemesterName);
	system(buffer);
	fprintf(ostream,"Executed command %s\n",buffer);
}

#define MSG3 "\n\nData will be copied from an existing semester to the new one.\n\
You will be asked to supply the name of an existing semester to use. You should\n\
use the most recent semester of the same type as this one ( fall, spring, etc.)\n\
If the program is unable to use your choice, it will complain.\n\n\n"

dialog3:
DialogData.test = &SemesterExists;
strcpy(DialogData.longmsg,MSG3);
strcpy(DialogData.shortmsg,"Enter the name of the semester to use as template\n");
if(!quiet)DialogData.verbose = 1;
else DialogData.verbose = 0;


			
	if(Dialogue(istream,ostream,errorstream,ostream,"",
		&DialogData,&GetUser))return 1; 


	strcpy(TemplateName,DialogData.usrinpt);
	DialogData.verbose = 0;
	DialogData.test = NULL;
	strcpy(DialogData.shortmsg,"");	
	if(!quiet){fprintf(ostream,"\nIs this correct?[yn]\n");
		fprintf(ostream,"Template Name  = %s\n",TemplateName);
		Dialogue(istream,ostream,errorstream,ostream,"",
			&DialogData,&GetUser);
		if(DialogData.usrinpt[0]=='n') goto dialog3;
	}

if(script_only) goto dialog7; 

#define MSG4 "\nThe program will now copy the people, rooms, and jobs data \n\
files from the example directory to the new directory. REMEMBER TO CAREFULLY\n\
READ AND EDIT THESE FILES TO BRING THEM UP TO DATE.\n\n"

	if(!quiet) fprintf(ostream,"%s", MSG4);

	strcpy(buffer,WorkingDirectory);
        strcat(buffer,PATHSEP);
        strcat(buffer,"data");
        strcat(buffer,PATHSEP);
	strcat(buffer,TemplateName);
        strcat(buffer,PATHSEP);
        strcat(buffer,"people");
	strcpy(buffer2,WorkingDirectory);
         strcat(buffer2,PATHSEP);
        strcat(buffer2,"data");
         strcat(buffer2,PATHSEP);
	strcat(buffer2,SemesterName);
         strcat(buffer2,PATHSEP);
        strcat(buffer2,"people");

	fprintf(ostream,"Copying %s ---> %s\n",buffer,buffer2);
	if(CopyFile(buffer,buffer2))
		myerror(errorstream,"Copy Failed\n");
	else fprintf(ostream,"Copy succeeded\n");

	strcpy(buffer,WorkingDirectory);
         strcat(buffer,PATHSEP);
        strcat(buffer,"data");
         strcat(buffer,PATHSEP);
	strcat(buffer,TemplateName);
         strcat(buffer,PATHSEP);
        strcat(buffer,"jobs");
	strcpy(buffer2,WorkingDirectory);
         strcat(buffer2,PATHSEP);
        strcat(buffer2,"data");
         strcat(buffer2,PATHSEP);
	strcat(buffer2,SemesterName);
         strcat(buffer2,PATHSEP);
        strcat(buffer2,"jobs");

	fprintf(ostream,"Copying %s ---> %s\n",buffer,buffer2);
	if(CopyFile(buffer,buffer2))
		myerror(errorstream,"Copy Failed\n");
	else fprintf(ostream,"Copy succeeded\n");

	strcpy(buffer,WorkingDirectory);
         strcat(buffer,PATHSEP);
        strcat(buffer,"data");
         strcat(buffer,PATHSEP);
	strcat(buffer,TemplateName);
         strcat(buffer,PATHSEP);
        strcat(buffer,"rooms");
	strcpy(buffer2,WorkingDirectory);
         strcat(buffer2,PATHSEP);
        strcat(buffer2,"data");
         strcat(buffer2,PATHSEP);
	strcat(buffer2,SemesterName);
         strcat(buffer2,PATHSEP);
        strcat(buffer2,"rooms");

	fprintf(ostream,"Copying %s ---> %s\n",buffer,buffer2);
	if(CopyFile(buffer,buffer2))
		myerror(errorstream,"Copy Failed\n");
	else fprintf(ostream,"Copy succeeded\n");

#define MSG5 "\n\nThe remaining data file, the times file, will now be created.\n\
You will be prompted to supply a number of dates. The program will\n\
accept a number of formats for dates, but  you might as well stick with the \n\
standard month/day/year format. For example, when you are asked for the \n\
start of a semester, you could type 8/25/96, indicating the Sunday before \n\
the first day of classes. (Technically, the program specifies points in time \n\
by means of both a date and a time of day. To keep things simple, this setup \n\
program will always assume the time of day is midnight. Thus, when you typed\n\
8/25/96 above, you were indicating that the semester begins at midnight on\n\
the Sunday before the start of classes.)\n\n\n"

	if(!quiet)fprintf(ostream,"%s",MSG5); 

dialog4:
DialogData.test = &VerifyDate;
strcpy(DialogData.longmsg,"");
strcpy(DialogData.shortmsg,"Enter the Date of the start of the semester\n");
if(!quiet)DialogData.verbose = 1;
else DialogData.verbose = 0;


			
	if(Dialogue(istream,ostream,errorstream,ostream,"",
		&DialogData,&GetUser))return 1; 


	strcpy(SemStart,DialogData.usrinpt);
	DialogData.verbose = 0;
	DialogData.test = NULL;
	strcpy(DialogData.shortmsg,"");	
	if(!quiet){fprintf(ostream,"\nIs this correct?[yn]\n");
		fprintf(ostream,"Semester Start  = %s\n",SemStart);
		Dialogue(istream,ostream,errorstream,ostream,"",
			&DialogData,&GetUser);
		if(DialogData.usrinpt[0]=='n') goto dialog4;
	}


dialog5:
DialogData.test = &VerifyDate;
strcpy(DialogData.longmsg,"");
strcpy(DialogData.shortmsg,"Enter the Date of the last day of classes.\n");
if(!quiet)DialogData.verbose = 1;
else DialogData.verbose = 0;


			
	if(Dialogue(istream,ostream,errorstream,ostream,"",
		&DialogData,&GetUser))return 1; 


	strcpy(EndClasses,DialogData.usrinpt);
	DialogData.verbose = 0;
	DialogData.test = NULL;
	strcpy(DialogData.shortmsg,"");	
	if(!quiet){fprintf(ostream,"\nIs this correct?[yn]\n");
		fprintf(ostream,"End of Classes  = %s\n",EndClasses);
		Dialogue(istream,ostream,errorstream,ostream,"",
			&DialogData,&GetUser);
		if(DialogData.usrinpt[0]=='n') goto dialog5;
	}

dialog6:
DialogData.test = &VerifyDate;
strcpy(DialogData.longmsg,"");
strcpy(DialogData.shortmsg,"Enter the Date of the last day of finals\n");
if(!quiet)DialogData.verbose = 1;
else DialogData.verbose = 0;


			
	if(Dialogue(istream,ostream,errorstream,ostream,"",
		&DialogData,&GetUser))return 1; 


	strcpy(EndSemester,DialogData.usrinpt);
	DialogData.verbose = 0;
	DialogData.test = NULL;
	strcpy(DialogData.shortmsg,"");	
	if(!quiet){fprintf(ostream,"\nIs this correct?[yn]\n");
		fprintf(ostream,"End of Semester  = %s\n",EndSemester);
		Dialogue(istream,ostream,errorstream,ostream,"",
			&DialogData,&GetUser);
		if(DialogData.usrinpt[0]=='n') goto dialog6;
	}


#define MSG6 "\n\nHolidays: you will be asked to enter a set of dates which\n\
inform the program of days when classes are cancelled due to University\n\
holidays. Each Holiday is defined by a start date and an end date. You will\n\
also be asked to supply a name for each holiday. Pick any short descriptive\n\
name. When you are finished entering holidays, enter q to quit the holidays\n\
dialog.\n\n\n"

	if(!quiet)fprintf(ostream,"%s",MSG6);

HolidayData.number = 0;
	if(Dialogue(istream,ostream,errorstream,ostream,"Holiday Dialogue",
		&HolidayData,&HolidayDialog))return 1;


/* Now create the new times file */

	strcpy(buffer,WorkingDirectory);
         strcat(buffer,PATHSEP);
        strcat(buffer,"data");
         strcat(buffer,PATHSEP);
	strcat(buffer,TemplateName);
         strcat(buffer,PATHSEP);
        strcat(buffer,"time");
	strcpy(buffer2,WorkingDirectory);
         strcat(buffer2,PATHSEP);
        strcat(buffer2,"data");
         strcat(buffer2,PATHSEP);
	strcat(buffer2,SemesterName);
         strcat(buffer2,PATHSEP);
        strcat(buffer2,"time");

	fprintf(ostream,"Creating  %s\n",buffer2);
	if(CopyTimesFile(buffer,buffer2))
		myerror(errorstream,"Failed to make times file\n");
	else fprintf(ostream,"Made times file\n");

#define MSG7 "\n\nYou must still edit the times file since NonPeriodic Time\n\
slots and time slots defined in terms of dates rather than offsets could \n\
not be updated.\n\n\nThe program will now create a blank schedule. You \n\
be asked to provide a string to use for data which has not yet been \n\
entered (e.g., instructors, times and rooms. ) Use any short string such\n\
as TBA or just hit space bar and return if you want a blank to appear instead.\n\n\n"

	if(!quiet)fprintf(ostream,"%s", MSG7);


dialog7:
DialogData.test = NULL;
strcpy(DialogData.longmsg,"");
strcpy(DialogData.shortmsg,"Enter a string to stand for missing data.\n");
if(!quiet)DialogData.verbose = 1;
else DialogData.verbose = 0;


			
	if(Dialogue(istream,ostream,errorstream,ostream,"",
		&DialogData,&GetUser))return 1; 


	strcpy(NoData,DialogData.usrinpt);
	DialogData.verbose = 0;
	DialogData.test = NULL;
	strcpy(DialogData.shortmsg,"");	
	if(!quiet){fprintf(ostream,"\nIs this correct?[yn]\n");
		fprintf(ostream,"Missing Data  = %s\n",NoData);
		Dialogue(istream,ostream,errorstream,ostream,"",
			&DialogData,&GetUser);
		if(DialogData.usrinpt[0]=='n') goto dialog7;
	}


/* Now create the new schedule file */

	strcpy(buffer,WorkingDirectory);
         strcat(buffer,PATHSEP);
        strcat(buffer,"data");
         strcat(buffer,PATHSEP);
         strcat(buffer,"sched.");
	strcat(buffer,TemplateName);
	strcpy(buffer2,WorkingDirectory);
	if(!script_only){
         strcat(buffer2,PATHSEP);
          strcat(buffer2,"data");
           strcat(buffer2,PATHSEP);
           strcat(buffer2,"sched.");
	  strcat(buffer2,SemesterName);
	}
	else {
         strcat(buffer2,PATHSEP);
          strcat(buffer2,"data");
           strcat(buffer2,PATHSEP);
	  strcat(buffer2,SemesterName);
	}

	fprintf(ostream,"Creating  %s\n",buffer2);
	if(CopyScheduleFile(buffer,buffer2))
		myerror(errorstream,"Failed to make schedule file\n");
	else fprintf(ostream,"Made schedule file %s\n",buffer2);

if(!script_only)
fprintf(ostream,"\n\nNew semester initialization complete.\n");
else
fprintf(ostream,"\n\nSchedule file creation complete.\n");



/* Clean up and exit */

	 remove(TempFilePath);
#ifdef __MSDOS__
	  system(StartUpDrive);  /* Restore default drive */
	  strcpy(buffer,"cd ");
	  strcat(buffer,StartUpDirectory);
	  system(buffer);   /* Restore startup directory */ 
#else
	  chdir(StartUpDirectory);
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
	if(strcmp(word,"q") == 0) return 2;
	if(strcmp(word,"wd") == 0) return 3;
	if(strcmp(word,"b")==0) return 4;

/* arg not found */
	return -2;
}

/* myerror: generic error reporting routine */

void myerror(FILE *stream,char *fmt, ...)
{
	va_list args;

	va_start(args,fmt);
	fprintf(stream,"%s: ",ProgramName);
	vfprintf(stream,fmt,args);  
	va_end(args);
	return;
}

/* fatal: generic error reporting routine */

void fatal(FILE *stream,char *fmt, ...)
{
	va_list args;

	va_start(args,fmt);
	fprintf(stream,"%s: Fatal Error.\n",ProgramName);
	vfprintf(stream,fmt,args);
	fprintf(stream,"\n");
	va_end(args);
	exit(1);
}


/* SemesterExits: return 1 if WorkingDirectory/data/name exists and is
   writable by this process, 0 otherwise. */

int SemesterExists(char *name)
{

#ifdef __MSDOS__

        DIR *ptr;

	/* build path */
	strcpy(buffer,WorkingDirectory);
        strcat(buffer,"\\data\\");
	strcat(buffer,name);
        /* try to open */
        if((ptr = opendir(buffer))==NULL) {
                myerror(errorstream,"Unable to open %s errno = %d\n",buffer,errno);
		return 1;
        }
        else closedir(ptr);
        return 0;
#else
	struct stat buf;

	/* build path */
	strcpy(buffer,WorkingDirectory);
	strcat(buffer,"/data/");
	strcat(buffer,name);

	if(stat(buffer,&buf)){
		myerror(errorstream,"Unable to stat %s errno = %d\n",buffer,errno);
		return 1;
	}
	if(!(buf.st_mode && S_IWRITE)){
		myerror(errorstream,"Unable to write %s\n",buffer);
		return 1;
	}
	return 0;
#endif
}

/* CopyFile: does just what the name says */

int CopyFile(char *source,char *dest)
{
	FILE *input, *output;
	int c;

	if(source == NULL || dest == NULL){
		myerror(errorstream,"Unable to copy. No file name provided.\n");
		return 1;
	}

	if((input = fopen(source,"r")) == NULL){
		myerror(errorstream,"Unable to open %s.\n",source);
		return 1;
	}

	if((output = fopen(dest,"w")) == NULL){
		myerror(errorstream,"Unable to open %s.\n",source);
		return 1;
	}

	while((c = fgetc(input)) != EOF)
		fputc(c,output);

	fclose(input);
	fclose(output);
	return 0;
}

/* VerifyDate: returns 1 if DateString is a comprehensible date, 0 else */

int VerifyDate(char *DateString)
{
	char buf[SHORTNAME];
	char numbuf[10];
	char *ptr;
	int temp;


	strcpy(buf,DateString);

	/* Check Month */

	if((ptr = strtok(buf,FIELDSEPS))==NULL){
		myerror(errorstream,"Can't find month in %s\n",DateString);
		return 1;
	} 
	strcpy(numbuf,ptr);
	if((temp = atoi(numbuf)) > 12 || temp < 0 ){
		myerror(errorstream,"Month out of range in %s\n",DateString);
		return 1;
	}

	/* Check Day */   

	if((ptr=strtok(NULL,FIELDSEPS))==NULL){
		myerror(errorstream,"Can't find day in %s\n",DateString);
		return 1;
	} 
	strcpy(numbuf,ptr);
	if((temp = atoi(numbuf)) > 31 || temp < 1 ){
		myerror(errorstream,"Day out of range in %s\n",DateString);
		return 1;
	}



	/* Check Year */

	if((ptr=strtok(NULL,FIELDSEPS))==NULL){
		myerror(errorstream,"Can't find year in %s\n",DateString);
		return 1;
	} 
	strcpy(numbuf,ptr);
	temp = atoi(numbuf);
	if((temp >= 100 && temp < 1970)||(temp < 70)  ){
		myerror(errorstream,"Year out of range in %s\n. Must be after 1969.",DateString);
		return 1;
	}

/* Everything OK */

	return 0;
}


char linebuf[MAXLINE];

/* CopyTimesFile: copy and update the time.sem file from the example semester
   to the destination one */

int CopyTimesFile(char *source, char *dest)
{
	char *ptr ;
	int parsed = 0;
	int i;
	FILE *input, *output;
	int line = 0;

	ptr = linebuf;

	numtokens = 0;

/* Open source and destination files */

	if((input = fopen(source,"r"))==NULL){
		myerror(errorstream,"Unable to open %s\n",source);
		return 1;
	}
	if((output = fopen(dest,"w"))==NULL){
		myerror(errorstream,"Unable to open %s\n",dest);
		return 1;
	}

/* Loop over lines in timesfile */

	while(fgets(ptr,MAXLINE + linebuf -ptr ,input) != NULL){
	line++;

/* Now parse the contents of linebuf */

	numtokens = tokenize(linebuf,Tokens);
	if(numtokens == -1){       /* line continues */
		 ptr = ptr + strlen(ptr);
		 continue;
	}

	ptr = linebuf;

     if(numtokens != 0){
	if(strcmp(Tokens[0]->text,"SemesterStart")==0){
	fprintf(output,"%s %s %s\n","SemesterStart",SemStart,"0:00");
	parsed = 1;
	}

	if(strcmp(Tokens[0]->text,"EndOfClasses")==0){
	fprintf(output,"%s %s %s\n","EndOfClasses",EndClasses,"5:00");
	parsed = 1;
	}

	if(strcmp(Tokens[0]->text,"EndOfSemester")==0){
	fprintf(output,"%s %s %s\n","EndOfSemester",EndSemester,"9:00");
	parsed = 1;
	}


	if(strcmp(Tokens[0]->text,"PeriodicTimeSlot")==0) {

/* Check the format of token 3. If it is a number then assume that what
   follows is a list of numerical offsets from semester start. If is a
   date then what follows is a list of date-time pairs */

		if(strpbrk(Tokens[3]->text,"/-") != NULL) {

/* it's a date! */

	/* don't update  such lines: parsed = 0 */

		}


		else { /* It's an offset */
		fprintf(output,"%s",linebuf);
		parsed = 1;
		}


	}

/* NonPeriodic Time Slot entry */
/* Don't copy these */

	if(strcmp(Tokens[0]->text,"NonPeriodicTimeSlot")==0) {
   
	}
		
/* Holidays  entry */

	if(strcmp(Tokens[0]->text,"Holidays")==0) {
	fprintf(output,"Holidays ");
	for(i=0;i<HolidayData.number-1;i++)
		fprintf(output,"\"%s\" %s %s  %s %s \\\n",
			HolidayData.name[i],
			HolidayData.starts[i],
			"0:00",
			HolidayData.ends[i],
			"0:00");
	fprintf(output,"\"%s\" %s %s  %s %s \n",
			HolidayData.name[i],
			HolidayData.starts[i],
			"0:00",
			HolidayData.ends[i],
			"0:00");

	parsed = 1;
	}
		
/* Content lines which cannot be updated are clearly marked and commented */
	if(!parsed){
		fprintf(output,"################ EDIT THIS LINE ##########\n");		fprintf(output,"#%s",linebuf);
	}

     }

        /* Pass through comments, etc */
	else fprintf(output,"%s",linebuf);
		
/* prepare for next line */

	ptr = linebuf;
	parsed = 0;
	} 
fclose(input);
fclose(output);
return 0;
}

/* Creates a "blank" schedule for a new semester by setting all teachers,
	times, and rooms to "TBA"
*/ 

int CopyScheduleFile(char *source, char *dest)
{
	char *ptr ;
	int parsed = 0;
	FILE *input, *output;
	int line = 0;

	ptr = linebuf;

	numtokens = 0;

/* Open source and destination files */

	if((input = fopen(source,"r"))==NULL){
		myerror(errorstream,"Unable to open %s\n",source);
		return 1;
	}
	if((output = fopen(dest,"w"))==NULL){
		myerror(errorstream,"Unable to open %s\n",dest);
		return 1;
	}

	fprintf(output,"set MISSINGDATA \"%s\"\n",NoData);

/* Loop over lines in  source file */


	while(fgets(ptr,MAXLINE + linebuf -ptr ,input) != NULL){
	line++;

/* Now parse the contents of linebuf */

	numtokens = tokenize(linebuf,Tokens);
	if(numtokens == -1){       /* line continues */
		 ptr = ptr + strlen(ptr);
		 continue;
	}

	ptr = linebuf;

     if(numtokens != 0){ /* content line */
	if(strcmp(Tokens[0]->text,"schedule")==0){
	fprintf(output,"%s \"%s\" \"%s\"\n","schedule",SemesterName,Tokens[2]->text);
	parsed = 1;
	}

	if(strcmp(Tokens[0]->text,"course")==0){
		if(numtokens == 3)
			fprintf(output,"%s \"%s\" \"%s\"\n","course",Tokens[1]->text,NoData);
		if(numtokens == 2)	
			fprintf(output,"%s \"%s\"\n","course",Tokens[1]->text);
	parsed = 1;
	}

	if(strcmp(Tokens[0]->text,"class")==0){
	fprintf(output,"%s \"%s\" \"%s\" \"%s\" \"%s\" \"%s\"\n","class",Tokens[1]->text,
		NoData,NoData,NoData,Tokens[5]->text);
	parsed = 1;
	}
	if(strcmp(Tokens[0]->text,"assign")==0){
	fprintf(output,"%s \"%s\" \"%s\" \"%s\"\n","assign",Tokens[1]->text,
		NoData,Tokens[3]->text);
	parsed = 1;
	}


		
/* All other content lines are echoed as is */
	if(!parsed){
	fprintf(output,"%s",linebuf);
	}

     }

        /*  copy comments, blank lines */
	else fprintf(output,"%s",linebuf);
		
/* prepare for next line */

	ptr = linebuf;
	parsed = 0;
	} 
fclose(input);
fclose(output);
return 0;
}
