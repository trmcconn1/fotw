// runner.cpp: implementation of Runner class


#define TOO_FAST 8.5
#define TOO_SLOW -4.0
#define MAX_INFORMAL 2

#include "runner.h"
#include<float.h>
#include<math.h>

#include "groups.h"

const int EventFieldWidths[]={7,7,7,7,7,7,7,7,8,8,10,10,10,10,10,10};
const int RelayFieldWidths[]={7,7,8,8,8};
const int NameFieldWidth = 13;

int mytrunc(double );
extern time_t now;
int AgeGroupNo(struct tm* DOB, const struct tm* rundate, const char *Sex);

// Default Constructor
Runner::Runner()
{
	Dob = new tm;
	no_runs = 0;
	cleared = 0;
        special = 0;    
	no_official = 0;
	no_relays = 0;
	no_informal = 0;
	best_15k_is_10M=0;
	best_1500m_is_mile = 0;
	best_15k_is_mar = 0;
	best_15k_is_hmar = 0;
	TotalPoints = 0;
	assert(Dob != 0);
	int i;

		for( i = 0; i< NOEVENTS; i++)
		BestRuns[i] = 0;
		for( i = 0; i< NORELAYS; i++)
		BestRelays[i] = 0;

}


// Interactive Constructor -- this is obsolete
/*
Runner::Runner(int mytype)
{
	int ContFlag = 1;
	int month,day,year;
	char buffer[BUFFERSIZ];

	no_runs = 0;
	cleared = 0;
        special = 0;
	no_official = 0;
	best_15k_is_10M=0;
	best_1500m_is_mile=0;
	no_informal = 0;
	no_relays = 0;
	TotalPoints = 0;
	for(int i = 0; i< NOEVENTS; i++)
	BestRuns[i] = 0;
   for( i = 0; i< NORELAYS; i++)
		BestRelays[i] = 0;
	Dob = new  tm;
	assert(Dob != 0);
	assert(mytype >=0);   // Useless. For signature only
	while(ContFlag == 1){
		Main_Window << "Input information on this runner." << endl;
		Main_Window << "Short Name ( e.g. tmcconnell ): ";
		cin >> ShortName;
		Main_Window << endl << "Long Name (e.g. Terry R. McConnell): ";
		gets(buffer);
		strcpy(LongName,buffer);
		Main_Window << endl << "Team Name ( use 3 letter abbreviation ): ";
		cin >> TeamName;
		Main_Window << endl << "Sex: ";
		cin >> Sex;
		Main_Window << endl << "Give month of birth[1-12]: ";
		month = atoi(gets(buffer));
		Main_Window << endl << "Give day of month[1-31]: ";
		day = atoi(gets(buffer));
		Main_Window << endl << "Give year of birth[...1999]: ";
		year = atoi(gets(buffer));
		Dob->tm_year = year - 1900;
		Dob->tm_mon = month -1;
		Dob->tm_mday = day;
		Main_Window << "\nIs the following information correct?\n" ;
		PrintAll(&Main_Window);
		Main_Window << endl << "[yn] ";
		cin >> buffer;
		ContFlag = ( buffer[0] == 'y' ? 0 : 1 );
		}
}
*/
// Constructor called with data
Runner::Runner(char *sname,char *tname,char *sex,int mo,
	int day,int yr,char *lname )

{
	int i;
	no_runs = 0;
	cleared = 0;
        special = 0;
	no_official = 0;
	best_15k_is_10M=0;
	best_15k_is_hmar = 0;
	best_15k_is_mar = 0;
	best_1500m_is_mile=0;
	no_informal = 0;
	TotalPoints = 0;
	no_relays = 0;
		for( i = 0; i< NOEVENTS; i++)
		BestRuns[i] = 0;
      for( i = 0; i< NORELAYS; i++)
		BestRelays[i] = 0;
	strcpy(ShortName,sname);
	strcpy(LongName,lname);
	strcpy(TeamName,tname);
	strcpy(Sex,sex);
	Dob = new  tm;
	assert(Dob != 0);
	if(yr >= 1900)
		Dob->tm_year = yr-1900;
	else
		Dob->tm_year = yr;
	Dob->tm_mon = mo -1;
	Dob->tm_mday = day;

}

// Copy Constructer

Runner::Runner(Runner& ARunner)
{
	int i,j;
	no_runs = ARunner.no_runs;
	cleared = ARunner.cleared;
        special = ARunner.special;
	no_official = ARunner.no_official;
	best_15k_is_10M=ARunner.best_15k_is_10M;
	best_1500m_is_mile=ARunner.best_1500m_is_mile;
	best_15k_is_mar = ARunner.best_15k_is_mar;
	best_15k_is_hmar = ARunner.best_15k_is_hmar;
	no_informal = ARunner.no_informal;
	no_relays = ARunner.no_relays;
   TotalPoints = ARunner.TotalPoints;
	for(i = 0;i<no_runs;i++)
	{ Runs[i] = new  RunNode;
	  assert (Runs[i] != 0);
		Runs[i]->type = ARunner.Runs[i]->type;
		Runs[i]->distance = ARunner.Runs[i]->distance;
		Runs[i]->time = ARunner.Runs[i]->time;
		Runs[i]->date.tm_year = ARunner.Runs[i]->date.tm_year;
		Runs[i]->date.tm_mon = ARunner.Runs[i]->date.tm_mon;
		Runs[i]->date.tm_mday = ARunner.Runs[i]->date.tm_mday;
	}
		for( j = 0; j< NOEVENTS; j++)
		BestRuns[j] = ARunner.BestRuns[j];
		for(  j = 0; j< NORELAYS; j++)
		BestRelays[j] = ARunner.BestRelays[j];

	Dob = new tm;
	assert (Dob != 0);
	strcpy(ShortName,ARunner.ShortName);
	strcpy(LongName,ARunner.LongName);
	strcpy(TeamName,ARunner.TeamName);
	strcpy(Sex,ARunner.Sex);
	Dob->tm_year = ARunner.Dob->tm_year;
	Dob->tm_mon = ARunner.Dob->tm_mon;
	Dob->tm_mday = ARunner.Dob->tm_mday;
}
// Destructor
Runner::~Runner()
{
	delete Dob;
	for(int i = 0;i<no_runs;i++)
	  delete Runs[i];

}

int Runner::GetRelayPoints(int index)
{
	if (index >= no_relays) return -1;
	return Relays[index]->points;
}

int Runner::GetRelayNumber()
{
	return no_relays;
} 
void Runner::AddRelay( struct RelayNode *ARelay)
{
	int i = no_relays++;
	int temp;
	int k;
	Relays[i] = ARelay;
	int j = Relays[i]->LegDistance-1;
		/* See if this is a best Relay at this distance */
		if((BestRelays[j]==0 )||(BestRelays[j]->points < Relays[i]->points)
		|| ((BestRelays[j]->points == Relays[i]->points)&&
		(BestRelays[j]->time > Relays[i]->time))){
		if(BestRelays[j]==0)TotalPoints += Relays[i]->points;
      else
			TotalPoints += Relays[i]->points - BestRelays[j]->points;
			BestRelays[j]=Relays[i];
			}
		switch(Relays[i]->type){
			case official:
				no_official++;
				if(cleared < 4){   // check for number of dates required
				cleared++;  // but take away if he already has credit:
				temp = cleared;
				for(k=0;k<no_runs;k++) 
					if((ARelay->date.tm_year==Runs[k]->date.tm_year)&&
					   (ARelay->date.tm_mon==Runs[k]->date.tm_mon)&&
					   (ARelay->date.tm_mday==Runs[k]->date.tm_mday) && (Runs[k]->type == official)){
					   cleared--;
					   break;
					   }
				if(temp==cleared)          
				for(k=0;k<i;k++)
					if((ARelay->date.tm_year==Relays[k]->date.tm_year)&&
					   (ARelay->date.tm_mon==Relays[k]->date.tm_mon)&&
					   (ARelay->date.tm_mday==Relays[k]->date.tm_mday)){
					   cleared--;
					   break;
					  }
				 }

				break;
			case informal:
			default: ;
		}
}

void Runner::AddRun( const struct RunNode *ARun )
{          int i = no_runs++;
	   int k;
	   int temp;
	   char buffer[BUFFERSIZ];
	   double pts;
		Runs[i] = new  RunNode;
		Runs[i]->type = ARun->type;
		switch(Runs[i]->type){
			case official:
				no_official++;
				if(cleared < 4){   // check for number of dates required
				cleared++;  // but take away if he already has credit:
				temp = cleared;
				for(k=0;k<i;k++) 
					if((ARun->date.tm_year==Runs[k]->date.tm_year)&&
					   (ARun->date.tm_mon==Runs[k]->date.tm_mon)&&
					   (ARun->date.tm_mday==Runs[k]->date.tm_mday)&&(Runs[k]->type == official)){
					   cleared--;
					   break;
					 }
				if(temp==cleared)          
				for(k=0;k<no_relays;k++)
					if((ARun->date.tm_year==Relays[k]->date.tm_year)&&
					   (ARun->date.tm_mon==Relays[k]->date.tm_mon)&&
					   (ARun->date.tm_mday==Relays[k]->date.tm_mday)){
					   cleared--;
					   break;
					  }
				 }
				break;
			case informal:
				no_informal++;
				if(no_informal > MAX_INFORMAL)
					fprintf(errstream,"Too many informals: %s %d/%d/%d\n",
					ShortName,ARun->date.tm_mon+1,ARun->date.tm_mday,
					ARun->date.tm_year);

			default: ;
		}
		int j = Runs[i]->distance = ARun->distance;
		Runs[i]->time = ARun->time;
		Runs[i]->date.tm_year = ARun->date.tm_year;
		Runs[i]->date.tm_mon = ARun->date.tm_mon;
		Runs[i]->date.tm_mday = ARun->date.tm_mday;
		pts =  score(AgeGroupNo(Dob,&(ARun->date),Sex),ARun->distance,
		ARun->time);
		if (pts >= TOO_FAST/resolution){
			SecsToStr(Runs[i]->time,buffer);
			fprintf(errstream,"Very fast time: %s %s %s \n",
			ShortName,EventNames[Runs[i]->distance],
			buffer);
			}
		if ( pts <= TOO_SLOW/resolution ){
			SecsToStr(Runs[i]->time,buffer);
			fprintf(errstream,"Very slow time: %s %s %s \n",
			ShortName,EventNames[Runs[i]->distance],
			buffer);
			}
		temp = mytrunc( pts );
		Runs[i]->points = ( (double)temp < 1/resolution ? (int)(1/resolution) : temp );
		Runs[i]->pts = pts;

		/* See if this is a best best run at this distance */

                if((BestRuns[j]==0 )||(BestRuns[j]->pts < Runs[i]->pts)){


			/* Code to handle various special cases */

		       if((j==index_55m)||(j==index_1500m)||(j==index_mile)||(j==index_hmar)||(j==index_mar)||(j==index_15k)||(j==index_10M)||(j==index_100m)){	                         

                        if(exclusive_extremes && ((j==index_15k)||(j==index_100m))){

                        /* Code in this if was
                           used in 1995-96 when we scored the better of
                           the 100m and 15k. I am leaving it here with a
                           special flag so that the program can generate
                           results for previous years.
                        */

                       
                        if(BestRuns[index_100m]==0){
			 if(BestRuns[index_15k]==0)
			  temp = 0;
			 else temp = BestRuns[index_15k]->points;
			 }
			else {   
			if(BestRuns[index_15k]==0)
			 temp = BestRuns[index_100m]->points;
			else
			temp = ( BestRuns[index_100m]->points >
			   BestRuns[index_15k]->points ?
			    BestRuns[index_100m]->points :
			    BestRuns[index_15k]->points ) ;
                        }  
	   /* temp now has the better of the 15k and 100m points */
                       if(Runs[i]->points > temp)
			 TotalPoints += Runs[i]->points - temp;
                         
		 }

	    /* Code to handle score better of Mile and 1500m points. Rule
	     *  change in 2002-3 */

                          if(either_mile_or_metric && ((j==index_1500m)||(j==index_mile))){
                       		if(BestRuns[index_mile]==0){
			 		if(BestRuns[index_1500m]==0){
			  			temp = 0;
						if(j==index_mile)
							best_1500m_is_mile = 1;
					}
			 		else {
						temp = BestRuns[index_1500m]->points;
						if((j==index_mile)&&(Runs[i]->pts 
							> GetPoints(index_1500m)))
							best_1500m_is_mile = 1;
						else
							best_1500m_is_mile = 0;
					}
			 	}
			   	else {   
			        	if(BestRuns[index_1500m]==0){
			 			temp = BestRuns[index_mile]->points;
						if((j==index_1500m)&&(Runs[i]->pts
							> GetPoints(index_mile)))
							best_1500m_is_mile = 0;
						else best_1500m_is_mile = 1;
					}
					else {
						temp = ( BestRuns[index_mile]->points >
			   				BestRuns[index_1500m]->points ?
			    				BestRuns[index_mile]->points :
			    				BestRuns[index_1500m]->points ) ;
						if((Runs[i]->pts > GetPoints(index_mile))&&
							(Runs[i]->pts > GetPoints(index_1500m)))
							if(j == index_1500m)
							   best_1500m_is_mile = 0;
						        else
							   best_1500m_is_mile = 1;
					}
                        	}  
	   /* temp now has the better of the 1500m and 1 mile points */
                       		if(Runs[i]->points > temp)
			 		TotalPoints += Runs[i]->points - temp;
                         
			  	}

	    /* Code to handle score better of 55m and 100m points. Rule
	     *  change in 2003-4 */

                          if(either_100m_or_55m && ((j==index_55m)||(j==index_100m))){
                       		if(BestRuns[index_100m]==0){ // no 100m time
			 		if(BestRuns[index_55m]==0){
			  			temp = 0;
						if(j==index_100m)
							best_100m_is_55m = 0;
					}
			 		else {
						temp = BestRuns[index_55m]->points;
						if((j==index_100m)&&(Runs[i]->pts 
							> GetPoints(index_55m)))
							best_100m_is_55m = 0;
						else
							best_100m_is_55m = 1;
					}
			 	}
			   	else {   // got a 100m time
			        	if(BestRuns[index_55m]==0){
			 			temp = BestRuns[index_100m]->points;
						if((j==index_55m)&&(Runs[i]->pts
							> GetPoints(index_100m)))
							best_100m_is_55m = 1;
						else best_100m_is_55m = 0;
					}
					else {
						temp = ( BestRuns[index_100m]->points >
			   				BestRuns[index_55m]->points ?
			    				BestRuns[index_100m]->points :
			    				BestRuns[index_55m]->points ) ;
						if((Runs[i]->pts > GetPoints(index_55m))&&
							(Runs[i]->pts > GetPoints(index_100m)))
							if(j == index_55m)
							   best_100m_is_55m = 1;
						        else
							   best_100m_is_55m = 0;
					}
                        	}  
	   /* temp now has the better of the 100m and 55m mile points */
                       		if(Runs[i]->points > temp)
			 		TotalPoints += Runs[i]->points - temp;
                         
			  	}

/* Code to handle scoring best of 15k, 10M, hmar, and mar as 15k, introduced 2006 */

				  if(enable_combined_dr && ( (j==index_15k)||(j==index_10M)||(j==index_hmar)
					  ||(j==index_mar)))
				  {

					  int a=0;
					  int b=0;
					  int c=0;
					  int d=0;
					  int e=0;
					 
					  if(BestRuns[index_15k])a = BestRuns[index_15k]->points;
					  if(BestRuns[index_10M])b = BestRuns[index_10M]->points;
					  if(BestRuns[index_hmar])c = BestRuns[index_hmar]->points;
					  if(BestRuns[index_mar])d = BestRuns[index_mar]->points;
					  e = Runs[i]->pts;

						if((e>a)&&(e>b)&&(e>c)&&(e>d)){// new best distance run
					              // need to set flags to indicate type of new best
							if(j==index_15k){

								best_15k_is_10M = 0;
								best_15k_is_hmar = 0;
								best_15k_is_mar = 0;
							}
							else { 
								if(j==index_10M){
									best_15k_is_10M = 1;
									best_15k_is_hmar = 0;
									best_15k_is_mar = 0;
								}
								else if(j==index_hmar){
									best_15k_is_10M = 0;
									best_15k_is_hmar = 1;
									best_15k_is_mar = 0;
								}
								else {/* must be mar */
									best_15k_is_10M = 0;
									best_15k_is_hmar = 0;
									best_15k_is_mar = 1;

								}
							}
							// Find the max m of a,b,c,d so we can credit point total
							// with e-m
							temp = (a > b)? a : b;
							if(c > temp) temp = c;
							if(d > temp) temp = d;
							TotalPoints += (e - temp);
						}
						/* else not a new best distance run.  */
					
				  }

            /* Code to handle scoring better of 15K and 10M point */

                          if((either_10m_or_15k) && ((j==index_15k)||(j==index_10M)
							  )){
                       		if(BestRuns[index_10M]==0){
			 		if(BestRuns[index_15k]==0){
			  			temp = 0;
						if(j==index_10M)
							best_15k_is_10M = 1;
					}
			 		else {
						temp = BestRuns[index_15k]->points;
						if((j==index_10M)&&(Runs[i]->pts 
							> GetPoints(index_15k)))
							best_15k_is_10M = 1;
						else
							best_15k_is_10M = 0;
					}
			 	}
			   	else {  // have a 10 miler already
 
			        	if(BestRuns[index_15k]==0){
			 			temp = BestRuns[index_10M]->points;
						if((j==index_15k)&&(Runs[i]->pts
							> GetPoints(index_10M)))
							best_15k_is_10M = 0;
						else best_15k_is_10M = 1;
					}
					else {
						temp = ( BestRuns[index_10M]->points >
			   				BestRuns[index_15k]->points ?
			    				BestRuns[index_10M]->points :
			    				BestRuns[index_15k]->points ) ;
						if((Runs[i]->pts > GetPoints(index_10M))&&
							(Runs[i]->pts > GetPoints(index_15k)))
							if(j == index_15k)
							   best_15k_is_10M = 0;
						        else
							   best_15k_is_10M = 1;
					}
                        	}  
	   /* temp now has the better of the 15k and 10M points */
                       		if(Runs[i]->points > temp)
			 		TotalPoints += Runs[i]->points - temp;
                         
			  	}
                       }
		  	else  // no special rule or Not 10M, 15k, or 100m
		    		if(BestRuns[j]==0)TotalPoints += Runs[i]->points;
		    		else
		       			TotalPoints += Runs[i]->points - BestRuns[j]->points;
		BestRuns[j]=Runs[i];    // new best run at this dist
		}

}

/* The purpose of the following routine is to mark this runner as
   having run the silly  preseason (xc) event added in fall 96.
   */

void Runner::AddSpecial(void)
{
        special = 1;
        no_official++;  // The bloody thing is an official run
                        // We don't need to worry about checking for
                        // cleared status since this goddamn thing is only
                        // run at the beginning of the season

        TotalPoints += (int)(1/resolution);  
        cleared++;
}

void Runner::SetName(char *name)
{
	strcpy(ShortName,name);
}

char *Runner::GetName()
{
	return ShortName;
}

Runner & Runner::operator=( const Runner& ARunner)
{
	strcpy(ShortName,ARunner.ShortName);
	strcpy(LongName,ARunner.LongName);
	strcpy(TeamName,ARunner.TeamName);
	strcpy(Sex,ARunner.Sex);
	Dob->tm_year = ARunner.Dob->tm_year;
	Dob->tm_mon = ARunner.Dob->tm_mon;
	Dob->tm_mday = ARunner.Dob->tm_mday;
	no_runs = ARunner.no_runs;
	no_official = ARunner.no_official;
	no_informal = ARunner.no_informal;
        special = ARunner.special;
	for(int i=0;i<ARunner.no_runs;i++){
		Runs[i]->type = ARunner.Runs[i]->type;
		Runs[i]->distance = ARunner.Runs[i]->distance;
		Runs[i]->time = ARunner.Runs[i]->time;
		Runs[i]->date.tm_year = ARunner.Runs[i]->date.tm_year;
		Runs[i]->date.tm_mon = ARunner.Runs[i]->date.tm_mon;
		Runs[i]->date.tm_mday = ARunner.Runs[i]->date.tm_mday;
		}
		return *this;
} 

int Runner::operator<(const Runner& RHSRunner) const
{
	// runners are ordered by the second char of their short
	// name, and secondarily by the first char.
	if (ShortName[1] > RHSRunner.ShortName[1]) return 0;
	if (ShortName[1] < RHSRunner.ShortName[1]) return 1;
	if (ShortName[0] < RHSRunner.ShortName[0]) return 0;
	return 0;
}

struct tm *Runner::GetDob()
{
	return Dob;
}

//  Return the best time in the given event

double Runner::GetTime(int eventno)
{
	if( BestRuns[eventno] == 0 ) return 0.0;
	return BestRuns[eventno]->time;
}
double Runner::GetPoints(int eventno)
{
	if( BestRuns[eventno] == 0 ) return -11.0;
	return BestRuns[eventno]->pts;
}

int Runner::GetTotalPoints()
{
	return TotalPoints;
}

char *Runner::GetSex() 
{
	return Sex;
}
void Runner::BestRunReport(int eventno, fstream *out) const
{
	char buffer[BUFFERSIZ];

	
	if(BestRuns[eventno] != 0){
	::SecsToStr(BestRuns[eventno]->time,buffer);
	*out << ShortName << " " << buffer << " = " << 
	setprecision(2) << (BestRuns[eventno]->pts)*resolution << endl;
	}
	else *out << ShortName << " ( No Time ) " << endl;
       
}

int Runner::operator==(const Runner& RHSRunner) const
{  // First check if the runners have the same shortname
	if (strcmp(ShortName,RHSRunner.ShortName)!=0)
		return 0;
	// And bail out if their LongNames then differ, e.g.,
	// we have 2 jsmiths (heaven forbid :-)
	assert(strcmp(LongName,RHSRunner.LongName)==0);
	return 1;
}

char *Runner::GetTeamName()
{
	return TeamName;
}

void Runner::PrintAll(fstream *out) const
{
	int i;
	char buffer[BUFFERSIZ];
	char buffer2[BUFFERSIZ];

 
	sprintf(buffer," %d/%d/%d ",Dob->tm_mon+1,
		Dob->tm_mday,Dob->tm_year);
	*out << ShortName << " ( " << LongName << " )" << buffer
		<< " " << AgeGroups[AgeGroupNo(Dob,localtime(&now),Sex)] <<
		" " << TeamName << endl << endl;
	*out << (no_runs + no_relays) <<  " performances to date" <<
	"<" << no_official << " official, " << (((no_runs + no_relays-
	no_official) - no_informal)) << " outside, " <<
	no_informal << " informal>: " << endl;
        if(special) *out << "This runner ran the xc preseason event. Bully for 'em." << endl;
	if(cleared >= 4) *out << "This runner has run on at least 4 official dates." << endl;
        else *out << "This runner has run on " << cleared << " official dates." << endl;
	for(  i = 0;i<no_runs;i++){
	  sprintf(buffer," %d/%d/%d ", Runs[i]->date.tm_mon+1,
		 Runs[i]->date.tm_mday,Runs[i]->date.tm_year);
	  ::SecsToStr(Runs[i]->time,buffer2);
	  if(BestRuns[Runs[i]->distance]==Runs[i])*out << "*";
	  else *out << " ";
	*out << buffer << EventNames[Runs[i]->distance] <<
	  " " << TypeNames[Runs[i]->type] << " " << buffer2
	  <<"="<< ((double)(Runs[i]->points))*resolution << endl;
	  }
	for( i = 0; i<no_relays;i++){
		sprintf(buffer," %d/%d/%d ", Relays[i]->date.tm_mon+1,
		 Relays[i]->date.tm_mday,Relays[i]->date.tm_year);
	  ::SecsToStr(Relays[i]->time,buffer2);
	  if(BestRelays[Relays[i]->LegDistance - 1]==Relays[i])*out << "*";
	  else *out << " ";
	*out << buffer << EventNames[Relays[i]->distance] <<
	  " " << TypeNames[Relays[i]->type] << " " << buffer2 <<
     "=" << ((double)(Relays[i]->points))*resolution 
	  <<"  [ " << Relays[i]->Legs[0] <<" "<< Relays[i]->Legs[1]<< " "
	  << Relays[i]->Legs[2] <<" "<< Relays[i]->Legs[3] << " ]" << endl;
	  }
	  *out << "Total Points: " << ((double)(TotalPoints))*resolution << endl;
	  
}

// END OF MEMBER FUNCTIONS

// Overloaded << operator for this class : This prints out
// A one-line summary of a runner's performance suitable
// For a line in "Scorall"

ostream& operator << (ostream& output, Runner &ARunner)
{   char buffer[BUFFERSIZ];
	int i,j;
 // Left Justify name:
	output.setf(ios::left,ios::adjustfield);
	output.width(NameFieldWidth);
	output << ARunner.GetName();     // Runner's Name
	output.width(5);
	output << ARunner.GetTeamName();     // Team Name
        if(ARunner.special) output << " +";
        else output << "  ";
	
	output.width(1);
	if(ARunner.cleared < 4) output << "[";
	else output << " ";
	sprintf(buffer,"%-4.1f",((double)(ARunner.TotalPoints))*resolution);
	output.width(4);
	// output << ((double)(ARunner.TotalPoints))*resolution;
	output << buffer;
	output.width(1);
        if(ARunner.cleared < 4) output << "]";
	else output << " ";

	output.setf(ios::right,ios::adjustfield);
	for(i = 0;i<NOEVENTS-NORELAYS;i++){       // Display Results
	 if(ExcludedEventsHasMember(i) == 0){

		/* Logic below is hard to read. We want to enter -------
                  to show missing field. We do this when there is no best
                  run for this index event, unless this index is tied to
                  another one: currently, due to 98-99 rule change, this is
                  so for the 15K and 10M events. (also mile, 1500: 2002) 
                  (also 55m, 100m: 2004) 
				  
				  Also: in 2006 we have a new rule that allows any of
				  15k, 10M, hmar, and mar to count for "15k" */

		j = i; // i is index of column head. j is index of event we get result from.

		if(enable_combined_dr && (i==index_15k)){
			if(ARunner.best_15k_is_10M)j=index_10M;
			if(ARunner.best_15k_is_hmar)j=index_hmar;
			if(ARunner.best_15k_is_mar)j=index_mar;
		}
		if(either_10m_or_15k){
			if((i==index_15k)&&ARunner.best_15k_is_10M)j=index_10M;
			if((i==index_10M)&&!ARunner.best_15k_is_10M)j=index_15k;
		}
		if(either_100m_or_55m){
			if((i==index_100m)&&ARunner.best_100m_is_55m)j=index_55m;
			if((i==index_55m)&&!ARunner.best_100m_is_55m)j=index_100m;
		}
		if(either_mile_or_metric){
			if((i==index_mile)&&!ARunner.best_1500m_is_mile)j=index_1500m;
			if((i==index_1500m)&&ARunner.best_1500m_is_mile)j=index_mile;
		}
		if(ARunner.BestRuns[j]==0){
			if((either_10m_or_15k&&((i==index_10M)||(i==index_15k)))||(either_100m_or_55m&&((i==index_100m)||(i==index_55m)))||(either_mile_or_metric&&((i==index_1500m)||(i==index_mile))))
				output.width(EventFieldWidths[i]+6);
			else if(enable_combined_dr&&(i==index_15k))
				output.width(EventFieldWidths[i]+6);
			else
			output.width(EventFieldWidths[i]+5);
			output.fill('-');
			output << " ";
		}
		else {
		  output.width(EventFieldWidths[i]);
		  ::SecsToStr(ARunner.BestRuns[j]->time,buffer);
		  output << buffer;

		  /* Mark time with asterisk if it is run in opposite event
		   * from the one whose name heads this column */
          if(enable_combined_dr)
			  if(i==index_15k && (j != i)){// bug: if the default changes from 15k this'll break
				  output.width(1);
				  output << "*";
			  }
		  if(either_10m_or_15k || either_mile_or_metric || either_100m_or_55m){
			if((i==index_15k)||(i==index_10M)){
				if(((i==index_15k)&&ARunner.best_15k_is_10M)||
				((i==index_10M)&&!ARunner.best_15k_is_10M)){
					output.width(1);
					output << "*";
				}
				else {
					output.width(1);
					output << " ";
				}
			}
			if((i==index_55m)||(i==index_100m)){
				if(((i==index_100m)&&ARunner.best_100m_is_55m)||
				((i==index_55m)&&!ARunner.best_100m_is_55m)){
					output.width(1);
					output << "*";
				}
				else {
					output.width(1);
					output << " ";
				}
			}
			if((i==index_mile)||(i==index_1500m)){
				if(((i==index_mile)&&!ARunner.best_1500m_is_mile)||
				((i==index_1500m)&&ARunner.best_1500m_is_mile)){
					output.width(1);
					output << "*";
				}
				else {
					output.width(1);
					output << " ";
				}
			}
		  }

		  /* Append point value for this performance */

		  output.width(1);
		  output << "=";
		  output.width(3);
		  sprintf(buffer,"%3.1f", ((double)(ARunner.BestRuns[j]->points))*resolution);
		  output << buffer;
		output << " ";
		}
		}
		output.fill(' ');
	}
	for( i = 0;(i<NORELAYS); // +1 offsets us from the 55m which is not
                                   // a relay leg event
		i++){
	      if(ExcludedRelaysHasMember(i+1) == 0){
		if(ARunner.BestRelays[i]==0){
			output.width(RelayFieldWidths[i]+4);
			output.fill('-');
			output << " ";
		}
		else {
		output.width(RelayFieldWidths[i]);
		::SecsToStr(ARunner.BestRelays[i]->time,buffer);
		output << buffer;
		output.width(1);
		output << "=";
		output.width(3);
		  sprintf(buffer,"%3.1f", ((double)(ARunner.BestRelays[i]->points))*resolution);
		output << buffer;
		}
	      }
	      output.fill(' ');
	}
	output << endl;
	return output;
}

// Returns Index into AgeGroups Vector

int AgeGroupNo(struct tm* DOB, const struct tm* rundate, const char *Sex)
{
	int age;  // on the day of run CompetitionYear
        int old_age;  // on may 15: for old scoring method

	if((DOB->tm_mon < rundate->tm_mon)||((DOB->tm_mon==rundate->tm_mon)&&(DOB->tm_mday
		<= rundate->tm_mday)))
                        age = rundate->tm_year - DOB->tm_year;
        else age = rundate->tm_year  - DOB->tm_year - 1;

        if((DOB->tm_mon < 4)||((DOB->tm_mon == 4)&&(DOB->tm_mday <= 15)))
                old_age = CompetitionYear -1900 - DOB->tm_year;
        else old_age = CompetitionYear - 1900 - DOB->tm_year - 1;

        if(use_old_age) age = old_age;
        if(age <= 29 && age >= 20){
                        if (strcmp(Sex,"m")==0)return 5;
                else return 24;
		}
        if(strcmp(Sex,"m")==0 && age > 29)
                return (age-30)/5 + 6;
        if(strcmp(Sex,"f")==0 && age > 29)
                 return  (age-30)/5 + 25;
        if(age <= 19 && age > 11)
                if(strcmp(Sex,"m")==0)
                 return (age-12)/2 + 1;
                else return (age-12)/2 +20;
       if(age <= 11 && age > 9)
               if(strcmp(Sex,"m")==0)
                 return 0;
               else return 19;
	/* if(age <= 9)
		if(strcmp(Sex,"m")==0)
		 return 0; 
		else return 18;*/
	   return 5;  // open male if all else fails 
}

double score(int AgeGroup, int EventNo, double time)
{
	return (20.0*(table[AgeGroup][EventNo]/time) - 10.0)/resolution;
}
int mytrunc(double p)
{
	int temp;
	if( (p < ceil(p)) && (fabs(p - ceil(p)) <= FLT_EPSILON) )
	temp = (int)p + 1;
	else temp = (int)p;
	return temp;
}
