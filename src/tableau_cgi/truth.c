/* truth.c: implement truth table related routines */

#include "global.h"
#include "parse.h"


#define MAX_VARS 256  /* This is way more than sufficient */

static int bit_field[MAX_VARS]; /* truth, falsity assignments to variables */


/* set_bits: set the global bit_field array according to the binary digits
 * of n. Returns TRUE if it was done, FALSE if there was an error. 
 * Unused fields are set to false */

int set_bits( int n )
{
	int d = 0;  /* the digit */
	int j = 0;

	if(n < 0) return FALSE;

	while( j < MAX_VARS ) {
		d = n % 2;
		n = n/2;
		if(d) bit_field[j] = TRUE;
		else bit_field[j] = FALSE;
		j++;
	}
	return TRUE;
}

void table_header( struct treenode *);
void table_line( struct treenode *);

int truth_table(struct treenode *parse_tree)
{

	int n,i,j,k,l;
	int vcount=0;
	char buf[1024];
	struct treenode **node_array;
	struct treenode *p;

	/* Flatten parse_tree into an array with variables sorted last
	 * alphabetically. */
	n = n_nodes(parse_tree);
	node_array = (struct treenode **)malloc(n*sizeof(struct 
				treenode *));
	flatten(parse_tree,node_array);
	krqsort((void *)node_array, 0, n-1,
		(int (*)(const void *, const
		void *))compare_node_by_name);

	/* Find out how many distinct variables there are */

	buf[0]=0;
	for(i=0;i<n;i++){
		p = node_array[i];
		if(p->type == LEAF)
			if(strcmp(p->name,buf)!=0){
				strcpy(buf,p->name);
				vcount++;
			}
	}

	i = 1;
	j = 0;
	while(j++ < vcount)
		i *= 2;

	/* i = 2^vcount */

	/* print out truth table header line */

	table_header(parse_tree);
	printf("\n");

	/* loop over truth assignments: print truth table line for it at
	 * end of loop */

	for(j = 0; j < i ; j++){

		if (set_bits(j) != TRUE){
#if DEBUG >= 1
			printf("truth_table: failed to set bit field\n");
#endif
			return FALSE;
		}

		/* set the sign field in the parse tree nodes */

	 	/* Loop over distinct variables and set their (leaf) signs */
	
		buf[0]=0;
		k = -1;
		for(l=0;l<n;l++){
			p = node_array[l];
			if(p->type == LEAF)
				if(strcmp(p->name,buf)==0){
					if(k<0)/* should not happen */
						fprintf(stderr,"truth_table: negative leaf index\n");
					else
						p->sign = bit_field[k]; 
				}
				else {
					k++;
					strcpy(buf,p->name);
					p->sign = bit_field[k];
				}
		}

		/* Walk the parse tree depth first, setting the rest of
		 * its node's signs as you go */

		init_walk(parse_tree);
		p = parse_tree;
		while(p){
			int left,right;
			if(p->type != LEAF){
				if(p->type == NOT){
					if((p->right)->sign == TRUE)
						p->sign = FALSE;
					else p->sign = TRUE;
				}
				else {
				left = (p->left)->sign;
				right = (p->right)->sign;
				switch(p->type){

					case AND:
						if((left == TRUE)&&(right == TRUE)) p->sign = TRUE;
								else p->sign = FALSE;
						break;
					case OR:
						if((left == TRUE)||(right == TRUE)) p->sign = TRUE;
								else p->sign = FALSE;
						
						break;
					case IMPLIES:
						if(!((left == TRUE)&&(right == FALSE))) p->sign = TRUE;
								else p->sign = FALSE;
						break;
					case EQUIVALENCE:
						if((left == TRUE)==(right == TRUE)) p->sign = TRUE;
								else p->sign = FALSE;
						break;
					case NAND:
						if(!((left == TRUE)&&(right == TRUE))) p->sign = TRUE;
								else p->sign = FALSE;
						break;
					case STROKE:
						if(!((left == TRUE)||(right == TRUE))) p->sign = TRUE;
								else p->sign = FALSE;
					default:
						fprintf(stderr,"truth_table:UNKNOWN NODE TYPE\n");
						break;

				}
				}
			}

			p = depth_next(p);
		} /* end of tree walk loop */

		/* Now dump the truth assignments to form a truth table */
		table_line(parse_tree);
		printf("\n");

	} /* end loop over truth assignments */
	printf("\n");
	return TRUE;
}


void table_header(struct treenode *t)
{

	struct treenode *l,*p, *r;
		int n,i;

		if(!t)return;
		if(t->type==LEAF){
			printf("%s",t->name);
			return;
		}
		r = t->right;
		l = t->left;
		if(t->precedence > precedence(t->type))
			printf("(");
		table_header(l);
		printf(" %s ",written_as[t->type]); fflush(stdout);
		table_header(r);
		if(t->precedence > precedence(t->type))
			printf(")"); 
}

void space(int n)
{
	int i=0;
	while(i++<n)printf(" ");
}

static level = 0;

void table_line(struct treenode *t)
{

	struct treenode *l,*p, *r;
		char tc='t',fc='f';
		int n,i;

		if(level == 0) {tc='T'; fc = 'F';}

		if(!t)return;
		if(t->type == LEAF){
			if(t->sign == TRUE)
				printf("t");
			else printf("f");
			space(strlen(t->name)-1);
			return;
		}
		r = t->right;
		l = t->left;
		if(t->precedence > precedence(t->type))
			printf(" "); /* gap for ( above */
		level++;
		table_line(l);
		level--;
		if(t->sign == TRUE) printf(" %c",tc);
		else printf(" %c",fc);
		space(strlen(written_as[t->type]));
		level++;
		table_line(r);
		level--;
		if(t->precedence > precedence(t->type))
			printf(" "); 
}
