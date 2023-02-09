/* This file gets included by all source files */

#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>

/* Make the following larger at your peril. The dictionary source files
   become huge and the compiler will probably barf. Also you will need to
   modify code in main.c to create one more initial letter. */

#define MAXDEPTH 4

/* The basic data structure: contains all relevant information about
   a dictionary node  */

/* There is one node in the dictionary for each string of length <= depth+1
   that occurs. The frequency member lists the number of times the string
   of given name occurs. For example, there is a (relatively low frequency)
   node named "q" attached to the root. Attached to that is a node named
   "qu", but very few others. In turn, attached to "qu" is "qui", "qua",
   etc. The set of children of a given node is organized as a linked list. */
   

struct NodeStruct {
        char *name;
        int num_children;
        int freq;
        struct NodeStruct *sibling;
        struct NodeStruct *firstchild; /* pointer to head of children list */
        struct NodeStruct *lastchild;
        struct NodeStruct *parent;
};


extern struct NodeStruct Root;

/* defined in node.c */

extern struct NodeStruct *GetNode(char *, struct NodeStruct *);
extern struct NodeStruct *AddChild(struct NodeStruct *, char );

/* dumps contents of a node for debugging: */
extern void NodeInfo(struct NodeStruct *);

/* Generic error routine defined in main.c */
extern void fatal(FILE *,char *fmt,...);
