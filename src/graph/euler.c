/* Euler.c: routines related to Eulerian tours. An Eulerian tour traverses
every edge of a graph exactly once before returning to its starting
vertex */

#include<stdio.h>
#include<stdlib.h>
#include "graph.h"

/* is_euler: return 1 if an euler tour exists for the given graph, 0
otherwise. It is well known (and due to Euler himself) that a tour exists
if and only if every vertex has even degree */

int is_euler(struct graph_struct *g)
{
	int i,n;
	if(!g)return 0;
	n = g->nvertices;
	for(i=0;i<n;i++)
		if((g->vertices[i])->nedges % 2)return FALSE;
	return TRUE;
}
