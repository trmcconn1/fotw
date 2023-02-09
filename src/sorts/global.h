/* global.h: common include file */

#include <stdio.h>
#include <stdlib.h>

#ifndef MAX_PATH
#define MAX_PATH 256
#endif

#define TRUE 1
#define FALSE 0

/* profiling variables */
extern int compares;  /* in compare.c */
extern int swaps;    /*  in util.c */
extern int depth;    /*  recursive depth: in util.c */
extern int calls; /*     in util.c */
extern int overhead;  /* in util.c: it's interpretation varies from one
			 algorithm to another */

extern void swap(void *, int, int, size_t);
extern int cmp_int(void *, void *);
extern int *random_int_data(int,int,int,int,long);

extern void quicksort(void *, size_t, size_t, int (*)(const void *,const void *));
extern void insertsort(void *, size_t, size_t, int (*)(const void *,const void *));
extern void _mergesort(void *, size_t, size_t, int (*)(const void *,const void *));
extern void bubblesort(void *, size_t, size_t, int (*)(const void *,const void *));
extern void _heapsort(void *, size_t, size_t, int (*)(const void *,const void *));
extern void shellsort(void *, size_t, size_t, int (*)(const void *,const void *));
extern void selectsort(void *, size_t, size_t, int (*)(const void *,const void *));
extern void countingsort(void *, size_t, size_t, int (*)(const void *,const void *));
