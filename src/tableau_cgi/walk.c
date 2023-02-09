/* tree.c: routines dealing with trees */

#include "global.h"
#include "parse.h"

/* This must be called before beginning a tree walk.
 */

int init_walk(struct treenode *tree)
{
	struct treenode *c;
	struct treenode *old_p;

	/* Walk tree, setting up parent pointers */

	c = tree;
	old_p = c->parent;
	c->parent = NULL; /* So we can tell where the top is */

#if DEBUG >= 2
	printf("init_walk called\n");
#endif

	/* Swoop down and to the left, setting parents as you go */

	while(c != NULL){
#if DEBUG >= 2
	printf("init type = %d depth = %d\n",c->type,c->level);
#endif
		c->usage = TOUCHED;
		if(c->left && (c->left)->usage != TOUCHED) {
			c = c->left;
			continue;
		}
		if(c->right && (c->right)->usage != TOUCHED){
		       c = c->right;
		       continue;
		}
		else {
			c = c->parent;
			continue;
		}
	}
#if DEBUG >= 2
	printf("init_walk returns OK\n");
#endif
	tree->parent = old_p; /* restore original pointer */
	return OK;
}

/* This is used to set parent pointers after a parse tree is complete.  */

void set_parent_pointers(struct treenode *t)
{
	struct treenode *c;

	/* Walk tree, setting up parent pointers */

	c = t;
	c->parent = NULL;

	/* Swoop down and to the left, setting parents as you go */

	while(c != NULL){
		c->usage = TOUCHED;
		if(c->left && (c->left)->usage != TOUCHED) {
			(c->left)->parent = c;
			c = c->left;
			continue;
		}
		if(c->right && (c->right)->usage != TOUCHED){
		       (c->right)->parent = c;
		       c = c->right;
		       continue;
		}
		else {
			c = c->parent;
			continue;
		}
	}
}


/* Return the next node in a depth first walk of the given tree. 
 * Returns NULL when the walk is complete. You must call init_walk
 * before calling this for the first time in a given walk.  The usage field
 * of a node is marked USED once it is visited */

struct treenode *depth_next( struct treenode *current)
{

	struct treenode *p;
	static struct treenode *top_p = NULL;
	static int in_progress = 0;

#if DEBUG >= 2
	printf("depth_next type = %d depth = %d\n",current->type,current->level);
#endif

	/* init_walk marks everything as touched */
	if(!current)return NULL;

	p = current;
	if(!in_progress){ /* First time: remember top of tree */
		top_p = p->parent;
		in_progress = 1;
	}

backtrack:

	if(p == top_p){ /* We are done */
		in_progress = 0;
		top_p = NULL;
		return NULL;
	}
	 
	while(p && p->left && ((p->left)->usage == TOUCHED)) {
		p = p->left;
	}
	if(p && p->right && (p->right)->usage == TOUCHED) {
		p = p->right; 
		goto backtrack;
	}
	if (p && p->usage == USED){ 
	       p = p->parent;
	       goto backtrack;
	}

	if(p)p->usage = USED;
	return p;
}


/* Return the number of leaf nodes in the given tree */

int n_leaves(struct treenode *tree)
{
	struct treenode *p;
	int n = 0;

	if(init_walk(tree)!=OK)return 0;

	p = tree;
	while( p ){

		if(p->type == LEAF)n++;
		p = depth_next(p);

	}
	init_walk(tree); /* clean up */
	return n;
}

int n_nodes(struct treenode *tree)
{
	struct treenode *p;
	int n = 0;

	if(init_walk(tree) != OK)return 0;
	p = tree;
	while(p = depth_next(p)) n++;
	init_walk(tree);
	return n;
}

int depth(struct treenode *tree)
{
	struct treenode *p;
	int n = 0;

	init_walk(tree);
	p = tree;
	while(p = depth_next(p))
	       if(p->level>n)n=p->level;
	init_walk(tree);
	return n;
}
