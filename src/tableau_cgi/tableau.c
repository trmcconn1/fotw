/* tableau.c: routines for creation and management of tableaux */

#include "global.h"
#include "parse.h"


struct treenode *next_unused(struct treenode *);
struct treenode *next_unused_alpha(struct treenode *);
int is_alpha(struct treenode *);
void handle_alpha(struct treenode *);
void swap_node_data(struct treenode *, struct treenode *);
void handle_beta(struct treenode *);
void handle_not(struct treenode *);
void inc_level(struct treenode *, int);
int print_width(struct treenode *);
void indent(int);

/* Create complete analytic tableau for parse trees of expressions pointed to
 * by t. The signs tell whether the expressions are to be deemed TRUE
 * or FALSE for the purpose of creating the tableau.  
 * Return pointer to root of resulting tableau tree */

struct treenode *tableau(struct treenode *t[], int sign[], int nn)
{

	struct treenode *root, *anode, *q,*p;
	int i;

	if(!t) return NULL;


	/* Build intial part of tableau */

	if(!(root = alloc_node(NULL))){
			fprintf(stderr,"tableau: cannot allocate root node\n");
			return NULL;
	}	
	root->type = t[0]->type;
	root->level = 0;
	root->sign = sign[0];
	root->usage = EMPTY; 
	root->expression = t[0];
	root->parent = NULL;
	root->parent0 = NULL; /* original parent */
	root->left = root->right = NULL;
	q = root;

	for(i=1;i<nn;i++){
		if(!(anode = alloc_node(NULL))){
			fprintf(stderr,"tableau: cannot allocate node %d\n",i);
			exit(1);
		}
		anode->type = t[i]->type;
		anode->level = i;
		anode->sign = sign[i];
		anode->usage = EMPTY;
		anode->expression = t[i];
		anode->parent=q;
		anode->parent0=NULL;
		anode->left=anode->right = NULL;
		q->left = anode;
		q = anode;
	}

	/* Build the tableau using alpha and beta rules until saturation */

	while(q = next_unused(root)){ /* loop until saturated */
		if(is_alpha(q)== FALSE){
			p = next_unused_alpha(q); /* use alpha nodes before beta                                                     if possible */
			if(p)swap_node_data(p,q);
		}
		q->usage = USED;		
		if(q->type == NOT){
			handle_not(q);
			continue;
		}
		if(q->sign == TRUE) {
			switch(q->type){
				/* alpha node */
					case STROKE:
					case AND:
						handle_alpha(q);
						break;
				/* beta node */
					case NAND:
					case IMPLIES:
					case OR:
						handle_beta(q);
						break;
					default: /* LEAF */
						break;
			}
		}
		else  {  /* FALSE */
			switch(q->type){
				/* alpha node */
					case NAND:
					case IMPLIES:
					case OR:
						handle_alpha(q);
						break;
				/* beta node */
					case AND:
					case STROKE:
						handle_beta(q);
						break;
					default: /* LEAF */
						break;
			}
		}
		
	}
	return root;
}

/* Leave tree position data (mostly pointers) alone, just swap node data */

void swap_node_data(struct treenode *a, struct treenode *b)
{
	struct treenode *t;
	int tmp;
	char *temp;

	t = b->parent0;
	b->parent0 = a->parent0;
	a->parent0 = t;

	t = b->expression;
	b->expression = a->expression;
	a->expression = t;

	tmp = b->type;
	b->type = a->type;
	a->type = tmp;

	tmp = b->sign;
	b->sign = a->sign;
	a->sign = tmp;

	/* This would be wrong!
	tmp = b->level;
	b->level = a->level;
	a->level = tmp;
	*/

	tmp = b->usage;
	b->usage = a->usage;
	a->usage = tmp;

	tmp = b->precedence;
	b->precedence = a->precedence;
	a->precedence = tmp;

	tmp = b->closure;
	b->closure = a->closure;
	a->closure = tmp;

	temp = b->name;
	b->name = a->name;
	a->name = temp;

}


/*  Return pointer to next unused 
 *  node in top down (secondary left-right,) order. 
 *  */

struct treenode *next_unused(struct treenode *t){

	struct treenode *p;

	if(!t)return NULL;
	if(t->usage != USED)return t;
	p = next_unused(t->left);
	if(p)return p;
	return next_unused(t->right);
}

/* Same as next_unused, but find alpha nodes only. */

struct treenode *next_unused_alpha(struct treenode *t){

	struct treenode *p;

	if(!t)return NULL;
	if((t->usage != USED) && (is_alpha(t)==TRUE))return t;
	p = next_unused_alpha(t->left);
	if(p)return p;
	return next_unused_alpha(t->right);

}
	
int is_alpha(struct treenode *q)
{
	if(!q) return FALSE;
	if(q->type == LEAF)return FALSE;
	if(q->type == NOT)return TRUE;
	if(q->sign == TRUE){
		if(q->type == AND)return TRUE;
		return FALSE;
	}
	else {
		if(q->type == AND)return FALSE;
		return TRUE;
	}
}

void handle_alpha(struct treenode *q)
{
	struct treenode *new1, *new2, *r, *l;


#if DEBUG >= 2
	printf("Handling alpha: ");
	dump_parse_tree(q->expression,0); 
	printf("\n");
#endif

	if(!(new1 = alloc_node(NULL))) {
		fprintf(stderr,"handle_alpha: cannot alloc memory for new node\n");
		exit(1);
	}
	if(!(new2 = alloc_node(NULL))) {
		fprintf(stderr,"handle_alpha: cannot alloc memory for new node\n");
		exit(1);
	}

	new1->type = ((q->expression)->left)->type;
	if(new1->type == LEAF) new1->name = strdup(((q->expression)->left)->name);
	new1->level = q->level+1;
	new1->usage = EMPTY;
	new1->parent0=q; /* once set, does not change. Used to track which
			    expression gave rise to this node */
	new1->expression = (q->expression)->left;
	new2->type = ((q->expression)->right)->type;
	if(new2->type == LEAF) new2->name = strdup(((q->expression)->right)->name);
	new2->usage = EMPTY;
	new2->level = q->level+2;
	new2->expression = (q->expression)->right;
	new2->parent0=q;

	/* set signs of new nodes here */

	if(q->sign == TRUE){
		switch(q->type){
			case AND:
				new1->sign = new2->sign = TRUE;
				break;
			case STROKE:
				new1->sign = new2->sign = FALSE;
				break;
			default:
				break;
		}
	}
	else { /* FALSE */
		switch(q->type){
			case OR:
				new1->sign = new2->sign = FALSE;
				break;
			case IMPLIES:
				new1->sign = TRUE;
				new2->sign = FALSE;
				break;
			case NAND:
				new1->sign = new2->sign = TRUE;
				break;
			default:
				break;
		}
	}
	
	/* insert in tableau tree */
	r = q->right;
	l = q->left;

	q->left = new1;
	q->right = NULL;
	new1->parent = q;
	new1->left = new2;
	new2->parent = new1;
	new1->right = NULL;
	new2->left = l;
	if(l)l->parent = new2;
	new2->right = r;	
	if(r)r->parent = new2;
	if(l)inc_level(l,2);
	if(r)inc_level(r,2);
}

void handle_not(struct treenode *q)
{
	struct treenode *new, *r, *l;

#if DEBUG >= 2
	printf("Handling not: ");
	dump_parse_tree(q->expression,0);
	printf("\n");
#endif
	if(!(new = alloc_node(NULL))) {
		fprintf(stderr,"handle_not: cannot alloc memory for new node\n");
		exit(1);
	}
	new->type = ((q->expression)->right)->type;
	if(new->type == LEAF)new->name = strdup(((q->expression)->right)->name);
	new->level = q->level+1;
	new->usage = EMPTY;
	new->expression = (q->expression)->right;
	new->parent0 = q;

	/* set sign of new node here */

	if(q->sign == TRUE)
		new->sign = FALSE;
	else new->sign = TRUE;

	/* insert in tableau tree */
	r = q->right;
	l = q->left;

	q->left = new;
	q->right = NULL;
	new->parent = q;
	new->left = l;
	if(l)l->parent = new;
	new->right = r;	
	if(r)r->parent = new;
	if(l)inc_level(l,1);
	if(r)inc_level(r,1);
}


void handle_beta(struct treenode *q){
	struct treenode *new1, *new2, *r, *l;

#if DEBUG >= 2
	printf("Handling beta: ");
	dump_parse_tree(q->expression,0);
	printf("\n");
#endif
	if(!(new1 = alloc_node(NULL))) {
		fprintf(stderr,"handle_alpha: cannot alloc memory for new node\n");
		exit(1);
	}
	if(!(new2 = alloc_node(NULL))) {
		fprintf(stderr,"handle_alpha: cannot alloc memory for new node\n");
		exit(1);
	}

	new1->type = ((q->expression)->right)->type;
	if(new1->type == LEAF) new1->name = strdup(((q->expression)->right)->name); 
	new1->usage = EMPTY;
	new1->level = q->level+1;
	new1->expression = (q->expression)->right;
	new1->parent0 = q;
	new2->type = ((q->expression)->left)->type;
	if(new2->type == LEAF)new2->name = strdup(((q->expression)->left)->name);
	new2->usage = EMPTY;
	new2->level = q->level+1;
	new2->expression = (q->expression)->left;
	new2->parent0 = q;

	/* set signs of new nodes here */

	if(q->sign == TRUE){
		switch(q->type){
			case OR:
				new1->sign = new2->sign = TRUE;
				break;
			case IMPLIES:
				new2->sign = FALSE;
				new1->sign = TRUE;
				break;
			case NAND:
				new1->sign = new2->sign = FALSE;
				break;
			default:
				break;
		}
	}
	else { /* FALSE */
		switch(q->type){
			case AND:
				new1->sign = new2->sign = FALSE;
				break;
			case STROKE:
				new1->sign = new2->sign = TRUE;
				break;

			default:
				break;
		}
	}
	
	/* insert in tableau tree */
	r = q->right; /* save these */
	l = q->left;

	q->right = new1;
	q->left = new2;
	new1->parent = q;
	new2->parent = q;
	if(!r){
		new1->right = NULL;
		new2->right = NULL;
	}
	else {
		inc_level(r,1);
		new1->right = r;
		r->parent = new1;
		new2->right = dup_tree(r);
		(new2->right)->parent = new2;
	}
	if(!l){
		new1->left = NULL;
		new2->left = NULL;
	}
	else {
		inc_level(l,1);
		new1->left = l;
		l->parent = new1;
		new2->left = dup_tree(l);
		(new2->left)->parent = new2;
	}

}
	

void inc_level(struct treenode *t, int n)
{
	struct treenode *p;
	if(!t)return;
	init_walk(t);
	p = t;
	while ( p = depth_next(p))
		p->level += n;
	init_walk(t);
}

/* Return TRUE if branch containing this node is closed, FALSE otherwise */

int is_closed(struct treenode *t)
{
	struct treenode *p,*q;

	if(!t)return FALSE;
	p = t;
	if(!p->parent)return FALSE;
	while(p){
		q = p;
		while(q = q->parent)
			if(equal_expression(p->expression,q->expression)==TRUE)
				if(p->sign != q->sign)return TRUE;	
		p = p->parent;
	}
	return FALSE;
}


/* Print out analytic tableau in a readable format. BUG: This alters
 * the tree. 
 *
 * Getting the format right is a b*tch! */

void dump_tableau(struct treenode *t)
{
	struct treenode *p,*q,*r;
	char buf[16];
	struct treenode **node_array;
	int **widths;
	int i,n,level=0,m,j,w,k,l,lw,ln;

	if(!t)return;

	m = depth(t);

	/* It is very convenient for displaying the tableau to have all
	 * branches of equal length. Thus, we walk the tree and attach
	 * enough additional "closure" nodes at the bottom. These display
	 * in a special way (usually, as blank space) */

	init_walk(t);

	p=t;
	while(p = depth_next(p))
		if((p->left == NULL) && ((j = p->level) < m )){
			r = p;
			for(i=0;i<m-j;i++){
				q = alloc_node(NULL);
				if(!q){
					fprintf(stderr,"can't complete tree\n");
					return;
				}
				r->left = q;
				q->expression = r->expression;
				q->type = CLOSURE;
				q->sign = r->sign;
				q->parent = r;
				q->usage = USED;
				q->left = q->right = NULL;
				q->level = r->level+1;
				q->parent0 = r->parent0;
				r = q;
			 }	
		}
	init_walk(t);
	n = n_nodes(t);

	/* Reserve room to store branch field widths */
	widths = (int **)malloc(n*sizeof(int *));
	if(!widths){
		fprintf(stderr,"dump_tableau: cannot allocate memory\n");
		return;
	}
	for(i=0;i<n;i++){
		widths[i] = (int *)malloc(sizeof(int));
		if(!widths[i]){
			fprintf(stderr,"dump_tableau: cannot allocate memory\n");
			return;
		}
		*widths[i]=0;
	}

	/* Flatten tree into an array. Everything to the left of
	 * a given node in the tree precedes it in the flattened array. 
	 * This is particularly convenient for printing out the tableau. For
	 * each level of the tree we print the expression for each node
	 * on that level with spaces for the nodes that preceed and lie
	 * below it. This makes branches of the tree line up. */

	node_array = (struct treenode **)malloc(n*sizeof(struct 
				treenode *));
	if(!node_array){
		fprintf(stderr,"dump_tableau: cannot allocate memory\n");
		return;
	}
	flatten(t,node_array);

	/* Print heading */
	q = t;
	printf("\n\tAnalytic Tableau for");
	while(q){
		if(q->parent0==NULL){
			if(q->sign==TRUE)
        			printf(" T:");
			else printf(" F:");
			pretty_print(q->expression,MINIMAL,1);
			printf("\n\t                    ");
		}
		q=q->left;
	}
	printf("\n");

	/* figure out the widest row label */
	sprintf(buf,"%d.",m+1);
	lw = strlen(buf);
	for(level=0;level<=m+1;level++){ /* each level is a line of output */
		/* figure out how many nodes on this level */ 
		ln = 0;
		for(i=0;i<n;i++){
			p = node_array[i];
			if(p->level == level)ln++;
		}
		if(level == m+1)
			indent(lw); /* No label on closure row */
		else {
			sprintf(buf,"%d.",level+1);
			printf("%s",buf);
			indent(lw-strlen(buf));
		}
		j=0; /* number of nodes(maybe empty) printed on level so far */
		for(i=0;i<n;i++){
			w=0;
			p = node_array[i];
			if((level == m+1 && p->level == m)
					|| (p->level == level)){
				if(j) {
					printf("|");/* skip for first node */
				}
				if(level == m+1){

					/* Bottom row: display closure */

					if(is_closed(p)){
						printf(" X");
					}
					else printf(" O");
					w+=2;

				}
				else{

					if(p->type != CLOSURE){
						if(p->sign == TRUE) printf("t:");
						else printf("f:");
					}
					if(p->type != CLOSURE)
					       	w += pretty_print(p->expression,
						MINIMAL,1)+2;
					else 
						w += pretty_print(p->expression,MINIMAL,0)+2; 
					if((p->parent0)){
						sprintf(buf,"(%d)",
						   (p->parent0)->level + 1);
						w += strlen(buf);
						if(p->type != CLOSURE)
							printf("%s",buf);
					}
					if(p->type == CLOSURE)indent(w);

				}
				/* leave space for alignment */
				k = print_width(p);
				if(*widths[j]>=k) {
					/* see if width should shrink */
					r = p;
					while(r->parent)
						if(r->parent->right != NULL)
							break;
						else r = r->parent;
					k = print_width(r);
					*widths[j] = k;
				}
				else *widths[j]=k;
				/* Space saving measure: don't leave room
				 * if there are no following nodes on this 
				 * line */
				if((level == (m+1)) || (ln > j+1))indent(k-w);
				j++;
			}
		}
		printf("\n");
	}

	free(node_array);
}

/* Return space taken up by displaying this node's tree */

int print_width(struct treenode *t)
{
	char buf[16]; /* If this isn't big enough, you're going to need
			 a lot of paper in the printer! */
	int n=0,m=0;
	struct treenode *r;

	if(!t)return 0;

	/* If we are first on a row, the row label gets charged against
	 * our width */

	if(t->parent0){
		sprintf(buf,"(%d)",(t->parent0)->level + 1);
		n = strlen(buf);
	}
	/* The 2 accounts for the sign tag: f or t */
	if(t->expression)
		n += 2 + pretty_print(t->expression,MINIMAL,0);

	/* See if the children have a larger combined print width */
	if(t->left)
		m=print_width(t->left);
	/* +1 accounts for |. There can't be a right child w/o there
	 * being a left child. */
	if(t->right) m += print_width(t->right)+1;
	if(m > n)return m;
	return n;
}

void indent(int n)
{
	int i;
	for(i=0;i<n;i++)
		printf(" ");
}
