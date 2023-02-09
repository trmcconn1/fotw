/* db.h: declarations of the main database structures */

/* The declarations of calendar.h might also be included here, but they
   are sufficiently complex to deserve a file by themselves */
#include "calendar.h"


#define MAX_COURSES 60
#define MAX_CLASSES 100
#define MAX_SECTIONS 30
#define MAX_BLOCKS 30
#define MAX_SCHEDULES 10
#define MAX_BLOCK_SIZE 40
#define MAX_ASSIGNMENTSPERCLASS 10
#define MAX_ASSIGNMENTS 50
#define DELIMITER " 	\n"    /* space, tab , newline */
#define MAX_ROOMS  75
#define MAX_JOBS 50
#define MAX_PEOPLE 150
#define MAXCLASSESPERROOM 40
#define MAXCLASSESPERPERSON 10

#include "global.h"  /* It is important that this comes after all defines */

struct RoomStruct {
char name[SHORTNAME];
char bldg[SHORTNAME];
char number[NUMBERLEN];
int seats;
int exam_seats;
int numclasses;
struct ClassStruct *classes[MAXCLASSESPERROOM];
char usage[MAXLINE];
};

extern struct RoomStruct DefaultRoom;

struct JobStruct {
char name[SHORTNAME];
int num_types;
char **apt_type;
time_t start;
time_t end;
float load;
char description[MAXLINE];
};

extern struct JobStruct DefaultJob;

struct LoadStruct {
char semester[LONGNAME];
float load;
};

struct PersonStruct {
char shortname[SHORTNAME];
char apt_type[SHORTNAME];
time_t start;
time_t end;
int seniority;
float load_sem;
float load_ay;
float load_now;
int numloads;
struct LoadStruct *loads[MAX_SCHEDULES];
char longname[LONGNAME];
int numclasses;
struct ClassStruct *classes[MAXCLASSESPERPERSON];  /* will always be cast to class struct */
void *assignments[MAX_ASSIGNMENTSPERCLASS];
int numassignments;
};

extern struct PersonStruct DefaultPerson;

/* defined in load.h */

extern int numrooms;
extern struct RoomStruct *Rooms[];
extern int numjobs;
extern struct JobStruct *Jobs[];
extern int numpeople;
extern struct PersonStruct *People[];
extern int numblocks;
extern struct FinalBlockStruct *FinalBlocks[];

struct AssignStruct {
char name[LONGNAME];
struct JobStruct *job;
struct PersonStruct *holder;
char comment[LONGNAME];
};

struct ClassStruct {
char name[SHORTNAME];
int available;
int enrollment;
int attach;
char crse_name[SHORTNAME];
struct JobStruct *job;
struct SlotList *times;
struct RoomStruct *room;
struct RoomStruct *finalroom;
struct PersonStruct *teacher;
struct FinalBlockStruct *final;
char refno[NUMBERLEN];
char comment[LONGNAME];
int numclasses;
struct ClassStruct *classes[MAX_SECTIONS];      /* allows classes to be attached to classes */
int numassignments;
struct AssignStruct *assignments[MAX_ASSIGNMENTSPERCLASS];
};

struct FinalBlockStruct {
char name[SHORTNAME];
struct SlotList *exam_time;
struct SlotList *crse_time;
int numclasses;
struct ClassStruct *classes[MAX_BLOCK_SIZE];
};

extern struct FinalBlockStruct DefaultFinalBlock;

struct CourseStruct {
char name[SHORTNAME];
int seqno;
int available;
int enrollment;
int numclasses;
int numassignments;
char comment[LONGNAME];
struct PersonStruct *supervisor;
struct ClassStruct *classes[MAX_SECTIONS];
struct AssignStruct *assignments[MAX_ASSIGNMENTSPERCLASS];
};

struct ScheduleStruct {
char semester[SHORTNAME];
char name[LONGNAME];
struct ClassStruct *classes[MAX_CLASSES];
struct CourseStruct *Courses[MAX_COURSES];
struct AssignStruct *assignments[MAX_ASSIGNMENTS];
int numclasses;
int numcourses;
int numassignments;
};

/* defined in shell.c */

extern int numschedules;
extern struct ScheduleStruct *Schedules[];

/* Comparison Routines for sorting and searching. Cf compare.c */


extern int CompareCoursesByName(struct CourseStruct *, struct CourseStruct *);
extern int CompareCoursesBySeqno(struct CourseStruct *, struct CourseStruct *);
extern int ComparePeopleByShortName(struct PersonStruct *, struct PersonStruct *);
extern int CompareSlotsByName(struct SlotList *, struct SlotList *);
extern int CompareRoomsByName(struct RoomStruct *, struct RoomStruct *);
extern int CompareClassesByName(struct ClassStruct *, struct ClassStruct *);
extern int CompareJobsByName(struct JobStruct *, struct JobStruct *);
extern int CompareSchedulesBySemester(struct ScheduleStruct *,
	struct ScheduleStruct *);

