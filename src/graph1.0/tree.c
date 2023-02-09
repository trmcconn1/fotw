/* tree.c: routines related to trees */

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<limits.h>

#include "graph.h"

/* is_tree: test whether the graph is a tree or not. 
A tree is a connected graph that contains no cycles. If true is
returned, then the color field of each vertex gives the "depth" of the
vertex in the tree, i.e., the number of levels below from the root 
*/


int is_tree( struct graph_struct *g)
{

	int i,n,m,j;
	struct vertex_struct *v,*u;
	struct edge_struct *e;

	if(!g) return FALSE; /* How can it be a tree if it's not anything? */
	if(!is_connected(g))return FALSE;
	n = g->nvertices;

/* Initialize the color of each vertex to 0. This will become the level in
tree variable when we try to see if this is a tree.
*/
	for(i=0;i<n;i++)
		(g->vertices[i])->color = 0;

	(g->vertices[0])->color = 1;

	for(m=1;m<n;m++){  /* sum of possible tree levels */
		for(i=0;i<n;i++){ /* select level m vertices */
			v = g->vertices[i];
			if(v->color != m)continue;
			for(j=0;j<v->nedges;j++){
				e = v->edges[j];
				u = OPPOSITE_END(e,v);
				if(u->color == 0) {
					u->color  = m + 1;
					continue;
				}
				if(u->color == m-1)continue; /* skip parent */
				return FALSE;
			}
		}
	}
	return TRUE;
}
				

void dump_tree(struct graph_struct *g){

	unsigned i,n,m=0;
	unsigned j = 0;
	if(!g)return;
	if(!is_tree(g)){
		printf("%s is not a tree\n",g->name);
		return;
	}
	n = g->nvertices;
	for(m=1;m<n;m++){
		printf("lvl %d { ",m);
		for(i=0;i<n;i++){
			if((g->vertices[i])->color == m){
				printf("%s ",(g->vertices[i])->name); 
				j++;
			}
		}
		printf(" }\n");
		if(j == n)break; 
	}
	return;
}
				
/* prim: Use Prim's algorithm to construct a minimal spanning tree
(with respect to length of edges) for the given graph.
The result is a new graph added to the array of all graphs
whose name is the name of g with an appropriate addendum.

Prim's algorithm is a greedy algorithm that grows a tree from
the minimal length edge by adding the next remaining minimal edge to the 
growing tree. Returns FALSE if failure, TRUE if success  */

int prim(struct graph_struct *g){

	struct edge_struct *e,*f;
	struct vertex_struct *v,*u,*temp;
	struct graph_struct *tree;
	unsigned i,j,n,m,k;

	if(!g)return FALSE;
	n = g->nedges;
	if(!n)return TRUE; /* If no edges, then done */

	/* initialize the result tree */
	tree = &(my_graphs[ngraphs++]);
	tree->nvertices = 0; /* this will increase */ 
	tree->name = (char *)malloc(strlen(g->name)+16);
	strcpy(tree->name,g->name);
	strcat(tree->name,"_min_span_tree");
	k = 0;  
	tree->nedges = k; /* this will increase */

	/* initialize marks on edges of g to zero */
	for(i=0;i<n;i++)(g->edges[i])->mark = 0;

	/* Find the minumum length edge among those that are
        not yet marked and share a vertex with a marked edge */

	m = UINT_MAX/2;

	while(k < g->nvertices - 1){/*Thm: mst of graph has n - 1 edges */
	for(i=0;i<n;i++){ /* loop over all edges to find suitable one */
		e = g->edges[i]; /* trying this edge */
		if(k==0){ /* first time through, just hunt for shortest edge */
			if(e->length<m)m = e->length;
			continue;
		}
	/* after first pass, limit attention to smaller candidate set */
		if(e->length + 1 > m)continue; /* not a candidate for record */
		if(e->mark)continue; /* in the tree already */
		u = e->start; /* check if either end is in the tree */
		v = e->end;
		for(j=0;j<u->nedges;j++){
			f = u->edges[j];
			if(f->mark == 1){
/* code below assumes u has the vertex not currently in the tree, so we
                  must swap u and v */
				temp = u;
				u = v; 
			        v = temp;
				f = e;
				break;
			}
		}
		if(j < u->nedges){
			m = e->length; 
			continue;
		}
		/* Do the same check for the other end of e */
		for(j=0;j<v->nedges;j++){
			f = v->edges[j];
			if(f->mark == 1){
				f = e;
				break;
			}
		}
			
		if(j < v->nedges){
			m = e->length;
			continue;
		}
	}
	/* add both ends as vertices on first pass, else add vertex
         of e that is not already in graph (saved as u from above) */
	if(k==0){
		 v = e->start;
		 u = e->end;
		 u = creat_vertex(u->name);
		 v = creat_vertex(v->name);
		 tree->vertices[0]=u;
		 tree->vertices[1]=v;
		 tree->nvertices=2;
		 f = e;
	}
	else {
		u = tree->vertices[tree->nvertices] = creat_vertex(u->name);
		v = search_vertex(tree,v->name);
		tree->nvertices++;
	}
	f->mark = 1; /* mark e as in the tree */
	f = creat_edge(f->name);
	f->start = v;
	f->end = u;
	v->edges[v->nedges] = f;
	u->edges[u->nedges] = f;
	(v->nedges)++;
	(u->nedges)++;
	tree->edges[k++] = f; /* formally add edge to tree */
	tree->nedges = k;
	m = UINT_MAX;  /* reset the bar */
	}

	return TRUE;
}
				
/* kruskal: Use Kruskal's algorithm to construct a minimal spanning tree
(with respect to length of edges) for the given graph.
The result is a new graph in the array of all graphs
whose name is the name of g with an appropriate addendum.

Kruskal's algorithm is a greedy algorithm similar to Prim's algorithm,
except (a) the edge added at each stage can be from anywhere in the graph,
and (b) the edge added is the one of minimal length which does not form
a cycle when included with any edges added so far.  This results in a tree
once all vertices of g are included, but edges can be added along the way
such that the "tree" constructed so far is disconnected.

Returns FALSE if failure, TRUE if success.  
*/

int kruskal(struct graph_struct *g){

	struct edge_struct *e,*f;
	struct vertex_struct *v,*u,*temp;
	struct graph_struct *tree;
	unsigned i,j,n,m,k;

	if(!g)return FALSE;
	n = g->nedges;
	if(!n)return TRUE; /* If no edges, then done */

	/* initialize the result tree */
	tree = &(my_graphs[ngraphs++]);
	tree->nvertices = 0; /* this will increase */ 
	tree->name = (char *)malloc(strlen(g->name)+16);
	strcpy(tree->name,g->name);
	strcat(tree->name,"_min_span_tree");
	k = 0;  
	tree->nedges = k; /* this will increase */

	/* initialize marks on edges of g to zero */
	for(i=0;i<n;i++)(g->edges[i])->mark = 0;

	/* Find the unmarked minumum length edge
        that does not form a cycle if added to the tree */

	m = UINT_MAX/2;

	while(k < g->nvertices - 1){/*Thm: mst of graph has n - 1 edges */
	for(i=0;i<n;i++){ /* loop over all edges to find suitable one, f */
		e = g->edges[i]; /* trying this edge */
		if(k==0){ /* first time through, just hunt for shortest edge */
			if(e->length<m){
		             m = e->length;
			     f = e;
			}
			continue;
		}

	/* after first pass, limit attention to smaller candidate set */

		if(e->length + 1 > m)continue; /* not a candidate for record */
		if(e->mark)continue; /* in the tree already */
		u = e->start; 
		v = e->end;

/* If either vertex at the end of e (or both) is not currently in the tree, then
this edge is a candidate to be added */

		if( !(u=search_vertex(tree, u->name))){
			m = e->length;
			f = e;
			continue;
		}
		if( !(v=search_vertex(tree, v->name))){
			m = e->length;
			f = e;
			continue;
		}
/* Here's the tricky point: even if both ends of e lie on the "tree", the
  edge might still be admissable if the two ends lie in different components.
  The ends are in different components if the distance between them (w/o 
adding e,) as computed by djikstra, is infinite.
*/
		dijkstra(u,tree); /* fill color fields with distances */
		if(v->color == UINT_MAX){
			m = e->length;
			f = e;
			continue;
		}
		continue;
	}
			
/* a copy of edge f should now be added to the "tree", along with one or
   both vertices. We must be careful to connect newly created vertices
and edges properly. */
	 
	f->mark = 1; /* skip over e in subsequent searches */

	v = f->start;
	u = f->end;
	if(!(temp = search_vertex(tree,v->name))){
		v = creat_vertex(v->name);
		(tree->vertices)[(tree->nvertices)++] = v;
	}
	else v = temp;
	if(!(temp = search_vertex(tree,u->name))){
		u = creat_vertex(u->name);
		(tree->vertices)[(tree->nvertices)++] = u;
	}
	else u = temp;
	f = creat_edge(f->name);
	f->start = v;
	f->end = u;
	v->edges[(v->nedges)++] = f;
	u->edges[(u->nedges)++] = f;
	tree->edges[k++] = f; /* formally add edge to tree */
	tree->nedges = k;
	m = UINT_MAX;  /* reset the bar for next round */
	}

	return TRUE;
}
