/* calendar.h: declarations of structures and functions for creating,
   modifying, and managing the academic calendar */

#include "global.h"
#include<time.h>


#define MAX_LIST_SIZE 75
#define FIELDSEPS "/- "        /* in a date string */
#define MAXSLOTS 8  /* Nummber of time slot series in a slot list */
#define WEEK 604800

extern time_t SemesterStart; /* defined in load.c */
extern time_t EndOfClasses;
extern time_t EndOfSemester;

extern void myerror(FILE *, char *fmt, ...);  /* defined in main.c */

/* A time slot is an interval of time */

struct TimeSlot {
char *name;        /* Only used in holiday timeslots to name that holiday */
time_t start;     
time_t end;      
};

/* Class Meeting Times: the complete list of times at which a "class" 
   meets. A final exam is considered to be a "class". Many different
   classes will normally share one of these. 
*/

struct SlotList {
char *name;     /* e.g. mwf8 */
int number;
struct TimeSlot **timeslots;
char *description;
};

extern struct SlotList DefaultSlotList;

extern struct SlotList *TimeSlotLists[];
extern int NumberOfSlotLists;

/* Contains the data needed to create a list of meeting times */
struct SlotListSpec { 
char name[SHORTNAME];
int numberofslots; /* number of pairs below that actually get used */
/* offsets in time_t from beginning of a week */
time_t start[8];
time_t end[8];
char description[LONGNAME];
};

/* INTERFACE TO FUNCTIONS DEFINED IN calendar.c */

time_t ParseDate(char *datestring, char *timestring); /* Convert */
struct SlotList *CreatSlotList(struct SlotListSpec *);
void RmSlotList(struct SlotList *);
int SlotsDoOverlap(struct SlotList *, struct SlotList *,
			time_t *, time_t *);
void DumpSlotList(FILE *ostream,struct SlotList *);

/* Defined in compare.c */
extern int CompareSlotListsByName(struct SlotList *, struct SlotList *);
extern int CompareSlotsByTime(struct TimeSlot *, struct TimeSlot *);
