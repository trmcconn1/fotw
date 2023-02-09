/* tableau_cgi.c: cgi interface for a parser and prover for sentential logic   

	By Terry R. McConnell (trmcconn@syr.edu)

*/

#include "global.h"
#include "parse.h"
#define MAX_FILENAME 256

#define PROGNAME "tableau"
#define VERSION "1.0"
#define USAGE "tableau [ -sctadvhV] [ source files ... ]"

#define HELP "-h: print this helpful message\n\
-v: print version number and exit.\n\
-V: be more verbose.\n\
-t: display truth tables. \n\
-d: dump parse trees. \n\
-a: display analytic tableaux. \n\
-s: make a separate tableau for each source line.(override normal behavior)\n\
-c: combine contents of separate files\n\
Parse and analyze sentential logic wff(s) in file[s] (or stdin) \n\n"

/* Banner: print name of program etc */

void banner()
{			
	printf("\n\t\t\t%s( %s )\n\n",PROGNAME,VERSION);
}

/* error routine */

void internal_error(char *msg){

	fprintf(stderr,"%s",msg);
	exit(1);
}

/* The main program */

int tableau_cgi(char *wff, char *option, int sign)
{

	int rval;
	int dump_on = 0;
	int truthtable_on = 0;
	int tableau_on = 0;
	int n;
	struct treenode **node_array;
	int verbosity = 0;
	int separate_flag = 0;
	int combine_flag = 0;
	int nfiles = 0;
	int err_flag = 0;
	int do_run = 1;
	int i=1,j,k;
	int error_code = OK;
	FILE *current_input = stdin;
	char input_filename[MAX_FILENAME] = "stdin";
	struct treenode *parse_tree,*p; 
	struct treenode *expressions[MAX_EXPRESSIONS];
	int signs[MAX_EXPRESSIONS];
	int nn; 


	/* Process option */

		switch(option[1]){
		case 'v':
			printf("%s\n",VERSION);
			return 0;
		case 'V':
			verbosity = 1;
			break;
		case 'h':
			printf("\n%s\n\n",USAGE);
			printf("%s\n",HELP);
			return 0;
		case 't':
			truthtable_on = 1;
			break;
		case 's': 
			separate_flag = 1;
			break;
		case 'c':
			combine_flag = 1;
			break;
		case 'a':
			tableau_on = 1;
			break;
		default:
			fprintf(stderr,"%s:%s\n",PROGNAME,USAGE);
			return 1;
		}

	if(verbosity)banner();

	current_input = tmpfile();
	if(!current_input){
		fprintf(stderr,"%s: cannot open tempfile\n",PROGNAME);
		return 1;
	}
	line = 0;
	fprintf(current_input,"%s\n",wff);
	rewind(current_input);
	parse_tree = parse(current_input,&error_code,&sign);
	set_parent_pointers(parse_tree);
	if(truthtable_on)
		truth_table(parse_tree);
	if(tableau_on){
		expressions[0] = parse_tree;
		signs[0]=sign;
		if(p=tableau(expressions,signs,1))
			dump_tableau(p);
	}
	return 0;
}

int parenthesy(int style,struct treenode *l, struct treenode *p, 
		struct treenode *r)
{

	if(style == PEDANTIC)return 1;
	if(p->precedence > precedence(p->type))return 1;
	else return 0;
}


int pretty_print(struct treenode *t, int style,int print)
{

		struct treenode *l,*r;
		int n=0,pl,p0,pr;

		if(!t)return;
		if(t->type==LEAF){
			if(print)printf("%s",t->name);
			n+=strlen(t->name);
			return n;
		}

		r = t->right;
		l = t->left;


		if(parenthesy(style,l,t,r)){
			if(print)printf("("); 
			n++;
		}

		/* Print stuff to the left of us */
		n += pretty_print(l,style,print);

		/* Print self */
		if(print)printf("%s",written_as[t->type]);
		n += strlen(written_as[t->type]);

		/* print stuff to the right of us */
		n += pretty_print(r,style,print);

		if(parenthesy(style,l,t,r)){
			if(print)printf(")"); 
			n++;
		}
		return n;
}
