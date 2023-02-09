/* score15: score the noontime running league */

/* The following Data files must be declared in score.rc:

	RosterFile:   Information about all runners in the league
	ResultsFile:  All results submitted so far.
	TeamsFile:     Information about all teams in the league

Must be in the search path (DOS) or home directory (Unix):
	score.rc (Unix: .scorerc)     Global setup information
*/

/**********************************************************
		HEADER
*********************************************************/

#define __CPLUSPLUS__

#include "global.h"
#include "events.h"
#include "team.h"
#include "runner.h"
#include <ctype.h>
#include <string.h>
#include <fstream.h>
#include <time.h>
#ifndef __MSDOS__
#include<unistd.h>
#endif

#ifdef _BSD
extern "C" int chdir(char *);
extern "C" char *getwd(char *);
#endif


#ifdef __MSDOS__ 
const char opsep = '/';
char DriveLetter[3] = "\0\0";
char StartUpDrive[3];
#define INFO  "                   SCORE 1.6 (Chicken Haven Software)\n\
Menu Driven Program for managing and scoring the SU Noontime Running League.\n\
Reads data from various files in the working directory.\n\
 \n\
Usage: score [/f <results,roster,teams> /y <year> /s <scorall> /q /help /? /v]\n\
See the online help for more information.\n\
 "
#define USAGE "score [/f <results,roster,teams> /y <year> /s <scorall> /q /help /? /v]\n"

#else
const char opsep = '-';
 #define INFO  "                   SCORE 1.6 (Chicken Haven Software)\n\
Menu Driven Program for managing and scoring the SU Noontime Running League.\n\
Reads data from various files in the working directory.\n\
 \n\
Usage: score [-f <results,roster,teams> -i -y <year> -s <scorall> -filter -q \n\
		-help  -v]\n\
See the online help for more information.\n\
 "

#define USAGE "score [-f <results,roster,teams> -i -y <year> -s <scorall> -filter -q \n\
		-help -v]\n"
#endif

#ifndef VERSION
#define VERSION "1.61"                        
#endif

#define FATAL "Fatal Error. The \
errlog in Working Dir may have more information.\n\
Hit <RETURN> to return to the shell.\n"
#define MAX_ARG_LEN 80  /* Max length of combined command line options */


// INITIALIZED DATA


/* states for results file parser */
enum State {ROOT,DATED,DISTANCED,PERFORMANCED,RELAYED};
const char *TypeNames[]={"Outside","Official","Informal"};
int line = 1;        // line counter
int RunnerCount = 0;     
int number_teams = 0;
int use_old_age = 0;      // flag to use old age calculation method.
int exclusive_extremes = 0; // flag for either-or 100m, 15km (used in 95-96)
Runner *LookFor ; // Used in searches        
int found = 0;       // used in searches                                    
int Error_Flag = 0;    // When to clear error window
int SortFlag = 0; // 0: sort on name, 1: sort on total points
int direction = 0;           // flag for sort direction: descending is default        
double resolution = DEFAULT_RESOLUTION;
int _filter = 0; // flag for filter mode
int either_10m_or_15k = 0; // flag to allow 10 miles in place of 15k
int either_mile_or_metric = 0; // flag to allow 1 mile in place 1500
int either_100m_or_55m = 0;   // flag to allow 55m in place of 100m

// SUBROUTINES

int find_opt(char *option_string);  // For command line args
extern double score(int,int,double);
extern int mytrunc(double);
extern  int ParseResults(FILE *, 
	char **, struct RUNINFO *, int *);       // Parses results file
extern  int ParseTeamsFile(FILE *,Team *teams[], int *, int *);
extern int ParseRosterFile(FILE *,Team *teams[], Runner *AllRunners[],
	struct PERSON_INFO *, int *, int, int *);
extern int ParseRcFile(FILE *, struct EXCLUDEDRELAYS *, struct EXCLUDED *,
	int *, char *, char *, char *, char *, char *, 
		char *,char *, char *,char *);
extern "C" void krqsort( void *v[],int , int ,
 int (*)(void *,void *));    // Generic sort routine
extern "C" void *binsearch(void *,void *v[],
int, int (*) (void *, void *));  // Generic binary search
extern int RunnerReport(char *);
int CompareNames(Runner *, Runner *);    // orderings used with
int ComparePoints(Team *, Team *);   // sort and search routines
int CompareTimes(Runner *First, Runner *Second); 
void quit(int);
int CompareTotalPoints(Runner *First, Runner *Second);
int CompareIndPoints(Runner *First, Runner *Second); 
int MySort(void *v[], int, int,int);
extern "C" double atosecs( char *);   // convert time string
extern double score(int,int,double);  // scores an individual event
extern  int AgeGroupNo(struct tm*,const struct tm*,const char *);
extern int ScoreRelay(int,int,int,int,int,double,struct RelayNode *);
extern int ScoreRelay(int,int,int,int,int,double);
extern int scorall(fstream *);
int ExcludedEventsHasMember(int );
int ExcludedRelaysHasMember(int );
int load();            // loads all data

#ifdef __MSDOS__
extern int far gui();
#else
extern int gui();
#endif
void myerror();

// BCC Data

int index_1500m;
int index_mile;
int index_100m;
int index_15k;
int index_10M;
int index_55m;
time_t now;

char buffer[80];
char buffer2[80];          //  buffers



// Stuff used by ParseResults

struct RUNINFO {
	struct tm Date;           /* Of the run */
	char ShortName[SHORTNAMELEN];       /* Of the runner */
	char TimeString[15];      /* Time run in the event */
	int EventNumber;         /* index into EventNames */
	int RelayLegEventNumber;    
	int type;   /* index into TypeNames */
	char Leg1[20];
	char Leg2[20];
	char Leg3[20];     /* Short names of Relay Leg Runners */
	char Leg4[20];
	} rinfo;

// Stuff used by ParseRosterFile

struct PERSON_INFO{
	char ShortName[20];
	int month;
	int day;     /* DOB */
	int year;
	char sex[2];
	char teamname[SHORT_TEAM_NAME_LENGTH];
	} pinfo;

// Stuff read in from score.rc:
struct EXCLUDED Excluded;

struct EXCLUDEDRELAYS ExcludedRelays;

int CompetitionYear;                                       
char RosterFile[256];           // name of roster file  
char ResultsFile[256];   // Buffers for names of files
char TeamsFile[256];
char Scorall[256];
char WorkingDirectory[256];
char StartUpDirectory[256];
char TodaysDate[40];
char PrinterInitString[80];
char PrinterResetString[20];
char PrinterNormalString[20];
char TempFile[256];   // Path to a temporary file. Used for printout 

// Used in the load module

Runner *AllRunners[MAX_ROSTER];
char *Legs[4]; // Pointers to relay leg names.
int state; // of the parser
Team *teams[NO_TEAMS];
struct RunNode ARun;
struct RelayNode *ARelay;
struct RelayNode *RelayPtr;
Runner *ARelayRunner[4];
Runner *ARunner;
char *Tokens[5];     // From Parsing Results File

// Files and Streams
FILE *results;     /* fd for results file */                                       
FILE *roster;
FILE *teamnames;
FILE *rcfile;
FILE *tempfile;
FILE *errstream;

/************************************************************
		 END OF HEADER
************************************************************/

int
main(int argc, char **argv)
{
	int i,j,k;
	int StartupDelay = 5;
	char *ptr; // Utility pointer
	int c=-1;                /* numeric option code */
	char *info = INFO;
	char argstr[MAX_ARG_LEN];
	char name_buf[64];
	int report_flag = 0;
	fstream *out;
      
	 argstr[0] = opsep;
	 argstr[1] = '\0';

/* Record current directory so it can be restored */
#ifdef _BSD
	getwd(StartUpDirectory);
#else
	getcwd(StartUpDirectory,256);
#endif
#ifdef __MSDOS__
	/* Get startup default drive so it can be restored */
	_splitpath(StartUpDirectory,StartUpDrive,NULL,NULL,NULL);
#endif

	/* Open and read in Global Setup from rc file */

#ifdef __MSDOS__
	  if((rcfile = fopen(searchpath("score.rc"),"r"))==NULL){
		cout << FATAL << flush;
		gets(buffer);
		fprintf(stderr,"Cannot open score.rc\n");
		return 1; 
		}
#else
/* In Unix, we assume the rc-file is in the user's home directory. */
	  strcpy(buffer,getenv("HOME"));
	  strcat(buffer,"/.scorerc");
	  if((rcfile = fopen(buffer,"r"))==NULL){
		cout << FATAL << flush;
		fgets(buffer,2,stdin);
		fprintf(stderr,"Cannot open .scorerc. This file MUST be present\n");
		return 1;
		}
#endif
	  if(ParseRcFile( rcfile,&ExcludedRelays,&Excluded,&CompetitionYear,
		RosterFile,ResultsFile,TeamsFile,PrinterInitString,
		PrinterResetString,TempFile,Scorall,WorkingDirectory,
		PrinterNormalString) != 0){
		cout << FATAL << flush;
		fgets(buffer,2,stdin);
		fprintf(stderr,"Parse Error in setup file\n");
		return 1;
		}
		fclose(rcfile);

/* Kludge for 1995-6: we need to score the 100m and 15k in a special way:
	the higher of the two scores is used in the total score.
	We compute the index of these two events and store it, rather
	than building it in. This is good programming practice. 
*/
	for(i=0;(i<NOEVENTS)&&(strcmp(EventNames[i],"100m")!=0)
					;i++);
	index_100m = i;
	for(i=0;(i<NOEVENTS)&&(strcmp(EventNames[i],"55m")!=0)
					;i++);
	index_55m = i;

	for(i=0;(i<NOEVENTS)&&(strcmp(EventNames[i],"15k")!=0)
					;i++);
	index_15k = i;
	for(i=0;(i<NOEVENTS)&&(strcmp(EventNames[i],"10M")!=0)
					;i++);
	index_10M = i;

	/* Same kludge, 2002-3 */
	for(i=0;(i<NOEVENTS)&&(strcmp(EventNames[i],"1500m")!=0)
					;i++);
	index_1500m = i;
	for(i=0;(i<NOEVENTS)&&(strcmp(EventNames[i],"mile")!=0)
					;i++);
	index_mile = i;


      

	/* Store Todays Date in it's own buffer */
	now = time(NULL);
	strftime(TodaysDate,40,"%d %B %Y",localtime(&now));

// PROCESS COMMAND LINE OPTIONS

if(--argc > 0){  /* Anything on command line ? */
		++argv;
		while((argc > 0)&&((*argv[0]) == opsep)){
			argstr[0]=opsep;
			argstr[1]='\0';
			strcat(argstr,++*argv); /* start building argstr*/
			argc--;
			while((argc>0)&&((*(argv+1))[0]==opsep)&&((*(argv+1))[1]!=opsep)){
/* combine all options into a single string */
				++argv;
				argc--;
				strcat(argstr,++*argv);
			}
			++argv;
		       c = find_opt(argstr+1); /* look up code */
		switch(c){

			case -1: break;  /* default, e.g no options */
			case 9: /* filter mode */
				_filter = 1; 
				break;

			case 0: printf("%s\n",info);
				return 0;
			case 4:  // /q followed by /f
				StartupDelay = 0;
			case 1:          /* /f parse and set filenames */
				// glob up  command-line args up till the
				// the next /  or end of args
				buffer[0]='\0';
				while((argc >0)&&((*argv)[0]!=opsep)){
					strcat(buffer,*argv++);
					argc--;
					}
				  if(buffer[0]=='\0'){
					fprintf(stdout,USAGE);
					return 1;
					}
				  if((ptr = strtok(buffer,","))!=NULL)
				   strcpy(ResultsFile,ptr);
				  else { 
				   fprintf(stdout,USAGE);
				   return 1;
				  }
				  if((ptr = strtok(NULL,","))!=NULL)
				   strcpy(RosterFile,ptr);
				  else {
					fprintf(stdout,USAGE);
					return 1;
					}
				  if((ptr = strtok(NULL,"\0"))!=NULL)
				   strcpy(TeamsFile,ptr);
				   else {
					fprintf(stdout,USAGE);
					return 1;
				   }
				  break;
			case 2:   /* Give your version */
				printf("Version %s\n",VERSION);
				return 0;
			case 3:
				StartupDelay = 0;
				break;
			case 6:  // /q followed by /y
				StartupDelay = 0;
			case 5:  // The /y option
				CompetitionYear = atoi(*argv++);
				argc--;
				break;
			case 8:   // the /qs option
				StartupDelay = 0;
			case 7: strcpy(Scorall,*argv++);   // the /s option
				argc--;
				break;
			case 11: report_flag = 1;
				strcpy(name_buf,*argv++);
				argc--;
				break;
			case 10:  /* just dump info from rc file and exit */
				fprintf(stdout,"CompetitionYear = %d\n",CompetitionYear);
				fprintf(stdout,"WorkingDirectory = %s\n",WorkingDirectory);
				fprintf(stdout,"RosterFile = %s\n",RosterFile);
				fprintf(stdout,"ResultsFile = %s\n",ResultsFile);
				fprintf(stdout,"TeamsFile = %s\n",TeamsFile);
				fprintf(stdout,"Scorall = %s\n",Scorall);
				fprintf(stdout,"TempFile = %s\n",TempFile);
				fprintf(stdout,"Excluded Events: \n");
				for(i=0;i<Excluded.number;i++)
					fprintf(stdout,"%s\n",
					  EventNames[Excluded.events[i]]);
				fprintf(stdout,"Excluded Relays: \n");
				for(i=0;i<ExcludedRelays.number;i++)
					fprintf(stdout,"%s\n",
					EventNames[ExcludedRelays.events[i]]);
				return 0;
			case -2: ;  /* barf (don't use in your code) */
			default:
				// should use stderr, but its redirected
				fprintf(stdout,"%s: illegal option\n",argstr);
				return 1;
			}  // end switch
		}  // end while
	}         // end if
       

// Set appropriate default drive (Dos only)
#ifdef __MSDOS__
// Let's see if the working directory path supplies a drive letter
	_splitpath(WorkingDirectory,DriveLetter,NULL,NULL,NULL);
	if(DriveLetter[0]!='\0')
		system(DriveLetter);
	/* Else drive is the same as it was when pgm started */
/* change to working directory */
	strcpy(buffer,"cd ");
	strcat(buffer,WorkingDirectory);
	system(buffer);
#else
	if(chdir(WorkingDirectory)!=0){
		fprintf(stderr,"Unable to cd to working directory\n");
		quit(1);
	}
#endif

       errstream = fopen("errlog","w"); // send error messages to log
					// during data load
       fprintf(errstream,"Noontime Running League Scoring Program Version %s\n",
		VERSION);
       fprintf(errstream,"              By Terry McConnell\n\n");
       fprintf(errstream,"Today is %s\n",TodaysDate);
       fprintf(errstream,"Any Load Error Messages Follow:\n");
	// Initialize the table of standards
	// This table is built by running the mkstnds program

	InitTable();
	 
// LOAD DATA
        
       
	 if(load() == 1) {
			   cout << FATAL << flush;
			   fgets(buffer,2,stdin);
			   quit(1);
	 }

	fclose(errstream);
	if(report_flag){
		RunnerReport(name_buf);
	}
    	if(_filter) quit(0); 

	if(StartupDelay == 0) {     // Quick option
#ifdef __MSDOS__
		system("type errlog");
#else
		system("cat errlog | more");
#endif

		/* Small menu for actions */
try_again:
		cout << endl;
		cout << "Create Scorall ? [yn]";
		fgets(buffer,2,stdin);
		if(buffer[0]=='n')quit(0);
		if(buffer[0]!='y') goto try_again;	

		out = new fstream(Scorall,ios::out);
		if(out == NULL) {
			cout << FATAL << " Unable to open " << Scorall << endl;
			quit(1);
		}
		scorall(out);
		quit(0);
	}
       //fclose(errstream);
/* Copy errlog to tempfile and open it for read */
          if(TempFile[0] == '\0') {
		cout << FATAL << flush;
		 fprintf(stderr,"No Temp File: Check score.rc entry\n");
                fgets(buffer,2,stdin);
                quit(1);
	  }
#ifdef __MSDOS__
	  sprintf(buffer,"copy errlog %s", TempFile);
	  system(buffer);
#else

	fstream in("errlog",ios::in);
	fstream temp_out(TempFile,ios::out);
	while((i=in.get())!=EOF)temp_out.put(i);
	in.close();
	temp_out.close();

//	  sprintf(buffer,"cp errlog %s", TempFile);
#endif
//	  system(buffer);

#ifdef __MSDOS__
          if((tempfile = fopen(TempFile,"r"))== NULL){
		cout << FATAL << flush;
		fprintf(stderr,"Unable to open %s\n",TempFile);
                gets(buffer);
              quit(1);
	  }
         
#endif

 // go to user interface

	  if(1){
		fprintf(stderr,"Gui is not supported\n");
	  }

 // Clean up
	
	  quit(0);
	  return 0; /* Not needed, but prevents compiler warning */
}


/***************************************************************************
	 LOAD DATA SECTION: parse and load data on teams, runners, runs
***************************************************************************/

int load()
{
	int i,j,k,l;

	/* Open and read in the team names */
	
	 if((teamnames = fopen(TeamsFile,"r"))== NULL){
		fprintf(errstream,"Unable to open %s\n",TeamsFile);
		quit(1);
		}

	 if(ParseTeamsFile(teamnames, teams, &line, &number_teams) != 0){
		fprintf(errstream,"Exiting on error parsing teams file\n");
		quit(1);
	 }
		fclose(teamnames);

	/* Open the roster for reading in the team data */

	if((roster = fopen(RosterFile,"r"))==NULL){
		fprintf(errstream,"Cannot open %s\n",RosterFile);
		quit(1);
		}

	line = 1;
	ParseRosterFile(roster, teams, 
		AllRunners, &pinfo,&line, number_teams,&RunnerCount);

	fclose(roster);

	// Roster is now in core as AllRunners
	// Sort the roster
	krqsort((void **)AllRunners,0,RunnerCount-1,
		(int (*)(void *,void *))CompareNames);


	/* open results file */
	if((results = fopen(ResultsFile,"r"))==NULL){
		fprintf(errstream,"cannot open %s\n",ResultsFile);
		quit(1);
	}

	// Parse the Results file

	/* allocate memory for tokens */
	for(i=0;i<5;i++){
	Tokens[i] = (char *)malloc(20);
	assert(Tokens[i] != NULL);
	}
	line = 0;
	while((state = ParseResults(results,Tokens,&rinfo,&line))!= -1){
		if( state == PERFORMANCED){
                        if(rinfo.EventNumber != -1){    // -1 is returned for
                                                // the incredibly stupid
                                                // special event added in
                                                // fall 96
                         ARun.type = rinfo.type;
                         ARun.distance = rinfo.EventNumber;
                         ARun.time = atosecs(rinfo.TimeString);
                         ARun.date = rinfo.Date;
                        }
			Runner *LookFor = new Runner(rinfo.ShortName,"noteam","m",0,0,0," ");
			Runner *BRunner = (Runner *)binsearch((void *)LookFor,
			(void **)AllRunners,RunnerCount,
			(int (*)(void *,void *))CompareNames);
			delete LookFor;
			if ( BRunner != NULL ){
                         if(rinfo.EventNumber != -1){      // See above
                          BRunner->AddRun(&ARun);

/* If we are running in filter mode,  tack on 
   the score for this event */
			  if(_filter){
				double temp;
				double pts;
	         		pts = score(
				  AgeGroupNo(BRunner->GetDob(),&(ARun.date), BRunner->GetSex()), ARun.distance, ARun.time);
				temp = (double)mytrunc(pts);
				temp = (temp < 1/resolution ? 1/resolution : temp)*resolution;	
#ifdef _BSD
				printf(" =%.1g",temp);
#else
				printf(" =%g",temp);
#endif
			  }
			 }
                         else BRunner->AddSpecial();      
			}
			else {
				fprintf(errstream,"%s is not in the roster\n",rinfo.ShortName);
				fprintf(errstream,"%s, line %d\n",ResultsFile,line);
			}
			}
// Code to handle state == RELAYED
if( state == RELAYED){
			int errflag = 0;
			k = NO_TEAMS + 1;
			char *Legs[4];
			Legs[0]=rinfo.Leg1;
			Legs[1]=rinfo.Leg2;
			Legs[2]=rinfo.Leg3;
			Legs[3]=rinfo.Leg4;
			ARelay = new RelayNode;
			ARelay->type = rinfo.type;
			ARelay->distance = rinfo.EventNumber;
			ARelay->LegDistance = rinfo.RelayLegEventNumber;
			ARelay->time = atosecs(rinfo.TimeString);
			ARelay->date = rinfo.Date;
			int old_k = -1;
			for(i=0;i<4;i++){
				LookFor = new Runner(Legs[i],"noteam","m",0,0,0," ");
				ARelayRunner[i] = (Runner *)binsearch((void *)LookFor,
				(void **)AllRunners,RunnerCount,
				(int (*)(void *,void *))CompareNames);
				delete LookFor;
				if(ARelayRunner[i]==NULL){
					errflag =1;
					fprintf(errstream,"%s is not in roster\n",Legs[i]);
					fprintf(errstream,"%s, line %d\n",ResultsFile,line);
					break;
				}
				for( j =0;j<NO_TEAMS;j++)
					if(strcmp(teams[j]->GetShortName(),
								ARelayRunner[i]->GetTeamName())==0){
								k=j;
								if((old_k!=-1)&&(k!=old_k))
									errflag = 1;
								old_k = k;
								break;
								 }
								// k should now have team index

				ARelay->Legs[i]=ARelayRunner[i]->GetName();
			 ARelay->Dobs[i]=ARelayRunner[i]->GetDob();
		ARelay->Sexes[i]= ARelayRunner[i]->GetSex();
				}
				if((errflag ==0)&&(k!=(NO_TEAMS+1))){
				RelayPtr = (teams[k]->AddRelay(ARelay));
			for(i=0;i<4;i++)
					ARelayRunner[i]->AddRelay(RelayPtr);
			if(_filter){
				double temp;
				temp = (double)ARelayRunner[0]->GetRelayPoints(
					ARelayRunner[0]->GetRelayNumber()-1);
				temp = (temp < 1/resolution ? 1/resolution : temp)*resolution;	
#ifdef _BSD
				printf(" =%.1g",temp);
#else
				printf(" =%g",temp);
#endif
			}
				}
				else fprintf(errstream,"%s %s %s %s  on different or invalid team\n",
					rinfo.Leg1,rinfo.Leg2,rinfo.Leg3,rinfo.Leg4);
			}
	 if(_filter)printf("\n");
	 }
// Compute the team totals
	for(i=0;i<number_teams;i++)
	teams[i]->SetPoints();

// Sort the teams by total points scored
	krqsort((void **)teams,0,number_teams-1,
		(int (*)(void *,void *))ComparePoints);
/*************************************************************************
       END DATA LOAD SECTION
*************************************************************************/
return 0;
}

/* CompareNames: Returns -1 0 or 1 according as the name of
	the first runner is before, the same, or after that of the
	second, in the following ordering:

	inamea < jnameb iff nameai < namebj in alphabetic order
	where i and j are the first initials

	Used to sort runners by name

*/
int CompareNames(Runner *First, Runner *Second)
{
	char *first,*second;
	first = First->GetName();
	second = Second->GetName();
	if(direction == 0) {
	if(strcmp(first,second)==0)
		return 0;
	if (strcmp(first+1,second+1) == 0)
		return ( *first < *second ? -1 : 1 );
	return strcmp(first+1,second+1);
	}
	if(strcmp(first,second)==0)
		return 0;
	if (strcmp(first+1,second+1) == 0)
		return ( *first < *second ? 1 : -1 );
	return strcmp(second+1,first+1);
}

// Compare individuals by points for the current event.
// Contrast with ComparePoints which compares TEAMS by points

int CompareIndPoints(Runner *First, Runner *Second)
{
	double first,second;
	first = First->GetPoints(rinfo.EventNumber);
	second = Second->GetPoints(rinfo.EventNumber);
	if ( (first  == -11.0)&&(second == -11.0) ) return CompareNames(First,Second);
	if ( direction == 1){
		if(second == -11.0) return 1;      // hasn't run event
		if(first == -11.0) return -1;
		if(first == second) return CompareNames(First,Second);
		if(first < second ) return -1;
		return 1;
	}
	if(first == -11.0) return 1;
	if(second == -11.0)return -1;
	if(second == first) return CompareNames(First,Second);
	if(second < first ) return -1;
	return 1;
}

// Compare individuals by total points 
// Contrast with ComparePoints which compares TEAMS by points

int CompareTotalPoints(Runner *First, Runner *Second)
{
	int first,second;
	first = First->GetTotalPoints();
	second = Second->GetTotalPoints();
	if ( (first  == 0)&&(second == 0) ) return CompareNames(First,Second);
	if ( direction == 0){
		if(second == 0) return 1;      // hasn't run event
		if(first == 0) return -1;
		if(first == second) return CompareNames(First,Second);
		if(first < second ) return -1;
		return 1;
	}
	if(first == 0) return 1;
	if(second == 0)return -1;
	if(second == first) return CompareNames(First,Second);
	if(second < first ) return -1;
	return 1;
}


// CompareTimes: return -1,0,1 according as the first runner has run
//  faster,the same as, or slower than the second in the given event
//  if the direction flag is 0. Reverse roles of runners if the flag is not 0
//  Ties are broken by alpabetic order

int CompareTimes(Runner *First, Runner *Second)
{
	double first,second;
	first = First->GetTime(rinfo.EventNumber);
	second = Second->GetTime(rinfo.EventNumber);
	if ( first + second == 0.0 ) return CompareNames(First,Second);
	if ( direction == 0){
		if(second == 0.0) return -1;      // hasn't run event
		if(first == 0.0) return 1;
		if(first == second) return CompareNames(First,Second);
		if(first < second ) return -1;
		return 1;
	}
	if(first == 0.0) return -1;
	if(second == 0.0)return 1;
	if(second == first) return CompareNames(First,Second);
	if(second < first ) return -1;
	return 1;

}
/* ComparePoints: Return -1,0,1 according as the First team
		has scored fewer, than same as, or more points than
		the Second team.

		Used to sort teams by their point totals
*/

int ComparePoints(Team *First, Team *Second)
{
	int first,second;

	first = First->GetPoints();
	second = Second->GetPoints();
	if(first == second) return 0;
	if(first < second) return -1;
	return 1;
}

/*  find_opt: return a unique small integer for each possible option string */
/*  There should be a case in the switch statement in main to handle each. */
/*  -2 reserved for arg not found -1 reserved for no options */

int find_opt(char *word)
{
	if(strcmp(word,"help")==0)return 0;
	if(strcmp(word,"filter")==0)return 9;
	if(strcmp(word,"?")==0)return 0;
	if(strcmp(word,"f")==0)return 1;
	if(strcmp(word,"v")==0)return 2;
	if(strcmp(word,"q")==0)return 3;
	if(strcmp(word,"qf")==0)return 4;
	if(strcmp(word,"y")==0)return 5;
	if(strcmp(word,"qy")==0)return 6;
	if(strcmp(word,"s")==0) return 7;
	if(strcmp(word,"qs")==0) return 8;
	if(strcmp(word,"i")==0) return 10;
	if(strcmp(word,"r")==0) return 11;
/* arg not found */
	return -2;
}


// Called when there is an error
void myerror()                             
{
	Error_Flag = 1;
	return;
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
	
int ExcludedEventsHasMember(int i) {
	int j;
	for(j=0;j<Excluded.number;j++)
		if(Excluded.events[j]==i)
			return 1;
	return 0;
}

int ExcludedRelaysHasMember(int i) {
	int j;
	for(j=0;j<ExcludedRelays.number;j++)
		if(ExcludedRelays.events[j]==i)
			return 1;
	return 0;
}

void quit(int value){

//	 fclose(results);
//	 fclose(tempfile);
#ifdef __MSDOS__
	  system(StartUpDrive);  // restore Drive letter
	  strcpy(buffer,"cd ");
	  strcat(buffer,StartUpDirectory);
	  system(buffer);      // restore startup directory
	  system("cls");
#else
//	  chdir(StartUpDirectory);
//	  system("clear");
//	  system("echo \"\n\n\"");  // Assuming echo is sysv  
#endif
	  exit(value);
}

