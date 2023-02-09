/* longcurt.c: program to print out examples of the by-hand cube root
*  algorithm.
*
* By Terry R. McConnell 12/2003
*
* Usage: longcurt <radicand> <digits>
* where radicand is a decimal number and digits is a natural number
*
* Arguments can be arbitrarily long, subject only to the size of
* ARG_MAX on your system. POSIX.1 ensures that this is at least 4096 bytes.
*
* A bit of useful terminology: 
*   radicand = that whose root is to be extracted
*   subtrahend = that which is subtracted
*/

#include "global.h"
#define PROGNAME "longcurt"

/*   Algorithm and terminology:

             2.  1   5   4
           ___________________________
        3 /
       \/   10.000 000   <------------- radicand
             8
	    ----------
       	     2 000       <------------- second radicand
 1200 
  (61) 
    -
   61
   --
 1261        1 261       <------------ subtrahend
            ------
 132300        739 000
   (635)
      -                 ( Underscores denote digits filled in after
   3175                    estimation. )
   ----
 135500        677 500 
               -------
 13867500               <-------------- major partial
    (6454)              <-------------- (minor factor)
        -
    25816               <-------------- minor partial
    -----
 13893370               <-------------- total partial, contradictory as that
                                                       may sound.

One begins by grouping the radicand digits to the left of the decimal
point in threes (with the most significant digits possibly forming a partial
group of 1 or 2.) The mechanics are similar in outline to those of
the square root algorithm (see longsqrt.c) except for the formation of
the partials. Each (total) partial is the sum of a major and a minor partial.
The major is gotten by multiplying the square of the current result digits
by 300.  The minor partial is the product of
the next result digit and the minor factor. The minor factor is gotten by
multiplying the current result digits by 30, where the trailing zero is to
be replaced by the next result digit. Finally, the next result digit is
the largest possible digit such that its product with the resulting total
partial will not exceed the current radicand.

Why does this work? Let r denote the current approximation to the cube
root of the radicand, x. Then (10r + d)^3 = 1000r^3 + 300r^2d + 30rd^2 + d^3.
Let y denote  floor(x multiplied by the appropriate power of ten to
move the decimal right 3*(number of steps so far)). Then
 y - 1000r^3 represents the next radicand.  300r^2 is then the next
major partial. (30r+d) is then the minor factor and 30rd + d^2 is the
minor partial. Finally, 300r^2 + 30rd + d^2 is the total partial, with 
300r^2d + 30 rd^2 + d^3 the resulting subtrahend.

Formulation of analogous algorithms for fourth and higher order roots is
left as an exercise for the reader :-)

*/

#define BASE_SKIP 3 /* Fixed space skip that accounts for the size of
		       the \/ part of the radical sign */
static char estimate(char *,char *, char *, char **, char **);
static void indent(int);
static int print_len(char *);
static void print_triple(char *);

int 
longcurt(char *radicand, char *ndigits_str)
{
	int i=1,j,k,l;
	int digits_used = 0;
	int negative = 0;
	int leading_zeros = 0;
	int parity;
	int running_indent=0;
	int base_indent = 0;
	int step = 0;
	char *rcnds[ARG_MAX];   /* Successive radicands */
	char *shends[ARG_MAX]; /* Successive subtrahends */
	char *partials[ARG_MAX]; /* Successive total partials */
	char *Mpartials[ARG_MAX]; /* Successive major partials */
	char *mpartials[ARG_MAX]; /* Successive minor partials */
	char *mfactors[ARG_MAX]; /* Successive minor factors */
	char *result;    /* Successive digits in answer */
	char *rcnd;  /* Initial radicand */
	char c;
	int ndigits;
	int rlen,clen;
	char *p,*q; /* utility pointers */

	/* Analyze radicand argument for sanity, normalize, and store. */
	/* This is surprisingly tricky to get right. */

	if(radicand[0]=='-'){
		negative = 1;
		q = radicand+1;
	}
	else q = radicand;

	rlen = strlen(q);

	/* Garbage anywhere in arg ? */
	if(strspn(q,".0123456789") < rlen){
		fprintf(stderr,"%s: Radicand %s is not in proper decimal form.\n",
                            PROGNAME,radicand);
		exit(1);
	}

	clen = strspn(q,DIGITS);

	/* If clen < rlen then our arg apparently has a decimal point.
           Make sure this is the case. */

	if(clen < rlen) {
                /* Second test handles multiple decimal point(!) */
		if((*(q+clen) != '.') ||
                    ( strspn(q+clen+1,DIGITS)<rlen-clen-1) ){
			fprintf(stderr,"%s: Radicand %s is not in proper decimal form\n", PROGNAME, radicand);
			exit(1);
		}
	}

	/* Throw away any leading zeros */
	
	if(strspn(q,"0")){
		p = strpbrk(q,".123456789");
		if(!p){
			printf("The cube root of 0 is 0.\n");
			exit(0);
		}
		rlen -= (p-q);
		clen -= (p-q);
	}
	else p = q;
	rcnd = (char *)malloc((rlen+1)*sizeof(char));
	if(!rcnd){
		fprintf(stderr,"%s: memory error. No space for radicand.\n",PROGNAME);
		exit(1);
	}
	/* copy cleaned up arg, removing decimal point. It will be restored
           at the end using the value clen (characteristic length.) */

	for(j=0;j<clen;j++)
		rcnd[j] = p[j];
	if(j < rlen){
		j++;
		while(j<rlen){
			rcnd[j-1] = p[j]; /* skipped in p, not rcnd */
			j++;
		}
		rcnd[j-1] = 0;			
	}
	else rcnd[j] = 0;

/* Now that the decimal point is removed, we may have introduced more leading
zeros. */

	rlen = strlen(rcnd);
	if(strspn(rcnd,"0")){
		p = strpbrk(rcnd,".123456789");
		if(!p){
			printf("The cube root of 0 is 0.\n");
			exit(0);
		}
		rlen -= (p-rcnd);
		clen -= (p-rcnd); /* This may be negative */
		leading_zeros = p - rcnd; /* Save, for use below */
		for(j=0;j<rlen;j++)
			rcnd[j] = p[j];
		rcnd[j] = 0;
	}

	if(rlen == 0) { /* Our arg was 0 */
		printf("The cube root of 0 is 0\n");
		exit(0);
	}

	i++; /* next arg */

	/* Examine and store digits argument */

	ndigits = atoi(ndigits_str); /* Weak. Better to use strtol ? */

	if(ndigits <= 0){

		fprintf(stderr,"%s: Requested number of digits, %d, is out of range\n",
			PROGNAME,ndigits);
		exit(1);

	}

	/* Ready to roll ... */

	/* The first radicand is either the first digit of rcnd, if clen is
	 * congruent to 1 mod 3, the first 2 digits, if clen is congruent
	 * to 2 mod 3, or the first 3 otherwise. */

	rcnds[0] = (char *)malloc(4*sizeof(char));
	if(!rcnds[0]){
		fprintf(stderr,"main: can't allocate memory for rcnds[0]\n");
		exit(1);
	}
	if(clen >=0)
		parity = clen % 3;
	else { 
		parity = (-clen)%3;
		if(parity){parity = 3 - parity;}
	}
	switch(parity) { 

		case 1:
			rcnds[0][0] = rcnd[0];
			rcnds[0][1]=0;
			digits_used = 1;
			break;
		case 2:
			rcnds[0][0] = rcnd[0];
			if(rlen >= 2)
				rcnds[0][1] = rcnd[1];
			else rcnds[0][1]='0';
			rcnds[0][2] = 0;
			digits_used = 2;
			break;
		default: /* really case 0 */
			rcnds[0][0] = rcnd[0];
			if(rlen >= 2)
				rcnds[0][1] = rcnd[1];
			else { 
				rcnds[0][1]='0';
			       	rcnds[0][2]='0';
			}
			if(rlen >= 3)
				rcnds[0][2] = rcnd[2];
			else rcnds[0][2] = '0';
			rcnds[0][3] = 0;
			digits_used = 3;
			break;
	}

	/* allocate memory to store result */
	result = (char *)malloc((ndigits+1)*sizeof(char));
	if(!result ){
		fprintf(stderr,"main: Cannot allocate memory for answer\n");
		fprintf(stderr,"Requested precision too large for platform?\n");
		exit(1);
	}

	/* Step 0: figure first digit of answer */

	j = 1;
	i = atoi(rcnds[0]);
	while(j*j*j <= i)j++;
	result[0] = '0'+j-1;
	result[1]=0; /* properly terminate */
	/* set up zero-th subtrahend */
	shends[0] = (char *)malloc(4*sizeof(char));
	if(!shends[0]){
		fprintf(stderr,"main: cannot allocate memory for shends[0]\n");
		exit(1);
	}
	sprintf(shends[0],"%d",(j-1)*(j-1)*(j-1));

	partials[0] = (char *)strdup(result);

	/* Main loop of the algorithm: */
	step = 1;
	while(step < ndigits){

		/* Subtract the current subtrahend and bring down the
		 * next three digits of rcnd, or zeros if we are past the
		 * end of it. */

		p = sbc(rcnds[step-1],shends[step-1]);
		if(!p){
			fprintf(stderr,"main: subtraction step %d failed\n",
					step);
			exit(1);
		}
		q = shift_left(p);
		free(p);
		p = shift_left(q);
		free(q);
		q = shift_left(p);
		free(p);
		p = q;

		/* strip leading zeros from new radicand (sbc does not do 
		 * this.) */
		q = strpbrk(p,NONZERO_DIGITS);
		if(q) 
			rcnds[step] = strdup(q);
		else 
			rcnds[step] = strdup("000");
		free(p);
		i = strlen(rcnds[step]);
		if(digits_used < rlen)
			rcnds[step][i-3] = rcnd[digits_used++];
		if(digits_used < rlen)
			rcnds[step][i-2] = rcnd[digits_used++];
		if(digits_used < rlen)
			rcnds[step][i-1] = rcnd[digits_used++];


		/* Create the next major partial */ 

		p = multiply(result,result,NULL);

		if(!p){
			fprintf(stderr,"main: squaring step %d failed\n",
					step);
			exit(1);
		}
		q = times_digit(p,'3');
		free(p);
		if(!q){
			fprintf(stderr,"main: Tripling step %d failed\n",
					step);
			exit(1);
		}
		p = shift_left(q);
		free(q);
		Mpartials[step] = shift_left(p);
		free(p);

		/* Create the next minor factor */
		p = times_digit(result,'3');
		if(!p){
			fprintf(stderr,"main: tripling step %d failed\n",
					step);
			exit(1);
		}
		mfactors[step] = shift_left(p);
		free(p);

		/* Find the next digit, and create the next subtrahend */
		c = estimate(rcnds[step],Mpartials[step],mfactors[step],&p,&q);
		l = strlen(mfactors[step]);
		mfactors[step][l-1]=c;
		mpartials[step] = p;
		partials[step] = q;
		shends[step] = times_digit(partials[step],c);
		result[step]=c;
		step++;
		result[step]=0; /* properly terminate */

		/* quit, if answer has "come out even" */
		if((strcmp(rcnds[step-1],"000")==0)&&(digits_used >= rlen)) break;
	}

	/* change ndigits to the actual number computed, in case answer
	 * came out even. */

	ndigits = step;

	/* Print stuff out. This is the hard part! */

	printf("\n");

	/* Figure out the space taken up by the longest partial */
	if(ndigits > 1)
		base_indent = strlen(Mpartials[ndigits-1]);
	else base_indent = 0;

	indent(base_indent+BASE_SKIP);

	/* Print result. Keep track of length in k */
	if(negative){
		printf("-");
		k = 1;
	}
	else	k = 0;
	if(clen <= 0){
		printf("0.");
		k+=2;
		for(i = 0; i < leading_zeros/3; i++){ printf("  0 "); k+=4;}
		i=0; /* needed for continuation below */
	}
	else {
		switch(parity){
			case 1:
				printf("%c",result[0]);k+=1;
				break;
			case 2:
				printf(" %c",result[0]);k+=2;
				break;
			default:
				printf("  %c",result[0]); k += 3;
				break;
		}
		i = 1;
		while(3*i < clen){
			printf("   %c",result[i]);
			k += 4;
			i++;
		}
		printf(".");
		k += 1;
	}
	if(i < ndigits){printf("  %c",result[i++]);k += 3;}
	while(i < ndigits){
		printf("   %c",result[i++]);
		k += 4;
	}
	printf("\n");

	/* print the top bar of the radical */
	indent(base_indent+BASE_SKIP);
	for(i=0;i<k;i++)printf("_");
	printf("\n");

	/* print out radicand line with rest of surd symbol */

	indent(base_indent);
	printf("\\3/");

	/* The radicand's format depends on various factors. 
	 * It will be in one of the following formats:
	 *
	 * a) d ddd ddd ... [.ddd ddd [000 ... 000]]  (parity = 1)
	 * b) dd ddd .... [.ddd ddd [000 ... 000]] (parity = 2)
	 * b) ddd ddd ... [.ddd ddd [ ?0 ... 000]]
	 * c) 0.[000 ... 000][ ddd ... ddd]           (clen <= 0) 
	 *
	 * The digits are grouped in threes for visiblity of the conceptual
	 * grouping the algorithm is based upon. The decimal point 
	 * functions as a space. 
	 */

	running_indent = BASE_SKIP ; /* distance beyond base indent to
				       	beginning of significant part of next
				        	line */
	if(negative){
		printf("-");
		running_indent++;
	}

	j = 0; /* counts number of digits of result  */
	if(clen <= 0){
		parity = (-clen) % 3; /* parity is a misnomer here, but we keep
				      it from square root code */
		printf("0.");
		running_indent += 2;
		for(i = clen; i+2 < -1; i+=3) { /* leading zeros in threes to
						right of decimal */
			printf("000 "); 
			running_indent +=4;
		}
		if(i != 0){
			switch(i){
				case -1:	
					i++;
					printf("0");
					running_indent +=1;
					break;
				case -2:
					i += 2;
					printf("00");
					running_indent += 2;
					break;
				case -3:
					i+=3;
					printf("000 ");
					running_indent += 4;
					break;
			}
		}
		if(parity==1){
			if(rlen >= 2)
			printf("%c%c ", rcnd[0],rcnd[1]);
			else printf("%c0 ",rcnd[0]);
			i += 2;
			j++;
		}
		if(parity==2){printf("%c ",rcnd[0]);j++;i++;}
	}
	else {
		switch(parity){
			case 1:
				printf("%c",rcnd[0]); 
				j++;
				i = 1;
				break;
			case 2:
				printf("%c%c",rcnd[0],rcnd[1]);
				j++;
				i=2;
				break;
			default:
				printf("%c%c%c",rcnd[0],rcnd[1],rcnd[2]);
				j++;
				i = 3;
				break;
		}
		while(i+2 < clen){
			j++;
			printf(" %c%c%c",rcnd[i],rcnd[i+1],rcnd[i+2]);
			i+=3;
		}
		printf(".");
	}
	k = strlen(rcnd);
	if(clen==0){ /* Don't need space because of decimal point */
		if(j<ndigits)
		switch(k){
			case 1:
				printf("%c00",rcnd[i]);i++;j++;
				break;
			case 2:
				printf("%c%c0",rcnd[i],rcnd[i+1]);i+=2;j++;
				break;
			case 3:
				printf("%c%c%c",rcnd[i],rcnd[i+1],rcnd[i+2]);i+=3;j++;
				break;
			default:
				break;
		}
	}
	else { /* no space in first group */ 

		if(i >= k &&(j<ndigits)){printf("000"); j++;}
		if(i+1==k &&(j<ndigits)){printf("%c00",rcnd[i]);i++;j++;}
		if(i+2==k&&(j<ndigits)){printf("%c%c0",rcnd[i],rcnd[i+1]);i+=2;j++;}
		if(i+3<=k&&(j<ndigits)){printf("%c%c%c",rcnd[i],rcnd[i+1],rcnd[i+2]);i+=3;j++;}
        }
	while(i+2 < k &&(j<ndigits)){
		printf(" %c%c%c",rcnd[i],rcnd[i+1],rcnd[i+2]);
		j++;
		i += 3;
	}
	if(i+1 == k&&(j<ndigits)) {printf(" %c00", rcnd[i]); i += 1; j++;}
	if(i+2 == k&&(j<ndigits)) {printf(" %c%c0",rcnd[i],rcnd[i+1]); i+=2; j++;}
	if(i==clen&&(j<ndigits)){ printf(" 000"); i+=2; j++;}
	while(j < ndigits){
		printf(" 000");
		j++;
	}
	printf("\n");

	/* Print the rest of the lines in groups */

	for(i=0;i<ndigits-1;i++){

		if(i){ /* to save space, don't print the estimation part
                          first time */

		/* print the Major partial */
		k = strlen(Mpartials[i]);
		indent(base_indent-k);
		printf("%s\n",Mpartials[i]);


		/* print the minor factor */
		k = strlen(mfactors[i]);
		indent(base_indent-k-1);
		printf("(%s)\n",mfactors[i]);
		/* print the underbar under last digit of mfactor */
		indent(base_indent-1);
		printf("-\n");  /* hyphen character */

		/* print the minor partial */
		k = strlen(mpartials[i]);
		indent(base_indent-k);
		printf("%s\n",mpartials[i]);

		/* print the underbars under minor partial */
		indent(base_indent-k);
		for(j=0;j<k;j++)printf("-");
		printf("\n");
		} /* end skipping i = 0 */

		/* print the total partial */
		k = strlen(partials[i]);
		indent(base_indent-k);
		printf("%s",partials[i]);
 
		/* print the subtrahend */
		k = running_indent;
		indent(k + print_len(rcnds[i]) - print_len(shends[i]));
		print_triple(shends[i]);
		printf("\n");

		/* print the underbar under subtrahend */

		indent(base_indent+k);
		for(j=0;j<print_len(rcnds[i]);j++)printf("_"); /* Underscore character */
		printf("\n");

		/* print the next radicand */

		/* Careful. The next radicand is 4 characters longer than
                   you expect because of the zeros brought down */
		l = print_len(rcnds[i]) - print_len(rcnds[i+1]) + 4;
		if(l > 0) running_indent += l;
		indent(base_indent+running_indent);
		print_triple(rcnds[i+1]);
		printf("\n");
	}
	printf("\n\n");
	return 0;
}	

/* estimate: return the character corresponding to the largest digit d
 * such that d*(Mpartial+d*(mfactor + d)) <= rcnd. 
 *
 * Return d*(mfactor+d) through mpartial pointer, and Mpartial + d*(mfactor
 * + d) through tpartial pointer.
 * This routine does no sanity testing.
 * If you give it bad data, you're toast.  */

static char char_digits[] = "987654321";

char estimate(char *rcnd, char *Mpartial, char *mfactor, char **mpartial,
		char **tpartial)
{

	char *b,*s,c;
	int n,i=0;

	/* Allocate large enough working buffer. */

	n = strlen(Mpartial);
	b = (char *)malloc((n+1)*sizeof(char));

	if(!b){
		fprintf(stderr,"estimate: cannot allocate memory.\n");
		exit(1);
	}	

	strcpy(b,mfactor);
	n = strlen(b);

	/* There is no need for efficiency. We just use brute force, trying
	 * each digit in turn, from 9 down to 1. */

	while(i < 9){
		c = char_digits[i];
		b[n-1]=c;  /* b = mfactor + c */
		*mpartial = times_digit(b,c);  /* s = (mfactor+c)*c */
		*tpartial = add(Mpartial,*mpartial);
		s = times_digit(*tpartial,c);
		if(isgte(rcnd,s)==TRUE)break;
		free(s);
		free(*mpartial);
		free(*tpartial);
		i++;
	}
	if(i==9){
		c = '0';
		s = times_digit(mfactor,'0');
		*mpartial=s;
		s = (char *)strdup(Mpartial);
		*tpartial = s;
	}
	else free(s);  /* exited loop before free */
	return c;	
}

void indent(int n)
{
	int i;
	for(i=0;i<n;i++)printf(" ");
	return;
}

/* print_len: return the number of characters that will result from doing
   print_triple (see below) on the string s */

int print_len(char *s)
{
	int n;
	
	n = strlen(s);
	switch(n%3){
		case 1:
			return 1 + 4*(n-1)/3;
			break;
		case 2:
			return 2 + 4*(n-2)/3;
			break;
		default:
			return 3 + 4*(n-3)/3;
			break;
	}
}


/* print_triple: The string s represents a decimal integer dd...d. Print it
   in the format d ddd ddd ... ddd if the number of digits is congruent to
   1 mod 3, dd ddd ddd ... ddd if congruent to 2 mod 3,  and 
   ddd ddd ddd ... ddd if the number of digits is divisible by 3.
   */

void print_triple(char *s){

	int n, i = 0;

	n = strlen(s);

	switch(n%3){
		case 1:
			printf("%c",s[i++]);
			break;
		case 2:
			printf("%c%c",s[i],s[i+1]); 
			i += 2;
			break;
		default:
	 		printf("%c%c%c",s[i],s[i+1],s[i+2]); 
			i+=3; 
	}
	while(i+2 < n){
		printf(" %c%c%c",s[i],s[i+1],s[i+2]);
		i += 3;
	}
	return;
}
