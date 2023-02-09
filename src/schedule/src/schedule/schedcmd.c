/* schedcmd.c: implementation of shell commands which manipulate a schedule */


#include "shell.h"

extern struct ScheduleStruct *Schedules[];
extern struct ScheduleStruct *CurrentSchedule;
extern struct CourseStruct *CurrentCourse;
extern struct ClassStruct *CurrentClass;
extern struct ClassStruct *PendingClass[];
extern struct AssignStruct *CurrentAssignment;

extern int ClassLevel;

/* forward declarations */
int PrintClass(struct ClassStruct *, int,FILE *);
int PrintCourse(struct CourseStruct *, FILE *);
int PrintSchedule(struct ScheduleStruct *, FILE *);
int CanDo(struct PersonStruct *, struct JobStruct *);

static struct ScheduleStruct TSchedule;
static struct JobStruct TJob;
static struct PersonStruct TPerson;
static struct ClassStruct TClass;
static struct CourseStruct TCrs;
static struct RoomStruct TRoom;
static struct SlotList TSlot;
static struct FinalBlockStruct TBlock;

static char buffer[BUFSIZ];

int schedule(SHELLSTREAMS)
{
	struct ScheduleStruct *schedptr;
	int i;

	if(numtokens == 3 && strcmp(Tokens[1]->text,"-c")==0){
	strcpy(TSchedule.semester,Tokens[2]->text);
/* Search for the schedule with given semester name */
	krqsort((void **)Schedules,0,numschedules-1,(int(*) (void *,void *))
                  CompareSchedulesBySemester);
	if( (schedptr = (struct ScheduleStruct *) binsearch((void *)&TSchedule,
                (void **)Schedules,numschedules,
                (int(*) (void *,void *))CompareSchedulesBySemester)) == NULL){
		myerror(shell_errorstream,"No schedule for semester %s\n",
			Tokens[2]->text);
		return 0;
	}
	CurrentSchedule = schedptr;
	set("SEM",Tokens[2]->text,3,shell_ostream);
	return 0;
	}

	if(numtokens == 1) {
			fprintf(shell_ostream,"Currently loaded schedules:\n");
		for(i=0; i<numschedules; i++)
			fprintf(shell_ostream,"%d. %s\n",i,Schedules[i]->semester);
		return 0;
	}

	if(numtokens == 2) {
/* Search for the schedule with given semester name */
	strcpy(TSchedule.semester,Tokens[1]->text);
        krqsort((void **)Schedules,0,numschedules-1,(int(*) (void *,void *))
                  CompareSchedulesBySemester);
        if( (schedptr = (struct ScheduleStruct *) binsearch((void *)&TSchedule,
                (void **)Schedules,numschedules,
                (int(*) (void *,void *))CompareSchedulesBySemester)) == NULL){
                myerror(shell_errorstream,"No schedule for semester %s\n",
                        Tokens[1]->text);
		return 0;
	}
/* sort courses in source order for display */
	krqsort((void **)schedptr->Courses,0,schedptr->numcourses-1,(int(*) (void *,void *))
          	CompareCoursesBySeqno);

	fprintf(shell_ostream,"%-69s",schedptr->name);
	fprintf(shell_ostream,"%s\n\n",Date);
	fprintf(shell_ostream,"%s\n","COURSES:");
	for(i=0;i<schedptr->numcourses;i++)
		PrintCourse(schedptr->Courses[i],shell_ostream);
	fprintf(shell_ostream,"\n%s\n","STAND-ALONE CLASSES:");
	for(i=0;i<schedptr->numclasses;i++)
		PrintClass(schedptr->classes[i],1,shell_ostream);
	fprintf(shell_ostream,"\n%s\n","DEPARTMENTAL ASSIGNMENTS:");
	for(i=0;i<schedptr->numassignments;i++){
                 fprintf(shell_ostream,"%s\t%s\t%s\n",
                	 schedptr->assignments[i]->name,
                                	schedptr->assignments[i]->holder->shortname,
                                	schedptr->assignments[i]->job->name);
		}
	return 0;
	}


	if(numtokens == 3){
                if((Schedules[numschedules] = (struct ScheduleStruct *)malloc(sizeof(struct ScheduleStruct)))==NULL)
			fatal(shell_ostream,"Unable to alloc schedule structure\n");
		CurrentSchedule = Schedules[numschedules++];
		set("SEM",Tokens[1]->text,3,shell_ostream);
		strcpy(CurrentSchedule->semester,Tokens[1]->text);
		strcpy(CurrentSchedule->name,Tokens[2]->text);
		CurrentSchedule->numclasses = 0;
		CurrentSchedule->numassignments = 0;
		CurrentSchedule->numcourses = 0;
		return 0;
	}
	return 1;
}



int course(SHELLSTREAMS)
{
	int i;
	struct CourseStruct  *CrsPtr;
	struct PersonStruct  *PPtr;

	if(numschedules == 0) {
		myerror(shell_errorstream,"No schedule is loaded\n");
		return 0;
	}
	if(numtokens == 2 && strcmp(Tokens[1]->text,"end")==0){
		set("COURSE","",2,shell_ostream);
		return 0;
	}
	if(numtokens == 1){ /* display existing courses */
		krqsort((void **)CurrentSchedule->Courses,0,CurrentSchedule->numcourses-1,(int(*) (void *,void *))
          	CompareCoursesBySeqno);

		for(i=0;i<CurrentSchedule->numcourses;i++){
		     CrsPtr = CurrentSchedule->Courses[i];
                     fprintf(shell_ostream,"\n%s",CrsPtr->name);
                        if(CrsPtr->supervisor != NULL)
                                fprintf(shell_ostream," Supervisor: %s ",
                                        CrsPtr->supervisor->longname);
                        fprintf(shell_ostream,"( %d classes, %d assignments )",
                                CrsPtr->numclasses,
                                CrsPtr->numassignments);
                        fprintf(shell_ostream,"\n");
		}


		return 0;
	}
	if(numtokens == 2){

		/* see if there is an existing course by this name */

                krqsort((void **)CurrentSchedule->Courses,0,CurrentSchedule->numcourses-1,
		(int(*) (void *,void *)) CompareCoursesByName);
                SAFECPY(TCrs.name,Tokens[1]->text,SHORTNAME);
                if((CrsPtr = binsearch((void *)&TCrs,(void **)CurrentSchedule->Courses,CurrentSchedule->numcourses,
                (int(*) (void *,void *))CompareCoursesByName))==NULL){
                        myerror(shell_errorstream,"No such course: %s\n",
                                Tokens[1]->text);
                        return 0;
                }

                /* else */
	/* Print info about course */

		/* set the COURSE shell variable and clear CLASS variable*/
		set("COURSE",Tokens[1]->text,3,shell_ostream);
		set("CLASS","",2,shell_ostream);
		CurrentCourse = CrsPtr;

		PrintCourse(CrsPtr,shell_ostream);
                return 0;
	}

	if(numtokens > 3) return 1;
;
/* set course shell variable and clear CLASS variable */
       set("COURSE",Tokens[1]->text,3,shell_ostream);
       set("CLASS","",2,shell_ostream);
      
 /* see if there is an existing course by this name */
        krqsort((void **)CurrentSchedule->Courses,0,CurrentSchedule->numcourses-1,(int(*) (void *,void *))
	  CompareCoursesByName);
	SAFECPY(TCrs.name,Tokens[1]->text,SHORTNAME);
	if((CrsPtr = binsearch((void *)&TCrs,(void **)CurrentSchedule->Courses,CurrentSchedule->numcourses,
                (int(*) (void *,void *))CompareCoursesByName))==NULL) 
/* new course */
                 
            {
		if(CurrentSchedule->numcourses >= MAX_COURSES - 1){
			myerror(shell_errorstream,"Too many courses. %s not added.\n",
				Tokens[1]->text);
			return 0;
		}
		if((CrsPtr = (struct CourseStruct *)malloc(sizeof(struct CourseStruct)))==NULL){
			fatal(shell_errorstream,"Unable to malloc CourseStruct\n");
			return 0;
              }  
		CrsPtr->seqno = CurrentSchedule->numcourses;
		CrsPtr->enrollment = 0;
		strcpy(CrsPtr->comment,"");
		SAFECPY(CrsPtr->name,Tokens[1]->text,SHORTNAME);
		if(numtokens == 3) {
		if(strcmp(Tokens[2]->text,MissingData)!=0){
		 SAFECPY(TPerson.shortname,Tokens[2]->text,SHORTNAME);
		 krqsort((void **)People,0,numpeople-1,(int(*) (void *,void *)) ComparePeopleByShortName);
		 if((PPtr = binsearch((void*)&TPerson,(void **) People,numpeople,
			(int(*) (void *,void *))ComparePeopleByShortName)) == NULL ){
			myerror(shell_errorstream,"No such person %s\n",Tokens[2]->text);
			CrsPtr->supervisor = &(DefaultPerson);
		 }
		 else CrsPtr->supervisor = PPtr;
		}
		else CrsPtr->supervisor = &(DefaultPerson);
		CrsPtr->numclasses = 0;
		CrsPtr->numassignments = 0;
		CurrentSchedule->Courses[CurrentSchedule->numcourses++] = CrsPtr;
		}
	}
	else {
/* Existing Course */
		/* If no supervisor listed, I guess we remove existing one */
		if(numtokens == 2) CrsPtr->supervisor = &(DefaultPerson);
		if(numtokens == 3){
			if(strcmp(Tokens[2]->text,MissingData)!=0){
			  SAFECPY(TPerson.shortname,Tokens[2]->text,SHORTNAME);
			  krqsort((void **)People,0,numpeople-1,(int(*) (void *,void *)) ComparePeopleByShortName);
			  if((PPtr = binsearch((void *)&TPerson,(void **)People,
				numpeople,(int(*) (void *,void *))ComparePeopleByShortName)) == NULL )       
                          {
			     myerror(shell_errorstream,"No such person %s\n",Tokens[2]->text);
			     PPtr = &(DefaultPerson);
			  }
			  CrsPtr->supervisor = PPtr;
			}
			else CrsPtr->supervisor  = &(DefaultPerson);
		}
	}
	CurrentCourse = CrsPtr;
	return 0;
}


int class(SHELLSTREAMS) {
	
	int j;
	int temp;
	int newclass;
	struct ClassStruct **searchclasses;
	int searchnum;
	struct ClassStruct  *ClsPtr;
	time_t first,second;


	if(numschedules == 0) {
		myerror(shell_errorstream,"No schedule is loaded\n");
		return 0;
	}
	newclass = 0;


	if(numtokens==1){  /* Dump all classes of current course or class  */

		if(ClassLevel == 0)
		PrintClass((struct ClassStruct *)NULL,0,shell_ostream);
		else
		PrintClass(PendingClass[ClassLevel-1],ClassLevel,shell_ostream);
		return 0;
	}

/* Search for this class name in the current course/class: malloc if necessary */
	strcpy(TClass.name,Tokens[1]->text);
	if(ClassLevel > 0){
		searchclasses = PendingClass[ClassLevel-1]->classes;
		searchnum = PendingClass[ClassLevel-1]->numclasses;
	}
	else {
		if(GetShell("COURSE") != NULL){
		 searchclasses = CurrentCourse->classes;
		 searchnum    = CurrentCourse->numclasses;
		}
		else {
			searchclasses = CurrentSchedule->classes;
			searchnum = CurrentSchedule->numclasses;
		}
	}
	krqsort((void **)searchclasses,0,searchnum-1,(int(*) (void *,void *))
	  CompareClassesByName);
	if((ClsPtr = binsearch((void *)&TClass,(void **)searchclasses,searchnum,
		(int(*) (void *,void *))CompareClassesByName))==NULL){

/* new class */

	newclass = 1;
	if(numtokens == 2){ 
		myerror(shell_errorstream,"New class %s\n",Tokens[1]->text);
		return 1;
	}
	if (numtokens > 7 || numtokens < 6)return 1;

/* Create New Class */

	if((ClsPtr = (struct ClassStruct *)malloc(sizeof(struct ClassStruct)))==NULL){
			fatal(shell_errorstream,"Unable to malloc ClassStruct\n");
			return 0;
		}
        strcpy(ClsPtr->refno,"TBA");
	SAFECPY(ClsPtr->name,Tokens[1]->text,SHORTNAME);
	strcpy(ClsPtr->comment,"");
	ClsPtr->numclasses = 0;
	ClsPtr->attach = 0;
	ClsPtr->numassignments = 0;
        ClsPtr->finalroom = &DefaultRoom;
	ClsPtr->final = &DefaultFinalBlock;
	}

	if(numtokens == 2){
/* Print this class and sub classes */
		PrintClass(ClsPtr,ClassLevel + 1,shell_ostream);
		set("CLASS",Tokens[1]->text,3,shell_ostream);
		CurrentClass = ClsPtr;
		return 0;
	}

	if (numtokens > 7 || numtokens < 6)return 1;
         ClsPtr->available = 1;
	ClsPtr->enrollment = 0;
	 if(numtokens == 7 )
	  if(strcmp(Tokens[6]->text,"N")==0)
		ClsPtr->available = 0;
	  else { 
		/* enrollment is being entered */
		j = atoi(Tokens[6]->text);
                /* Sanity Check value of j */
                if(j<0) myerror(shell_errorstream,"Bad enrollment value: %d\n",j);
		else
			ClsPtr->enrollment = j;
	  }
		


/* Search for the room */

	if(strcmp(Tokens[2]->text,MissingData)==0)
		ClsPtr->room = &DefaultRoom;
	else {
	SAFECPY(TRoom.name,Tokens[2]->text,SHORTNAME);
	 krqsort((void **)Rooms,0,numrooms-1,(int(*) (void *,void *))
	  CompareRoomsByName);
	ClsPtr->room = binsearch((void *)&TRoom,(void **)Rooms,numrooms,
		(int(*) (void *,void *))CompareRoomsByName);
	if(ClsPtr->room == NULL){myerror(shell_errorstream,
	 "class: no such room %s\n",Tokens[2]->text);
	 ClsPtr->room = &DefaultRoom;
	 }
	}

/* Search for the Job */

	if(strcmp(Tokens[5]->text,MissingData)==0)
		ClsPtr->job = &DefaultJob;
	else {
	SAFECPY(TJob.name,Tokens[5]->text,SHORTNAME);
	 krqsort((void **)Jobs,0,numjobs-1,(int(*) (void *,void *))
	  CompareJobsByName);
	ClsPtr->job = binsearch((void *)&TJob,(void **)Jobs,numjobs,
		(int(*) (void *,void *))CompareJobsByName);
	if(ClsPtr->job == NULL){myerror(shell_errorstream,
	 "class: no such job %s\n",Tokens[5]->text);
		ClsPtr->job = &DefaultJob;
	}
	}



/* Search for the Teacher */

	if(strcmp(Tokens[3]->text,MissingData)==0)
		ClsPtr->teacher = &DefaultPerson;
	else {
	SAFECPY(TPerson.shortname,Tokens[3]->text,SHORTNAME);
	 krqsort((void **)People,0,numpeople-1,(int(*) (void *,void *))
	  ComparePeopleByShortName);
	ClsPtr->teacher = binsearch((void *)&TPerson,(void **)People,numpeople,
		(int(*) (void *,void *))ComparePeopleByShortName);
	if(ClsPtr->teacher == NULL){myerror(shell_errorstream,
	 "class: no such person %s\n",Tokens[3]->text);
	 ClsPtr->teacher = &DefaultPerson;
	 }

	else {  /* update load for this person */
		ClsPtr->teacher->load_now += ClsPtr->job->load;
		if(ClsPtr->teacher->load_now > ClsPtr->teacher->load_sem)
			myerror(shell_errorstream,"Semester load %g for %s exceeded: %g\n",
				ClsPtr->teacher->load_sem,ClsPtr->teacher->shortname,
				ClsPtr->teacher->load_now);
		if(ClsPtr->teacher->load_now > ClsPtr->teacher->load_ay)
			myerror(shell_errorstream,"AY load %g for %s exceeded: %g\n",
				ClsPtr->teacher->load_ay,ClsPtr->teacher->shortname,
				ClsPtr->teacher->load_now);

		/* Check that this person can do the job */

		if(!CanDo(ClsPtr->teacher,ClsPtr->job))
		   if(strcmp(ClsPtr->teacher->shortname,MissingData)!=0)
			myerror(shell_errorstream,"Appt %s of %s can't do job %s\n",
				ClsPtr->teacher->apt_type,
				ClsPtr->teacher->shortname,
				ClsPtr->job->name);
	}
	}

/* Search for the Time Slot */

	if(strcmp(Tokens[4]->text,MissingData)==0)
		ClsPtr->times = &DefaultSlotList;
	else {
	TSlot.name = common_buf;
	SAFECPY(TSlot.name,Tokens[4]->text,SHORTNAME);
	 krqsort((void **)TimeSlotLists,0,NumberOfSlotLists-1,(int(*) (void *,void *))
	  CompareSlotsByName);
	ClsPtr->times = binsearch((void *)&TSlot,
	(void **)TimeSlotLists,NumberOfSlotLists,
		(int(*) (void *,void *))CompareSlotsByName);
	if(ClsPtr->times == NULL){myerror(shell_errorstream,
	 "class: no such time slot %s\n",Tokens[4]->text);
	 ClsPtr->times = &DefaultSlotList;
	}
	}


	if(GetShell("COURSE") != NULL)
	 strcpy(ClsPtr->crse_name,CurrentCourse->name);
	else
	strcpy(ClsPtr->crse_name,"");

/* DATA CONSISTENCY CHECKS */

/*  two classes don't meet in same room at same time */
/*  also calculate max enrollment that meets in room */
	if(strcmp(ClsPtr->room->name,MissingData)!=0){
        temp = ClsPtr->enrollment;       
	for(j=0;j<ClsPtr->room->numclasses;j++)
		if(SlotsDoOverlap(ClsPtr->times,
		((struct ClassStruct *) (ClsPtr->room->classes[j]))->times,&first,&second)){
			strcpy(common_buf,ctime(&first));
myerror(shell_errorstream,"%s %s meeting at %s overlaps with %s %s meeting at %s\n",CurrentCourse->name, ClsPtr->name,
common_buf,
((struct ClassStruct *)(ClsPtr->room->classes[j]))->crse_name,
((struct ClassStruct *)(ClsPtr->room->classes[j]))->name,ctime(&second));
                temp += ClsPtr->room->classes[j]->enrollment;
		}
	}

/* Check enrollment against room capacity */

	if(strcmp(ClsPtr->room->name,MissingData)!=0){
         if(temp > ClsPtr->room->seats)
          myerror(shell_errorstream,"%s %s enrollment %d > seating capacity of %s: %d\n",
           ClsPtr->crse_name,ClsPtr->name,temp,
           ClsPtr->room->name,ClsPtr->room->seats);
         if(temp > ClsPtr->room->exam_seats)
          myerror(shell_errorstream,"%s %s enrollment %d > exam capacity of  %s: %d\n",
           ClsPtr->crse_name,ClsPtr->name,temp,
           ClsPtr->room->name,ClsPtr->room->exam_seats);
	 }

/* Check that a person does not have to be two different places at once */

	if(strcmp(ClsPtr->room->name,MissingData)!=0 &&
		strcmp(ClsPtr->teacher->shortname,MissingData)!=0)
	for(j=0;j<ClsPtr->teacher->numclasses;j++)
		if(SlotsDoOverlap(ClsPtr->times,
		((struct ClassStruct *) (ClsPtr->teacher->classes[j]))->times,&first,&second)&&
			(strcmp(ClsPtr->room->name,
		((struct ClassStruct *)(ClsPtr->teacher->classes[j]))->room->name)!=0)&&
		(strcmp(((struct ClassStruct *)(ClsPtr->teacher->classes[j]))->room->name,
			MissingData)!= 0)){
		myerror(shell_errorstream,"%s has to be two places at once: %s and %s\n",
			ClsPtr->teacher->shortname,ClsPtr->room->name,
		((struct ClassStruct *)(ClsPtr->teacher->classes[j]))->room->name);
		myerror(shell_errorstream,"Class %s %s not added.\n",
			CurrentCourse->name,ClsPtr->name);
		free(ClsPtr);
		return 0;
		}

/* Attach the current class to the current schedule,course or to the current class
   at the right recursive level */

	if(GetShell("COURSE") != NULL){
	if(ClassLevel > 0){
		if(PendingClass[ClassLevel-1]->numclasses >= MAX_SECTIONS -1){
		  myerror(shell_errorstream,"%s has too many sections\n",
			PendingClass[ClassLevel-1]->name);
		  free(ClsPtr);
		  return 0;
		}
		if(newclass)
		PendingClass[ClassLevel-1]->classes[PendingClass[ClassLevel-1]->numclasses++]=ClsPtr;
	}

	else { 
		if(CurrentCourse->numclasses >= MAX_SECTIONS -1){
		CurrentCourse->classes[CurrentCourse->numclasses]=ClsPtr;
		  myerror(shell_errorstream,"%s has too many sections\n",
			CurrentCourse->name);
		  free(ClsPtr);
		  return 0;
		}
		if(newclass)
		CurrentCourse->classes[CurrentCourse->numclasses++]=ClsPtr;
	}
	}
	else 
		if(newclass)
		CurrentSchedule->classes[CurrentSchedule->numclasses++] = ClsPtr;

	set("CLASS",ClsPtr->name,3,shell_ostream);
	CurrentClass = ClsPtr;

/* Make an entry for this class in this teachers's list of classes */

	if(ClsPtr->teacher != NULL && strcmp(ClsPtr->teacher->shortname,
		MissingData)!= 0){
	if(ClsPtr->teacher->numclasses >= MAXCLASSESPERPERSON -1){
		myerror(shell_errorstream,"%s has too many classes\n",
		ClsPtr->teacher->shortname);
		return 0;
	}
        if(strcmp(ClsPtr->teacher->shortname,MissingData)!=0)
	ClsPtr->teacher->classes[ClsPtr->teacher->numclasses++] =
		ClsPtr;
	}

/* Make an entry for this class in the room's list of classes */

	if(ClsPtr->room != NULL && strcmp(ClsPtr->room->name,MissingData)!=0){
	if(ClsPtr->room->numclasses >= MAXCLASSESPERROOM -1){
		myerror(shell_errorstream,"%s has too many classes\n",
		ClsPtr->room->name);
		return 0;
	}
	ClsPtr->room->classes[ClsPtr->room->numclasses++] =
		ClsPtr;
	}


	if(GetShell("COURSE") != NULL && ClsPtr->attach == 0){
	 if(ClassLevel == 0){
	   /* Update current course's enrollments */
	   CurrentCourse->enrollment = 0;
           for(j=0;j<CurrentCourse->numclasses;j++){
                if(CurrentCourse->classes[j]->attach == 0)
		CurrentCourse->enrollment += CurrentCourse->classes[j]->enrollment;
	   }
	  }
	else {
	   temp = PendingClass[ClassLevel-1]->enrollment;
	   PendingClass[ClassLevel-1]->enrollment = 0;
	   for(j=0;j<PendingClass[ClassLevel-1]->numclasses;j++){
		if(PendingClass[ClassLevel-1]->classes[j]->attach == 0)
	         PendingClass[ClassLevel-1]->enrollment +=
                  PendingClass[ClassLevel-1]->classes[j]->enrollment;
	   }
	   if(PendingClass[ClassLevel-1]->enrollment == 0)
              PendingClass[ClassLevel-1]->enrollment = temp;
	 }
	}

	return 0;
}

int attach(SHELLSTREAMS) {
	
	int j;
	int newclass;
	struct ClassStruct **searchclasses;
	int searchnum;
	struct ClassStruct  *ClsPtr;
	time_t first,second;


	if(numschedules == 0) {
		myerror(shell_errorstream,"No schedule is loaded\n");
		return 0;
	}
	newclass = 0;


/* Search for this class name in the current course/class: malloc if necessary */
	strcpy(TClass.name,Tokens[1]->text);
	if(ClassLevel > 0){
		searchclasses = PendingClass[ClassLevel-1]->classes;
		searchnum = PendingClass[ClassLevel-1]->numclasses;
	}
	else {
		if(GetShell("COURSE") != NULL){
		 searchclasses = CurrentCourse->classes;
		 searchnum    = CurrentCourse->numclasses;
		}
		else {
			searchclasses = CurrentSchedule->classes;
			searchnum = CurrentSchedule->numclasses;
		}
	}
	krqsort((void **)searchclasses,0,searchnum-1,(int(*) (void *,void *))
	  CompareClassesByName);
	if((ClsPtr = binsearch((void *)&TClass,(void **)searchclasses,searchnum,
		(int(*) (void *,void *))CompareClassesByName))==NULL){

/* new class */

	newclass = 1;

	if (numtokens > 7 || numtokens < 6)return 1;

/* Create New Class */

	if((ClsPtr = (struct ClassStruct *)malloc(sizeof(struct ClassStruct)))==NULL){
			fatal(shell_errorstream,"Unable to malloc ClassStruct\n");
			return 0;
		}
        strcpy(ClsPtr->refno,"TBA");
	strcpy(ClsPtr->comment,"");
	SAFECPY(ClsPtr->name,Tokens[1]->text,SHORTNAME);
	ClsPtr->numclasses = 0;
	ClsPtr->attach = 1;
	ClsPtr->numassignments = 0;
        ClsPtr->finalroom = &DefaultRoom;
	ClsPtr->final = &DefaultFinalBlock;
	}


	if (numtokens > 7 || numtokens < 6)return 1;

         ClsPtr->available = 1;
	ClsPtr->enrollment = 0;
	 if(numtokens == 7 )
	  if(strcmp(Tokens[6]->text,"N")==0)
		ClsPtr->available = 0;
	  else { 
		/* enrollment is being entered */
		j = atoi(Tokens[6]->text);
                /* Sanity Check value of j */
                if(j<0) myerror(shell_errorstream,"Bad enrollment value: %d\n",j);
		else
			ClsPtr->enrollment = j;
	  }
		


/* Search for the room */

	if(strcmp(Tokens[2]->text,MissingData)==0)
		ClsPtr->room = &DefaultRoom;
	else {
	SAFECPY(TRoom.name,Tokens[2]->text,SHORTNAME);
	 krqsort((void **)Rooms,0,numrooms-1,(int(*) (void *,void *))
	  CompareRoomsByName);
	ClsPtr->room = binsearch((void *)&TRoom,(void **)Rooms,numrooms,
		(int(*) (void *,void *))CompareRoomsByName);
	if(ClsPtr->room == NULL){myerror(shell_errorstream,
	 "class: no such room %s\n",Tokens[2]->text);
	 ClsPtr->room = &DefaultRoom;
	 }
	}

/* Search for the Job */

	if(strcmp(Tokens[5]->text,MissingData)==0)
		ClsPtr->job = &DefaultJob;
	else {
	SAFECPY(TJob.name,Tokens[5]->text,SHORTNAME);
	 krqsort((void **)Jobs,0,numjobs-1,(int(*) (void *,void *))
	  CompareJobsByName);
	ClsPtr->job = binsearch((void *)&TJob,(void **)Jobs,numjobs,
		(int(*) (void *,void *))CompareJobsByName);
	if(ClsPtr->job == NULL){myerror(shell_errorstream,
	 "class: no such job %s\n",Tokens[5]->text);
		ClsPtr->job = &DefaultJob;
	}
	}



/* Search for the Teacher */

	if(strcmp(Tokens[3]->text,MissingData)==0)
		ClsPtr->teacher = &DefaultPerson;
	else {
	SAFECPY(TPerson.shortname,Tokens[3]->text,SHORTNAME);
	 krqsort((void **)People,0,numpeople-1,(int(*) (void *,void *))
	  ComparePeopleByShortName);
	ClsPtr->teacher = binsearch((void *)&TPerson,(void **)People,numpeople,
		(int(*) (void *,void *))ComparePeopleByShortName);
	if(ClsPtr->teacher == NULL){myerror(shell_errorstream,
	 "class: no such person %s\n",Tokens[3]->text);
	 ClsPtr->teacher = &DefaultPerson;
	 }

	else {  /* update load for this person */
		ClsPtr->teacher->load_now += ClsPtr->job->load;
		if(ClsPtr->teacher->load_now > ClsPtr->teacher->load_sem)
			myerror(shell_errorstream,"Semester load %g for %s exceeded: %g\n",
				ClsPtr->teacher->load_sem,ClsPtr->teacher->shortname,
				ClsPtr->teacher->load_now);
		if(ClsPtr->teacher->load_now > ClsPtr->teacher->load_ay)
			myerror(shell_errorstream,"AY load %g for %s exceeded: %g\n",
				ClsPtr->teacher->load_ay,ClsPtr->teacher->shortname,
				ClsPtr->teacher->load_now);

		/* Check that this person can do the job */

		if(!CanDo(ClsPtr->teacher,ClsPtr->job))
		   if(strcmp(ClsPtr->teacher->shortname,MissingData)!=0)
			myerror(shell_errorstream,"Appt %s of %s can't do job %s\n",
				ClsPtr->teacher->apt_type,
				ClsPtr->teacher->shortname,
				ClsPtr->job->name);
	}
	}

/* Search for the Time Slot */

	if(strcmp(Tokens[4]->text,MissingData)==0)
		ClsPtr->times = &DefaultSlotList;
	else {
	TSlot.name = common_buf;
	SAFECPY(TSlot.name,Tokens[4]->text,SHORTNAME);
	 krqsort((void **)TimeSlotLists,0,NumberOfSlotLists-1,(int(*) (void *,void *))
	  CompareSlotsByName);
	ClsPtr->times = binsearch((void *)&TSlot,
	(void **)TimeSlotLists,NumberOfSlotLists,
		(int(*) (void *,void *))CompareSlotsByName);
	if(ClsPtr->times == NULL){myerror(shell_errorstream,
	 "class: no such time slot %s\n",Tokens[4]->text);
	 ClsPtr->times = &DefaultSlotList;
	}
	}


	if(GetShell("COURSE") != NULL)
	 strcpy(ClsPtr->crse_name,CurrentCourse->name);
	else
	strcpy(ClsPtr->crse_name,"");

/* DATA CONSISTENCY CHECKS */

/*  two classes don't meet in same room at same time */
       
	if(strcmp(ClsPtr->room->name,MissingData)!=0)
	for(j=0;j<ClsPtr->room->numclasses;j++)
		if(SlotsDoOverlap(ClsPtr->times,
		((struct ClassStruct *) (ClsPtr->room->classes[j]))->times,&first,&second)){
			strcpy(common_buf,ctime(&first));
myerror(shell_errorstream,"%s %s meeting at %s overlaps with %s %s meeting at %s\n",CurrentCourse->name, ClsPtr->name,
common_buf,
((struct ClassStruct *)(ClsPtr->room->classes[j]))->crse_name,
((struct ClassStruct *)(ClsPtr->room->classes[j]))->name,ctime(&second));
		}

/* Check enrollment against room capacity */

	if(strcmp(ClsPtr->room->name,MissingData)!=0){
         if(ClsPtr->enrollment > ClsPtr->room->seats)
          myerror(shell_errorstream,"%s %s enrollment %d > seating capacity of %s: %d\n",
           ClsPtr->crse_name,ClsPtr->name,ClsPtr->enrollment,
           ClsPtr->room->name,ClsPtr->room->seats);
         if(ClsPtr->enrollment > ClsPtr->room->exam_seats)
          myerror(shell_errorstream,"%s %s enrollment %d > exam capacity of  %s: %d\n",
           ClsPtr->crse_name,ClsPtr->name,ClsPtr->enrollment,
           ClsPtr->room->name,ClsPtr->room->exam_seats);
	 }

/* Check that a person does not have to be two different places at once */

	if(strcmp(ClsPtr->room->name,MissingData)!=0 &&
		strcmp(ClsPtr->teacher->shortname,MissingData)!=0)
	for(j=0;j<ClsPtr->teacher->numclasses;j++)
		if(SlotsDoOverlap(ClsPtr->times,
		((struct ClassStruct *) (ClsPtr->teacher->classes[j]))->times,&first,&second)&&
			(strcmp(ClsPtr->room->name,
		((struct ClassStruct *)(ClsPtr->teacher->classes[j]))->room->name)!=0)&&
		(strcmp(((struct ClassStruct *)(ClsPtr->teacher->classes[j]))->room->name,
			MissingData)!= 0)){
		myerror(shell_errorstream,"%s has to be two places at once: %s and %s\n",
			ClsPtr->teacher->shortname,ClsPtr->room->name,
		((struct ClassStruct *)(ClsPtr->teacher->classes[j]))->room->name);
		myerror(shell_errorstream,"Class %s %s not added.\n",
			CurrentCourse->name,ClsPtr->name);
		free(ClsPtr);
		return 0;
		}

/* Attach the current class to the current schedule,course or to the current class
   at the right recursive level */

	if(GetShell("COURSE") != NULL){
	if(ClassLevel > 0){
		if(PendingClass[ClassLevel-1]->numclasses >= MAX_SECTIONS -1){
		  myerror(shell_errorstream,"%s has too many sections\n",
			PendingClass[ClassLevel-1]->name);
		  free(ClsPtr);
		  return 0;
		}
		if(newclass)
		PendingClass[ClassLevel-1]->classes[PendingClass[ClassLevel-1]->numclasses++]=ClsPtr;
	}

	else { 
		if(CurrentCourse->numclasses >= MAX_SECTIONS -1){
		CurrentCourse->classes[CurrentCourse->numclasses]=ClsPtr;
		  myerror(shell_errorstream,"%s has too many sections\n",
			CurrentCourse->name);
		  free(ClsPtr);
		  return 0;
		}
		if(newclass)
		CurrentCourse->classes[CurrentCourse->numclasses++]=ClsPtr;
	}
	}
	else 
		if(newclass)
		CurrentSchedule->classes[CurrentSchedule->numclasses++] = ClsPtr;

	set("CLASS",ClsPtr->name,3,shell_ostream);
	CurrentClass = ClsPtr;

/* Make an entry for this class in this teachers's list of classes */

	if(ClsPtr->teacher != NULL && strcmp(ClsPtr->teacher->shortname,
		MissingData)!= 0){
	if(ClsPtr->teacher->numclasses >= MAXCLASSESPERPERSON -1){
		myerror(shell_errorstream,"%s has too many classes\n",
		ClsPtr->teacher->shortname);
		return 0;
	}
        if(strcmp(ClsPtr->teacher->shortname,MissingData)!=0)
	ClsPtr->teacher->classes[ClsPtr->teacher->numclasses++] =
		ClsPtr;
	}

/* Make an entry for this class in the room's list of classes */

	if(ClsPtr->room != NULL && strcmp(ClsPtr->room->name,MissingData)!=0){
	if(ClsPtr->room->numclasses >= MAXCLASSESPERROOM -1){
		myerror(shell_errorstream,"%s has too many classes\n",
		ClsPtr->room->name);
		return 0;
	}
	ClsPtr->room->classes[ClsPtr->room->numclasses++] =
		ClsPtr;
	}

	return 0;
}

int
subclass(SHELLSTREAMS)
{

	if(numtokens == 1){
		if(GetShell("CLASS")==NULL){
			myerror(shell_errorstream,"Cannot subclass: no current class.\n");
			return 0;
		}
		if(ClassLevel + 1 >= MAX_LVL) {
			myerror(shell_errorstream,"Too many subclass levels.\n");
			return 0;
		}
		PendingClass[ClassLevel++] = CurrentClass;
		sprintf(common_buf,"%d",ClassLevel);
		set("CLASS-LEVEL",common_buf,3,shell_ostream);
		return 0;
	}
	if((numtokens == 2) && (strcmp(Tokens[1]->text,"end")==0)){
		if(ClassLevel == 0) {
			myerror(shell_errorstream,"Already at class level 0. Cannot pop level\n");
			return 0;
		}
		ClassLevel--;
		set("CLASS",PendingClass[ClassLevel]->name,3,shell_ostream);
		CurrentClass = PendingClass[ClassLevel];
		sprintf(common_buf,"%d",ClassLevel);
		set("CLASS-LEVEL",common_buf,3,shell_ostream);
		return 0;
	}
	return 1;
}
		

int assign(SHELLSTREAMS)
{
	
	int i;
	struct CourseStruct *CrsPtr;
	struct AssignStruct *AssgnPtr;

	if(numschedules == 0) {
		myerror(shell_errorstream,"No schedule is loaded\n");
		return 0;
	}
	/* check for proper usage */

	if(numtokens == 1){  /* list all departmental assignments */
		fprintf(shell_ostream,"Current Departmental Assignments:\n");
		for(i=0;i<CurrentSchedule->numassignments;i++)
			fprintf(shell_ostream,"%d. %s\t%s\t%s\n",
				i,CurrentSchedule->assignments[i]->name,
				CurrentSchedule->assignments[i]->holder->shortname,
				CurrentSchedule->assignments[i]->job->name);
		return 0;
	}

	if(numtokens == 2){ /* list all assignments of course */


/* see if there is an existing course by this name */

	        krqsort((void **)CurrentSchedule->Courses,0,CurrentSchedule->numcourses-1,(int(*) (void *,void *))
       		   CompareCoursesByName);
        	SAFECPY(TCrs.name,Tokens[1]->text,SHORTNAME);
	        if((CrsPtr = binsearch((void *)&TCrs,(void **)CurrentSchedule->Courses,CurrentSchedule->numcourses,
                (int(*) (void *,void *))CompareCoursesByName))==NULL){
			myerror(shell_errorstream,"No such course: %s\n",
				Tokens[1]->text);
			return 0;
		}
		/* else */
		fprintf(shell_ostream,"%s: current assignments:\n",
			CrsPtr->name);
                for(i=0;i<CrsPtr->numassignments;i++)
                        fprintf(shell_ostream,"%d. %s\t%s\t%s\n",
                                i,CrsPtr->assignments[i]->name,
                                CrsPtr->assignments[i]->holder->shortname,
                                CrsPtr->assignments[i]->job->name);
                return 0;
	}
	if(numtokens !=4)return 1;

	/* Allocate an assignment structure */

	if(GetShell("COURSE") ==NULL && CurrentSchedule->numassignments >= MAX_ASSIGNMENTS -1){
		myerror(shell_errorstream,"Too many departmental assignments\n");
		return 0;
	}
	
	if((AssgnPtr = (struct AssignStruct *)malloc(sizeof(struct AssignStruct))) == NULL)
		fatal(shell_errorstream,"Unable to allocate memory for Assignment.\n");

	/* fill in the name of this assignment */
	SAFECPY(AssgnPtr->name,Tokens[1]->text,LONGNAME);

	/* search for person */

	if(strcmp(Tokens[2]->text,MissingData)==0)
		AssgnPtr->holder = &DefaultPerson;
	else {
	SAFECPY(TPerson.shortname,Tokens[2]->text,SHORTNAME);
         krqsort((void **)People,0,numpeople-1,(int(*) (void *,void *))
          ComparePeopleByShortName);
        AssgnPtr->holder = binsearch((void *)&TPerson,(void **)People,numpeople,
                (int(*) (void *,void *))ComparePeopleByShortName);
        if(AssgnPtr->holder == NULL){myerror(shell_errorstream,
         "class: no such person %s\n",Tokens[2]->text);
         AssgnPtr->holder = &DefaultPerson;
	}
	}

	/* search for job */

	 SAFECPY(TJob.name,Tokens[3]->text,SHORTNAME);
         krqsort((void **)Jobs,0,numjobs-1,(int(*) (void *,void *))
          CompareJobsByName);
        AssgnPtr->job = binsearch((void *)&TJob,(void **)Jobs,numjobs,
                (int(*) (void *,void *))CompareJobsByName);
        if(AssgnPtr->job == NULL){myerror(shell_errorstream,
         "class: no such job %s\n",Tokens[3]->text);
                AssgnPtr->job = &DefaultJob;
	}
	/* Do Sanity Checks */

	/* 1. Does Person's assignment type match the job's ? */

	if(!CanDo(AssgnPtr->holder,AssgnPtr->job))
	  if(strcmp(AssgnPtr->holder->shortname,MissingData)!=0)
                        myerror(shell_errorstream,"Appt %s of %s can't do job %s\n",
                                AssgnPtr->holder->apt_type,
                                AssgnPtr->holder->shortname,
                                AssgnPtr->job->name);

	/* 2. Check the Person's load */

	if(AssgnPtr->holder->numassignments >= MAX_ASSIGNMENTSPERCLASS){
		myerror(shell_errorstream,"%s has too many assignments\n",
			AssgnPtr->holder->shortname);
		return 0;
	}

	AssgnPtr->holder->load_now += AssgnPtr->job->load;
                if(AssgnPtr->holder->load_now > AssgnPtr->holder->load_sem)
                        myerror(shell_errorstream,"Semester load %g for %s exceeded: %g\n",
				AssgnPtr->holder->load_sem,
				AssgnPtr->holder->shortname,
	                                AssgnPtr->holder->load_now);
                if(AssgnPtr->holder->load_now > AssgnPtr->holder->load_ay)
                        myerror(shell_errorstream,"AY load %g for %s exceeded: %g\n",
				AssgnPtr->holder->load_ay,
				AssgnPtr->holder->shortname,
				AssgnPtr->holder->load_now);

	/* 3. Job's effective dates contained between person's ? */
		if(strcmp(AssgnPtr->holder->shortname,MissingData)!=0)
		if((AssgnPtr->job->start < AssgnPtr->holder->start)||
			(AssgnPtr->job->end > AssgnPtr->holder->end))
			myerror(shell_errorstream,"%s 's effective dates don't match job's: %s\n",
				AssgnPtr->holder->shortname,
				AssgnPtr->job->name);

	/* Set back pointer in Person struct */

		AssgnPtr->holder->assignments[AssgnPtr->holder->numassignments++]
			= AssgnPtr;

	/* If there is a current class, attach this assignment to it */
	
	if(GetShell("COURSE") != NULL)
	 if(ClassLevel > 0){
		if(PendingClass[ClassLevel-1]->numassignments >= 
			MAX_ASSIGNMENTSPERCLASS-1){
		 myerror(shell_errorstream,"Class %s has too many assignments\n",
			PendingClass[ClassLevel-1]->name);
		 return 0;
	}
PendingClass[ClassLevel-1]->assignments[PendingClass[ClassLevel-1]->numassignments++]
	=AssgnPtr;
	}
        else {
		if(CurrentCourse->numassignments >= MAX_ASSIGNMENTSPERCLASS-1){
		  myerror(shell_errorstream,"Course %s has too many assignments\n",
			CurrentCourse->name);
		  return 0;
		} 
		CurrentCourse->assignments[CurrentCourse->numassignments++]
			=AssgnPtr;
	}

/* else attach this assignment to department */

	else{ 
		 CurrentSchedule->assignments[CurrentSchedule->numassignments++]
			=AssgnPtr;
	}
	return 0;

}


int refno(SHELLSTREAMS)
{
	if(GetShell("CLASS")==NULL || CurrentClass == NULL){
		myerror(shell_errorstream,"refno: No current class\n");
		return 0;
	}
	if(numtokens != 2) return 1;

	SAFECPY(CurrentClass->refno,Tokens[1]->text,NUMBERLEN);
	return 0;
}

int comment(SHELLSTREAMS)
{
	if(numtokens != 2) return 1;
	if(GetShell("CLASS")==NULL || CurrentClass == NULL){
           if(GetShell("COURSE")==NULL || CurrentCourse == NULL){ 
		myerror(shell_errorstream,"comment: No current class\n");
		return 0;
	    }
	    else { 
		SAFECPY(CurrentCourse->comment,Tokens[1]->text,LONGNAME);   
	    	return 0;
	    }
	}
	SAFECPY(CurrentClass->comment,Tokens[1]->text,LONGNAME);
	return 0;
}


int
PrintCourse(struct CourseStruct *CrsPtr,FILE *shell_ostream)
{
		int i;
	
		fprintf(shell_ostream,"\n%s",CrsPtr->name);
                        if(CrsPtr->supervisor != NULL)
                                fprintf(shell_ostream," Supervisor: %s ",
                                        CrsPtr->supervisor->longname);
                        fprintf(shell_ostream,"( %d classes, %d assignments, %d enrollment )",
                                CrsPtr->numclasses,
				CrsPtr->numassignments,
				CrsPtr->enrollment);
			fprintf(shell_ostream,"\n");

	if(strlen(CrsPtr->comment)>0)
		fprintf(shell_ostream,"\n%s\n\n",CrsPtr->comment);

	/* Print all classes of this course */
	
                fprintf(shell_ostream,"%s: current classes:\n",
                        CrsPtr->name);
		for(i=0;i<CrsPtr->numclasses;i++)
			PrintClass(CrsPtr->classes[i],1,shell_ostream);

         /* Print all assignments attached to this course */

		if(CrsPtr->numassignments > 0){
                	fprintf(shell_ostream,"%s: Course assignments:\n",
                        CrsPtr->name);
                	for(i=0;i<CrsPtr->numassignments;i++)
                       		 fprintf(shell_ostream,"%d. %s\t%s\t%s\n",
                               		 i,CrsPtr->assignments[i]->name,
                                	CrsPtr->assignments[i]->holder->shortname,
                                	CrsPtr->assignments[i]->job->name);
		}

		fprintf(shell_ostream,"\n");
		return 0;
}


int PrintClass(struct ClassStruct *aclass, int level,FILE *shell_ostream)
{
	char buffer[LONGNAME];
	int i,j;
	char indent[] = "  ";


	if(level==0){ /* display classes of current course */
		if(GetShell("COURSE")!=NULL){
		for(i=0;i<CurrentCourse->numclasses;i++)
			PrintClass(CurrentCourse->classes[i],1,shell_ostream);
		return 0;
		}
		else {
			for(i=0;i<CurrentSchedule->numclasses;i++)
				PrintClass(CurrentSchedule->classes[i],1,shell_ostream);
			return 0;
		}
	}
/* Print the reference number */
        if(strcmp(aclass->refno,"TBA")==0)
        fprintf(shell_ostream,"%s\t",MissingData);
        else
	fprintf(shell_ostream,"%s\t",aclass->refno);
	if(aclass->available == 0)fprintf(shell_ostream," N   ");
	else if(aclass->enrollment !=0)fprintf(shell_ostream," %-4d",aclass->enrollment);
	else fprintf(shell_ostream,"     ");

/* Print level number of indents */
	for(i=0;i<level;i++)fprintf(shell_ostream,"%s",indent);

/* Print time, if known */
	fprintf(shell_ostream,"%s\t",aclass->name);
	if(aclass->times == NULL)
		sprintf(buffer, "%s ",MissingData);
	else sprintf(buffer,"%s ",aclass->times->description);
	fprintf(shell_ostream,"%-20s",buffer);

/* Print bldg and room if known */
	if(aclass->room == NULL)
		sprintf(buffer, "%s",MissingData);
	else  sprintf(buffer,"%s %s",aclass->room->bldg,
			aclass->room->number);
	fprintf(shell_ostream,"%s\t",buffer);

/* Print teacher, if known */

	if(aclass->teacher == NULL)
		sprintf(buffer, "%s\n",MissingData);
	else  sprintf(buffer,"%s\n",aclass->teacher->shortname);
	fprintf(shell_ostream,"%s",buffer);

/* Print assignments attached to this class, if any */

	for(i=0;i<aclass->numassignments;i++){
		for(j=0;j<level;j++)fprintf(shell_ostream,"%s",indent);
                fprintf(shell_ostream,"assignment: %d. %s\t%s\t%s\n",
                                i,aclass->assignments[i]->name,
                               aclass->assignments[i]->holder->shortname,
			aclass->assignments[i]->job->name);
	}
	if(strlen(aclass->comment)>0)
		fprintf(shell_ostream,"\n%s\n\n",aclass->comment);

/* Call recursively on subclasses */

	for(i=0;i<aclass->numclasses;i++)
		PrintClass(aclass->classes[i],level+1,shell_ostream);

	return 0;
}
	
/* CanDo: returns 1 if person can do a job, 0 otherwise */

int CanDo(struct PersonStruct *personptr, struct JobStruct *jobptr)
{
	int i;
	
	for(i=0;i<jobptr->num_types;i++)
	 if(strcmp(personptr->apt_type,*(jobptr->apt_type+i)) == 0)
		return 1;
	return 0; 
}

extern int CompareBlocksByName(struct FinalBlockStruct *,
                                struct FinalBlockStruct *);
int
CompareBlocksByClassTime(struct FinalBlockStruct *, struct FinalBlockStruct *);

extern int cmdline;

/* When called, read and parse lines until "finalblock end" is encountered */
/* For each line, assign the class to the appropriate final exam block */

int finalblock(SHELLSTREAMS)
{
	int automatic = 0;
	int loopflag = 0;
	int i,j,k;
	struct FinalBlockStruct *ABlock;
	struct CourseStruct *CrsPtr;
	struct ClassStruct *ClassPtr;
	struct RoomStruct *RoomPtr;

	TBlock.crse_time = &TSlot;

	if(numschedules == 0 || CurrentSchedule == NULL){
		myerror(shell_errorstream,"No current schedule.\n");
		return 0;
	}
	if(numtokens != 2) return 1;

	if(strcmp(Tokens[1]->text,"end")==0) return 0; /* probably previous
                                                         syntax error */

/* if auto keyword is given, we will search for exam block based on class time */

	if(strcmp(Tokens[1]->text,"auto")== 0)automatic = 1;

/* otherwise, name of block to assign to is given on command line */
	else {
		strcpy(TBlock.name,Tokens[1]->text);
		krqsort((void **)FinalBlocks,0,numblocks-1,(int(*) (void *,void *))
                  CompareBlocksByName);
        	if( (ABlock = (struct FinalBlockStruct *) binsearch((void *)&TBlock,
                  (void **)FinalBlocks,numblocks,
                  (int(*) (void *,void *))CompareBlocksByName)) == NULL){
                  myerror(shell_errorstream,"No such exam block %s\n",
                        Tokens[1]->text);
                  return 0;
		}
        }

/* read and parse input until finalblock end is encountered */

	while(1){
		loopflag = 0; /* tells whether we loop over classes of course*/
		fgets(buffer,MAXLINE,shell_istream);
		++cmdline;
		numtokens = tokenize(buffer,Tokens);
		if(strcmp(Tokens[0]->text,"finalblock") == 0 &&
                    strcmp(Tokens[1]->text,"end")== 0) return 0;

/* See whether the first token is the name of a course */
		strcpy(TCrs.name,Tokens[0]->text);
		krqsort((void **)CurrentSchedule->Courses,0,CurrentSchedule->numcourses-1,
		(int(*) (void *,void *)) CompareCoursesByName);

		if((CrsPtr = binsearch((void *)&TCrs,(void **)CurrentSchedule->Courses,
		CurrentSchedule->numcourses,
		(int(*) (void *,void *))CompareCoursesByName))!=NULL){

/* If so, and no class arg is given, then we will loop over classes of this course */
			if(numtokens == 2) loopflag = 1;
                        else {

/* else, search to see whether the 2nd arg is a class */

			  strcpy(TClass.name,Tokens[1]->text);
                          krqsort((void **)CrsPtr->classes,0,
                           CrsPtr->numclasses-1,(int(*) (void *,void *))
                           CompareClassesByName);
                           if((ClassPtr = binsearch((void *)&TClass,
                           (void **)CrsPtr->classes,CrsPtr->numclasses,
                           (int(*) (void *,void *))CompareClassesByName))==NULL){
                             myerror(shell_errorstream,"No such class: %s\n",
                                  Tokens[1]->text);
                             continue;
                           }
                        }
		}  /* End of case first arg is a course */

		else {

/* If first arg is not a course, see whether it is a class */

		strcpy(TClass.name,Tokens[0]->text);
                krqsort((void **)CurrentSchedule->classes,0,
                 CurrentSchedule->numclasses-1,(int(*) (void *,void *))
                 CompareClassesByName);
                 if((ClassPtr = binsearch((void *)&TClass,
                 (void **)CurrentSchedule->classes,CurrentSchedule->numclasses,
                 (int(*) (void *,void *))CompareClassesByName))==NULL){
		   myerror(shell_errorstream,"No such course or class: %s\n",
                     Tokens[0]->text);
                   continue;
		  } 
                }
 

		if(loopflag)j=CrsPtr->numclasses;
                else j = 1;

		for(i=0;i<j;i++){
                  if(loopflag)ClassPtr = CrsPtr->classes[i];

        	/* See whether the room exists */
		  strcpy(TRoom.name,Tokens[numtokens-1]->text);
                  krqsort((void **)Rooms,0,numrooms-1,(int(*) (void *,void *))
                  CompareRoomsByName);
                  RoomPtr = binsearch((void *)&TRoom,(void **)Rooms,numrooms,
                  (int(*) (void *,void *))CompareRoomsByName);
                  if(RoomPtr == NULL){
		   	myerror(shell_errorstream,
                        "no such room %s\n",Tokens[1]->text);
                        RoomPtr = &DefaultRoom;
		  }

                  /* Assign class to block and set final exam room */

                  ClassPtr->finalroom = RoomPtr;

	          if(automatic) {

	/* Need to search for exam block based on class time */

	            strcpy(TSlot.name,ClassPtr->times->name);
                    krqsort((void **)FinalBlocks,0,numblocks-1,(int(*) (void *,void *))
                    CompareBlocksByClassTime);
                    ABlock = binsearch((void *)&TBlock,(void **)FinalBlocks,
                    numblocks,
                    (int(*) (void *,void *))CompareBlocksByClassTime);
	            if(ABlock == NULL){
                      myerror(shell_errorstream,"No exam block for meeting time %s\n",
                      TSlot.name);
		      continue;
                    }
		   }
                   ABlock->classes[ABlock->numclasses++]=ClassPtr;
		   ClassPtr->final = ABlock;

/* Do any consistency checks here */
/* Does a person have two exams at the same time in two different places ? */
                   if(strcmp(RoomPtr->name,MissingData)!= 0)
		     for(k=0;k<ClassPtr->teacher->numclasses;k++)
                      if(strcmp(ClassPtr->teacher->classes[k]->crse_name,
                            ClassPtr->crse_name) != 0 ||
                         strcmp(ClassPtr->teacher->classes[k]->name,
                            ClassPtr->name) != 0)
                      if(SlotsDoOverlap(ClassPtr->teacher->classes[k]->final->exam_time,
                             ABlock->exam_time,NULL,NULL))
                       if(strcmp(ClassPtr->teacher->classes[k]->finalroom->bldg,
                              RoomPtr->bldg) != 0 ||
                          strcmp(ClassPtr->teacher->classes[k]->finalroom->name,
                                  RoomPtr->name)!=0)
                          myerror(shell_errorstream,"%s : possible final time conflict. blocks %s and %s\n",
                                  ClassPtr->teacher->shortname,
                                  ClassPtr->teacher->classes[k]->final->name,
                                  ABlock->name);

/* This one is a joke :-) */
                   if(strcmp(ClassPtr->teacher->shortname,"Troutman")==0)
                     myerror(shell_errorstream,"Troutman probably won't like this! :-)\n");
                    
               }    /* end loop over classes of course */
	}  /* end while */
}
