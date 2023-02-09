#define __CPLUSPLUS__

#include "global.h"
#include "runner.h"
#include "team.h"

#ifdef __MSDOS__
#include<constrea.h>
#endif

#include<iostream.h>
#include<fstream.h>
#include<string.h>

int EventNo( char *);

#define TAB "     "    /* 5 spaces */

#define NORMALPRINTER "\x1B&k2G"



// Overloaded << to perform some character translations.

ostream& operator <<= (ostream &os, char *InputString) {
	char buffer[BUFFERSIZ];
	char curr;
	int count =0;
	char *ptr;
	ptr = InputString;

	buffer[0]='\0';
	while((curr = *ptr++)!= '\0'){
		if(curr == '\t')
			strcat(buffer,TAB);
		else {
			count = strlen(buffer);
			*(buffer + count) = curr;
			*(buffer + count + 1) ='\0';
		}
	}
	 os << buffer;
	 return os;
}

extern FILE *errstream;
extern int CompetitionYear;
extern char TodaysDate[];
extern int SortFlag;
extern Runner *AllRunners[];
extern int RunnerCount;
extern int number_teams;
extern int line;
extern char *Tokens[];
extern Team *teams[];
extern int load();
extern "C" void krqsort( void *v[],int , int ,
 int (*)(void *,void *));    // Generic sort routine
extern "C" void *binsearch(void *,void *v[],
int, int (*) (void *, void *));  // Generic binary search
extern int CompareNames(Runner *, Runner *);    // orderings used with
extern int ComparePoints(Team *, Team *);   // sort and search routines
extern int CompareTimes(Runner *First, Runner *Second); 
extern int CompareTotalPoints(Runner *First, Runner *Second);
extern int CompareIndPoints(Runner *First, Runner *Second); 
extern int MySort(void **,int,int,int);
extern char PrinterInitString[];
extern char PrinterResetString[];
extern char PrinterNormalString[];
extern int direction;
extern int depth;

struct RUNINFO {
	struct tm Date;           /* Of the run */
	char ShortName[20];       /* Of the runner */
	char TimeString[15];      /* Time run in the event */
	int EventNumber;         /* index into EventNames */
	int RelayLegEventNumber;    
	int type;   /* index into TypeNames */
	char Leg1[20];
	char Leg2[20];
	char Leg3[20];     /* Short names of Relay Leg Runners */
	char Leg4[20];
	};
extern struct RUNINFO rinfo; 

extern struct StatusInfo {
char OpenFile[80];
char SortType[20];
char SortDirection[9];
char Message[80];
}mystatus;

  /* Write scorall on stream */

int scorall(fstream *out){
	int i;
  // Print Header
	*out << "Noontime Running League Results" << endl;
	*out << "Competition Year: " << CompetitionYear << endl;
	*out << TodaysDate << endl;
		  *out << endl;
   // Output headings
	 out->setf(ios::left,ios::adjustfield);
	 out->width(NameFieldWidth);
	 *out << "Name";
	 out->width(7);
	 *out << "Team";
	 out->width(5);
	 *out << "Pts";
	 out->setf(ios::right,ios::adjustfield);
	 for( i = 0;i<NOEVENTS-NORELAYS;i++){
	    if(ExcludedEventsHasMember(i) == 0){
	       out->width(EventFieldWidths[i]+5);
	       *out << EventNames[i];
	     }
	 }
	 for(i = 0;i<NORELAYS;i++){
	     if(ExcludedRelaysHasMember(i+1) == 0){
		out->width(RelayFieldWidths[i]+5);
		*out << EventNames[NOEVENTS-NORELAYS+i];
	      }
	 }
	   *out << endl;
	 switch(SortFlag){   // resort for output
	      case 1:
		  krqsort((void **)AllRunners,0,RunnerCount-1,
		    (int (*)(void *,void *))CompareTotalPoints);
		    break;
	       default: ; // nothing: already sorted by name
	 }

	 // Individual Results section

	 for(i=0;i<RunnerCount;i++)
	    *out << *(AllRunners[i]);
	 

	// restore default sort order
	  switch(SortFlag){   // resort for output
	   case 1:
	     krqsort((void **)AllRunners,0,RunnerCount-1,
		    (int (*)(void *,void *))CompareNames);
	     break;
	   default: ; // nothing: already sorted by name
	 }

	 // Team Results Section

	 *out << endl << endl;
	 *out << "Current Team Standings (Last to first.)" << endl;
	 out->setf(ios::left,ios::adjustfield);
	 out->width(TeamNameFieldWidth);
	 *out << "Team Name";
	 out->setf(ios::right,ios::adjustfield);
	 out->width(10);
	 *out << "Size" << setw(10) <<   "Reserve"
	      << setw(10) << "Pts(Rsv)" << setw(10) <<
	      "Pts(Tot)" << setw(10) << "Pts(Max)" << endl;

	 for( i=0;i<number_teams;i++)
		*out << *(teams[i]);
	return 0;
}
   /* display report on an individual runner */
   /* Return 1 if runner not found */

int RunnerReport(char * name)
{
      Runner *LookFor;
      Runner *ARunner;

      LookFor = new Runner("noname","noteam","m",0,0,0," ");
      LookFor->SetName(name);
      krqsort((void **)AllRunners,0,RunnerCount-1,
		    (int (*)(void *,void *))CompareNames);

      ARunner = (Runner *)binsearch((void *)LookFor,
		(void **)AllRunners,RunnerCount,
		(int (*)(void *,void *))CompareNames);
      delete LookFor;
			  if (ARunner != NULL){
				ARunner->PrintAll((fstream *)0);
				return 0;
			  }
			  else return 1;
 }

 /* Print the given file. Changes position in file, so save
    before invoking and restore afterwards */

#ifdef __MSDOS__
int PrintOut(FILE *tempfile, int setup)
 {
	char buffer[BUFFERSIZ];

       if(setup == 1) // landscape
    /* Initialize Printer for landscape and CR translation*/
	fprintf(stdprn,PrinterInitString);
       else fprintf(stdprn,PrinterNormalString);
	   // Send tempfile to the printer
	rewind(tempfile);
	while(fgets(buffer,BUFFERSIZ,tempfile)!=NULL)
		fprintf(stdprn,buffer);

       // restore default printer settings
       if(setup == 1)
	fprintf(stdprn,PrinterResetString);
	return 0;
}

#else 
int PrintOut(FILE *tempfile, int setup)
{
	FILE *temporary;
	char tempname[BUFFERSIZ];
	char buffer[BUFFERSIZ];
	
	tmpnam(tempname);   /* create name of temporary file */
	temporary = fopen(tempname,"w");
	if(setup == 1) // landscape
		fprintf(temporary,PrinterInitString);
	rewind(tempfile);
	while(fgets(buffer,BUFFERSIZ,tempfile)!=NULL)
		fprintf(temporary,buffer);
	if(setup == 1)
		fprintf(temporary,PrinterResetString);
	fclose(temporary);
	strcpy(buffer,"lpr  ");
	strcat(buffer,tempname);
	system(buffer);
	remove(tempname);
	return 0;
}
	
#endif


/*  Superlatives: best and worst performances */                         
/* Returns -1 in the event of an error */

#if 0
int Superlatives(fstream *out, char *event)
{                         
	int i,j,k;

	if((i = EventNo(event)) == -1) return -1; 
	if( i >= NOEVENTS - NORELAYS) return -1;
	rinfo.EventNumber = i;
	
	k = depth;
	MySort((void **)AllRunners,0,RunnerCount-1,SortFlag);
	if(direction == 1){       /* slowest */
	
	     *out << EventNames[rinfo.EventNumber] << ": Last Performers By " <<
	     mystatus.SortType << endl;
	     j = i=0;
	     if(SortFlag != BYLASTNAME){
		// skip runners who have not run this event
	     while ((i<RunnerCount)&&(AllRunners[i]->GetTime(rinfo.EventNumber)
		    == 0.0))i++;
	     j = i;
	     *out << "< " << i << " runners have not run the event >" << endl;
	     k +=i;
	     }
	     for(;(i<k)&&(i<RunnerCount);i++){
	       *out << i + 1-j << ". "; 
	       AllRunners[i]->BestRunReport(rinfo.EventNumber,out);
	     }
       }
       else {
	  *out << EventNames[rinfo.EventNumber] << ": First Performers By " <<
	  mystatus.SortType << endl;
	  for(i=0;(i<k)&&(i<RunnerCount);i++){
	      *out << i + 1 << ". "; 
	      AllRunners[i]->BestRunReport(rinfo.EventNumber,out);
	  }
	}
	krqsort((void **)AllRunners,0,RunnerCount-1,
	(int (*)(void *,void *))CompareNames);

	return 0;
}

#endif

int reload()
{
	int i;
       /* free up Token memory */
       for(i=0;i<5;i++)free(Tokens[i]);
       for(i=0;i<RunnerCount;i++)
	 delete AllRunners[i];
       for(i=0;i<number_teams;i++)
	 delete teams[i];
       number_teams = 0;
       line = 1;                // reset counters
       RunnerCount = 0;
       errstream = fopen("errlog","w");
       return load();
}

/* Returns the index of EventName in EventNames[], -1 if no such event */

int EventNo(char *EventName)
{
	int i;

	for(i=0;(i<NOEVENTS)&&(strcmp(EventNames[i],EventName)!=0)
            ;i++);
                  if(i>=NOEVENTS)
			return -1;
	return i;
}

