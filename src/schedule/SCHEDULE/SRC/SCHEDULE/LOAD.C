/* load.c: parsing and other routines to load data from data files */

#include "db.h"
#include<string.h>
#include<ctype.h>


#define SHELLSTREAMS FILE *shell_istream, FILE *shell_ostream,\
FILE *shell_errorstream, FILE *shell_promptstream
int line;


int tokenize(char *,struct TokenType **);
int ParseTimesFile(void);
int ParseRoomsFile(void);
int ParseJobsFile(void);
int ParsePeopleFile(void);
int FreeClass(struct ClassStruct *);
int FreeSlotList(struct SlotList *alist);
int load(SHELLSTREAMS);
extern char  *GetShell(char *);
extern set(char *, char *, int, FILE *);
extern int CompareTokensByPosition(struct TokenType *, struct TokenType *);
extern int ClassLevel;

time_t SemesterStart;
time_t EndOfClasses;
time_t EndOfSemester;

static struct SlotListSpec ASlotListSpec;
static struct RoomStruct TRoom;
static struct JobStruct TJob;
static struct PersonStruct TPerson;
static struct SlotList TSlot;

int update_flag = 0;

static int find_opt(char *); /* load uses this for option processing */


char linebuf[MAXLINE];

/* These database elements are defined in this file */
/* Bad or missing data gets assigned from Defaultxxx */

struct SlotList *TimeSlotLists[MAX_LIST_SIZE];
struct SlotList DefaultSlotList; 
struct FinalBlockStruct DefaultFinalBlock;
int NumberOfSlotLists = 0;
struct RoomStruct *Rooms[MAX_ROOMS];
struct RoomStruct DefaultRoom; 
int numrooms =0;
struct JobStruct *Jobs[MAX_JOBS];
struct JobStruct DefaultJob;
char *DefaultJobApptTypePtr;
char DefaultJobApptType[SHORTNAME];
int numjobs = 0;
struct PersonStruct *People[MAX_PEOPLE];
struct PersonStruct DefaultPerson;
int numpeople = 0;
int numblocks = 0;
struct FinalBlockStruct *FinalBlocks[MAX_BLOCKS];

/* load: shell function. (The streams args are there only for compatibility
	with other shell functions. It
	loads all data from external ascii files into internal data
      structures */

static char buffer[MAXPATH];
static char buffer2[MAXPATH];

/* setup default data types: invoked from main */
char DefaultSlotListname[SHORTNAME];
char DefaultSlotListdescription[LONGNAME];

void init_defaults()
{
	DefaultSlotList.name = DefaultSlotListname;
	DefaultSlotList.description = DefaultSlotListdescription;
	strcpy(DefaultSlotList.name,MissingData);
	DefaultSlotList.number = 0;
	strcpy(DefaultSlotList.description,MissingData);
        strcpy(DefaultPerson.shortname , MissingData);
	strcpy(DefaultPerson.apt_type,"None");
	DefaultPerson.start = DefaultPerson.end = 0;
	DefaultPerson.seniority = 0;
	DefaultPerson.load_sem = 11111;
	DefaultPerson.load_ay = 11111;
	DefaultPerson.load_now = 0;
        strcpy(DefaultPerson.longname , MissingData);
	DefaultPerson.numclasses = 0;
	DefaultPerson.numassignments = 0;
	strcpy(DefaultRoom.name,MissingData);
	strcpy(DefaultRoom.bldg,MissingData);
	strcpy(DefaultRoom.number,MissingData);
	DefaultRoom.seats = 0;
	DefaultRoom.exam_seats = 0;
	DefaultRoom.numclasses = 0;
	strcpy(DefaultRoom.usage,MissingData);
	strcpy(DefaultJob.name,MissingData);
	DefaultFinalBlock.numclasses = 0;
	strcpy(DefaultFinalBlock.name,MissingData);
	DefaultFinalBlock.exam_time = &DefaultSlotList;
	DefaultFinalBlock.crse_time = &DefaultSlotList;

/* set up array of appoint types for the default job */

	DefaultJob.num_types = 1;
        strcpy(DefaultJobApptType,"None");
        DefaultJobApptTypePtr = DefaultJobApptType;
        DefaultJob.apt_type = &DefaultJobApptTypePtr;
	DefaultJob.end = 0;
	DefaultJob.start = 0;
	DefaultJob.load = 0;
	strcpy(DefaultJob.description,MissingData);
	return;

}
	

int load(SHELLSTREAMS)
{
	int argc;
	char *args[10];
	char **argv;
	char argstr[SHORTNAME];
        static int loaded;
	int include_times = 1;
        static int source;
	int i;
	int j;
	int k;
	int c;
	

	update_flag = 0;
	include_times = 1;
        source =1;

	argc = numtokens;
	argv = args;
        if(numtokens >= 6)return 1;
	for(i=0;i<numtokens;i++)
		args[i] = Tokens[i]->text;

/* Process Command Line Options */

if(--argc > 0){  /* Anything on command line ? */
		++argv;
		while((argc > 0)&&((*argv[0]) == '-')){ /* loop while 
							there are flags */
			argstr[0]='-';
			argstr[1]='\0';
			strcat(argstr,++*argv); /* glob argstr*/
			argc--;
			while((argc>0)&&(strlen(*argv)<3)&& (strlen(*(argv+1))<3)&&
				((*(argv+1))[0]=='-')&&((*(argv+1))[1]!='-')){
				++argv;
				argc--;
				strcat(argstr,++*argv);
			}
			++argv; /* ok, consecutive flags now globbed into one*/
		       c = find_opt(argstr+1); /* look up flag code */
		switch(c){

/* 
   Make sure to increment argv and decrement argc 
   each time. The code for handling the arguments for a given flag must
   go in the case for that flag. 

*/

			case -1: break;  /* default, e.g no options */

			case 0: include_times = 0;
				break;
			case 2: include_times = 0;
			case 1: update_flag = 1;
				break;
			case 3:
				strcpy(Semester,*argv++);
				argc--;
				break;
			case 4: source = 0;
				break;
			case -2: ;  /* barf (don't use in your code) */
			default:
				return 1;
			} /* End flag switch */
	} /* End flag grabbing while loop */
} /* end if: any args ? */

/*   argc now gives a count of remaining arguments on command line,
	and argv points at the first one */


/* Free previous structures if a previous load has been done */
	if(loaded){
		if(!update_flag){

		 for(i=0;i<numrooms;i++)
                        free(Rooms[i]);
		 numrooms = 0;

		for(i=0;i<numblocks;i++)
			free(FinalBlocks[i]);
		numblocks = 0;
                 for(i=0;i<numjobs;i++){
			for(j=0;j<Jobs[i]->num_types;j++)
                                free(*(Jobs[i]->apt_type+j));
                        free(Jobs[i]->apt_type);
                        free(Jobs[i]);
		 }

		 numjobs = 0;
		

		 for(i=0;i<numpeople;i++){
			for(j=0;j<People[i]->numloads;j++)
                                free(People[i]->loads[j]);
                        free(People[i]);
		}
		 numpeople = 0;
		 for(i=0;i<NumberOfSlotLists;i++)
                        FreeSlotList(TimeSlotLists[i]);
		 NumberOfSlotLists = 0;

		for(k=0;k<numschedules;k++){
		for(i=0;i<Schedules[k]->numcourses;i++){
			for(j=0;j<Schedules[k]->Courses[i]->numclasses;j++)
				FreeClass(Schedules[k]->Courses[i]->classes[j]);
                        for(j=0;j<Schedules[k]->Courses[i]->numassignments;j++)
				free(Schedules[k]->Courses[i]->assignments[j]);
                        free(Schedules[k]->Courses[i]);
		}
		for(i=0;i<Schedules[k]->numassignments;i++)
                        free(Schedules[k]->assignments[i]);
                for(i=0;i<Schedules[k]->numclasses;i++)
                        FreeClass(Schedules[k]->classes[i]);
                free(Schedules[k]);
		}
                numschedules = 0;
		}

/* Get's done whether replacing or updating -- both times */
/* N.B. failing to free these shell variables can cause a rather subtle
   memory leak, since they were alloced during the schedule load. If not
   removed, the heap ends at the first one of the unfreed blocks */

		ClassLevel = 0;
		set("COURSE","",2,ostream);
		set("CLASS","",2,ostream);
                set("ASSIGNMENT","",2,ostream);
                set("LOADED","",2,ostream);

/* reset indices in default data structures */
		DefaultSlotList.number = 0;
		DefaultPerson.numclasses = 0;
		DefaultRoom.numclasses = 0;
	}  /* End of part that only gets done if previous load */     

/* reinitialize the default types in case something has changed */
	init_defaults();

if(source){
/* Process the times file */
	if(include_times){
	SAFECPY(buffer,WorkingDirectory, MAXPATH - 19);
	strcat(buffer,"/data/");
	strcat(buffer,Semester);
	strcat(buffer,"/time");
	holdstream = istream;
	if((istream = fopen(buffer,"r"))==NULL)
                fatal(stderr,"Unable to open %s",buffer);
	strcpy(buffer2,CurrentFile);
	strcpy(CurrentFile,buffer);
	ParseTimesFile();
	fclose(istream);
	istream = holdstream;
	strcpy(CurrentFile,buffer2);
	}

/* Process the rooms file */

	SAFECPY(buffer,WorkingDirectory, MAXPATH - 19);
	strcat(buffer,"/data/");
	strcat(buffer,Semester);
	strcat(buffer,"/rooms");
	holdstream = istream;
	if((istream = fopen(buffer,"r"))==NULL)
                fatal(stderr,"Unable to open %s",buffer);
	strcpy(buffer2,CurrentFile);
	strcpy(CurrentFile,buffer);
	ParseRoomsFile();
	fclose(istream);
	istream = holdstream;
	strcpy(CurrentFile,buffer2);

/* Process the jobs file */ 

	SAFECPY(buffer,WorkingDirectory, MAXPATH - 19);
	strcat(buffer,"/data/");
	strcat(buffer,Semester);
	strcat(buffer,"/jobs");
	holdstream = istream;
	if((istream = fopen(buffer,"r"))==NULL)
                fatal(stderr,"Unable to open %s",buffer);
	strcpy(buffer2,CurrentFile);
	strcpy(CurrentFile,buffer);
	ParseJobsFile();
	fclose(istream);
	istream = holdstream;
	strcpy(CurrentFile,buffer2);


/* Process the people file */ 

	SAFECPY(buffer,WorkingDirectory, MAXPATH - 19);
	strcat(buffer,"/data/");
	strcat(buffer,Semester);
	strcat(buffer,"/people");
	holdstream = istream;
	if((istream = fopen(buffer,"r"))==NULL)
                fatal(stderr,"Unable to open %s",buffer);
	strcpy(buffer2,CurrentFile);
	strcpy(CurrentFile,buffer);
	ParsePeopleFile();
	fclose(istream);
	istream = holdstream;
	strcpy(CurrentFile,buffer2);
        loaded = 1;
}
        if(!source){
          loaded = 0;
          set("LOADED","",2,NULL);
        }
        else
          set("LOADED",Semester,3,NULL);
	return 0;
}

int ParseTimesFile()
{
	char *ptr ;
	int parsed = 0;
	int i;
	int hold;

	TSlot.name = buffer;
      
	ptr = linebuf;

	line = 0;
	SemesterStart = (time_t) 0;
	EndOfClasses = (time_t) 0;


	numtokens = 0;
/* Loop over lines in timesfile */

	while(fgets(ptr,MAXLINE + linebuf -ptr ,istream) != NULL){

	line++;

/* Now parse the contents of linebuf */

	numtokens = tokenize(linebuf,Tokens);
	if(numtokens == -1){       /* line continues */
		 ptr = ptr + strlen(ptr);
		 continue;
	}
	if(numtokens == 0){
		ptr = linebuf;
		continue;
	}
	ptr = linebuf;

	if(strcmp(Tokens[0]->text,"SemesterStart")==0){
		if(numtokens != 3)
			myerror(errorstream,"Syntax error.\n" );
		else if((SemesterStart = ParseDate(Tokens[1]->text,Tokens[2]->text))==0) 
			myerror(errorstream,"Syntax error.\n"); 
	parsed = 1;

	}

	if(strcmp(Tokens[0]->text,"EndOfClasses")==0){
		if(numtokens != 3)
			myerror(errorstream,"Syntax error.\n") ;
		else if((EndOfClasses = ParseDate(Tokens[1]->text,Tokens[2]->text))==0)
			myerror(errorstream,"Syntax error. \n"); 
	parsed = 1;
	}

	if(strcmp(Tokens[0]->text,"EndOfSemester")==0){
		if(numtokens != 3)
			myerror(errorstream,"Syntax error. \n");
		else if((EndOfSemester = ParseDate(Tokens[1]->text,Tokens[2]->text))==0) 
			myerror(errorstream,"Syntax error.\n"); 
	parsed = 1;

	}


	if(strcmp(Tokens[0]->text,"PeriodicTimeSlot")==0) {
		SAFECPY(ASlotListSpec.name,Tokens[1]->text,SHORTNAME);
		SAFECPY(ASlotListSpec.description,Tokens[2]->text,LONGNAME);

/* Check the format of token 3. If it is a number then assume that what
   follows is a list of numerical offsets from semester start. If is a
   date then what follows is a list of date-time pairs */

		if(strpbrk(Tokens[3]->text,"/-") != NULL) {

/* it's a date! */
		/* make sure the  number of remaining tokens
		    on the line is divisible by 4*/                             
		if((numtokens-3) % 4 ) 
			myerror(errorstream,"Syntax error in %s\n",linebuf );
		if(numtokens - 3 <= 0)
			myerror(errorstream,"Syntax error in %s\n",linebuf);
		ASlotListSpec.numberofslots = 0;
		parsed = 1;

/* Build Slot List Specification from the contents of this line */
		for(i=0;i<(numtokens -3)/4;i++) {
			ASlotListSpec.start[i]=
			ParseDate(Tokens[3+4*i]->text,Tokens[4+4*i]->text) 
					-SemesterStart;
			ASlotListSpec.end[i]=
			ParseDate(Tokens[5+4*i]->text,Tokens[6+4*i]->text)
					- SemesterStart;
			ASlotListSpec.numberofslots++;
		}

/* Create a new Time slot list and add to the list of all of them */

		TimeSlotLists[NumberOfSlotLists++]
			= CreatSlotList(&ASlotListSpec);
		}

		else { /* It's an offset */

 /* make sure the  number of remaining tokens
		    on the line is divisible by 2*/
		if((numtokens-3) % 2 )
			myerror(errorstream,"Syntax error in %s\n",linebuf);
		if(numtokens - 3 <= 0)
			myerror(errorstream,"Syntax error in %s\n",linebuf);
		ASlotListSpec.numberofslots = 0;
		parsed = 1;

/* Build Slot List Specification from the contents of this line */
		for(i=0;i<(numtokens -3)/2;i++) {
			ASlotListSpec.start[i]= (time_t) atol(Tokens[3 + 2*i]->text);
			ASlotListSpec.end[i]= (time_t) atol(Tokens[4 + 2*i]->text);
			ASlotListSpec.numberofslots++;
		}

/* Create a new Time slot list and add to the list of all of them */
                TimeSlotLists[NumberOfSlotLists++]
			= CreatSlotList(&ASlotListSpec);

		}
	}

/* NonPeriodic Time Slot entry */

	if(strcmp(Tokens[0]->text,"NonPeriodicTimeSlot")==0) {
/* make sure the  number of remaining tokens
		    on the line is divisible by 4*/
		if((numtokens-3) % 4 )
			myerror(errorstream,"Syntax errorin %s\n",linebuf);
		 if(numtokens - 3 <= 0)
			myerror(errorstream,"Syntax error in %s\n",linebuf);
		parsed = 1;
		if((TimeSlotLists[NumberOfSlotLists]
			 = (struct SlotList *)malloc(sizeof(struct SlotList)))==NULL)
                fatal(stderr,"Failed to allocate memory for slot list.");      

		hold = TimeSlotLists[NumberOfSlotLists]->number = 0;    
		if((TimeSlotLists[NumberOfSlotLists]->name = (char *)malloc(SHORTNAME))==NULL)
                  fatal(stderr,"Unable to malloc slotlist name\n");
		if((TimeSlotLists[NumberOfSlotLists]->description = (char *)malloc(LONGNAME))==NULL)
                  fatal(stderr,"Unable to malloc slotlist name\n");
		SAFECPY(TimeSlotLists[NumberOfSlotLists]->name,Tokens[1]->text,
SHORTNAME); 
		SAFECPY(TimeSlotLists[NumberOfSlotLists]->description,
			Tokens[2]->text,LONGNAME);

/* alloc a big enough array of pointers to timeslots */
		if((TimeSlotLists[NumberOfSlotLists]->timeslots =
           (struct TimeSlot **)malloc((numtokens-3)/4*sizeof(struct TimeSlot *)))==NULL)
            fatal(stderr,"Unable to malloc timeslot *array\n");
	        for(i=0;i<(numtokens-3)/4;i++)
		if((TimeSlotLists[NumberOfSlotLists]->timeslots[i] =
                  (struct TimeSlot *)malloc(sizeof(struct TimeSlot)))==NULL)
                  fatal(stderr,"Unable to malloc slotlist \n");

		for(i=0;i<(numtokens -3)/4;i++) {
		  TimeSlotLists[NumberOfSlotLists]->timeslots[hold]->start =
			ParseDate(Tokens[3+4*i]->text,Tokens[4+4*i]->text);
		  TimeSlotLists[NumberOfSlotLists]->timeslots[hold++]->end =
			ParseDate(Tokens[5+4*i]->text,Tokens[6+4*i]->text);
		}
		TimeSlotLists[NumberOfSlotLists]->number = hold;
		NumberOfSlotLists++;

	}
		
/* FinalBlock entry */

	if(strcmp(Tokens[0]->text,"FinalBlock")==0) {
		if(numtokens != 4)
			myerror(errorstream,"Syntax error in %s\n",linebuf);
		parsed = 1;
		if((FinalBlocks[numblocks] =
                  (struct FinalBlockStruct *)malloc(sizeof(struct FinalBlockStruct)))==
                      NULL)
			fatal(stderr,"Failed to allocate memory for final block.");
		SAFECPY(FinalBlocks[numblocks]->name,Tokens[1]->text,SHORTNAME);
                FinalBlocks[numblocks]->numclasses = 0; 
	        krqsort((void **)TimeSlotLists,0,NumberOfSlotLists-1,(int(*) (void *,void *))
                CompareSlotsByName);
		strcpy(TSlot.name,Tokens[2]->text);
                FinalBlocks[numblocks]->exam_time = binsearch((void *)&TSlot,
                (void **)TimeSlotLists,NumberOfSlotLists,
                (int(*) (void *,void *))CompareSlotsByName);
                if(FinalBlocks[numblocks]->exam_time == NULL){
	            myerror(errorstream,
                    "load: no such time slot %s\n",Tokens[2]->text);
                    FinalBlocks[numblocks]->exam_time = &DefaultSlotList;
		}
		if(strcmp(Tokens[3]->text,"NA")==0)
		    FinalBlocks[numblocks]->crse_time = NULL;
                else {
		  strcpy(TSlot.name,Tokens[3]->text);
                  FinalBlocks[numblocks]->crse_time = binsearch((void *)&TSlot,
                   (void **)TimeSlotLists,NumberOfSlotLists,
                   (int(*) (void *,void *))CompareSlotsByName);
                  if(FinalBlocks[numblocks]->crse_time == NULL){
                    myerror(errorstream,
                    "load: no such time slot %s\n",Tokens[2]->text);
                    FinalBlocks[numblocks]->crse_time = &DefaultSlotList;
                   }
		}
		numblocks++;
	}

/* Holidays  entry */

	if(strcmp(Tokens[0]->text,"Holidays")==0) {
/* make sure the  number of remaining tokens
		    on the line is divisible by 5*/
		if((numtokens-1) % 5 )
			myerror(errorstream,"Syntax error in %s\n",linebuf);
		 if(numtokens - 1 <= 0)
		 myerror(errorstream,"Syntax error in %s\n",linebuf);
		parsed = 1;
		if((TimeSlotLists[NumberOfSlotLists]
			 = (struct SlotList *)malloc(sizeof(struct SlotList)))==NULL)
                fatal(stderr,"Failed to allocate memory for Holidays.");      
		if((TimeSlotLists[NumberOfSlotLists]->name = (char *)malloc(SHORTNAME))==NULL)
                  fatal(stderr,"Unable to malloc slotlist name\n");
		if((TimeSlotLists[NumberOfSlotLists]->description = (char *)malloc(LONGNAME))==NULL)
                  fatal(stderr,"Unable to malloc slotlist name\n");
		strcpy(TimeSlotLists[NumberOfSlotLists]->name,"Holidays"); 
		strcpy(TimeSlotLists[NumberOfSlotLists]->description,
			"Holidays");

/* alloc a big enough array of pointers to timeslots */
		if((TimeSlotLists[NumberOfSlotLists]->timeslots =
           (struct TimeSlot **)malloc((numtokens-1)/5*sizeof(struct TimeSlot *)))==NULL)
            fatal(stderr,"Unable to malloc timeslot *array\n");
	        for(i=0;i<(numtokens-1)/5;i++)
		if((TimeSlotLists[NumberOfSlotLists]->timeslots[i] =
                  (struct TimeSlot *)malloc(sizeof(struct TimeSlot)))==NULL)
                  fatal(stderr,"Unable to malloc slotlist \n");


		hold = TimeSlotLists[NumberOfSlotLists]->number = 0;    
		for(i=0;i<(numtokens -1)/5;i++) {
                if((TimeSlotLists[NumberOfSlotLists]->timeslots[hold]->name =
                 (char *)malloc(strlen(Tokens[1+5*i]->text)+1))==NULL)
                  fatal(stderr,"Unable to alloc space for Holiday name");
                  strcpy(TimeSlotLists[NumberOfSlotLists]->timeslots[hold]->name,
                        Tokens[1+5*i]->text);
		  TimeSlotLists[NumberOfSlotLists]->timeslots[hold]->start =
			ParseDate(Tokens[2+5*i]->text,Tokens[3+5*i]->text);
		  TimeSlotLists[NumberOfSlotLists]->timeslots[hold++]->end =
			ParseDate(Tokens[4+5*i]->text,Tokens[5+5*i]->text);
		}
		TimeSlotLists[NumberOfSlotLists]->number = hold;
		NumberOfSlotLists++;

	}
		


/* Prepare to process the next line */
	if(!parsed)
			myerror(errorstream,"Unable to parse timesfile %s\n",linebuf);
		
	ptr = linebuf;
	parsed = 0;
	} 
	if(SemesterStart == 0)
		myerror(errorstream,"Semester Start has not been set.\n");
	if(EndOfClasses == 0)
		myerror(errorstream,"End of Classes has not been set.\n");
return 0;
}

int ParseRoomsFile(){
	char *ptr ;
	struct RoomStruct *rmptr;
	int new_room = 0;
	int parsed = 0;

	ptr = linebuf;
	line = 0;
        new_room = 0;
	numtokens = 0;
/* Loop over lines in roomsfile */
	  while(fgets(ptr,MAXLINE +( linebuf - ptr),istream) != NULL){

		 line++;

/* Now parse the contents of linebuf */

		numtokens = tokenize(linebuf,Tokens);
		if(numtokens == -1){       /* line continues */
		  ptr = ptr + strlen(ptr);
		 continue;
		}
		if(numtokens == 0){
			 ptr = linebuf;
			continue;
		}
		ptr = linebuf;

	if(numtokens != 6)
		myerror(errorstream,"Syntax error.\n");
	
	if(update_flag) {
	krqsort((void **)Rooms,0,numrooms-1,(int(*) (void *,void *))
          CompareRoomsByName);

	strcpy(TRoom.name,Tokens[0]->text);

        if( (rmptr = (struct RoomStruct *) binsearch((void *)&TRoom,
                (void **)Rooms,numrooms,
                (int(*) (void *,void *))CompareRoomsByName)) == NULL){
	  new_room = 1;

	  if((rmptr = (struct RoomStruct *)malloc(sizeof(struct RoomStruct))) == NULL)
           fatal(stderr,"load: Unable to alloc space for room struct.\n");
	  }

	}
	else        /* not an update */
	{
	  if((rmptr = (struct RoomStruct *)malloc(sizeof(struct RoomStruct))) == NULL)
           fatal(stderr,"load: Unable to alloc space for room struct.\n");
	  new_room = 1;
	}

	SAFECPY(rmptr->name,Tokens[0]->text,SHORTNAME);
	SAFECPY(rmptr->bldg,Tokens[1]->text,SHORTNAME);
	SAFECPY(rmptr->number,Tokens[2]->text,NUMBERLEN);
	if(strcmp(Tokens[3]->text,"NA")==0)
		rmptr->seats = 0;
	else rmptr->seats = atoi(Tokens[3]->text);
	if(strcmp(Tokens[4]->text,"NA")==0)
		rmptr->exam_seats = 0;
	else rmptr->exam_seats = atoi(Tokens[4]->text);
	SAFECPY(rmptr->usage,Tokens[5]->text,MAXLINE);
	rmptr->numclasses = 0;
	if(new_room) Rooms[numrooms++] = rmptr;
	parsed = 1;


/* Prepare to process the next line */
	if(!parsed)
		myerror(errorstream,"Unable to parse line %s\n",linebuf);
	
	ptr = linebuf;
	new_room = 0;
	parsed = 0;
	} 

	return 0;
}


int ParseJobsFile(){
	char *ptr ;
	int i,n;
	int new_job = 0;
	struct JobStruct *jbptr;
	int parsed = 0;
	char *tokenptrs[20];
	char *tokenptr;

	ptr = linebuf;
	line = 0;
	numtokens = 0;
        new_job = 0;

/* Loop over lines in jobsfile */
	  while(fgets(ptr,MAXLINE + (linebuf - ptr),istream) != NULL){

		 line++;

/* Now parse the contents of linebuf */

		numtokens = tokenize(linebuf,Tokens);
		if(numtokens == -1){       /* line continues */
		  ptr = ptr + strlen(ptr);
		 continue;
		}
		if(numtokens == 0){
			 ptr = linebuf;
			continue;
		}
		ptr = linebuf;

	if(numtokens != 6)
		myerror(errorstream,"Syntax error. \n");
	if(update_flag) {
	krqsort((void **)Jobs,0,numjobs-1,(int(*) (void *,void *))
          CompareJobsByName);

	strcpy(TJob.name,Tokens[0]->text);

        if( (jbptr = (struct JobStruct *) binsearch((void *)&TJob,
                (void **)Jobs,numjobs,
                (int(*) (void *,void *))CompareJobsByName)) == NULL){
	  new_job = 1;

	  if((jbptr = (struct JobStruct *)malloc(sizeof(struct JobStruct))) == NULL)
           fatal(stderr,"load: Unable to alloc space for job struct.\n");
	  }

	}
	else        /* not an update */
	{
	  if((jbptr = (struct JobStruct *)malloc(sizeof(struct JobStruct))) == NULL)
           fatal(stderr,"load: Unable to alloc space for job struct.\n");
	  new_job = 1;
	}
	SAFECPY(jbptr->name,Tokens[0]->text,SHORTNAME);

/* set up array of appointment types for this job */
/* Token[1] should be either the name of a single appointment type, or
   a | delimited list of appointment types */

        if(!new_job){         /* free this stuff in case it changes */
           for(i=0;i<jbptr->num_types;i++)
                 free(*(jbptr->apt_type+i));
           free(jbptr->apt_type);
        }

	n =  1;

        if((tokenptrs[0] = (char *)malloc(20))==NULL)
                fatal(stderr,"Unable to alloc space for job type tokens\n");
	strcpy(tokenptrs[0],strtok(Tokens[1]->text,"|"));
	while((tokenptr = strtok(NULL,"|")) != NULL){
                tokenptrs[n++] = (char *)malloc(20);
		strcpy(tokenptrs[n-1],tokenptr);
	}
	jbptr->num_types = n;
        if((jbptr->apt_type = (char **)malloc(n*(sizeof ptr))) ==NULL)
        fatal(stderr,"load: Unable to alloc space for job apt_type array.\n");
	for(i=0;i<n;i++){
                if((*(jbptr->apt_type+i) = (char *)malloc(20)) == NULL)
        fatal(stderr,"load: Unable to alloc space for job apt_type.\n");
		strcpy(*(jbptr->apt_type+i),tokenptrs[i]);
	}
       for(i=0;i<n;i++) free(tokenptrs[i]);          
	
	if(strcmp(Tokens[2]->text,"NA")==0)
		jbptr->start  = SemesterStart;
	else 
		jbptr->start = ParseDate(Tokens[2]->text,"0:00");       
	if(strcmp(Tokens[3]->text,"NA")==0)
		jbptr->end = EndOfSemester;
	else 
		jbptr->end = ParseDate(Tokens[3]->text,"0:00"); 

	jbptr->load = atof(Tokens[4]->text);
	SAFECPY(jbptr->description,Tokens[5]->text,MAXLINE);
	if(new_job) Jobs[numjobs++] = jbptr;
	parsed = 1;


/* Prepare to process the next line */
	if(!parsed)
		myerror(errorstream,"Unable to parse line %s\n",linebuf);
	
	ptr = linebuf;
	parsed = 0;
        new_job = 0;
	} 

	return 0;
}



int ParsePeopleFile(){
	char *ptr ;
	int i;
	struct PersonStruct *jbptr;
	int new_person = 0;
	int parsed = 0;
	int found;

	ptr = linebuf;
	line = 0;
	numtokens = 0;
        new_person = 0;

/* Loop over lines in peoplefile */
	  while(fgets(ptr,MAXLINE + linebuf - ptr ,istream) != NULL){

		 line++;

/* Now parse the contents of linebuf */

		numtokens = tokenize(linebuf,Tokens);
		if(numtokens == -1){       /* line continues */
		  ptr = ptr + strlen(ptr);
		 continue;
		}
		if(numtokens == 0){
			 ptr = linebuf;
			continue;
		}
		ptr = linebuf;

	if(numtokens != 9)
		myerror(errorstream,"Syntax error. \n");
	
if(update_flag) {
	krqsort((void **)People,0,numpeople-1,(int(*) (void *,void *))
          ComparePeopleByShortName);

	strcpy(TPerson.shortname,Tokens[0]->text);

        if( (jbptr = (struct PersonStruct *) binsearch((void *)&TPerson,
                (void **)People,numpeople,
                (int(*) (void *,void *))ComparePeopleByShortName)) == NULL){
	  new_person = 1;

	  if((jbptr = (struct PersonStruct*)malloc(sizeof(struct PersonStruct))) == NULL)
           fatal(stderr,"load: Unable to alloc space for person struct.\n");
	  }

	}
	else        /* not an update */
	{
	  if((jbptr = (struct PersonStruct *)malloc(sizeof(struct PersonStruct))) == NULL)
           fatal(stderr,"load: Unable to alloc space for person struct.\n");
	  new_person = 1;
	}

	SAFECPY(jbptr->shortname,Tokens[0]->text,SHORTNAME);
	SAFECPY(jbptr->apt_type,Tokens[1]->text,SHORTNAME);
	if(strcmp(Tokens[2]->text,"NA")==0)
		jbptr->start  = SemesterStart;
	else 
		jbptr->start = ParseDate(Tokens[2]->text,"0:00");       
	if(strcmp(Tokens[3]->text,"NA")==0)
		jbptr->end = EndOfSemester;
	else 
		jbptr->end = ParseDate(Tokens[3]->text,"0:00"); 

	jbptr->seniority = atoi(Tokens[4]->text);
		jbptr->load_now = atof(Tokens[5]->text);
	jbptr->load_sem = atof(Tokens[6]->text);
	jbptr->load_ay = atof(Tokens[7]->text);
/* Update loads array */
	if(new_person || (update_flag == 0)){
		jbptr->numloads = 1;
		 if((jbptr->loads[0] = (struct LoadStruct *)malloc(sizeof (struct LoadStruct))) == NULL)
                        fatal(stderr,"load: unable to alloc space for load struct \n");
		strcpy(jbptr->loads[0]->semester,Semester);
		jbptr->loads[0]->load = jbptr->load_sem;
	}
	else {    /* see if there is already a load entry for this semester */
		found = 0;
		for(i=0;i<jbptr->numloads;i++)
			if(strcmp(jbptr->loads[i]->semester,Semester)==0){
				found = 1;
				break;
			}
		if(found)jbptr->loads[i]->load = jbptr->load_sem;
		else {
		  if((jbptr->loads[jbptr->numloads] =
			(struct LoadStruct *)malloc(sizeof (struct LoadStruct))) == NULL)
                   fatal(stderr,"load: unable to alloc space for load struct\n");
		  strcpy(jbptr->loads[jbptr->numloads]->semester,Semester);
		  jbptr->loads[jbptr->numloads++]->load = jbptr->load_sem;
		}
	} 
	jbptr->numclasses = 0;
	jbptr->numassignments = 0;
	SAFECPY(jbptr->longname,Tokens[8]->text,LONGNAME);
	if(new_person) People[numpeople++] = jbptr;
	parsed = 1;


/* Prepare to process the next line */
	if(!parsed)
		myerror(errorstream,"Unable to parse line %s\n",linebuf);
	
	ptr = linebuf;
	new_person = 0;
	parsed = 0;
	} 

	return 0;
}


		

/* Break Line into tokens delimited by DELIMITER and return number of
      tokens found. Return -1 if more input is needed. Implements
	metacharacter escapes, quoting, and variable substitution
*/
/* This is an absolutely central routine for the operation of all the
   parsers in this file, as well as of the command line shell */

int tokenize(char *line,struct TokenType **tokens)
{
	int count = 0;
	static int debug_count = 0;
	static int debug_break;
	static int first = 1;
	char *ptr,*ptrhold;
	char buffer[MAXLINE];
	char tokenbuf[MAXLINE]; /* hold a token while building */
	char holdbuffer[MAXLINE];
	enum {NORMAL, ESCAPED};
	char  *bufptr, *cptr;
	int state = NORMAL;
	int len;  /* Used during variable expansion */


	SAFECPY(buffer,line,MAXLINE);

	if(buffer == NULL) return 0;

	if(debug){
		if(first){
			fprintf(errorstream,"Debug mode: echo every data and command line\n");
			fprintf(errorstream,"Enter number lines to next break, or 0 if no break\n");
			scanf("%d",&debug_break);
			first = 0;
		}
		myerror(errorstream,"%s\n",buffer);
		if(debug_break != 0 && debug_count == debug_break){
			fprintf(errorstream,"Enter number lines to next break, or 0 if no further break\n");
			scanf("%d",&debug_break);
			debug_count = 0;
		}
		++debug_count;
	}

	ptr = buffer; 
	bufptr = holdbuffer;


		if(strlen(buffer)>=MAXLINE){ 
		myerror(errorstream,"Line %s too long\n",buffer);
			return 0;
		}
		
/* Skip comment lines and lines starting with \n */

		if((buffer[0] == COMMENT)||(buffer[0]=='\n'))
			return 0;


/* First pass: collapse all multiple escapes. Determine whether newline
is escaped and request more input if so. Expand shell variables. */             
/* The processed line ends up in holdbuf */


	while( *ptr != '\0'){
		if(bufptr - holdbuffer >= MAXLINE-1){
			myerror(errorstream,"Line %s became too long during variable expansion\n",buffer);
			break;
		}
		if(state == NORMAL)
	  /* Is it a meta char ? if not, just copy */
			if((cptr = strchr(METACHARS,*ptr)) == NULL)
				*bufptr++ = *ptr++;
			else {  /* if so, see which one */
				switch(*cptr) {
				case '\\':
					if(strchr(METACHARS,*(++ptr))==NULL)
					*bufptr++ = '\\';   /* skip it */
					else state = ESCAPED;
					break;
				case '$':
/* If the next character is a { grab everything up to } as a token. It should
be the name of an existing shell variable. If not, complain and copy. */
					if(*(ptr+1)=='{') {
						len=strcspn(ptr+2,"}");
						strncpy(tokenbuf,ptr+2,len);
						ptr=ptr+2;
					}
					else {
/* Grab stuff till next whitespace or quote as a token. It should be the name of
    an existing shell variable. If not, complain and copy (no substitution. */
					len = strcspn(ptr+1,"\" \t\n");
					strncpy(tokenbuf,ptr+1,len);
					}
					*(tokenbuf + len) = '\0';
					if(GetShell(tokenbuf) == NULL){
					   myerror(errorstream,"No such shell variable %s\n",tokenbuf);
		
					  *bufptr++ = *ptr++;
					   break;
					}
					strcpy(bufptr,GetShell(tokenbuf));
					ptr += len + 1;
					bufptr += strlen(GetShell(tokenbuf));
					break;
				 default:
					*bufptr++ = *ptr++;
				}
			}
		else  { /* state is ESCAPED */
			switch(*ptr){
				case '\n': /* substitute a space */
				       *bufptr++ = ' ';
					ptr++;
					break;
				case '\\':
					/* if escaped backslash followed
					   by a quote, insert whitespace */
					*bufptr++ = *ptr++;
					if(*ptr=='\"')
					*bufptr++ = ' ';
					break;
				case '\"': /* special case: keep escape in 
					    front of a quote -- it gets
					    handled on the next pass. */
					*bufptr++ = '\\';
					*bufptr++ = *ptr++;
					break;
				default:
				*bufptr++ = *ptr++;
			}
				state = NORMAL;
		}
	}

/* properly terminate buffer */
	*bufptr = '\0';
/* copy the hold buffer back over the original one */
	strcpy(buffer, holdbuffer);

/* Check whether last character was an escaped newline */
	if(*(buffer+strlen(buffer)-1)!='\n')
		return -1; /* need more input */


/* OK, now all escapes have been fixed. */

/* Next find all quote delimited tokens */


/* Find the first unescaped quote */

	ptr = buffer;
	while(((ptr = strchr(ptr,'\"'))!=NULL) &&
		(ptr > buffer) && (*(ptr-1)=='\\')){
			*(ptr-1)=' ';  /* change \ to blank */
			ptr++;
		}
	while(ptr != NULL){   /* ptr pts at leading quote */
		ptrhold = ptr++;  /* save it */

/* advance to next unescaped quote */ 
		while(((ptr = strchr(ptr,'\"'))!=NULL) &&
		  (*(ptr-1)=='\\')){
			*(ptr-1) = ' ';  /* change \ to blank */
			ptr++;
		}

	if(ptr == NULL){
		myerror(errorstream,"Unmatched \" in %s\n",buffer);
		return 0;
	}

/* ptr now points at close quote. blank and copy into token buffer */

	*ptr = '\0';         /* mark end of string */
	*ptrhold++ = ' ';    /* convert leading quote to whitespace */
	SAFECPY((*tokens)->text,ptrhold,MAXTOKEN);
	(*tokens++)->position = ptrhold - buffer;
	if(count >= MAXTOKS){
			myerror(errorstream,"Too many tokens. Must be < %d\n",
				MAXTOKS);
			return count;
		}

	count++;
	while(ptrhold <= ptr)   /* convert token just found to whitespace */
	   *ptrhold++ = ' ';
/* advance to next unescaped quote */ 
	while(((ptr = strchr(ptr,'\"'))!=NULL) &&
	  (*(ptr-1)=='\\')){
		*(ptr-1) = ' ';
		ptr++;
		}
	}


/* Get tokens delimited by white space */

	ptr = strtok(buffer,DELIMITER);
	while(ptr != NULL){
		if(count >= MAXTOKS){
			myerror(errorstream,"Too many tokens. Must be < %d\n",
				MAXTOKS);
			return count;
		}
		SAFECPY((*tokens)->text,ptr,MAXTOKEN);
		(*tokens++)->position = ptr -buffer;
		count++;
		ptr = strtok(NULL,DELIMITER);
	}

/* Sort the tokens into the order they occured */
	krqsort((void **)Tokens,0,count-1,
	  (int(*) (void *,void *))CompareTokensByPosition);
	return count;
}


/* Free Class: A class contains pointers to classes, so they must be freed
    recursively.
*/

int FreeClass( struct ClassStruct *aclass)
{
	int i;
	if(aclass->numclasses == 0) {
		free(aclass);
		return 0;
	}
	for(i=0;i<aclass->numclasses;i++)
		FreeClass(aclass->classes[i]);
	free(aclass);
	return 0;
}

int FreeSlotList(struct SlotList *alist)
{
	int i;

	if(alist == NULL)return 1;
        for(i=0;i<alist->number;i++){
                if(strcmp(alist->name,"Holidays")==0)
                  free(alist->timeslots[i]->name);
		free(alist->timeslots[i]);
        }
	free(alist->timeslots);
	free(alist->name);
	free(alist->description);
	free(alist);
	return 0;
}

int
find_opt(char *word)
{
	if(strcmp(word,"t")==0)return 0;
	if(strcmp(word,"u")==0)return 1;
	if(strcmp(word,"ut")==0) return 2;
	if(strcmp(word,"tu")==0) return 2;
	if(strcmp(word,"sem") ==0) return 3;
	if(strcmp(word,"free")==0)return 4;

	/* arg not found */
	return -2;
}
