/* Implementations of external additions to monitor. Each external command will
be called from monitor.a in response to the command line

Prompt>  foo arg1 arg2 ...

as int _foo( int argc, char **args, char **envp ) 

*/

/* declaration of set internal command so external commands can call it */
extern int _set(int, char **, char **);

/* Forward  declarations of implemented functions */

int info(int, char **, char **); 
int rcenv(int, char **,char **);
int dump(int, char **,char **);
int toggle(int, char **,char **);
int ephem(int, char **,char **);
int save(int, char **, char **);
int load(int, char **, char **);
int dialog(int,char **,char **);
int port(int,char **,char **);
int drop(int,char **,char **);

/* This should always equal the number of distinct external commands */

#define N_EXTERNALS 10 
int n_externals = N_EXTERNALS;

/* This array of pointers to strings should list the names of the internal
   commands */

char *external_names[N_EXTERNALS] = {
	
	"info",
	"rcenv",
	"dump",
	"toggle",
	"ephem",
	"save",
	"load",
	"dialog",
	"port",
	"drop",
};

/* This array of pointers to functions should list the entry points of
the internal commands */

int (*external_entry_points[N_EXTERNALS])(int,char **, char**) = {

	&info,
	&rcenv,
	&dump,
	&toggle,
	&ephem,
	&save,
	&load,
	&dialog,
	&port,
	&drop,
};

/* This array of pointers to strings should list the usage lines for 
the internal_commands */

char *external_usage[N_EXTERNALS] = {

	"info\n",
	"rcenv file\n",
	"dump [-i] [n | all]] \n",
	"toggle\n",
        "ephem [-u <nn>] date time [ date time [ interval ]]\n",
	"save [-i] n file\n",
	"load [-i] [-t tag ] file\n",
	"dialog [-o] \n",
	"port [-i] [-r name] [n]\n",
	"drop [-i] [n]\n",	

};

/* This array of pointers to strings should list the help strings for
the internal commands */

char *external_help[N_EXTERNALS] = {
	
	"print information about the monitor program",
	"save script in file that would restore current environment",
	"list last orbit [-i: initial conditions ] [ number n | all orbits]",
	"interchange quiet and more verbose operation",
	"print daily [interval] ephemeris for 1 month [until 2nd date] \nfor default orbit [orbit number n]", 
	"append to file elements [-i: initial conditions] of orbit n.\n",
	"load elements [-i initial conditions ] saved in file [only the one with the given tag]\n",
	"Start an elements dialog with the user [-o: osculating orbit]\n",
	"Store last orbit as new initial conditions [-i reverse] [rename name] [orbit n]\n", 
	"Drop last orbit [-i initial condition ] [orbit n]\n",
};

/* Implementations of the external commands go below */

/* These can be stubs that call stuff implemented in external files */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ephemeris.h"
#include<float.h>
#include<math.h>
#include<time.h>
#include "constants.h"

/* info: just print a goofy banner line */

int info(int argc, char **argv, char **envp)
{ 

	printf("\n\nEphemeris, Version %s, by Terry R. McConnell\n\n",VERSION);
	return 0;
}

extern struct orbital_elements * elements_dialog(void);
extern struct orbital_elements * osculating_dialog(void);

/* Right now there is only one dialog (implemented in dialog.c) but others
could easily be added and invoked from here */

int dialog(int argc, char **argv, char **envp)
{
	struct orbital_elements *an_orbit;
	int osculating = 0;

/* process command line */

	if(argc > 0){
	   while(argc && ((*argv)[0] == '-')){
		  if(strcmp(*argv,"-o")==0){
			osculating = 1;
			argv++;argc--;
			continue;
		  }
		  fprintf(stderr, "dialog: Unknown option %s\n", *argv);
		  fprintf(stderr, "dialog [-o ] \n");
		  return 1;
	   }
	}

	if(!osculating)
	if(an_orbit = elements_dialog()){
		orbit_lib[orbit_lib_size++] = an_orbit;
		return 0;
	}
	if(osculating)
	if(an_orbit = osculating_dialog()){
		orbit_lib[orbit_lib_size++] = an_orbit;
		return 0;
	}
	return 1;
}


int drop(int argc, char **argv, char **envp)
{
	int ic = 0;
	int n,j;

	n = nbuiltins + orbit_lib_size;

/* process command line */

	if(argc > 0){
	   while(argc && ((*argv)[0] == '-')){
		  if(strcmp(*argv,"-i")==0){
			ic = 1;
			n = nics;
			argv++;argc--;
			continue;
		  }
		  fprintf(stderr, "drop: Unknown option %s\n", *argv);
		  fprintf(stderr, "drop [-i ] n \n");
		  return 1;
	   }
	}
	if(argc){
		n = atoi(*argv++);
		argc--;
	}

	if(ic){
		if((n<1)||(n>nics)){
			fprintf(stderr,"drop: no such ic number %d\n",n);
			return 1;
		}

		free(ic_lib[n-1]->name);
		free(ic_lib[n-1]);
		for(j=n;j<nics;j++)ic_lib[j-1]=ic_lib[j];
		nics--;
	}
	else {
		if((n<1)||(n>orbit_lib_size+nbuiltins)){
			fprintf(stderr,"drop: no such orbit number %d\n",n);
			return 1;
		}
		if(n<=nbuiltins){
			fprintf(stderr,"drop: cannot drop builtin orbit\n");
			return 1;
		}
		free(orbit_lib[n-nbuiltins-1]->name);
		free(orbit_lib[n-nbuiltins-1]);
		for(j=n;n<orbit_lib_size+nbuiltins;j++)orbit_lib[j-nbuiltins-1]
			=orbit_lib[j-nbuiltins];
		orbit_lib_size--;
	}
	return 0;
}

int port(int argc, char **argv, char **envp)
{
	struct orbital_elements *an_orbit;
	struct initial_conditions *an_ic;
	int ic2orbit = 0;
	int n,k;
	char *newname = NULL;

	n = nbuiltins + orbit_lib_size;

/* process command line */

	if(argc > 0){
	   while(argc && ((*argv)[0] == '-')){
		  if(strcmp(*argv,"-i")==0){
			ic2orbit = 1;
			n = nics;
			argv++;argc--;
			continue;
		  }
		  if(strcmp(*argv,"-r")==0){
			argv++;argc--;
			if(*argv)
				newname = strdup(*argv);
		        argv++;argc--;	
		        continue;
		  }
		  fprintf(stderr, "port: Unknown option %s\n", *argv);
		  fprintf(stderr, "port [-i ] n \n");
		  return 1;
	   }
	}
	if(argc){

		n = atoi(*argv++);
		argc--;
	}

	if(ic2orbit){
		if((n<1)||(n>nics)){
			fprintf(stderr,"port: no such ic number %d\n",n);
			return 1;
		}
		an_orbit = initialconditions2elements(ic_lib[n-1]);
		if(!an_orbit){
			fprintf(stderr,"port failed\n");
			return 1;
		}
		if(newname)
			an_orbit->name = newname;
		orbit_lib[orbit_lib_size++] = an_orbit;
	}
	else {
		if((n<1)||(n>nbuiltins+orbit_lib_size)){
			fprintf(stderr,"port: no such orbit number %d\n",n);
			return 1;
		}
		if(n<=nbuiltins)
			an_ic = elements2initialconditions(&(builtins[n-1]));
		else
			an_ic = elements2initialconditions(orbit_lib[n-nbuiltins-1]);
		if(!an_ic){
			fprintf(stderr,"port failed\n");
			return 1;
		}
		if(newname)
			an_ic->name = newname;
		ic_lib[nics++] = an_ic;
	}
	return 0;
}


/* rcenv: Opens its first argument and writes a monitor script there that 
would restore the current environment if used as rc file */

int rcenv(int argc, char **argv, char **envp)
{
	char *filename;
	FILE *savefile;
	char *envstr,*p;

	filename = *argv;
	savefile = fopen(filename,"w");
	if(!savefile){
		fprintf(stderr,"rcenv: cannot open %s\n",filename);
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

/* Save numbered orbital elements to a file. The elements
are appended to the file. With -i option we save initial
conditions instead  
 */

int save(int argc, char **argv, char **envp)
{

	char *filename;
	FILE *savefile;
	char *envstr,*p;
	int ic = 0;
	unsigned char useenv = 0;
	int j,n;
	struct orbital_elements *an_orbit = NULL;
	struct initial_conditions *an_ic = NULL;
	time_t timenow;
	char buffer[64];

/* process command line */

	if(argc > 0){
	   while(argc && ((*argv)[0] == '-')){
		  if(strcmp(*argv,"-e")==0){
			useenv=1; 
			argv++;argc--;
			continue;
		  }
		  if(strcmp(*argv,"-i")==0){
			ic=1; 
			argv++;argc--;
			continue;
		  }
		  fprintf(stderr, "save: Unknown option %s\n", *argv);
		  fprintf(stderr, "save [-e name | n] file\n");
		  return 1;
	   }
	}
	else {
		  fprintf(stderr, "save [-e name | n] file\n");
		  return 1;
	}

/* there should be 2 more args */

	if(argc != 2) {
		  fprintf(stderr, "save [-e name | n] file\n");
		  return 1;
	}

	if(useenv){
		n = strlen(*argv);
		j=0;
		while(*envp){
			if(strncmp(*envp,*argv,n)==0){
				j = atoi((*envp+n+1));
				break;
			}
			*envp++;
		}
		if(!j)fprintf(stderr,"save:%s not found in environment\n",*argv);
		*argv++;
	}
	else j = atoi(*argv++);

	filename = *argv;
	savefile = fopen(filename,"a+");
	if(!savefile){
		fprintf(stderr,"save: cannot open %s\n",filename);
		return 1;
	}


/* Construct a mnemonic for this orbit */
	printf("You may enter a mnemonic [tag] for this orbit (<63 chars, no whitespace\n");
	printf("If you just hit return at the prompt a timestamp will be used)\n");
	printf("Ephemeris> ");
	buffer[0]='\0';
	fgets(buffer,63,stdin);
	time(&timenow);
	if(strcmp(buffer,"\n")==0)strftime(buffer,63,"%Y%m%d@%H:%M:%S(UTC)\n",
		gmtime(&timenow));

	if(ic){
		if((j<1)||(j>nics)){
		fprintf(stderr,"save: ic number %d, is out of range 1..%d\n",                              j,nics);
		return 1;
		}
		an_ic = ic_lib[j-1];
        	fprintf(savefile,"%s",buffer);
		fprintf(savefile,"%s\n",an_ic->name);
		fprintf(savefile,"%d\n",an_ic->center);
		fprintf(savefile,"%d\n",an_ic->reference);
		fprintf(savefile,"%g\n",an_ic->x);
		fprintf(savefile,"%g\n",an_ic->y);
		fprintf(savefile,"%g\n",an_ic->z);
		fprintf(savefile,"%g\n",an_ic->vx);
		fprintf(savefile,"%g\n",an_ic->vy);
		fprintf(savefile,"%g\n",an_ic->vz);
		fprintf(savefile,"%g\n",an_ic->epoch);
		fprintf(savefile,"%g\n",an_ic->mratio);
		fprintf(savefile,"%s",buffer);
	}
	else {
		if((j<1)||(j>nbuiltins)){
		fprintf(stderr,"save: orbit number %d, is out of range 1..%d\n",                              j,nbuiltins);
		return 1;
		}
		an_orbit = &(builtins[j-1]);

		fprintf(savefile,"%s",buffer);
		fprintf(savefile,"%s\n",an_orbit->name);
		fprintf(savefile,"%d\n",an_orbit->center);
		fprintf(savefile,"%d\n",an_orbit->reference);
		fprintf(savefile,"%g\n",an_orbit->BigOmega);
		fprintf(savefile,"%g\n",an_orbit->i);
		fprintf(savefile,"%g\n",an_orbit->a);
		fprintf(savefile,"%g\n",an_orbit->e);
		fprintf(savefile,"%g\n",an_orbit->omega);
		fprintf(savefile,"%g\n",an_orbit->arg);
		fprintf(savefile,"%g\n",an_orbit->epoch);
		fprintf(savefile,"%g\n",an_orbit->period);
		fprintf(savefile,"%g\n",an_orbit->t0);
		fprintf(savefile,"%s",buffer);
	}

/* clean up */

	fclose(savefile);
	return 0;
}

/* load: load an orbit, i.e., add it to the array of pointers
to orbital_element structs orbit_lib. The elements were saved previously
in a file by the save command.

*/

static char line_buf[81];

int load(int argc, char **argv, char **envp)
{

	char buffer[64];
	int load_one = 0; 
	int ic = 0;
	int n;
	FILE *input_file;
	struct orbital_elements *an_orbit;
	struct initial_conditions *an_ic;

/* process command line */

	if(argc > 0){
	   while(argc && ((*argv)[0] == '-')){
		  if(strcmp(*argv,"-t")==0){
			argv++;argc--;
			if(argc == 0){
		  		fprintf(stderr, "load [-t tag] file\n");
				return 1;
			}
			strcpy(buffer,*argv);
			load_one = 1;
			argv++;argc--;
			continue;
		    }
		   if(strcmp(*argv,"-i")==0){
			argv++;argc--;
			ic = 1;
			continue;
		  }
		  fprintf(stderr, "load: Unknown option %s\n", *argv);
		  fprintf(stderr, "load [-i -t tag] file\n");
		  return 1;
	   }
	}
	else {
		  fprintf(stderr, "load [-i -t tag ] file\n");
		  return 1;
	}
	if(argc == 0){
		 fprintf(stderr, "load [-i -t tag] file\n");
		 return 1;
	}
	input_file = fopen(*argv,"r");
	if(!input_file){
		fprintf(stderr,"load: unable to open %s\n",*argv);
		return 1;
	}
      if(!ic){
	while(fgets(line_buf,80,input_file)){
		
		an_orbit = (struct orbital_elements *)malloc(
			sizeof(struct orbital_elements));
		if(!an_orbit){
			fprintf(stderr,"load: cannot alloc memory\n");
			return 1;
		}
		if(!fgets(line_buf,80,input_file)){
			fprintf(stderr,"load: input corrupt\n");
			free(an_orbit);	
			return 1;
		}
		n = strlen(line_buf);
		line_buf[n-1]='\0'; /* kill the newline */

		an_orbit->name = (char *)malloc(n*sizeof(char));
		if(!an_orbit->name){
			fprintf(stderr,"load: cannot alloc memory\n");
			free(an_orbit);
			return 1;
		}
		strcpy(an_orbit->name,line_buf);

/* center */
		if(!fgets(line_buf,80,input_file)){
			fprintf(stderr,"load: input corrupt\n");
			free(an_orbit);	
			return 1;
		}
		n = strlen(line_buf);
		line_buf[n-1]='\0'; /* kill the newline */
		an_orbit->center = atoi(line_buf);

/* reference */

		if(!fgets(line_buf,80,input_file)){
			fprintf(stderr,"load: input corrupt\n");
			free(an_orbit->name);
			free(an_orbit);	
			return 1;
		}
		n = strlen(line_buf);
		line_buf[n-1]='\0'; /* kill the newline */
		an_orbit->reference = atoi(line_buf);

/* BigOmega */
		if(!fgets(line_buf,80,input_file)){
			fprintf(stderr,"load: input corrupt\n");
			free(an_orbit->name);
			free(an_orbit);	
			return 1;
		}
		n = strlen(line_buf);
		line_buf[n-1]='\0'; /* kill the newline */
		an_orbit->BigOmega = atof(line_buf);

/* i */
		if(!fgets(line_buf,80,input_file)){
			fprintf(stderr,"load: input corrupt\n");
			free(an_orbit->name);
			free(an_orbit);	
			return 1;
		}
		n = strlen(line_buf);
		line_buf[n-1]='\0'; /* kill the newline */
		an_orbit->i = atof(line_buf);

/* a */
		if(!fgets(line_buf,80,input_file)){
			fprintf(stderr,"load: input corrupt\n");
			free(an_orbit->name);
			free(an_orbit);	
			return 1;
		}
		n = strlen(line_buf);
		line_buf[n-1]='\0'; /* kill the newline */
		an_orbit->a = atof(line_buf);

/* e */
		if(!fgets(line_buf,80,input_file)){
			fprintf(stderr,"load: input corrupt\n");
			free(an_orbit->name);
			free(an_orbit);	
			return 1;
		}
		n = strlen(line_buf);
		line_buf[n-1]='\0'; /* kill the newline */
		an_orbit->e = atof(line_buf);

/* omega */
		if(!fgets(line_buf,80,input_file)){
			fprintf(stderr,"load: input corrupt\n");
			free(an_orbit->name);
			free(an_orbit);	
			return 1;
		}
		n = strlen(line_buf);
		line_buf[n-1]='\0'; /* kill the newline */
		an_orbit->omega = atof(line_buf);

/* arg */
		if(!fgets(line_buf,80,input_file)){
			fprintf(stderr,"load: input corrupt\n");
			free(an_orbit->name);
			free(an_orbit);	
			return 1;
		}
		n = strlen(line_buf);
		line_buf[n-1]='\0'; /* kill the newline */
		an_orbit->arg = atof(line_buf);

/* epoch */

		if(!fgets(line_buf,80,input_file)){
			fprintf(stderr,"load: input corrupt\n");
			free(an_orbit->name);
			free(an_orbit);	
			return 1;
		}
		n = strlen(line_buf);
		line_buf[n-1]='\0'; /* kill the newline */
		an_orbit->epoch = atof(line_buf);

/* period */

		if(!fgets(line_buf,80,input_file)){
			fprintf(stderr,"load: input corrupt\n");
			free(an_orbit->name);
			free(an_orbit);	
			return 1;
		}
		n = strlen(line_buf);
		line_buf[n-1]='\0'; /* kill the newline */
		an_orbit->period = atof(line_buf);

/* t0 */
		if(!fgets(line_buf,80,input_file)){
			fprintf(stderr,"load: input corrupt\n");
			free(an_orbit->name);
			free(an_orbit);	
			return 1;
		}
		n = strlen(line_buf);
		line_buf[n-1]='\0'; /* kill the newline */
		an_orbit->t0 = atof(line_buf);
		
		if(!fgets(line_buf,80,input_file)){
			fprintf(stderr,"load: input corrupt\n");
			free(an_orbit->name);
			free(an_orbit);	
			return 1;
		}
		if(load_one){
			if(strcmp(buffer,line_buf)==0){
				orbit_lib[orbit_lib_size++] = an_orbit;
				printf("Loaded %s",buffer);
				break;
			}
			free(an_orbit->name);
			free(an_orbit);
			continue;
		}
		orbit_lib[orbit_lib_size++] = an_orbit;
		printf("Loaded %s",buffer);
	}	
       }
      if(ic){
	while(fgets(line_buf,80,input_file)){
		
		an_ic = (struct initial_conditions *)malloc(
			sizeof(struct initial_conditions));
		if(!an_ic){
			fprintf(stderr,"load: cannot alloc memory\n");
			return 1;
		}
		if(!fgets(line_buf,80,input_file)){
			fprintf(stderr,"load: input corrupt\n");
			free(an_ic);	
			return 1;
		}
		n = strlen(line_buf);
		line_buf[n-1]='\0'; /* kill the newline */

		an_ic->name = (char *)malloc(n*sizeof(char));
		if(!an_ic->name){
			fprintf(stderr,"load: cannot alloc memory\n");
			free(an_ic);
			return 1;
		}
		strcpy(an_ic->name,line_buf);

/* center */
		if(!fgets(line_buf,80,input_file)){
			fprintf(stderr,"load: input corrupt\n");
			free(an_ic);	
			return 1;
		}
		n = strlen(line_buf);
		line_buf[n-1]='\0'; /* kill the newline */
		an_ic->center = atoi(line_buf);

/* reference */

		if(!fgets(line_buf,80,input_file)){
			fprintf(stderr,"load: input corrupt\n");
			free(an_ic->name);
			free(an_ic);	
			return 1;
		}
		n = strlen(line_buf);
		line_buf[n-1]='\0'; /* kill the newline */
		an_ic->reference = atoi(line_buf);

/* x */
		if(!fgets(line_buf,80,input_file)){
			fprintf(stderr,"load: input corrupt\n");
			free(an_ic->name);
			free(an_ic);	
			return 1;
		}
		n = strlen(line_buf);
		line_buf[n-1]='\0'; /* kill the newline */
		an_ic->x = atof(line_buf);

/* y */
		if(!fgets(line_buf,80,input_file)){
			fprintf(stderr,"load: input corrupt\n");
			free(an_ic->name);
			free(an_ic);	
			return 1;
		}
		n = strlen(line_buf);
		line_buf[n-1]='\0'; /* kill the newline */
		an_ic->y = atof(line_buf);

/* z */
		if(!fgets(line_buf,80,input_file)){
			fprintf(stderr,"load: input corrupt\n");
			free(an_ic->name);
			free(an_ic);	
			return 1;
		}
		n = strlen(line_buf);
		line_buf[n-1]='\0'; /* kill the newline */
		an_ic->z = atof(line_buf);

/* vx */
		if(!fgets(line_buf,80,input_file)){
			fprintf(stderr,"load: input corrupt\n");
			free(an_ic->name);
			free(an_ic);	
			return 1;
		}
		n = strlen(line_buf);
		line_buf[n-1]='\0'; /* kill the newline */
		an_ic->vx = atof(line_buf);

/* vy */
		if(!fgets(line_buf,80,input_file)){
			fprintf(stderr,"load: input corrupt\n");
			free(an_ic->name);
			free(an_ic);	
			return 1;
		}
		n = strlen(line_buf);
		line_buf[n-1]='\0'; /* kill the newline */
		an_ic->vy = atof(line_buf);

/* vz */
		if(!fgets(line_buf,80,input_file)){
			fprintf(stderr,"load: input corrupt\n");
			free(an_ic->name);
			free(an_ic);	
			return 1;
		}
		n = strlen(line_buf);
		line_buf[n-1]='\0'; /* kill the newline */
		an_ic->vz = atof(line_buf);

/* epoch */

		if(!fgets(line_buf,80,input_file)){
			fprintf(stderr,"load: input corrupt\n");
			free(an_ic->name);
			free(an_ic);	
			return 1;
		}
		n = strlen(line_buf);
		line_buf[n-1]='\0'; /* kill the newline */
		an_ic->epoch = atof(line_buf);

/* mratio */

		if(!fgets(line_buf,80,input_file)){
			fprintf(stderr,"load: input corrupt\n");
			free(an_ic->name);
			free(an_ic);	
			return 1;
		}
		n = strlen(line_buf);
		line_buf[n-1]='\0'; /* kill the newline */
		an_ic->mratio = atof(line_buf);

		if(!fgets(line_buf,80,input_file)){
			fprintf(stderr,"load: input corrupt\n");
			free(an_orbit->name);
			free(an_orbit);	
			return 1;
		}

		if(load_one){
			if(strcmp(buffer,line_buf)==0){
				ic_lib[nics++] = an_ic;
				printf("Loaded %s",buffer);
				break;
			}
			free(an_ic->name);
			free(an_ic);
			continue;
		}
		ic_lib[nics++] = an_ic;
		printf("Loaded %s",buffer);
	}	
       }
	fclose(input_file);
	return 0;
}

/* dump: dump info on currently loaded orbits */

int dump(int argc, char **argv, char **envp)
{
	int n;
	int ics = 0;

	n = nbuiltins + orbit_lib_size;

/* process command line */

	if(argc > 0){
	   while(argc && ((*argv)[0] == '-')){
		  if(strcmp(*argv,"-i")==0){
			ics=1; 
			n = nics;
			argv++;argc--;
			continue;
		  }
		  fprintf(stderr, "dump: Unknown option %s\n", *argv);
		  fprintf(stderr, "dump [-i] [n]\n");
		  return 1;
	   }
	}
	if(argc){
		if(strcmp(*argv,"all")== 0){
			dump_all_orbits();
			return 0;
		}
		n = atoi(*argv);
	}

	if(ics){
		if((n <= 0)||(n > nics)){
			printf("No such initial condition number %d\n,",n);
			return 1;
		}
		dump_ic(ic_lib[n-1]);
		return 0;
	}
	if((n <= 0)||(n > nbuiltins+orbit_lib_size)){
		printf("No such orbit number %d\n,",n);
		return 1;
	}
	if(n <= nbuiltins)
		info_orbit(&builtins[n-1]);
	else
		info_orbit(orbit_lib[n-nbuiltins-1]);
	return 0;
}

/* toggle the global verbose variable and return its new value */
int toggle(int argc, char **argv, char **envp)
{
static	char *setargv[3];
static	char a[] = "VERBOSE";
static	char b[] = "ON";
static	char c[] = "OFF";

	setargv[0] = (char *)&a;

	if(verbose){
		verbose = 0;
		printf("Operation is now quiet(er)\n");
		setargv[1] = (char *)&c;
		_set(2,(char **)&setargv,envp);
	}
	else{ 
		verbose = 1;
		printf("Operation is now more verbose\n");
		setargv[1] = (char *)&b;
		_set(2,(char **)&setargv,envp);
	}
	return verbose;
}

#define BANNER "\n			Ephemeris (2.0)\n\n"	 
#define USAGE "ephemeris [-u <nn>] date time [ date time [ interval ]]"
extern int secs(char *,double *);
extern int ParseDate(char *,double *);
extern void facts(double);
extern time_t jd2timet(double ajd);

int
ephem(int argc, char **argv,char **envp)
{
	int n = 1,i=1,j;
	int m=1;
	double scale = AU_METERS;
	double secs_after_midnight_utc, epoch,start_epoch,end_epoch,delta_epoch;
	char buf[1024];
	struct event *ecl_event;
	struct spherical_coordinate *cel_coord;
	struct orbital_elements *an_orbit = NULL ;
	time_t atime;
	char timebuf[13] = "Local Date  "; /* After header is printed, this gets overwritten with date strings */

	/* Process command line options. argc will count number of args
            that remain. */

	if(argc > 0){
	   while(argc && ((*argv)[0] == '-')){
		  if(strcmp(*argv,"-u")==0){
			argv++;
			j = atoi(*argv++);
			argc -=2;	
			if((j<1)||(j>nbuiltins+orbit_lib_size)){
				fprintf(stderr,"ephem: argument of -u, %d, is out of range 1..%d\n",j,nbuiltins+orbit_lib_size);
				exit(1);
			}
			if(j<=nbuiltins)
				an_orbit = &(builtins[j-1]);
			else
				an_orbit = orbit_lib[j-nbuiltins-1];
			continue;
		  }
		  fprintf(stderr, "ephemeris: Unknown option %s\n", argv[i]);
		  fprintf(stderr, "%s\n",USAGE);
		  return 1;
	   }
	}
	else {
		  fprintf(stderr, "%s\n",USAGE);
		  return 1;
	}

	
/* If we got here there should be at least two more arguments - the starting
   date and time. If not present, the user needs to be instructed on how
   to use the program */

	if(argc < 2){
		fprintf(stderr, "%s\n",USAGE);
		return 1;
	}

	/* assume what's left on the command is a date followed by a time */

	if(!an_orbit)an_orbit = &(builtins[0]); /* default orbit */

	if(an_orbit->center == EARTH_CENTERED)scale = 1000.0;
	strcpy(buf,*argv++);
	argc--;
	if(ParseDate(buf,&start_epoch)) {
		fprintf(stderr, "ephemeris: cannot parse %s as date\n",
			buf);
		return 1;
	}

	if(argc == 0){
		fprintf(stderr,"ephemeris: time of day missing.\n");
		return 1;
	}
	strcpy(buf,*argv++);
	argc--;
	if(secs(buf,&secs_after_midnight_utc)){
		fprintf(stderr, "ephemeris: couldn't interpret %s as time xx:xx:xx.x\n", buf);
		return 1;
	}
        /* Adjust for actual utc time of day. Recall that the julian day 
            began at the previous noon, UTC */

	start_epoch += (secs_after_midnight_utc)/(24.0*60.0*60.0);  

	/* preset defaults for these */
	delta_epoch = 1.0;
	end_epoch = start_epoch + 30.0;

	/* If there is anything else on the command line it must be
           the optional end_epoch, etc. Repeat above exercise, if so */

	if(argc){
		strcpy(buf,*argv++);
		argc--;
		if(ParseDate(buf,&end_epoch)) {
			fprintf(stderr, "ephemeris: cannot parse %s as date\n",
				buf);
			return 1;
		}
		strcpy(buf,*argv++);
		argc--;
		if(secs(buf,&secs_after_midnight_utc)){
			fprintf(stderr, "ephemeris: couldn't interpret %s as time\n",
				buf);
			return 1;
		}


		end_epoch += (secs_after_midnight_utc)/(24.0*60.0*60.0);  

		/* Finally, if there is an optional delta, fetch and process */

		if(argc){
			strcpy(buf,*argv++);
			argc--;
			if(secs(buf,&delta_epoch)){
				fprintf(stderr, "ephem: couldn't interpret %s as time\n",
					buf);
				return 1;
			}

			/* convert from seconds to days */
			delta_epoch = delta_epoch/(3600.0*24.0);

			/* Do a sanity check: if it's <= 0 we'll loop
                           forever */
			if(delta_epoch < .0001){
				fprintf(stderr,"ephem: crazy delta %g\n",
					delta_epoch);
				return 1;
			}
		}
	}


	printf(BANNER);
	printf("Ephemeris for %s\n\n", an_orbit->name);
	if(verbose)
	printf("%sJD       C.Long  C. Lat    R.A.(h m s)    Declination   r\n",timebuf);
	else
	printf("%sJD       C.Long  C. Lat    R.A.(h m s)    Declination\n",timebuf);
	printf("-----------------------------------------------------------------\n");
	for(epoch = start_epoch; epoch  <= end_epoch; epoch += delta_epoch){
		int hr,min,deg,mm,sign;
		double sec,temp,ss;

		ecl_event = ephemeris(epoch,an_orbit);
		if(!ecl_event){
			fprintf(stderr,"main: ephemeris returned NULL\n");
			exit(1);
		}
		cel_coord = convert_to(ecl_event->where,CELESTIAL);
		if(!cel_coord){
			fprintf(stderr,"ephemeris: error converting to ra/decl\n");
			exit(1);
		}

		/* Express Right Ascension in hours, minutes, seconds */

		temp = DEGREES(cel_coord->phi);
		hr = (int) (temp/15.0);
		temp -= ((double)hr)*15.0;
		temp = temp*4.0;  /* 4 = 60/15: degrees --> minutes of time */
		min = (int)temp;
		temp -= min;
		sec = 60*temp;

		/* Express Declination  in degrees, minutes, seconds */

		temp = DEGREES(cel_coord->theta);
		if(temp<0)sign = -1;
		else sign = 1;
		temp = fabs(temp);
		deg = (int)temp;
		temp -= deg;
		deg = sign*deg;
		mm = (int)(60.0*temp);
		ss = 60.0*(60.0*temp - mm);
		
		/* Figure out the local date corresponding to JD of event */
	    atime = jd2timet(ecl_event->when);
		strftime(timebuf,12,"%x", localtime(&atime));
		printf("%s ",timebuf);
		if(verbose){
		printf("%-12.1f%-8.2f%-8.2f  %2d %2d %5.2f   %3d %2d %5.2f  %6.2f \n",
		  ecl_event->when, DEGREES(ecl_event->where->phi),
                  DEGREES(ecl_event->where->theta),hr,min,sec,deg,mm,ss,
			ecl_event->where->r/scale); 
		}
		else {
		printf("%-12.1f%-8.2f%-8.2f  %2d %2d %5.2f   %3d %2d %5.2f \n",
		  ecl_event->when, DEGREES(ecl_event->where->phi),
                  DEGREES(ecl_event->where->theta),hr,min,sec,deg,mm,ss); 
		}
		free(ecl_event->where);
		free(ecl_event);
		free(cel_coord);
	}
	return 0;
}
