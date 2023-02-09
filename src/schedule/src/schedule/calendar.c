/* calendar.c: implementation of functions interfaced in calendar.h for
     creating, managing, and using the academic calendar */


#include "calendar.h"

#include<string.h>
#include<math.h>



int atosecs(char *);
time_t ParseDate( char *, char *);
time_t TimeChange(time_t,time_t);


/* Parse a date and time string, eg 4/30/54 8:35   */
/* 4-30-54, 4/30/1954, and "4 30 54" are all ok */
/* Returns a calendar time or 0 if there is an error */
/* Time string should be given as a:bb[am,pm]. Military is ok */

/* N.B. After the year 1999 the full year must be given: 2001, not 01 */


static char buffer[LONGNAME];

time_t ParseDate( char *DateString , char *TimeString)
{
	char buf[SHORTNAME];
	char numbuf[NUMBERLEN];
	char *ptr;
	time_t temp;
	time_t hours_adj;  /* Needed to adjust when TimeString has pm */
	struct tm *Date;
	time_t calendartime;


/* Obtain a pointer to static tm structure maintained by the C-Library */
/* It is important to allow the library to initialize as many components
   as possible. */
	calendartime = 0;
	Date = localtime(&calendartime);
	Date->tm_hour = 0;
	Date->tm_min = 0;
	Date->tm_sec = 0;

/* Date should now represent midnight local time on January 1 1970.  */
	

/* Next adjust these components of Date : tm_mday, tm_mon, tm_year
   according to the contents of DateString */

/* First, parse the DateString */

	SAFECPY(buf,DateString,SHORTNAME);

	/* Get Month */
	if((ptr = strtok(buf,FIELDSEPS))==NULL){
		myerror(errorstream,"Syntax error in %s\n",DateString);
		return (time_t) 0;
	} 
	SAFECPY(numbuf,ptr,NUMBERLEN);
	if((temp = atoi(numbuf)) > 12 || temp < 0 ){
		myerror(errorstream,"Month out of range in %s\n",DateString);
		return (time_t) 0;
	}

/* and set it */

	Date->tm_mon = temp-1;

	/* Get Day */   

	if((ptr=strtok(NULL,FIELDSEPS))==NULL){
		myerror(errorstream,"Syntax error in %s\n",DateString);
		return (time_t) 0;
	} 
	SAFECPY(numbuf,ptr,NUMBERLEN);
	if((temp = atoi(numbuf)) > 31 || temp < 1 ){
		myerror(errorstream,"Day out of range in %s\n",DateString);
		return (time_t) 0;
	}

/* and set it */

	Date->tm_mday = temp;

	/* Get Year */
	if((ptr=strtok(NULL,FIELDSEPS))==NULL){
		myerror(errorstream,"Syntax error %s\n",DateString);
		return (time_t) 0;
	} 
	SAFECPY(numbuf,ptr,NUMBERLEN);
	temp = atoi(numbuf);
	if((temp >= 100 && temp < 1970)||(temp < 70)  ){
		myerror(errorstream,"Year out of range in %s\n. Must be after 1969.",DateString);
		return (time_t) 0;
	}

/* and set it */

	Date->tm_year = (temp > 99 ? temp - 1900 : temp);

/* Make a call to mktime to get a calendar time. */
/* Then call localtime to make sure that dst flag gets set */
/* Reset to midnight just in case */
/* We should end up with  midnight local time on this date  */

	calendartime = mktime(Date);
	localtime(&calendartime);
	Date->tm_hour = 0;
	calendartime = mktime(Date);


 
/* Now we go on to add the appropriate offset to calendar time
depending on the hours, minutes and seconds. */

/* In order to use the atosecs routine
we check the time string to see if it has more than one colon. If not,
   we append :00 to the end. If it has no colons we put :00:00 at end.*/

/* Check for p.m. PM etc */
/* This is not foolproof. If the timestring contains crazy stuff it 
    will barf */

	SAFECPY(buf,TimeString,SHORTNAME); 
	if((ptr = strpbrk(buf,"aApP ")) != NULL){
		if((strpbrk(ptr,"aApP") != NULL) && (
			(*strpbrk(ptr,"aApP") == 'p')||(*strpbrk(ptr,"aApP")
			== 'P'))) 
			hours_adj = 12;
		*ptr  = '\0'; /* cut off a.m., etc */
	}
	else 
		hours_adj = 0;

/* The routine atosecs(), which converts a time string to seconds expects
   a string of the form hh:mm:ss. Make sure it is so */

	if(strchr(buf,':')==strrchr(buf,':')){ /* One or none */
		if(strchr(buf,':')==NULL)
			strcat(buf,":00:00");
		else strcat(buf,":00");
	}

 

	/* put seconds since midnight in temp */
	 temp = (time_t) atosecs(buf);

	if( temp / 3600 + hours_adj >= 24 ) hours_adj = 0;
/* must have been 12:xx pm -- in this case we added too much */

	temp += hours_adj*3600;   /* correct for p.m. if any */


	calendartime += (time_t) temp;
	localtime(&calendartime);

	return calendartime;
}

/* DumpSlotList: Print a slot list on stream in a readable form */

void DumpSlotList(FILE *ostream, struct SlotList *AList)
{
	int i;
	int cols;
	struct TimeSlot *tptr[MAX_LIST_SIZE];
	
	if(AList == NULL){
		myerror(errorstream,"Could not dump Slot list\n");
		return;
	}
	cols = atoi(COLS);

/* Sort the time slots in order of their starting times */
	for(i=0;i<AList->number;i++)
		tptr[i] = AList->timeslots[i];

	krqsort((void **)tptr,0,AList->number-1,
		(int(*)( void *, void *))CompareSlotsByTime);

	fprintf(ostream,"                  %s (%s)\n",AList->description,AList->name);
	for(i=0;i<AList->number;i++){
		strftime(buffer,LONGNAME,"%m/%d %H:%M->", 
			localtime(&(tptr[i]->start)));
		fprintf(ostream,"%s",buffer);
		strftime(buffer,LONGNAME,"%m/%d %H:%M", 
			localtime(&(tptr[i]->end)));
		fprintf(ostream,"%s",buffer);
                if(strcmp(AList->name,"Holidays")==0 && tptr[i]->name != NULL)
			fprintf(ostream," ( %s )",tptr[i]->name);
/* Print every other one on a new line */
	if( (i % cols) != cols-1) fprintf(ostream,"  ");
	else fprintf(ostream,"\n");
	}
	if((i%cols)!=0)fprintf(ostream,"\n");
	return;
}

/* SlotsDoOverlap: see whether two time slot lists have any overlapping
	components. Return 1 if so, 0 else. If so, put pointers to the 
	beginning (time_t) of the
	first pair of overlapping time slots in the 3rd and 4th params. */

int SlotsDoOverlap(struct SlotList *FirstList, struct SlotList *SecondList,
			time_t *BadFromFirst,
			time_t *BadFromSecond)
{
	int i,j; /* counters */

/* Handle trivial cases */

	if((FirstList==NULL)||(SecondList==NULL)||(FirstList->number==0)||
		(SecondList->number==0)) return 0;

	for(i=0;i<FirstList->number;i++)
		for(j=0;j<SecondList->number;j++)
			if((FirstList->timeslots[i]->end < 
				SecondList->timeslots[j]->start)||
				(SecondList->timeslots[j]->end <
				 FirstList->timeslots[i]->start))
				continue;
			else {
				if((BadFromFirst != NULL) &&
					(BadFromSecond != NULL)){
				*BadFromFirst = FirstList->timeslots[i]->start;
				*BadFromSecond =SecondList->timeslots[j]->start;
				}
				return 1;
			}
	return 0;
}

/* CreatSlotList: create a slotlist according to a specification or dialogue,
	allocate memory, and return a pointer to it, NULL if creation failed */

struct SlotList *CreatSlotList( struct SlotListSpec *ASpec)
{       
	struct SlotList *AList;
	int i,j,k;
	struct TimeSlot ASlot,TSlot;
	struct TimeSlot *TSlotPtr;
	struct SlotList TList;  /* Template of list used locally */
	struct SlotList *Holidays;

	if((AList = (struct SlotList *)malloc(sizeof(struct SlotList)))==NULL)
		fatal(errorstream,"Failed to allocate memory for slot list.");

	TList.number = 1;  
	AList->number = 0;

	TList.name = buffer;
        TList.timeslots = &TSlotPtr;
        TSlotPtr = &TSlot;

	if((AList->name = (char *)malloc(SHORTNAME))==NULL)
		fatal(errorstream,"Failed to allocate memory for slot list name.");
	if((AList->description = (char *)malloc(LONGNAME))==NULL)
		fatal(errorstream,"Failed to allocate memory for slot list description.");

	strcpy(TList.name ,"Holidays");

	/* Sort the list of Slot lists by name */
	krqsort((void **)TimeSlotLists,0,NumberOfSlotLists-1,
		(int(*)( void *, void *))CompareSlotListsByName);

	/* Find the one called Holidays */
	Holidays = (struct SlotList *)binsearch((void *)&TList,
			(void **)TimeSlotLists,NumberOfSlotLists,
			(int (*)( void *, void *))CompareSlotListsByName);
		
	if(Holidays == NULL)
		myerror(errorstream,"Holidays not found when creating Slot List\n");

		strcpy(AList->name,ASpec->name);
		strcpy(AList->description,ASpec->description);

/* For each of the start-end pairs in the specification, create a list of
	time slots one week apart until end of classes */
		
	if( ASpec->numberofslots > MAXSLOTS){
		myerror(errorstream,"%s has more than %d slot series. Only %d are supported\n",
			ASpec->name,MAXSLOTS,MAXSLOTS);
		return AList;
	}

	/* Count how many timeslots will be required */
	k=0;
	for(i=0;i<ASpec->numberofslots;i++){
		j=0;
                while( ( SemesterStart + ASpec->end[i] + WEEK*j)
                         < EndOfClasses){
				k++;
				j++;
		}
	}

	/* Allocate memory for array of pointers to timeslots */
	if((AList->timeslots = (struct TimeSlot **)malloc(k*sizeof(struct TimeSlot *)))
		== NULL)
        	fatal(stderr,"Unable to malloc space for slotlist *array\n");

	/* Allocate memory for the timeslots themselves */
	for(i=0;i<k;i++)
		if((AList->timeslots[i] = (struct TimeSlot *)malloc(sizeof(struct TimeSlot)))
                        == NULL)
        	fatal(stderr,"Unable to malloc space for timeslot\n");

	for(i=0;i<ASpec->numberofslots;i++){
		j = 0;
		while( (ASlot.end = SemesterStart + ASpec->end[i] + WEEK*j)
			 < EndOfClasses){

/* Adjust for a switch from DST to EST ( or vice-versa ). This would produce
   a "long" (resp. short ) week so we must add ( resp subtract ) an hour. */

				ASlot.end += 3600*TimeChange(SemesterStart, 
				    SemesterStart + ASpec->end[i] + 
					(time_t)WEEK*j); 

			ASlot.start = SemesterStart + ASpec->start[i] + WEEK*j;
				ASlot.start += 3600*TimeChange(SemesterStart ,
				    SemesterStart + ASpec->start[i] + 
					(time_t) WEEK*j); 

			TList.timeslots[0]->start = ASlot.start;
			TList.timeslots[0]->end = ASlot.end;
			if(!SlotsDoOverlap(&TList, Holidays,NULL,NULL)){
				AList->timeslots[AList->number]->start =
					ASlot.start;
				AList->timeslots[AList->number++]->end =
					ASlot.end;
			}
		j++;
		}
	}

	/* free unused timeslots: k count didn't check for Holidays */

	for(i=AList->number;i<k;i++)
		free(AList->timeslots[i]);


	return AList;
}

/* Given a string in the form hours:min:secs, convert to number of
seconds */

int atosecs(char *string)
{
	int hh,mm,ss;

	hh = atoi(strtok(string,":"));
	mm = atoi(strtok(NULL,":"));
	ss = atoi(strtok(NULL,":"));

	return 3600*hh + 60*mm + ss;
}

/* TimeChange: returns -1 if the first argument is EST and 2nd is EDT,
	0 if both are either EST or EDT, and 1 if the first is EDT and
	the second is EST.
*/

time_t TimeChange(time_t A, time_t B)
{
	int i,j;

	i = localtime(&A)->tm_isdst > 0 ? 1 : 0;
	j = localtime(&B)->tm_isdst > 0 ? 1 : 0;
	return  (time_t) (i - j);
}
