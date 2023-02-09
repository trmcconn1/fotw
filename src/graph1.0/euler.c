/* Euler.c: routines related to Eulerian tours. An Eulerian tour traverses
every edge of a graph exactly once before returning to its starting
vertex */

#include<stdio.h>
#include<stdlib.h>
#include "graph.h"
#include<limits.h>

/* is_euler: return 1 if an euler tour exists for the given graph, 0
otherwise. It is well known (and due to Euler himself) that a tour exists
if and only if the graph is connected and every vertex has even degree */

int is_euler(struct graph_struct *g)
{
	int i,n;
	if(!g)return 0;
	if(!is_connected(g))return FALSE;
	n = g->nvertices;
	for(i=0;i<n;i++)
		if((g->vertices[i])->nedges % 2)return FALSE;
	return TRUE;
}


/* Construct an Euler Tour, if one exists, using Fleury's algorithm. 

The algorithm: select any available edge from the current vertex as long as
it is not a cut edge, i.e., an edge whose removal would disconnect the 
graph. Mark the edge chosen so that it cannot be chosen again when and if 
the tour returns to the current vertex. The algorithm ends when there is
no admissible choice of edge from the current vertex. 

Returns pointer to a walk if successful, NULL otherwise.

*/

struct walk_struct *fleury(struct graph_struct *g)
{

	int n,k,i,j,m;
	struct vertex_struct *v,*u;
	struct edge_struct *e;
	struct walk_struct *w;

	if(!g)return NULL;

	/* Determine how many edges there are so we can allocate a walk
           structure of the right size */

	k = g->nedges;
	w = alloc_walk(k);
	if(!w) return NULL;

	/* Unmark all edges and make sure their lengths are set to 1 in
	   case somebody has mucked with them */

	for(i=0;i<k;i++){
		(g->edges[i])->mark = 0;
		(g->edges[i])->length = 1;
	}
	
	/* Select vertex 0 of the graph as start ( = end ) of the euler 
           tour */

	v = g->vertices[0];
	w->start = v;

	j = 0; /* j will count how many edges have been traversed */

	while(j<k){

		/* See how many unmarked edges from this vertex. If only
                   one, we must take it */

		m = 0;
		for(i=0;i<v->nedges;i++)
			if((v->edges[i])->mark == 0)m++;

		if(m == 0) break; /* blocked at this vertex, so done */

		/* Go to first unmarked edge */

		for(i=0;i<v->nedges;i++)
			if((v->edges[i])->mark==0)break;

		e = v->edges[i];
		e->length = UINT_MAX/2;  
		u = OPPOSITE_END(e,v);

		if(m == 1){  /* Only one choice - follow it */
			e->mark = 1;
			v = u;
			*((w->edges)+j*sizeof(struct edge_struct *)) = e;
			j++;
			continue;
		}

	/* If there are multiple unmarked edges, find the first one that
           is not a cut edge and follow it */

		while(i<v->nedges){
			
			e = v->edges[i++];
			if(e->mark)continue;
			e->length = UINT_MAX/2;  
			u = OPPOSITE_END(e,v);
			dijkstra(v,g);
			if(u->color >= UINT_MAX/2){ /* It's a cut edge */
				e->length = 1;
				continue;
			}
		
		/* Ok to traverse e */

			e->mark = 1;  /* Leave long length on traversed edge */
			break;
		}
		if(i == v->nedges)break;  /* All cut edges. Blocked */
		v = u;
		*((w->edges)+j*sizeof(struct edge_struct *)) = e;
		j++;
	}
	if(j == k){
		w->length = k;
		if(!quiet){
			printf("Euler Tour for %s:\n",g->name);
			dump_walk(w);
			printf("\n");
		}
		return w;
	}
	else 
		if(!quiet)printf("%s does not have an Euler Tour\n",g->name);

	free_walk(w); 
	return NULL;
}
