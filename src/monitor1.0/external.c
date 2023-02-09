/* Implementations of external additions to monitor. Each external command will
be called from monitor.a in response to the command line

Prompt>  foo arg1 arg2 ...

as int _foo( int argc, char **args, char **envp ) 

*/

/* For deeclarations of implemented functions */

int info(int, char **, char **); 

/* This should always equal the number of distinct external commands */

#define N_EXTERNALS 1
int n_externals = N_EXTERNALS;

/* This array of pointers to strings should list the names of the internal
   commands */

char *external_names[N_EXTERNALS] = {
	
	"info",
};

/* This array of pointers to functions should list the entry points of
the internal commands */

int (*external_entry_points[N_EXTERNALS])(int,char **, char**) = {

	&info,
};

/* This array of pointers to strings should list the usage lines for 
the internal_commands */

char *external_usage[N_EXTERNALS] = {

	"info\n",
};

/* This array of pointers to strings should list the help strings for
the internal commands */

char *external_help[N_EXTERNALS] = {
	
	"info: print information about the monitor program",
};


/* Implementations of the external commands go below */


#include <stdio.h>

/* info: just print a goofy banner line */

int info(int argc, char **argv, char **envp)
{ 

	printf("\n\nMonitor, Version %s, by Terry R. McConnell\n\n",VERSION);
	return 0;
}
