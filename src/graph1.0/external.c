/* Implementations of external additions to monitor. Each external command will
be called from monitor.a in response to the command line

Prompt>  foo arg1 arg2 ...

as int _foo( int argc, char **args, char **envp ) 

*/

/* Forward  declarations of implemented functions */

int info(int, char **, char **); 
int save(int, char **,char **);
int readdefs(int, char **, char **);
int graphinfo(int, char **,char **);
int euler(int, char **, char **);
int exec(int,char **,char **);
int delete(int,char **,char **);
int savedef(int, char **, char **);
int rename_graph(int, char **, char **);
int mst(int, char **, char **);
int copy(int, char **, char **);

/* This should always equal the number of distinct external commands */

#define N_EXTERNALS 11
int n_externals = N_EXTERNALS;

/* This array of pointers to strings should list the names of the internal
   commands */

char *external_names[N_EXTERNALS] = {
	
	"info",
	"save",
	"readdefs",
	"graphinfo",
	"euler",
	"exec",
	"delete",
	"savedef",
	"rename",
	"minsptree",
	"copy",
};

/* This array of pointers to functions should list the entry points of
the internal commands */

int (*external_entry_points[N_EXTERNALS])(int,char **, char**) = {

	&info,
	&save,
	&readdefs,
	&graphinfo,
	&euler,
	&exec,
	&delete,
	&savedef,
	&rename_graph,
	&mst,
	&copy,
};

/* This array of pointers to strings should list the usage lines for 
the internal_commands */

char *external_usage[N_EXTERNALS] = {

	"info\n",
	"save file\n",
	"readdefs [files]\n",
	"graphinfo [i]\n",
	"euler i\n",
	"exec [cmd args...]\n",
	"delete i\n",
	"savedef i file\n",
	"rename i name\n",
	"minsptree i\n",
	"copy i\n",
};

/* This array of pointers to strings should list the help strings for
the internal commands */

char *external_help[N_EXTERNALS] = {
	
	"info: print information about the monitor program",
	"save: save script in file that would restore current environment",
	"readdefs: read graph definitions from files in turn",
	"graphinfo: print graph names [ graph i details ]",
	"euler: display euler cycle of graph number i, if it has one",
	"exec: execute cmd args in the shell",
	"delete: delete graph number i",
	"savedef: save definition of graph i in file",
	"rename: change name of graph i to name",
	"minsptree: create new graph = a minimal spanning tree of i",
	"copy: create exact copy of graph i, appropriately renamed",
};


/* Implementations of the external commands go below */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* info: just print a goofy banner line */

int info(int argc, char **argv, char **envp)
{ 

	printf("\n\nMonitor, Version %s, by Terry R. McConnell\n\n",VERSION);
	return 0;
}

/* Remove graph number i from the list of graphs. Return 0 for
success, 1 for failure  */

extern int delete_graph(int);

int delete(int argc, char **argv, char **envp)
{
	int i;
	i = atoi(argv[0]);
	if(!delete_graph(i)){
		fprintf(stderr,"Unable to delete graph %i\n",i);
		return 1;
	}
	return 0;
}

/* save: Opens its first argument and writes a monitor script there that 
would restore the current environment if used as rc file */

int save(int argc, char **argv, char **envp)
{
	char *filename;
	FILE *savefile;
	char *envstr,*p;

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

#include "graph.h"

/* readdefs: reads graph definitions from file named as arg[0] */

extern do_readdefs(int,char **);

int readdefs(int argc, char **argv, char **envp){

	do_readdefs(argc,argv);
	
}

int graphinfo(int argc, char **argv, char **envp){

	int j;

	if(argc == 0){

		printf("\nCurrently defined graphs:\n");
		for(j=0;j<ngraphs;j++)
			printf("%d. %s\n",j,(my_graphs[j]).name);
		return 0;
	}
	
	j = atoi(argv[0]);
	if(j >= ngraphs) {
		fprintf(stderr,"Graph %d is not defined\n",j);
		return -1;
	}
	dump_info(&(my_graphs[j]));
	dump_def(&(my_graphs[j]),stdout);
	return 0;
}

int euler(int argc, char ** argv, char **envp){

	int j;
	struct walk_struct *w;

	j = atoi(argv[0]);
	if(j >= ngraphs) {
		fprintf(stderr,"Graph %d is not defined\n",j);
		return -1;
	}
	w = fleury(&(my_graphs[j]));
	if(!w){
		printf("Graph %d is not eulerian\n",j);
		return -1;
	}
	dump_walk(w);
	return 0;
}	

static char buffer[1024];

int exec(int argc, char **argv, char **envp){

	int i;
	sprintf(buffer,"%s ",argv[0]);
	for(i=1;i<argc;i++){
		strcat(buffer," ");
		strcat(buffer,argv[i]);
	}
	return system(buffer);
}


int savedef(int argc, char **argv, char **envp){

	int i;
	struct graph_struct *g;
	FILE *ostream;

	i = atoi(argv[0]);
	g = &(my_graphs[i]);
	
	ostream = fopen(argv[1],"w");
	if(!ostream){

		fprintf(stderr,"savedef:cannot open %s\n",argv[1]);
		return FALSE;
	}
	dump_def(g,ostream);
	fclose(ostream);
	return TRUE;
}
	

int rename_graph(int argc, char **argv, char **envp){

	int i;
	struct graph_struct *g;

	i = atoi(argv[0]);
	if(i >= ngraphs){
		fprintf(stderr,"rename: no such graph %d\n",i);
		return FALSE;
	}
	g = &(my_graphs[i]);
	if(g->name)free(g->name);
	g->name = strdup(argv[1]);
	return TRUE;
}

/* creat a minimal spanning tree of numbered graph */

int mst(int argc, char **argv, char **envp){
	int i;
	struct graph_struct *g;
	i = atoi(argv[0]);
	if(ngraphs == MAX_GRAPHS){
		fprintf(stderr,"minsptree: %d = MAX_GRAPHS reached. Cannot create.\n",
			MAX_GRAPHS);
		return FALSE;
	}
	if(i >= ngraphs){
		fprintf(stderr,"minsptree: no such graph %d\n",i);
		return FALSE;
	}
#ifdef USE_PRIM
	if(prim(&(my_graphs[i]))==FALSE){
		fprintf(stderr,"minsptree: unable to create spanning tree\n");
		return FALSE;
	}
#else
	if(kruskal(&(my_graphs[i]))==FALSE){
		fprintf(stderr,"minsptree: unable to create spanning tree\n");
		return FALSE;
	}

#endif

	return TRUE;
}

int copy(int argc, char **argv, char **envp){
	int i;
	i = atoi(argv[0]);
	if(ngraphs == MAX_GRAPHS){
		fprintf(stderr,"copy: %d = MAX_GRAPHS reached. Cannot create.\n",
			MAX_GRAPHS);
		return FALSE;
	}
	if(copy_graph(&(my_graphs[i])))return TRUE;
	fprintf(stderr,"copy: copy_graph failed on  graph %d\n",i);
	return FALSE;
}
