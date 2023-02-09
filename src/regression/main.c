/* regression.c: fit line or circle to given data
 
usage: regression [options] xfile yfile

where xfile and yfile should each consist of a column of numbers.
If the number of lines is not the same, the shorter file determines
the number of data points.

		By Terry McConnell

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 1024
#define OPSEP '-'
#define USAGE "regression [-t <o|c|h> -m -hv] xfile yfile"
#define HELP0 "read (x,y) pairs and fit curve. Print report"
#define HELP1 "xfile,yfile: floating point numbers, one per line"
#define HELP2 "-t change fit type from default vertical"
#define HELP3 "type o = orthogonal, h = horizontal, c = circular"
#define HELP4 "-h: print this helpful message and exit"
#define HELP5 "-v: print version number and exit"
#define HELP6 "-m: print maple input file"
#define VERSION "1.0"

/* Codes for supported types of regression */
#define VERTICAL 0
#define ORTHOGONAL 1
#define CIRCULAR 2
#define HORIZONTAL 3

#ifndef min
#define min(a,b)  ((a) > (b) ? (b) : (a))
#endif

extern int vertical(double *X, double *Y, double n, 
	double *m, double *b, double *ssr);
extern int orthogonal(double *X, double *Y, double n, 
	double *m, double *b, double *ssr);


extern double *X,*Y; /* declared in regression.c and initialized below */

int  main(int argc, char **argv)
{
	int i;
	int maple = 0;
	int regression_style = VERTICAL;
	double *p,*q;
	FILE *xfile, *yfile;
	double var, xbar, ybar, cov, m, b, ssr;
	double n = 0.0,k=0.0;
	char temp[MAXLINE];       

	/* Process Command Line */

	if(argc > 1) {
		while(argc > 1)
		  if((*++argv)[0]==OPSEP)
			switch((*argv)[1]){
				case 'h':
					printf("\n%s\n",USAGE); 
					printf("\n%s\n",HELP0);
					printf("%s\n",HELP1);
					printf("%s\n",HELP2);
					printf("%s\n",HELP3);
					printf("%s\n",HELP4);
					printf("%s\n",HELP5);
					printf("%s\n",HELP6);
					printf("\n");
					argc--;
					return 0;
					break;
				case 'v':
					printf("%s\n",VERSION);
					return 0;
					break;  
				case 'm':
					argc--;
					maple = 1;
					break;
				case 't':
					argc--;
					argv++;
					if(strcmp("o",*argv)==0){
						regression_style = ORTHOGONAL;
						argc--;
						break;
					}
					if(strcmp("c",*argv)==0){
						regression_style = CIRCULAR;
						argc--;
						break;
					}
					if(strcmp("h",*argv)==0){
						regression_style = HORIZONTAL;
						argc--;
						break;
					}
					fprintf(stderr,"%s\n",USAGE);
					fprintf(stderr,"unrecognized -t option %s\n",*argv);
					return 1;
				default:
					fprintf(stderr,"regression: Invalid flag\n");
					fprintf(stderr,"%s\n",USAGE);
					return 1;
			}
		else {
			if(!argc){
				fprintf(stderr,"regression:%s\n",USAGE);
				return 1;
			}

 /* Open data files for read */

			if((xfile = fopen( *argv, "r")) == NULL){
				printf("regression: unable to open xfile\n");
				return 1;
			}
			*argv++; 
			argc--;
			if(!argc){
				fprintf(stderr,"regression:%s\n",USAGE);
				return 1;
			}
			if((yfile = fopen( *argv, "r")) == NULL){
				fprintf(stderr,"regression: unable to open yfile\n");
				return 1;
			}
			argc--;
		}

		/* compute size of data set (n) */

		while (	fgets(temp,MAXLINE,xfile) != NULL )
			n = n+1.0;
		fseek(xfile,0L,SEEK_SET);
		while (	fgets(temp,MAXLINE,yfile) != NULL )
			k = k+1.0;
		fseek(yfile,0L,SEEK_SET);
		n = min(n, k); /* the shorter file determines n */
		if (n==0.0) {
			fprintf(stderr,"regression: empty data file\n");
			return 1;
		}

/* Allocate memory for data */

		X = (double *)malloc(((int)n)*sizeof(double));
		Y = (double *)malloc(((int)n)*sizeof(double));
		if(!X || !Y){
			fprintf(stderr,"regression: cannot alloc memory\n");
			return 1;
		}
		fseek(xfile,0L,SEEK_SET);
		fseek(yfile,0L,SEEK_SET);
		k = 0.0; p = X; q = Y;

/* Initialize X and Y data from files */

		while(k < n){
			*p++ = atof(fgets(temp,MAXLINE,xfile));
			*q++ = atof(fgets(temp,MAXLINE,yfile));
			k += 1.0;
		}
		fclose(xfile); fclose(yfile);

		/* Do computations in chosen type */

		switch(regression_style){

		case VERTICAL:  /* the default */

			if(vertical(X,Y,n,&m,&b,&ssr)){
			fprintf(stderr,"regression: there is something wrong\n");
			return 1;
			}

			printf("slope: %.3f\tintercept: %.3f\t ssr: %.3f\n",m,b,ssr);
			break;

		case HORIZONTAL:

			if(vertical(Y,X,n,&m,&b,&ssr)){
			fprintf(stderr,"regression: there is something wrong\n");
			return 1;
			}

			printf("slope: %.3f\tintercept: %.3f\t ssr: %.3f\n",-1.0/m,-b/m,ssr);
			break;

		case ORTHOGONAL:

			if(orthogonal(X,Y,n,&m,&b,&ssr)){
			fprintf(stderr,"regression: there is something wrong\n");
			return 1;
			}

			printf("slope: %.3f\tintercept: %.3f\t ssr: %.3f\n",m,b,ssr);
			break;

		default:

			fprintf(stderr,"regression: unknown style\n");
			return 1;
		}

	}
	
	else { /* empty command line */
		fprintf(stderr,"regression:%s\n",USAGE);
		return 1;
	}

/* clean up */

	free(X);
	free(Y);
	return 0;
}

