
char linebuf[MAXLINE];

int CopyTimesFile(char *source, char *dest)
{
	char *ptr ;
	int parsed = 0;
	int i;
	int hold;
	FILE *input, *output;
	int line = 0;

	ptr = linebuf;

	numtokens = 0;

	if((input = fopen(source,"r"))==NULL){
		myerror(errstream,"Unable to open %s\n",source);
		return 1;
	}
	if((output = fopen(dest,"w"))==NULL){
		myerror(errstream,"Unable to open %s\n",dest);
		return 1;
	}

/* Loop over lines in timesfile */

	while(fgets(ptr,MAXLINE + linebuf -ptr ,input) != NULL){
	line++;

/* Now parse the contents of linebuf */

	numtokens = tokenize(linebuf,Tokens);
	if(numtokens == -1){       /* line continues */
		 ptr = ptr + strlen(ptr);
		 continue;
	}

	ptr = linebuf;

	if(strcmp(Tokens[0]->text,"SemesterStart")==0){
	fprintf(output,"%s %s %s","SemesterStart",SemStart,"0:00");
	parsed = 1;
	}

	if(strcmp(Tokens[0]->text,"EndOfClasses")==0){
	fprintf(output,"%s %s %s","EndOfClasses",EndClasses,"5:00");
	parsed = 1;
	}

	if(strcmp(Tokens[0]->text,"EndOfSemester")==0){
	fprintf(output,"%s %s %s","EndOfSemester",EndSemester,"9:00");
	parsed = 1;
	}


	if(strcmp(Tokens[0]->text,"PeriodicTimeSlot")==0) {

/* Check the format of token 3. If it is a number then assume that what
   follows is a list of numerical offsets from semester start. If is a
   date then what follows is a list of date-time pairs */

		if(strpbrk(Tokens[3]->text,"/-") != NULL) {

/* it's a date! */

	/* omit such lines */
		parsed = 1;

		}


		else { /* It's an offset */
		fprintf(output,"%s",linebuf);
		parsed = 1;
		}


	}

/* NonPeriodic Time Slot entry */
/* Don't copy these */

	if(strcmp(Tokens[0]->text,"NonPeriodicTimeSlot")==0) {
   
		parsed = 1;
	}
		
/* Holidays  entry */

	if(strcmp(Tokens[0]->text,"Holidays")==0) {
	fprintf(output,"Holidays ");
	for(i=0;i<HolidayData.number-1,i++)
		fprintf(output,"\"%s\" %s %s  %s %s \\\n",
			HolidayData.name[i],
			HolidayData.starts[i],
			"0:00",
			HoldiayData.ends[i],
			"0:00");
	fprintf(output,"\"%s\" %s %s  %s %s \n",
			HolidayData.name[i],
			HolidayData.starts[i],
			"0:00",
			HoldiayData.ends[i],
			"0:00");

	parsed = 1;
	}
		


/* Prepare to process the next line */

        /* Pass through comments, etc */
	if(!parsed)fprintf(output,"%s\n",linebuf);
		
	ptr = linebuf;
	parsed = 0;
	} 
return 0;
}

