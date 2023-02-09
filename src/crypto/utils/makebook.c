/* makebook.c: create a text file suitable to be used as the "book" with
   the book cypher.

   Author: Terry R. McConnell

   Prints random characters to stdout, 64 to a line. The desired frequency 
   distribution is given by a static table (see below.) If every character
   is not represented in the output, the program will print an error 
   message. If this happens, you can retry with a different seed or increase
   the output file size. (If any character is assigned a frequency that
   makes it very rare, there is a non-zero probability that it will never
   get generated. Increasing file size makes this undesired outcome less
   likely.) 

   Program also reports the Chi-square statistic for the Expected vs Actual
   frequencies in the text file created. 
 
*/

#include <stdio.h>
#include <stdlib.h>


#define VERSION "1.0"
#define USAGE "makebook [ -s number -f number  -h -v] "
#ifndef _SHORT_STRINGS
#define HELP "makebook [ -s number -f number -h -v ]\n\n\
Create a file of random text suitable for use with book cypher.\n\n\
-s: Use next argument as RNG seed. (otherwise use system time as seed.)\n\
-f: Use next argument as output file size (KB). Default = 20KB.\n\
-v: Print version number and exit. \n\
-h: Print this helpful information. \n\n"
#else
#define HELP USAGE
#endif


#ifdef _NO_RANDOM
#define RANDOM rand
#define SRANDOM srand
#else
#ifndef RANDOM
#define RANDOM random
#endif
#ifndef SRANDOM
#define SRANDOM srandom
#endif
#endif


/* The following static array lists the assumed frequencies for each
   of the printable chars. I used a published table of frequencies
   for lower case letters of the alphabet. Capital letter frequencies
   are about 1/10th as large, for no particularly good reason. 
   The rest I just made up. Adjust if you feel like it. */

static int freq[95] =
{               /* Character number  Character */
100,		/* 32  SPACE */
10,		/* 33  !  */
10,		/* 34  "  */
1,		/* 35  #  */
5,		/* 36  $  */
1,		/* 37  %  */
1,		/* 38  &  */
1,		/* 39  '  */
5,		/* 40  (  */
5,		/* 41  )  */
1,		/* 42  *  */
1,		/* 43  +  */
1,		/* 44  ,  */
5,		/* 45  -  */
10,		/* 46  .  */
10,		/* 47  /  */
10,		/* 48  0  */
10,		/* 49  1  */
10,		/* 50  2  */
10,		/* 51  3  */
10,		/* 52  4  */
10,		/* 53  5  */
10,		/* 54  6  */
10,		/* 55  7  */
10,		/* 56  8  */
10,		/* 57  9  */
3,		/* 58  :  */
5,		/* 59  ;  */
1,		/* 60  <  */
1,		/* 61  =  */
1,		/* 62  >  */
10,		/* 63  ?  */
1,		/* 64  @ */
8,		/* 65  A */
2,		/* 66  B */
3,		/* 67  C */
4,		/* 68  D */
12,		/* 69  E */
2,		/* 70  F */
2,		/* 71  G */
6,		/* 72  H */
7,		/* 73  I */
1,		/* 74  J */
1,		/* 75  K */
4,		/* 76  L */
2,		/* 77  M */
7,		/* 78  N */
8,		/* 79  O */
2,		/* 80  P */
1,		/* 81  Q */
6,		/* 82  R */
6,		/* 83  S */
9,		/* 84  T */
3,		/* 85  U */
1,		/* 86  V */
2,		/* 87  W */
1,		/* 88  X */
2,		/* 89  Y */
1,		/* 90  Z */
1,		/* 91  [ */
1,		/* 92  \ */
1,		/* 93  ] */
1,		/* 94  ^ */
3,		/* 95  _ */
1,		/* 96  ` */
82,		/* 97  a */
15,		/* 98  b */
28,		/* 99  c */
43,		/* 100  d */
127,		/* 101  e */
22,		/* 102  f */
20,		/* 103  g */
61,		/* 104  h */
70,		/* 105  i */
2,		/* 106  j */
8,		/* 107  k */
40,		/* 108  l */
24,		/* 109  m */
67,		/* 110  n */
75,		/* 111  o */
19,		/* 112  p */
1,		/* 113  q */
60,		/* 114  r */
63,		/* 115  s */
91,		/* 116  t */
28,		/* 117  u */
10,		/* 118  v */
24,		/* 119  w */
2,		/* 120  x */
20,		/* 121  y */
1,		/* 122  z */
1,		/* 123  { */
1,		/* 124  | */
1,		/* 125  } */
1		/* 126  ~ */
};

#define MAX_FILE_SIZE 20   /* KB */

#define INITIAL_SEED 3445  /* Had to be something */
#ifndef _MAX_RAND
#define _MAX_RAND RAND_MAX
#endif


/* return a number chosen at random from 0,1...,k-1 */

int random_on(int k){

        double U;   /* U(0,1) random variable */
        int rval;

        U = ((double)RANDOM())/((double)_MAX_RAND);
        rval = (int)((double)k*U);
        return rval;
}


int main(int argc, char **argv)
{

	int i,j=0,k,l,n;
	long seed = 0;
	int file_kb = MAX_FILE_SIZE;
	int freq_total = 0;
	double chi_square,O,E;
	char buffer[1024];  /* Keep size = 1 KB */
	int observed_freq[95];

	/* Process command line */
	while(++j < argc){
		if(argv[j][0] == '-')
			switch(argv[j][1]){ 
				case 's':
				case 'S':
					if(j+1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					seed = atol(argv[j+1]);
					j++;
					continue;
				case 'f':
				case 'F':
					if(j+1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					file_kb = atoi(argv[j+1]);
					j++;
					continue;	
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
					fprintf(stderr,"makebook: unkown option %s\n",
						argv[j]);
					exit(1);
			}
	}

	if(j > argc){
		fprintf(stderr,"%s\n",USAGE);
		exit(1);
	}
 
	/* If no seed is supplied, then use current system time */
	
	if(!seed)
		if((seed = (long)time(NULL)) == -1){
			seed = INITIAL_SEED; /* if all else fails */
			fprintf(stderr, "Using seed = %d\n",INITIAL_SEED);
		}
		
	SRANDOM((unsigned)seed);

	/* Sum the frequencies */

	for(j=0;j<95;j++)freq_total += freq[j];

	/* initialize observed frequencies */

	for(j=0;j<95;j++)observed_freq[j] = 0;
	

	i = 0;
	while(i++ < file_kb){

		/* Fill buffer with random characters */

		for(n=0;n<1024;n++){
			j = random_on(freq_total);
			k=0;
			l=0;
			while(k<j+1) k += freq[l++];
			buffer[n] = 31 + l; /* The char number */ 
			(observed_freq[l-1])++;
		}

		/* write buffer out */
		for(n=0;n<1024/64;n++){
			for(k=0;k<64;k++)
				putchar(buffer[k+64*n]);
			putchar('\n');
		}
	}

	/* Is every char represented ? If not, inform user. */
	
	k = 0;
	l = 0;
	for(k=0;k<95;k++)
		if(!observed_freq[k])l=1;

	if(l)fprintf(stderr,"Error: unrepresented characters in book\n");
	else fprintf(stderr,"Book successfully created.\n");

	/* Report distance between actual and desired frequencies */
	
	chi_square = 0.0;
	for(j=0;j<95;j++){
		E = (double)freq[j]*((double)file_kb*1024.0)/((double)freq_total);
		O = (double)observed_freq[j];
		chi_square += (O-E)*(O-E)/E;
	} 
	fprintf(stderr,"Chi-square (df=94) : %g\n",chi_square);	

	/* Print some helpful advice */
	fprintf(stderr,"\n\nSet book file permissions to 400.\n");
	fprintf(stderr,"Keep in a directory not readable or searchable by others.\n");

	return 0;
}
