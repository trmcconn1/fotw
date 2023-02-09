/* Implements simple data base */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"

#define DB_SIZE 65536

struct db_entry *my_db[MAX_DB_SIZE];
int db_size = 0;

struct db_entry *alloc_entry(int n1, int n2, int n3, int n4, int n5)
{
	struct db_entry *rval;

	rval = (struct db_entry *)malloc(sizeof(struct db_entry));
	if(!rval) return NULL;
	
	rval->name  = malloc(n1*sizeof(char));
	if(!rval->name) {
		free(rval);
		return NULL;
	}
	rval->team = malloc(n2*sizeof(char));
	if(!rval->team){
		free(rval->name);
		free(rval);
		return NULL;
	}
	rval->ag = malloc(n3*sizeof(char));
	if(!rval->ag){
		free(rval->name);
		free(rval->team);
		free(rval);
		return NULL;
	}
	rval->event = malloc(n4*sizeof(char));
	if(!rval->event){
		free(rval->name);
		free(rval->team);
		free(rval->ag);
		free(rval);
		return NULL;
	}
	rval->date = malloc(n5*sizeof(char));
	if(!rval->date){
		free(rval->name);
		free(rval->team);
		free(rval->event);
		free(rval->ag);
		free(rval);
		return NULL;
	}
	return rval;	
}

void free_entry(struct db_entry *p)
{

	if(!p) return;
	if(p->ag)free(p->ag);
	if(p->event)free(p->event);
	if(p->name)free(p->name);
	if(p->team)free(p->team);
	if(p->date)free(p->date);
	free(p);
}

int cmp_by_event_up(struct db_entry **a, struct db_entry **b)
{
	return(strcmp((*a)->event,(*b)->event));
}
int cmp_by_event_down(struct db_entry **a, struct db_entry **b)
{
	return(-strcmp((*a)->event,(*b)->event));
}
int cmp_by_ag_up(struct db_entry **a, struct db_entry **b)
{
	return(strcmp((*a)->ag,(*b)->ag));
}
int cmp_by_ag_down(struct db_entry **a, struct db_entry **b)
{
	return(-strcmp((*a)->ag,(*b)->ag));
}
int cmp_by_name_up(struct db_entry **a, struct db_entry **b)
{
	return(strcmp((*a)->name,(*b)->name));
}
int cmp_by_name_down(struct db_entry **a, struct db_entry **b)
{
	return(-strcmp((*a)->name,(*b)->name));
}

int cmp_by_event_and_ag_up(struct db_entry **a, struct db_entry **b){

	if(strcmp((*a)->event,(*b)->event)==0)return(strcmp((*a)->ag,(*b)->ag));
	else return(strcmp((*a)->event,(*b)->event));
}
int cmp_by_event_and_ag_down(struct db_entry **a, struct db_entry **b){
	return(-cmp_by_event_and_ag_up(a,b));
}
void sort_db_by_event(int direction){

		if(direction == 0)
			qsort((void *)my_db,(size_t)db_size,
				sizeof(struct db_entry *),
			(int (*)(const void *, const void *))cmp_by_event_up);
		else
			qsort((void *)my_db,(size_t)db_size,
				sizeof(struct db_entry *),
			(int (*)(const void *,const void *))cmp_by_event_down);
}
void sort_db_by_ag(int direction){

		if(direction == 0)
			qsort((void *)my_db,(size_t)db_size,
				sizeof(struct db_entry *),
			(int (*)(const void *, const void *))cmp_by_ag_up);
		else
			qsort((void *)my_db,(size_t)db_size,
				sizeof(struct db_entry *),
			(int (*)(const void *,const void *))cmp_by_ag_down);
}
void sort_db_by_name(int direction){

		if(direction == 0)
			qsort((void *)my_db,(size_t)db_size,
				sizeof(struct db_entry *),
			(int (*)(const void *, const void *))cmp_by_name_up);
		else
			qsort((void *)my_db,(size_t)db_size,
				sizeof(struct db_entry *),
			(int (*)(const void *,const void *))cmp_by_name_down);
}
void sort_db_by_event_and_ag(int direction){

		if(direction == 0)
			qsort((void *)my_db,(size_t)db_size,
				sizeof(struct db_entry *),
			(int (*)(const void *, const void *))cmp_by_event_and_ag_up);
		else
			qsort((void *)my_db,(size_t)db_size,
				sizeof(struct db_entry *),
			(int (*)(const void *,const void *))cmp_by_event_and_ag_down);
}

struct db_entry *search_db_by_event_and_ag(struct db_entry *p){

	return (struct db_entry *)bsearch((void *)p,(void *)my_db,(size_t)db_size,sizeof(struct db_entry *),
			(int (*)(const void *, const void *))cmp_by_event_and_ag_up  );
}
