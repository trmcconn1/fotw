/* Main module of my myslice classlist ps file to webwork lst file 
converter. You can upload a classlist file from the faculty center of
myslice.syr.edu.  The uploaded file will be in .xls format and named ps.xls.
The ps file needs to be opened and saved as a csv. The fields in this
file are: Notify,Photo,ID,Name,Email Addr,Units,Permission Used, Progarm
and Plan,Level. 

Webwork adds students to a course by importing a .lst file, which must be
in a specific format - also comma separated, but using only a few of the
fields in the .csv, and in a different order. This program performs the
necessary conversion. It loops over lines in the input file, and prints
appropriate fields to stdout  */

/* Build: Edit makefile for your system and 
do make ps2lst in this directory.  For usage info type

ps2lst -h

at the shell prompt. Typical usage would be

ps2lst foo.csv > foo.lst

where foo.csv is the converted classlist file you downloaded from myslice.
The file foo.lst can be uploaded into webwork using its file manager, and
then students in it added to a course using webwork's classlist editor.

Input files can be either cr-lf delimited (dos/windows) or lf delimited (unix)
The output file is always unix format since webwork expects this.*/ 


#include <stdio.h>
#include <stdlib.h>
#include "parse.h"
#include <string.h>

#define OPSEP '-'
#define USAGE "ps2lst [-q -h -v] [datafile]"
#define HELP0 "read ps file from stdin or file and print lst file to stdout"
#define HELP1 "-q: do not include comment header line in output"
#define HELP2 "-h: print this helpful message and exit"
#define HELP3 "-v: print version number and exit"
#define VERSION "1.0"

int main(int argc, char **argv)
{
	int status = MY_NORMAL;
	int quiet = 0;
	char *p,*q;
	char buffer[256];

	FILE *istream = stdin;

	/* Process Command Line */

	if(argc > 1) {
		while(argc > 1)
		  if((*++argv)[0]==OPSEP)
			switch((*argv)[1]){
				case 'h':
					printf("\n%s\n",USAGE); 
					printf("\n%s\n",HELP0);
					printf("%s\n",HELP1);
					printf("%s\n",HELP2);
					printf("%s\n",HELP3);
					printf("\n");
					argc--;
					exit(1);
					break;
				case 'v':
					printf("%s\n",VERSION);
					exit(1);
					break;  /* reminder */
				case 'q':
					argc--;
					quiet = 1;
					break;
				default:
					fprintf(stderr,"ps2lst: Invalid flag\n");
					fprintf(stderr,"%s\n",USAGE);
					exit(1);
			}
		else {
	        	if(!(istream = fopen(*argv,"r"))){
				fprintf(stderr,"ps2lst: error opening %s\n",*argv);
				exit(1);
 
			}
			argc--;
		}
	}

	/* Print out header comment line, if desired */
	if(!quiet)
	printf("#ID,Last,First,Status,Comment,Section,Recitation,Email Addr,Userid,Password,Permission\n");

	/* One time only parse of header line at top of .csv */
	p = parse(istream,&status);
	if(p == NULL)parse_error("No header line found");
	if(strcmp(p,"Notify")!=0)parse_error("Invalid header line");
	while(status != MY_EOL) /* skip header line */
		p = parse(istream,&status);  

	while(status != MY_EOF){ /* loop over lines of input file */

		/* skip extraneous EOL characters (mainly for DOS
			compatibility) */
		while(status == MY_EOL)p = parse(istream,&status);
		if(status == MY_EOF)break;
	/* skip first two (unused) fields - one already skipped */
		p = parse(istream,&status);  
		p = parse(istream,&status);  
		if(p == NULL)parse_error("No ID field");
	 
	/* print out the ID field with following comma */
		printf("%s,",p);

		p = parse(istream,&status);  
		if(p == NULL)parse_error("No last name field");

        /* The csv puts quotes around last,first, but we split on the ,
	   so this will parse as 2 fields - what we want, except that the
	   leading " has to be stripped from last-name field */

	/* print out the last name field, skipping quote char */
		printf("%s,",p+1);

		p = parse(istream,&status);  
		if(p == NULL)parse_error("No first name field");
	/* print out first name field, similarly skipping final " char */
		strcpy(buffer,p);
		q = strchr(buffer,'"');
	        if(q)*q = '\0';		
		printf("%s,",buffer);
	
	/* print out status field, which is always = c, followed by 4
	   empty fields */

		printf("c,,,,");

		p = parse(istream,&status);  
		if(p == NULL)parse_error("No email address field");
	/* print out the email address field */
		printf("%s,",p);
		
	/* Now we need to extract the userid from the email address. It is
	  the part before the @ character */

		strcpy(buffer,p);
		q = strchr(buffer,'@');
		if(q == NULL)parse_error("Malformed email address");
		*q = '\0';
		printf("%s,,\n",buffer); /* Include final empty field */

	/* skip over last 4 (unused) fields in input file */
		p = parse(istream,&status);
		p = parse(istream,&status);
		p = parse(istream,&status);
		p = parse(istream,&status);
		p = parse(istream,&status); /* new line char */
	} /* loop back to next input line */

	return 0;
}
