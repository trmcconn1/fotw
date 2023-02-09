/* Matches: code to read a text file on stdin and analyze it for
matches. A match is a substring that occurs more than once. For example
abccbabab contains matches of substrings of length 1,2, and 3. The
match of ab at the end is an example of a square. The match of bab at
the end is an overlap. The amount of overlap - what we shall call the
intimacy of the match - is 1 in this case. Squares have intimacy 0.
The first match of ab, ab(ccb)ab is a match of intimacy -3. 

In normal operation the program will print statistical information
on all matches of a given length. For example, matches 5 < foo will 
print information on matches of substrings of length 5 in file foo. One can 
limit attention to matches of a given intimacy or higher by using
the -i option. 

This is not a toy: DO NOT PLAY WITH MATCHES!

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MYBUFSIZ 1024
#define VERSION "1.0"
#define USAGE "\nmatches [-hvVdnx] [-i k] l: analyze stdin for matching substrings of length l\n"
#define USAGE1 "-h: print this helpful information and exit\n"
#define USAGE2 "-v: print version number and exit\n"
#define USAGE8 "-V: more verbose output\n"
#define USAGE3 "-d: turn on debug mode\n"
#define USAGE4 "-n: ignore newlines in input\n"
#define USAGE5 "-x: run experimental code and exit (may do nothing)\n"
#define USAGE6 "-i: look for matches of intimacy k or more only \n"
#define USAGE7 "(intimacy = amount of overlap, 0 is a square)\n"
#define HELP USAGE USAGE1 USAGE2 USAGE8 USAGE3 USAGE4 USAGE5 USAGE6 USAGE7


static char buffer[MYBUFSIZ];

/* Define USEMYKMP to use the following function, defined in kmp.c
in place of stdlib strstr */

extern char *my_strstr(const char *src, const char *target);  

int main(int argc, char **argv)
{
	int n = 0;
	char c;
	char *txt,*p,*q;
	int l = 3; /* really not a default: l must be given on command line. */
	int debug = 0;
	int verbose = 0;
	int ignore_nl = 0;
	int intimacy_limit = 0;
	int i = 0;
	int mn = 0,in = 0, ii = 0; /*  counts, running total for i  */

	/* Process command line options. argc will always count number of args
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
		  if(strcmp(*argv,"-d")==0){
			  debug = 1;
			  verbose = 1;
			  ++argv;
			  argc--;
			  continue;
		  }  
		  if(strcmp(*argv,"-n")==0){
			  ignore_nl = 1;
			  ++argv;
			  argc--;
			  continue;
		  }
		  if(strcmp(*argv,"-V")==0){
			  verbose = 1;
			  ++argv;
			  argc--;
			  continue;
		  }
		  if(strcmp(*argv,"-i")==0){
			  argc--; ++argv;
			  if(argc <= 0){
				  fprintf(stderr,"matches: usage:%s\n",USAGE);
				  return 1;
			  }
			  i = atoi(*argv);
			  intimacy_limit = 1;
			  argv++;
			  argc--;
			  continue;
		  }
		  if(strcmp(*argv,"-x")==0){/* Put any experimental stuff here */
			  printf("This is a test, this is ONLY a test!\n");
			  
			  return 0;
		  }
		  fprintf(stderr, "matches: Unknown option %s\n", *argv);
		  fprintf(stderr, "%s\n",USAGE);
		  return 1;
	   }
	

	/* There should still be something (i.e. l) left on the command line */

	if(argc == 0){
		fprintf(stderr,"matches: usage %s\n", USAGE);
		return 1;
	}

	/* Assume what's left on the command line is a length */
	
	l = atoi(*argv);

	if(l <= 0 ){ /* sanity check */
		fprintf(stderr,"matches: length %d is impossible\n",l);
		return 1;
	}

	if(debug)printf("matches: debug mode is on\n");

	txt = malloc(MYBUFSIZ); /* will be dynamically resized if needed */
	if(!txt){
		fprintf(stderr,"matches: unable to allocate memory for input\n");
		return 1;
	}

	/* Read in text string to analyze */
	if(debug)printf("matches: about to read data\n");

	while((c = fgetc(stdin))!=EOF){
		if(ignore_nl && (c == '\n'))continue;
		if(((n+1)%MYBUFSIZ)==0){ /* Need more memory  (What did I just say?) */ 
			if(debug)printf("Increasing memory size\n");
			p = realloc(txt,n+MYBUFSIZ);
			if(!p){
				fprintf(stderr,"matches: failed to allocate more memory, n = %d\n",n);
				free(txt); 
				return 1;
			}
			txt = p;
		}
		txt[n++] = c;
	}

	txt[n]='\0'; /* Make txt a null terminated string */

	
	if(debug)printf("matches: Read %d bytes\n",n);

		p = txt;

	
		while((p-txt)+l<n){

			strncpy(buffer,p,l);
			*(buffer+l)='\0';

			/* Find the next match of length l, if there are any */

#ifdef USEMYKMP
			q = my_strstr(p+1,buffer);
#else
			q = strstr(p+1,buffer);
#endif
			if(q){  /* found one */
				mn++;
				if(intimacy_limit && ((l - (q-p)) < i)){p++; continue; /* skip it */ }
				/* l - (q-p) measures overlap */
				in++;  /* count it */
				ii += (l - (q-p));  /* running total for average */
				if(verbose)printf("match of %s intimacy %ld, starts at %ld\n",buffer,l-(q-p),(p-txt));
			}
			p++;
		}

	/* Report values */
	printf("Read %d characters\n",n);
	printf("Found %d matches of length %d\n",mn,l);
	if(mn){
		if(intimacy_limit)printf("Found %d matches of intimacy i > %d\n",in,i-1);
		if(in)printf("i bar = %f, (avg intimacy)\n", ((double)ii/(double)in));
	}

	/* clean up */
	free(txt);
	return 0;

}
