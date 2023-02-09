/* harvest: snip uuencoded sections out of a text file. 
Used to do uploads inside a terminal session 
   (Poor man's kermit.)  */

/* The text file we have in mind is a script transcript of a terminal 
session during which the user might say uuencode foo foo. This will 
cause a block of text to be echo'ed to the screen that has the form

begin 555 foo

...

end

Between these lines (i.e. the ...) is the uuencoding of the file foo. Depending
on what happens in the terminal session there may be many such blocks. 
This routine removes all such sections and writes them to files in the
working directory that have the names given on the begin line. It also
writes these names to stdout for the use of shell scripts that would
process the resulting files (uudecode, e.g.)

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parse.h"

#define MAX_FILENAME 256
#define MAX_LINE 2048
#define MAX_BLOCKS 64

#define VERSION "1.0"
#define USAGE "\nharvest [-hv] [filename] \n"
#define USAGE1 "-h: print this helpful message and exit\n"
#define USAGE2 "-v: print version number and exit\n"
#define USAGE3 "snip uuencoded sections out of input stream\n\n"
#define HELP USAGE USAGE1 USAGE2 USAGE3 

static char buffer[MAX_LINE];
static int nblocks;
static int starts[MAX_BLOCKS]; /* lines where sections start */
static int stops[MAX_BLOCKS];  /* lines where sections stop */
static char *filenames[MAX_BLOCKS];

extern char *parse(FILE *input,int *status);

int
main(int argc, char **argv)
{
	
	char *p;
	int line = 0,i;
	FILE *output = stdout;
	FILE *input = stdin;
	int status;
	char perm[4];
	char filename[MAX_FILENAME];
	int start=-1,stop = -1;
	int in_block = 0;

	/* Process command line options. argc will count number of args
            that remain. */

	
	argv++; argc--; /* skip program name */
    	while((argc > 0) && (*argv)[0] == '-'){
		  if(strcmp(*argv,"-h")==0){
			printf("%s\n", HELP);
			exit(0);
		  }
		  if(strcmp(*argv,"-v")==0){
			printf("%s\n",VERSION);
			return 0;
		  }
		  if(strcmp(*argv,"-x")==0){/* Put experimental stuff here */
			  printf("This is a test, this is ONLY a test!\n");
			  
			  return 0;
		  }
		  fprintf(stderr, "harvest: Unknown option %s\n", *argv);
		  fprintf(stderr, "%s\n",USAGE);
		  return 1;
	   }

	if(argc){
		input = fopen(*argv,"r");
		if(!input){
			fprintf(stderr,"harvest: unable to open %s\n",*argv);
			return 1;
		}
		argv++; argc--;
	}
	while(1){ /* mark beginning and end of uuencoded blocks */

		p = parse(input,&status);
		if(status == MY_ERROR){
			fprintf(stderr,"parse error in line %d\n",line);
			break;
		}
		if(status == MY_EOL)line++;
		if(status == MY_EOF)break;
		if(strcmp(p,"begin")==0){ /* start to check for proper
                                             begin line format */
			p = parse(input,&status);
			if(strlen(p)==3){
				strcpy(perm,p);
				if(!isdigit(perm[0]))continue;
				if(!isdigit(perm[1]))continue;
				if(!isdigit(perm[2]))continue;
			}
			p = parse(input,&status);
			strncpy(filename,p,MAX_FILENAME);
			starts[nblocks] = line;
			in_block = 1;
		}
		if(strcmp(p,"end")==0)
			if(in_block){
				filenames[nblocks]=strdup(filename);
				if(!filenames[nblocks]){
					fprintf(stderr,"harvest: unable to allocate memory\n");
					return 1;	
		
				}
				if(nblocks >= MAX_BLOCKS){
					fprintf(stderr,"harvest: too many encoded file blocks. Recompile with larger limit\n");
					return 1;
				}
				stops[nblocks++] = line;
				in_block = 0;
				continue;
			}
	}

	/* Now go back and cut out each block */

	for(i=0;i<nblocks;i++){
		rewind(input);
		line = 0;
		output = fopen(filenames[i],"w");
		if(!output){
			fprintf(stderr,"Unable to open file %s for write\n",
				filenames[i]);
			return 1;
		}
		while(fgets(buffer,MAX_LINE,input)!=NULL){
			if((line >= starts[i])&&(line <= stops[i]))
				fprintf(output,"%s",buffer);
			line++;
		}
		printf("%s ",filenames[i]);
		fclose(output);
	}
	fclose(input);
	return 0;
}


