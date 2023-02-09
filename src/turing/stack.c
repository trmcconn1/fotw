/* stack.c: Implementation of a basic FIFO stack */

#include<stdlib.h>
#include "stack.h"


struct stackstruct *alloc_stack( int capacity )
{
	
	struct stackstruct *p;

	p = (struct stackstruct *)malloc(sizeof(struct stackstruct));
	if(!p)return NULL;

	p->mem = (void **)malloc(capacity*sizeof(void *));

	if(!p->mem){
		free(p);
		return NULL;
	}

	p->size = 0;
	p->capacity = capacity;
	p->top = NULL;
	
	return p;
}

void free_stack(struct stackstruct *p)
{

	free(p->mem);
	free(p);
}

/* push the item on the indicated stack. Return 1 if not possible, 0 else */

int push_stack(struct stackstruct *s, void *p)
{

	if(s->size >= s->capacity)return 1;  /* no can do */	

	s->mem[s->size] = p;
	s->top = s->mem[s->size++];
	return 0;

}

/* pop an item from the stack and return it */

void *pop_stack(struct stackstruct *s)
{
	void *t;

	if(s->size==0)return NULL;

	t = s->top;
	--(s->size);
	s->top = s->mem[s->size-1];
	return t;
}
	
