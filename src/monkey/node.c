/* node.c:  Implementation of a tree-structured dictionary  */

/* By Terry McConnell */


#include "global.h"
#define MAXNAMELEN 6


/* GetNode: returns a pointer to the node  whose name component
   matches the pattern, or NULL if there is no such node. Search
   proceeds from given root on down. This is just a brute force search,
   since there are at most 27 children */

struct NodeStruct *GetNode(char *pattern, struct NodeStruct *root)
{
	struct NodeStruct *ANode;
	struct NodeStruct *found = NULL;
	int i=0,k;

	/* if the pattern does not start out like root's name
               the search cannot proceed  */

	k = strlen(root->name);
	if(k > strlen(pattern))return NULL;


        if(strcmp(root->name,"")!=0)
	  if(strncmp(pattern,root->name,k)!= 0)
		return NULL;

	/* if pattern matches root's name, the search has ended successfully */

	if(strcmp(root->name,pattern)==0)
		return root;
	
	/* Otherwise, continue recursively with root replaced by each of
            root's children */

	ANode = root->firstchild;
	while(i<root->num_children){
	  if((found=GetNode(pattern, ANode))!=NULL)
		return found;
	  else ANode = ANode->sibling;
	  i++;
	}
	return NULL;
}

/* AllocNode: allocate memory and initialize a new node */

struct NodeStruct *AllocNode(char *name)
{
	struct NodeStruct *ANode;

	if(name == NULL)
		fatal(stderr,"Node cannot have NULL name\n");

	if((ANode = (struct NodeStruct *)malloc(sizeof(struct NodeStruct)))
             == NULL)
		fatal(stderr,"Unable to alloc new node\n");

	if((ANode->name = (char *)malloc(sizeof(char)*(strlen(name)+1)))
             == NULL)
		fatal(stderr,"Unable to alloc new node name\n");

	strcpy(ANode->name,name);
	ANode->num_children = 0;
	ANode->freq = 0;
	ANode->firstchild = NULL;
	ANode->sibling = NULL;
	ANode->lastchild = NULL;
	ANode->parent = NULL;

	return ANode;
}

/* NodeInfo: Print info on node on stdout */
void NodeInfo(struct NodeStruct *ANode)
{
	if(ANode == NULL)
		fatal(stderr,"Can't print info about NULL node\n");

	printf("Node Name: %s\n",ANode->name);
	printf("%d children\n",ANode->num_children);
	printf("freq = %d\n",ANode->freq);
	return;
}

/* AddChild: add a child node to the given one with the given char
   added to name
*/
struct NodeStruct *AddChild(struct NodeStruct *ANode, char c)
{
	struct NodeStruct *Child;
	char namebuf[MAXNAMELEN];
	int n;

	if(ANode == NULL) return NULL;
	n = strlen(ANode->name);

	strcpy(namebuf,ANode->name);
	*(namebuf + n) = c;
	*(namebuf + n + 1) = '\0';

	Child = AllocNode(namebuf);
	Child->parent = ANode;
	if(ANode->num_children == 0)
		ANode->firstchild = ( ANode->lastchild = Child );
	else {
		ANode->lastchild->sibling = Child;
		ANode->lastchild = Child;
	}
	ANode->num_children++;
	return Child;
}

void SetFreq(struct NodeStruct *ANode, int freq)
{

	ANode->freq = freq;
	return;
}
		
