/* stack.h: header for stack.c and files that use it */

struct stackstruct {

	int size;
	int capacity;
	void **mem;
	void *top;

};

extern struct stackstruct *alloc_stack(int);
extern void free_stack(struct stackstruct *);
extern int push_stack(struct stackstruct *, void *);
extern void *pop_stack(struct stackstruct *);
