/* monitor: very basic, generic command interpreter */

#include "macros.h"
#include "internal.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


	int promptlen = sizeof(PROMPT_STRING);
	extern int debug;
	char msg0[] = "Enter help for a list of commands, ^D to exit\n\n";
	char copy_env = 0;
	char *source =  NULL;           /* =0 for stdin, memory reference else */
	char *source_orig = 0; /* needed to support branching in scripts */
	char alone   =  1;           /* set to 0 by main */
	int envc=0;
	int nargs=0;
	void *my_ptr;     /* pointer to whatever */
	static char expandbuf[BUF_SIZ];
	static char linebuf1[BUF_SIZ]; /* buffer area to read into */
	static char linebuf2[BUF_SIZ];   /* storage for parsed command line */
	static char linebuf3[BUF_SIZ];   /* storage for parsed command line */
	static char linebuf4[BUF_SIZ];   /* storage for parsed command line */
	char *lineargs[BUF_SIZ];   /* pointers to parsed line strings */
	char *environment[ENV_SIZ];   /* environment pointers storage */
	extern FILE *rcfile;
	extern int n_externals;
	extern char *external_names[];
	extern int (*external_entry_points[])(int,char **,char **);


/* This is an "rc file" that is really just to test our ability
   to read from memory */

	char default_rc[] = "info\nset RCFILE DEFAULT";

/* Memory management- make sure memory is filled with nulls at start */

	static char data[DATA_SIZ];/*storage for stuff eg environment strings*/
        static int data_bytes=0; /* amount of memory used */

/* Return pointer to next available n bytes of memory */
/* Next available is where there are n+1 nulls in a row */
/* We have to avoid stepping on the null terminator of a block that 
is in use */

char *grab(int n)
{
	int i = 0,j=0;

	while(1){
		if(*(data+i)){i++;j=0;}
		else {i++;j++;}
		if(i >= DATA_SIZ)return NULL;
		if(j >= n+1){
			data_bytes += n;
			return data+i-n+1;
		}
	}
}
	
/* release previously grabbed memory: fill biggest non-null section
of memory from m with nulls  */

void release(char *m)
{
	int i=0;
	if(m)while((m-data+i < DATA_SIZ)&&(*(m+i) != '\0'))*(m+i++) = '\0';
	data_bytes -= i;
		
}

/* BUG: over time, the data area will become fragmented. At some point
we may need a defrag user command */

/* This just prints a little report used by status command */

void memory(void)
{
	int i = 0;
	char *m,c;
	double p,q;

	printf("Data buffer size = %d\n",DATA_SIZ);
	printf("Bytes used = %d\n",data_bytes);
	m = data+DATA_SIZ;
	while(m>data){
		if(*--m)break;
		i++;
	}
	printf("Bytes in last unused block = %d\n",i);
	p = (double)data_bytes;
	q = (double)(DATA_SIZ - i);
	printf("Fragmentation = %3.1f%%\n",(1.0 - p/q)*100.0);
	if(debug){ /* dump a picture of start of used memory */
		for(i=0;i<79;i++){
			c = data[i];
			if(c)putchar(c);
			else putchar('*');
		}
		putchar('\n');
	}
	
	return;
} 

/* Self-explanatory. Make sure unused env slots are null pointers  */

char *my_getenv(char *p, char **envp)
{

	char *e,*q;
	int i = 0;

	if(!p)return NULL;
	e = envp[i++];
	while(e){
		q = strchr(e,'=');
		*q = '\0';
		if(strcmp(e,p)==0){
			*q = '=';
			return q+1;
		}
		*q = '=';
		e = envp[i++];
	}
	return NULL;
}
	
/* Input processing */

int expand(char *d, char *s, FILE *input) 
{
	char escape_on = 0;
	char in_whitespace = 0;
	char c,*p,*q;

	if(!s)return 1;
	while(*s){
	c = *s;
	if((c!=' ')&&(c!='\t'))in_whitespace = 0;
	switch(c) {

		case '\\':
			if(escape_on){
				escape_on = 0;
				break;
			}
			else {
				escape_on = 1;
				s++;
				continue;
			}	
		case  ' ':
		case  '\t':

		/* if ongoing run of whitespace */

			if(in_whitespace && escape_on){
				escape_on = 0;
				break;
			}
			if(in_whitespace) { /* but no escape */
				s++;
				continue; /* skip extra white space */
			}

	        /* else */

			if(escape_on) {
				escape_on = 0;
				break;
			}
			/* else not escaped and not in whitespace */
			in_whitespace = 1;
			c = '\0';
			break;

		case '$':
		/* Variable expansion will be done later on the args. Here
                 we just need to double the $ so
                 the later processing will know not to do the expansion */

			if(escape_on) {
				*d++ = '$';
				escape_on = 0;
				break;
			}

			break;
		case '\n':
			if(!escape_on){
				c = '\0';
				break;
			}

		/* Here's where we allow for a line continuation */

			if((source == NULL)&&(input == stdin)) printf("%s","?");
		        if(!fgets(linebuf1,BUF_SIZ,input))return 1;
			s = linebuf1;
			*d++ = c;
			escape_on = 0;
			continue;

	} /* end switch */ 
	*d++=c;
	s++;
	if(escape_on)escape_on=0;
	}  /* end while */
	return 0;
}

/* entry point for the monitor */

int monitor(int argc, char **argv, char **envp){

	FILE *input = stdin;
	long position;
	ICMD cmd;
	char *p,*q,*r,c;
	int i=0,n,j,escape,found;

/* Initialize internal command jump table */

	initialize_jumptable();

/* Copy calling shell environment if needed */
	if(copy_env){
	    while(1){
		p = envp[envc];
		if(p==NULL)break;
		n = strlen(p)+1;
		if(n+data_bytes >= DATA_SIZ)break;
		strncpy(data+data_bytes,p,n);
		environment[envc++] = data + data_bytes;
		data_bytes+=n;	
		if(envc >= ENV_SIZ) break;
	     }
	}

/* Make sure rest of env pointers are NULL */
	for(i=envc;i<ENV_SIZ;i++)environment[i]=NULL;

/* Make sure unused rest of data area is nulled */
	release(data+data_bytes);

/* 
 set $0,$1, ... to the contents of strings passed
 as arguments to monitor
 

 We will use   expandbuf to hold the temporary
 strings that represent 0,1, ... 9

*/

	strcpy(linebuf1,"set");
	for(i=0;i<argc;i++){
		sprintf(expandbuf,"%c",'0'+i);
		lineargs[0]=linebuf1;
		lineargs[1]=expandbuf;
		lineargs[2]=argv[i];
		Set(3,lineargs,environment);
	}

/* If main did not find an rc file, use our little default
internal one */

	if(rcfile)input = rcfile;
	else {
		source = default_rc;
		source_orig = source;
	}

/* main loop */

	while(1){

		/* clean up from last round */
		for(i=0;i<nargs;i++)lineargs[i]=NULL;
		for(i=0;i<BUF_SIZ;i++)linebuf2[i]='\0';
		nargs = 0;

		if(source == NULL){ /* not reading from memory */
			if(input==stdin)
				printf("%s",PROMPT_STRING);
/* if EOF is read from terminal, quit program, else we were reading
from a file and go back to the terminal */
			if(!fgets(linebuf1,BUF_SIZ,input)){
				if(input == stdin)break;
				else {
					input = stdin;	
					continue;
				}
			}
		}
		else { /* we are reading from memory */
			n = strcspn(source,"\n");
			if(!n) {
				input = stdin;
				source = NULL;
				continue;
			}
			if(n>=BUF_SIZ-1)n = BUF_SIZ - 2;
			strncpy(linebuf1,source,n+1);	
			linebuf1[n+1]='\0';
			source += strlen(linebuf1);	
		}
		linebuf1[BUF_SIZ-2] = '\n'; /* Just in case */
		linebuf1[BUF_SIZ-1] = '\0';

/* skip empty lines */
		if(strlen(linebuf1)==strspn(linebuf1," \t\n"))continue;

/* If the line begins with a # (comment) or : (label) skip it */
		if(linebuf1[0]=='#')continue;
		if(linebuf1[0]==':')continue;

/* process the input line for extra whitespace, line continuation with
   escaping */
		expand(linebuf2,linebuf1,input);

/* set up argv for command */

		i=0;
		p = linebuf2;
		while(*p){
			lineargs[i++]=p;
			p += strlen(p)+1;
		}
		nargs = i;

/* Now see if any args have $ symbols in them so we can do variable
expansion. At this stage $ escapes itself, so we should expand 
variable names that occur with a leading unescaped $ */

		/* expanded args build up in linebuf3 */ 
		r = linebuf3;
		linebuf3[0]='\0';

		for(i=0;i<nargs;i++){
			/* quick test to see if arg i needs expansion  */
		     if(strchr(lineargs[i],'$')){
				/* yes, so build new arg in expandbuf */
			expandbuf[0]='\0';
			escape = 0;
			j=0;
			p = lineargs[i]; /* The arg string pointed to by p
				will stay in  linebuf2 but will not be 
                                used. */
			while(*p){
				while(*p == '$'){ /* handle $s in a row */
					escape = 1;
					p++;
					if(*p == '$'){
						expandbuf[j++]='$';
						expandbuf[j]='\0';
						escape = 0;
						p++;
					}
				}
				if(!escape){ /* no pending $ */
					/* continue to build arg */
					expandbuf[j++]=*p++;
					expandbuf[j]='\0';
					continue;
				}
				/* pending $, so we need to gather var name */
				q=p; /* peg start of name */
				while(*p && (*p !='$'))p++;
                                c = *p; /* remember for restore later */
				*p='\0'; /* peg end of name */
				q = my_getenv(q,environment); /* get value */
				if(q){
				  strcat(expandbuf+j,q); /* tack it on */
				  j+=strlen(q); /* find new end of arg */
				}
				*p = c;
			} /* done expanding current arg into expandbuf */
			/* BUG: degpending on how much expansion took place
                          we could overrun linebuf3 */
			strcat(r,expandbuf); /*recall:r points into linebuf3*/	
			lineargs[i] = r;
			r += strlen(expandbuf);
		     } /* Done expanding ith arg */
			/* process next arg */
		}

/* Ready to roll. Call the entry point of the command */

		p = lineargs[0];

/* Pop is a special case. Skip first arg (=pop) and write a new
command line using the rest into linebuf2. Arrange to read from
linebuf2 and go around again.  */

		if(strcmp(p,"pop")==0){
			linebuf4[0] = '\0';
			for(i=1;i<nargs;i++){
				strcat(linebuf4,lineargs[i]);
				strcat(linebuf4," ");
			}
			strcat(linebuf4,"\n");
			source = linebuf4;
			source_orig = linebuf4;
			continue;
		}

/* If is also a special case. Sigh, too many special cases */

		if(strcmp(p,"if")==0){
			if(nargs < 4)continue; /* nothing to do */
			q = my_getenv(lineargs[1],environment);
			if(!q)continue;
			if(strcmp(lineargs[2],q)!=0)continue;
			for(i=0;i<nargs-3;i++)lineargs[i]=lineargs[i+3];
			nargs -=3;
			p = lineargs[0];
		}

/* Goto is the last special case */

		if(strcmp(p,"goto")==0){
			if(input == stdin)continue;
			if(nargs < 2) continue;
			sprintf(linebuf4,":%s",lineargs[1]);
			if(source){
				q = strstr(source_orig,linebuf4);
				if(!q)continue;
				source=q;
				source_orig = q;
				continue;
			}	
			if(input){
				position = ftell(input);
				rewind(input);
				found = 0;
				while(fgets(linebuf1,BUF_SIZ,input)){
					q = strstr(linebuf1,linebuf4);
					if(!q)continue;
					if(q==linebuf1){found=1;break;}
				}
				if(found)continue;
				fseek(input,position,0);
				continue;
			}
		}

		cmd = internals[*p - 'a'];
	
		if(cmd)i = cmd(nargs,lineargs,environment);
/* quit is a special case */
		if(cmd == Quit)return i;
		if(!cmd ||( i == -1)) {

/* Try to execute an external command by this name */
			found = 0;
			for(j=0;j<n_externals;j++)
			  if(strcmp(lineargs[0],external_names[j])==0){
				i = external_entry_points[j](nargs,lineargs,environment);
				found = 1;
			        break;
			  }
			  if(!found){
				/* try to execute a script by this name */
				source = my_getenv(lineargs[0],environment);
				if(source){
/* set positional parameters $0, ... $9 to args so they can be
accessed from within the script */
					sprintf(linebuf1,"set");
					lineargs[10]=linebuf1;
					for(i=0;(i<9)&&(i<nargs);i++){
						sprintf(expandbuf,"%d",i);
						lineargs[11]=expandbuf;
						lineargs[12]=lineargs[i];
						Set(3,lineargs+10,environment);
					}
					for(i=nargs;i<9;i++){
						sprintf(expandbuf,"%d",i);
						lineargs[11]=expandbuf;
						Set(2,lineargs+10,environment);
					}
/* Katie bar the door! the source pointer has now been thrown off by all those
uses of set, so we need to get the proper pointer again */
					source = my_getenv(lineargs[0],environment);
					source_orig = source;
					input = NULL;
					continue;
				}
				fprintf(stderr,"monitor: no such command %s\n",linebuf2);	
			  }
			  continue;
		}	
		if(debug)
			if(i==-1)
			  fprintf(stderr,"misspelled internal command\n");

/* react to other return values if need be */
		

	} /* end of main loop */	
	return 0;
}
