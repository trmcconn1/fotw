/* main.c: main program of polyhedron package */


#include<stdio.h>
#include<stdlib.h>
#include "poly.h"

#define VERSION "1.0"
#define USAGE "poly [ -h -v]"
#define HELP "poly [ -h -v ]\n\n\
Manipulate Finite Simplicial Complexes\n\n\
-v: Print version number and exit. \n\
-h: Print this helpful information. \n\n"

int
main(int argc, char **argv)
{
	int i=0,j=0,m;
	struct spx *spxp, *bottom, *front, *left, *right;
	struct scpx *cpxp;

	/* Process command line */
	while(++j < argc){
		if(argv[j][0] == '-')
			switch(argv[j][1]){ 
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
					fprintf(stderr,"poly: unkown option %s\n",
						argv[j]);
					exit(1);
			}
		else {
			fprintf(stderr,"%s\n",USAGE);
			exit(1);
		}
	}

#if 0
	spxp = alloc_simplex(3,"My simplex");	
	printf("The simplex is \n");
	dump_simplex(spxp);
#endif

	/* Create a sphere complex */ 
	cpxp = alloc_complex("Sphere Complex");
	bottom = alloc_simplex(2,"Bottom Face");
	front  = alloc_simplex(2,"Front Face");
	attach_simplex_to_complex(bottom,NULL,NULL,cpxp);
	attach_simplex_to_complex(front,front->facets[0],bottom->facets[0],
		cpxp);
	left = alloc_simplex(2,"Left Face");
	right = alloc_simplex(2,"Right Face");
	attach_simplex_to_complex(left,left->facets[0],bottom->facets[1],
		cpxp);
	attach_simplex_to_complex(left,left->facets[2],front->facets[1],
		cpxp);
	dump_complex(cpxp);
#if 0
	attach_simplex_to_complex(right,right->facets[0],bottom->facets[2],
		cpxp);
	attach_simplex_to_complex(right,right->facets[2],front->facets[2],
		cpxp);
	attach_simplex_to_complex(right,right->facets[1],left->facets[1],
		cpxp);
/*	collect_garbage(); */ 
	status();
#endif

#if 0
	/* attach sides to bottom */

	spxp2 = alloc_simplex(2,"Back Face");
	attach_simplex(spxp2,spxp2->facets[0],spxp->facets[0],cpxp);
	spxp2 = alloc_simplex(2,"Left Face");
	attach_simplex(spxp2,spxp2->facets[0],spxp->facets[1],cpxp);
	spxp2 = alloc_simplex(2,"Right Face");
	attach_simplex(spxp2,spxp2->facets[0],spxp->facets[2],cpxp);

#endif
	return 0;
}
	


