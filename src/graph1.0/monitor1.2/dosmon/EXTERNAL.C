/* Implementations of external additions to monitor. Each external command will
be called from monitor.a in response to the command line

Prompt>  foo arg1 arg2 ...

as int _foo( int argc, char **args, char **envp ) 

*/

/* Forward  declarations of implemented functions */

int info(void); 
int save(int, char **,char **);

/* This should always equal the number of distinct external commands */

#define N_EXTERNALS 2
int n_externals = N_EXTERNALS;

/* This array of pointers to strings should list the names of the internal
   commands */

char *external_names[N_EXTERNALS] = {
	
	"info",
	"save",
};

/* This array of pointers to functions should list the entry points of
the internal commands */

int (*external_entry_points[N_EXTERNALS])(int,char **, char**) = {

	(int (*)(int,char**,char**))&info,
	&save,
};

/* This array of pointers to strings should list the usage lines for 
the internal_commands */

char *external_usage[N_EXTERNALS] = {

	"info\r\n",
	"save file\t\r\n",
};

/* This array of pointers to strings should list the help strings for
the internal commands */

char *external_help[N_EXTERNALS] = {
	
	"info: print information about the monitor program\r",
	"save: save script in file that would restore current environment\r",
};


/* Implementations of the external commands go below */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* info: just print a goofy banner line */

int info(void)
{ 

	printf("\n\nMonitor, Version %s (16 bit DOS), by Terry R. McConnell\n\n",VERSION);
	return 0;
}

/* save: Opens its first argument and writes a monitor script there that 
would restore the current environment if used as rc file */

int save(int argc, char **argv, char **envp)
{
	char *filename;
	FILE *savefile;
	char *envstr,*p;

	if(argc == 0)return -1;

	filename = *argv;
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
