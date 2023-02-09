/* Routines related to Dijkstra's algorthm for finding paths and 
distances */

#include "graph.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

/* dijkstra: determine the distance from v0 to each vertex in graph g. Return
   maximum distance from v0. 

For Dijkstra's algorithm, see p. 19 in Bondy and Murty.

*/

static int dijkstra_no_init; /* used by omega to turn off part of the
                                 initialization  */

int dijkstra(struct vertex_struct *v0, struct graph_struct *g)
{

	unsigned i,j=0,L,n,k,m,d=0;
	struct vertex_struct *v,*w; 
	struct edge_struct *e;

	if(!g)return -1;
	if(!v0)return -1;

	n = g->nvertices;

	/* We use color field to record distances from v0 */

	/* Initialize graph by setting colors to initial distance from
	   v0, i.e. 0 for v0 and "infinity" for everything else.  */

	for(i = 0; i < n; i++){
		if(!dijkstra_no_init)
			(g->vertices[i])->color = UINT_MAX;
		(g->vertices[i])->mark = 1;
	}
	(v0)->color = 0;
	(v0)->mark = 0;
	v = v0; /* v will be current vertex */

	while(j < n){

/* The cluster of distanced vertices grows by one each time. We use the mark
   field to keep track of which vertices are distanced. The color field
   records the distance.  */

		for(i=0;i<n;i++) {
			if((g->vertices[i])->mark){ /* Not yet distanced ? */
				w = g->vertices[i];
				for(k = 0; k < w->nedges; k++){
					e = (w->edges)[k];
					L = e->length;
			                if(v == OPPOSITE_END(e,w))
						w->color = 
                                                  w->color < (v->color)+L ?
						   w->color : (v->color)+L ;
				}
			}
		}
/* Find a minimum distance vertex in the not yet distanced set */
		m = UINT_MAX;
		for(i=0;i<n;i++)
			if((g->vertices[i])->mark)
				if((g->vertices[i])->color < m){
					m = (g->vertices[i])->color;
					w = g->vertices[i];
				}
		v = w; /* Add this vertex to the distanced set */
		w->mark = 0;
		j++;
	}
	for(i=0;i<n;i++)
		if((g->vertices[i])->color > d)
			d = (g->vertices[i])->color;
	if(debug){
		fprintf(stderr,"dijkstra: graph = %s, vertex = %s\n",
			g->name,v0->name);
		for(i=0;i<n;i++)
			fprintf(stderr,"vertex %s: distance = %u\n",
				(g->vertices[i])->name,
				(g->vertices[i])->color);	
	}
	if(debug)fprintf(stderr,"dijkstra returns %u\n",d);
	return d;

}

/* Return TRUE or FALSE according as graph is connected or not. Return -1
   in case of an error */

int is_connected(struct graph_struct *g)
{

	struct vertex_struct *v;

/* This relies on the fact that dijkstra fisrt sets distances to infinity, and 
   in a disconnected graph some distances will remain infinity after the
   algorithm */

	if(!g)return -1;
	if(g->nvertices <= 1)return 0;

	v = g->vertices[0];

	if(dijkstra(v,g) == UINT_MAX) return FALSE;
	return TRUE;

}

unsigned int diameter(struct graph_struct *g)
{

	unsigned int i,n,m=0,k;
	if(!g)return 0;
	n = g->nvertices;
	if(n<2)return 0;
	for(i=0;i<n;i++){
		k = dijkstra(g->vertices[i],g);
		if(k > m)m = k;
	}
	return m;
}

/* omega(G) returns the number of connected components of
   G.  We return -1 if there is an error */

int omega(struct graph_struct *g)
{

	unsigned n,m,i;
	struct vertex_struct *v;

	if(!g)return -1;

	n = g->nvertices;
	if(n==0)return 0;   /* We deem omega(empty graph) = 0 */
	v = g->vertices[0];

	/* dijkstra will mark distances from v in the connected component
           of v, and everything outside will be marked "infinity" */

	if(dijkstra(v,g) == 0) return 0;

	/* Subsequent calls to dijkstra must not reset distances in
           previously considered components to infinity. The purpose of this
           flag is to prevent this behavior */
	dijkstra_no_init = 1;

	m = 1; /* 1 connected component so far */
	while(m<n){ 
		/* see if any vertices untouched by dijkstra remain */
		for(i=0;i<n;i++)
		    if((g->vertices[i])->color == UINT_MAX)break;
		if(i==n)break; /* all components found */
		v = g->vertices[i];
		/* Use dijkstra again to mark out component of v */
		dijkstra(v,g);
		m++; /* count another component */
	}
	if(debug)fprintf(stderr,"omega: %s has %u connected components\n",
			g->name,m);
	dijkstra_no_init = 0; /* clean up */
	return m;
}
