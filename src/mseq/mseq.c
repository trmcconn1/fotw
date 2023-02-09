/* mseq.c: main program of M-sequence analyzer.
	             By Terry R. McConnell

	The M-sequence is a psuedo random sequence introduced by 
A. Ehrenfeucht and J. Mycielski in "A psuedo-random sequence -- how random
is it?", Amer. Math. Monthly 99(1992),373-375. In brief, the sequence of
binary digits starts 010. Thereafter, one determines the next digit as
follows: find the longest sequence occuring at the end which also occurs 
previously. If there is more than one previous occurrence, consider the last
one. The next digit is the opposite of the one following that last previous
occurrence. Thus, the sequence "avoids repetition." 

The first few digits are  01001101011100010000... . Little is known
about this sequence. See also the Author's unpublished manuscript, Laws of
Large Numbers for Some Non-repetitive Sequences. */


/* To explain the mechanism of this program it is necessary to review a few
facts about the m-sequence. The longest sequence ending at position n that
occurs earlier is called the match sequence at time n. It can be shown
that each finite sequence of 0s and 1s occurs exactly once as the match
sequence, except for the sequences that begin the m-sequence (0, 01, 010, 
0100, etc.) These occur as the match sequence exactly twice.  Since each
finite sequence of 0s and 1s can be associated with a node of the infinite
binary tree, we can create a tree structured database that contains information
about the match at each time n. 

The central routine of this file, build(), iteratively creates the terms of
the m-sequence. For each term it also creates a tree node (see the declaration
of node_info in mseq.h for the structure of a node) and fills in information.
*/

#include "mseq.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


#define DEFAULT_O_PARAM 5L;

struct node_info *nodes[MAX_NODES];/* Memory for the tree structured database*/
struct node_info *node_at_time[MAX_NODES]; /* Accounts for duplication in
                                              nodes */ 
char mseq[MAX_NODES+1] = START_STR;    /* Memory for the m-sequence itself. */
unsigned long max_n = MAX_NODES;

struct node_info root = {
	0L,  /* assume root filled at time 0 */
	0L,
	0,   /* match length is 0 */
	0,
	0,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
};

unsigned long build(void);

int main(int argc, char **argv)
{
	int i,j=0;
	unsigned long node_cnt;
	unsigned long o_param = DEFAULT_O_PARAM;


	/* Process command line */
	while(++j < argc){
		if(argv[j][0] == '-')
			switch(argv[j][1]){ 
				case 'n':
					if(j+1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					max_n = atol(argv[j+1]);
					if(max_n > MAX_NODES)
						fatal("%lu terms greater than max (%lu)", max_n, MAX_NODES);
					j++;
					continue;
				case 'o':
					if(j+1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					o_param = atol(argv[j+1]);
					j++;
					continue;
				case'i':
					if(j+1 >= argc) {
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					strcpy(mseq,argv[j+1]);
					j++;
					continue;
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
					fatal("Unknown option %s", argv[j]);
			}
		else {
			fatal("%s\n",USAGE);
			exit(1);
		}
	}
 
	/* Generate m sequence and build database */

	node_cnt = build();

	/* Do stuff */

/*
	printf(DEFAULT_MESSAGE);
	printf(HELP);
*/

	/* Include one of the following or something else from util.c
           in order to generate useful reports. */

 	print_mseq(0L,999L);  
/*
	print_match_strings((int)o_param); 
	match_order_report((int)o_param); 
	equi_distrib_report(1,10000);
	excursions(0L,o_param);
	init_match_times();
	match_back_report(4);

	for(i=1;i<15;i++)
		printf("c_%d = %g\n",i,compression(i));

*/
	/* clean up: NB can't free root node  */
	for(i=1;i<node_cnt;i++)free_node(nodes[i]);
	return 0;
}


/* Generic fatal error routine */

void fatal(char *fmt,...)
{
	va_list ap;
	char buffer[MAX_MSG_LEN];

	va_start(ap,fmt);
	vsprintf(buffer,fmt,ap);

	fprintf(stderr, "mseq: fatal error: %s\n", buffer);
	va_end(ap);
	exit(1);
}

/* Generic warning routine */

void warn(char *fmt,...)
{
	va_list ap;
	char buffer[MAX_MSG_LEN];

	va_start(ap,fmt);
	vsprintf(buffer,fmt,ap);

	fprintf(stderr,"mseq: warning: %s\n", buffer);
	va_end(ap);
	return;
}

/* alloc_node: allocate memory for a tree node. Return pointer to it or bomb
   with a fatal error */

struct node_info *alloc_node(void)
{
	struct node_info *a_node; 

	a_node = malloc(sizeof(struct node_info));
	if(a_node) return a_node;

	fatal("Unable to allocate memory for a tree node\n");
	return NULL;
}


/* init_node: initialize a node. Return OK if ok, NOT_OK if not  */

int init_node(struct node_info *a_node)
{
	if(a_node){
		a_node->n = 0L;
		a_node->n2 = 0L;
		a_node->m = 0;
		a_node->depth = 0;
		a_node->matches = NULL;
		a_node->matches_from = NULL;
		a_node->child0 = NULL;
		a_node->child1 = NULL;
		a_node->parent = NULL;
		return OK;
	}
	else return NOT_OK;
}

/* free_node: free memory allocated for a node */

int free_node(struct node_info *a_node)
{
	free(a_node);
	return OK;
}

/* node_name: returns string name for node: 0 = left branch, 1 = right branch
	(It is up to the caller to free the returned string.)
*/

char *node_name(struct node_info *a_node)
{
		struct node_info *tmp;
		char *buf,*p;

		if(!(buf = malloc(MAX_LEVEL+1)))
			fatal("node_name: cannot allocate name string buffer");

		/* Initialize buffer to contain the null string */
		buf[0] = '\0';

		if(a_node){
			tmp = a_node->parent;
			if(!tmp) return buf;
			strcpy(buf,p=node_name(tmp)); /* recursively begin with
                                                       name of parent */
			free(p);
			if(a_node == tmp->child0)
				strcat(buf,"0");
			else
				if(a_node == tmp->child1)
					strcat(buf,"1");
				else warn("node_name %s called with inconsistent tree",buf);
			return buf;

		}
		warn("node_name called with null pointer\n");
		return NULL;
}

/* dump_node: debugging routine. Print out some info about this node */

int dump_node(struct node_info *a_node)
{
		char *p;

		printf("Info for node %s:\n",p = node_name(a_node));
		free(p);
		printf("\tFirst Match Time = %lu\n",a_node->n);
		if(a_node->mc > 1)
			printf("\tSecond Match Time = %lu\n",a_node->n2);
		printf("\tMatch Length = %d\n",a_node->m);
		printf("\tMatch Count = %d\n",a_node->mc);
		printf("\tPending excursions = %d\n",a_node->depth);
		if(a_node->parent){
			printf("\tParent = %s\n", p = node_name(a_node->parent));
			free(p);
		}
		if(a_node->child0){
			printf("\tChild 0 = %s\n", p = node_name(a_node->child0));
			free(p);
		}
		if(a_node->child1){
			printf("\tChild 1 = %s\n", p = node_name(a_node->child1));
			free(p);
		}
}

/* find_node: return pointer to node corresponding to 01 string name, or NULL
if there is no such node */

struct node_info *find_node(char *name)
{
	struct node_info *a_node = nodes[0];
	char *p = name;
	int n;

	if(name){
		n = strlen(name);
		while((p-name)< n){
			if(a_node){
				if(*p++ == '1')a_node = a_node->child1;
				else a_node = a_node->child0;
			}
			else return NULL;
		}	
		return a_node;
	}
	return NULL;
}

/* The beef: Build the msequence and tree. Return number of nodes built. */
/* Note that internally we count time from n = 0 in the C tradition, but
   add 1 for printouts */

unsigned long build(void)
{
	unsigned long n = 3L, node_cnt = 0L;
	struct node_info *a_node;
	char buf[MAX_LEVEL + 1];
	char *p,*q,*qq;
	char new;
	int m = 1; /* Match length */
	int m_old,exc_depth = 0;

	/* Initialize tree. The first two terms of the sequence do not
           generate matches, so one can think of these as corresponding
           to the root node. Thereafter, exactly one 01 sequence matches
           an each time (n) step, so there is exactly one node
           corresponding with each time step from n=3 on. (Not quite true:
           initial sequences match twice so on the second match the node
           is not created. Instead, its match count is incremented.  */
	
	nodes[node_cnt++] = &root;

	node_at_time[0] = node_at_time[1]  = &root;

	/* Here we enter the main loop which generates each successive digit
           of mseq and creates/updates the node for the corresponding match
           sequence. */

	while(n < max_n){ /* n = 3 on entry */

		/* store current match length */

		m_old = m;

		/* point at longest possible matching string at end of mseq */

		p = mseq+(n - ++m);

		/* search for earlier occurence of p. If none, make p shorter
		   until we can point q to a candidate earlier p */

		while(((q=strstr(mseq,p))==NULL) || (q == p)) p++,m--;

		/* Stash a copy of the match string p for later use */

		strcpy(buf,p);

		/* Now point q at the last such earlier string */

		while(((qq = strstr(q+1,p))!=NULL)&&(qq != p))q=qq;	

		/* Set next digit of m sequence to be opposite the one
                    at the end of matched string beginning at q */

		new = ( *(q+m) == '1' ? '0' : '1' );

		/* Now create and setup new tree entry */

		if(a_node = find_node(mseq+n-m)){
			/* Existing node: increase its match count */
			(a_node->mc)++;
			a_node->n2 = n-1;
		}
		else { /* Must create and setup new node */
			a_node = alloc_node();
			init_node(a_node);
			nodes[node_cnt++] = a_node;
			a_node->n = n-1;
			a_node->m = m;
			a_node->mc = 1;

			/* The parent node has name equal to the head
                           of the saved match string */

			buf[strlen(buf)-1]='\0';
			a_node->parent = find_node(buf);
			if(!a_node->parent)warn("Node has no parent %s",buf);

			/* Set pointer in parent node to point to child */
			if(mseq[n-1] == '1')
				(a_node->parent)->child1 = a_node;
			else	
				(a_node->parent)->child0 = a_node;

			a_node->matches = node_at_time[q-mseq+m-1];
			if(!node_at_time[q-mseq]->matches_from)
				node_at_time[q-mseq]->matches_from = a_node;
		}	
		/* set the excursion depth field by comparing the new
                   match length, m, with the old match length, m_old */

		if(m < m_old) exc_depth += (m_old - m);
		else if(m > m_old)
			if(exc_depth)(exc_depth)--;
		a_node->depth = exc_depth;

		node_at_time[n-1] = a_node;
		mseq[n++] = new;
	}
	return node_cnt;
}

/* Bottom of mseq.c */
