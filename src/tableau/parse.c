/* parse.c: Parser for sentential logic wffs . Uses lex generated 
 * scanner. */

#define PARSE_C

#include "global.h"
#include "parse.h"

extern void yyrestart(FILE *);
extern void reset(void);


static int recursive_level;

/* This is a parser for sentential logic. The grammar is as follows:
 *
 * S is statement or start symbol, V is variable
 *
 * V -> S
 * (S) -> S
 * S OR S -> S
 * S AND S -> S
 * S IMPLIES S -> S
 * NOT S -> S
 * S NAND S -> S
 * S STROKE S -> S
 *
 *
 * We call yylex for next input token and use recursive decent parsing,
 * which is ok since the grammar is LLR(1) */

/* All non-associative connectives associate to the right by default, i.e.,
 * if not over-ridden by parentheses. For example, A -> B -> C means
 * A -> ( B -> C )
*/

int is_binary_connective(int);
int precedence(int);
int associativity(int);
struct treenode *attach_node( struct treenode *, struct treenode *,int);

/* parse: Read stream until end of file and return pointer to parse tree.
 * If no valid parse tree is generated, return NULL.
 * OK is returned through rval if all goes well. Otherwise we try to
 * return a value (defined in global.h) that tells what went wrong. 
 *
 * We use recursive decent parsing, with one token lookahead.
 * One complication is illustrated by an expression such as 
 * ( A OR B ) AND C.   If we return after ) then we will never get to
 * parse the "AND C", since we are returning from the top level. To get 
 * around that, we wrap a loop around the main parse operation and iterate
 * the loop only when we read a ). 
 * 
 * The other option would be to have the caller repeatedly call parse until
 * there is nothing more to read, but this approach has the advantage of
 * making the parser self-contained.
 *
 * Precedence and associativity issues are handled by attach_node.
 *
 * */

struct treenode *parse ( FILE *stream, int *rval, int *sign)
{

	int token;
	int lookahead;
	int error_code = OK;
	int next_op;
	char tokenbuf[MAXTOKEN];
	struct treenode *new  = NULL, *left;

	*rval = OK;  /* hope for the best */

	if(recursive_level == 0){
		yyin = stream; /* Point scanner to stream */
		reset();
		/* yyrestart(stream); */
	}

	recursive_level++;

    while(1){   /* N.B: break leads to return */

      token = yylex();

#if DEBUG >= 2
      printf("Read token = %d, level=%d\n",token, recursive_level);
#endif

      if((token == END_OF_INPUT) || (token == NEWLINE))break;
      if(token == FALSE_SIGN) {
	      	discard();
		*sign = FALSE;
		continue;
      }
      if(token == TRUE_SIGN) {
		discard();
	  	*sign = TRUE;
		continue;
     }	

      if(!new){  /* New parse, rather than ongoing one after ) */

	strcpy(tokenbuf,current_token); /* Save copy of token buffer for 
					   leaf name */
	discard(); /* clear token buffer in lexer */
	lookahead = yylex(); /* May unput later, so don't discard now */
        if(lookahead == FALSE_SIGN) 
		*sign = FALSE;
      	if(lookahead == TRUE_SIGN) 
	  	*sign = TRUE;

#if DEBUG >= 2
	printf("Read lookahead = %d, level=%d\n", lookahead,recursive_level);
#endif

	if((token == VARIABLE) && ((lookahead == RIGHT_DELIMITER)||
				(lookahead == END_OF_INPUT) ||
				(lookahead == NEWLINE) ||
			        (lookahead == TRUE_SIGN)||
			        (lookahead == FALSE_SIGN))){
			if( (new = alloc_node(tokenbuf)) == NULL){
				*rval = MEMORY_ERROR;
				break;
			}
			new->type = LEAF;
			new->level = recursive_level;
			new->left = new->right = NULL;
#if DEBUG >= 2
			printf("Allocated right leaf node, level=%d\n",
					recursive_level);
#endif
			discard();
			break;
	}

	if((token == VARIABLE) && (is_binary_connective(lookahead)==TRUE)){

			discard(); /* lookahead remembers operator */

		/* We have read var connective ... . We need to alloc
		 * a node for this connective, and set var as its left leaf. */

			if( (left = alloc_node(tokenbuf)) == NULL){
				*rval = MEMORY_ERROR;
				new = NULL;
			        break;				
			}

#if DEBUG >= 2
			printf("Allocated left leaf node, level=%d\n",
					recursive_level);
#endif

			left->type = LEAF;
			left->level = recursive_level;
			left->right = left->left = NULL;

			/* Next we parse stuff to right of our connective. */

#if DEBUG >= 2
			printf("Push to evaluate right operand, level=%d\n",
					recursive_level);
#endif

			new = parse(stream,&error_code,sign);
			if(!new){
				*rval = PARSE_ERROR;
#if DEBUG >= 2
				printf("Doink. Error code = %d, level=%d\n",error_code, recursive_level);
#endif
				free_node(left);
				break;
			}

#if DEBUG >= 2
			printf("Successfully parsed right operand tree, level=%d\n", recursive_level);
#endif

			new = attach_node(left,new,lookahead);
			new->level = recursive_level;
			break;
	}

	if(token == NOT){

			myunput(); /* put back following (variable name ?)*/
			discard();
			if((left = alloc_node(NULL)) == NULL){
				*rval = MEMORY_ERROR;
				break;
			}
#if DEBUG >= 2
			printf("Allocate NOT node, push to evaluate operand,level=%d\n", recursive_level);
#endif
			left->type = NOT;
			left->precedence = precedence(NOT);
			left->level = recursive_level;
			if((new = parse(stream,&error_code,sign))==NULL){
#if DEBUG >= 2
				printf("Doink. Error code = %d, level=%d\n",error_code, recursive_level);
#endif
				*rval = error_code;
				free(left);
				break;	
			}
#if DEBUG >= 2
			printf("Back from push, level=%d\n",recursive_level);
#endif
			new = attach_node(left,new,NOT);
			new->level = recursive_level;
			break;

			
	} /* End of NOT case */

	if(token == LEFT_DELIMITER){
		
#if DEBUG >= 2
		printf("Read Left delimiter. Push. level = %d\n",recursive_level);
#endif
		myunput();
		discard();
		new = parse(stream,&error_code,sign);
		if(new == NULL){
#if DEBUG >= 2
			printf("Doink. Error code = %d, level=%d\n",error_code,recursive_level);
#endif
			*rval = error_code;
			break;
		}
#if DEBUG >= 2
		printf("Back from push, level=%d\n", recursive_level);
#endif
	
		new->precedence = MAX_PRECEDENCE; /* Grouped unit */
		continue;  /* with ongoing parse */
	}
    }
 else {  /* continuing parse after ) */

	discard();
	if(token == RIGHT_DELIMITER) break;

	if(!is_binary_connective(token)){
		*rval = PARSE_ERROR;
		free_tree(new);
		new = NULL;
		break;
	}
	left = new;
	new = parse(stream,&error_code,sign);
	if(!new){
		*rval = error_code;
#if DEBUG >=2
		printf("Doink. Error code = %d, level=%d\n",error_code, recursive_level);
#endif
		break;
	}
	new = attach_node(left,new,token);
	new->level = recursive_level;
	break;
   } /* end new vs. continuing parse if ... else */

 } /* end while */

 recursive_level--;
 return new;

}

int is_binary_connective(int x)
{
	switch(x) {

		case OR:
		case AND:
		case IMPLIES:
		case EQUIVALENCE:
		case NAND:
		case STROKE:
			return TRUE;
		default:
			return FALSE;
	}
}

int precedence(int x)
{
	switch(x) {

		case EQUIVALENCE:
			return 0;
		case IMPLIES:
			return 1;
		case OR:
		case STROKE:
			return 2;
		case AND:
			return 3;
		case NOT:
			return 4;
		default:
			return 0;
	}
	return 0;
}

int associativity(int x)
{
	switch(x) {

		case EQUIVALENCE:
		case OR:
		case AND:
			return ASSOCIATIVE;
		case IMPLIES:
		case NAND:
		case NOT:
		case STROKE:
			return RIGHT_ASSOCIATIVE;
		default:
			return ASSOCIATIVE;
	}
	return ASSOCIATIVE;
}

/* attach_node: graft a new node (anode) on the top of an existing tree. 
How to do this depends on the root node operator's (op's) precedence vs the 
precedence of the current operator (op) It will either look like

                     next_op  <-------- anode's operator          
                     /    \
                   /        \ 
                  op       anode's right arg 
                /   \           
              /       \
            L           R

where L is root's left operand and R is anode's left operand. This would
be the case when op takes precedence over next_op. 
			 
Otherwise, we have    op
                     /   \
                   /       \
                L           anode
 
*/                  

struct treenode *attach_node( struct treenode *root, struct treenode *anode,
		int op)
{

	struct treenode *temp,*new;
	int next_op;

	if( op != NOT) /* only need temp in binary case */
	if( (temp = alloc_node(NULL)) == NULL)
		return NULL;

	if(anode->type == LEAF){
		if(op == NOT){
			root->right = anode;
			root->left = NULL;
			return root;
		}
		temp->type = op;
		temp->precedence = precedence(op);
		temp->right = anode;
		temp->left = root;
		new = temp;
	}
	else {
		next_op = anode->type;
		if((anode->precedence < precedence(op)) ||
		   ((anode->precedence == precedence(op))&&
		   (associativity(op)==LEFT_ASSOCIATIVE))) {
			if((new = alloc_node(NULL)) == NULL){
				free(temp);
				return NULL;
			}
			if(op == NOT){
				root->right = anode->left;
				root->left = NULL;
			}
			else {
				temp->left = root;
				temp->type = op;
				temp->precedence = precedence(op);
				temp->right = anode->left;
			}
			new->precedence = precedence(next_op);
			new->type = next_op;
			if(op == NOT) new->left = root;
			else new->left = temp;
			new->right = anode->right;
			free_node(anode);
		}
		else {
#if DEBUG >= 2
				printf("Invoking precedence rule %d over %d, level = %d\n",					next_op,op,recursive_level);
#endif
				if(op == NOT){
					root->left = NULL;
					root->right = anode;
					return root;
				}
				temp->type = op;
				temp->precedence = precedence(op);
				temp->left = root;
				temp->right = anode;
				new = temp;
			}
	}
	return new;
}
