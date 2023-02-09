/* mkhtmtab.c: Tool for writing the html code to create tables.

	By Terry R. McConnell

  Compile: cc -o mkhtmtab -D__UNIX__ mkhtmtab.c 

*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#ifdef __UNIX__
#include<unistd.h>
#else
#error This code requires implementation of isatty
#endif


#define VERSION "1.0"
#define PROGNAME "mkhtmtab"
#define USAGE "mkhtmtab [-cvh -H [header] ]"
#define HELP "\n-h: print this helpful message\n\
-v: print version number and exit\n\
-c: take next letter following the c as the field separator\n\
-H: take following argument as the table header. (No effect if interactive.)\n\n\
Print html output to create table from lines read on stdin.\n\
If stdin is not a tty then table header can be set with -H option.\n\n"


#define PROMPT "mkhtmtab> "
#define SEPCHAR ','
#define MAXLINE 1024
#define TRUE 1

int mktable(char *, char *);
static char data[0xFFFFFF];
static char caption[MAXLINE+1];

#ifdef _DEBUG_
char test_table[] = {'1','\0','2','\0','3','\0','\0','4','\0','5','\0',
	'6','\0','\0','7','\0','8','\0','9','\0','\0','\0'};
char *test_caption = "Table of the Nonzero Digits";
#endif


int
main(int argc, char **argv){

	int i=1,n,row=1;
	char line[MAXLINE+1];
	char sepchar = SEPCHAR;
	char separray[] = {'\0','\0'};
	FILE *file = stdin;
	char *d,*l;


	/* Process command line */
	if(argc > 1)
	while(i < argc && argv[i][0]=='-'){
		switch(argv[i][1]){
			case 'v':
				printf("%s\n",VERSION);
				return 0;
			case 'h':
				printf("\n%s\n",USAGE);
				printf("%s\n",HELP);
				return 0;
				break;
			case 'H':
				if(i+1 <= argc)
					strcpy(caption,argv[i+1]);
				i++;
				break;
			case 'c':
				if(strlen(argv[i])>1)
						sepchar=argv[i][2];
				break;
			default:
				fprintf(stderr,"%s:%s\n",PROGNAME,USAGE);
				return 1;
		}
		i++;
	}

	separray[0] = sepchar;

#ifdef _DEBUG_
	mktable(test_table, test_caption); 
#endif

if(isatty(0)){
	fprintf(stderr,"\n\n\t\t\t%s version %s\n", PROGNAME,VERSION);
	fprintf(stderr,"\t\t\tby Terry R. McConnell\n\n\n");
	fprintf(stderr,"Enter table heading (or just return if no heading)> ");
	fgets(caption,MAXLINE,file);
	caption[strlen(caption)-1]='\0';
	fprintf(stderr,"\nEnter each row on a separate line with %c between entries\n",sepchar);
	fprintf(stderr,"An empty row ends the table.\n\n");
}

	d = data; /* Save pointer to start of table data */

	while(TRUE){
		if(isatty(0))
			fprintf(stderr,"row %d> ",row++);
		fgets(line,MAXLINE,file);
		line[strlen(line)-1] = '\0'; /* nuke newline */
		if((n = strlen(line)) == 0)break;
		/* parse input line */
		l = line;  /* save start of line pointer */
		if(strtok(line,separray))	
			while(strtok(NULL,separray));
		for(i=0;i<n;i++)*d++ = *l++;
	       *d++ = '\0';	
	       *d++ = '\0';
	}
	if(isatty(0))
		fprintf(stderr,"\n");
	*d++ = '\0';
	mktable(data,caption);
	return 0;	
}

/* We assume the data consists of a 1 dimensional array of chars. Table data
 * are separated by 
 * null characters. 2 successive nulls ends a row. 3 successive nulls ends
 * the table. Thus, the 3x3 table of the nonzero digits would be passed as 
 * the following array of chars: 1020300405060070809000
 *
 * This return does no sanity testing on its arguments. If they're wrong,
 * you're toast.
 */ 

int mktable(char *data, char *caption) {

		int i;
		char *p = data;

		printf("<TABLE BORDER>\n");
		if(caption && strlen(caption))
			printf("<CAPTION>%s</CAPTION>\n",caption);
		while(*p){
			printf("<TR>");
			while(*p){
				printf("<TD>%s</TD>",p);
				p += strlen(p);
				p++;
			}
			printf("</TR>\n");
			p++;
		}
		printf("</TABLE>\n");
}
