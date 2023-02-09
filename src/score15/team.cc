// Implementation of Team Class



#include "runner.h"
#include "team.h"

extern int mytrunc( double );
const int TeamNameFieldWidth = 20;

// Default Constructor
Team::Team()
{
	teamsize = 0;   // No need to construct anything
	TotalPoints = 0;
}

// Interactive Constructor
/*
Team::Team(char *name)
{
	int ContFlag = 1;
	char buffer[BUFFERSIZ];
	 no_relays = 0;
	teamsize = 0;
	TotalPoints = 0;
	while(ContFlag == 1){
		Main_Window << "Input information on this team." << endl;
		strcpy(ShortName,name);
		Main_Window << endl << "Long Name (e.g. Tenure Tracksters): ";
		gets(buffer);
		strcpy(LongName,buffer);
		Main_Window << endl << "Name of Captain (e.g., Terry R. McConnell) " ;
		gets(buffer);
		strcpy(Captain,buffer);
		Main_Window << "\nIs the following information correct?\n" ;
		PrintAll(&Main_Window);
		Main_Window << endl << "[yn] ";
		cin >> buffer;
		ContFlag = ( buffer[0] == 'y' ? 0 : 1 );
		}
}
*/
 // Constructor called with data
Team::Team(char *name, char* fullname, char *captainsname)
{
	teamsize = 0;
	TotalPoints=0;
	no_relays=0;
	strcpy(ShortName,name);
	strcpy(LongName, fullname);
	strcpy(Captain, captainsname);
}

// Copy Constructor
Team::Team(Team &ATeam)
{
	int i;
	no_relays=0;
	teamsize = ATeam.teamsize;
	TotalPoints = ATeam.TotalPoints;
	no_relays = ATeam.no_relays;
	strcpy(ShortName,ATeam.ShortName);
	strcpy(LongName,ATeam.LongName);
	strcpy(Captain,ATeam.Captain);
	for( i=0; i<teamsize; i++){
		Runners[i] = new Runner;
		*(Runners[i]) = *ATeam.Runners[i];
		}
	for(i =0; i<no_relays;i++){
		Relays[i] = new RelayNode;
		*(Relays[i]) = *ATeam.Relays[i];
		}
}
Team::~Team()
{
;
}
 // Assignment
Team & Team::operator=(Team &ATeam)
{
	int i;
	teamsize = ATeam.teamsize;
	TotalPoints = ATeam.TotalPoints;
	no_relays = ATeam.no_relays;
	strcpy(ShortName,ATeam.ShortName);
	strcpy(LongName,ATeam.LongName);
	strcpy(Captain,ATeam.Captain);

	for(i=0; i<teamsize; i++)
	*(Runners[i]) = *(ATeam.Runners[i]);
	for(i =0; i<no_relays;i++){
		Relays[i] = new RelayNode;
		*(Relays[i]) = *ATeam.Relays[i];
		}
	return *this;
}
 // Assignment
Team & Team::operator=(Team *ATeamPtr)
{
	int i;
	teamsize = ATeamPtr->teamsize;
	TotalPoints = ATeamPtr->TotalPoints;
	no_relays = ATeamPtr->no_relays;
	strcpy(ShortName,ATeamPtr->ShortName);
	strcpy(LongName,ATeamPtr->LongName);
	strcpy(Captain,ATeamPtr->Captain);
		for(i=0; i<teamsize; i++)
	*(Runners[i]) = *(ATeamPtr->Runners[i]);
	for( i =0; i<no_relays;i++){
		Relays[i] = new RelayNode;
		*(Relays[i]) = *(ATeamPtr->Relays[i]);
		}
   return *this;
}
// Return total points this team has scored
int Team::GetPoints()
{
	SetPoints();    /* Make sure point totals are current */
	return TotalPoints;
}

// Print a report on this team to output stream
void Team::PrintAll(fstream *out) const
{
	char buffer[BUFFERSIZ];
	int i;
	
	*out << ShortName << " ( " << LongName << " ) " <<
		"Captain: " << Captain << endl;
	*out << "Total Points: " << ((double)TotalPoints)*resolution << endl;
	*out << "Team Size: " << teamsize << endl;

	// Compute Points on Reserve
	int no_reserve = 0;
	int res_pts = 0;
	for (i = 0;i<teamsize; i++)
		if(Runners[i]->cleared < 4){
			no_reserve++;
			res_pts += Runners[i]->TotalPoints;
		}
	*out << "Runners on Reserve: " << no_reserve << endl;
	*out << "Points on Reserve: " << ((double)res_pts*resolution) << endl;
  // for each event, compute various stuff
  *out << "Event    " << "Ptcpnts   " << "Runs      " << "Points     " <<
  "<Potential>"<< endl;
  *out << "***********************************************" << endl;
  out->setf(ios::left,ios::adjustfield); 
  out->width(10);               
  out->fill(' ');
  for(i=0;i<NOEVENTS-NORELAYS;i++){
   if(ExcludedEventsHasMember(i)==0){
	int evnt_pts = 0;
	int evnt_potl = 0;
	int evnt_runs = 0;
	int evnt_ptcpnts = 0;
	for(int j=0;j<teamsize;j++){
		if(Runners[j]->BestRuns[i] != NULL){
		    if(Runners[j]->cleared > 3)
		     evnt_pts += Runners[j]->BestRuns[i]->points;   
		     evnt_potl += Runners[j]->BestRuns[i]->points;
			++evnt_ptcpnts;
			for(int k=0;k<Runners[j]->no_runs;k++)
				if(Runners[j]->Runs[k]->distance == i)
					++evnt_runs;
		}
	}
	*out << setw(10) << EventNames[i] << setw(10) << 
	evnt_ptcpnts << setw(10) << evnt_runs << setw(10) << ((double)evnt_pts*resolution); 
	sprintf(buffer,"<%.1f>",((double)evnt_potl*resolution));
	*out << setw(10) << buffer << endl;
	 }
	}
  for(i=0 ;i<NORELAYS;i++){
   if(ExcludedRelaysHasMember(i)==0){
	 int evnt_pts = 0;
	 int evnt_potl = 0;
	 int evnt_runs = 0;
	 int evnt_ptcpnts = 0;
	for(int j=0;j<teamsize;j++){
		if(Runners[j]->BestRelays[i] != NULL ){
		    if(Runners[j]->cleared > 3)
		     evnt_pts += Runners[j]->BestRelays[i]->points;   
		     evnt_potl += Runners[j]->BestRelays[i]->points;
			++evnt_ptcpnts;
			for(int k=0;k<Runners[j]->no_relays;k++)
                                if(Runners[j]->Relays[k]->distance == i+NOEVENTS-NORELAYS)
					++evnt_runs;
		}
	}
	*out << setw(10) << EventNames[i+NOEVENTS-NORELAYS] << setw(10) << 
	evnt_ptcpnts << setw(10) << evnt_runs << setw(10) << ((double)evnt_pts)*resolution; 
	sprintf(buffer,"<%.1f>",((double)evnt_potl)*resolution);
	*out << setw(10) << buffer << endl;
	 }
	}
       

}

char *Team::GetShortName()
{
	char *ptr;
	ptr = ShortName;
	return ptr;
}
// Add a runner. Return 1 if maximum size of team is exceeded.
int Team::AddRunner(Runner *ARunner)
{
	Runners[teamsize] =  ARunner;
	teamsize++;
	assert(teamsize < 220); // not really needed. For paranoia only
	return ( teamsize < MAX_RUNNERS ? 0 : 1);
}
void Team::SetPoints()
{
	TotalPoints = 0; // to keep it from growing!
	for(int i =0;i<teamsize; i++){
		if(Runners[i]->cleared >= 4)      // 4 different dates to count
		  TotalPoints += Runners[i]->TotalPoints;
	}
}

RelayNode* Team::AddRelay(RelayNode *ARelay)
{
	int i = no_relays++;
	Relays[i] = ARelay;
		Relays[i]->points =
		ScoreRelay(
		AgeGroupNo(Relays[i]->Dobs[0],&(ARelay->date),Relays[i]->Sexes[0]),
		AgeGroupNo(Relays[i]->Dobs[1],&(ARelay->date),Relays[i]->Sexes[1]),
		AgeGroupNo(Relays[i]->Dobs[2],&(ARelay->date),Relays[i]->Sexes[2]),
		AgeGroupNo(Relays[i]->Dobs[3],&(ARelay->date),Relays[i]->Sexes[3]),
		Relays[i]->LegDistance,
		Relays[i]->time,Relays[i]);
	assert(no_relays < 200);
	return Relays[i];
}

// END OF MEMBER FUNCTION IMPLEMENTATIONS

// Overloaded << for this class
// One line summary suitable for inclusion in Scorall

ostream& operator<<(ostream &output,Team & ATeam)
{
	char buffer[BUFFERSIZ];
	output.setf(ios::left,ios::adjustfield);
	output.width(TeamNameFieldWidth);
	output << ATeam.LongName;
	output.setf(ios::right,ios::adjustfield);
	output.width(10);
	// Compute Points on Reserve
	int no_reserve = 0;
	int res_pts = 0;
	for (int i = 0;i<ATeam.teamsize; i++)
		if(ATeam.Runners[i]->cleared < 4){
			no_reserve++;
			res_pts += ATeam.Runners[i]->TotalPoints;
		}
      output.fill(' ');
	output <<  ATeam.teamsize << setw(10) <<
	no_reserve; 
	sprintf(buffer,"%10.1f%10.1f",
		((double)res_pts)*resolution,
		((double)ATeam.TotalPoints)*resolution);
	output << setw(20) << buffer;
	sprintf(buffer,"<%.1f>",((double)(ATeam.TotalPoints)+(double)res_pts)*resolution);
	/* Put Max(i.e. potential) points in brackets */
	output << setw(10)
	<<  buffer << endl << endl;
	return output;
}

int ScoreFixedDMR(int AG1,int AG2,int AG3, int AG4, double time)
{
//         Score a DMR when runners are in given order
	int temp; /* second is number of leg type, 0 = 100m,1=200m,2=400m,
	3=800m,4=1500m */
	int indx1,indx2,indx3 ; //index number of individual leg distances
	int i;
	for(i=0;(i<NOEVENTS)&&(strcmp(EventNames[i],"800m")!=0);i++);
		indx1 = i;
	for(i=0;(i<NOEVENTS)&&(strcmp(EventNames[i],"400m")!=0);i++);
		indx2 = i;
	for(i=0;(i<NOEVENTS)&&(strcmp(EventNames[i],"1600m")!=0);i++);
		indx3 = i;

	/* std for 1200m = (1/2)(std_for_800m + std_for_1600m)
	   
	*/

	temp = mytrunc((20.0*((table[AG1][indx1]+table[AG2][indx2]+
		(table[AG3][indx1]+table[AG3][indx3])/2+table[AG4][indx3])/time)-10.0)/resolution);
	return ( (double)temp < 1/resolution ? (int)(1/resolution) : temp);
}

int ScoreRelay(int AG1,int AG2,int AG3, int AG4, int EventNo, double time,
	struct RelayNode *ARelay)
{
	int temp,old;
	void * ptr1, *ptr2, *ptr3, *ptr4;
	int worst = 10;
	int oldi,oldj,oldk,oldl;
	int AGs[4];
	AGs[0]=AG1;
	AGs[1]=AG2;
	AGs[2]=AG3;
	AGs[3]=AG4;
	if(EventNo == 5) { /* DMR */
	for(int i=0;i<4;i++){
	for(int j=0;(j<4);j++){
	if(j==i)continue;
	for(int k=0;(k<4);k++){
	if((k==j)||(k==i))continue;
	for(int l=0;(l<4);l++){
	if((l==k)||(l==j)||(l==i))continue;
	temp = ScoreFixedDMR(AGs[i],AGs[j],AGs[k],AGs[l],time);
	old = worst;
	if ( old < temp ) {
		worst = old;
	}
	else {
		worst = temp;
		oldi = i;
		oldj = j;                // for sorting legs into
		oldk = k;               // presumed order.
		oldl = l;
	}
	} } } }
	ptr1 = ARelay->Legs[oldi];            // sort legs in presumed order
	ptr2 = ARelay->Legs[oldj];
	ptr3 = ARelay->Legs[oldk];
	ptr4 = ARelay->Legs[oldl];
	ARelay->Legs[0] = (char *)ptr1;
	ARelay->Legs[1] = (char *)ptr2;
	ARelay->Legs[2] = (char *)ptr3;
	ARelay->Legs[3] = (char *)ptr4;
	ptr1 = ARelay->Sexes[oldi];
	ptr2 = ARelay->Sexes[oldj];
	ptr3 = ARelay->Sexes[oldk];
	ptr4 = ARelay->Sexes[oldl];
	ARelay->Sexes[0] = (char *) ptr1;
	ARelay->Sexes[1] = (char *)ptr2;
	ARelay->Sexes[2] = (char *)ptr3;
	ARelay->Sexes[3] = (char *)ptr4;
	ptr1 = ARelay->Dobs[oldi];
	ptr2 = ARelay->Dobs[oldj];
	ptr3 = ARelay->Dobs[oldk];
	ptr4 = ARelay->Dobs[oldl];
	ARelay->Dobs[0] = (struct tm *)ptr1;
	ARelay->Dobs[1] = (struct tm *)ptr2;
	ARelay->Dobs[2] = (struct tm *)ptr3;
	ARelay->Dobs[3] = (struct tm *)ptr4;

	return worst;
	}   /* else */
	temp = mytrunc((20.0*((table[AG1][EventNo]+table[AG2][EventNo]+
		table[AG3][EventNo]+table[AG4][EventNo])/time)-10.0)/resolution);
	return ((double)temp < 1/resolution ? (int)(1/resolution) : temp);
}
int ScoreRelay(int AG1,int AG2,int AG3, int AG4, int EventNo, double time)
{
	int temp,old;
	int worst = 10;
	int AGs[4];
	AGs[0]=AG1;
	AGs[1]=AG2;
	AGs[2]=AG3;
	AGs[3]=AG4;
	if(EventNo == 5) { /* DMR */
	for(int i=0;i<4;i++){
	for(int j=0;(j<4);j++){
	if(j==i)continue;
	for(int k=0;(k<4);k++){
	if((k==j)||(k==i))continue;
	for(int l=0;(l<4);l++){
	if((l==k)||(l==j)||(l==i))continue;
	temp = ScoreFixedDMR(AGs[i],AGs[j],AGs[k],AGs[l],time);
	old = worst;
	if ( old < temp ) {
		worst = old;
	}
	else {
		worst = temp;
	}
	} } } }
	return worst;
	}   /* else */
	temp = mytrunc((20.0*((table[AG1][EventNo]+table[AG2][EventNo]+
		table[AG3][EventNo]+table[AG4][EventNo])/time)-10.0)/resolution);
	return (double)temp < 1/resolution ? (int)(1/resolution) : temp;
}
