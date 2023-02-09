/* poly.h: data structures for modelling finite (compact) simplicial 
 * complexes */

#define MAX_VERTICES 0xFFFFFFFF
#define MAX_FACETS 1024
#define MAX_RAMIFY 1024
#define MAX_SIMPLICES 0xFFFF
#define MAX_COMPLEXES 0xFF
#define MAX_NAME 256
#define TRUE 0
#define FALSE 1

/* spx: A simplex structure */
struct spx {
		int p; /* number of vertices = dimension + 1 */
		int n_facets;
		int n_parents;
		int id;
		char *name;
		struct spx *facets[MAX_FACETS]; /* Null for vertices */
		struct spx *parents[MAX_RAMIFY]; /* Null for top level */
};

/* Simplicial complex structure */

struct scpx {
		int n; /* its total size, in simplices */
		char *name;
		int id;
		struct spx *simplices[MAX_SIMPLICES]; /* one needn't point
							 at all subsimplices,
							 only the top level
							 ones */
};


/* Functions implemented in spx.c */

extern void dump_simplex(struct spx *);
extern struct spx *alloc_simplex(int dim,char *);
extern struct scpx *alloc_complex(char *);
extern struct scpx *attach_complex(struct scpx *, struct spx *,
	struct scpx *, struct spx *);
extern struct scpx *attach_simplex_to_complex(struct spx *s, struct spx *a, struct spx *b, struct scpx *c);
extern void free_simplex(struct spx *);
extern void dump_complex(struct scpx *);
extern void collect_garbage(void);
extern void status(void);
