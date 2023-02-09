/* audit.c: implementation of the audit  shell command */

#include "shell.h"

#define HEADING "		MATHEMATICS DEPARTMENT STAFFING AUDIT"

static int find_opt(char *);
float ClassLoad(struct ClassStruct *, struct JobStruct *);
int CanDo(struct PersonStruct *, struct JobStruct *);

int
audit(SHELLSTREAMS)
{

	int argc;
	char *args[10];
	char **argv;
	char argstr[SHORTNAME];
	int i,j,k,c,l;
	int totals_col = 0;
	int found;
	int use_ay = 0;

	static float CumDemandByJob[MAX_JOBS];
	static float CurDemandByJob[MAX_JOBS];
	static float CurAvailByJob[MAX_JOBS];
	static float CumAvailByJob[MAX_JOBS];
	static float CumDemandTotal;
	static float CurDemandTotal;
	static float CumAvailTotal;
	static float CurAvailTotal;


/* process command line options */

argc = numtokens;
	argv = args;
	if(numtokens >= 10)return 1;
	for(i=0;i<numtokens;i++)
		args[i] = Tokens[i]->text;


if(--argc > 0){  /* Anything on command line ? */
		++argv;
		while((argc > 0)&&((*argv[0]) == '-')){ /* loop while 
							there are flags */
			argstr[0]='-';
			argstr[1]='\0';
			strcat(argstr,++*argv); /* glob argstr*/
			argc--;
			while((argc>0)&&(strlen(*argv)<3)&& (strlen(*(argv+1))<3)&&
				((*(argv+1))[0]=='-')&&((*(argv+1))[1]!='-')){
				++argv;
				argc--;
				strcat(argstr,++*argv);
			}
			++argv; /* ok, consecutive flags now globbed into one*/
		       c = find_opt(argstr+1); /* look up flag code */
		switch(c){

/* 
   Make sure to increment argv and decrement argc 
   each time. The code for handling the arguments for a given flag must
   go in the case for that flag. 

*/

			case -1: break;  /* default, e.g no options */

			case 0: 
				if(argc > 0) return 1;
				for(i=0;i<numjobs;i++){
					CumAvailByJob[i] = 0;
					CumDemandByJob[i] = 0;
				}
				CumDemandTotal = 0;
				CumAvailTotal  = 0;
				return 0;

			case 3: use_ay = 1;
			case 1: totals_col = 1;
				break;
			case 2: use_ay = 1;
				break;
					
			case -2: ;  /* barf (don't use in your code) */
			default:
				return 1;
			} /* End flag switch */
	} /* End flag grabbing while loop */
} /* end if: any args ? */

/*   argc now gives a count of remaining arguments on command line,
	and argv points at the first one */

	if(numschedules == 0){
		myerror(shell_errorstream,"No schedules are currently loaded.\n");
		return 0;
	}
	fprintf(shell_ostream,HEADING);
	fprintf(shell_ostream,"\n\n");

	fprintf(shell_ostream,"%-20s","Semester:");
	for(i=0;i<numschedules;i++)
		fprintf(shell_ostream,"%s\t",Schedules[i]->semester);
	if(totals_col)fprintf(shell_ostream,"Total");

/* Generate Demand Section */

	fprintf(shell_ostream,"\nDemand --------------------------\n");

/* Loop over jobs */
/* Classify load by job */

	for(i=0;i<numjobs;i++){
		fprintf(shell_ostream,"%-20s",Jobs[i]->name);
		/* loop over semesters */
		for(j=0;j<numschedules;j++){
			/* loop over courses */
			for(k=0;k<Schedules[j]->numcourses;k++){
				/* loop over classes */
				for(l=0;l<Schedules[j]->Courses[k]->numclasses;
					l++){

				  CurDemandByJob[i] += ClassLoad(
					Schedules[j]->Courses[k]->classes[l],
						Jobs[i]);

				}
			/* loop over assignments */
				for(l=0;l<Schedules[j]->Courses[k]->numassignments;l++)	
				  if(strcmp(Schedules[j]->Courses[k]->assignments[l]->job->name,
					Jobs[i]->name)==0)
					CurDemandByJob[i] +=
					  Schedules[j]->Courses[k]->assignments[l]->job->load;
			}
			/* loop over stand-alone classes */
			for(k=0;k<Schedules[j]->numclasses;k++)
				CurDemandByJob[i] += ClassLoad(
					Schedules[j]->classes[k],Jobs[i]);
			/* loop over stand-alone assignments */
			for(k=0;k<Schedules[j]->numassignments;k++)
				if(strcmp(Schedules[j]->assignments[k]->job->name,Jobs[i]->name)==0)CurDemandByJob[i] += Jobs[i]->load;

			 fprintf(shell_ostream,"%-8g",
				CurDemandByJob[i]);
			CumDemandByJob[i] += CurDemandByJob[i];
			CurDemandByJob[i] = 0;
		}
		if(totals_col)fprintf(shell_ostream,"%g",CumDemandByJob[i]);
		fprintf(shell_ostream,"\n");

	}

/* repeat the whole thing to total all jobs */
		fprintf(shell_ostream,"%-20s","Total");
		/* loop over semesters */
		for(j=0;j<numschedules;j++){
			/* loop over courses */
			for(k=0;k<Schedules[j]->numcourses;k++){
				/* loop over classes */
				for(l=0;l<Schedules[j]->Courses[k]->numclasses;
					l++){

				  CurDemandTotal += ClassLoad(
					Schedules[j]->Courses[k]->classes[l],
						NULL);

				}
			/* loop over assignments */
				for(l=0;l<Schedules[j]->Courses[k]->numassignments;l++)	
					CurDemandTotal +=
					  Schedules[j]->Courses[k]->assignments[l]->job->load;
			}
			/* loop over stand-alone classes */
			for(k=0;k<Schedules[j]->numclasses;k++)
				CurDemandTotal += ClassLoad(
					Schedules[j]->classes[k],NULL);
			/* loop over stand-alone assignments */
			for(k=0;k<Schedules[j]->numassignments;k++)
				CurDemandTotal += 
				 Schedules[j]->assignments[k]->job->load;

			 fprintf(shell_ostream,"%-8g",
				CurDemandTotal);
			CumDemandTotal += CurDemandTotal;
			CurDemandTotal = 0;
		}
		if(totals_col)fprintf(shell_ostream,"%g",CumDemandTotal);
		fprintf(shell_ostream,"\n");


/* Generate Avail Section */

	fprintf(shell_ostream,"\n\nAvailable ----------------------\n");

/* Loop over jobs */

	for(i=0;i<numjobs;i++){
                fprintf(shell_ostream,"%-20s",Jobs[i]->name);
		for(j=0;j<numschedules;j++){
			for(k=0;k<numpeople;k++)
		        	if(CanDo(People[k],Jobs[i])){
				 if(!use_ay){ /* look up load for this semester */
				  found = 0;
				  for(l=0;l<People[k]->numloads;l++)
				    if(strcmp(People[k]->loads[l]->semester,
					Schedules[j]->semester)==0){
						found = 1;
						break;
					}
				  if(found) CurAvailByJob[i] +=
			           People[k]->loads[l]->load;
				  }
				  else CurAvailByJob[i] += People[k]->load_ay/2;
				}
         		fprintf(shell_ostream,"%-8g",
                                CurAvailByJob[i]);
                        CumAvailByJob[i] += CurAvailByJob[i];
                        CurAvailByJob[i] = 0;
                }
                if(totals_col)fprintf(shell_ostream,"%g",CumAvailByJob[i]);
                fprintf(shell_ostream,"\n");
	}

/* repeat the whole thing to total all jobs */
                fprintf(shell_ostream,"%-20s","Total");
                /* loop over semesters */
                for(j=0;j<numschedules;j++){
			for(k=0;k<numpeople;k++){
				 if(!use_ay){
				  found = 0;
                                  for(l=0;l<People[k]->numloads;l++)
                                    if(strcmp(People[k]->loads[l]->semester,
                                        Schedules[j]->semester)==0){
                                                found = 1;
                                                break;
                                        }
                                  if(found) CurAvailTotal +=
                                   People[k]->loads[l]->load;
				 }
				 else CurAvailTotal +=
					People[k]->load_ay/2;
			}
			fprintf(shell_ostream,"%-8g",CurAvailTotal);
			CumAvailTotal += CurAvailTotal;
			CurAvailTotal = 0;
		}
		if(totals_col)fprintf(shell_ostream,"%g",CumAvailTotal);
		fprintf(shell_ostream,"\n");

/* Generate the Available - Demand Section */

/* Ok, I realize it's very inefficient to repeat both calculations above
   in order to form their difference, but so what ? This program doesn't
   get run very often, and I'm too sick of this program to cope with
   doing it the right way :-( */

	fprintf(shell_ostream,"\nAvailable - Demand\n");
	fprintf(shell_ostream,"---------------------------------\n");

/* Loop over jobs */
/* Classify load by job */

	for(i=0;i<numjobs;i++){
		CumAvailByJob[i] = 0;
		CumDemandByJob[i] = 0;
		CumDemandTotal = 0;
		CumAvailTotal = 0;
		fprintf(shell_ostream,"%-20s",Jobs[i]->name);
		/* loop over semesters */
		for(j=0;j<numschedules;j++){
			/* loop over courses */
			for(k=0;k<Schedules[j]->numcourses;k++){
				/* loop over classes */
				for(l=0;l<Schedules[j]->Courses[k]->numclasses;
					l++){

				  CurDemandByJob[i] += ClassLoad(
					Schedules[j]->Courses[k]->classes[l],
						Jobs[i]);

				}
			/* loop over assignments for this course */
				for(l=0;l<Schedules[j]->Courses[k]->numassignments;l++)	
				  if(strcmp(Schedules[j]->Courses[k]->assignments[l]->job->name,
					Jobs[i]->name)==0)
					CurDemandByJob[i] +=
					  Schedules[j]->Courses[k]->assignments[l]->job->load;
			}
			/* loop over stand-alone classes */
			for(k=0;k<Schedules[j]->numclasses;k++)
				CurDemandByJob[i] += ClassLoad(
					Schedules[j]->classes[k],Jobs[i]);
			/* loop over stand-alone assignments */
			for(k=0;k<Schedules[j]->numassignments;k++)
				if(strcmp(Schedules[j]->assignments[k]->job->name,Jobs[i]->name)==0)CurDemandByJob[i] += Jobs[i]->load;

			for(k=0;k<numpeople;k++)
                                if(CanDo(People[k],Jobs[i])){
				 if(!use_ay){
                                  found = 0;
                                  for(l=0;l<People[k]->numloads;l++)
                                    if(strcmp(People[k]->loads[l]->semester,
                                        Schedules[j]->semester)==0){
                                                found = 1;
                                                break;
                                        }
                                  if(found) CurAvailByJob[i] +=
                                   People[k]->loads[l]->load;
				  }
				  else CurAvailByJob[i] += People[k]->load_ay/2;
                                }

			 fprintf(shell_ostream,"%-8g",
				CurAvailByJob[i]-CurDemandByJob[i]);
			CumDemandByJob[i] += CurDemandByJob[i];
			CumAvailByJob[i] += CurAvailByJob[i];
			CurDemandByJob[i] = 0;
			CurAvailByJob[i] = 0;
		}
		if(totals_col)fprintf(shell_ostream,"%g",CumAvailByJob[i]-
			CumDemandByJob[i]);
		fprintf(shell_ostream,"\n");

	}

/* repeat the whole thing to total all jobs */
		fprintf(shell_ostream,"%-20s","Total");
		/* loop over semesters */
		for(j=0;j<numschedules;j++){
			/* loop over courses */
			for(k=0;k<Schedules[j]->numcourses;k++){
				/* loop over classes */
				for(l=0;l<Schedules[j]->Courses[k]->numclasses;
					l++){

				  CurDemandTotal += ClassLoad(
					Schedules[j]->Courses[k]->classes[l],
						NULL);

				}
			/* loop over assignments for this course */
				for(l=0;l<Schedules[j]->Courses[k]->numassignments;l++)	
					CurDemandTotal +=
					  Schedules[j]->Courses[k]->assignments[l]->job->load;
			}
			/* loop over stand-alone classes */

			for(k=0;k<Schedules[j]->numclasses;k++)
				CurDemandTotal += ClassLoad(
					Schedules[j]->classes[k],NULL);

			/* loop over stand-alone assignments */

			for(k=0;k<Schedules[j]->numassignments;k++)
				CurDemandTotal += 
				 Schedules[j]->assignments[k]->job->load;
			for(k=0;k<numpeople;k++){
				if(!use_ay){
                                 found = 0;
                                 for(l=0;l<People[k]->numloads;l++)
                                 if(strcmp(People[k]->loads[l]->semester,
                                       Schedules[j]->semester)==0){
                                              found = 1;
                                              break;
                                     }
                                  if(found) CurAvailTotal +=
                                   People[k]->loads[l]->load;
				 }
				 else CurAvailTotal += People[k]->load_ay/2;
                        }

			 fprintf(shell_ostream,"%-8g",
				CurAvailTotal - CurDemandTotal);
			CumDemandTotal += CurDemandTotal;
			CumAvailTotal += CurAvailTotal;
			CurDemandTotal = 0;
			CurAvailTotal = 0;
		}
		if(totals_col)fprintf(shell_ostream,"%g",CumAvailTotal - 
			CumDemandTotal);
		fprintf(shell_ostream,"\n");


/* Generate Legend */
	fprintf(shell_ostream,"\n\nLegend\n------------------------------\n");
	for(i=0;i<numjobs;i++)
                fprintf(shell_ostream,"1 %-20s  =  %g units\n",
			Jobs[i]->name,Jobs[i]->load);

	/* clean up and return */
	for(i=0;i<numjobs;i++){
		CurAvailByJob[i] = 0;
		CurDemandByJob[i] = 0;
	}
	CurDemandTotal = 0;
	CurAvailTotal = 0;
	return 0;
}

int find_opt(char *word)
{
	if(strcmp(word,"c")==0) return 0;
	if(strcmp(word,"t")==0) return 1;
	if(strcmp(word,"a")==0) return 2;
	if(strcmp(word,"at") == 0) return 3;
	if(strcmp(word,"ta") == 0) return 3;

	/* arg not found */
	return -2;
}
	
/* recursively total loads for this class and those of its subclasses
   and assignments
   that match job
   if Job is NULL then count all assignments
*/

float ClassLoad(struct ClassStruct *aclass, struct JobStruct *ajob)
{
	float total = 0;
	int i;

	if(ajob != NULL){
	if(aclass->numclasses == 0){  /* no subclasses */
		total = 0;
		if(aclass->available){
		 if(strcmp(aclass->job->name,ajob->name)==0)
			total = ajob->load;
		 for(i=0;i<aclass->numassignments;i++)
			if(strcmp(aclass->assignments[i]->job->name,
				ajob->name) == 0)
				total += ajob->load;
		}
		return total;
	}

	if((aclass->available)&&(strcmp(aclass->job->name,ajob->name)==0))
		total = ajob->load;
	else total = 0;
	for(i=0; i<aclass->numclasses;i++)
		total += ClassLoad(aclass->classes[i],ajob);
	for(i=0;i<aclass->numassignments;i++)
			if(strcmp(aclass->assignments[i]->job->name,
				ajob->name) == 0)
				total += ajob->load;
	return total;
	}
	
/* Case of a NULL job */
/* Count class load, subclass loads etc regardless of the job */

	if(aclass->numclasses == 0){
		total = 0;
		if(aclass->available)
		total = aclass->job->load;
		for(i=0;i<aclass->numassignments;i++)
			total += aclass->assignments[i]->job->load;
	return total;
	}
	if(aclass->available)
	total = aclass->job->load;
	else total = 0;
	for(i=0;i<aclass->numclasses;i++)
		total += ClassLoad(aclass->classes[i],NULL);
	for(i=0;i<aclass->numassignments;i++)
		total += aclass->assignments[i]->job->load;
	return total;

}
