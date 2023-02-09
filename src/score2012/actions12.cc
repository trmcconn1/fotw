#define __CPLUSPLUS__

#ifdef UNDER_CE
#include "global.h"
#else
#include "global12.h"
#endif
#include "runner12.h"
#include "team12.h"
#include <math.h>


#ifdef __MSDOS__
// #include<constrea.h>
#endif

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
			count = (int)strlen(buffer);
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

/* Write Results Spreadsheet on File Stream */

int spreadsheet(FILE *outfile){

	int i,j,n,m,k;
	Runner *rr;
	struct RunNode **rs;
	struct RelayNode **rls;
	struct RelayNode *rl;
	struct RunNode *r;
	char buffer[32];

	/* Print Column Headings */

	fprintf(outfile,"NAME,TEAM,SEX,AG,EVENT,DATE,TIME(SECS),POINTS,TYPE(0=outside 1=official 2=informal)\n");
    /* Loop over runners, and over their runs, writing one line for each one */
	for(i=0;i<RunnerCount;i++){
		rr = AllRunners[i];
		n = rr->GetNumberOfRuns();
		m = rr->GetRelayNumber();
		rs = rr->GetRunsPointer();
		rls = rr->GetRelaysPointer();
		for(j=0;j<n;j++){
			r = rs[j];
			fprintf(outfile,"%s,",rr->GetName());
			fprintf(outfile,"%s,",rr->GetTeamName());
			fprintf(outfile,"%s,",rr->GetSex());
			k = AgeGroupNo(rr->GetDob(),&(r->date),rr->GetSex());
			fprintf(outfile,"%s,",AgeGroups[k]);
			fprintf(outfile,"%s,",EventNames[r->distance]);
#ifdef UNDER_CE
			sprintf(buffer,"%d/%d/%d",(r->date).tm_mon+1,(r->date).tm_mday,(r->date).tm_year+1900);
#else
			strftime(buffer,31,"%m/%d/%Y",&(r->date));
#endif
			fprintf(outfile,"%s,",buffer);
			fprintf(outfile,"%f,",r->time);
			if(resolution == 0.5)
				fprintf(outfile,"%d.%d,",r->points/2,5*(r->points % 2));
			else fprintf(outfile,"%d,",r->points);
			fprintf(outfile,"%d\n",r->type);
		}
		/* Repeat for relays */
		for(j=0;j<m;j++){
			rl = rls[j];
			fprintf(outfile,"%s,",rr->GetName());
			fprintf(outfile,"%s,",rr->GetTeamName());
			fprintf(outfile,"%s,",rr->GetSex());
			k = AgeGroupNo(rr->GetDob(),&(rl->date),rr->GetSex());
			fprintf(outfile,"%s,",AgeGroups[k]);
			fprintf(outfile,"%s,",EventNames[rl->distance]);
			#ifdef UNDER_CE
			sprintf(buffer,"%d/%d/%d",(r->date).tm_mon+1,(r->date).tm_mday,(r->date).tm_year+1900);
#else
			strftime(buffer,31,"%m/%d/%Y",&(rl->date));
#endif
			fprintf(outfile,"%s,",buffer);
			fprintf(outfile,"%f,",rl->time);
			if(resolution == 0.5)
				fprintf(outfile,"%d.%d,",rl->points/2,5*(rl->points % 2));
			else fprintf(outfile,"%d,",rl->points);
			fprintf(outfile,"%d\n",rl->type);
		}
	}
	return 0;
}

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
	     if(ExcludedRelaysHasMember(i) == 0){
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
	 *out << "Current Team Standings (First to last.)" << endl;
	 out->setf(ios::left,ios::adjustfield);
	{ // adjust width for Team name column if needed
		int len = TeamNameFieldWidth;
		for(i=0;i<number_teams;i++)
			if(strlen(teams[i]->GetLongName())>len)
				len  = strlen(teams[i]->GetLongName());
	 	out->width(len);
	}
	 *out << "Team Name";
	 out->setf(ios::right,ios::adjustfield);
	 out->width(10);
	 *out << "Size" << setw(10) <<   "Reserve"
	      << setw(10) << "Pts(Rsv)" << setw(10) <<
	      "Pts(Tot)" << setw(10) << "Pts(Max)" << endl;

	 for( i=number_teams-1;i>=0;i--)
		*out << *(teams[i]);
	 *out << endl;
	 *out << endl;
	 *out << "Legend:" << endl;
	 *out << "*  Denotes alternate distance run for event" << endl;
	 *out << "[] Denotes reserve score: runner has not finished required official runs" << endl;
	 *out << "{} Denotes non-compliant score: too many outside events" << endl;
	 *out << "   Non-compliant and reserve scores are not counted in team totals";

	return 0;
}
   /* display report on an individual runner */
   /* Return 1 if runner not found */

int RunnerReport(char * name, fstream *out)
{
      Runner *LookFor;
      Runner *ARunner;
      char buffer[2]; /* just to avoid compiler warning creating dummy runner */

      LookFor = new Runner(buffer,buffer,buffer,0,0,0,buffer);
      LookFor->SetName(name);
      krqsort((void **)AllRunners,0,RunnerCount-1,
		    (int (*)(void *,void *))CompareNames);

      ARunner = (Runner *)binsearch((void *)LookFor,
		(void **)AllRunners,RunnerCount,
		(int (*)(void *,void *))CompareNames);
      delete LookFor;
			  if (ARunner != NULL){
				ARunner->PrintAll(out);
				return 0;
			  }
			  else return 1;
 }

 /* Print the given file. Changes position in file, so save
    before invoking and restore afterwards */

int PrintOut(FILE *tempfile, int setup)
 {
	

       //if(setup == 1) // landscape
    /* Initialize Printer for landscape and CR translation*/
	//fprintf(stdprn,PrinterInitString);
       //else fprintf(stdprn,PrinterNormalString);
	   // Send tempfile to the printer
	//rewind(tempfile);
	//while(fgets(buffer,BUFFERSIZ,tempfile)!=NULL)
		//fprintf(stdprn,buffer);

       // restore default printer settings
      // if(setup == 1)
	//fprintf(stdprn,PrinterResetString);
	return 0;
}

#if 0  /* This is obsolete */
int PrintOut(FILE *tempfile, int setup)
{
	FILE *temporary;
	char tempname[BUFFERSIZ] = "score2012XXXXXX";
	char buffer[BUFFERSIZ];
	
	temporary = mkstemp(tempname);   /* create name of temporary file */
	if(setup == 1) // landscape
		fprintf(temporary,"%s",PrinterInitString);
	rewind(tempfile);
	while(fgets(buffer,BUFFERSIZ,tempfile)!=NULL)
		fprintf(temporary,"%s",buffer);
	if(setup == 1)
		fprintf(temporary,"%s",PrinterResetString);
	strcpy(buffer,"lpr  ");
	strcat(buffer,tempname);
	system(buffer);
	fclose(temporary);
	return 0;
}
#endif

/*  Superlatives: best and worst performances */                         
/* Returns -1 in the event of an error */


int Superlatives(fstream *out, char *event)
{                         
	int i,j,k;

	if((i = EventNo(event)) == -1) return -1; 
	if( i >= NOEVENTS - NORELAYS) return -1;
	rinfo.EventNumber = i;
	
	k = 25;
	SortFlag = BYTIMES;
	MySort((void **)AllRunners,0,RunnerCount-1,SortFlag);
	if(direction == 1){       /* slowest */
	
	     *out << EventNames[rinfo.EventNumber] << ": Last Performers By " <<
	     " Times " << endl;
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
		  " Times " << endl;
	  for(i=0;(i<k)&&(i<RunnerCount);i++){
	      *out << i + 1 << ". "; 
	      AllRunners[i]->BestRunReport(rinfo.EventNumber,out);
	  }
	}
	krqsort((void **)AllRunners,0,RunnerCount-1,
	(int (*)(void *,void *))CompareNames);

	return 0;
}




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

int MySort(void **array, int left, int right, int flag)
{
	switch(flag) {
	case BYLASTNAME:
		 krqsort((void **)array,left,right,
			(int (*)(void *,void *))CompareNames);
		 return 0;
	case BYTOTALPOINTS:
		 krqsort((void **)array,left,right,
			(int (*)(void *,void *))CompareTotalPoints);
		 return 0;
		
	case BYEVENTPOINTS:        
		 krqsort((void **)array,left,right,
			(int (*)(void *,void *))CompareIndPoints);
		 return 0;
	case BYTIMES:
		 krqsort((void **)array,left,right,
			(int (*)(void *,void *))CompareTimes);
		 return 0;
	}
	return 0;
}
