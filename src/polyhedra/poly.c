/* poly.c: implement simplices and simplicial complexes */

/* Finite simplicial complexes can topologically model arbitrarily complex
 * surfaces, but are themselves simple data structures ideal for computer
 * manipulation. Conceptually, a simplicial complex is a set of finite
 * sets (called simplices,) each made of vertices. A vertex is also a 0-simplex.
 * Any subset of a simplex is again an element of the complex, and this
 * is the only rule! 
 *
 * In general complexes can be infinite, and the number of simplices 
 * containing a given simplex can be infinite. We consider, of course,
 * only finite ones. They are, equivalently, compact in the weak topology.
 */

#include "poly.h"
#include <stdio.h>
#include <stdlib.h>

int next_id = 0;
int n_complexes = 0;

static struct spx *get_vertex(struct spx *, int );
static int has_simplex(struct spx *, struct scpx *);
static int has_sub_simplex(struct spx *, struct spx *);
static int attach_simplex(struct spx *, struct spx *);


/* Simplex Registry: Used to keep track of all allocated simplices for
   the purpose of garbage collection */

static struct spx *simplex_registry[MAX_SIMPLICES];


/* Complex Registry: Same, but for complexes */

static struct scpx *complex_registry[MAX_COMPLEXES];


/* Enter simplex a in first non-void registry slot. */

static void register_simplex(struct spx *a)
{
	int i;
	if(a != NULL){
		for(i=0;i<MAX_SIMPLICES;i++)
			if(simplex_registry[i]==NULL) break;
		if(i == MAX_SIMPLICES)
			fprintf(stderr,"register_simplex: simplex registry full.\n");
		else simplex_registry[i] = a;
	}
}

/* Allocate a simplex of dimension dim. Return NULL if it fails. 
*/

struct spx *alloc_simplex(int dim,char *p)
{
	struct spx *a,*t;
	long k,j,l;
	char buf[MAX_NAME];
	struct spx *(*my_simplices)[]; /* Pointer to array of pointers to
                                          simplices. Gotta love C! */
	int i;

	/* Calculate 2^(dim+1), 1 + the number of subsimplices needed */
	/* We don't create an empty simplex */
	k = 1L;
	for(i=0;i<=dim;i++) k *= 2;

	if(k > MAX_SIMPLICES){
		fprintf(stderr,"alloc_simplex: too many simplices.\n");
		return NULL;
	}
		
	/* Allocate room for an array of vertices */

	my_simplices = (struct spx *(*)[])malloc(k*sizeof(struct spx *));
	if(my_simplices == NULL) return NULL;

	/* Create and initialize the sub simplices of our simplex */

	for(j=1L;j<k;j++){
		if( (a = (*my_simplices)[j] = (struct spx *)malloc(sizeof(struct spx))) == NULL)
			return NULL;

		a->p = 0;
		a->id = next_id++;
		a->n_facets = 0;
		a->n_parents = 0;
		a->facets[0]=NULL;
		if(j<k-1 || (p==NULL)) { /* Top level name handled below */
			sprintf(buf,"%d",a->id);
			a->name = (char *)malloc((strlen(buf)+1)*sizeof(char));
			if(a->name == NULL)return NULL;
			strcpy(a->name,buf);
		}
		if(j == k-1) a->p = dim+1; /* Others are set below */
		register_simplex(a);
	}

	/* Give top level simplex (index = k-1) a name, if need be */

	if(p){
		a = (*my_simplices)[k-1L];
		sprintf(buf,"%s",p);
		a->name = (char *)malloc((strlen(buf)+1)*sizeof(char));
		if(a->name == NULL)return NULL;
		strcpy(a->name,buf);
	}

	/* Now go through the whole array and set the facet and parent
           pointers appropriately. This is all done with bit-diddling on
	   the index: The binary digit structure of the indices mirrors the
           structure of the simplex */

	for(j=1L;j<k-1;j++){

		a = (*my_simplices)[j];

		/* Set parent pointers */
		
		l = 1L;
		for(i=0;i<=dim;i++){
			t = (*my_simplices)[j|l];
			if(t != a){
				t->facets[t->n_facets++]=a;
				a->parents[a->n_parents++]=t; 
			}
			l = l<<1;
		}
		if(a->n_facets>0)
			a->p = a->n_facets;
		else a->p = 1;  /* 0-simplex has 1 point, no faces */
	}
	return (*my_simplices)[k-1L];  /* Return top-level simplex */
}

/* get vertex: returns pointer to vertex of index n of simplex s. It
   assumes vertex n is across from facet n.  */

struct spx *get_vertex(struct spx *s, int n)
{
	struct spx *p;

	if(s == NULL) return NULL;
	if(s->p <= n) return NULL;
	if(n < 0) return NULL;

	/* 0 <= n < p */

	if(s->p == 1) return s; /* already vertex n = 0 */

	if(n == 0) p = s->facets[1];
	else p = s->facets[0];

	return get_vertex(p,n);
}

	
/* free_simplex: does what is says. */

void free_simplex(struct spx *s)
{
	int i;
	struct spx* v0;

	if(s == NULL)return;
	free(s->name);
	free(s);
}
	
		
/* Note that simplicial complexes can be empty, but this is designated
 * by a single NULL simplex pointer */

struct scpx *alloc_complex(char *p)
{
	int i;
	struct scpx *an_scpx;
	char buf[MAX_NAME];

	if((an_scpx = (struct scpx *)malloc(sizeof(struct scpx))) == NULL)
			return NULL;
	an_scpx->n = 0;
	an_scpx->id = n_complexes++;
	an_scpx->simplices[0]=NULL;
	if(!p)sprintf(buf,"%d",an_scpx->id);
	else sprintf(buf,"%s",p);
	an_scpx->name = (char *)malloc((strlen(buf)+1)*sizeof(char));
	if(an_scpx->name == NULL)return NULL;
	strcpy(an_scpx->name,buf);
	for(i=0;i<MAX_COMPLEXES;i++)
		if(complex_registry[i] == NULL)break;
	if(i == MAX_COMPLEXES)
		fprintf(stderr,"alloc_complex: complex registry full.\n");
	else complex_registry[i] = an_scpx;
	return an_scpx;
}

/* A simple debugging dump to see what we've got */
static int my_level;

void dump_simplex(struct spx *s)
{
	int i,j;

	for(i=0;i<my_level;i++)printf("\t");
	printf("%d-simplex [%s]: <",s->p-1,s->name);
	for(j=0; j< s->n_parents;j++)
		printf("%s ",(s->parents[j])->name);
	if(j>0)printf("\b>\n");
	else printf(">\n");
	my_level++;
	for(i=0;i<s->n_facets;i++)
		dump_simplex(s->facets[i]);
	my_level--;
}

void dump_complex(struct scpx *c)
{
	int i;

	printf("Dump of complex [%s] (%d simplices):\n",c->name,c->n);
	for(i=0;i<c->n;i++)dump_simplex(c->simplices[i]);
	printf("\n");
}

/* Report on memory usage */
void status(void)
{
	int i,n;
		
	n = 0;
	for(i=0;i<MAX_SIMPLICES;i++)
		if(simplex_registry[i] != NULL){
			printf("%s\n",simplex_registry[i]->name);
			n++;
		}

	printf("status: %d registered simplices,",n);
	n = 0;
	for(i=0;i<MAX_COMPLEXES;i++)
		if(complex_registry[i] != NULL)n++;
	printf(" %d registered complexes.\n",n);
}
	
int has_simplex(struct spx *a, struct scpx *c)
{
	int i;
	
	if(c == NULL) return FALSE;
	if(a == NULL) return TRUE;

	for(i=0;i<c->n;i++)
		if(has_sub_simplex(c->simplices[i],a)==TRUE)return TRUE;
	return FALSE;
}

/* Top down search to see if the simplex s has a as a subsimplex. Return
 * TRUE, if so, else return FALSE */

static int has_sub_simplex(struct spx *s, struct spx *a)
{
	int i;
	struct spx *x;

	if(s == NULL) return FALSE;
	if(a == NULL) return TRUE;
	if(s == a)return TRUE;
	if(s->facets[0]==NULL)
		if(a->id == s->id) return TRUE;
		else return FALSE;
	for(i=0;i<s->n_facets;i++)
		if( has_sub_simplex(s->facets[i],a) == TRUE) return TRUE;
	return FALSE;
}

/* Attach complex new to complex acpx, returning a pointer to new augmented
   complex (i.e., to a_cpx), or NULL is there is any problem. The attachment
   is done by adjoining new_attach_point -- which must be a subsimplex of
   some simplex of new -- to the complex a_cpx. Simplices of new which don't
   contain the attach point are simply added to a_cpx. The complex new is
   is then destroyed (although most of its simplices are not.)
 */
 
struct scpx *attach_complex(struct scpx *new,  
	struct spx *new_attach_point, struct scpx *a_cpx,
	 struct spx *old_attach_point)
{

	int i;
	struct spx *p;

	if(!new) return a_cpx;
	if(!a_cpx)return new;

	for(i=0; i< new->n; i++){

		if(has_sub_simplex(new->simplices[i],new_attach_point)==TRUE){
			if(!attach_simplex_to_complex(new->simplices[i],
				new_attach_point,
				old_attach_point,a_cpx))return NULL;
		}
		else 
			a_cpx->simplices[a_cpx->n++]=new->simplices[i];
	}
	free(new->name);
	free(new);
	return a_cpx;
}

/* Attach simplex new to complex an_scpx by identifying new_attach_point
 * with old_attach_point. Return NULL if there is failure to do this for
 * any reason, else return pointer to newly augmented simplicial complex. 
 *
 * For the first simplex in the complex containing old_attach_point, it
 * calls attach_simplex to do the real work.
 *
 * Note that the old complex is not altered in this process */


struct scpx *attach_simplex_to_complex(struct spx *new,
struct spx *new_attach_point, struct spx *old_attach_point, 
struct scpx *an_scpx)
{

	int i,j,k,m,n,p;
	int found = FALSE;
	struct spx *simplex = NULL;

	/* sanity checks */
	if(has_sub_simplex(new,new_attach_point)==FALSE){
		fprintf(stderr,"attach: new attach point not subsimplex\n");
		return NULL;
	}
	if(an_scpx == NULL) return NULL;
	if(new == NULL)return an_scpx; /* Nothing to be done */

	if(an_scpx->n==0){  /* Nothing there to attach to */
		if(new_attach_point != NULL)return NULL;
		/* Add new as first simplex of the complex */
		an_scpx->n = 1;
		an_scpx->simplices[0] = new;
		return an_scpx;
	}

	for(i=0;i<an_scpx->n;i++){

		simplex = an_scpx->simplices[i];
		if(has_sub_simplex(simplex,old_attach_point)==TRUE) {
			found = TRUE;
			break;
		}

	}
	if(found == FALSE){
			 fprintf(stderr,"failed to find existing attach point\n");
			 return NULL;
	}
	if(attach_simplex(new_attach_point,old_attach_point)==
			FALSE){
		fprintf(stderr,"failed to attach %s to %s\n", new->name, old_attach_point->name);
		return NULL;
	}
	else {
		if(has_simplex(new,an_scpx) == FALSE){
			n = an_scpx->n;
			an_scpx->simplices[n] = new;
			an_scpx->n++;
		}
	}
	/*free_simplex(new_attach_point);*/ /* He's dead, Jim. */
	return an_scpx;
	
}


/* Attach whatever contains n to whatever contains o by indentifying n with
   o. Attach point simplex n is destroyed. n and o must be of the same
   dimension. Returns TRUE if everything comes out ok, FALSE otherwise. */

int attach_simplex(struct spx *n, struct spx *o)
{

        int i,j;
        struct spx *p;
        int found;

        if(n == NULL) {
                if(o == NULL) return TRUE;  /* OK, but does nothing */
                return FALSE;
        }
        if(o == NULL ) return FALSE;
        if(o->p != n->p) return FALSE;


        /* attach facets of n to facets of o */

        for(j=0;j<n->p;j++)
                 attach_simplex(n->facets[j],o->facets[j]);


        /* Set facet pointers in n's parents to o */

        for(i=0;i<n->n_parents;i++){

                p = n->parents[i];

                /* Find the pointer to n */
                found = FALSE;
                for(j=0;j<p->p;j++)
                        if(p->facets[j] == n){
                                found = TRUE;
                                break;
                        }
                if(found == FALSE)return FALSE;
                p->facets[j] = o;
		o->parents[o->n_parents++] = p;

          }

          return TRUE;
}

/* collect_garbage: free up any simplices in the registry that don't
   belong to any simplex of any complex */

void collect_garbage(void)
{
	int i,j,k,found;
	struct scpx *c;
	struct spx *p;

	for(k=0; k < MAX_SIMPLICES; k++){
		if(simplex_registry[k]==NULL)continue;
		p = simplex_registry[k];
		found = FALSE;

		for(i=0;i<MAX_COMPLEXES;i++){
			if(complex_registry[i]==NULL)continue;
			c = complex_registry[i];
			for(j=0;j<c->n;j++)
				if(has_sub_simplex(c->simplices[j],p)==TRUE){
					found = TRUE;
					break;
				}
			if(found == TRUE)break;
		}
		if(found == FALSE){
			free_simplex(p);
			simplex_registry[k] = NULL;
		}
	}	
}
