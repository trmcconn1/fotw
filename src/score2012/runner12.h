// runner.h: header for runner class


#define __CPLUSPLUS__

#ifndef RUNNER
#define RUNNER
#ifdef UNDER_CE
#include "global.h"
#else
#include "global12.h"
#endif
using namespace std;

class Team;

extern FILE *errstream;

 struct RunNode {
  int type;
  int distance;
  double time;
  struct tm date;
  int points;
  double pts;         /* a wasteful patch */
  };

typedef struct RunNode RunNode;

  struct RelayNode {
	int type;
	int distance;
   int LegDistance;
	double time;
	struct tm date;
  int points;
	char *Legs[4];
	struct tm *Dobs[4];
	char *Sexes[4];   
	};

class Runner {
	friend class Team;
	friend ostream& operator<<(ostream&,Runner &);
	friend ostream& operator<<(ostream&,Team &);
	friend int SecsToStr(double, char *);
	friend int AgeGroupNo(struct tm* DOB, const struct tm* rundate,const char *Sex);
	friend double score(int,int,double);
public:
	int compliant;  // flag for new rule 2012
	struct RunNode *BestRuns[26];
	Runner();
	Runner(int); // interactive constructor
	Runner(Runner&);
   Runner(char *,char *,char*,int,int,int,char *);
	~Runner();
	void AddRun(const struct RunNode *);
        void AddSpecial(void);  // Mark runner as having done the special
                               // preseason event introduced fall 1996
	char *GetName();
	char *GetTeamName();
	double GetTime(int);
	double GetPoints(int);
	int GetTotalPoints();
	int GetRelayPoints(int);
	int GetRelayNumber();
	int GetNumberOfRuns();
	struct RunNode **GetRunsPointer();
	struct RelayNode **GetRelaysPointer();
	void SetName(char *);

	Runner& operator=( const Runner&);
	void PrintAll(fstream *) const;
	void BestRunReport(int,fstream *) const;
   void AddRelay(struct RelayNode*);
	int operator<(const Runner &)const;
	int operator==(const Runner &)const;
	struct tm *GetDob();
   char *GetSex();

  char ShortName[80];
  char LongName[80];
  char TeamName[5];
  char Sex[2];
  struct tm *Dob;  // Date of Birth
  int no_runs;
  int cleared;
  int no_relays;
  int no_official;
  int best_15k_is_10M;  // flag for new rule 98-99
  int best_1500m_is_mile;  // flag for new rule 98-99
  int best_100m_is_55m;    // flag for new rule 2003-4
  int best_15k_is_mar;     // flags for new rule for 2006
  int best_15k_is_hmar;
  int best_8k_is_10k;
  int best_10k_is_8k;
  int best_3200r_is_manley;  // flags for new 2011-12 rule
  int best_1600r_is_manley;
  int best_800r_is_manley;
  int no_informal;
  int special;             // flag to say whether this runner has done
                          //  the special pre-season event added in 1996
  int TotalPoints;
  struct RelayNode *Relays[MAX_RELAYS];
  struct RunNode *Runs[MAX_RUNS];
						// array of pointers to runnodes
  
  struct RelayNode *BestRelays[8];
  };
#endif
