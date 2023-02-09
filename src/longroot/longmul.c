/* longmul.c: program to print out examples of the by-hand multiplication
*  algorithm.
*
* By Terry R. McConnell 12/2003
*
* Usage: longmul <m1> <m2>
* where m1 and m2 are positive decimal numbers
*
* Arguments can be arbitrarily long, subject only to the size of
* ARG_MAX on your system. POSIX.1 ensures that this is at least 4096 bytes.
*
*/

#include "global.h"

#define USAGE "longmul [-hv] <m1> <m2>"
#define PROGNAME "longmul"
#define HELP "\n\nlongmul [-hv] <m1> <m2>\n\n\
-h: print this helpful information and exit.\n\
-v: print version number and exit.\n\n\
Print listing of by-hand calculation of m1 x m2 (positive decimal numbers)"

/*   Algorithm and terminology:
 *
 *   The algorithm is the familiar one from arithmetic. The work is done
 *   by the multiply routine in libarbp.c. 
 *
 *          237   <- multiplicand
 *    	     28   <- multiplier
 *          ---
 *         1896   <- partials
 *         474    <-  """
 *         ----
 *         5636   <- result
 *
*/

#ifdef DEBUG 
char test_string[] = "280";  
#endif

void indent(int);

int 
main(int argc, char **argv)
{

	int i=1,j,k,l,n;
	char **partials; /* Successive partials */
	char *result;   
	char *m1,*m2;
	int m1len,m2len,c1len,c2len;
	char *mpr,*mcnd;
	char *p; /* utility pointers */

	/* Process any options */
	while(i < argc && argv[i][0]=='-'){
		switch(argv[i][1]){
			case 'v':
				printf("%s\n",VERSION);
				return 0;
			case 'h':
				printf("%s\n",HELP);
				return 0;
				break;
			default:
				fprintf(stderr,"%s:%s\n",PROGNAME,USAGE);
				fprintf(stderr,"radicand must be nonnegative.\n");
				return 1;

		}
		i++;
	}

	/* If we got here we should have two more arguments */

	if( argc - i < 2 ) {

		fprintf(stderr,"%s: Usage: %s\n\n",PROGNAME,USAGE);
		exit(1);

	}

	/* Analyze arguments for sanity, normalize, and store. */
	/* This is surprisingly tricky to get right. */

	m1len = strlen(argv[i]);

	/* Garbage anywhere in arg ? */
	if(strspn(argv[i],".0123456789") < m1len){
		fprintf(stderr,"%s: m1 %s is not in proper decimal form.\n",
                            PROGNAME,argv[i]);
		exit(1);
	}

	c1len = strspn(argv[i],DIGITS);

	/* If c1len < mlen then our arg apparently has a decimal point.
           Make sure this is the case. */

	if(c1len < m1len) {
                /* Second test handles multiple decimal point(!) */
		if((argv[i][c1len] != '.') ||
                    ( strspn(argv[i]+c1len+1,DIGITS)<m1len-c1len-1) ){
			fprintf(stderr,"%s: m1 %s is not in proper decimal form\n", PROGNAME, argv[i]);
			exit(1);
		}
	}

	/* Throw away any leading zeros */
	
	if(strspn(argv[i],"0")){
		p = strpbrk(argv[i],".123456789");
		if(!p){
			printf("The product is 0.\n");
			exit(0);
		}
		m1len -= (p-argv[i]);
		c1len -= (p-argv[i]);
	}
	else p = argv[i];
	m1 = (char *)malloc((m1len+1)*sizeof(char));
	if(!m1){
		fprintf(stderr,"%s: memory error. No space for m1.\n",PROGNAME);
		exit(1);
	}
	/* copy cleaned up arg, removing decimal point. It will be restored
           at the end using the value clen (characteristic length.) */

	for(j=0;j<c1len;j++)
		m1[j] = p[j];
	if(j < m1len){
		j++;
		while(j<m1len){
			m1[j-1] = p[j]; /* skipped in p, not m1 */
			j++;
		}
		m1[j-1] = 0;			
	}
	else m1[j] = 0;

/* Now that the decimal point is removed, we may have introduced more leading
zeros. */

	m1len = strlen(m1);
	if(strspn(m1,"0")){
		p = strpbrk(m1,".123456789");
		if(!p){
			printf("The product is 0.\n");
			exit(0);
		}
		m1len -= (p-m1);
		c1len -= (p-m1); /* This may be negative */
		for(j=0;j<m1len;j++)
			m1[j] = p[j];
		m1[j] = 0;
	}

	if(m1len == 0) { /* Our arg was 0 */
		printf("The product is 0.\n");
		exit(0);
	}

	i++; /* next arg */

	/* repeat the whole thing for second argument */

	m2len = strlen(argv[i]);

	/* Garbage anywhere in arg ? */
	if(strspn(argv[i],".0123456789") < m2len){
		fprintf(stderr,"%s: m2 %s is not in proper decimal form.\n",
                            PROGNAME,argv[i]);
		exit(1);
	}

	c2len = strspn(argv[i],DIGITS);

	/* If c2len < mlen then our arg apparently has a decimal point.
           Make sure this is the case. */

	if(c2len < m2len) {
                /* Second test handles multiple decimal point(!) */
		if((argv[i][c2len] != '.') ||
                    ( strspn(argv[i]+c2len+1,DIGITS)<m2len-c2len-1) ){
			fprintf(stderr,"%s: m2 %s is not in proper decimal form\n", PROGNAME, argv[i]);
			exit(1);
		}
	}

	/* Throw away any leading zeros */
	
	if(strspn(argv[i],"0")){
		p = strpbrk(argv[i],".123456789");
		if(!p){
			printf("The product is 0.\n");
			exit(0);
		}
		m2len -= (p-argv[i]);
		c2len -= (p-argv[i]);
	}
	else p = argv[i];
	m2 = (char *)malloc((m2len+1)*sizeof(char));
	if(!m2){
		fprintf(stderr,"%s: memory error. No space for m2.\n",PROGNAME);
		exit(1);
	}
	/* copy cleaned up arg, removing decimal point. It will be restored
           at the end using the value clen (characteristic length.) */

	for(j=0;j<c2len;j++)
		m2[j] = p[j];
	if(j < m2len){
		j++;
		while(j<m2len){
			m2[j-1] = p[j]; /* skipped in p, not m2 */
			j++;
		}
		m2[j-1] = 0;			
	}
	else m2[j] = 0;

/* Now that the decimal point is removed, we may have introduced more leading
zeros. */

	m2len = strlen(m2);
	if(strspn(m2,"0")){
		p = strpbrk(m2,".123456789");
		if(!p){
			printf("The product is 0.\n");
			exit(0);
		}
		m2len -= (p-m2);
		c2len -= (p-m2); /* This may be negative */
		for(j=0;j<m2len;j++)
			m2[j] = p[j];
		m2[j] = 0;
	}

	if(m2len == 0) { /* Our arg was 0 */
		printf("The product is 0\n");
		exit(0);
	}

	/* Ready to roll ... */

	result = multiply(m1,m2,&partials);

	/* Print stuff out. */

	printf("\n");

	/* Figure out the space taken up by the display */
	k = m1len;
	l = m2len;
	if(l<k){ 
		n = k;
		k = l;
		l = n;
		mpr = m2;
		mcnd = m1;
	}
	else {
		mpr = m1;
		mcnd = m2;
	}
	n = k+l+1;

	indent(n-l);
	printf("%s\n",mcnd);
	indent(n-k-2);
	printf("x %s\n",mpr);
	if(k < l-1)
		indent(n-l);
	else if(k == l-1)
		indent(n-l-1);
	else indent(n-l-2);
	for(i=0;i<l;i++)printf("-");
	printf("\n");
	for(i=0;i<k;i++){
		indent(n-strlen(partials[i]));
		printf("%s\n",partials[i]);
	}
	indent(1);
	for(i=1;i<n;i++)printf("-");
	printf("\n");
	indent(n-strlen(result));
	printf("%s\n\n",result);
	return 0;
}	

void indent(int n)
{
	int i;
	for(i=0;i<n;i++)printf(" ");
	return;
}
