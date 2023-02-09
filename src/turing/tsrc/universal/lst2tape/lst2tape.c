/* lst2tape: utility program to convert turing -l output to a string
 * suitable to be used for a .tape directive 

	By Terry R. McConnell (trmcconn@syr.edu)
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdarg.h>

#define PROGNAME "lst2tape"
#define USAGE "lst2tape [ -v -h ] < source > dest"
#define VERSION "1.0"
#define MAXLINE 256

#define HELP "-h: print this helpful message\n\
-v: print version number and exit\n\n\
Converts turing -l output to a string to be used in a .tape directive for \n\
the universal turing machine.\n\n"

int unzero(int);  /* squeeze the digit 0 out of a number */

/* Banner: print name of program etc */

void banner()
{			
	printf("\n\t\t\t%s( %s )\n\n",PROGNAME,VERSION);
}


int error( char *format, ...) {

	va_list ap;

	va_start(ap, format);
       	vfprintf(stderr,format,ap);
	fprintf(stderr,"\n");
	va_end(ap);
	exit(1);
}

static char linebuf[MAXLINE+1];

/* The main program */

int main(int argc, char **argv)
{

	int i=1,line=1,n;
	int verbosity = 0;
	int oldstate,newstate;
	char c,w,*p;

	/* Process command line */

	while((i<argc) && (argv[i][0]=='-')){
		switch(argv[i][1]){
		case 'v':
			printf("%s\n",VERSION);
			return 0;
		case 'h':
			printf("\n%s\n\n",USAGE);
			printf("%s\n",HELP);
			return 0;
		default:
			fprintf(stderr,"%s:%s\n",PROGNAME,USAGE);
			return 1;
		}
		i++;
	}

	if(verbosity)banner();

	/* Loop reading lines from stdin. Each line has the format
	 * -k;m;x;-n where k,m,n are natural numbers and x is either a
	 * natural number or the symbols L or R. Negative numbers are state
	 * numbers and positive numbers represent indices into the 
	 * alphabet array. We only support alphabets of size 2 here, so
	 * l (and possibly x) need to be 0 or 1.  
	 *
	 * All we have to do is translate the above line into
	 * Bkyzn;
	 * where y is B or | according as m is 0 or 1, and z is obtained from
	 * x by the following translation: R -> r, L -> l, 0 -> B, and 1 ->
	 * |. We then concatenate all these strings and terminate with
	 * a final * character (the cseg/dseg separator.)
	 *
	 * */


	while(fgets(linebuf,MAXLINE,stdin)!=NULL){

		p = strtok(linebuf,";");
		if(!p)error("Malformed line. Number %d = %s\n",line,linebuf);
		oldstate = unzero(-atoi(p));
		p = strtok(NULL,";");
		if(!p)error("Malformed line. Number %d = %s\n",line,linebuf);
		n = atoi(p);
		if(n < 0 || n > 1) error("Alphabet index out of range line %d\n",line);
		if(n==0)c='B';
		else c = '|';
		p = strtok(NULL,";");
		if(!p)error("Malformed line. Number %d = %s\n",line,linebuf);
		switch(p[0]){
			case 'R':
				w = 'r';
				break;
			case 'L':
				w = 'l';
				break;
			case '0':
				w = 'B';
				break;
			case '1':
				w = '|';
				break;
			default:
				error("Unknown action %c, line %d.\n",
						p[0],line);
				break;
		}
		p = strtok(NULL,";\n");
		if(!p)error("Malformed line. Number %d = %s\n",line,linebuf);
		newstate = unzero(-atoi(p));
		printf("B%d%c%c%d;",oldstate,c,w,newstate);
		line++;
	}
	printf("B*");
	return 0;

}

/* This routine implements a 1-1 transformation on the natural numbers
 * such that the range contains no numbers having the digit 0. This is
 * needed because the universal machine does not allow 0 in a state number
 * since 0 stands for blank tape. The algorithm is to convert x to octal and
 * then add 1 to each octal digit.
 *
 * It is also convenient to map 1 to 1, since 1 is the intial state. We treat
 * 1 as a special case.
 */

int unzero( int x ){

	int q = x,r=0,d;

	if(x == 1) return 1;

	while(q){

		d = q % 8;
		r = 10*r + (d+1);
		q = q/8;
	}	

	return r;
}
