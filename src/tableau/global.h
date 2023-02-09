/* global.h: Included by all .c source files in this package */

/* The usual goodies: */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAXTOKEN 1024
#define MAX_EXPRESSIONS 1024

/* Types of return from the parser */

#define OK 0
#define PARSE_ERROR 1
#define MEMORY_ERROR 2
#define SCAN_ERROR 3
#define UNKNOWN_ERROR 4
#define TOAST 5

/* Types of Tableau node */

#define SENTENCE 100
#define CLOSURE  101

#define TRUE 1
#define FALSE 0

#define CLOSED 1
#define OPEN 0

/* Values for usage field in tree node */

#define EMPTY 0
#define TOUCHED 1
#define USED 2

/* How agressive to be putting in parentheses */

#define MINIMAL 0
#define MODERATE 1
#define PEDANTIC 2

/* Structure describing node in both parse tree and tableau tree */

struct treenode {

	int type;    /* What is the operation/type at this node? */
	int level;   /* At what depth in tree does this node occur ? */
	int precedence; /* Precedence of operation. Can be overridden
			   by grouping (..) */
	char *name;   /* The variable name if this is a leaf */
	int sign;    /*  Used for tableau node */
	/* children: */
	struct treenode *left;  
	struct treenode *right;
	struct treenode *parent; 
	struct treenode *expression; /* used when this is a tableau node */
	struct treenode *parent0;    /* " " */
	int usage;   /* Used to mark node during creation of tableau tree */
	int closure;

};

/* Implemented in main.c */
void dump_parse_tree(struct treenode *,int);
int pretty_print(struct treenode *,int,int);

/* Implemented in parse.c: */
extern struct treenode *parse( FILE *, int *, int *);
extern int precedence(int);

/* Implemented in memory.c */
extern struct treenode *alloc_node(char *);
extern void free_node(struct treenode *);
extern void free_tree(struct treenode *);
extern struct treenode *copy_node(struct treenode *);
extern struct treenode *dup_tree(struct treenode *);

extern int line; /* unused for now. Defined in lex.l */

/* implemented in walk.c */
extern int init_walk(struct treenode *);
extern int n_leaves(struct treenode *);
extern int n_nodes(struct treenode *);
extern int depth(struct treenode *);
extern struct treenode *depth_next(struct treenode *);
extern void set_parent_pointers(struct treenode *);

/* implemented in node.c */
extern int compare_node_by_name(const struct treenode *, const struct treenode *);
extern void node_dump(struct treenode *);
extern int flatten(struct treenode *, struct treenode **);
extern int rightness(struct treenode *);
extern int equal_expression(struct treenode *, struct treenode *);
extern int is_descendent(struct treenode *, struct treenode *);

/* implemented in qsort.c */
void krqsort ( void *arrayname[], int, int, int (*)( const void *, const void *));

/* implemented in truth.c */
int truth_table(struct treenode *);

/* implemented in tableau.c */
struct treenode *tableau(struct treenode *[], int[], int);
void dump_tableau(struct treenode *);
