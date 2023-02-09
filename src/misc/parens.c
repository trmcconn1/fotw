/* parens.c: implementation of a recursive descent parser to check whether
   parentheses are properly matched.

   compile: cc -o parens parens.c

   usage: Reads stdin until EOF. Ignores everything except left and right
   parentheses. Prints out only the parentheses up to the point at which 
   any error occurs. An error message is also printed. If no error message
   is printed, then parentheses are properly matched. Program returns 1 if
   there is a missmatch, and 0 otherwise.

*/


/* The grammer is:

Wff: '(' Wff ')' Wff | epsilon

*/

#include <stdio.h>
#include <stdlib.h>

int lookahead;
int lines, characters;
void match(int);

void wff(void)
{
	if(lookahead == '('){
		match('('); printf("(");
		wff();
		match(')'); printf(")");
		wff();
	}
}

int next_token()
{
	int c;

	while(1){
		c = getchar();
		characters++;
		if( c == '\n'){
			lines++;
			characters = 0;
		}
		if( c == EOF) break;
		if( c == '(') break;
		if( c == ')') break;
	}
	return c;
}

void error()
{
	fprintf(stderr,"Unmatched parentheses. Line %d, character %d\n",
		lines,characters);
	exit(1);
}

void match(int foo){

	if(lookahead == foo){
		lookahead = next_token();
		return;
	}
	else error();
}

int main()
{
	while((lookahead = next_token())!= EOF)
		wff();
	return 0;
}
