// dbruijn.cpp : Defines the entry point for the console application to generate debruijn
// strings by various methods

#define CPLUSPLUS  // only this file is C++. All the others are C
#define _CRT_SECURE_NO_WARNINGS   // Shut up, already!
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include "stdafx.h"
#include <windows.h>

#endif
#include "debruijn.h"

#ifndef VERSION
#define VERSION "1.0"
#endif
#define USAGE "debruijn [-2345h] [-ev n [seed]] [[-abcv][-r m] [-m j] [-f filename] n [key]]:\n Create debruijn string of order n using key\n"
#define USAGE1 "-h: print this helpful usage information\n-v: produce more verbose output\n"
#define USAGE2 "-a: dump anti-path cycle decomposition\n"
#define USAGE3 "-2: print all order 2 debruijn cycles\n-3: print all order 3 debruijn cycles\n"
#define USAGE4 "-4: print all order 4 debruijn cycles\n-5: print all order 5 debruijn cycles. (Why no -6 ? :-)\n"
#define USAGE5 "-b: reverse (backwards) output\n-c: bitwise complement output\n"
#define USAGE7 "-f: get the key from the character file named instead of from command line\n"
#define USAGE6 "-r: rotate output by m places (first digit becomes m+2nd, etc)\n"
#define USAGE8 "-e: create n terms of seeded Ehrenfeucht-Mycielski sequence\n"
#define USAGE9 "-m: use method number j instead of default (Ehrenfeucht-Mycielski) method\n"
#define USAGE10 "\t1: prefer 1 method  (key is ignored)\n"
#define USAGE11 "\t2: key word method (first n characters of key used as key word)\n"
#define USAGE12 "\t3: key sequence method with prefer 1 preference and debruijn (n-1) key\n"
#define USAGE13 "\t4: key sequence with random preference seeded by initial n-1 string\n\t   of key\n"
#define USAGE14 "\t5: linear recurrence, key = recursion octal code \n\t   (no key: list known maximum period codes)\n"
#define USAGE15 "\t6: prefer same, key ignored\n\t7: prefer opposite, key ignored"

#define HELP USAGE USAGE1 USAGE2 USAGE3 USAGE4 USAGE5 USAGE6 USAGE7 USAGE8 USAGE9 USAGE10 USAGE11 USAGE12 USAGE13 USAGE14 USAGE15

static char keybuf[MAX_STRING]; // used in this file
extern "C" char key[];   // used by readfile 



void order5all(void);  /* Implemented in this file after main */

/*  preference functions for keyseq, needed by order5all */
char onepref(char *){
	return '1';
}
static int first_call;
static unsigned seed;
char randompref(char *p)
{
	char *s;
	if(first_call == 0){ // seed RNG from p
		s=p;
		while(*s != '\0'){if(*s = '1')seed = 2*seed + 1;
			else seed = 2*seed; s++;}
		srand(seed); 
		first_call = 1;
	}
	/* BUG: we should use a better RNG */
	if(rand() > RAND_MAX/2)return '1';
	else return '0';
}

#ifdef WIN32
int main(int argc, WCHAR* argv[])
#else
int main(int argc, char *argv[])
#endif
{
	int n;
	int method = 0;  /* 0 = default, 1 = prefer1, 2 = keyword */
	int keylen;
	int namelen;
	int emflag = 0;
	int dumpantiflag = 0;
	int complementflag = 0;
	int readfileflag = 0;
	int reverseflag = 0;
	int rotateby = 0;
	int i,j=0;
	char *p;
	char *keysrc;
	char *keylib[] = {(char *)TESTSEQ1,(char *)TESTSEQ2,(char *)TESTSEQ3,(char *)TESTSEQ4,(char *)TESTSEQ5,(char *)TESTSEQ6,(char *)TESTSEQ7,(char *)TESTSEQ8};
	char namebuf[256];
	unsigned octalkey;

/* BUG: We really should use wide chars as option letters. Otherwise it 
   may break on a different endian machine.  */

	/* Process command line */
	while(++j < argc){
		if(argv[j][0] == '-')
			switch(argv[j][1]){ 
				case '-':
					++j;
					break;
				case 'v':
				case 'V':
					verbose = 1;
					continue;
				case 'a':
					dumpantiflag = 1;
					continue;
				case 'b':
					reverseflag = 1;
					continue;
				case 'c':
					complementflag = 1;
					continue;
				case 'e':
					emflag = 1;
					continue;
				case 'r':
					++j;
#ifdef WIN32
					rotateby = _wtoi(argv[j]);
#else
					rotateby = atoi(argv[j]);
#endif
					continue;
				case 'm':
					++j;
#ifdef WIN32
					method = _wtoi(argv[j]);
#else
					method = atoi(argv[j]);
#endif
					continue;
				case 'f':
					++j;
#ifdef WIN32
					wcstombs_s((size_t *)&namelen,namebuf,256,argv[j],wcslen(argv[j]));
#else
					strcpy(namebuf,argv[j]);
					namelen = strlen(namebuf);
#endif
					readfileflag = 1;
					continue;
				case '?':
				case 'h':
				case 'H':
					printf("\n\ndebruijn (version %s) by Terry R. McConnell (trmcconn@syr.edu)\n",VERSION);
					printf("Maximum order supported = %d. Set by MAX_LEVEL from makefile.\n",MAX_LEVEL);
					printf("%s\n",HELP);
					exit(0);
				case '2':
					printf("11001\n");
					return 0;
				case '3':
					printf("1110100011\n");
					printf("1110001011\n");
					return 0;
				case '4':
					for(i=0;i<8;i++){
						for(j=0;j<19;j++)printf("%c",*(keylib[i]+j));
						printf("\n");
					}
					for(i=0;i<8;i++){
						for(j=0;j<19;j++)printf("%c",COMPLEMENT(*(keylib[i]+j)));
						printf("\n");
					}
					return 0;
				case '5':
					order5all();
					return 0;
				default:
					fprintf(stderr,"debruijn: unkown option %s\n",
						argv[j]);
					fprintf(stderr,"%s\n",USAGE);
					exit(1);
			}
		break;
	}

	if(j >= argc){ /* we need a non-option argument */
		fprintf(stderr,"debruijn: usage error.\n");
		fprintf(stderr,"%s\n",USAGE);
		return 1;
	}
#ifdef WIN32
	n = _wtoi(argv[j]);
#else
	n = atoi(argv[j]);
#endif
	if(n <= 0){
		fprintf(stderr,"debruijn: bad argument: %s. Must be integer 1 - %d\n",argv[j],MAX_LEVEL);
		return 1;
	}
	j++;
	if(readfileflag){
			keylen = readfile(namebuf);
			keysrc = key;
	}
	else {
		keysrc = keybuf;
		if(j<argc){
#ifdef WIN32
			wcstombs_s((size_t *)&keylen,keybuf,MAX_STRING,argv[j],wcslen(argv[j]));
#else
			strcpy(keybuf,argv[j]);
#endif
			keylen = (int) strlen(keybuf);
		}
		else keylen = 0;
	}
	if(!emflag &&(n > MAX_LEVEL)){
		fprintf(stderr,"Order %d too large (Max supported = %d)\n", n,MAX_LEVEL);
		return 1;
	}
	if(emflag){
		p = EM(keysrc,n);
		if(verbose)printf("[seed=%s]",keysrc);
		if(p)printf("%s\n",p+strlen(keysrc));
		else { /* This should not happen */
			fprintf(stderr,"He's dead, Jim\n");
			fprintf(stderr,"Failed to generate output. Please report circumstances to trmcconn@syr.edu\n");
			return 1;
		}
		return 0;
	}
	if((method < 0)||(method > 7)){
		fprintf(stderr,"Unknown method = %d. Using default.\n",method);
		method = 0;
	}
	if(method == 0)
		p = fixedpoint(keysrc,n-1); // default algorithm
	if(method == 1)
		p = prefer1(n);
	if(method == 2){
		if(keylen < n){
			fprintf(stderr,"key length %d too small. Must be at least %d\n",keylen,n);
			return 1;
		}
		if(keylen > n)keysrc[n]='\0';
		p = keyword(keysrc,n);
	}
	if(method == 3){
		if(!isdebruijn(keysrc,n-1)){
			fprintf(stderr,"key is not a debruijn string of order n-1\n");
			return 1;
		}
		p = keyseq(keysrc,&onepref,n);
	}
	if(method == 4){
		if(!isdebruijn(keysrc,n-1)){
			fprintf(stderr,"key is not a debruijn string of order n-1\n");
			return 1;
		}
		p = keyseq(keysrc,&randompref,n);
	}
	if(method == 5){
		if(strlen(keysrc)==0){
			if(verbose)printf("Octal keys from Table III-5, Golomb, Shift Register Sequences\n");
			if(n==1){printf("%s\n",DEG1CODES);return 0;}
			if(n==2){printf("%s\n",DEG2CODES);return 0;}
			if(n==3){printf("%s\n",DEG3CODES);return 0;}
			if(n==4){printf("%s\n",DEG4CODES);return 0;}
			if(n==5){printf("%s\n",DEG5CODES);return 0;}
			if(n==6){printf("%s\n",DEG6CODES);return 0;}
			if(n==7){printf("%s\n",DEG7CODES);return 0;}
			if(n==8){printf("%s\n",DEG8CODES);return 0;}
			if(n==9){printf("%s\n",DEG9CODES);return 0;}
			printf("No codes yet reported. Feel free to edit debruijn.h to add more.\n");
			return 1;
		}
		sscanf(keysrc,"%o",&octalkey);
		p = linear((unsigned)octalkey,n);
		if(!p) { /* This should never happen */
			fprintf(stderr,"He's dead, Jim\n");
		    fprintf(stderr,"Failed to generate output. Please report circumstances to trmcconn@syr.edu\n");
		    return 1;
		}
		if(!isdebruijn(p,n)){
			fprintf(stderr,"Key did not yield a debruijn string. Try another.\n");
		}
	}
	if(method == 6){
		p = prefer_same(n);
	}
	if(method == 7){
		p = prefer_opposite(n);
	}
	if(!p){
		/* This should never happen */
		fprintf(stderr,"He's dead, Jim\n");
		fprintf(stderr,"Failed to generate output. Please report circumstances to trmcconn@syr.edu\n");
		return 1;
	}
	if(complementflag)complement(p);
	if(reverseflag)reverse(p);
	if(rotateby)rotate(p,rotateby,n);
	printf("%s\n",p);
	if(verbose)dumppath(p,n);
	if(dumpantiflag)dumpantipath(p,n);
	return 0;
}



/* Generate all 2048 possible debruijn cycles of order 5 
 */

static unsigned int prefselect; /* 0-2^8 */
static char mypref(char *argstr){
	
	unsigned int i,k=0,m;
	/* Find binary value of argument string of length 3 */
	m = 1;
	for(i=0;i<3;i++){if(argstr[i]=='1')k+=m;m = 2*m;}/* k in range 0-7 */

	/* get the kth binary digit of prefselect to see what to return */
	m = prefselect;
	for(i=0;i<k;i++)m = m/2;
	if(m%2)return '1';
	return '0';
}


void order5all ( void ){
	
	unsigned int i,j;
	char *p;

	char *keylib[] = {(char *)TESTSEQ1,(char *)TESTSEQ2,(char *)TESTSEQ3,(char *)TESTSEQ4,(char *)TESTSEQ5,(char *)TESTSEQ6,(char *)TESTSEQ7,(char *)TESTSEQ8};

	for(i=0;i<8;i++)
		for(j=0;j<256;j++){
			prefselect  = j;
			p = keyseq(keylib[i],&mypref,5);
			if(p==NULL){fprintf(stderr,"order5all: got null pointer from keyseq i=%d,j=%d\n",i,j);return;}
			printf("%s\n",p);
		}
}





						
