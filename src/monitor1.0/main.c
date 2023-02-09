/* To allow monitor to be assembled directly by nasm while still allowing
   for external commands to be written in c, this file sets up the stack
   the way monitor is expecting it. Any use of the standard C library requires
   linking the c run-time startup code. Among other things this takes the
   stack as set up by the OS  - i.e. exactly what monitor expects -
   and prepares for the call of main. Thus, our main has to undo what
   runtime did. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZ 512
extern void monitor(void);
extern char linebuf1[];
extern char rcscript[];
static char *s;  
static int i,n;

int main(int argc, char **argv, char **envp) {

	FILE *rcfile;
	char *dest = rcscript;
	if(argc > 1){
			rcfile = fopen(argv[1],"r");
			if(!rcfile){
				fprintf(stderr,"Unable to open %s\n",argv[1]);
				return 1;
			}
			while(fgets(linebuf1,BUF_SIZ,rcfile)){
				strcpy(dest,linebuf1);
				dest += strlen(linebuf1);
			}
			*dest = '\0';
			fclose(rcfile);
	}	
			

	/* Put a null word on the stack to indicate top of environment
           string pointers */

	asm volatile (
		"xor %%eax, %%eax\n\t"
		"pushl %%eax\n\t"
		:
		:
		:
	);
	n++;

	/* Stack up the successive environment string pointers ending
           with a NULL word that will mark end of args */

	do
	{
		s = *envp;
		asm volatile (
			"pushl %0\n\t"
			:
			: "r" (s)
			:
		);	
		envp++;
		n++;
	}
	while(*envp != 0);

/* Put a Null byte on stack to indicate end of argv pointers */
/* Not sure why this needs to be done, because I'd think the last time
   through the do ... while loop would already push a null pointer. Maybe
   the compiler optimization is playing games with us ? */

	asm volatile (
		"xor %%eax, %%eax\n\t"
		"pushl %%eax\n\t"
		:
		:
		:
	);

	/* Stack up the successive argument vector string pointers */ 
	/* BUG: these are stacked bass-ackwards (same for environ) */

	for(i=0;i<argc;i++)
	 {
		s = *argv;
		asm volatile (
			"pushl %0\n\t"
			:
			: "r" (s)
			:
		);	
		argv++;
		n++;
	}

	/* Finally, stack up the argc */

	asm volatile (
		"pushl %0\n\t"  
		 :
		 : "r" (argc)
		 :
	);
	n++;

	/* Call the entry point of monitor */

	monitor();

	/* Unstack everything that was put there */

	for(i=0;i<n;i++)
		asm volatile (
			"popl %%eax\n\t"  
		 	:
		 	: 
		 	:
		);
	return 0;
}
