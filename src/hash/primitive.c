/* primitive.c: Find a hash function f that will map a given set of N distinct integers onto small distinct integers.

The function, if it exists, has the form f(x) = 2^x mod p  where 2  is a primitive root of the prime p. The prime is
chosen so that p-1 does not divide any difference of the given integers.

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>

/* Table of good p from H. Stark, Elementary Number Theory, Markham, Chicago, 1970, p. 339 */

static unsigned p[] = { 3,5,11,13,19,29,37,53,59,61,67,83,101,107,131,139,149,163,173,179,181,197,211,227,269,293,317,347,
349,373,379,389,419,421,443,461,467,491};

static int K = sizeof(p)/sizeof(int);

#define NMAX 16384
#define INFO  "Create hash function to map x1... to unique small integers"
#define USAGE "primitive [-hqvx] -f | x1, x2, ..."
#define MAX_ARG_LEN 80             /* Max length of combined options */
#define VERSION "1.0"
int find_opt(char *option_string);
void myerror(char *fmt, ...);
extern int phi(int);
int is_good_modulus(int m);
unsigned mul_inv(unsigned);
char ProgramName[256];
static int c[NMAX];
static unsigned u[NMAX];
static int quiet = 0;

int main(int argc, char **argv)
{

	unsigned N,i,j,k,m,s;
	int Max,min;
	unsigned Range;
	int code=-1;                /* numeric option code */
	char *info = INFO;
	char opsep = '-';      /* Standard for Unix */
	char argstr[MAX_ARG_LEN];
	int read_file = 0;
	char line_buf[256];

/* Program name is available in ProgramName */

	strcpy(ProgramName,argv[0]);

/* Process Command Line Options */

	if(--argc > 0){  /* Anything on command line ? */
                ++argv;
                while((argc > 0)&&((*argv[0]) == opsep)){ /* loop while 
							there are flags */
                        argstr[0]=opsep;
                        argstr[1]='\0';
                        strcat(argstr,++*argv); /* glob argstr*/
                        argc--;
                        while((argc>0)&&(strlen(*argv)<3)&&
				((*(argv+1))[0]==opsep)&&((*(argv+1))[1]!=opsep)){
                                ++argv;
                                argc--;
                                strcat(argstr,++*argv);
                        }
                        ++argv; /* ok, consecutive flags now globbed into one*/
                       code = find_opt(argstr+1); /* look up flag code */
                switch(code){

			case -1: break;  /* default, e.g no options */

			case 0: printf("%s\n",USAGE);
			        printf("%s\n",info);
				return 0;
			case 1: printf("%s\n",VERSION);
				return 0;
			case 2: read_file = 1;
				fprintf(stderr,"%s: reading from stdin\n",ProgramName);
				break; 
			case 3: 
/*
				for(i=3;i<100;i++)
					if(is_good_modulus(i))
						printf("%d (phi = %d)\n",i,phi(i));
*/
				printf("Inverse of 11 = %u\n",
					m = mul_inv(11));
				printf("Product = %u\n",
					11U*m);
				printf("17 mod 11 = %u\n",
					17 - 11*(17*m));
				return 0;
				break;
			case 4:
				quiet = 1;
				break;
                        case -2: ;  /* barf (don't use in your code) */
			default:
				fprintf(stderr,"%s: illegal option\n",argstr);
				fprintf(stderr,USAGE);
				return 1;
			} /* End flag switch */
	} /* End flag grabbing while loop */
} /* end if: any args ? */
else {
	fprintf(stderr,USAGE);
	return 1;
}

/*   argc now gives a count of remaining arguments on command line,
	and argv points at the first one */

	/* For now, just get the target numbers from the command line. Later, we should add an option to read them
            from a file */

	if(read_file){
		N = 0;
		while(fgets(line_buf,256,stdin)!=NULL){
			c[N] = atoi(line_buf);
			u[N] = (unsigned)c[N];
			N++;
		}
	}
	else {
 		N = argc-1;
	
		for(i=0;i<N;i++){	
			c[i] = atoi(argv[i+1]);
			u[i] = (unsigned)c[i];
		}
	}
	/* Brute force search for smallest p that works: done at "compile time" */

	for(k=0;k<K;k++){
		for(i=0;i<N;i++){
			for(j=0;j<i;j++){
				if(u[i]==u[j]){
					myerror("%s\n","Non unique labels");
					return 1;	
				}
				s = u[i] > u[j] ? u[i] - u[j] : u[j] - u[i];
				if((s%(p[k]-1))==0)break; 
			}
			if(j<i)break;
		}
		if(i == N)break;
	}
	Max = min = c[1];
	for(i = 0; i < N; i++){
		if(c[i] > Max)Max = c[i];
		if(c[i] < min)min = c[i];
	}
	Range = (unsigned)(Max - min);
	if(k<K){
		s = p[k];
		j = 0;
		while((s < UINT_MAX)&&(s > 0)){s = s<<1; j++;}
		if(!quiet){
			printf("Found suitable p = %d\n",p[k]);
			printf("Largest representable integer = %u\n",UINT_MAX);
			printf("Largest safe shift = %d binary digits left\n",j-1);
			printf("Largest case label = %d\n",Max);
			printf("Smallest case label = %d\n",min);
			printf("Case label range = %u\n",Range);
			printf("Load factor = %f\n", ((double)N)/((double)p[k]));
		}
		s = j-1;
		for(i=0;i<N;i++){
/* Stuff that is done at "run time" */
			m = 1;
			j = u[i] % (p[k]-1);
			while(j>s) {m = (m<<s)%p[k]; j -=s; }
			if(j) m = (m<<j)%p[k];
			printf("i = %d, c[i] = %d, hash value = %u\n",i+1,c[i],m);
		}
			
	}
	else {

		printf("Bummer, man. No suitable prime found. So fire me.\n");
		return 1;	
	}

}

/*  find_opt: return a unique small integer for each possible option string */
/*  There should be a case in the switch statement in main to handle each. */
/*  -2 reserved for arg not found -1 reserved for no options */

/* N.B. word doesn't contain the leading hyphen */

int find_opt(char *word)
{
	if(strcmp(word,"help")==0)return 0;
	if(strcmp(word,"h") == 0)return 0;
	if(strcmp(word,"v") == 0) return 1;
	if(strcmp(word,"f") == 0) return 2;
	if(strcmp(word,"x") == 0) return 3;
	if(strcmp(word,"q") == 0) return 4;
	if(strcmp(word,"qf")==0){quiet=1;return 2;}
	if(strcmp(word,"fq")==0){quiet=1;return 2;}

/* arg not found */
	return -2;
}

/* myerror: generic error reporting routine */

void myerror(char *fmt, ...)
{
        va_list args;

        va_start(args,fmt);
        fprintf(stderr,"%s: ",ProgramName);
        vfprintf(stderr,fmt,args);
        fprintf(stderr,"\n");
        va_end(args);
        exit(1);
}


/* Return 1 if 2 is a primitive root for the given modulus, else 0 */


int is_good_modulus(int m)
{

	int i,k,j;

	if(m%2 == 0)return 0;
	k = phi(m);
	j = 1;
	for(i=0;i<k;i++){
		j = ((j<<1)%m);
		if(j == 1)break;
	}
	if(i == k-1)return 1;
	return 0;
}

unsigned mul_inv(unsigned p)
{
	unsigned q,r;
	if(p%2==0)return 0;
	r = p;
	while(r != 1){
		q = r;
		r *= p;  /* reduced mod UINT_MAX */
	}
	return q;
}	
