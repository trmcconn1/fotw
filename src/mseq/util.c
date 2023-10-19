/* util.c: subroutines that can be included in the main program of
           mseq.c in order to generate various kinds of reports about the
           m-sequence. Usage information is documented before each routine. 

	   Routines added to this file should also be declared extern in
           mseq.h so they will be visible in mseq.c.
*/

#include "mseq.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Does what it says: returns 2^n */

unsigned long powerof2(int n)
{
	int i = 0;
	unsigned long ans = 1L;

	while(i++ < n) ans *= 2L;
	return ans;
}

/* Print terms of the m-sequence from the starting index value to the
   ending one inclusive. This puts a newline after every 80th term.
*/

int print_mseq(unsigned long start, unsigned long end){

		unsigned long i;

		if(end > max_n)
			fatal("print_mseq: time %lu too large\n",end);

		for(i=start;i<= end;i++){
			if(!(i%80))printf("\n");
			printf("%c",mseq[i]);
		}
		return 0;
}

/* Print the strings of a given length in the order they match */

unsigned long print_match_strings(int length)
{
	unsigned long k = 0L;
	unsigned long count = 0L, n;

	/* How many strings are we looking for? */

	n = powerof2(length) + 1; /* +1 because initial string matches twice */

	/* skip over nodes that have too short match lengths */

	while(node_at_time[k]->m < length){
		if(k >= max_n-1)
			fatal("print_match_strings: time %lu too large",k);
		k++;
	}

	while(count < n){
		if(k >= max_n-1)
			fatal("print_match_strings: time %lu too large",k);
		if(node_at_time[k]->m == length){
			printf("%s match time  = %lu\n",node_name(node_at_time[k]),k);
			count++;
		}
		k++;
	}
	return k;
}

	

/* Report on proportion of sequences of the form sx s.th 
(a) both 0s and 1s match before sx,
(b) one of them does,
(c) neither of them does. */

int match_order_report(int len)
{
	unsigned long n_both = 0L, n_one = 0L, n_neither = 0L;
	unsigned long n,l,j,i=0L,k=0L;
	char buf[MAX_LEVEL+2];
	struct node_info *a_node;

	n = powerof2(len) + 1L;

	/* Advance counter to first match sequence of length len */

	while(node_at_time[k]->m < len){
		if(k >= max_n-1)
			fatal("print_match_strings: time %lu too large",k);
		k++;
	}

	/* Loop over match sequences of this length */

	while(i < n){

		if(k >= max_n-1)
			fatal("print_match_strings: time %lu too large",k);

		/* Do we really have a match of length len ? */
		if(node_at_time[k]->m == len){

		/* Stash s in buffer leaving room for 0 or 1 prefix */
			strcpy(buf+1,node_name(node_at_time[k]->parent));

		/* Record match times of 0s and 1s */

			buf[0] = '1';
			if(!(a_node = find_node(buf)))
				fatal("match_order_report: Cannot find node for %s\n",buf);
			j = a_node->n;
			buf[0] = '0';
			if(!(a_node = find_node(buf)))
				fatal("match_order_report: Cannot find node for %s\n",buf);
			l = a_node->n;

		/* Increment appropriate counter */

			if((j<k)&&(l<k)) n_both++;
			else
				if((j<k)||(l<k)) n_one++;
				else n_neither++;
			i++;
		}
		k++;
	}
	/* Print stuff out */
	printf("\n\nFor strings of the form sx, length = %d:\n\n",len);
	printf("Both 0s and 1s match before sx: %lu/%lu\n",n_both,n);
	printf("One of 0s and 1s match before sx: %lu/%lu\n",n_one,n);
	printf("Neither 0s and 1s match before sx: %lu/%lu\n\n",n_neither,n);
}

/* match_back_report: reports the fraction of sequences of given length n
                      which match back to a position in the sequence at which
                      the match length has each possible value */

int match_back_report(int len)
{
	unsigned long l,j,k=0L;
	unsigned long counts[MAX_LEVEL+2];

	/* initialize counts */
	for(j=0L;j<MAX_LEVEL+2;j++)counts[j] = 0L;

	/* Advance counter to first match sequence of length len */

	while(node_at_time[k]->m < len){
		if(k >= max_n-1)
			fatal("print_match_strings: time %lu too large",k);
		k++;
	}

	while(k < max_n-1){

		/* Do we have a match of length len ? */
		if(node_at_time[k]->m == len){
			/* is it a first match ? */
			if(node_at_time[k]->n2 == 0L){ 
				l = (node_at_time[k]->matches)->m;
				counts[l]++;
			}
			
		}
		k++;
	}

	/* print stuff out */
	printf("\nn: count of times strings of length %d match at places where m = n\n",len);
	printf("------------------------------------------------------------\n\n");
	for(j=0;j<MAX_LEVEL+2;j++)
		printf("%d: %lu\n",j,counts[j]);
	return 0;
}

/* equi_distrib_report: counts the proportion of time each sequence of given
length occurs in the m_sequence */

int equi_distrib_report(int len, unsigned long n)
{
	unsigned long *count[MAX_NODES];
	int i,j,k;
	unsigned long bins;
	double chi_square=0.0, bin_mu;

	/* MAX_LEVEL is a reasonable sanity cutoff for len */
	if(len > MAX_LEVEL)
		fatal("equi_distrib_report: length parameter %d too big (%d max)\n", len,MAX_LEVEL);
	if(n > max_n-1)
		fatal("equi_distrib_report: n parameter %lu too big (%lu max)\n",n,max_n);
	bins = powerof2(len);

	/* Generate one counter for each sequence of length len */
	for(i=0;i<bins;i++){
		count[i]=malloc(sizeof(unsigned long));
		*(count[i]) = 0L;
	}
	for(i=len-1;i<n;i++){

		/* Generate the integer whose binary expansion is
                   mseq[i-len+1]...mseq[i] */

		k = (mseq[i]=='1' ? 1 : 0);
		for(j=i-1;j>i-len;j--)
			if(mseq[j]=='1')
				k = 2*k + 1;
			else 
				k = 2*k;

		/* increment kth counter */

		(*count[k])++;
	}

	/* Print stuff out */
	printf("\nDistribution of sequences of length %d in first %lu terms.\n\n",
		len,n);
	printf("Seq:\tcount/total ( percent )\n");
	printf("------------------------------------\n");

	for(k=0;k<bins;k++){
		i = k;
		for(j=0;j<len;j++){
			if(i%2)printf("1");
			else
				printf("0");
			i = i/2;
		}
		printf(":\t%lu/%lu( %g )\n",*count[k],n,(100.0*(double)*count[k])/((double)n));
	}
	printf("\nFor equidistribution expect %g percent\n\n",
		100.0/((double)bins));

	/* Calculate Chi-Square statistic */
	bin_mu = ((double)n)/((double)bins);
	for(i=0;i<bins;i++) 
		chi_square += 
		 ((double)*count[i] - bin_mu)*((double)*count[i] - 
			bin_mu)/bin_mu;
	printf("Chi Square = %g\n\n",chi_square);	

	/* clean up */
	for(i=0;i<bins;i++)
		free(count[i]);
	return 0;
}

/* excursions: print out proportion of time between given limits spent in
      excursions of various depths, and counts of excursions from each
      of the depths. 

      N.B.: if an excursion begins at index start it will be missed.

      What are excursions? See Laws of Large Numbers ... for an explanation.
*/

#define MAX_EXC_DEPTH 5

int excursions(unsigned long start, unsigned long end)
{
	unsigned long i,j;
	unsigned long time_counts[MAX_EXC_DEPTH];
	unsigned long exc_counts[MAX_EXC_DEPTH];
	unsigned long prev_depth;

	/* sanity checks */
	if(start > end)
		fatal("excursions: called with end smaller than start\n");
	if(end > max_n)
		fatal("excursions: end %lu too big ( %lu max )\n",end,max_n);

	/* initialize counters */
	for(i=0;i<MAX_EXC_DEPTH;i++){
		time_counts[i] = 0L;
		exc_counts[i] = 0L;
	}

	prev_depth = node_at_time[start]->depth;

	/* count using depth field of node_info struct */
	for(i=start;i<=end;i++){
	
		/* keep track of time at each excursion level */
		time_counts[node_at_time[i]->depth]++;	

		/* count beginning of new excursion of each type whenever
			depth goes up */
		if(node_at_time[i]->depth > prev_depth)
			for(j=prev_depth;j<node_at_time[i]->depth;j++)
				exc_counts[j]++;

		/* save depth for next round */
		prev_depth = node_at_time[i]->depth;
		
	}

	/* Print stuff out */
	printf("\nDistribution of times at excursion depths for %lu terms (%lu to %lu).\n\n",
		end-start+1L,start,end);
	printf("depth:\tcount/total ( percent )\n");
	printf("------------------------------------\n");
	for(i=0;i<MAX_EXC_DEPTH;i++)
		printf("%d:\t%lu/%lu ( %g )\n",i,time_counts[i],end-start+1L,
			(100.0*(double)time_counts[i]/(double)(end-start+1L)));
	printf("\n");
	printf("\nNumber of excursions from each depth for same range.\n\n");
	printf("depth:\tcount\n");
	printf("---------------------------\n");
	for(i=0;i<MAX_EXC_DEPTH;i++)
		printf("%d:\t%lu\n",i,exc_counts[i]);
	printf("\n");
	return 0;
}

/* init_match_times(void): list times (up to max_n) at which the match length
   reaches a new record value. Recall that an initial string is always matched
   at such times */

int
init_match_times(void){

	unsigned long i;
	
	printf("Second match times before time %lu (sequence, match length, time)\n\n",max_n+1);
	for(i=0L;i<max_n-1L;i++)
		if((node_at_time[i])->n2 == i)
			printf("%s: m=%d, time = %lu\n",
				node_name(node_at_time[i]), node_at_time[i]->m,
				i+1L);
				
	return 0;
}

/* compression: the compression numbers c_n are defined as follows: Fix n
and let N be the count of the number of times k during n,....2^n+n-1 that the
sequence of length n ending at time k does not occur previously. Equivalently,
it is the count of the times the match length < n. The compression is
defined as  (N/2^n). For example, if the compression is 1 then all sequences
of length n are packed into 2^n terms of the overall sequence as efficiently
as possible.

This routine returns the compression number or -1.0 in case of an error.

*/

double
compression(int n)
{
	unsigned long terms_req,i;
	unsigned long count = 0L;

	/* Make sure the request is reasonable */

	if(n <= 0){
		warn("compression: argument must be at least 1\n");
		return -1.0;
	}
	terms_req = powerof2(n)+n-1;
	if(terms_req >= max_n ){
		warn("%lu terms needed but only %lu available\n",terms_req,
			max_n);
		return -1.0;
	}

	for(i=n-1;i<terms_req;i++)
		if((node_at_time[i])->m < n)count++;

	return  ((double)count)/((double)powerof2(n));
}
