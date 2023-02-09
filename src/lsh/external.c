/* Implementations of external additions to monitor. Each external command will
be called from monitor.a in response to the command line

Prompt>  foo arg1 arg2 ...

as int _foo( int argc, char **args, char **envp ) 

*/

/* Forward  declarations of implemented functions */

int info(int, char **, char **); 
int save(int, char **,char **);
#ifdef UNIX
int exec(int, char **, char **);
int cd(int, char **, char **);
#endif

/* This should always equal the number of distinct external commands */

#ifdef UNIX
#define N_EXTERNALS 4
#else
#define N_EXTERNALS 2
#endif
int n_externals = N_EXTERNALS;

/* This array of pointers to strings should list the names of the internal
   commands */

char *external_names[N_EXTERNALS] = {
	
	"info",
	"save",
#ifdef UNIX
	"exec",
	"cd",
#endif
};

/* This array of pointers to functions should list the entry points of
the internal commands */

int (*external_entry_points[N_EXTERNALS])(int,char **, char**) = {

	&info,
	&save,
#ifdef UNIX
	&exec,
	&cd,
#endif
};

/* This array of pointers to strings should list the usage lines for 
the internal_commands */

char *external_usage[N_EXTERNALS] = {

	"info",
	"save file",
#ifdef UNIX
	"exec cmd args ...",
	"cd [path]",
#endif
};

/* This array of pointers to strings should list the help strings for
the internal commands */

char *external_help[N_EXTERNALS] = {
	
	"info: print information about the monitor program",
	"save: save script in file that would restore current environment",
#ifdef UNIX
	"exec: (unix) run cmd args ... in the system shell",
	"cd: change working directory to $HOME [path]",
#endif
};


/* Implementations of the external commands go below */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef UNIX
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#endif

/* info: just print a goofy banner line */

int info(int argc, char **argv, char **envp)
{ 

	printf("\n\nMonitor, Version %s, by Terry R. McConnell\n\n",VERSION);
	return 0;
}

/* save: Opens its first argument and writes a monitor script there that 
would restore the current environment if used as rc file */

int save(int argc, char **argv, char **envp)
{
	char *filename;
	FILE *savefile;
	char *envstr,*p;

	if(argc != 2)return 1;
	filename = argv[1];
	savefile = fopen(filename,"w");
	if(!savefile){
		fprintf(stderr,"save: cannot open %s\n",filename);
		return -1;
	}
	/* Loop over the environment */ 
	while((envstr = *envp++) != NULL){

		/* print set foo bar when envstr is foo=bar */

		fprintf(savefile,"set ");
		p = strchr(envstr,'=');  
		*p = '\0';  /* temporarily terminate foo part */
		fprintf(savefile,"%s ",envstr); 
		*p++ = '='; /* put = sign back in place */ 

		/* the bar part is harder since me must quote all white space */	
		while(*p != '\0'){ 

		/* quote white space */

			if((*p == ' ')||(*p == '\t')||(*p == '\n')){		
				fputc('\\',savefile);
				fputc(*p++,savefile);
				continue;
			}
		/* quote other special chars */

			if((*p == '$')||(*p == '#')||(*p == '\\')){		
				fputc('\\',savefile);
				fputc(*p++,savefile);
				continue;
			}
			fputc(*p++,savefile);
		} /* end loop over chars of value string */
		fputc('\n',savefile); /* newline to finish set command */
	} /* end loop over env */
	fclose(savefile);
	return 0;
}

#ifdef UNIX
/* Under unix, execute the command and args given by argv[1], ... */

int exec(int argc, char **argv, char **envp){

	pid_t child;
	int rval;

	if(argc <= 1)return 1;

	child = fork();
	if(child) wait(&rval);  /* wait for (any) child to quit if parent */
	/* else we are the child */
	else {
	  rval = execve(argv[1], argv+1, envp);
	  exit(rval); /* only gets done if exec fails */
	}

	return rval;
}

int cd(int argc, char **argv, char **envp)
{
	int rval;

	if(argc == 1)
		return chdir(getenv("HOME"));
	return chdir(argv[1]);
}
#endif
