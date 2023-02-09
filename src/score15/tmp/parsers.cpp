/* Parsers.c:  */

#define __CPLUSPLUS__

#include "global.h"        
#include <string.h>
#include "team.h"
#include "runner.h"

#define DATE "date"
#define DISTANCE "distance"
#define LINESIZ 80
#define DELIMITER "     \n"
#define COMMENT '*'
#define FIELDSEPS " /-  " /* delimiters in a valid date */


extern int _filter;
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
struct PERSON_INFO{
        char ShortName[20];
        int month;
        int day;     /* DOB */
        int year;
        char sex[2];
        char teamname[SHORT_TEAM_NAME_LENGTH];
        };

extern const char *EventNames[];
enum State {ROOT,DATED,DISTANCED,PERFORMANCED,RELAYED};
void ParseDate( char *DateString , struct tm *Date);
int tokenize(char *line,char **tokens); 
extern FILE *errstream;

int
ParseResults(FILE *results, char *Tokens[5], struct RUNINFO *rinfo,int *line)
{
        char buffer[BUFFERSIZ];
	char NoNLbuffer[BUFFERSIZ];
        static int count;
        static int state = ROOT;
        int i;
        char EventName[8];

        while(fgets(buffer,LINESIZ,results) != NULL) {
		if(_filter){
			strcpy(NoNLbuffer,buffer);
			NoNLbuffer[strlen(NoNLbuffer)-1] = '\0';
			printf("%s",NoNLbuffer);
		}
/* Skip comments: */
                         if((buffer[0] != COMMENT)&&
                                (buffer[0] != '\n')) {
/* And empty lines */
            if((count = tokenize(buffer,Tokens)) != 0) {

                switch(state) {
                        case ROOT:
                                if(strcmp(Tokens[0],DATE) != 0) {
                                                fprintf(errstream,"Syntax error: %s %s %s\n",
                                DATE," expected on line ",buffer);
                                return -1;
                          }
                          if(count < 2){
                            fprintf(errstream,"Syntax error: %s %s\n",
                                "Date string expected on line "
                                        ,buffer);
                                 return -1;
                          }
                          ParseDate(Tokens[1],&(rinfo->Date));

                          if(count < 3){
                            fprintf(errstream,"Syntax error: %s %s Official type assumed.\n",
                                "Type string expected on line "
                                        ,buffer);
				rinfo->type = official;
                          }
			  else {
                          if(strcmp(Tokens[2],"informal")==0)
                                rinfo->type = informal;
                           else if(strcmp(Tokens[2],"outside")==0)
                                rinfo->type = outside;
                                else if(strcmp(Tokens[2],"regular")==0)
                                  rinfo->type = official;
                                  else {
                                  fprintf(errstream,"Unknown type %s. Official assumed.\n",Tokens[2]
                                        );
				  rinfo->type = official;
				  }
                          }
                          state = DATED;

                          break;

                        case DATED:
        /* What to do if we get another dateline */
        if(strcmp(Tokens[0],DATE) == 0) {
                            if(count < 1){
                              fprintf(errstream,"Syntax error: %s %s\n",
                                "Date string expected on line "
                                        ,buffer);
                                        return -1;
                            }
                            if(count < 2){
                              fprintf(errstream,"Syntax error: %s %s\n",
                                        "Type string expected on line "
                                        ,buffer);
                                        return -1;
                            }
                            ParseDate(Tokens[1],&(rinfo->Date));
                            if(strcmp(Tokens[2],"informal")==0)
                                rinfo->type = informal;
                             else if(strcmp(Tokens[2],"outside")==0)
                                rinfo->type = outside;
                             else if(strcmp(Tokens[2],"regular")==0)
                                rinfo->type = official;
                             else 
                                                fprintf(errstream,"Unknown Run type %s\n",
                                        Tokens[2]);
                           state = DATED;

                            break;
                         }
                        else {  /* Not another DateLine */

                          if(strcmp(Tokens[0],DISTANCE) != 0) {
                            fprintf(errstream,"Syntax error: %s %s %s\n",
                                DISTANCE," expected on line ",buffer);
                                return -1;
                          }
                            if(count < 1){
                                        fprintf(errstream,"Syntax error: %s %s\n",
                                        "Event name expected on line "
                                        ,buffer);
                                        return -1;
                            }
                        strcpy(EventName,Tokens[1]);
                        rinfo->RelayLegEventNumber = 0;

             /* Handle the crufty special event handled in Fall 1996 */
                                if(strcmp("XC",EventName)==0){
                                        rinfo->EventNumber = -1;
                                        state = DISTANCED;
                                        break;
                                 }     
                                 if(strcmp("400r",EventName)==0)rinfo->RelayLegEventNumber=1;
                                 if(strcmp("800r",EventName)==0)rinfo->RelayLegEventNumber=2;
                                 if(strcmp("1600r",EventName)==0)rinfo->RelayLegEventNumber=3;
                                 if(strcmp("3200r",EventName)==0)rinfo->RelayLegEventNumber=4;
                                 if(strcmp("dmr",EventName)==0)rinfo->RelayLegEventNumber=5;
                        for(i=0;(i<NOEVENTS)&&(strcmp(EventNames[i],EventName)!=0)
                                        ;i++);
                                  if(i>=NOEVENTS){
                                        fprintf(errstream,"Unknown eventname: %s\n",EventName);
                                        fprintf(errstream,"Results, line %d\n",*line);
                                        return -1;
                                        }
                                        rinfo->EventNumber = i;
                                        state = DISTANCED;
                                                                        break;
                           }
         case DISTANCED:
         case PERFORMANCED:
         case RELAYED:
/* What to do if we get a dateline */
                          if(strcmp(Tokens[0],DATE) == 0) {
                            if(count < 1){
                              fprintf(errstream,"Syntax error: %s %s\n",
                                "Date string expected on line "
                                        ,buffer);
                                        return -1;
                            }
                            if(count < 2){
                              fprintf(errstream,"Syntax error: %s %s\n",
                                        "Type string expected on line "
                                        ,buffer);
                                        return -1;
                            }
                            ParseDate(Tokens[1],&(rinfo->Date));
                            if(strcmp(Tokens[2],"informal")==0)
                                rinfo->type = informal;
                             else if(strcmp(Tokens[2],"outside")==0)
                                rinfo->type = outside;
                             else if(strcmp(Tokens[2],"regular")==0)
                                rinfo->type = official;
                             else 
                                                fprintf(errstream,"Unknown Run type %s\n",
                                        Tokens[2]);
                                          state = DATED;
                            break;
                         }
                           else {  /* Another Distance Line */

                             if(strcmp(Tokens[0],DISTANCE) == 0) {
                               if(count < 1){
                                 fprintf(errstream,"Syntax error: %s %s\n",
                                        "Event name expected on line "
                                        ,buffer);
                                        return -1;
                              }
                                  strcpy(EventName,Tokens[1]);
                                 rinfo->RelayLegEventNumber = 0;
                                 if(strcmp("400r",EventName)==0)rinfo->RelayLegEventNumber=1;
                                 if(strcmp("800r",EventName)==0)rinfo->RelayLegEventNumber=2;
                                 if(strcmp("1600r",EventName)==0)rinfo->RelayLegEventNumber=3;
                                 if(strcmp("3200r",EventName)==0)rinfo->RelayLegEventNumber=4;
                                 if(strcmp("dmr",EventName)==0)rinfo->RelayLegEventNumber=5;
                                 for(i=0;(i<NOEVENTS)&&(strcmp(EventNames[i],EventName)!=0)
                                        ;i++);
                                  if(i>=NOEVENTS){
                                        fprintf(errstream,"Unknown eventname: %s\n",EventName);
                                        fprintf(errstream,"Results, line %d\n",*line);
                                        return -1;
                                        }
                                                 rinfo->EventNumber = i;
                                                 state = DISTANCED;
                                                 break;
                            }
                                 /* Now we should have performance lines */

              /* First lets handle the doofy special preseason event
                 added in fall 1996 */
                                if(count == 1) {
                                strncpy(rinfo->ShortName,Tokens[0],SHORTNAMELEN);
                                   state = PERFORMANCED;
				   *(rinfo->ShortName+SHORTNAMELEN-1)='\0';
                                   break;
                                }
                                if(count ==2) {
                           strncpy(rinfo->ShortName,Tokens[0],SHORTNAMELEN);
                                strcpy(rinfo->TimeString,Tokens[1]);
                                state = PERFORMANCED;
                                }
                                else if (count == 5) {         /* Relay */
                                        strcpy(rinfo->Leg1,Tokens[0]);
                                        strcpy(rinfo->Leg2,Tokens[1]);
                                        strcpy(rinfo->Leg3,Tokens[2]);
                                        strcpy(rinfo->Leg4,Tokens[3]);
                                        strcpy(rinfo->TimeString,Tokens[4]);
                                        state = RELAYED;
                                        }
                                else {
                                fprintf(errstream,"Syntax error: %s %s\n",
                                                                                "Name and timestring expected on line ",
                                                                                 buffer);
                                return -1;
                                }
				*(rinfo->ShortName+SHORTNAMELEN-1)='\0';
                                   break;
                           }

                        default:   /* Should never get here */
                          fprintf(errstream,"Unknown State\n");
                          return -1;
                }  /* End Switch */
                (*line)++;
                return state;
                  }     /* End empty line catch */
                          }       /* End comment line catch */
		  else
			if(_filter)printf("\n");
        }          /* End get-input-line */
        return -1;   /* return 1 at EOF */
}                  /* End main */

/* Parse a date string, eg 4/30/54 -> struct tm  */
/* e.g., 4-30-54, 4/30/1954, and "4 30 54" are all ok */

void ParseDate( char *DateString , struct tm *Date)
{
        char buf[15];
        char numbuf[5];
        char *ptr;
        int temp;


        strcpy(buf,DateString);
        
        /* Get Month */
        if((ptr = strtok(buf,FIELDSEPS))==NULL){
                fprintf(errstream,"Syntax error in %s\n",DateString);
                return;
        } 
        strcpy(numbuf,ptr);
        if((temp = atoi(numbuf)) > 12 || temp < 0 )
                fprintf(errstream,"Month out of range in %s\n",DateString);
        Date->tm_mon = temp-1;

        /* Get Day */   
        if((ptr=strtok(NULL,FIELDSEPS))==NULL){
                fprintf(errstream,"Syntax error in %s\n",DateString);
                return;
        } 
        strcpy(numbuf,ptr);
        if((temp = atoi(numbuf)) > 31 || temp < 1 )
                fprintf(errstream,"Day out of range in %s\n",DateString);
        Date->tm_mday = temp;

        /* Get Year */
        if((ptr=strtok(NULL,FIELDSEPS))==NULL){
                fprintf(errstream,"Syntax error %s\n",DateString);
                return;
        } 
        strcpy(numbuf,ptr);
        if((temp = atoi(numbuf))  < 1901 && temp > 99)
                fprintf(errstream,"Year out of range in %s\n",DateString);

/* Y2K fix: if temp <= 99 it's a two digit date. If it's less than 71, we'll
   assume it's a +2000 date. That will get us through the year 2070, which
   is good enough for me.
*/
   
	if(temp <= 99) 
		if(temp <= 70)temp += 2000;
        Date->tm_year = (temp > 99 ? temp - 1900 : temp);
        
        return; 
}

/* Break Line into tokens delimited by DELIMITER and return number of
      tokens found */

int tokenize(char *line,char **tokens)
{
        int count = 0;
        char *ptr;
        char buffer[BUFFERSIZ];

        strcpy(buffer,line);

        if((ptr=strtok(buffer,DELIMITER)) == NULL) return count; 
        strcpy(*tokens++,ptr);
        ++count;
        while((ptr = strtok(NULL,DELIMITER))!=NULL){
                ++count;
                strcpy(*tokens++,ptr);
        }
        return count;
}
        
// Parses the file named as TeamsFile in score.ini 

int
ParseTeamsFile( FILE *teamnames,Team *teams[], int *line, int *number_teams )
{
        char teamname[SHORT_TEAM_NAME_LENGTH];
        char fullteamname[LONG_NAME_LENGTH];
        char captainsname[LONG_NAME_LENGTH];
        int scanned; // Number of fields in input line
        char LineBuffer[256];

         while(fgets(LineBuffer,256,teamnames) != NULL){
          if((LineBuffer[0] != COMMENT)&&(
                LineBuffer[0] != '\n')){ // skip comments and blank lines 
         scanned = sscanf(LineBuffer,
         "%s \" %[^\"] \" \" %[^\"]\"",teamname,fullteamname,
         captainsname);
                if(scanned != 3) {
                        fprintf(errstream,"Syntax error, teams line %d\n",*line);
                        return 1;
                        }
                teams[(*line)-1] = new Team(teamname,fullteamname,captainsname);
                (*line)++;
                (*number_teams)++;
                }
            }
            return 0;

}

int
ParseRosterFile(FILE *roster, Team *teams[], Runner *AllRunners[],
        struct PERSON_INFO *pinfo, int *line, int number_teams, int *RunnerCount)
{
        int i;
        int found = 0;
        char LineBuffer[256];
	char XtraBuffer[256];
        int scanned;

	(*line)--;
	LineBuffer[256] = '\0';
        while(fgets(LineBuffer,256,roster)!= NULL){
          (*line)++;
	  if(LineBuffer[256]){
		fprintf(errstream,"Roster line %d is too long. Skipping it.\n",
			*line);
		LineBuffer[256] = '\0';
		continue;
	  }
          if((LineBuffer[0] != COMMENT)&&(
                LineBuffer[0] != '\n')){ // skip comments and blank lines 
        scanned=sscanf(LineBuffer,"%s %d %d %d %s %s %s",pinfo->ShortName,
                &(pinfo->month),&(pinfo->day),&(pinfo->year),pinfo->sex,
			pinfo->teamname,XtraBuffer);
                if( scanned < 6) {
                        fprintf(errstream,"Syntax error, roster line %d\n",*line);
			fprintf(errstream,"Bad line is:\n");
			fprintf(errstream,"%s\n",LineBuffer);
                       	continue; 
                        }
		if( scanned > 6) {
			fprintf(errstream,"Extra stuff on roster line %d ignored\n",
				*line);
			fprintf(errstream,"Extra stuff is:\n");
			fprintf(errstream,"%s\n",XtraBuffer);
		}
/* See whether this team already exists */
        for( i=0;i<number_teams;i++)
                if(strcmp(teams[i]->GetShortName(),pinfo->teamname) == 0){
                        found=1;
                        break;
                }
// See if team is found
         if(found == 0){ 
                fprintf(errstream,"No such team: %s Line %d\n",pinfo->teamname,*line);
                }
         else {
         if(*RunnerCount < MAX_ROSTER-1){
/* And add runner to it */
         AllRunners[*RunnerCount]=new
                Runner(pinfo->ShortName,pinfo->teamname,pinfo->sex,
                pinfo->month,pinfo->day,pinfo->year,
                        "No Long Name Available");
        if( teams[i]->AddRunner(AllRunners[(*RunnerCount)++])==1){;
                fprintf(errstream,"Reached Maximum Team Size: %d\n",MAX_RUNNERS);
                fprintf(errstream,"Reduce roster, or recompile with larger limit.\n");
                return 1;
         }
         found = 0;
         }
         else {
                fprintf(errstream,"Reached Maximum Roster Size: %d\n",MAX_ROSTER);
                fprintf(errstream,"Reduce roster, or recompile with larger limit.\n");
                return 1;
               }
         }  // end else 
         }  // end if-- skipping comments if
        }   // End while
        return 0;
}  // end routine

// Parse the file score.ini in the current directory 

int
ParseRcFile(FILE *rcfile, struct EXCLUDEDRELAYS *ExcludedRelays,
        struct EXCLUDED *Excluded, int *CompetitionYear,
        char *RosterFile, char *ResultsFile, 
        char *TeamsFile, char *PrinterInitString, 
        char *PrinterResetString, char *TempFile, char *Scorall, 
        char *WorkingDirectory, char *PrinterNormalString)
{
        char LineBuffer[256];
        char buffer[80], buffer2[80];
        int i;
          while(fgets(LineBuffer,256,rcfile)!=NULL){
          if((LineBuffer[0] != COMMENT)&&(
                LineBuffer[0] != '\n')){ // skip comments and blank lines 
          sscanf(LineBuffer,"%s %s",buffer,buffer2);
                if(strcmp(buffer,"CompetitionYear")==0){
                        *CompetitionYear = atoi(buffer2);
                        continue;
                        }
                if(strcmp(buffer,"TeamsFile")==0){
                        strcpy(TeamsFile,buffer2);
                        continue;
                        }
                if(strcmp(buffer,"PointResolution")==0){
                        resolution = atof(buffer2);
                        continue;
                        }
                if(strcmp(buffer,"UseOldAge")==0){
                        use_old_age=1;
                        continue;
                        }
                if(strcmp(buffer,"ExclusiveExtremes")==0){
                        exclusive_extremes=1;
                        continue;
                        }
                if(strcmp(buffer,"Use10MFor15K")==0){
					    if(enable_combined_dr == 0)
                          either_10m_or_15k=1;
                        continue;
                        }
				if(strcmp(buffer,"EnableCombinedDR")==0){
                        enable_combined_dr=1;
						either_10m_or_15k = 0;
                        continue;
				}
		if(strcmp(buffer,"Use55MFor100M")==0){
			either_100m_or_55m=1;
			continue;
		}
                if(strcmp(buffer,"UseMetricForMile")==0){
                        either_mile_or_metric=1;
                        continue;
                        }

                if(strcmp(buffer,"Scorall")==0){
                        strcpy(Scorall,buffer2);
                        continue;
                }
                if(strcmp(buffer,"WorkingDirectory")==0){
                        strcpy(WorkingDirectory,buffer2);
                        continue;
                }

                if(strcmp(buffer,"RosterFile")==0){
                        strcpy(RosterFile,buffer2);
                        continue;
                        }
                if(strcmp(buffer,"ResultsFile")==0){
                        strcpy(ResultsFile,buffer2);
                        continue;
                        }
                if(strcmp(buffer,"PrinterInitString")==0){
                        strcpy(PrinterInitString,buffer2);
                        continue;
                        }
                if(strcmp(buffer,"PrinterResetString")==0){
                        strcpy(PrinterResetString,buffer2);
                        continue;
                        }
                if(strcmp(buffer,"PrinterNormalString")==0){
                        strcpy(PrinterNormalString,buffer2);
                        continue;
                        }

                if(strcmp(buffer,"TempFile")==0){
                        strcpy(TempFile,buffer2);
                        continue;
                        }

                if(strcmp(buffer,"ExcludedRelay")==0){
                // Look up the index number of this relay
                  for(i=0;(i<NOEVENTS)&&(strcmp(EventNames[i],buffer2)!=0)
                                        ;i++);
                  // And add it to the list
		  if(i<NOEVENTS){
                  	Excluded->events[Excluded->number++]=i;
                  ExcludedRelays->events[ExcludedRelays->number++]=i-NOEVENTS+NORELAYS+1;

		  }
                  continue;
                }
                if(strcmp(buffer,"ExcludedEvent")==0){
                // Look up the index number of this event
                  for(i=0;(i<NOEVENTS)&&(strcmp(EventNames[i],buffer2)!=0)
                                        ;i++);
                  // And add it to the list
                  Excluded->events[Excluded->number++]=i;
                  continue;
                }

                        // Nothing recognizable found
                        fprintf(errstream, "Unable to interpret line from score.ini: %s\n",LineBuffer);
                        return 1;
                       
                    }     // end if: skip comments 
                }           // end while for parsing score.ini
                return 0;
} // end routine
