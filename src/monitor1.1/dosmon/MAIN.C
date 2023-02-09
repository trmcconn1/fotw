/* To allow monitor to be assembled directly by nasm while still allowing
   for external commands to be written in c, this file sets up the stack
   the way monitor is expecting it. Any use of the standard C library requires
   linking the c run-time startup code. Among other things this takes the
   stack as set up by the OS 
   and prepares for the call of main. Thus, our main has to undo what
   runtime did. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZ 512
extern void monitor(void);
extern char linebuf1[];
extern char rcscript[];
extern char copy_env;
extern char alone;
static char *s;  
static int i,n;

#define USAGE "monitor [-v -h -e] [file]"
#define HELP "command monitor with startup file"
#define HELP1 "-v: print version number and exit"
#define HELP2 "-h: print this helpful information and exit"
#define HELP3 "-e: include OS environment in program environment"

int main(int argc, char **argv, char **envp) {

	FILE *rcfile;
	char *dest = rcscript;

	argv++; /* skip program name */
	argc--; /* and reduce arg count by one */

	while(argc && ((*argv)[0]=='-')){
		switch((*argv)[1]){
			case 'v':
				printf("%s\n",VERSION);
				return 0;
			case 'h':
				printf("%s\n",USAGE);
				printf("%s\n",HELP);
				printf("%s\n",HELP1);
				printf("%s\n",HELP2);
				printf("%s\n",HELP3);
				return 0;
			case 'e':
				copy_env = 1;	
				break;
			default:
				fprintf(stderr,"monitor: -%c unknown option\n", 
				      (*argv)[1]);
				fprintf(stderr,"%s\n",USAGE);
				return 1;
		}
		--argc;argv++;
	}
	if(argc > 0){
		rcfile = fopen(*argv,"rb");
		if(!rcfile){
			fprintf(stderr,"Unable to open %s\n",*argv);
				return 1;
		}
		while(fgets(linebuf1,BUF_SIZ,rcfile)){
			strcpy(dest,linebuf1);
			dest += strlen(linebuf1);
		}
		*dest = '\0';
		fclose(rcfile);
	}
	
	alone = 0;  /* Tell monitor not to run in stand-alone mode */

	/* Put a null word on the stack to indicate top of environment
           string pointers */

	_asm { xor ax, ax 
	       push ax
	};
	n++;

	/* Stack up the successive environment string pointers ending
           with a NULL word that will mark end of args */

	do
	{
		s = *envp;
		_asm {
			mov ax,s
			push ax
		};
		envp++;
		n++;
	}
	while(*envp != 0);

/* Put a Null byte on stack to indicate end of argv pointers */
/* Not sure why this needs to be done, because I'd think the last time
   through the do ... while loop would already push a null pointer. Maybe
   the compiler optimization is playing games with us ? */

	_asm {
		xor ax, ax
		push ax
	};
	n++;

	/* Stack up the successive argument vector string pointers */ 
	/* BUG: these are stacked bass-ackwards (same for environ) */

	for(i=0;i<argc;i++)
	 {
		s = *argv;
		_asm {
			mov ax,s
			push ax
		};
		argv++;
		n++;
	}

	/* Finally, stack up the argc */

	_asm  {
		mov ax,argc
		push ax
	};
	n++;

	/* Call the entry point of monitor. Monitor does not return. */

	monitor();

	/* Unstack everything that was put there.
            */


	for(i=0;i<n;i++)
		_asm {
			pop ax
		};
	return 0;
}
