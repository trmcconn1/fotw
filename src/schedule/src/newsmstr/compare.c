/* compare.c: comparison functions used with bsearch and qsort library
    routines. Each must return -1 if the first arg is < the second,
    0 if they are equal, and 1 else */

#include "db.h"
#include<string.h>

int
CompareVarsByName(struct ShellVar *first, struct ShellVar *second)
{
/* NULL comes before anything */
        if(first == NULL)
                if(second == NULL) return 0;
                else return -1;

        if(second == NULL) return 1;

	return strcmp(first->name,second->name);
}

int
CompareTokensByPosition(struct TokenType *first, struct TokenType *second)
{
/* NULL comes before anything */
        if(first == NULL)
                if(second == NULL) return 0;
                else return -1;

        if(second == NULL) return 1;
	if(first->position == second->position) return 0;
	return first->position < second->position ? -1 : 1;
}


struct ShellCmd {
char *name;
char *usage;             /* Associative array of shell variables */
char *description;
int (*function) ();
};

int
CompareCmdsByName(struct ShellCmd *first, struct ShellCmd *second)
{
/* NULL comes before anything */
        if(first == NULL)
                if(second == NULL) return 0;
                else return -1;

        if(second == NULL) return 1;

	return strcmp(first->name,second->name);
}
