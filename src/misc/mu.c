/* mu.c: Derive "Theorems" in Hofstader's Mu Language. */


/* 
 * Hofstader's Mu language is a very simple model of a formal language
 * and deductive system. It was introduced in D. Hofstader, Godel, Escher,
 * Bach: an Eternal Golden Braid, Vintage Books, 1980, pp 33-35. 
 *
 * The language consists of strings in the letters M, I, and U and has
 * a single axiom: MI. 
 *
 * There are 4 rules of deduction which allow new strings to be derived
 * from old. These are described by the following "productions", in which
 * X and Y stand for an arbitrary (possibly empty) strings:
 *
 * 1) XI -> XIU
 *
 * 2) XIIIY -> XUY
 *
 * 3) XUUY -> XY
 *
 * 4) MX -> MXX
 *
 * All strings derived from the axiom using one or more applications of
 * 1-4 are called theorems.
 *
 * This program prints theorems until no more rules can be applied. If
 * at any point more than one rule applies, one of them is selected at
 * random.
 *
 * Why is the language called mu? Because Hofstader poses the following
 * puzzle in his book: Is MU a theorem?

*/

/* compile: cc -o mu mu.c 

      Use -D_NO_RANDOM if your library doesn't have random/srandom. Most do,
       	but the only truly portable RNG is rand/srand. Unfortunately it has
        very poor performance, so you should use random if possible.

      Use -D_MAX_RAND=  to set the size of the maximum value returned by
         random(). The portable RNG rand() always returns a maximum of 
         RAND_MAX (defined in stdlib.h), but some implementations of random
         do not use this value. Read the man page for random to be sure. A
	 common value is 2^31-1 = 2147483647. In so, and this is not the
         value of RAND_MAX on your system, you would compile with
         -D_MAX_RAND=214748367.

      Use -D_SHORT_STRINGS if your compiler does not support multiline
          string constants.
*/


#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<string.h>

#define VERSION "1.0"
#define USAGE "mu [ -s number -h -v] [string]"
#ifndef _SHORT_STRINGS
#define HELP "mu [ -s number -h -v ] [string]\n\n\
Print theorems generated by Hofstader's MU language starting from the\n
given string, if provided, or from the string MI if not. All strings must\n
start with M and contain only the letters U and I after that.\n\n\
-s: Use next argument as RNG seed. (otherwise use system time as seed.)\n\
-v: Print version number and exit. \n\
-h: Print this helpful information. \n\n"
#else
#define HELP USAGE
#endif

#define TRUE 1
#define FALSE 0

#ifdef _NO_RANDOM
#define RANDOM rand
#define SRANDOM srand
#else
#define RANDOM random
#define SRANDOM srandom
#endif

/* Default values */
#define MAX_LENGTH 4096
#define MAX_CYCLES 6
#define INITIAL_SEED 31415
#define AXIOM "MIIIIIIII"

int
main(int argc, char **argv)
{
                                      
	int i,j = 0,k;
	int ok0 = TRUE;
	int ok1 = TRUE;
	int ok2 = TRUE;
	int matched = FALSE;
	int cycle = 0;
	char *p,*q;
	char thm[MAX_LENGTH] = AXIOM; 
	char buf[MAX_LENGTH];
	int len = 2;
	long seed=0;

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
					fprintf(stderr,"mu: unkown option %s\n",
						argv[j]);
					exit(1);
			}
	}
 
	if(j < argc){
		strcpy(thm,argv[j]);
		len = strlen(thm);
		/* Bug: do a sanity check on thm here */
	}

	/* If no seed is supplied, then use current system time */
	
	if(!seed)
		if((seed = (long)time(NULL)) == -1){
			seed = INITIAL_SEED; /* if all else fails */
			fprintf(stderr, "Using seed = %d\n",INITIAL_SEED);
		}
		
	SRANDOM((int)seed);

	while(1) 
	{
		/* print the current theorem */

		if(cycle++ > MAX_CYCLES)break;
		printf("%s\n",thm);
		ok0=ok1=ok2=TRUE; /* Reset for next deduction */
		matched = FALSE;

		while(1){

		/* Get a random number between 0 and 2 to decide which
		 * production to try next. Note that 3 is always applicable,
		 * but we use it only as a last resort */

			j = RANDOM()%3;
			switch(j){

			case 0:
				if(ok0 != TRUE)continue;
				if(thm[len-1] == 'I'){
					matched = TRUE;
					if(len < MAX_LENGTH-1){
						thm[len++] = 'U';
						break;
					}
					else exit(1);
				}
				else ok0 = FALSE;
				break;
			case 1:
				if(ok1 != TRUE)continue;
				p = strstr(thm,"III");
				if(p){
					matched = TRUE;
					/* Find out how many IIIs there are */
                                        i=1;
					q=p+1;
					while(strstr(q,"III")){
						i++;
						q++;
					}
					if(i>1){
						k = random()%i;
						/* We'll go with the kth
						 * one */
						p=thm;
						for(i=0;i<k;i++)
							p=strstr(p+1,"III");
					}
					*p = '\0';
					strcpy(buf,thm);
					strcat(buf,"U");
					p += 3;
					strcat(buf,p);
					strcpy(thm,buf);
					len -= 2;
					break;
				}
				else ok1 = FALSE;
				break;
			case 2:
				if(ok2 != TRUE)continue;
				p = strstr(thm,"UU");
				if(p){
					/* Find out how many UUs there are */
					matched = TRUE;
                                        i=1;
					q=p+1;
					while(strstr(q,"UU")){
						i++;
						q++;
					}
					if(i>1){
						k = random()%i;
						/* We'll go with the kth
						 * one */
						p=thm;
						for(i=0;i<k;i++)
							p=strstr(p+1,"UU");
					}
					*p = '\0';
					strcpy(buf,thm);
					p += 2;
					strcat(buf,p);
					strcpy(thm,buf);
					len -= 2;
					break;
				}
				else ok2 = FALSE;
				break;
			default:
				fprintf(stderr,"Impossible production number generated: %d\n", j);
				exit(1);
			}
			if((ok0 == FALSE)&&(ok1==FALSE)&&(ok2==FALSE)){

				/* Do last (doubling) production as a
				 * last resort */
				strcpy(buf,thm+1);
				if(2*len-1 < MAX_LENGTH){
					strcat(thm,buf);
					len += strlen(buf);
				}
				else exit(1);
				matched = TRUE;
			}
			if(matched==TRUE)break; /* Go to outer loop to make next
					     theorem */
			else continue;
		}
	}

	return 0;
}
	


