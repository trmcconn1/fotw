/* longsqrt.c: program to print out examples of the by-hand square root
*  algorithm.
*
* By Terry R. McConnell 12/2003
*
* Usage: longsqrt <radicand> <digits>
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
#define PROGNAME "longsqrt"

/*   Algorithm and terminology:

            1. 4  1
           ___________________________
          /
       \/   2.00 00   <------------- radicand
     1       
     _      1
            -------
	    1 00      <------------- next radicand
    24        96      <------------- subtrahend      
     -      ____
	       4 00
   281         2 81
     -  
            -------
    ^
    |
 Partials

We shall term the numbers written down the right margin the partials. Each
one is obtained by multiplying the current approximate answer (w/o decimal
point) by 2 and leaving a space (shown by the underscore) for the next digit
of the answer. The completed partial (i.e., with the underscore filled in
with the new digit) is then multiplied by the new digit to form the next
subtrahend (written directly to the right of the partial.) The new digit is
chosen as large as possible such that the subtrahend is < the current radicand. 

In the program, the partial has a zero digit in place of the underscore to
facilitate adding on the new digit.

Why does this work? Let r denote the current approximation to the square
root of the radicand, x. Then (10r + d)^2 = 100r^2 + 20r*d + d^2. Let y
denote  floor(x multiplied by the appropriate power of ten to
move the decimal right 2*(number of steps so far)). Then
 y - 100r^2 represents the next radicand. ( 20r + ? ) is then the next partial 
and (20r + d)*d is the next subtrahend. Here d is the new digit.

*/

#define BASE_SKIP 3 /* Fixed space skip that accounts for the size of
		       the \/ part of the radical sign */

#ifdef DEBUG 
char test_string[] = "280";  
#endif

char estimate(char *,char *);
void indent(int);
int print_len(char *);
void print_double(char *);

int longsqrt( char *radicand, char *ndigits_str)
{
	int i=1,j,k,l;
	int digits_used = 0;
	int leading_zeros = 0;
	int parity;
	int running_indent=0;
	int base_indent = 0;
	int step = 0;
	char *rcnds[ARG_MAX];   /* Successive radicands */
	char *shends[ARG_MAX]; /* Successive subtrahends */
	char *partials[ARG_MAX]; /* Successive partials */
	char *result;    /* Successive digits in answer */
	char *rcnd;  /* Initial radicand */
	char c;
	int ndigits;
	int rlen,clen;
	char *p,*q; /* utility pointers */

	/* Analyze radicand argument for sanity, normalize, and store. */
	/* This is surprisingly tricky to get right. */

	rlen = strlen(radicand);

	/* Garbage anywhere in arg ? */
	if(strspn(radicand,".0123456789") < rlen){
		fprintf(stderr,"%s: Radicand %s is negative or not in proper decimal form.\n",
                            PROGNAME,radicand);
		exit(1);
	}

	clen = strspn(radicand,DIGITS);

	/* If clen < rlen then our arg apparently has a decimal point.
           Make sure this is the case. */

	if(clen < rlen) {
                /* Second test handles multiple decimal point(!) */
		if((radicand[clen] != '.') ||
                    ( strspn(radicand+clen+1,DIGITS)<rlen-clen-1) ){
			fprintf(stderr,"%s: Radicand %s is not in proper decimal form\n", PROGNAME, radicand);
			exit(1);
		}
	}

	/* Throw away any leading zeros */
	
	if(strspn(radicand,"0")){
		p = strpbrk(radicand,".123456789");
		if(!p){
			printf("The square root of 0 is 0.\n");
			exit(0);
		}
		rlen -= (p-radicand);
		clen -= (p-radicand);
	}
	else p = radicand;
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
			printf("The square root of 0 is 0.\n");
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
		printf("The square root of 0 is 0\n");
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
	 * odd, or the first 2 digits, if clen is even */

	rcnds[0] = (char *)malloc(3*sizeof(char));
	if(!rcnds[0]){
		fprintf(stderr,"main: can't allocate memory for rcnds[0]\n");
		exit(1);
	}
	if(clen >= 0)
		parity = clen % 2;
	else 
		parity = (-clen)%2;
	if(parity) { /* odd */
		rcnds[0][0] = rcnd[0];
		rcnds[0][1]=0;
		digits_used = 1;
	}
	else {  /* even */
		rcnds[0][0] = rcnd[0];
		if(rlen >= 2)
			rcnds[0][1] = rcnd[1];
		else rcnds[0][1] = '0';
		rcnds[0][2] = 0;
		digits_used = 2;
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
	while(j*j <= i)j++;
	result[0] = '0'+j-1;
	result[1]=0; /* properly terminate */
	/* set up zero-th subtrahend */
	shends[0] = (char *)malloc(3*sizeof(char));
	if(!shends[0]){
		fprintf(stderr,"main: cannot allocate memory for shends[0]\n");
		exit(1);
	}
	sprintf(shends[0],"%d",(j-1)*(j-1));
	partials[0] = (char *)strdup(result);

	/* Main loop of the algorithm: */
	step = 1;
	while(step < ndigits){

		/* Subtract the current subtrahend and bring down the
		 * next two digits of rcnd, or zeros if we are past the
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
		/* strip leading zeros from new radicand (sbc does not do 
		 * this.) */
		free(q);
		q = strpbrk(p,NONZERO_DIGITS);
		if(q) 
			rcnds[step] = strdup(q);
		else 
			rcnds[step] = strdup("00");
		free(p);
		i = strlen(rcnds[step]);
		if(digits_used < rlen)
			rcnds[step][i-2] = rcnd[digits_used++];
		if(digits_used < rlen)
			rcnds[step][i-1] = rcnd[digits_used++];

		/* Create the next partial */ 

		p = times_digit(result,'2');
		if(!p){
			fprintf(stderr,"main: doubling step %d failed\n",
					step);
			exit(1);
		}
		partials[step] = shift_left(p);
		free(p);


		/* Find the next digit, and create the next subtrahend */

		c = estimate(rcnds[step],partials[step]);
		i = strlen(partials[step]);
		partials[step][i-1]=c; /* add c to partial */
		shends[step] = times_digit(partials[step],c);
		result[step]=c;
		step++;
		result[step]=0; /* properly terminate */
		/* quit, if answer has "come out even" */
		if((strcmp(rcnds[step-1],"00")==0)&&(digits_used >=rlen)) break;
	}

	/* change ndigits to the actual number computed, in case answer
	 * came out even. */

	ndigits = step;

	/* Print stuff out. This is the hard part. */

	printf("\n");

	/* Figure out the space taken up by the longest partial */
	base_indent = strlen(partials[ndigits-1]);
	indent(base_indent+BASE_SKIP);

	/* Print result. Keep track of length in k */
	k = 0;
	if(clen <= 0){
		printf("0.");
		k+=2;
		for(i = 0; i < leading_zeros/2; i++){ printf(" 0 "); k+=3;}
		i=0; /* needed for continuation below */
	}
	else {
		if(parity) {printf("%c",result[0]);k+=1;}
		else { printf(" %c",result[0]); k += 2;}
		i = 1;
		while(2*i < clen){
			printf("  %c",result[i]);
			k += 3;
			i++;
		}
		printf(".");
		k += 1;
	}
	if(i < ndigits){printf(" %c",result[i++]);k += 2;}
	while(i < ndigits){
		printf("  %c",result[i++]);
		k += 3;
	}
	printf("\n");

	/* print the top bar of the radical */
	indent(base_indent+BASE_SKIP);
	for(i=0;i<k;i++)printf("_");
	printf("\n");

	/* print out radicand line with rest of surd symbol */

	indent(base_indent);
	printf(" \\/");

	/* Properly displaying the radicand is tricky. 
	 * It will be in one of the following formats:
	 *
	 * a) d dd dd ... [.dd dd [ 00 ... 00]]  (parity = 1)
	 * b) dd dd ... [.dd dd [ ?0 ... 00]]
	 * c) 0.[00 ... 00][ dd ... dd]           (clen <= 0) 
	 *
	 * The digits are grouped in pairs for visiblity of the conceptual
	 * grouping the algorithm is based upon. The decimal point 
	 * also functions as a space. 
	 */

	running_indent = BASE_SKIP ; /* distance beyond base indent to
				       	beginning of significant part of next
				        	line */
	j = 0; /* counts number of result digits we've accounted for */
	if(clen <= 0){
		if(clen % 2)parity = 1;
		printf("0.");
		running_indent += 2;
		for(i = clen; i+1 < 0; i+=2) { /* leading zeros in pairs to
						right of decimal */
			printf("00 "); 
			running_indent +=3;
		}
		if(i == -1){
			i++;
			printf("0");
			running_indent +=1;
		}
		/* i counts number of radicand digits used */
		if(parity){printf("%c ", rcnd[0]);j++; i++;}
	}
	else {
		if(parity){
			printf("%c",rcnd[0]);
			j++;
			i = 1;
		}
		else  {
			printf("%c%c",rcnd[0],rcnd[1]);
			j++;
			i = 2;
		}
		while(i+1 < clen){
			j++;
			printf(" %c%c",rcnd[i],rcnd[i+1]);
			i+=2;
		}
		printf(".");
	}
	k = strlen(rcnd);
	if((i==k)&&(j<ndigits)){ printf("00"); i++; j++;}
	if((i+1==k)&&(j<ndigits)){printf("%c0",rcnd[i]);i+=2;j++;}
	if((i+1 < k)&&(j<ndigits)){
		printf("%c%c",rcnd[i],rcnd[i+1]);
		j++;
		i+=2;
	}
	while((i+1 < k)&&(j<ndigits)){
		printf(" %c%c",rcnd[i],rcnd[i+1]);
		j++;
		i += 2;
	}
	if((i+1 == k)&&(j<ndigits)) {printf(" %c0", rcnd[i]); i += 2; j++;}
	while(j < ndigits){
		printf(" 00");
		j++;
	}
	printf("\n");

	/* Print the rest of the lines in groups */

	for(i=0;i<ndigits-1;i++){

		/* print the partial */

		k = strlen(partials[i]);
		indent(base_indent-k);
		printf("%s",partials[i]);

		/* print the subtrahend */
		k = running_indent; /* save for printing underbar later */
		running_indent += print_len(rcnds[i]) - print_len(shends[i]);
		indent(running_indent);
		print_double(shends[i]);
		printf("\n");

		/* print the underbars */

			/* print the partial underbar */

		indent(base_indent-1);
		printf("-");  /* hyphen character */
		 
			/* print the underbar under subtrahend */

		indent(k);
		for(j=0;j<print_len(rcnds[i]);j++)printf("_"); /* Underscore character */
		printf("\n");

		/* print the next radicand */

		/* Careful. The next radicand is 3 characters longer than
                   you expect because of the zeros brought down */
		l = print_len(shends[i]) - print_len(rcnds[i+1]) + 3;
		if(l > 0) running_indent += l;
		indent(base_indent+running_indent);
		print_double(rcnds[i+1]);
		printf("\n");
	}

	printf("\n\n");
	return 0;
}	

/* estimate: return the character corresponding to the largest digit d
 * such that d*(partial+d) <= rcnd. This routine does no sanity testing.
 * If you give it bad data, you're toast.  */

static char char_digits[] = "987654321";

char estimate(char *rcnd, char *partial)
{

	char *b,*s,c;
	int n,i=0;

	/* Allocate large enough working buffer. */

	n = strlen(partial);
	b = (char *)malloc((n+1)*sizeof(char));

	if(!b){
		fprintf(stderr,"estimate: cannot allocate memory.\n");
		exit(1);
	}	

	strcpy(b,partial);

	/* There is no need for efficiency. We just use brute force, trying
	 * each digit in turn, from 9 down to 1. */

	while(i < 9){
		c = char_digits[i];
		b[n-1]=c;  /* b = partial + c */
		s = times_digit(b,c);  /* s = (partial+c)*c */
		if(isgte(rcnd,s)==TRUE)break;
		free(s);
		i++;
	}
	if(i==9)
		c = '0';
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
   print_double (see below) on the string s */

int print_len(char *s)
{
	int n;
	
	n = strlen(s);
	if(n%2) return 1 + 3*(n-1)/2;
	else return 2 + 3*(n-2)/2;
}


/* print_double: The string s represents a decimal integer dd...d. Print it
   in the format d dd dd ... dd if the number of digits is odd, and 
   dd dd dd ... dd if the number of digits is even.
   */

void print_double(char *s){

	int n, i = 0;

	n = strlen(s);

	if(n%2)printf("%c",s[i++]);
	else { printf("%c%c",s[i],s[i+1]); i+=2; }
	while(i+1 < n){
		printf(" %c%c",s[i],s[i+1]);
		i += 2;
	}
	return;
}
