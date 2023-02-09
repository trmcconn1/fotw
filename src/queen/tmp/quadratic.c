/* linear.c: Solve quadratic congruences and related things 
 *
 *  By Terry R. McConnell
 *
*/             


/* compile: cc -o quadratic quadratic.c zp.c

      Use -D_SHORT_STRINGS if your compiler does not support multiline
          string constants.

   Run quadratic -h for usage information.

*/


#include "global.h"
#include "decl.h"
#include "latex.h"
#include<stdio.h>
#include<stdlib.h>

#define VERSION "1.0"
#define USAGE "quadratic [ -l -L -t -V -h -v -- ] [a [ b c ]] p"
#ifndef _SHORT_STRINGS
#define HELP "\nquadratic [ -l -L -t -V -h -v --] a [ b c ] p\n\n\
Solve the quadratic congruence ax^2 + bx + c  = 0 (mod p) (p prime)\n\n\
-l: Find the Legendre symbol of a mod p (no b or c arguments)\n\
-L: List all quadratic residues modulo p (p argument only)\n\
-V: Produce verbose ascii output\n\
-t: Produce verbose latex output\n\
--: Signal end of options so that negative a and or b can be input.\n\
-v: Print version number and exit. \n\
-h: Print this helpful information. \n\n"
#else
#define HELP USAGE
#endif


#ifdef MAIN
int
main(integer argc, char **argv)
{
	integer a=0,b=0,c=0,p,h,s,d,x,y,z;
	int nargs = 4;
	integer j=0,k;
	int verbose = 0;
	int tex = 0;
	int legendre_only = 0;
	int residues_only = 0;
	char bop[2] = "+";
	char cop[2] = "+"; 

	/* Process command line */
	while(++j < argc){
		if(argv[j][0] == '-')
			switch(argv[j][1]){ 
				case '-':
					++j;
					break;
				case 'v':
					printf("%s\n",VERSION);
					exit(0);
				case 'V':
					verbose = 1;
					continue;
				case 'l':
					nargs =  2;
					legendre_only = 1;
					continue;
				case 'L':
					nargs = 1;
					residues_only = 1;
					continue;
				case 't':
					verbose = 1;
					tex = 1;
					continue;
				case '?':
				case 'h':
				case 'H':
					printf("%s\n",HELP);
					exit(0);
				default:
					fprintf(stderr,"quadratic: unkown option %s\n",
						argv[j]);
					fprintf(stderr,"%s\n",USAGE);
					exit(1);
			}
		break;
	}

	if(j + nargs != argc){
		fprintf(stderr,"quadratic: usage error.\n");
		fprintf(stderr,"%s\n",USAGE);
		return 1;
	}
	if(nargs == 1)
		p = atoi(argv[j++]);
	else if(nargs == 2){
		a = atoi(argv[j++]);
		p = atoi(argv[j++]); 
	}
	else {
		a = atoi(argv[j++]);
		if(a == 0){
			fprintf(stderr,"quadratic: equation is linear. Use linear program to solve.\n");
			exit(1);
		}
		b = atoi(argv[j++]);
		c = atoi(argv[j++]);
		p = atoi(argv[j++]);
	}
	
	if(b<0) bop[0]='\0';  /* for nicer printing */
	if(c<0) cop[0]='\0';

	if(p==0){
		fprintf(stderr,"quadratic: modulus must be nonzero.\n");
		return 1;
	}
	if(p < 0) p = -p;

	if(tex)printf(LATEX_HEADER);
	if(legendre_only) {
		if(verbose){
			if(tex)printf("$\\genfrac(){}{1}{%ld}{%ld} = %d$\n",
a,p,legendre_symbol(a,p));
			else printf("(%ld/%ld) = %d\n",a,p,legendre_symbol(a,p));
		}
		else printf("%d\n", legendre_symbol(a,p));
	}
	if(residues_only){
		for(a = 1;a<p;a++)
			if(legendre_symbol(a,p)==1)printf("%d ",a);
		printf("\n");
	}

	if((legendre_only == 0) && (residues_only == 0)){

	/* We are solving ax^2 + bx + c = 0 (mod p) */

	/* Begin by computing discriminant */

	if(verbose == 1)
		if( tex == 1)
			printf("The solution set of $ %ld x^2 %s %ld x %s %ld \\equiv 0 \\pmod{%ld} $ = ",a,bop,b,cop,c,p);
		else 
			printf("The solution set of %ld x^2 %s %ld x %s %ld congruent to zero modulo %ld = ", a,bop,b,cop,c,p);

	d = normalize(b*b - ((integer)4)*a*c,p);

	if(p == 2) { /* 2 is always a special case, but silly */

	}

	x = normalize(- b * inverse(2*a,p),p);

	if( d == 0) { /* Solution is just x */

		if(tex == 1) printf("\\{ %ld \\}\n");
		printf("{%ld}\n",x);
		return 0;
		
	}

	if((z=sqrtp(d,p)) == -1) { /* No solutions */

		if(tex == 1)printf( "$\\nullset$\n");
		else 
			printf("Empty Set\n");
		return 0;
	}

	z = z*inverse(2*a,p);

	if(tex == 1)
		printf("\\{%ld, %ld\\}\n", normalize(x+z,p), normalize(x-z,p));
	else
		printf("{%ld, %ld}\n", normalize(x+z,p), normalize(x-z,p)); 

	}

	if(tex)printf(LATEX_FOOTER);
	return 0;

}
#endif

