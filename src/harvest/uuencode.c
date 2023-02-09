/* Main program for a routine that converts stdin from binary to 
 * printable format, or vice versa, and prints result to stdout. This
 * is intended to be a workalike to the standard Unix uuencode/decode
 * pair. See the manual page on any unix-like system.
 *
 * The work is done by the subroutines in pc2bin.c */

#include "crypto.h"
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#ifndef PATH_MAX
#define PATH_MAX 255
#endif


#define TO_PRINTABLE 0
#define TO_BINARY 1

#ifdef PROGRAM_NAME
#undef PROGRAM_NAME
#define PROGRAM_NAME "uuencode"
#endif

static char *myname = PROGRAM_NAME;

int main(int argc, char **argv)
{
	char input[63];    /* input text */
	char output[63];  /* output text */
	char namebuf[PATH_MAX+1] = "";
	char namebuf2[PATH_MAX+1] = "";
	int perms = 0755;
	int o_fd = -1;
	struct stat mystat;
	FILE *source;
	char *destination_name;
	int direction = TO_PRINTABLE;
	int n,m,i,k;

	/* Find out what name we are called under */
	/* Lazy man's basename: */
	myname = strrchr(argv[0],'/');
	if(!myname)myname = argv[0];
	else myname = myname+1;

	if(strcmp(myname,"uudecode")==0)
		direction = TO_BINARY;

	if(direction == TO_PRINTABLE){

	/* See whether there is file to read input from. This will be
	 * so when two arguments are provided */

		if(argc == 3){
			source = fopen(argv[1],"r");
			stat(argv[1],&mystat);
		}
		else {
			source = stdin;
			destination_name = "/dev/stdout";
			fstat(0,&mystat);
		}

		perms = (mystat.st_mode&0777);

		if(!source){
			fprintf(stderr,"%s: could not open %s\n",myname,argv[1]);
			exit(1);
		}

		if(argc == 1){
			fprintf(stderr,"Usage: uuencode [source] name \n");
			exit(1);
		}

		destination_name = argv[argc-1];


		printf("begin %o %s\n",perms,destination_name);
	        fflush(stdout);	

		while(n = fread(input,sizeof(unsigned char),45,source)){
			if(ferror(source)){
				my_error("Read error\n");
				exit(1);
			}
			m=bin2pc(input,output+1,n); /* For a full input, m will be
					       60 */
			output[0]=n+' '; /* store number of encoded bytes as first
				    char on line */
			output[m+1]='\n';
			if(write(1,output,m+2) != m+2){
				my_error("Write error on stdout\n");
				exit(1);
			}
		}
		printf("%c\n",'`'); /* Terminate body with line containing
				       a ` character */
		printf("end\n");
	}
	if(direction == TO_BINARY){

		i = 1;
		if(argc > 1){
			if(argv[1][0]=='-'){
				if(strcmp(argv[1],"-o")==0){
					if(argc <= 2){
						fprintf(stderr,"Usage: uudecode [-o output] [file]...\n");
						exit(1);
					}
					strcpy(namebuf2,argv[2]);
				}
				else {
					fprintf(stderr,"Usage: uudecode [-o output] [file]...\n");
					exit(1);
				}
				i = 3;
			}
		}
		do {  /* loop over source files */
			if( i >= argc) source = stdin;
			else {
				source = fopen(argv[i],"r");
				if(source == NULL){
					fprintf(stderr,"Could not open %s\n", argv[i]);
					i++;
					continue;
				}
			}
			i++;
			fscanf(source,"begin %o %s",&perms,namebuf); 
			n = strlen(namebuf);
			if(!n){
				fprintf(stderr,"Bad file format\n");
				exit(1);
			}
			if(strlen(namebuf2)>0){ /* -o override */
				o_fd = open(namebuf2,O_WRONLY | O_CREAT, perms);
				strcpy(namebuf2,""); /* only works once */
			} 
			else {
				if(strcmp(namebuf,"/dev/stdout")==0)
				o_fd = 1;
				else o_fd = open(namebuf,O_WRONLY | O_CREAT, perms);
			}
			if(o_fd == -1){
				my_error("Could not create output file\n");
				exit(1);
			}
			fgets(input,63,source); /* Throw away newline at 
						   end of begin
						  line */
			while(fgets(input,63,source)){
				m = strlen(input)-2; 
				n = input[0]-' ';
				if(n==0 || n == 64)break;
				k =  pc2bin(input+1,output,m);
				if(k<n) {
					my_error("Corrupt line.\n");
					exit(1);
				}
				if(write(o_fd,output,n) != n){
					my_error("Write error\n");
					exit(1);
				}
			}
			close(o_fd);
			fclose(source);
		} while ( i < argc );
	}
 	exit(0);
}

void my_error(char *msg)
{
	fprintf(stderr,"%s: %s",myname,msg);
}
