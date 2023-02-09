
/* shelldb.c: shell commands for displaying data */

#include "shell.h"

int drooms(SHELLSTREAMS)
{
		int i,j;

		if(numtokens == 1){
			fprintf(shell_ostream," Currently defined Rooms \n");
			for(i=0;i<numrooms;i++)
			  fprintf(shell_ostream,"%d. %s\n",i+1,Rooms[i]->name);
			return 0;
		}
		if(numtokens == 2){
			 i = atoi(Tokens[1]->text);
                         if((i<1)||(i>numrooms)){
				myerror(shell_errorstream,
					"Room choice out of range\n");
				return 1;
			 }

                        i--;   /* convert to C-style index */

			fprintf(shell_ostream,"Name = %s\n",Rooms[i]->name);
			fprintf(shell_ostream,"Bldg = %s\n",Rooms[i]->bldg);
			fprintf(shell_ostream,"Number= %s\n",Rooms[i]->number);
			fprintf(shell_ostream,"Seating = %d\n",Rooms[i]->seats);
			fprintf(shell_ostream,"Exam Seating  = %d\n",Rooms[i]->exam_seats);
			fprintf(shell_ostream,"Usage  = %s\n",Rooms[i]->usage);
			fprintf(shell_ostream,"Classes currently meeting here: \n");
			for(j=0;j<Rooms[i]->numclasses;j++){
			fprintf(shell_ostream,"%s  " ,
				((struct ClassStruct *)(Rooms[i]->classes[j]))->crse_name);
			fprintf(shell_ostream,"%s  " ,
				((struct ClassStruct *)(Rooms[i]->classes[j]))->name);
			if(((struct ClassStruct *)(Rooms[i]->classes[j]))->times == NULL)
				fprintf(shell_ostream,"%s  ",MissingData);            
			else
				fprintf(shell_ostream,"%s  " ,
				((struct ClassStruct *)(Rooms[i]->classes[j]))->times->name);
			if(((struct ClassStruct *)(Rooms[i]->classes[j]))->room == NULL)
				fprintf(shell_ostream,"%s  ",MissingData);            
			else
				fprintf(shell_ostream,"%s  %s\n" ,
				((struct ClassStruct *)(Rooms[i]->classes[j]))->room->bldg,
				((struct ClassStruct *)(Rooms[i]->classes[j]))->room->number);
		}

			return 0;
		}
		return 1;
}

int dtimes(SHELLSTREAMS)
{
		int i;

		if(numtokens == 1){
			for(i=0;i<NumberOfSlotLists;i++){
                                fprintf(shell_ostream,"%d. %s\n",i+1,TimeSlotLists[i]->name);
			}
			return 0;
		}
		if(numtokens == 2) {
			i = atoi(Tokens[1]->text);
                        if((i<1)||(i>NumberOfSlotLists)){
				myerror(shell_errorstream,
					"Time slot list number out of range\n");
				return 0;
			}
                        i--; /* Convert to C-style index */
			DumpSlotList(shell_ostream,TimeSlotLists[i]);
			return 0;
		}
		return 1;
}

int ComparePeopleByRankandName(struct PersonStruct *, struct PersonStruct *);

int dpeople(SHELLSTREAMS)
{
	int i,j;
	char buffer[SHORTNAME];

		krqsort((void **)People,0,numpeople-1,(int(*) (void *,void *))
                  ComparePeopleByRankandName);

		if(numtokens == 1){
			buffer[0] = '\0';
			for(i=0;i<numpeople;i++){
			  if(strcmp(People[i]->apt_type,buffer) != 0)
                           fprintf(shell_ostream,"\n %s:\n\n",People[i]->apt_type);
			  fprintf(shell_ostream,"%d. %s\n",i+1,People[i]->shortname);
			strcpy(buffer,People[i]->apt_type);
			}
			return 0;
		}
		if(numtokens == 2){ i = atoi(Tokens[1]->text);
                if((i<1)||(i>numpeople)){
				myerror(shell_errorstream,
					"Person choice out of range\n");
				return 0;
			}
                i--; /* convert to C-style index */
                fprintf(shell_ostream,"%s ( %s )\n",
                 People[i]->longname,People[i]->apt_type);
                 strftime(common_buf,SHORTNAME,"%m/%d/%Y",
                   localtime(&(People[i]->start)));
                fprintf(shell_ostream,"Appointment Date: %s\n", common_buf);
                fprintf(shell_ostream,"Academic Year Load: %g\n",People[i]->load_ay);
                fprintf(shell_ostream,"Assigned so far this semester: %g\n",
                People[i]->load_now);
                fprintf(shell_ostream,"Semester load(s):\n");
		for(j=0;j<People[i]->numloads;j++)
			fprintf(shell_ostream,"%s: %g\n",
			  People[i]->loads[j]->semester,People[i]->loads[j]->load);
		fprintf(shell_ostream,"Classes Assigned:\n");
		for(j=0;j<People[i]->numclasses;j++){
			fprintf(shell_ostream,"%s  " ,
				((struct ClassStruct *)(People[i]->classes[j]))->crse_name);
			fprintf(shell_ostream,"%s  " ,
				((struct ClassStruct *)(People[i]->classes[j]))->name);
			if(((struct ClassStruct *)(People[i]->classes[j]))->times == NULL)
				fprintf(shell_ostream,"%s  ",MissingData);            
			else
				fprintf(shell_ostream,"%s  " ,
                                ((struct ClassStruct *)(People[i]->classes[j]))->times->description);
			if(((struct ClassStruct *)(People[i]->classes[j]))->room == NULL)
				fprintf(shell_ostream,"%s  ",MissingData);            
			else
				fprintf(shell_ostream,"%s  %s\n" ,
				((struct ClassStruct *)(People[i]->classes[j]))->room->bldg,
				((struct ClassStruct *)(People[i]->classes[j]))->room->number);
		}

		return 0;
		}
	return 1;
}

int djobs(SHELLSTREAMS)
{
	int i,j;

		if(numtokens == 1){
			fprintf(shell_ostream," Currently defined Jobs \n");
			for(i=0;i<numjobs;i++){
			  fprintf(shell_ostream,"%d. %s (",i+1,Jobs[i]->name);
			  for(j=0;j<Jobs[i]->num_types;j++)
			  fprintf(shell_ostream," %s ",*(Jobs[i]->apt_type+j));
			  fprintf(shell_ostream,")\n");
			}
			return 0;
		}
		if(numtokens == 2){
		 i = atoi(Tokens[1]->text);
                if((i<1)||(i>numjobs)){
				myerror(shell_errorstream,
					"Job choice out of range\n");
				return 0;
			}
                i--; /* convert to C-style index */
		fprintf(shell_ostream,"Name = %s\n",Jobs[i]->name);
		fprintf(shell_ostream,"Appointment types: ");
			  for(j=0;j<Jobs[i]->num_types;j++)
			  fprintf(shell_ostream," %s ",*(Jobs[i]->apt_type+j));
			  fprintf(shell_ostream,"\n");
		fprintf(shell_ostream,"Start Date = %s",
			asctime(localtime(&(Jobs[i]->start))));
		fprintf(shell_ostream,"End Date = %s",
			asctime(localtime(&(Jobs[i]->end))));
		fprintf(shell_ostream,"Load = %g\n",Jobs[i]->load);
		fprintf(shell_ostream,"Description  = %s\n",Jobs[i]->description);
		return 0;
		}
		return 1;
}

int
dblocks(SHELLSTREAMS){
	int i;

	fprintf(shell_ostream,"\nCurrently defined final examination blocks:\n");
	for(i=0;i<numblocks;i++){
	  fprintf(shell_ostream,"\n%d. Blockname: %s\n",i+1,FinalBlocks[i]->name);
	  fprintf(shell_ostream,"Exam Time: %s( %s )\n",FinalBlocks[i]->exam_time->description,
            FinalBlocks[i]->exam_time->name);
	  if(FinalBlocks[i]->crse_time == NULL)
           fprintf(shell_ostream,"Courses assigned by name.\n");
          else
           fprintf(shell_ostream,"Class Time: %s( %s )\n",
           FinalBlocks[i]->crse_time->description,
           FinalBlocks[i]->crse_time->name);
	}
	return 0;
}

int CompareBlocksByName(struct FinalBlockStruct *, struct FinalBlockStruct *);
extern struct ScheduleStruct *CurrentSchedule;

int 
dfinals(SHELLSTREAMS){
	int i,j;
	struct FinalBlockStruct TBlock;
	struct FinalBlockStruct *ABlock;

	if(numtokens > 2) return 1;
	if(numschedules == 0 || CurrentSchedule == NULL){
		myerror(shell_errorstream,"No schedule loaded.\n");
                return 0;
	}

	if(numtokens == 2) {
        	strcpy(TBlock.name,Tokens[1]->text);
		krqsort((void **)FinalBlocks,0,numblocks-1,(int(*) (void *,void *))
                 CompareBlocksByName);
        	if( (ABlock = (struct FinalBlockStruct *) binsearch((void *)&TBlock,
          	(void **)FinalBlocks,numblocks,
          	(int(*) (void *,void *))CompareBlocksByName)) == NULL){
          		myerror(shell_errorstream,"No such exam block %s\n",
                	Tokens[1]->text);
          		return 0;
		}
		fprintf(shell_ostream,"\nFinal Exam Block %s\n\n",ABlock->name);       
                fprintf(shell_ostream,"%-5s%-6s%-20s%-20s%-10s%-10s%-s\n",
                "Crs", "Class","Instructor","Exam Time","Bldg","Room",
                "Enrollment");
                for(j=0;j<ABlock->numclasses;j++)
                  fprintf(shell_ostream,"%-5s%-6s%-20s%-20s%-10s%-10s%-d\n",
                    ABlock->classes[j]->crse_name,
                    ABlock->classes[j]->name,
                    ABlock->classes[j]->teacher->shortname,
                    ABlock->exam_time->description,
                    ABlock->classes[j]->finalroom->bldg,
                    ABlock->classes[j]->finalroom->number,
                    ABlock->classes[j]->enrollment);
		return 0;
	}

	for(i=0;i<numblocks;i++){
		fprintf(shell_ostream,"\nFinal Exam Block %s\n\n",FinalBlocks[i]->name);
                fprintf(shell_ostream,"%-5s%-6s%-20s%-18s%-10s%-10s%-s\n",
                    "Crs", "Class","Instructor","Exam Time","Bldg","Room",
                     "Enrollment");
		for(j=0;j<FinalBlocks[i]->numclasses;j++)
                 fprintf(shell_ostream,"%-5s%-6s%-20s%-18s%-10s%-10s%-d\n",
                  FinalBlocks[i]->classes[j]->crse_name,
                  FinalBlocks[i]->classes[j]->name,
                  FinalBlocks[i]->classes[j]->teacher->shortname,
                  FinalBlocks[i]->exam_time->description,
                  FinalBlocks[i]->classes[j]->finalroom->bldg,
                  FinalBlocks[i]->classes[j]->finalroom->number,
                  FinalBlocks[i]->classes[j]->enrollment);
	}

/* Print out classes that have no final assigned */

	fprintf(shell_ostream,"\nNo final yet assigned:\n\n");
	for(i=0;i<CurrentSchedule->numcourses;i++)
		for(j=0;j<CurrentSchedule->Courses[i]->numclasses;j++)
			if(strcmp(CurrentSchedule->Courses[i]->classes[j]->final->name,
				MissingData)==0)
				fprintf(shell_ostream,"%-5s%-6s%-20s%-18s%-10s%-10s%-d\n",
                  		  CurrentSchedule->Courses[i]->classes[j]->crse_name,
                                  CurrentSchedule->Courses[i]->classes[j]->name,
                                  CurrentSchedule->Courses[i]->classes[j]->teacher->shortname,
                                  MissingData,
                                  MissingData,
                                  MissingData,
                                  CurrentSchedule->Courses[i]->classes[j]->enrollment);

	for(i=0;i<CurrentSchedule->numclasses;i++)
		if(strcmp(CurrentSchedule->classes[i]->final->name,
                     MissingData)==0)
				fprintf(shell_ostream,"%-5s%-6s%-20s%-18s%-10s%-10s%-d\n",
                  		  CurrentSchedule->classes[i]->crse_name,
                                  CurrentSchedule->classes[i]->name,
                                  CurrentSchedule->classes[i]->teacher->shortname,
                                  MissingData,
                                  MissingData,
                                  MissingData,
                                  CurrentSchedule->classes[i]->enrollment);

	return 0;
}
           
