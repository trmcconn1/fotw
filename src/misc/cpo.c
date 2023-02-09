/* cpo.c: Source for "Copy Over" utility */

/* Version 1.1, By Terry McConnell, December 1997 */

/* In response to a question on Usenet. Note that gnu dd with the notrunc
   option includes the functionality of cpo -c */

/* Compile: cc -o cpo -D_POSIX_SOURCE cpo.c  */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#include <string.h>

#define VERSION "1.1"
#define INFO  "                  CHICKEN HAVEN SOFTWARE\n\
cpo: Copy file on top of another.\n\
usage cpo [-ci] [-n records] [-version] [infile ] outfile\n\
	-c: Use bytes as records (lines are default.)\n\
	-i: Interactive: echoes line about to be overwritten. \n\
	    Used only with stdin. Not available with -c.\n\
	-n: Use the next arg as maximum number of records to copy.\n\
If no input file is given, stdin is used. "    

#define USAGE "usage: cpo [-ci] [-n records ] [-version] [ infile ] outfile\n"
#define MAX_ARG_LEN 80             /* Max length of combined options */
#define LONGLINE 1023  /* Longest line we will tolerate in a text file.
			Counts the newline at end. */

static char ProgramName[256];

static int find_opt(char *option_string);
static void myerror(char *fmt, ...);

int main(int argc, char **argv)
{
	int c=-1;                /* numeric option code */
	char *info = INFO;
	char opsep = '-';      /* Standard for Unix */
	char mode[4];
	char mode_ext[] = "b";
	char argstr[MAX_ARG_LEN];
	FILE *input,*output,*tempfile;
	char buffer1[LONGLINE+1];
	char buffer2[LONGLINE+1];
	char OutputFileName[PATH_MAX];
	int chold;
	long records = -1L;
	int done = 0;  /* = 0 as long as there are more lines in the input */
	int interactive = 0;
	int lines = 1; /* flag: Use lines as records */

strcpy(ProgramName,argv[0]);

/* Process Command Line Options */

if(--argc > 0){  /* Anything on command line ? */
                ++argv;
                while((argc > 0)&&((*argv[0]) == opsep)){ /* loop while 
							there are flags */
                        argstr[0]=opsep;
                        argstr[1]='\0';
                        strcat(argstr,++*argv); /* glob argstr*/
                        argc--;
                        while((argc>0)&&((*(argv+1))[0]==opsep)&&((*(argv+1))[1]!=opsep)){
                                ++argv;
                                argc--;
                                strcat(argstr,++*argv);
                        }
                        ++argv; /* ok, consecutive flags now globbed into one*/
                       c = find_opt(argstr+1); /* look up flag code */
                switch(c){


			case -1: break;  /* default, e.g no options */

			case 0: printf("%s\n",info);
				return 0;
			case 1:
				lines = 0; /* Use bytes as records */
				mode_ext[0] = '\0';
				break;   
			
			case 4: lines = 0;  /* both c and n options */	
				mode_ext[0] = '\0';    /*FALLTHROUGH*/
			case 2: records = atol(*argv++);
				argc--;
				break;

			case 3: interactive = 1;
                                break;
			case 5: printf("Version %s\n",VERSION);
				return 0;

			case 6: myerror("Interactive mode not available for character records\n");
				break;
			case 7: myerror("Number of records cannot be given in interactive mode\n");
				break;
                        case -2: ;  /* barf */
			default:
				myerror("%s: %s\n%s"," illegal option ",argstr,USAGE);
			} /* End flag switch */
	} /* End flag grabbing while loop */
} /* end if: any args ? */


/* Make sure we have an infile and an outfile name on command line */

	if(argc == 0){
		myerror("%s\n",USAGE);
	}

	if(interactive && argc > 1)
		myerror("Interactive mode cannot be used with file input");

	if(argc > 1){
/* Make sure we can open the input file */
		strcpy(mode,"r");
		if((input = fopen(*argv,strcat(mode,mode_ext) )) == NULL) {
			myerror("cannot open %s\n",*argv);
		}
		argv++;
		argc--;
		setvbuf(input,NULL,_IOLBF,LONGLINE+1);  /* Line buffer */
	}
	else input = stdin;

/* Try to open the output file */
	strcpy(OutputFileName,*argv);
	strcpy(mode,"r+");
	if((output = fopen(OutputFileName,strcat(mode,mode_ext))) == NULL) {
		myerror("cannot open %s\n",OutputFileName);
	}
	argv++;
	argc--;
	if(argc > 0)
		fprintf(stderr,"cpo: extra arguments on command line ignored\n");

	if(lines){

/* Try to create a temporary file */
		if((tempfile = tmpfile())==NULL){
			myerror("unable to create temp file.\n");
		}

/* Copy lines from input file to tempfile until exhausted and then
   copy from output file till exhausted */

       		 while(fgets(buffer2,LONGLINE,output)!=NULL){
			if(records-- == 0L) done = 1;
			if(interactive && ! done )
				printf("%s",buffer2);
			if((! done) && (fgets(buffer1,LONGLINE,input)!=NULL))
				fputs(buffer1,tempfile);
			else { 	
				fputs(buffer2,tempfile);
		       		done = 1;
			}
		}

/* Copy any remaining records from the input file, if applicable */
		if(!done)
			while(fgets(buffer1,LONGLINE,input)!=NULL)
				fputs(buffer1,tempfile);

		rewind(tempfile);
		fclose(output);

	/* reopen output file and copy tempfile over it */
		strcpy(mode,"w+");
		if((output = fopen(OutputFileName,strcat(mode,mode_ext))) == NULL) {
			myerror("cannot open %s for final write.\n",OutputFileName);
		}
		while(fgets(buffer1,LONGLINE,tempfile)!=NULL)
			fputs(buffer1,output);

		fclose(tempfile);

	}
	else {
/* Copy bytes from input file to output file */
		while(records-- != 0L && (chold = fgetc(input))!= EOF)
			fputc(chold,output);
	}
	fclose(input);	
	fclose(output);

	return 0;
}

/*  find_opt: return a unique small integer for each possible option string */
/*  There should be a case in the switch statement in main to handle each. */
/*  -2 reserved for arg not found -1 reserved for no options */

int find_opt(char *word)
{
	if(strcmp(word,"help")==0)return 0;
	if(strcmp(word,"c")==0)return 1;
	if(strcmp(word,"n")==0)return 2;
	if(strcmp(word,"i")==0)return 3;
	if(strcmp(word,"cn")==0)return 4;
	if(strcmp(word,"version")==0)return 5;
	if(strcmp(word,"ci")==0)return 6;
	if(strcmp(word,"ic")==0)return 6;
	if(strcmp(word,"in")==0)return 7;
	if(strcmp(word,"ni")==0)return 7;

/* arg not found */
	return -2;
}

/* myerror: generic error reporting routine */

void myerror(char *fmt, ...)
{
	va_list args;

	va_start(args,fmt);
	fprintf(stderr,"%s: ",ProgramName);
	vfprintf(stderr,fmt,args);
	fprintf(stderr,"\n");
	va_end(args);
	exit(1);
}
