/* shell.c:  command interpreter */

#include "shell.h"

void Prompt( FILE * , char * );
static int firstcall = 1;
static int shllvl = 0;

char COLS[LONGNAME] = "2";

int ClassLevel = 0;
int numschedules = 0;
struct ScheduleStruct *Schedules[MAX_SCHEDULES];
struct ScheduleStruct *CurrentSchedule;
struct CourseStruct *CurrentCourse;
struct ClassStruct *CurrentClass;
struct ClassStruct *PendingClass[MAX_LVL];
struct AssignStruct *CurrentAssignment;

int nvars;
int numtokens;
struct ShellVar *ShellVars[MAXVARS];
char *GetShell(char *); /* return value of shell variable */


/* Declarations of shell commands */

int ncmds; 
static struct ShellCmd *CmdPtrs[MAX_CMDS];

/* defined in load.c */
extern int load(SHELLSTREAMS);

/* defined in roomchrt.c */
extern int roomchart(SHELLSTREAMS);

/* defined in shelldb.c */
extern int dtimes(SHELLSTREAMS);
extern int dpeople(SHELLSTREAMS);
extern int djobs(SHELLSTREAMS);
extern int drooms(SHELLSTREAMS);
extern int dblocks(SHELLSTREAMS);
extern int dfinals(SHELLSTREAMS);

/* defined in this file */
int set(char *, char *, int, FILE *);
int shift(FILE *);
int echo(SHELLSTREAMS);
int cd(SHELLSTREAMS);
int push(SHELLSTREAMS);
int edit(SHELLSTREAMS);
int myexit(SHELLSTREAMS);
int redirect(SHELLSTREAMS);
int redirect2(SHELLSTREAMS);
int myread(SHELLSTREAMS);
int take(SHELLSTREAMS);
int more(SHELLSTREAMS);
int help(SHELLSTREAMS);
int status(SHELLSTREAMS);
int toggledebug(SHELLSTREAMS);
int uncondbranch(SHELLSTREAMS);

/* defined in schedcmd.c */
extern int course(SHELLSTREAMS);
extern int class(SHELLSTREAMS);
extern int subclass(SHELLSTREAMS);
extern int assign(SHELLSTREAMS);
extern int refno(SHELLSTREAMS);
extern int schedule(SHELLSTREAMS);
extern int attach(SHELLSTREAMS);
extern int comment(SHELLSTREAMS);
extern int finalblock(SHELLSTREAMS);

/* defined in audit.c */
extern int audit(SHELLSTREAMS);


/* Declaration of the array of all shell commands except set and end */


struct ShellCmd {
char *name;
char *usage;
char *description;
int (*function)(FILE *, FILE *, FILE *,FILE *);    
} Cmds[] = 
{/*          djobs   */
"djobs",
"djobs [n]",
"djobs [n] : display names of all jobs, or info about job n." ,
djobs,
/*          dpeople  */
"dpeople",
"dpeople [n]",
"dpeople [n] : display names of personnel, or info about person n.",
dpeople,
/*          drooms   */
"drooms",
"drooms [n]",
"drooms [n] : display names of all rooms, or info about room n.",
drooms,
/*          dtimes  */
"dtimes",
"dtimes [n]",
"dtimes [n] : display names of time slot lists, or info about list n.",
dtimes,
/*          echo    */
 "echo", 
"echo [-n] <args>",
"echo [-n] [ ... ]: print ... on the shell's output stream.",
echo,
/*          load    */
"load",
"load [-t -u -sem <semester> -free]",
"load [-t -u -sem <semester> -free]: causes the database to be loaded from files.\n\
	-t: don't load time slot information\n\
	-u: update rooms, people, and jobs, rather than overwriting\n\
	-sem: use <semester> rather than $SEM. Reset $SEM\n\
	-free: unloads any currently loaded data.",
load,
/*           cd      */
"cd",
"cd <path>",
"cd <path>: change the program's working directory to <path>. Reset $CWD.",
cd,
/*           push    */
"push",
"push [args]",
"push: [args]: invoke a system command interpreter, pass args on command line.",
push,
/*           edit    */
"edit",
"edit <filename>",
"edit <filename>: Invoke $EDITOR to edit filename.",
edit,
/*           exit    */
"exit",
"exit",
"exit: exit schedule ungracefully. In Dos this will not restore startup\n\
	 directory.",
myexit,
/*           > (redirect) */
">",
"> [file]",
"> [file]: subsequent output goes to file.\n\
	 To restore to screen: end \n",
redirect,
"2>",
"2> [file]",
"2> [file]: subsequent error messages go to file.\n\
	 To restore to screen: end \n",
redirect2,
"take",
"take <file>",
"take <filename>: source filename in a subshell",
take,
"?",
"? [command]",
"? [command]: Print a brief description of each shell command.",
help,
"course",
"course [end] [name [supervisor]]",
"course [end] [name [supervisor]]: Display all courses of current schedule.\n\
 [Unset the course shell variable ]\n \
[ [Display only course name] [create course name with supervisor ]]",
course,
"class",
"class [name] [name room teacher time job] [enroll]",
"class [name] [name room teacher time job] [enroll]:\n\
Displays classes of current course\n\
[Display named class and sub-classes]\n\
[Create class name and set room, teacher, time, and job [set enrollment]].",
class,
"attach",
"attach [name room teacher time job] [enroll]",
"attach [name room teacher time job] [enroll]: \n\
	[Create attachment name and set room, teacher, time, and job].",
attach,
"subclass",
"subclass [end]",
"subclass [end]: push [pop] to [from] lower class level",
subclass,
"status",
"status",
"status: print information about memory usage (DOS only)",
status,
"assign",
"assign [course] [name person job]",
"assign [course] [name person job]: List all assignments \n \
	[List all assignments of course]\n\
	[create a new assignment of given name].\n",
assign,
"more",
"more <shell command>",
"more <shell command>: page the output of shell command",
more,
"refno",
"refno <number>",
"refno <number>: attaches reference number to the current course.",
refno,
"schedule",
"schedule [-c <semester>] [[semester] [description]] \n",
"schedule [-c <semester> ]  [semester] [description]: \n\
	 List all currently loaded schedules. \n\
	[Change current schedule to semester]\n\
	[[Display schedule for semester]\n\
	[ Create a new schedule for given semester with given description.] \n\
	Description is used as a title on listings. \n",
schedule,
"read",
"read",
"read: get a line of user input. Store in $USER_INPUT",
myread,
"audit",
"audit [-c -t -a] \n",
"audit [-c -t -a] : print audit report for currently loaded schedules\n\
	-c: reset internal totals variables to zero\n\
	-t: include a totals column\n\
	-a: use academic year load figure.",
audit,
"toggledebug",
"toggledebug",
"toggledebug: toggles debug,  which causes every script line to be echoed.",
toggledebug,
"roomchart",
"roomchart [start_date start_time end_date end_time]",
"roomchart [start_date start_time end_date end_time]: create a room chart\n\
 specification file for rendering. Get info from user [or command line]",
roomchart,
"if",
"if <NAME> [command]",
"if <NAME> [command]: execute command if shell variable NAME is defined.",
uncondbranch,
"comment",
"comment [text]",
"comment [text]: attaches text to comment field of current course or class",
comment,
"dblocks",
"dblocks",
"dblocks: dumps information on final examination blocks",
dblocks,
"finalblock",
"finalblock <[end] [auto] [<name>]> ",
"finalblock <[end] [auto] [<name>]>: \n\
	[end block associating classes with a final exam block.\n\
	[auto: begin block listing classes associated with final by time]\n\
        [name: begin associating classes with final exam block name]\n\
Lines between finalblock ... finalblock end have the format\n\
crse [class] room  -or- class room",
finalblock,
"dfinals",
"dfinals [name]",
"dfinals [name]: List final exam schedule [only for block name]",
dfinals
};

int skip_input = 0; /* Go on to next iteration of main shell loop,
                      without waiting for user input. Allows subroutines to
                      run a command */

int CompareVarsByName(struct ShellVar *, struct ShellVar *);
int CompareCmdsByName(struct ShellCmd *, struct ShellCmd *);
char common_buf[BUFSIZ];
char littlebuf[LONGNAME];

int cmdline = 0;  /* current line of input */

int 
shell(SHELLSTREAMS,char *errmsg, void *data)
{

/* This routine processes one line for each call: The line may be retained for
	append, ignored,  or parsed and a command executed. */

	static char buffer[MAXLINE];  /* input buffer */
    
	int cmdlen;
	int i;
       
	static char *ptr = buffer;  /* current end of input */
	struct ShellCmd *cmdptr;
	struct ShellCmd TCmd;      /* Template command for search */

	TCmd.name = littlebuf;     /* point name at a buffer */

/* don't use buffer for anything but input until after tokenize -- use
    littlebuf instead */

/* Allocate and initialize built in shell variables if this 
is a first call and not a sub shell */
#define STATICVARS 5

	if(firstcall && (shllvl == 0)){

		for(i=0;i<STATICVARS;i++){
			ShellVars[i] = (struct ShellVar *) malloc( sizeof(struct ShellVar));
		if(ShellVars[i]==NULL)
			fatal(shell_errorstream,"Unable to alloc Shell vars.\n");
		}

/* The values of the first shell variables are set to externally
    defined addresses. These cannot be freed, so if you add more of them,
    also add to the if statement in the ^D section below */


		if((ShellVars[0]->name = (char *)malloc(SHORTNAME))!=NULL)
			strcpy(ShellVars[0]->name,"CWD");
		else fatal(shell_errorstream,"Unable to alloc shell vars.\n");
			ShellVars[0]->value = WorkingDirectory;
		if((ShellVars[1]->name = (char *)malloc(SHORTNAME))!=NULL)
			strcpy(ShellVars[1]->name,"SEM");
		else fatal(shell_errorstream,"Unable to alloc shell vars.\n");
			ShellVars[1]->value = Semester;
		if((ShellVars[2]->name = (char *)malloc(SHORTNAME))!=NULL)
			strcpy(ShellVars[2]->name,"DATE");
		else fatal(shell_errorstream,"Unable to alloc shell vars.\n");
			ShellVars[2]->value=Date;
		if((ShellVars[3]->name = (char *)malloc(SHORTNAME))!=NULL)
			strcpy(ShellVars[3]->name,"VERSION");
		else fatal(shell_errorstream,"Unable to alloc shell vars.\n");
			ShellVars[3]->value=Version;
		if((ShellVars[4]->name = (char *)malloc(SHORTNAME))!=NULL)
			strcpy(ShellVars[4]->name,"MISSINGDATA");
		else fatal(shell_errorstream,"Unable to alloc shell vars.\n");
			ShellVars[4]->value=MissingData;

		nvars = STATICVARS;

		set("ECHO","OFF",3,shell_ostream);
		set("ROWS","25",3,shell_ostream);
		set("CLASS-LEVEL","0",3,shell_ostream);
  
		krqsort((void **)ShellVars,0,nvars-1,(int(*) (void *,void *))
		  CompareVarsByName);
		for(i=0;i<nvars;i++)
			ShellVars[i]->indx = i;
		ncmds = sizeof(Cmds)/sizeof(struct ShellCmd);
		for(i=0;i<ncmds;i++)
			CmdPtrs[i] = &(Cmds[i]);
		krqsort((void **)CmdPtrs,0,ncmds-1,(int(*)(void *,void *))
		   CompareCmdsByName);

		numschedules = 0;

	}
	if(firstcall){  /* Also do in first call of a subshell */
		cmdline = 0;
		firstcall = 0;
	/* The shell takes care of reporting its own error messages --
		does not use Dialog's error message buffer */
		strcpy(errmsg,"");


	}

	sprintf(littlebuf,"%d",shllvl);
	set("SHELL-LEVEL",littlebuf,3,shell_ostream);
		 

/* Display our handsome prompt */

	if(strcmp("OFF",GetShell("ECHO"))!=0 && strcmp(GetShell("PROMPT"),"")!=0)
	Prompt(prompt_stream,GetShell("PROMPT"));

/* Get a command line from input */
/* If the cmdlen is zero, then user hit ^D, so dialogue is over */

      if(!skip_input){

	if(fgets(ptr,MAXLINE,shell_istream) == NULL) {
		for(i=0;i<nvars;i++){

/* This is bad, but I don't know how else to do it. 5 of the variables
are set equal to statically alloced external variables and cannot be
free-d */       if((strcmp(ShellVars[i]->name,"CWD")!=0)&&
		    (strcmp(ShellVars[i]->name,"DATE")!= 0)&&
		    (strcmp(ShellVars[i]->name,"SEM")!= 0)&&
		     (strcmp(ShellVars[i]->name,"MISSINGDATA")!=0)&&
		    (strcmp(ShellVars[i]->name,"VERSION")!= 0))
			free(ShellVars[i]->value);
			free(ShellVars[i]->name);
			free((ShellVars[i]));
		}

		nvars = 0;
		firstcall=1;
		return DIALOGUE_ABORT;
	}

	cmdlen = strlen(buffer);


/* parse the contents of the input buffer */

	numtokens = tokenize(buffer,Tokens);
	if(numtokens == -1) {
		ptr = buffer + cmdlen;   /* We need more input - line continues
							  */
		return DIALOGUE_SUCCESS;
	}
	ptr = buffer;  /* reset for next line */
	cmdline++;
	line = cmdline;
	if(numtokens == 0)               /* uninteresting line */
		 return DIALOGUE_SUCCESS;

     }  /* Dont skip input section ends */
     else skip_input = 0;

/* Process individual commands */

/*  The set command is a special case */

	if(strcmp(Tokens[0]->text,"set")==0) {         
		set(Tokens[1]->text,Tokens[2]->text,numtokens,shell_ostream);
		goto ret;
	}

/*   Search for name of command */

	strcpy(TCmd.name,Tokens[0]->text);
	if( (cmdptr = (struct ShellCmd *) binsearch((void *)&TCmd,
		(void **)CmdPtrs,ncmds,
		(int(*) (void *,void *))CompareCmdsByName)) != NULL)
			{ 
				if((*cmdptr->function)(shell_istream,
	shell_ostream,shell_errorstream,prompt_stream)==1)
					myerror(shell_errorstream,"%s: %s\n",
						cmdptr->name,cmdptr->usage);
				goto ret;
			}

	if(strcmp(Tokens[0]->text,"shell")==0){
		firstcall = 1;
		shllvl++;
		sprintf(buffer,"Sub-Shell Level %d",shllvl);
		Dialogue(shell_istream,shell_ostream,shell_errorstream,
                        prompt_stream,buffer,(void *)NULL,
		(int(*)(FILE *,FILE *,FILE *,FILE *,char *,void *))&shell);
		goto ret;
	}

	 if(strcmp(Tokens[0]->text,"end")==0){
		if(shllvl >0)shllvl--;
		if(numtokens == 3) set(Tokens[1]->text,Tokens[2]->text,3,shell_ostream);             
		if(numtokens == 2) set(Tokens[1]->text,"",2,shell_ostream);             
		   return DIALOGUE_ABORT;
	}

/* If we get to here, the command is not recognized */

	myerror(shell_errorstream,"shell: %s -- no such command. Use ? to see list.\n",
	Tokens[0]->text);
	return DIALOGUE_NONFATAL;

ret:    return DIALOGUE_SUCCESS;
}


void
Prompt( FILE *promptstream, char *PromptString )
{
	if(PromptString != NULL)
	  fprintf(promptstream, "%s ",PromptString);
	else fprintf(promptstream,"Schedule> ");
	return;

}       


/* Returns the index of the shell variable having the given name, -1 if
   no such exists */

int GetIndx(char *name)
{
	struct ShellVar *ptr;
	struct ShellVar TVar;
	char buffer[SHORTNAME];

	TVar.name = buffer;
	SAFECPY(TVar.name,name,SHORTNAME);
	krqsort((void **)ShellVars,0,nvars-1,(int(*) (void *,void *))
	  CompareVarsByName);

	if( (ptr = (struct ShellVar *) binsearch((void *)&TVar, 
		(void **)ShellVars,nvars,
		(int(*) (void *,void *))CompareVarsByName)) == NULL)
		return -1;

	return ptr->indx;
}

/* GetShell: return pointer to value of named shell variable */

char *GetShell(char *name)
{
	int i;

	if((i = GetIndx(name))==-1) return NULL;
	return ShellVars[i]->value;
}


/* set command: manages shell variables */

int set(char *name, char *value, int numtokens, FILE *shell_ostream)
{
	int i;
	struct ShellVar *ptr;

	if(numtokens == 1){ 
		for(i=0;i<nvars;i++)
			fprintf(shell_ostream,"%s = %s\n",ShellVars[i]->name,
				ShellVars[i]->value);
		return 0;
	}

	if(numtokens == 2) {
		if((i = GetIndx(name))== -1) return -1;
/* remove the variable of this name. First, swap it with the last one */
		if(i < nvars-1){
			ptr = ShellVars[nvars -1];
			ShellVars[nvars -1] = ShellVars[i];
			ShellVars[i] = ptr;
		}

	/* Now free the last variable */
                free(ShellVars[nvars-1]->name);
                free(ShellVars[nvars-1]->value);
		free(ShellVars[nvars-- -1]);

/* and resort the array of shell variables */

		krqsort((void **)ShellVars,0,nvars-1,(int(*) (void *,void *))
		 CompareVarsByName);
		for(i=0;i<nvars;i++) 
			ShellVars[i]->indx = i;
		return 0;
	}

	if(numtokens == 3) {
		if((i = GetIndx(name))== -1){
		 if(nvars >= MAXVARS){
			myerror(errorstream,"Too many Shell Variables. Not added.\n");
			return 0;
		 }
		 ShellVars[nvars] = (struct ShellVar *)malloc(sizeof(struct ShellVar));
		 if((ShellVars[nvars]->name = (char *)malloc(SHORTNAME))!=NULL)
		 {SAFECPY(ShellVars[nvars]->name,name,SHORTNAME);}
		 else fatal(errorstream,"Unable to malloc shell var.\n");
		 if((ShellVars[nvars]->value = (char *)malloc(LONGNAME))!=NULL)
		 {SAFECPY(ShellVars[nvars]->value,value,LONGNAME);}
		 else fatal(errorstream,"Unable to malloc shell var.\n");
		 ShellVars[nvars]->indx = nvars++;      
/* resort the shell variable array so we can reset indices */
		krqsort((void **)ShellVars,0,nvars-1,(int(*) (void *,void *))
		CompareVarsByName);
		for(i=0;i<nvars;i++) 
			ShellVars[i]->indx = i;
		}
		else {SAFECPY(ShellVars[i]->value,value,LONGNAME);}

/* A kludge: if we redefine the missing data string, then it will have
    little effect unless we reinitialize the default data structures
    that used it.
*/
        if(strcmp(name,"MISSINGDATA")==0) init_defaults();
	return 0;
	}
	else myerror(errorstream,"Usage: set [name] [value]\n");
	return -1;
}

/* echo: Print Tokens[1] ... on output stream */

int echo(SHELLSTREAMS)
{     
	int i;
	int newline = 1;

	if(numtokens == 1) return 0;
	if(strcmp(Tokens[1]->text,"-n") == 0)
		newline = 0;
	for(i=1+1-newline;i<numtokens;i++)
		fprintf(shell_ostream,"%s ",Tokens[i]->text);
	if( newline) fprintf(shell_ostream,"\n");
	return 0;
}


int cd(SHELLSTREAMS)
{

		if(numtokens != 2)
			return 1;
#ifdef __MSDOS__
		strcpy(common_buf,"cd ");
		strcat(common_buf,Tokens[1]->text);
		system(common_buf);
		set("CWD",Tokens[1]->text,3,shell_ostream);
		strcpy(WorkingDirectory,Tokens[1]->text);
#else
		strcpy(common_buf,Tokens[1]->text);
		if(chdir(common_buf)!=0){
			myerror(shell_errorstream,"Unable to cd to %s\n",common_buf);
			return 0;
		}
		set("CWD",Tokens[1]->text,3,shell_ostream);
		strcpy(WorkingDirectory,Tokens[1]->text);
#endif
		return 0;
}

int push(SHELLSTREAMS)
{
	int i;
#ifdef __MSDOS__

		strcpy(common_buf,"command "); /* command.com */
		if(numtokens > 1){
                                strcat(common_buf, " /C ");
                                for(i=1;i<numtokens;i++){
               	                 strcat(common_buf," ");
               	                 strcat(common_buf,Tokens[i]->text);
                                }
                        }

                        system(common_buf);

#else 
		if(GetShell("SHELL")==NULL)
			strcpy(common_buf,"/bin/sh ");
		else {
			strcpy(common_buf,GetShell("SHELL"));
			if(numtokens > 1){
				strcat(common_buf, " -c \"");
				for(i=1;i<numtokens;i++){
				strcat(common_buf," ");
				strcat(common_buf,Tokens[i]->text);
				}
				strcat(common_buf,"\"");
			}
			
			system(common_buf);
		}
#endif
		return 0;       
}

int edit(SHELLSTREAMS)
{
	
		
		if(numtokens != 2)
			return 1;       
		if(GetShell("EDITOR")==NULL){
			myerror(shell_errorstream,"No EDITOR shell variable defined\n");
			return 0;
		}
		strcpy(common_buf,GetShell("EDITOR"));
		strcat(common_buf,"  ");
		strcat(common_buf,Tokens[1]->text);
		system(common_buf);
		return 0;
	}

int myexit(SHELLSTREAMS)
{
	exit(0);
	return 0;    /* avoids compiler warning */
}

int redirect(SHELLSTREAMS)
{
	FILE *new_output;

	if(numtokens != 2)
		return 1;
	if((new_output =fopen(Tokens[1]->text,"w"))==NULL){
		myerror(shell_errorstream,"Unable to open %s\n",Tokens[1]->text);
		return 0;
	}
	shllvl++;
	if(GetShell("ECHO") != NULL && strcmp(GetShell("ECHO"),"ON")==0)
	fprintf(shell_ostream,"Output redirected to %s. Give end command to restore.\n",
		Tokens[1]->text);
	Dialogue(shell_istream,new_output,shell_errorstream,prompt_stream,
                        "",(void *)NULL,&shell);
	fclose(new_output);
	return 0;
		
}

int redirect2(SHELLSTREAMS)
{

		FILE *new_errs;

		if(numtokens != 2)
			return 1;
		if((new_errs= fopen(Tokens[1]->text,"w"))==NULL){
			myerror(shell_errorstream,"Unable to open %s\n",Tokens[1]->text);
			return 0;
		}
		shllvl++;
		if(GetShell("ECHO") != NULL && strcmp(GetShell("ECHO"),"ON")==0)
		fprintf(shell_ostream,"Errors redirected to %s. Give end command to restore.\n",
			Tokens[1]->text);
		Dialogue(shell_istream,shell_ostream,new_errs,prompt_stream,
			"",NULL,&shell);
		fclose(new_errs);
		return 0;
}

int take(SHELLSTREAMS)
{
        int temp;
        FILE *takefile;

	if(numtokens != 2)
		return 1;
	if((takefile = fopen(Tokens[1]->text,"r"))!=NULL){
		strcpy(CurrentFile,Tokens[1]->text);
                temp = shllvl++;
		firstcall = 1;
		set("ECHO","OFF",3,shell_ostream);

		Dialogue(takefile,shell_ostream,shell_errorstream,
			prompt_stream,"",
		NULL,
		(int (*)(FILE *, FILE *, FILE *, FILE *, char *, void *))&shell);

		shllvl = temp;
		fclose(takefile);
		strcpy(CurrentFile,"stdin");
	}
	else
		myerror(shell_errorstream,"Unable to open %s\n",Tokens[1]->text);
	return 0;
}

int help(SHELLSTREAMS)
{
	int i;
	struct ShellCmd *ptr;
	struct ShellCmd TCmd;

	if(numtokens == 1) {
		fprintf(shell_ostream,"Brief descriptions of available shell commands. \n\n");
		for(i=0;i<ncmds;i++)
			fprintf(shell_ostream,"%s\n",CmdPtrs[i]->description);
		fprintf(shell_ostream,"set [name] [value]: Show all shell vars [set name = value].\n");
		fprintf(shell_ostream,"end [name] [value]: Exit shell [set name = value on exit].\n");
		return 0;
	}
	if(numtokens == 2) {
		if(strcmp(Tokens[1]->text,"set")==0){
			fprintf(shell_ostream,"set [name] [value]: Show all shell vars [set name = value].\n");
			return 0;
		}
		if(strcmp(Tokens[1]->text,"end")==0){
			fprintf(shell_ostream,"end [name] [value]: Exit shell [set name = value on exit].\n");
			return 0;
		}
		TCmd.name = common_buf;
		strcpy(TCmd.name,Tokens[1]->text);
		if((ptr = (struct ShellCmd *) binsearch((void *)&TCmd,
			(void **)CmdPtrs,ncmds,
			(int(*) (void *,void *))CompareCmdsByName)) == NULL){
			myerror(shell_errorstream,"Command %s does not exist.\n",Tokens[1]->text);
			return 0;
		}
	fprintf(shell_ostream,"%s\n",ptr->description);
	return 0;
	}
	return 1;
}


int status(SHELLSTREAMS)
{
#ifdef __MSDOS__
	static int firstcall = 1;
	static long core_prev, farcore_prev;
	static int stack_prev;
        long cp,fp;

                fprintf(shell_ostream,"\nTotal stack size: %u\n\n",_stklen);
        if(farheapcheck() == _HEAPCORRUPT)
                fprintf(shell_ostream,"Warning! Heap is corrupt!\n");
        cp = coreleft();
        fp = farcoreleft();
	if(firstcall){
                fprintf(shell_ostream,"Total core left %lu\n",cp);
                fprintf(shell_ostream,"Far heap left: %lu\n",fp);
		fprintf(shell_ostream,"Stack used: %d\n",_stklen-_SP);
		firstcall = 0;
	}
	else {
		fprintf(shell_ostream,"\t\tFar Heap\tTotal\t\tStack used\n");
		fprintf(shell_ostream,"Previous Call:\t%lu\t\t%lu\t\t%d\n",
			farcore_prev,core_prev,stack_prev);
		fprintf(shell_ostream,"Current Call:\t%lu\t\t%lu\t\t%d\n",
                                fp,cp,_stklen - _SP);
		fprintf(shell_ostream,"--------------------------------------------------\n");
                if(fp >= farcore_prev)
        	fprintf(shell_ostream,"Difference: \t%lu\t\t%lu\t\t%d\n",
                        fp-farcore_prev,cp-core_prev,stack_prev - _stklen + _SP);
                else
                fprintf(shell_ostream,"Difference: \t-%lu\t\t-%lu\t\t%d\n",
                        farcore_prev-fp,core_prev-cp,stack_prev - _stklen + _SP);

	}
        core_prev = cp;
        farcore_prev = fp;
	stack_prev = _stklen - _SP;
#else
	fprintf(shell_ostream,"This command is only useful in DOS\n");
#endif

	return 0;
}


int
shift(FILE *errorstream)
{
	int i;
	if(numtokens == 0){
		myerror(errorstream,"Cannot shift\n");
		return 1;
	}
	for(i=1;i<numtokens;i++){
		strcpy(Tokens[i-1]->text,Tokens[i]->text);
		Tokens[i-1]->position = Tokens[i]->position -1;
	}
	numtokens--;
	return 0;
}


int
more(SHELLSTREAMS)
{
	char buffer[MAXPATH];
	FILE *tempfile;
	struct ShellCmd TCmd, *cmdptr;

	/* check usage */
	if(numtokens < 2)return 1;

	/* Prepare to run a shell command from here */
	if(shift(shell_errorstream))return 0;

        /* The set command is a special case since it's not in the ShellCmd
           array */
        if(strcmp(Tokens[0]->text,"set")==0){
                if(numtokens == 1){
                 if((tempfile = fopen(TempFilePath,"w+"))==NULL){
                         myerror(shell_errorstream,"Unable to open %s\n",
			TempFilePath);
                         return 0;
                 }
                 set("","",1,tempfile);
                 fclose(tempfile);
                 strcpy(buffer,"more < ");
                 strcat(buffer,TempFilePath);
                 system(buffer);
                 return 0;
                }
                else{ skip_input = 1;
                      return 0;
                }
        }
	/* Search for a shell command named Tokens[0] */

	TCmd.name = buffer;
	strcpy(TCmd.name,Tokens[0]->text);
        if( (cmdptr = (struct ShellCmd *) binsearch((void *)&TCmd,
                (void **)CmdPtrs,ncmds,
                (int(*) (void *,void *))CompareCmdsByName)) == NULL){
			myerror(shell_errorstream,"No such command %s\n",
				Tokens[0]->text);
			return 0;
	}
	if((tempfile = fopen(TempFilePath,"w+"))==NULL){
		myerror(shell_errorstream,"Unable to open %s\n",
			TempFilePath);
		return 0;
	}

	/* Now run the shell command. Its output goes in the tempfile */

        if((*cmdptr->function)(shell_istream,
        tempfile,shell_errorstream,prompt_stream)==1){
		fclose(tempfile);
		return 1;
	}		

	/* command suceeded. Run system version of more on tempfile */
	
	fclose(tempfile);
	strcpy(buffer,"more < ");
	strcat(buffer,TempFilePath);
	system(buffer);
	return 0;
}

/* read: get a line of input from user, store in shell variable
USER_INPUT. Tokens are available as usual. */
/* If only a return is read then USER_INPUT is not set */

int
myread(SHELLSTREAMS)
{
	char *ptr;

	common_buf[0] = '\0';
	ptr = common_buf;

	Prompt(prompt_stream,SECONDARYPROMPT);

	if(fgets(ptr,MAXLINE,istream) == NULL) return 0;
	ptr = common_buf + strlen(common_buf);

	while((numtokens = tokenize(common_buf,Tokens))==-1)
	{
		Prompt(prompt_stream,SECONDARYPROMPT);
		fgets(ptr,MAXLINE,istream);
		ptr = common_buf + strlen(common_buf);
	}
	
	if(numtokens == 0)set("USER_INPUT","",2,shell_ostream);
	else set("USER_INPUT",common_buf,3, shell_ostream);
	return 0;
}

int
toggledebug(SHELLSTREAMS)
{
	if(debug) debug = 0;
	else debug = 1;
	return 0;
} 

int
uncondbranch(SHELLSTREAMS)
{
	if(numtokens <= 2) return 1;

	if(GetShell(Tokens[1]->text) != NULL){
		shift(shell_errorstream);
		shift(shell_errorstream);
		skip_input = 1;
	}
	return 0;
}
