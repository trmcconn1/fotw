/* main module of my Noontime League Records manager */ 

#include <stdio.h>
#include <stdlib.h>
#include "parse.h"
#include "db.h"
#include <string.h>
#include <time.h>

/* Where the data are */
#define RUNSCSV "/home/mcconnel/public_html/runs.csv"
#define RECORDCSV "/home/mcconnel/Noontime_League/csvs/record.csv"
#define RECORDNEW "/home/mcconnel/Noontime_League/csvs/record_new.csv"
#define RECORDFORMAT "/home/mcconnel/Noontime_League/csvs/recordformat.csv"


#define OPSEP '-'
#define USAGE "agrecords [-hvqp] "
#define HELP0 "Manage age group records for the noontime running league"
#define HELP1 "reads runs.csv,record.csv. Writes record_new.csv,recordformat.csv"
#define HELP2 "-h: print this helpful message and exit"
#define HELP3 "-v: print version number and exit"
#define HELP4 "-q: quiet. Don't print play by play"
#define HELP5 "-p: Print text records to stdout, not a .csv"
#define VERSION "1.0"

int is_relay(char*);
char *comment(char *);
extern int sunsecs(float,char *);


int main(int argc, char **argv)
{
	int status = MY_NORMAL;
	int print_option = 0;
	int verbose = 1;
	int i,nruns=0;
	char *p;
	int ag_len,name_len,team_len,event_len,date_len;
	char buffer[MAX_FIELD];
	char datetime[32];
	char name[MAX_FIELD];
	char team[MAX_FIELD];
	char sex;
	char ag[MAX_FIELD];
	char event[MAX_FIELD];
	char date[MAX_FIELD];
	double mytime;
	double pts;
	int type;
	struct db_entry *arun,*arecord;
	time_t time_now;

	/* Time Stamp */
	time_now = time(NULL);
	strftime(datetime,31,"%d %B, %Y",localtime(&time_now));

	FILE *istream = stdin;
	FILE *recordnew, *recordformat;

	/* Process Command Line */

	if(argc > 1) {
		while(argc > 1)
		  if((*++argv)[0]==OPSEP)
			switch((*argv)[1]){
				case 'h':
					printf("\n%s\n",USAGE); 
					printf("\n%s\n",HELP0);
					printf("%s\n",HELP1);
					printf("%s\n",HELP2);
					printf("%s\n",HELP3);
					printf("%s\n",HELP4);
					printf("%s\n",HELP5);
					printf("\n");
					argc--;
					exit(1);
					break;
				case 'v':
					printf("%s\n",VERSION);
					exit(1);
					break;  /* reminder */
				case 'q':
					argc--;
					argv++;
					verbose = 0;
					break;
				case 'p':
					argc--;
					argv++;
					verbose = 0;
					print_option = 1;
					break;
				default:
					fprintf(stderr,"agrecords: Invalid flag\n");
					fprintf(stderr,"%s\n",USAGE);
					exit(1);
			}
		else {
	        	if(!(istream = fopen(*argv,"r"))){
				fprintf(stderr,"agrecords: error Opening %s\n",*argv);
				exit(1);
 
			}
			argc--;
		}
	}

/* Load the database */

	if(verbose)printf("Timestamp %s\n",datetime);
	if(verbose)printf("Reading record performances...\n");
      	if(!(istream = fopen(RECORDCSV,"r"))){
		fprintf(stderr,"agrecords: error Opening %s\n",RECORDCSV);
		exit(1);
	}
 

	while(status != MY_EOF){

		p = parse(istream,&status);
		if(p == NULL)parse_error("name field null return");
		while(status == MY_EOL)p = parse(istream,&status);
		if(status == MY_EOF)break;

		if(p == NULL)parse_error("name field null return");
		name_len = strlen(p);
		strncpy(name,p,MAX_FIELD);
		if(status != MY_NORMAL)parse_error(p);

		p = parse(istream,&status);
		if(p == NULL)parse_error("team field null return");
		team_len = strlen(p);
		strncpy(team,p,MAX_FIELD);
		if(status != MY_NORMAL)parse_error(p);

		p = parse(istream,&status);
		if(p == NULL)parse_error("sex field null return");
		sex = p[0];
		if(status != MY_NORMAL)parse_error(p);

		p = parse(istream,&status);
		if(p == NULL)parse_error("ag field null return");
		ag_len = strlen(p);
		strncpy(ag,p,MAX_FIELD);
		if(status != MY_NORMAL)parse_error(p);

		p = parse(istream,&status);
		if(p == NULL)parse_error("event field null return");
		event_len = strlen(p);
		strncpy(event,p,MAX_FIELD);
		if(status != MY_NORMAL)parse_error(p);

		p = parse(istream,&status);
		if(p == NULL)parse_error("date field null return");
		date_len = strlen(p);
		strncpy(date,p,MAX_FIELD);
		if(status != MY_NORMAL)parse_error(p);

		p = parse(istream,&status);
		if(p == NULL)parse_error("time field null return");
		mytime = atof(p);
		if(status != MY_NORMAL)parse_error(p);
	
		p = parse(istream,&status);
		if(p == NULL)parse_error("pts field null return");
		pts = atof(p);
		if(status != MY_NORMAL)parse_error(p);

		p = parse(istream,&status);
		if(p == NULL)parse_error("type field null return");
		type = atoi(p);
		if((status != MY_EOL)
			&&(status != MY_EOF))parse_error(p);

		my_db[db_size] = alloc_entry(name_len+2,team_len+1,
			ag_len+1,event_len+1,date_len+1);
		if(!my_db[db_size]){
			fprintf(stderr,"Unable to allocate db entry\n");
			exit(1);
		}
		strcpy(my_db[db_size]->ag,ag);
		strcpy(my_db[db_size]->event,event);
		strcpy(my_db[db_size]->name,name);
		strcpy(my_db[db_size]->team,team);
		strcpy(my_db[db_size]->date,date);
		my_db[db_size]->sex = sex;
		my_db[db_size]->time = mytime;
		my_db[db_size]->pts = pts;
		my_db[db_size]->type = type;
		if(db_size == MAX_DB_SIZE){ 
			fprintf(stderr,"Maximum data base size reached\n");
			break;
		}
		db_size++;
	}

	if(verbose)printf("Read %d records from records database\n",db_size);
	fclose(istream);

	/* Sort database */

	sort_db_by_event_and_ag(0);

	if(verbose)printf("Opening runs.csv...\n");
      	if(!(istream = fopen(RUNSCSV,"r"))){
		fprintf(stderr,"agrecords: error Opening %s\n",RUNSCSV);
		exit(1);
	}

/* Loop over lines in the performance ytd spreadsheet */

	status = MY_NORMAL;
	line_no = 0;
	while(status != MY_EOF){

		p = parse(istream,&status);
		if(p == NULL)parse_error("name field null return");
		while(status == MY_EOL)p = parse(istream,&status);
		if(status == MY_EOF)break;

		if(p == NULL)parse_error("name field null return");
		name_len = strlen(p);
		strncpy(name,p,MAX_FIELD);
		if(status != MY_NORMAL)parse_error(p);

		p = parse(istream,&status);
		if(p == NULL)parse_error("team field null return");
		team_len = strlen(p);
		strncpy(team,p,MAX_FIELD);
		if(status != MY_NORMAL)parse_error(p);

		p = parse(istream,&status);
		if(p == NULL)parse_error("sex field null return");
		sex = p[0];
		if(status != MY_NORMAL)parse_error(p);

		p = parse(istream,&status);
		if(p == NULL)parse_error("ag field null return");
		ag_len = strlen(p);
		strncpy(ag,p,MAX_FIELD);
		if(status != MY_NORMAL)parse_error(p);

		p = parse(istream,&status);
		if(p == NULL)parse_error("event field null return");
		event_len = strlen(p);
		strncpy(event,p,MAX_FIELD);
		if(status != MY_NORMAL)parse_error(p);

		p = parse(istream,&status);
		if(p == NULL)parse_error("date field null return");
		date_len = strlen(p);
		strncpy(date,p,MAX_FIELD);
		if(status != MY_NORMAL)parse_error(p);

		p = parse(istream,&status);
		if(p == NULL)parse_error("time field null return");
		mytime = atof(p);
		if(status != MY_NORMAL)parse_error(p);
	
		p = parse(istream,&status);
		if(p == NULL)parse_error("pts field null return");
		pts = atof(p);
		if(status != MY_NORMAL)parse_error(p);

		p = parse(istream,&status);
		if(p == NULL)parse_error("type field null return");
		type = atoi(p);
		if((status != MY_EOL)
			&&(status != MY_EOF))parse_error(p);

		if(type != 1) continue;  /* skip all but official runs */
		if(is_relay(event)) continue;  /* skip relays */

		arun = alloc_entry(name_len+2,team_len+1,
			ag_len+1,event_len+1,date_len+1);
		if(!arun){
			fprintf(stderr,"Unable to allocate db entry\n");
			exit(1);
		}
		strcpy(arun->ag,ag);
		strcpy(arun->event,event);
		strcpy(arun->name,name);
		strcpy(arun->team,team);
		strcpy(arun->date,date);
		arun->sex = sex;
		arun->time = mytime;
		arun->pts = pts;
		arun->type = type;
		nruns++;

		/* Search records data base for this event/ag */

		/* arecord = search_db_by_event_and_ag(arun); */ 
		for(i=0;i<db_size;i++){
			arecord = my_db[i];
			if((arecord->name)[0] == '#')continue; 
			if(strcmp(arun->event,arecord->event)==0)
				if(strcmp(arun->ag,arecord->ag)==0)break;
		}
		if(i == db_size) arecord = NULL;
		if(!arecord){
			if(verbose)printf("New Record!\n");
			if(verbose)printf("%s,%s,%c,%s,%s,%s,%f,%f,%d\n", 
				arun->name,arun->team,arun->sex,arun->ag,
				arun->event,arun->date,arun->time,arun->pts,
				arun->type);
			my_db[db_size++] = arun;
		}
		else {
			if(arun->time < arecord->time){
				arecord->name = comment(arecord->name);
				if(verbose)printf("Improved Record!\n");
				if(verbose)printf("%s,%s,%c,%s,%s,%s,%f,%f,%d\n"				, arecord->name,arecord->team,arecord->sex,
				arecord->ag, arecord->event,arecord->date,
				arecord->time,arecord->pts,arecord->type);
				if(verbose)printf("%s,%s,%c,%s,%s,%s,%f,%f,%d\n"				, arun->name,arun->team,arun->sex,arun->ag,
				arun->event,arun->date,arun->time,arun->pts,
				arun->type);
				my_db[db_size++] = arun;
			}
			else free_entry(arun);
		    }
	}

	if(verbose)printf("Read %d performances\n",nruns);
	fclose(istream);
	
	/* Resort db in preparation to printing stuff out */
	sort_db_by_event_and_ag(0);

	if(print_option){ /* only human readable stuff to stdout */
	printf("\tNoontime Running League Age Group Records %s\n\n",datetime);
	printf("%-15s %s %3s %5s%12s%12s\t%s\n","Name","Team","AG","Event",
		"Time ","Date     ","Points"); 
		
	for(i=0;i<db_size;i++){
		arecord = my_db[i];
		if((arecord->name)[0] == '#')continue;
		sunsecs(arecord->time,buffer); /* nicely format time */
		printf("%-15s %s %3s %5s%12s%12s\t%2.1f\n", 
		arecord->name,arecord->team,arecord->ag,
		arecord->event,buffer,arecord->date,arecord->pts);
	}
	} /* end human readable version */
	else {
	if(verbose)printf("Writing %s and %s\n",RECORDNEW,RECORDFORMAT);
	if(!(recordnew = fopen(RECORDNEW,"w"))){
		fprintf(stderr,"agrecords: error Opening %s\n",RECORDNEW);
		exit(1);
	}
	if(!(recordformat = fopen(RECORDFORMAT,"w"))){
		fprintf(stderr,"agrecords: error Opening %s\n",RECORDFORMAT);
		exit(1);
	}
	fprintf(recordformat,"NAME,TEAM,SEX,AG,EVENT,DATE,TIME,PTS,TYPE\n");

	for(i=0;i<db_size;i++){
		arecord = my_db[i];
		fprintf(recordnew,"%s,%s,%c,%s,%s,%s,%f,%f,%d\n",
		 arecord->name,arecord->team,arecord->sex,arecord->ag,
		arecord->event,arecord->date,arecord->time,arecord->pts,
		arecord->type);
		sunsecs(arecord->time,buffer);
		fprintf(recordformat,"%s,%s,%c,%s,%s,%s,\"=\"\"%s\"\"\",%f,%d\n"
		, arecord->name,arecord->team,arecord->sex,arecord->ag,
		arecord->event,arecord->date,buffer,arecord->pts,arecord->type);
	}
	fclose(recordnew);
	fclose(recordformat);
	} /* end printing new .csv files */
		
/* clean up for form's sake */

	for(i = 0; i<db_size;i++)free_entry(my_db[i]);
	return 0;
}


int is_relay(char *p)
{
	if(strcmp(p,"800r")==0)return 1;
	if(strcmp(p,"1600r")==0)return 1;
	if(strcmp(p,"3200r")==0)return 1;
	if(strcmp(p,"4x1manley")==0)return 1;
	if(strcmp(p,"4x2manley")==0)return 1;
	if(strcmp(p,"4x3manley")==0)return 1;
	return 0;
}

/* prepend the comment character # to the string, free the string and return a pointer to the new one */

char *comment(char *p)
{
	int n;
	char *r;

	if(!p)return NULL;
	n = strlen(p);
	r = (char *)malloc((n+2)*sizeof(char));
	if(!r){
		fprintf(stderr,"agrecords: unable to allocate memory\n");
		exit(1);
	}
	r[0] = '#';
	r[1] = '\0';
	strcat(r,p);
	free(p);
	return r;
}
		
