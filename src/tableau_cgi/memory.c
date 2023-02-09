#include "global.h"
#include "parse.h"


struct treenode *alloc_node(char *name)
{
	struct treenode *p;
	char *n = NULL;
	if(name)
		n = (char *)malloc((strlen(name)+1)*sizeof(char));
	else n = NULL;
	if(n) strcpy(n,name);
	p = (struct treenode *)malloc(sizeof(struct treenode));
	p->name = n;
	p->usage = EMPTY;
	return p;
}

void free_node(struct treenode *p)
{
	if(p->name)free(p->name);
	free(p);
}

void free_tree(struct treenode *p)
{
	if(p->left)free_tree(p->left);
	if(p->right)free_tree(p->right);
	free_node(p);
}

/* Return a pointer to an exact copy of treenode given, but don't set
 * tree location fields: parent, left, right. */

struct treenode *copy_node(struct treenode *t)
{
	struct treenode *p;

#if DEBUG >= 2
	printf("copy node: ");
	dump_parse_tree(t->expression,0);
	printf("\n");
#endif
	p = alloc_node(t->name);
	if(!p)return NULL;
	p->type = t->type;
	p->level = t->level;
	p->precedence = t->precedence;
	p->sign = t->sign;
	p->expression = t->expression;
	p->usage = t->usage;
	p->parent0 = t->parent0;
	return p;
}

/* Return pointer to exact (new) copy of tree including and below
 * node pointed to by t. Calls self recursively. Does not set parent
 * field of new root node (caller should do that) */

struct treenode *dup_tree(struct treenode *t)
{
	struct treenode *l,*r,*p;

	if(!t)return NULL;
	p = copy_node(t);
	p->right = dup_tree(t->right);
	if(p->right)(p->right)->parent = p;
	p->left = dup_tree(t->left);
	if(p->left)(p->left)->parent = p;
	return p;
}
