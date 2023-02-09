/* Auxiliary routines */


#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "graph.h"

	
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

}
