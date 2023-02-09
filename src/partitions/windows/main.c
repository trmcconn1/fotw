/* main.c: main module of partition programs
 * 
 *
 * 	By Terry R. McConnell
 */

#include "partition.h"

#define VERSION "1.0"
#define USAGE "partition [ -d -p -l -t -h -v -f <k> -a <m>] n"
#ifndef _SHORT_STRINGS
#define HELP "\n\npartition [-f <k> -l -p -V  -t -h -v ] n\n\n\
Print the number of partitions of n\n\n\
-d: Dump list of available filter names and indices and exit\n\
-f: Filter using next argument as index of available filter\n\
-a: Pass m as argument to the filter (if it takes one)\n\
-t: Print a table of the partition function\n\
-l: List partitions rather than just count them\n\
-p: Apply operation to pentagonal numbers rather than partitions\n\
-x: Run experimental code and exit\n\
-v: More verbose operation\n\
-h: Print this helpful information \n\n"
#else
#define HELP USAGE
#endif

unsigned long C[N_MAX];

int verbose = 0;
int debug = 0;

int
main(int argc, char **argv)
{
	int j=0, n = N_MAX,k;
	int index;
	unsigned char i;
	unsigned long m;
	int do_table = 0;
	int do_pentagonal = 0;
	int do_list = 0;
	unsigned long count = 0L;
	int (*my_filter)(unsigned char (*)[],int,int) = NULL;
	char *filter_name;
	char *filter_info;

	/* Process command line */

	*argv++;
	argc--;
	while(argc){
		if((*argv)[0] == '-'){
			switch((*argv)[1]){ 
				case '?':
				case 'h':
				case 'H':
					printf("\nPartition Version %s\n",VERSION);
					printf("%s\n",HELP);
					exit(0);
				case 'c':
					m = ULONG_MAX;
					k = 0;
					while(m){
						m = m>>1;	
						k++;
					} /* k = log(ULONG_MAX) */
					printf("ULONG_MAX = %lu = 2^%dn",ULONG_MAX,k);
					n = k/2;
					k=0L;
					m = (unsigned long)n;
					while(m){
						m = m>>1;
						k++;
					}
					printf("cc -o partition -DN_MAX=%d partition.c\n",n+k/2);
					exit(0);
				case 'f':
					*argv++;
					argc--;
					index = atoi(*argv);
					if(index > n_filters){
						printf("filter index too big\n");
						exit(1);
					}
					my_filter = my_filters[index].point;
					filter_name = my_filters[index].name;
					filter_info = my_filters[index].info;
					break;	
				case 'a':
					*argv++;
					argc--;
					filter_arg = atoi(*argv);
					break;
				case 't':
					do_table = 1;
					break;
				case 'd':
					dump_filter_names();
					exit(0);
				case 'l':
					do_list = 1;
					break;
				case 'V':
				case 'v':
					verbose = 1;
					break;
				case 'p':
					do_pentagonal = 1;
					break;
				case 'x':
					initializeE();
					j = 0;
					while(j<20){
						i=j;
						if((m=E2Num())>15L){
							j = jump(i);
							continue;
						}
						if(m == 15L){ 
							listE();
							j = jump(i);
							continue;
						}
						j = next();
					}
					exit(0);
				default:
					fprintf(stderr,"partition: unkown option %s\n",
						*argv);
					exit(1);
			}
			*argv++;
			argc--;
		}
		else break;
	}

	if(argc)
		n = atoi(*argv); /* Undocumented "feature": n = N_MAX by
                                    default */

	if(n > N_MAX){
		printf("The value n is bigger than N_MAX = %d\n",N_MAX);
		printf("Result will probably be incorrect\n");
		printf("Change N_MAX setting in Makefile\n");
	}
	if(do_table){ /* initialize array of counts */

		for(j=0;j<N_MAX;j++)C[j]=0L;
	}

/* If a filter has been set we do things the slow way, by enumerating
all partitions. Otherwise we just use the recursion formula (below) */

	if(my_filter){
		initializeE(); /* zero out partition array in next.c */
		j = 0;
		while(j<=n){
			i=j;
			if((m=E2Num())>(unsigned long)n){
				j = jump(i);
				continue;
			}
			if(do_table){
				if(filter(my_filter,m))
					C[m]++;
			}
			if(m == (unsigned long)n){ 
				
				if(filter(my_filter,n)){
					count++;
					if(do_list)
					 	listE();
				}
				j = jump(i);
				continue;
			}
			j = next();
		}
		if(!do_list && !do_table){
			if(verbose)printf("%lu %s partitions of %d\n",
				count,filter_info,n);
			else printf("%lu\n",count);
			exit(0);
		}
	}
	else {
		C[0] = 1L; /* recursion base needs this */
		k = 0;
		if(do_pentagonal)
			while(k++ < n)
				C[k] = (unsigned long)PENTAGONAL(k);
		else
			while(k++ < n)
				C[n] = partitions(n);
	}
	if(do_table){
		if(do_pentagonal)
		 printf("\n\tTable of the pentagonal numbers\n\n");
		else {
		 printf("\n\tTable of the partition function\n");
		 if(my_filter)
			printf("\t(%s)\n",filter_info);
		 printf("\n");
		}
		printf("n\tP(n)\n");
		printf("-----------------------\n");
		for(k=1;k<=n;k++)
			printf("%d\t%lu\n",k,C[k]);
	}
	else printf("%lu\n",C[n]);
	return 0;
}
	 
