/* tape.c: manage Turing tape. Allocate, free, grow as needed. */

#include<stdlib.h>
#include<stdio.h>
#include "global.h"

#define GROW_FACTOR 2

int tape_size; 
extern int *tape;  /* Defined in turing.c */

int *alloc_tape(int size)
{
	int *p;
	p =  (int *)calloc(size,sizeof(int));
	if(p) tape_size = size;
	else tape_size = 0;
	return p;

}

void free_tape(){

        free(tape);

}

int grow_tape( ){

	int *p;
	int i; 
#ifdef DEBUG
	fprintf(stderr,"grow_tape: old = %d ",tape_size);
#endif
	p = (int *)calloc(tape_size*GROW_FACTOR,sizeof(int));
	if(!p)return FALSE;
	for(i=0;i<tape_size;i++)
		p[i] = tape[i];
	tape_size *= GROW_FACTOR;
	free_tape();
	tape = p;
#ifdef DEBUG
	fprintf(stderr,"new = %d\n",tape_size);
#endif
	return TRUE;

}
	
		
