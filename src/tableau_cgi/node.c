/* node.c: implement routines that involve tree nodes */


#include "global.h"
#include "parse.h"

/* For sorting leaves by name: */

int compare_node_by_name( const struct treenode *anode , const struct treenode *bnode )
{

	/* null pointers sort first; maybe we want to find out
	 * about them soonest. */

	if(!anode){
		if(!bnode)return 0;
		return -1;
	}
	else if(!bnode) return 1;
	else {
		/* Non leaf nodes sort before leaves. This seems the more
		 * natural order */

	       if(!(anode->type == LEAF)) {
		       if(!(bnode->type == LEAF)) return 0;
		       return -1;
	       }
       	       if(!(bnode->type == LEAF)) return 1;	       
	       else {

			/* Finally, we sort lexicographically on the
			 * leaf name. Again, null strings come first. */

		       if(!anode->name){
			       if(!anode->name)return 0;
			       else return -1;
		       }
		       else if(!bnode->name)return 1;
				else return strcmp(anode->name,bnode->name);
		}

	}
}

void node_dump( struct treenode *anode)
{
	if(anode)
	switch(anode->type){
		case AND:
			printf("AND node\n");
			break;
		case OR:
			printf("OR node\n");
			break;
		case NOT:
			printf("NOT node\n");
			break;
		case IMPLIES:
			printf("IMPLIES node\n");
			break;
		case NAND:
			printf("NAND node\n");
			break;
		case STROKE:
			printf("STROKE node\n");
			break;
		case LEAF:
			printf("Leaf name = %s\n",anode->name);
			break;
		default:
			break;
	}
}

/* flatten: Store the successive nodes of tree in array. Return the
 * number of nodes stored. */

int flatten (struct treenode *tree, struct treenode **node_array)
{

	struct treenode *p;
	int i = 0;

	if(!tree || !node_array)return 0;

	p = tree;
	init_walk(tree);
	while(p = depth_next(p))
		node_array[i++] = p;
	init_walk(tree);
	return i;

}

int is_descendent( struct treenode *t, struct treenode *s)
{
	if(!t) return TRUE;
	if(!s) return FALSE;
	if(t == s) return TRUE;

	if(t->left && (is_descendent(t->left,s)==TRUE))return TRUE;
	if(t->right && (is_descendent(t->right,s)==TRUE))return TRUE;
	return FALSE;
}


int node_depth(struct treenode *t)
{
	int n = 0;
	struct treenode *p;

	if(!t)return -1;
	p = t;
	while(p = p->parent)n++;
	return n;
}


/* How many nodes "to the right" is the present node within its tree ?
 * We assume a binary tree with all nodes present. Thus we count the
 * number of times n we are the right child on the way up the tree towards
 * the root. We return 2^n - 1;
 */

int rightness(struct treenode *t)
{
	struct treenode *p;
	int *d;
	int r=0,n,i;

	if(!t)return -1;

	n = depth(t);
	if(n==0)return 0;

	d = (int *)malloc(n*sizeof(int));
	if(!d){
		fprintf(stderr,"rightness: cannot allocate space\n");
		exit(1);
	}
	p = t;
	while (p->parent){
		if(p == (p->parent)->right) d[i++] = 1;
		else d[i++]=0;
		p = p->parent;
	}
	for(i=n-1;i>=0;i--)r = 2*r + d[i];
	free(d);
	return r;
}

/* equal_expression: return true if two trees reduce to the same expression */

int equal_expression(struct treenode *t1, struct treenode *t2){

	if(t1 == t2) return TRUE;
	if(t1){
		if(!t2) return FALSE;
		if(t1->type == LEAF){
			if(t2->type != LEAF)return FALSE;
			if(strcmp(t1->name,t2->name)==0)return TRUE;
			return FALSE;
		}
		if(t2->type == LEAF)return FALSE;
		if((t1->type == t2->type) && (equal_expression(t1->left,t2->left) == TRUE)
			       &&
		       (equal_expression(t1->right,t2->right)== TRUE))
	 		return TRUE;
		return FALSE;
	}		
	return FALSE;
}

			

