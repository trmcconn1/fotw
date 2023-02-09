// team.h: header for team class


#ifndef TEAM
#define TEAM

#define __CPLUSPLUS__
#define MAX_TEAM_RELAYS 200
#define MAX_RUNNERS 220


#ifdef UNDER_CE
#include "global.h"
#else
#include "global12.h"
#endif
using namespace std;

class Runner;

class Team {
friend ostream& operator<<(ostream&,Team &);
friend class Runner;
friend int ScoreRelay(int,int,int,int,int,double,struct RelayNode *);
friend int AgeGroupNo(struct tm* DOB, const struct tm *rundate,const char *Sex);
public:
	Team();
	Team(char *); // interactive constructor
   Team(char *,char *, char *);
	Team(Team&);
	~Team();   // No need to destruct
   struct RelayNode* AddRelay(RelayNode *);
	void PrintAll(fstream *) const ; // Print General Info
	void SetPoints(); // total up points of everybody on team
   int GetPoints(); // Returns the number of points team scored
   int GetReservePoints();
	char *GetShortName();
	char *GetLongName();
   int AddRunner(Runner *);
	Team &operator=(Team &);
   Team &operator=(Team *);
private:
	char ShortName[SHORT_TEAM_NAME_LENGTH];
	char LongName[LONG_NAME_LENGTH];
	char Captain[LONG_NAME_LENGTH];
	int teamsize;
	int no_relays;
	int ReservePoints;
	int TotalPoints;
	Runner *Runners[MAX_RUNNERS];
	struct RelayNode* Relays[MAX_TEAM_RELAYS];
	};
#endif
