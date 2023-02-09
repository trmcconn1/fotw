/* shell.c: pared-down command interpreter for use with newsmstr program*/
/* This really isn't used at all at present, other than to read the
    rc file -- and even that doesn't accomplish anything. But it's here
    if it's ever needed */

#include "shell.h"

void Prompt( FILE * , char * );
static int firstcall = 1;
static int shllvl = 0;

char COLS[LONGNAME] = "2";


int nvars;
int numtokens;
struct ShellVar *ShellVars[MAXVARS];
char *GetShell(char *); /* return value of shell variable */

int line = 0;
int tokenize(char *line,struct TokenType **tokens);
/* Declarations of shell commands */

int ncmds; 
static struct ShellCmd *CmdPtrs[MAX_CMDS];



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
int help(SHELLSTREAMS);
int status(SHELLSTREAMS);


/* Declaration of the array of all shell commands except set and end */


struct ShellCmd {
char *name;
char *usage;
char *description;
int (*function)(FILE *, FILE *, FILE *,FILE *);    
} Cmds[] = 
{
/*          echo    */
 "echo", 
"echo [-n] <args>",
"echo [-n] [ ... ]: print ... on the shell's output stream.",
echo,
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
"status",
"status",
"status: print information about memory usage (DOS only)",
status,
"read",
"read",
"read: get a line of user input. Store in $USER_INPUT",
myread
};

int CompareVarsByName(struct ShellVar *, struct ShellVar *);
int CompareCmdsByName(struct ShellCmd *, struct ShellCmd *);
int CompareTokensByPosition(struct TokenType *, struct TokenType *);

char common_buf[BUFSIZ];
char littlebuf[LONGNAME];

int 
shell(SHELLSTREAMS,char *errmsg, void *data)
{

/* This routine processes one line for each call: The line may be retained for
	append, ignored,  or parsed and a command executed. */

	static char buffer[MAXLINE];  /* input buffer */
	static char PromptString[PROMPTLEN];
	int cmdlen;
	int i;
       
	static int cmdline = 0;  /* current line of input */
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
		 

/* See if the user has supplied a prompt. Use default otherwise */

	if(GetShell("PROMPT") != NULL)
	{	SAFECPY(PromptString,GetShell("PROMPT"),PROMPTLEN);}
	else
		strcpy(PromptString,"newsmstr> ");

/* Display our handsome prompt */

	if(strcmp("OFF",GetShell("ECHO"))!=0)
	Prompt(prompt_stream,PromptString);

/* Get a command line from input */
/* If the cmdlen is zero, then user hit ^D, so dialogue is over */

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
	fprintf(promptstream, "%s ",PromptString);
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
		system("command"); /* command.com */
#else 
		if(GetShell("SHELL")==NULL)
			strcpy(common_buf,"/bin/sh ");
		else {
			strcpy(common_buf,GetShell("SHELL"));
			for(i=1;i<numtokens;i++){
			strcat(common_buf," ");
			strcat(common_buf,Tokens[i]->text);
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
		temp = shllvl;
		shllvl++;
		firstcall = 1;
		set("ECHO","OFF",3,shell_ostream);

		Dialogue(takefile,shell_ostream,shell_errorstream,
			shell_ostream,"",
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
			fprintf(shell_ostream,"%s\n",Cmds[i].description);
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

/* read: get a line of input from user, store in shell variable
USER_INPUT, return number of bytes read. Tokens are available as usual. */

int
myread(SHELLSTREAMS)
{
	char *ptr;

	common_buf[0] = '\0';
	ptr = common_buf;


	
	Prompt(prompt_stream,"newsmstr> ");

	if(fgets(ptr,MAXLINE,stdin) == NULL) return 0;
	ptr = common_buf + strlen(common_buf);

	while((numtokens = tokenize(common_buf,Tokens))==-1)
	{
		Prompt(prompt_stream,SECONDARYPROMPT);
		fgets(ptr,MAXLINE,stdin);
		ptr = common_buf + strlen(common_buf);
	}
	
	set("USER_INPUT",common_buf,3, shell_ostream);
	return strlen(common_buf);
}


		

/* Break Line into tokens delimited by DELIMITER and return number of
      tokens found. Return -1 if more input is needed. Implements
	metacharacter escapes, quoting, and variable substitution
*/
/* This is an absolutely central routine for the operation of all the
   parsers in this file, as well as of the command line shell */

int tokenize(char *line,struct TokenType **tokens)
{
	int count = 0;
	char *ptr,*ptrhold;
	char buffer[MAXLINE];
	char tokenbuf[MAXLINE]; /* hold a token while building */
	char holdbuffer[MAXLINE];
	enum {NORMAL, ESCAPED};
	char  *bufptr, *cptr;
	int state = NORMAL;
	int len;  /* Used during variable expansion */


	SAFECPY(buffer,line,MAXLINE);

	if(buffer == NULL) return 0;


	ptr = buffer; 
	bufptr = holdbuffer;


		if(strlen(buffer)>=MAXLINE){ 
		myerror(errorstream,"Line %s too long\n",buffer);
			return 0;
		}
		
/* Skip comment lines and lines starting with \n */

		if((buffer[0] == COMMENT)||(buffer[0]=='\n'))
			return 0;


/* First pass: collapse all multiple escapes. Determine whether newline
is escaped and request more input if so. Expand shell variables. */             
/* The processed line ends up in holdbuf */


	while( *ptr != '\0'){
		if(bufptr - holdbuffer >= MAXLINE-1){
			myerror(errorstream,"Line %s became too long during variable expansion\n",buffer);
			break;
		}
		if(state == NORMAL)
	  /* Is it a meta char ? if not, just copy */
			if((cptr = strchr(METACHARS,*ptr)) == NULL)
				*bufptr++ = *ptr++;
			else {  /* if so, see which one */
				switch(*cptr) {
				case '\\':
					if(strchr(METACHARS,*(++ptr))==NULL)
					*bufptr++ = '\\';   /* skip it */
					else state = ESCAPED;
					break;
				case '$':
/* If the next character is a { grab everything up to } as a token. It should
be the name of an existing shell variable. If not, complain and copy. */
					if(*(ptr+1)=='{') {
						len=strcspn(ptr+2,"}");
						strncpy(tokenbuf,ptr+2,len);
						ptr=ptr+2;
					}
					else {
/* Grab stuff till next whitespace or quote as a token. It should be the name of
    an existing shell variable. If not, complain and copy (no substitution. */
					len = strcspn(ptr+1,"\" \t\n");
					strncpy(tokenbuf,ptr+1,len);
					}
					*(tokenbuf + len) = '\0';
					if(GetShell(tokenbuf) == NULL){
					   myerror(errorstream,"No such shell variable %s\n",tokenbuf);
		
					  *bufptr++ = *ptr++;
					   break;
					}
					strcpy(bufptr,GetShell(tokenbuf));
					ptr += len + 1;
					bufptr += strlen(GetShell(tokenbuf));
					break;
				 default:
					*bufptr++ = *ptr++;
				}
			}
		else  { /* state is ESCAPED */
			switch(*ptr){
				case '\n': /* substitute a space */
				       *bufptr++ = ' ';
					ptr++;
					break;
				case '\\':
					/* if escaped backslash followed
					   by a quote, insert whitespace */
					*bufptr++ = *ptr++;
					if(*ptr=='\"')
					*bufptr++ = ' ';
					break;
				case '\"': /* special case: keep escape in 
					    front of a quote -- it gets
					    handled on the next pass. */
					*bufptr++ = '\\';
					*bufptr++ = *ptr++;
					break;
				default:
				*bufptr++ = *ptr++;
			}
				state = NORMAL;
		}
	}

/* properly terminate buffer */
	*bufptr = '\0';
/* copy the hold buffer back over the original one */
	strcpy(buffer, holdbuffer);

/* Check whether last character was an escaped newline */
	if(*(buffer+strlen(buffer)-1)!='\n')
		return -1; /* need more input */


/* OK, now all escapes have been fixed. */

/* Next find all quote delimited tokens */


/* Find the first unescaped quote */

	ptr = buffer;
	while(((ptr = strchr(ptr,'\"'))!=NULL) &&
		(ptr > buffer) && (*(ptr-1)=='\\')){
			*(ptr-1)=' ';  /* change \ to blank */
			ptr++;
		}
	while(ptr != NULL){   /* ptr pts at leading quote */
		ptrhold = ptr++;  /* save it */

/* advance to next unescaped quote */ 
		while(((ptr = strchr(ptr,'\"'))!=NULL) &&
		  (*(ptr-1)=='\\')){
			*(ptr-1) = ' ';  /* change \ to blank */
			ptr++;
		}

	if(ptr == NULL){
		myerror(errorstream,"Unmatched \" in %s\n",buffer);
		return 0;
	}

/* ptr now points at close quote. blank and copy into token buffer */

	*ptr = '\0';         /* mark end of string */
	*ptrhold++ = ' ';    /* convert leading quote to whitespace */
	SAFECPY((*tokens)->text,ptrhold,MAXTOKEN);
	(*tokens++)->position = ptrhold - buffer;
	if(count >= MAXTOKS){
			myerror(errorstream,"Too many tokens. Must be < %d\n",
				MAXTOKS);
			return count;
		}

	count++;
	while(ptrhold <= ptr)   /* convert token just found to whitespace */
	   *ptrhold++ = ' ';
/* advance to next unescaped quote */ 
	while(((ptr = strchr(ptr,'\"'))!=NULL) &&
	  (*(ptr-1)=='\\')){
		*(ptr-1) = ' ';
		ptr++;
		}
	}


/* Get tokens delimited by white space */

	ptr = strtok(buffer,DELIMITER);
	while(ptr != NULL){
		if(count >= MAXTOKS){
			myerror(errorstream,"Too many tokens. Must be < %d\n",
				MAXTOKS);
			return count;
		}
		SAFECPY((*tokens)->text,ptr,MAXTOKEN);
		(*tokens++)->position = ptr -buffer;
		count++;
		ptr = strtok(NULL,DELIMITER);
	}

/* Sort the tokens into the order they occured */
	krqsort((void **)Tokens,0,count-1,
	  (int(*) (void *,void *))CompareTokensByPosition);
	return count;
}

