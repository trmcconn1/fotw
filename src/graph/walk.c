/* walk.c: routines having to do with walks, paths, circuits, ... */

#include <stdlib.h>
#include <stdio.h>
#include "graph.h"

/* We use terminology from Bondy and Murty. A walk is a list of the form
v1e1v2e2...envn of alternating vertices and edges between them that represents
a journey through a graph that starts at vertex v1 and ends at vn. 

key declaration from graph.h: 
struct walk_struct {
	int length;
	struct vertex_struct *start;
	struct edge_struct **edges;
}
 
*/


/* Allocate space for a walk of length n and return a pointer to it */

struct walk_struct *alloc_walk(int n)
{
	struct walk_struct *w;
	
	w = (struct walk_struct *)malloc(sizeof(struct walk_struct));
	if(!w) return NULL;

	w->edges = (struct edge_struct **)malloc(n*sizeof(struct edge_struct *));
	if(!w->edges){
		free(w);
		return NULL;
	}
	return w;	
}

void free_walk(struct walk_struct *w)
{
	if(!w)return;
	free(w->edges);
	free(w);
	return;
}

/* Make sure a walk is valid and return a pointer to its last vertex, or
   NULL if the walk is not valid */

struct vertex_struct *test_walk(struct walk_struct *w)
{
	int i,n;
	struct vertex_struct *v;
	struct edge_struct *e;

	if(!w)return NULL;

	v = w->start;
	e = *(w->edges);
	n = w->length;
	
	for(i=0;i<n;i++)
		if(e->start == v)
			v = e->end;
		else if(e->end == v)
			v = e->start;
		else return NULL;
	return v;
}

	
/* Print steps of a walk in human readable form */

void dump_walk( struct walk_struct *w)
{

	int i,n;
	struct edge_struct *e;
	struct vertex_struct *v;

	if(!w)return;
	n = w->length;
	if(!n)return;

	e = *(w->edges);
	v = w->start;

	for(i=0;i<n;i++){
		if(e->start == v){
			printf(":%s->%s",v->name,(e->end)->name);
			v = e->end;
		}
		else {	
			printf(":%s->%s",(e->end)->name,v->name);
			v = e->start;
		}
		e += sizeof(struct edge_struct *);
	}
	return;
}

/* Tests for various special types of walk */
		
