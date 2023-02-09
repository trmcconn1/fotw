/* Common declarations for graph theory package */

#define MAX_GRAPHS 16     /* Maximum size of predefined graph library */
#define MAX_VERTICES 256 /* Maximum number of vertices in a graph */
#define MAX_EDGES 256
#define TRUE 1
#define FALSE 0


struct vertex_struct {
	char *name;    /* Name of vertex, if it has one */
	unsigned color;    /* Used for marking by various algorithms */
	char mark;
	unsigned nedges;   /* The degree of the vertex */
	struct edge_struct *edges[MAX_EDGES]; 
};

struct edge_struct {

	char *name;    /* Name of edge, if it has one */
	struct vertex_struct *start;
	struct vertex_struct *end;
	char mark;
	unsigned color;    /* Used for marking by various algorithms */
	unsigned length;
};

struct graph_struct {
	char *name;
	unsigned nvertices;
	unsigned nedges;
	struct vertex_struct *vertices[MAX_VERTICES];
	struct edge_struct *edges[MAX_EDGES];
	char mark;
	char directed;
} my_graphs[MAX_GRAPHS];

struct walk_struct {
	int length;
	struct vertex_struct *start;
	struct edge_struct **edges;
};





#define OPPOSITE_END(e,v)  ( (e)->start == (v) ? (e)->end : (e)->start )  

/* Defined in util.c */
extern struct vertex_struct *creat_vertex(char *);
extern struct edge_struct *creat_edge(char *);
extern struct vertex_struct *search_vertex(struct graph_struct *, char *);
extern void dump_info(struct graph_struct *);

/* Defined in dijkstra.c */
extern int dijkstra(struct vertex_struct *, struct graph_struct *);
extern int omega(struct graph_struct *);

/* Defined in euler.c */
int is_euler(struct graph_struct *);

/* Defined in main.c */
extern int debug;
extern int quiet;
extern int ngraphs;



