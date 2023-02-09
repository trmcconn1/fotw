/* global.h: common includes for all of my score program */

#ifndef GLOBAL
#define GLOBAL
#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <fstream>


#define NO_TEAMS 6  // Number of teams to allocate
#define MAX_ROSTER 220     /* Total number of runners to allocate*/
#define MAX_RUNS 60  //For a given runner.
#define MAX_RELAYS 50  // For array declarations
#define BUFFERSIZ 256
#define SHORT_TEAM_NAME_LENGTH 5
#define SHORTNAMELEN 20
#define LONG_NAME_LENGTH 80
#define DEFAULT_RESOLUTION 1.0  /* resolution of the point scale */
#define ERRLOGPATH "My Documents\\Noontime League\\errlog.txt"
#define RAWRESULTSPATH "My Documents\\Noontime League\\raw.txt"
#define NOEVENTS 27   /* As of 2012 */
#define NORELAYS 8

#ifdef __MSDOS__
// #include <constrea.h> 
#include <dos.h>
#include <direct.h>
#else
#include <unistd.h>
#endif

extern int index_55m;
extern int index_200m;
extern int index_400m;
extern int index_800m;
extern  int index_100m; // these events are treated differntly
extern  int index_15k; // starting in 1995-6
extern  int index_10M; // starting in 1995-6
extern int index_1500m;
extern int index_mile;
extern int index_hmar;
extern int index_mar;
extern int index_8k;
extern int index_5M;
extern int index_10k;
extern int index_3xmanley;
extern int index_2xmanley;
extern int index_1xmanley;
extern int index_dmr;
extern const char *EventNames[];    // defined in score13.cpp
extern const char *TypeNames[];
extern const int EventFieldWidths[]; // For Print out.
extern const int RelayFieldWidths[];
extern const int NameFieldWidth;
extern const int TeamNameFieldWidth;
extern int AgeGroupNo(struct tm*, const struct tm*, const char *);

extern int EventNo(char *);

struct EXCLUDED {
int events[NOEVENTS];
int number;
};
extern struct EXCLUDED Excluded;
struct EXCLUDEDRELAYS {
int events[NORELAYS];
int number;
};
extern struct EXCLUDEDRELAYS ExcludedRelays;
extern int ExcludedEventsHasMember(int);
extern int ExcludedRelaysHasMember(int);
extern "C" void InitTable(void);    /* function to initialize table */
extern "C" void InitLegacyTable(void); 
extern "C" int SecsToStr(double, char *);

/* The  dimension below must = NOEVENTS -NORELAYS */
extern "C" double table[][19];   /* table of standards */
extern int CompetitionYear;
extern int ReserveLimit;
extern double resolution;
extern int use_old_age;
extern int two_pt_old_age_min;
extern int either_100m_or_55m;
extern int exclusive_extremes;
extern int enforce_official_minimum;
extern int legacy_table;
extern int either_10m_or_15k;
extern int either_55m_or_15k;
extern int either_8k_or_10k;
extern int either_mile_or_metric;
extern int enable_combined_dr;
extern int manleyor3200r;
extern int manleyor1600r;
extern int manleyor800r;
extern  const char *AgeGroups[];
enum Type {outside,official,informal};
enum SortStyle {BYLASTNAME,BYTOTALPOINTS,BYEVENTPOINTS,BYTIMES};
#endif
