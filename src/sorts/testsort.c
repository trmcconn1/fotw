#include "global.h"
#include <string.h>
#include <time.h>

#define USAGE "testsort [-hvplf -s <n> -m <n> -c <n> -M <n> -r <n> -n <n>] [name]"
#define HELP "\ntestsort [-hvplf -s <n> -m <n> -c <n> -M <n> -r <n> -n <n>] [name]\n\n\
Test bed for sort routines: sort test data and report on performance.\n\
Data set (if not input) is of size n and from the range [lo,hi] in steps of r.\n\n\
-h:	print this helpful message.\n\
-v:     print version number and exit.\n\
-l:     list the sorted data. (Default is not to list.)\n\
-s:     use n as seed for random number generator.\n\
-p:     print available sort method numbers (use with -c option.)\n\
-c:     use sort method n.\n\
-m:	set lo = n (default=0.)\n\
-M:	set hi = n (default=99.)\n\
-r:     set r = n  (default=1.)\n\
-f:     read data from file (name if present, else stdin.)  Data is \n\
	assumed to be whitespace delimited integers. If this option is not\n\
	specified then random data is generated in accordance with the \n\
	options.\n\
-n:     Generate data set of size n (default = 50.)\n\n"	

#define LO 0  /* low range of integer data to use by default */
#define HI 99  /* hi of range of integer data to use by default */
#define RES 1  /* default resolution of integer data */
#define NN 50  /* default size of a generated data set */
#define SEED 32222L
#define INITIAL_BUF_SIZE 256 /* Initial capacity of buffer when reading data
				from a file. It is grown as needed. */

#define SORTS 8 /* We don't seem to use this */
/* When you add a new one, edit the -p option section, add a section for
 * the new sort, and add a declaration of the new sort routine in global.h */

#define QUICKSORT 1  /* The default */
#define INSERTSORT 2
#define MERGESORT 3
#define BUBBLESORT 4
#define HEAPSORT 5
#define SHELLSORT 6
#define SELECTSORT 7
#define COUNTINGSORT 8

int hi = HI,lo = LO,res = RES;

int
main(int argc, char **argv)
{
	int i,*d,*p,j=0;
	int buffer_cap;
	clock_t elapsed;
	/* Depending on platform, we may not be able to achieve this
	 * time resolution. (Probably only nearest hundredth) */
	int secs,tenths,hundredths,thousandths,tenthousandths;
	int do_list = FALSE;
	int read_data = FALSE;
	char filename[MAX_PATH+1];
	FILE *data = stdin;
	int sort = QUICKSORT;
	int n = NN;
	long seed = 0L;
	int lo = LO, hi = HI, res = RES;


	/* Process command line */
	while(++j < argc){
		if(argv[j][0] == '-')
			switch(argv[j][1]){ 
				case 'c':
					sort = atoi(argv[j+1]);
					j++;
					continue;
				case 'l':
					do_list = TRUE;
					continue;
				case 'f':
					read_data = TRUE;
					continue;
				case 's':
				case 'S':
					if(j+1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					seed = atol(argv[j+1]);
					j++;
					continue;
				case 'n':
				case 'N':
					if(j+1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					n = atoi(argv[j+1]);
					j++;
					continue;	
				case 'm':
					if(j+1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					lo = atoi(argv[j+1]);
					j++;
					continue;
				case 'M':
					if(j+1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					hi = atoi(argv[j+1]);
					j++;
					continue;

				case 'r':
					if(j+1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					res = atoi(argv[j+1]);
					j++;
					continue;
				case 'p':
					printf("\n\nTestsort: Available sort method. Use -c <number shown>\n");
					printf("Quick sort: %d\n",QUICKSORT);
					printf("Insertion sort: %d\n",INSERTSORT);
					printf("Merge sort: %d\n",MERGESORT);
					printf("Bubble sort: %d\n",BUBBLESORT);
					printf("Heap sort: %d\n",HEAPSORT);
					printf("Shell sort: %d\n",SHELLSORT);
					printf("Selection sort: %d\n",SELECTSORT);
					printf("Counting sort: %d\n",COUNTINGSORT);
					printf("\n\n");
					exit(0);
				case 'v':
				case 'V':
					printf("%s\n",VERSION);
					exit(0);
				case '?':
				case 'h':
				case 'H':
					printf("%s\n",HELP);
					exit(0);
				default:
					fprintf(stderr,"testsort: unkown option %s\n",
						argv[j]);
					exit(1);
			}
		else break;
	}

	if(read_data == TRUE){  /* Read data from file or stdin */
		if(j<argc){
			strcpy(filename,argv[j]);
			if(!(data = fopen(filename,"r"))){
				fprintf(stderr,"testsort: cannot open %s\n",filename);
				exit(1);
			}
		}
		/* prepare to read in integer data */
		buffer_cap = INITIAL_BUF_SIZE;
		d = (int *)malloc(INITIAL_BUF_SIZE*sizeof(int));
		if(!d){
			fprintf(stderr,"testsort: cannot allocate memory for data.\n");
			exit(1);
		}
		n = 0;
		p = d;
		while(fscanf(data,"%d",p++)!=EOF){
			n++;
			if(n==buffer_cap){ /* need to grow buffer */
				buffer_cap *= 2;
				d = realloc(d,buffer_cap);
				if(!d){
					fprintf(stderr,"testsort: cannot reallocate data memory.\n");
					exit(1);
				}
				p = d+n;
			}
		}
		if(data != stdin)fclose(data);
	printf("Problem size n = %d records of %d bytes.(User supplied data in nominal range [%d... %d])\n",
			n,(int)sizeof(int),lo,hi);
	}

	else { /* Generate random data */
 
	/* If no seed is supplied, then use current system time */
	
		if(!seed)
			if((seed = (long)time(NULL)) == -1){
				seed = SEED; /* if all else fails */
				fprintf(stderr, "Using seed = %ld\n",SEED);
			}
		
		d = random_int_data(n,lo,hi,res,seed);

	printf("Problem size n = %d records of %d bytes.(Random data %d to %d by %d)\n",
			n,(int)sizeof(int),lo,hi,res);
	}


	switch(sort){  /* choose sorting method */

/* Breaking these out separtately allows us to do customized profiling for
 * each sorting method, though it should be kept as consistent as possible
 * to allow for comparisons */

/*************** Quicksort ********************************/
	case QUICKSORT:
	printf("Quicksort algorithm\n");
	elapsed = clock();
	quicksort((void *)d, n, sizeof(int), (int (*)(const void *, const void *))cmp_int);
	elapsed = clock() - elapsed;
	printf("swaps = %d, compares = %d, max depth = %d, calls = %d\n\n",swaps,compares,depth,calls);
	break;
/*************** Selection Sort ********************************/
	case SELECTSORT:
	printf("Selection Sort algorithm\n");
	elapsed = clock();
	selectsort((void *)d, n, sizeof(int), (int (*)(const void *, const void *))cmp_int);
	elapsed = clock() - elapsed;
	printf("swaps = %d, compares = %d, overhead = %d\n\n",swaps,compares,overhead);
	break;
/***************** Insert sort **************************/
	case INSERTSORT:
	printf("Insert sort algorithm\n");
	elapsed = clock();
	insertsort((void *)d, n, sizeof(int), (int (*)(const void *,const void *))cmp_int);
	elapsed = clock()-elapsed;
	printf("swaps = %d, compares = %d, overhead = %d\n\n",swaps,compares,overhead);
	break;
/***************** Merge sort **************************/
	case MERGESORT:
	printf("Merge sort algorithm\n");
	elapsed = clock();
	_mergesort((void *)d, n, sizeof(int), (int (*)(const void *,const void *))cmp_int);
	elapsed = clock()-elapsed;
	printf("calls = %d, compares = %d, depth = %d, overhead = %d\n\n",calls,compares,depth,overhead);
	break;
/***************** Bubble sort **************************/
	case BUBBLESORT:
	printf("Bubble sort algorithm\n");
	elapsed = clock();
	bubblesort((void *)d, n, sizeof(int), (int (*)(const void *,const void *))cmp_int);
	elapsed = clock()-elapsed;
	printf("swaps = %d, compares = %d, overhead = %d\n\n",swaps,compares,overhead);
	break;
/************************************************************/
/***************** Heap sort **************************/
	case HEAPSORT:
	printf("Heap sort algorithm\n");
	elapsed = clock();
	_heapsort((void *)d, n, sizeof(int), (int (*)(const void *,const void *))cmp_int);
	elapsed = clock()-elapsed;
	printf("swaps = %d, compares = %d, overhead = %d\n\n",swaps,compares,overhead);
	break;
/************************************************************/
/***************** shell sort **************************/
	case SHELLSORT:
	printf("Shell sort algorithm\n");
	elapsed = clock();
	shellsort((void *)d, n, sizeof(int), (int (*)(const void *,const void *))cmp_int);
	elapsed = clock()-elapsed;
	printf("swaps = %d, compares = %d, overhead = %d\n\n",swaps,compares,overhead);
	break;
/************************************************************/
/***************** counting sort **************************/
	case COUNTINGSORT:
	printf("Counting sort algorithm\n");
	elapsed = clock();
	countingsort((void *)d, n, sizeof(int), (int (*)(const void *,const void *))cmp_int);
	elapsed = clock()-elapsed;
	printf("swaps = %d, overhead = %d\n\n",swaps,overhead);
	break;
/************************************************************/
	default:
		fprintf(stderr,"testsort: unknown sort method %d\n",sort);
		exit(1);
	} /* End switch on sort method */

	secs = (int)(elapsed/CLOCKS_PER_SEC);
	elapsed = elapsed - ((clock_t)secs)*CLOCKS_PER_SEC;
	elapsed = 10*elapsed;
	tenths = (int)(elapsed/CLOCKS_PER_SEC);
	elapsed = elapsed - ((clock_t)tenths)*CLOCKS_PER_SEC;
	elapsed = 10*elapsed;
	hundredths = (int)(elapsed/CLOCKS_PER_SEC);
	elapsed = elapsed - ((clock_t)hundredths)*CLOCKS_PER_SEC;
	elapsed = 10*elapsed;
	thousandths = (int)(elapsed/CLOCKS_PER_SEC);
	elapsed = 10*elapsed;
	elapsed = elapsed - ((clock_t)thousandths)*CLOCKS_PER_SEC;
	elapsed = 10*elapsed;
	tenthousandths = (int)(elapsed/CLOCKS_PER_SEC);
	printf("Elapsed time: %d.%d%d%d%d seconds.\n\n",secs,tenths,hundredths,
			thousandths,tenthousandths);
	if(do_list){ /* quick and dirty */
		printf("--------------------\n");
		printf("Sorted data:\n");
		for(i=0;i<n;i++){
			if((i%10)==0)printf("\n");
			printf("%d\t",*(d+i));
		}
		printf("\n-------------------------\n\n");
	}

	return 0;

}
