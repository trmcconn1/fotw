/* 3half.c:  search for a good hash function based on the baker's transformation */


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>


/* Table of powers of 2 */
unsigned p[] = {1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536,131072,262144,524288,1048576,
2097152,4194304,8388608,16777216,33554432,67108864,134217728,268435356,536870912,1073741824,2147483648};

static unsigned K = 32;

#define NMAX 16384
#define INFO  "Create hash function to map x1... to unique small integers"
#define USAGE "dwarf [-hqvx] -f | x1, x2, ..."
#define MAX_ARG_LEN 80             /* Max length of combined options */
#define VERSION "1.0"
int find_opt(char *option_string);
void myerror(char *fmt, ...);
extern int phi(int);
unsigned rotate(unsigned k, int n);
unsigned scramble(unsigned k);
char ProgramName[256];
static int c[NMAX];
static unsigned u[NMAX];
static unsigned v[NMAX];
static int quiet = 0;

int main(int argc, char **argv)
{

	unsigned N,i,j,k,m,s,l;
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

			/* -x option: for testing things */

				m = 1;
				for(i=1;i<32;i++)
					printf("%d rotated right %d times = %u\n",m,i,rotate(m,i));

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

/* Sanity check */

	for(i=0;i<N;i++){
		for(j=0;j<i;j++)
			if(u[i]==u[j]){
				myerror("%s\n","Non unique labels");
				return 1;	
			}
		v[i]=u[i];
	}

/*  Brute force search */

	m = 31;
	for(l=0;l<1000;l++){	
	for(k=0;k<K;k++){
		for(i=0;i<N;i++){
			for(j=0;j<i;j++){
				s = v[i] > v[j] ? v[i] - v[j] : v[j] - v[i];
				if((s%(p[k]))==0)break; 
			}
			if(j<i)break;
		}
		if(i == N)break;
	}
	if(k < m)m = k;  /* New record */
	for(i=0;i<N;i++)v[i] = scramble(v[i]);
	}

	k = m; /* The best we could do */

/* Gather some statistics on the data */
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
			printf("Found best power of 2 = %d\n",p[k]);
			printf("Largest representable integer = %u\n",UINT_MAX);
			printf("Largest safe shift = %d binary digits left\n",j-1);
			printf("Largest case label = %d\n",Max);
			printf("Smallest case label = %d\n",min);
			printf("Case label range = %u\n",Range);
			printf("Load factor = %f\n", ((double)N)/((double)p[k]));
		}
#if 0
		s = j-1;
		for(i=0;i<N;i++){
/* Stuff that is done at "run time" */
			m = 3;
			j = u[i] % p[k];
			while(j) {m *= 3; m = m % p[k+2]; j--;}
			printf("i = %d, c[i] = %d, hash value = %u\n",i+1,c[i],m);
		}
#endif
			
	}
	else {

		printf("Bummer, man. No suitable power of 2 found. So fire me.\n");
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


/* Slow version of rotate right for algorithm testing */

unsigned rotate(unsigned k, int n)
{
	unsigned lsd;
	int j;

	for(j=0;j<n;j++){
		lsd = k&1;
		k = k >> 1;
		if(lsd) k += (1<<31);
	}
	return k;
}
	
/* scramble a 32 bit number by using a "baker's transformation" */

unsigned scramble(unsigned k)
{
	k = (k & 0xFFFF)*65536 + k/65536; /* swap low and high words */
	return rotate(k,1);
}	
