/* Generate the Thue-Morse sequence and related things. */

/* The sequence is an infinite binary sequence introduced by Axel Thue in a 1906 paper,
Ueber unendliche Zeichenreihen, Norske Vid. Selsk. Skr. I. Mat. Nat. Kl., Christiana no. 7, 1-22. It was
later rediscovered by Marston Morse in the context of dynamical systems: ___, 1921, Recurrent
geodesics on a surface of negative curvature, Trans. Am. Math. Soc., 22, 84-100. It is a fixed point of
the substitution 0 -> 01 and 1 -> 10, and may be constructed, as we do here, by iterating this substitution
indefinitely begining with a 0. The key property of the sequence is that it contains no sections that overlap
each other. The tricky part of showing this is to show that the substitution cannot introduce overlaps where there
were none already. See pp. 23-25 of M. Lothaire, Combinatorics on Words, Cambridge University Press, 
Cambridge, UK, 1997 for a very readable and elegant treatment.

One of Thue's main uses of the sequence was to generate an example of an infinite word m on an alphabet of 3 letters
that contains no squares, i.e., no subsequence of the form tt with t a non-empty word. An example on the alphabet 
{a,b,c} can be constructed as the unique infinite word that produces the Thue-Morse sequence under the 
substituion t: c -> a, b -> ab, a -> abb. See Lothaire, op. cit. p. 26. Our method actually uses Proposition
2.3.2 on page 27 of Lothaire which says that m can be obtained by iterating the map phi on the letter a to
obtain a fixed point. phi(a) = abc, phi(b) = ac, phi(c) = b. The proof of Proposition 2.3.2 is left as an
exercise, so here is a sketch: check the identity t(phi(.)) = mu(t(.)) For example t(phi(a))
t(abc) = abbaba, and mu(t(a)) = mu(abb) = abbaba. Thus t(phi^n(a)) = mu^n(abb). Taking the limit as n ->
infinity gives t(m) = Thue-Morse, and as shown page 26, there is a unigue square free word with this property. */

#define _CRT_SECURE_NO_WARNINGS   /* Shut up, already */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define VERSION "1.0"
#define USAGE "\nthue [-hvdtcrlxi] [-u a b ] [-o filename] n: Print n digits of the Thue-Morse\nsequence followed by newline\n"
#define USAGE1 "-h: print this helpful message and exit\n"
#define USAGE2 "-v: print version number and exit\n"
#define USAGE3 "-d: turn on debug mode\n"
#define USAGE4 "-t: run experimental (test) code and exit (may do nothing)\n"
#define USAGE5 "-u: use the following two characters in place of digits 0 and 1\n"
#define USAGE6 "-c: print bitwise complement\n"
#define USAGE7 "-r: print output in reverse order\n"
#define USAGE8 "-o: print to filename rather than stdout\n"
#define USAGE9 "-l: print a newline character after every 80 characters\n"
#define USAGE10 "-x: print square free sequence on letters a,b,c instead (-c, -u ignored)\n"
#define USAGE11 "-i: print some reference information about the sequence and exit\n"
#define HELP USAGE USAGE1 USAGE2 USAGE3 USAGE4 USAGE5 USAGE6 USAGE7 USAGE8 USAGE9 USAGE10 USAGE11

#define INFO1 "\n\nThe sequence is an infinite binary sequence introduced by Axel Thue in \n"
#define INFO2 "Ueber unendliche Zeichenreihen, Norske Vid. Selsk. Skr. I. Mat. Nat. Kl.,\n"
#define INFO25 "Christiana no. 7, 1-22. (Published in 1906. Followup in 1912.)\n"
#define INFO3 "It was later rediscovered by M. Morse in the context of dynamical systems. \n"
#define INFO4 "See M.Morse, 1921, Recurrent geodesics on a surface of negative curvature,\n"
#define INFO5 "Trans. Am. Math. Soc., 22, 84-100. One of Thue's main uses of the sequence\n"
#define INFO6 "was to generate an infinite word on three letters that contains no square, i.e.\n"
#define INFO7 "no subword of the form tt where t is a nonempty word. See pp. 23-27\n"
#define INFO8 "of M. Lothaire, Combinatorics on Words, Cambridge University Press,\n"
#define INFO9 "Cambridge, UK, 1997.\n\n"
#define INFO INFO1 INFO2 INFO25 INFO3 INFO4 INFO5 INFO6 INFO7 INFO8 INFO9

char *mu(char *, int);
char *phi(char *, int, int *);
void complement(char *src, int n);
void reverse(char *src, int n);

int verbose = 0;
int debug = 0;
int reverseflag = 0;
int complementflag = 0;
int squarefreeflag = 0;
int lineflag = 0;
char zero = '0'; /* symbols for the digits */
char one = '1';  

int
main(int argc, char **argv)
{
	
	char *t,*s;
	int m,n,k;
	FILE *output = stdout;

	/* Process command line options. argc will count number of args
            that remain. */

	
	argv++; argc--; /* skip program name */
    while((argc > 0) && (*argv)[0] == '-'){
		  if(strcmp(*argv,"-h")==0){
			printf("%s\n", HELP);
			exit(0);
		  }
		   if(strcmp(*argv,"-i")==0){
			printf("%s\n", INFO);
			exit(0);
		  }
		  if(strcmp(*argv,"-v")==0){
			printf("%s\n",VERSION);
			return 0;
		  }
		  if(strcmp(*argv,"-d")==0){
			  debug = 1;
			  verbose = 1;
			  ++argv;
			  argc--;
			  continue;
		  }
		  if(strcmp(*argv,"-u")==0){ 
			  if(argc == 0){
				  fprintf(stderr,"thue: usage %s\n",USAGE);
				  return 1;
			  }
			  ++argv;
			  argc--;
			  zero = *argv[0];
			   if(argc == 0){
				  fprintf(stderr,"thue: %s\n",USAGE);
				  return 1;
			  }
			  ++argv;
			  one = *argv[0];
			  argc--;argc--;
			  ++argv;
			  continue;
		  }

		  if(strcmp(*argv,"-c")==0){
			  complementflag = 1;
			  argv++; argc--;
			  continue;
		  }
		  if(strcmp(*argv,"-l")==0){
			  lineflag = 1;
			  argv++; argc--;
			  continue;
		  }
		  if(strcmp(*argv,"-x")==0){
			  squarefreeflag = 1;
			  argv++; argc--;
			  continue;
		  }

		  if(strcmp(*argv,"-r")==0){
			  reverseflag = 1;
			  argv++; argc--;
			  continue;
		  }

		  if(strcmp(*argv,"-o")==0){
			  if(argc == 0){
				  fprintf(stderr,"thue: %s\n",USAGE);
				  return 1;
			  }
			  argv++; argc--;
			  output = fopen(*argv,"w");
			  if(!output){
				  fprintf(stderr,"thue: unable to open %s for output\n",*argv);
				  return 1;
			  }
			  argv++;argc--;
			  continue;
		  }
		  
		  if(strcmp(*argv,"-t")==0){/* Put experimental stuff here */
			  printf("This is a test, this is ONLY a test!\n");
			  
			  return 0;
		  }
		  fprintf(stderr, "thue: Unknown option %s\n", *argv);
		  fprintf(stderr, "%s\n",USAGE);
		  return 1;
	   }
	

	/* There should still be something left on the command line */

	if(argc == 0){
		fprintf(stderr,"thue: usage %s\n", USAGE);
		return 1;
	}

	/* Assume what's left on the command line is a number of digits */
	
	if(debug)printf("thue: debug is on\n");
	
	n = atoi(*argv++);
	argc--;
	if(debug)printf("thue: n = %d digits requested.\n",n);

	if(n <= 0){ /* sanity check */
		fprintf(stderr,"thue: invalid number (%d) of terms requested\n",n);
		return 1;
	}

	k = 1;
	t = malloc(k*sizeof(char));
	if(!t){
		fprintf(stderr,"thue: unable to allocate initial memory in main routine \n");
		return 1;
	}
	
	if(squarefreeflag){ /* Generate Thue's example of a square free sequence on the
						three letter alphabet {a,b,c}. This is done by iterating the phi subsitution
						on the letter a */
		t[0] = 'a'; 
		while(k < n){
			s = phi(t,k,&m);
			free(t);k = m;
			if(!s){
				fprintf(stderr,"thue: null pointer returned by mu routine\n");
				return 1;
			}
			t = s;
		}
	}
	else {
	/* Generate the Thue-Morse sequence by iterating mu operator: mu(a) = ab, mu(b) = ba */
		t[0] = zero;
		while (k/2 < n){
			s = mu(t,k);
			free(t);
			if(!s){
				fprintf(stderr,"thue: null pointer returned by mu routine\n");
				return 1;
			}
			t = s;
			k = 2*k;
		}
	}

	/* Modify output as requested */
	if(reverseflag)reverse(s,n);
	if((!squarefreeflag )&& complementflag)complement(s,n);

	/* Print to stdout the number of terms requested */

	for(k=0;k<n;k++){
		fprintf(output,"%c",s[k]);
		if(lineflag&&(((k+1)%80)==0))fprintf(output,"\n");
	}
	fprintf(output,"\n"); /*BUG: may get two newlines at eof */
	free(s); /* clean up */	
	fclose(output);
}


/* Do character-wise complement of src array in place. n is the number of terms */

void complement(char *src, int n)
{
	int i = 0;
	for(i = 0; i< n; i++)
		if(src[i] == zero) src[i] = one;
		else src[i] = zero;
}

/* reverse first n charcters of src in place */

void reverse(char *src, int n)
{
	char temp;
	int i;

	for(i = 0; 2*i < n; i++){
		temp = src[i];
		src[i] = src[n - i - 1];
		src[n-i-1] = temp;
	}
}

/* mu: perform the following substitution on each binary digit of src: 0 -> 01, 1 -> 10. The size of src
must be supplied with n. Returns a pointer to a character array containing the result of the substitution. 
It is up to the caller to free any unneeded memory */

char *mu(char *src, int n)
{
	int m;
	int i;
	int j = 0;

	char *rval;
	m = 2*n;
	rval = (char *)malloc(m);
	if(!rval)return NULL;
	for(i=0;i<n;i++)
		if(src[i]==zero)
		{rval[j++]=zero;rval[j++]=one;}
		else {rval[j++]=one;rval[j++]=zero;}

	return rval;
}

/* phi: perform the following substitution on the alphabet {a,b,c}: a -> abc, b -> ac, c -> b. The substitution
is performed on each character of src. The size of src
must be supplied with n. Returns a pointer to a character array containing the result of the substitution. 
Also returns through the pointer argument the number of characters in the returned array.
It is up to the caller to free any unneeded memory */

char *phi(char *src, int n, int *l)
{
	int m;
	int i,j=0;
	char *rval;
	
	if(n <= 0)return NULL;

	m = 3*n; /* worst case is when src contains all a's */
	rval = (char *)malloc(m);
	if(!rval)return NULL;

	for(i=0;i<n;i++)
		switch(src[i]){
			case 'a':
				rval[j++]='a';rval[j++]='b';rval[j++]='c';
				break;
			case 'b':
				rval[j++]='a';rval[j++]='c';
				break;
			case 'c':
				rval[j++]='b';
				break;
			default:
				break;
	}
	*l = j;
	return rval;
}


	






	




