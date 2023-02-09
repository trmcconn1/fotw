/* Auxiliary routines */


#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "graph.h"

/* Note that we have no free vertex or free edge routine. This is because
   edges and vertices should not be destroyed since they can be shared
   among more than 1 graph
*/
	
/* Create and initialize a vertex structure */

struct vertex_struct *creat_vertex(char *name)
{
	struct vertex_struct *v;

	if(!name) return NULL;
	v = (struct vertex_struct *)malloc(sizeof(struct vertex_struct));
	if(!v)return NULL;
	v->name = (char *)malloc(strlen(name)+1);
	if(!v->name){
		free(v);
		return NULL;
	}
	strcpy(v->name,name);
	if(debug)fprintf(stderr,"Create Vertex: %s\n",name);
	v->nedges = 0;
	v->color = 0;
	v->mark = 0;
	return v;
}

/* Create and initialize an edge structure */

struct edge_struct *creat_edge(char *name)
{
	struct edge_struct *e;

	e = (struct edge_struct *)malloc(sizeof(struct edge_struct));
	if(!e)return NULL;
	if(name){
		e->name = (char *)malloc(strlen(name)+1);
		strcpy(e->name,name);
	}
	if(debug)fprintf(stderr,"Create Edge: %s\n",name);
	e->color = 0;
	e->mark = 0;
	e->length = 1;
	return e;
}
	


int cmp_vertices_by_degree( struct vertex_struct *a, struct vertex_struct *b)
{
	return a->nedges < b->nedges ? 0 : 1;
}

/* For now: just a linear search */

struct vertex_struct *search_vertex(struct graph_struct *g,char *name)
{
	int i;

	if(g == NULL)return NULL;
	for(i=0;i<g->nvertices;i++)
		if(strcmp((g->vertices[i])->name,name)==0)
			return g->vertices[i];
	return NULL;
}

void dump_info(struct graph_struct *g)
{
	int i;
	printf("Name: %s %u vertices, %u edges\n degree vector = <",g->name,g->nvertices,
		g->nedges);
	/* Sort vertices by degree */
	qsort(&(g->vertices[0]), g->nvertices, sizeof(struct vertex_struct *),
		(int (*)(const void *, const void *))&cmp_vertices_by_degree);
	for(i=0;i<g->nvertices;i++)
		printf(" %s[%u] ",(g->vertices[i])->name,
			(g->vertices[i])->nedges);
	printf(">\n");
	if((i=omega(g)) == 1){
		printf("%s is connected\n",g->name);
		printf("%s has diameter %u\n",g->name,diameter(g));
	}
	else printf("%s has %d components\n",g->name,i);
	dump_tree(g);
}

/* same_edge: true if {start,end} are equal as sets for the two edges. 
Such edges may have multiple addresses to allow for non-simple graphs */

int same_edge(struct edge_struct *u, struct edge_struct *v)
{
	if((u->start == v->start)&&(u->end == v->end))return TRUE;
	if((u->start == v->end)&&(u->end == v->start))return TRUE;
	return FALSE;
}

/* Determine if g is simple or not. A simple graph is one without multiple
edges between the same set of vertices and without any edges that loop back
to the same vertex . Returns -1 for error */

int is_simple(struct graph_struct *g)
{
	int i,j,n;
	struct edge_struct *e;

	if(!g)return -1;
	n = g->nedges;
	for(i=0;i<n;i++){
		e = g->edges[i];
		if(e->start == e->end)return FALSE;
		for(j=0;j<i;j++)
			if(same_edge(e,g->edges[j]))return FALSE;
	}
	return TRUE;	
}
	
