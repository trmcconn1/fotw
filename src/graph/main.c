/* Main module of my programs for analyzing graphs */


/* Build: Edit makefile for your system and 
do make graph in this directory.  For usage info type

graph  -h

at the shell prompt. Typical usage would be

*/ 


#include <stdio.h>
#include <stdlib.h>
#include "parse.h"
#include "graph.h"
#include <string.h>

#define COMMAND 0
#define DEFINITION 1

#define OPSEP '-'
#define USAGE "graph [-q -h -v -d]"
#define HELP0 "read graph definitions from stdin"
#define HELP1 "-q: quiet(er) operation"
#define HELP2 "-h: print this helpful message and exit"
#define HELP3 "-v: print version number and exit"
#define HELP4 "-d: treat graphs as digraphs"
#define VERSION "1.0"

struct edge_struct *creat_edge(char *);
struct vertex_struct *creat_vertex(char *);
struct vertex_struct *search_vertex(struct graph_struct *, char *);
void dump_info(struct graph_struct *);
int cmp_vertices_by_degree( struct vertex_struct *a, struct vertex_struct *b);

int quiet;
int debug = 1;
static char directed = 0; /* default: treat graphs as undirected */
                          /* change with -d option */
			  /* BUG: should be per graph. */
int ngraphs=0;

int main(int argc, char **argv)
{
	int status = MY_NORMAL;
	char *p;
	int i;
	char buffer[256];
	char state = COMMAND;
	int nvertices=0;
	int nedges=0;      /* total edges in current graph */
	int newvertex = 1;  
	struct vertex_struct *current_vertex,*q;
	struct edge_struct *current_edge;
	struct graph_struct *current_graph;

	FILE *istream = stdin;

	/* Process Command Line */

	if(argc > 1) {
		while(argc > 1)
		  if((*++argv)[0]==OPSEP)
			switch((*argv)[1]){
				case 'h':
					printf("\n%s\n",USAGE); 
					printf("\n%s\n",HELP0);
					printf("%s\n",HELP1);
					printf("%s\n",HELP2);
					printf("%s\n",HELP3);
					printf("%s\n",HELP4);
					printf("\n");
					argc--;
					exit(1);
					break;
				case 'v':
					printf("%s\n",VERSION);
					exit(1);
					break;  /* reminder */
				case 'q':
					argc--;
					quiet = 1;
					break;
				case 'd':
					argc--;
					directed = 1;
					break;
				default:
					fprintf(stderr,"graph: Invalid flag\n");
					fprintf(stderr,"%s\n",USAGE);
					exit(1);
			}
		else {
	        	if(!(istream = fopen(*argv,"r"))){
				fprintf(stderr,"graph: error opening %s\n",*argv);
				exit(1);
 
			}
			argc--;
		}
	}

	while(status != MY_EOF){ /* loop over lines of input file */

		p = parse(istream,&status);
 
		/* skip extraneous EOL characters (mainly for DOS
			compatibility) */
		while(status == MY_EOL){
			p = parse(istream,&status);
			newvertex = 1;
		}
		if(status == MY_EOF)break;

		if(status == STATE_CHANGE){  
			nedges = 0; nvertices = 0; newvertex = 1;
			/* Toggle */
			if(state == DEFINITION) state = COMMAND;
			else state = DEFINITION;
			continue;
		}

		switch(state){

			case COMMAND:

/* Later: handle command keywords here */

				if(debug)fprintf(stderr,"Create Graph: %s\n",p);
				current_graph = &(my_graphs[ngraphs++]);	
				current_graph->name = (char *)malloc(strlen(
					p)+1);
				
				strcpy(current_graph->name,p);
				current_graph->nvertices = 0;
				current_graph->nedges = 0;
				current_graph->directed = directed;
				nvertices = 0;
				nedges = 0;
				newvertex = 1;
				break;
				
			case DEFINITION:
				if(newvertex){ /* start of line: p = vertex */
					newvertex = 0;
					q = search_vertex(current_graph,p);
					if(!q){
					current_vertex = creat_vertex(p);
					current_graph->vertices[nvertices++]
					    = current_vertex;
					(current_graph->nvertices)++;
					} /* else vertex already exists */
					else current_vertex = q;
					break;
				}
				/* After creating a vertex named by the
                                 first field, treat any remaining fields as far
                                 ends of edges for this vertex */
				/* build an edge name */
				strcpy(buffer,current_vertex->name);
				if(current_graph->directed)
					strcat(buffer,"->");
				else strcat(buffer,"-");
				strcat(buffer,p);
				current_edge = creat_edge(buffer);

				/* Do we need to create a vertex? */
				q = search_vertex(current_graph,p);
				if(!q){
					q = creat_vertex(p);
					current_graph->vertices[nvertices++]
					        = q;
					(current_graph->nvertices)++;

				}
				current_edge->start = current_vertex;
				current_edge->end = q;
				current_graph->edges[nedges] = current_edge;
				(current_graph->nedges)++;
				current_vertex->edges[current_vertex->nedges] =
					current_edge;
				(current_vertex->nedges)++;
				if(!current_graph->directed){
					q->edges[q->nedges] = current_edge;
					(q->nedges)++;
				}
				nedges++;
				break;

			default:
				break;
	
		}
                                          
	} /* loop back to next input line */

/*
	for(i=0;i<ngraphs;i++)
		 dump_info(&(my_graphs[i]));
*/
/*
	for(i=0;i<ngraphs;i++)
		 dijkstra(my_graphs[i].vertices[0],&(my_graphs[i]));
*/
		
	for(i=0;i<ngraphs;i++)
		omega(&(my_graphs[i]));

	return 0;
}
