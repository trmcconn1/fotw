/* zdata.c: Make a normal data set with prescribed mean and standard deviation
*/

	By Terry R. McConnell

  Compile: cc -o zdata   zdata.c 

*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define VERSION "1.0"
#define PROGNAME "zdata"
#define USAGE "zdata [-cvh ] n mu sigma"
#define HELP "\n-h: print this helpful message\n\
-v: print version number and exit\n\
-c: take next letter following the c as the field separator\n\
Print to stdout n simulated independent observations of a normal \
with mean mu and standard deviation sigma\n\n"


#define TRUE 1

int
main(int argc, char **argv){



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
			default:
				fprintf(stderr,"%s:%s\n",PROGNAME,USAGE);
				return 1;
		}
		i++;
	}


	return 0;	
}

