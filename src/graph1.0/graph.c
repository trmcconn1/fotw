/*  readdef module of my programs for analyzing graphs: read graph defs
    from file(s)  */



#include <stdio.h>
#include <stdlib.h>
#include "parse.h"
#include "graph.h"
#include <string.h>

#define COMMAND 0
#define DEFINITION 1

struct edge_struct *creat_edge(char *);
struct vertex_struct *creat_vertex(char *);
struct vertex_struct *search_vertex(struct graph_struct *, char *);
void dump_info(struct graph_struct *);
void dump_def(struct graph_struct *,FILE *);
int cmp_vertices_by_degree( struct vertex_struct *a, struct vertex_struct *b);
int delete_graph(int);

extern int quiet;
extern int debug;
extern char directed; /* default: treat graphs as undirected */
                          /* change with -d option */
			  /* BUG: should be per graph. */
int ngraphs=0;

struct graph_struct my_graphs[MAX_GRAPHS];


int do_readdefs(int argc, char **argv)
{
	int status = MY_NORMAL;
	char *p;
	int i,j=0;
	char buffer[256];
	char state = COMMAND;
	int nvertices=0;
	int ngraphsold;
	int nedges=0;      /* total edges in current graph */
	int newvertex = 1;  
	struct vertex_struct *current_vertex,*q;
	struct edge_struct *current_edge;
	struct graph_struct *current_graph;
	FILE *istream;


	while(j<argc){
		ngraphsold = ngraphs;
		istream = fopen(argv[j],"r");
		if(!istream){
			fprintf(stderr,"readdefs: cannot open %s\n",argv[j]);
			j++;
			continue;
		} 
	status = MY_NORMAL;
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

				if(debug)fprintf(stderr,"Create Graph: %s\n",p);
				if(ngraphs >= MAX_GRAPHS){
					fprintf(stderr,"readdefs: no more graph slots\n");
					return -1;
				}	
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
				if(!current_graph->directed && (q != 
                                         current_vertex)){
					q->edges[q->nedges] = current_edge;
					(q->nedges)++;
				}
				nedges++;
				break;

			default:
				break;
	
		}
                                          
	} /* loop back to next input line */
	if(!quiet)printf("Defined %d graphs from %s\n",ngraphs - ngraphsold,
		argv[j]);
	fclose(istream);
	j++;
	}
	if(!quiet) printf("Total number of defined graphs = %d\n",ngraphs);
	return 0;
}


int delete_graph(int i)
{
	int j;
	if(i>=ngraphs)return FALSE;
	ngraphs--;
	for(j=i;j<ngraphs;j++)my_graphs[j]=my_graphs[j+1];
	return TRUE;
}

void dump_def(struct graph_struct *g, FILE *ostream)
{

	int n,i;
	if(!g)return;

	fprintf(ostream,"%s { ",g->name);
	
	n = g->nedges;

	for(i=0;i<n;i++)
		fprintf(ostream,"%s %s; ",((g->edges[i])->start)->name,
			((g->edges[i])->end)->name);

	fprintf(ostream,"}\n");
		
}

/* Make an exact copy of the given graph, appropriately renamed  
The result is a new graph added to the array of all graphs.*/

int copy_graph(struct graph_struct *g){

	struct edge_struct *e,*f;
	struct vertex_struct *v,*u,*temp;
	struct graph_struct *copy;
	unsigned i,j,n,m,k;

	if(!g)return FALSE;

	/* initialize the result tree */
	copy = &(my_graphs[ngraphs++]);
	copy->name = (char *)malloc(strlen(g->name)+7);
	strcpy(copy->name,g->name);
	strcat(copy->name,"_copy");
	n = g->nvertices;
	for(i=0;i<n;i++){
		u = (g->vertices[i]);
		v = (copy->vertices)[i] = creat_vertex(u->name); 
		v->color = u->color;
		v->mark = u->mark;
	}
	copy->nvertices = n;
	for(i=0;i<g->nedges;i++){
		f = g->edges[i];
		e = creat_edge(f->name);
		(copy->edges)[i] = e;
		e->length = f->length;
		e->mark = f->mark;
		e->color = f->color;
		e->start = search_vertex(copy,(f->start)->name);
		e->end = search_vertex(copy,(f->end)->name);
	}
	copy->nedges = g->nedges;
	for(i=0;i<n;i++){
		v = (g->vertices[i]);
		u = (copy->vertices)[i]; 
		m = u->nedges = v->nedges;
		for(j=0;j<m;j++){
			e = (v->edges)[j];
			for(k=0;k<m;k++){
				f = (copy->edges)[k];
				if(strcmp(f->name,e->name)==0)break;
			}
			(u->edges[j] = f);
		}
	}
	return TRUE;
}

