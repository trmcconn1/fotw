
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mtwist.h"
#include "randistrs.h"

#define USAGE "dally [-hv] m"
#define VERSION "1.0"

int
main(int argc, char *argv[]) 
{
	char  c;
	double mean, time;

	while (--argc > 0 && (*++argv)[0] == '-')
		while ( c = *++argv[0])
		switch (c) {
		case 'h':
			printf("%s\n",USAGE);
			printf("Wait random time with mean m and return\n");
			return 0;
		case 'v':
			printf("%s\n",VERSION);
			return 0;
		default:
			printf("dally: illegal option %c\n", c);
			return 1;
		}
	if(argc != 1){
		printf("Usage: %s\n",USAGE);
		return 1;
	}
	mt_seed();
	mean = atof(*argv);
	time = rd_exponential(mean);
	printf("Waiting %g seconds\n",time);
	return 0;
}
