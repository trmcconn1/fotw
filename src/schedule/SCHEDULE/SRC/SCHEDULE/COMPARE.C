/* compare.c: comparison functions used with bsearch and qsort library
    routines. Each must return -1 if the first arg is < the second,
    0 if they are equal, and 1 else */

#include "db.h"
#include<string.h>



int
CompareSlotListsByName(struct SlotList *first, struct SlotList *second)
{
	/* NULL comes before anything */
	if(first == NULL)
		if(second == NULL) return 0;
		else return -1;

	if(second == NULL) return 1;

	return strcmp(first->name,second->name);
}

int
CompareSlotsByTime(struct TimeSlot *first, struct TimeSlot *second)
{
        /* NULL comes before anything */
        if(first == NULL)
                if(second == NULL) return 0;
                else return -1;

        if(second == NULL) return 1;

	if(first->start == second->start)return 0;
        return first->start < second->start ? -1 : 1;

}

int
CompareSlotsByName(struct SlotList *first, struct SlotList *second)
{
        /* NULL comes before anything */
        if(first == NULL)
                if(second == NULL) return 0;
                else return -1;

        if(second == NULL) return 1;

	return strcmp(first->name,second->name);
}

int
CompareRoomsByName(struct RoomStruct *first, struct RoomStruct *second)
{
        /* NULL comes before anything */
        if(first == NULL)
                if(second == NULL) return 0;
                else return -1;

        if(second == NULL) return 1;

	return strcmp(first->name,second->name);
}

int
CompareVarsByName(struct ShellVar *first, struct ShellVar *second)
{
/* NULL comes before anything */
        if(first == NULL)
                if(second == NULL) return 0;
                else return -1;

        if(second == NULL) return 1;

	return strcmp(first->name,second->name);
}

int
CompareTokensByPosition(struct TokenType *first, struct TokenType *second)
{
/* NULL comes before anything */
        if(first == NULL)
                if(second == NULL) return 0;
                else return -1;

        if(second == NULL) return 1;
	if(first->position == second->position) return 0;
	return first->position < second->position ? -1 : 1;
}


struct ShellCmd {
char *name;
char *usage;             /* Associative array of shell variables */
char *description;
int (*function) ();
};

int
CompareCmdsByName(struct ShellCmd *first, struct ShellCmd *second)
{
/* NULL comes before anything */
        if(first == NULL)
                if(second == NULL) return 0;
                else return -1;

        if(second == NULL) return 1;

	return strcmp(first->name,second->name);
}

int
CompareCoursesByName(struct CourseStruct *first, struct CourseStruct *second)
{
/* NULL comes before anything */
        if(first == NULL)
                if(second == NULL) return 0;
                else return -1;

        if(second == NULL) return 1;

	return strcmp(first->name,second->name);
}

int
ComparePeopleByShortName(struct PersonStruct *first, struct PersonStruct *second)
{
/* NULL comes before anything */
        if(first == NULL)
                if(second == NULL) return 0;
                else return -1;

        if(second == NULL) return 1;

	return strcmp(first->shortname,second->shortname);
}


int
ComparePeopleByRankandName(struct PersonStruct *first, struct PersonStruct *second)
{
/* NULL comes before anything */
        if(first == NULL)
                if(second == NULL) return 0;
                else return -1;

        if(second == NULL) return 1;

	if(strcmp(first->apt_type,second->apt_type)==0)
	  return strcmp(first->shortname,second->shortname);
	return strcmp(first->apt_type,second->apt_type);
}

int
CompareClassesByName(struct ClassStruct *first, struct ClassStruct *second)
{
/* NULL comes before anything */
        if(first == NULL)
                if(second == NULL) return 0;
                else return -1;

        if(second == NULL) return 1;

	return strcmp(first->name,second->name);
}

int
CompareJobsByName(struct JobStruct *first, struct JobStruct *second)
{
/* NULL comes before anything */
        if(first == NULL)
                if(second == NULL) return 0;
                else return -1;

        if(second == NULL) return 1;

        return strcmp(first->name,second->name);
}


int
CompareCoursesBySeqno(struct CourseStruct *first, struct CourseStruct *second)
{
/* NULL comes before anything */
        if(first == NULL)
                if(second == NULL) return 0;
                else return -1;
        if(second == NULL) return 1;

	if(first->seqno == second->seqno) return 0;
	return first->seqno < second->seqno ? -1 : 1;
}

int
CompareSchedulesBySemester(struct ScheduleStruct *first, struct ScheduleStruct *second)
{
/* NULL comes before anything */
        if(first == NULL)
                if(second == NULL) return 0;
                else return -1;

        if(second == NULL) return 1;

        return strcmp(first->semester,second->semester);
}

int
CompareBlocksByName(struct FinalBlockStruct *first, struct FinalBlockStruct *second)
{
	/* NULL comes before anything */
        if(first == NULL)
                if(second == NULL) return 0;
                else return -1;

        if(second == NULL) return 1;

	return strcmp(first->name,second->name);
}

int
CompareBlocksByClassTime(struct FinalBlockStruct *first, struct FinalBlockStruct *second)
{
	/* NULL comes before anything */
        if(first == NULL)
                if(second == NULL) return 0;
                else return -1;

        if(second == NULL) return 1;

	if(first->crse_time == NULL)
		if(second->crse_time == NULL) return strcmp(first->name,second->name);
		else return -1;

	if(second->crse_time == NULL) return 1;

	return strcmp(first->crse_time->name,second->crse_time->name);
}


