/* trees.c: see how different the density of trees as seen by trees can brom
density as seen from a randomly selected point. Motivated by question of Ruth
Yanai */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_TREES_PER_PLOT 100
#define PLOTS 64

struct plot {

	int n; /* number of trees in plot */
	double off[MAX_TREES_PER_PLOT]; /* offset of each tree from start of its
 					plot */
	int x0; /* start of this plot. Plots are intervals of unit length on 	                   the x axis */
};

struct plot plots[PLOTS];

int main(int argc, char **argv){

	int i,j,k,m,n,l,nn;
	double f,c;
	double a,b;

	srandom(110);

	/* initialize plots by scattering an exponential number of trees
           uniformly in each plot, independent of the others */

	for(i=0;i<PLOTS;i++){

		plots[i].x0 = i;
		f = ((double)random())/((double)RAND_MAX);
		c =  -10.0*log(1.0 - f);
		k = (int)c;
		if(k > MAX_TREES_PER_PLOT)k = MAX_TREES_PER_PLOT;
		plots[i].n = k;
		for(j=0;j<k;j++)
			(plots[i]).off[j] = ((double)random())/((double)RAND_MAX);
	}
	
	/* run through array and report the density of trees in each plot and
           their overall density */

	c = 0.0;
	for(i=0;i<PLOTS;i++){
		printf("Plot %d: %d to %d + 1: ",i,plots[i].x0,plots[i].x0);
		printf("%g\n", (double)plots[i].n);
		c += (double)plots[i].n;
	}
	printf("Average density = %g\n",c/((double)PLOTS));

	/* total trees: */
	nn = (int)c;

	/* Now select 64 trees at random and tally the count of trees within
	   distance 1/2 of the selected tree in either direction */

	c = 0.0;
	for(i=0;i<PLOTS;i++){

		k = (int)(((double)random()/(double)RAND_MAX)*(double)nn);
		j = 0;
		for(m=0;m<PLOTS;m++){
			if(plots[m].n + j > k)break;
			j += plots[m].n;
		}
		if(m == PLOTS)m--; /* should never happen */
		l =  plots[m].n + j - k ;  

		/* selected lth tree in mth plot */

		f = (double)plots[m].x0 + plots[m].off[l];
		a = f  - 0.5;
		b = a + 1.0;

		/* now count trees between a and b */
		l = 0;
		if(m > 0)
			for(n=0;n < plots[m-1].n;n++)
				if(plots[m-1].off[n] + ((double)plots[m-1].x0) > a)l++;
		if(m < PLOTS)
			for(n=0;n < plots[m].n;n++)
				if(plots[m].off[n] + ((double)plots[m].x0) < b)l++;

		printf("Tree nghd %d: %g to %g: ",i,a,b);
		printf("%d trees\n", l);
		c += (double)l;
	}
	printf("Average density = %g\n",c/((double)PLOTS));
	return 0;

}
