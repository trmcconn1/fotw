/* mkrotor.c: create a random permutation suitable to be used as an enigma
   rotor (see enigma.c in the crypto package.) Also handles creation of
   enigma reflectors.

   Author: Terry R. McConnell
 
   This program was mainly intended as a tool to help the author set up the
   rotor definitions in enigma.c, but it could be of some interest to
   abstract algebra students. For example, it prints the cycle decomposition
   of permutations. Because it is not intended for general use, the
   programming is not bulletproof. (Not that any of my programs are bullet-
   proof, but this one is certainly not.) 

   To configure for other alphabets, redefine NCHARS and START_CHAR

*/

#include <stdio.h>
#include <stdlib.h>


#define VERSION "1.0"
#define USAGE "mkrotor [ -s number -g number -r -c -h -v] "
#ifndef _SHORT_STRINGS
#define HELP "\nmkrotor [ -s number -g number -r -c -h -v ]\n\n\
Create a random permutation for 'wiring' an enigma machine rotor.\n\n\
-s: Use next argument as RNG seed. (otherwise use system time as seed.)\n\
-r: Make a reflector (transpositions only) rather than a rotor.\n\
-g: Specify cycle lengths (may be repeated.) Must total to char set size. \n\
-c: Print the cycle decomposition of the rotor permutation.\n\
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



#define INITIAL_SEED 3445  /* Had to be something */
#ifndef _MAX_RAND
#define _MAX_RAND RAND_MAX
#endif

#ifndef NCHARS
#define NCHARS 95  
#endif
#ifndef START_CHAR
#define START_CHAR 32 
#endif

/* return a number chosen at random from 0,1...,k-1 */

int random_on(int k){

        double U;   /* U(0,1) random variable */
        int rval;

        U = ((double)RANDOM())/((double)_MAX_RAND);
        rval = (int)((double)k*U);
        return rval;
}

/* Return a cyclic permutation of letters. Marks letters used with -1 */

int * gen_cycle(int len, int src_len, int *letters){

	int *buf,j,i,l,k;
	
	if((buf = (int *)malloc(len*sizeof(int)))==NULL){
		fprintf(stderr,"gen_cycle: cannot malloc\n");
		exit(1);
	}

	/* Generate random cyclic permutation of length len
           in buf array. Algorithm: Put randomly chosen letter
           in position 0 of buf array, and mark position in letters
           with a -1. Then choose a letter at random from the unmarked
           letters, and continue until len have been chosen. */


		i = 0;
		while(i < len){

			j = random_on(src_len-i)+1;

			/* find jth unmarked letter */
			l=0,k=0;
			while((k<src_len)&&(l<j)){
				if(*(letters+k)!=-1)l++;
				k++;
			} /* k-1 is now index of jth unmarked letter */ 

			*(buf+i)=*(letters+k-1); /* choose letter */
			*(letters+k-1) = -1;   /* mark as choosen */
			i++;
		}
		return buf;

}

/* Extract letters not marked as -1 from src and return as int array. 
   Return length of array through pointer */

int * extract_letters(int slen, int *rlen, int *src){

	int *buf,j,i,l,k;
	

	/* make a pass through src to count unmarked letters */

	k=0;
	for(i=0;i<slen;i++)
		if(*(src+i)!=-1)k++;


	/* allocate return buffer */

	if((buf = (int *)malloc(k*sizeof(int)))==NULL){
		fprintf(stderr,"extract_letters: cannot malloc\n");
		exit(1);
	}

	/* 2nd pass to transfer letters */

	j=0;
	for(i=0;i<slen;i++)
		if(*(src+i)!=-1)
			*(buf+j++) = *(src+i);

	*rlen = k; /* poke return length */
	free(src);
	return buf;
}

int main(int argc, char **argv)
{

	int i,j=0,k,l,m;
        /* These control various functions of the program */
	int c_flag = 0;     /* print cycle decompostions */
	int r_flag = 0;     /* create reflector: transpositions only */
	int g_flag = 0;     /* create with given cycle lengths */
	int n_flag = 1;     /* "normal" operation: random permutation */
	int cycles = 0;
	int nchars = 0;
	int lengths[NCHARS];
	long seed = 0;
	int mark_buf[NCHARS];
	int rbuf[NCHARS];

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
				case 'g':
				case 'G':
					g_flag=1;
					n_flag=0;
					if(cycles>NCHARS){
						fprintf(stderr,"mkrotor: too many cycles\n");
						exit(1);
					}
					i = lengths[cycles] = atoi(argv[j+1]);
					nchars += i;
					/* Sanity check on i */
					if((i<=0) || (i >= NCHARS)){
						fprintf(stderr,"mkrotor: cycle length %d crazy\n",i);
						exit(1);
					} 
					cycles++;
					j++;
					continue;
				case 'v':
				case 'V':
					printf("%s\n",VERSION);
					exit(0);
				case 'c':
				case 'C':
					c_flag = 1;
					continue;
				case 'r':
				case 'R':
					r_flag = 1;
					n_flag = 0;
					continue;
				case '?':
				case 'h':
				case 'H':
					printf("%s\n",HELP);
					exit(0);
				default:
					fprintf(stderr,"mkrotor: unkown option %s\n",
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


	if(n_flag){

	/* Generate random permutation on 0...NCHARS and store
           in mark_buf array. Algorithm: Put 0 in a position in the
           mark_buf array chosen at random. We say that position has now
           been "marked", since its value has changed from the unmarked
           value (-1, set below.) Then put 1 in a position chosen at
           random from the remaining unmarked positions, etc. */

	/* initialize marks */

		for(j=0;j<NCHARS;j++)mark_buf[j] = -1;

		i = 0;
		while(i < NCHARS){

			j = random_on(NCHARS-i)+1;

			/* find jth unmarked position */
			l=0,k=0;
			while((k<NCHARS)&&(l<j)){
				if(mark_buf[k]==-1)l++;
				k++;
			} /* k-1 is now index of jth unmarked entry */ 

			mark_buf[k-1]=i; /* mark entry */ 
			i++;
		}
	}

	if(r_flag) { /* Making a reflector. Generate a permutation made up
                  only of transpositions (if NCHARS is even,) or of all
                  transpositions except for one fixed point if NCHARS is
                  odd. Except for this, the permutation is to be chosen
                  at random. The algorithm here is more complicated. We
                  mark the initial number in each transposition in mark_buf
                  randomly as before, and mark the second number in an
                  auxiliary array called rbuf. Later, we go back through
                  and mark_buf and fill in the second members of 
                  each transposition. */ 

	/* initialize marks */

		for(j=0;j<NCHARS;j++)mark_buf[j] = -1;
		for(j=0;j<NCHARS;j++)rbuf[j] = -1;
		i = 0;
		m = 0;
		while(m < NCHARS){ /* m increases by 2 on each successful
                                      pass through this loop. A pass can be
                                      unsuccessful if it would generate a
                                      fixed point. */

			j = random_on(NCHARS-m)+1;

			/* find jth unmarked entry. entries in rbuf
                           are considered as marked for this purpose. */
			l=0,k=0;
			while((k<NCHARS)&&(l<j)){
				if((mark_buf[k]==-1)&&(rbuf[k]==-1))l++;
				k++;
			} /* k-1 is index of jth unmarked entry */ 

			 /* If this would make a fixed point, we'll just 
		            have to try again. But be careful if NCHARS is 
			    odd: there will have to be one fixed point left 
                            over at the end, so we don't want to loop forever 
                            in that case.  */ 
			if((m<NCHARS-1) && (i==(k-1)))continue;

			mark_buf[k-1]=i; /* mark as found */ 
			rbuf[i] = k-1; /* Will get marked below */
			i++;
			m+=2;
			/* skip over ones that will get added later */
			while((i<NCHARS)&&(mark_buf[i]!=-1))i++;
		}

	    /* We still need to record the second half of transpositions */

		for(i=0;i<NCHARS;i++)
			if((k=mark_buf[i])!=-1)
				mark_buf[k] = i;

	}

	if(g_flag){ /* Make permutation with cycle lengths stored in
                       lengths array, otherwise random */

		int slen;
		int len;
		int *letters;
		int *acycle;
		int cur_letter;
		int next_letter;

		
		if(nchars != NCHARS){
			fprintf(stderr,"mkrotor: cycles use %d chars < total = %d\n",
				nchars,NCHARS);
			exit(1);
		}
		letters = (int *)malloc(NCHARS*sizeof(int));
		if(!letters){
			fprintf(stderr,"mkrotor: cannot malloc\n");
			exit(1);
		}
		for(slen=0;slen<NCHARS;)
			*(letters+slen) = START_CHAR+slen++;

		for(i=0;i<cycles;i++){

			len = lengths[i];
			if(len <= 0)break;
			if(len>slen){
				fprintf(stderr,"mkrotor: cycle too big\n");
				exit(1);
			}
			acycle = gen_cycle(len,slen,letters);

			/* store permutation indices */
			cur_letter = *(acycle)-START_CHAR;
			for(j=0;j<len;j++){
				next_letter = *(acycle+j)-START_CHAR;
				mark_buf[cur_letter] = next_letter; 
				cur_letter = next_letter;
				}
			mark_buf[cur_letter]=*(acycle)-START_CHAR;
			letters = extract_letters(slen,&slen,letters);
		}
		free(letters);
	}

	/* write buffer out */
	if(!c_flag){
		printf("{"); /* initial boilerplate */
		for(i=0;i<NCHARS;i++)
			printf("%d,",mark_buf[i]);
		printf("}\n"); /* final boilerplate */
	}
	else { /* print cycle decomposition of permutation */

		/* Follow a cycle, printing out the corresponding
                   char values as we go, and converting the permutation
                   entries back to -1 to mark them as counted */
		k=0;
		i=1;
		while(k<NCHARS){
			j = mark_buf[k];
			mark_buf[k] = -1;
			printf("Cycle %d: %c",i++,START_CHAR+j);
			m=0;
			while((l=mark_buf[j])!=-1){ /* return to marked at 
							end of a cycle */
				printf("%c",START_CHAR+l);
				mark_buf[j]=-1;
				j=l;
				m++;
			}
			printf("  (length %d)\n",m+1);

			/* Find beginning of next cycle, if any */
			while((k<NCHARS)&&(mark_buf[k]==-1))k++;
		}
	}

	return 0;
}
