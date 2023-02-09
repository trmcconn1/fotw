/* main module of my charger records file manager */ 

#include <stdio.h>
#include <stdlib.h>
#include "parse.h"
#include <string.h>
#include "db.h"
#include "latex.h"
#include <time.h>

#define OPSEP '-'
#define USAGE "recman [-a <ag> -d <a|e> -l -hv] [datafile]"
#define HELP0 "read database from stdin or file and print selected data to stdout"
#define HELP1 "-a <ag>: select only records with given age group"
#define HELP2 "-h: print this helpful message and exit"
#define HELP3 "-v: print version number and exit"
#define HELP4 "-d: dump list of unique age groups or events and exit"
#define HELP5 "-l: with -a: print a latex source file"
#define VERSION "1.0"


int main(int argc, char **argv)
{
	int status = MY_NORMAL;
	int i,j;
	char *p;
	int ag_len,event_len,name_len,mark_len,date_len;
	int max_name_len = 0, max_event_len = 0, max_mark_len = 0,
		max_date_len = 0;
	char buffer[MAX_FIELD];
	char age_group[MAX_FIELD];
	char ag[MAX_FIELD];
	char event[MAX_FIELD];
	char name[MAX_FIELD];
	char mark[MAX_FIELD];
	char date[MAX_FIELD];
	int dump_ag_flag = 0;
	int dump_events_flag = 0;
	int print_text_ag_records_flag = 1;
	int latex_flag = 0;
	time_t time_now;

	FILE *istream = stdin;

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
				case 'a':
					argc--;
					argv++;
					strncpy(age_group,*argv,MAX_FIELD);
					argc--;
					break;
				case 'l':
					argc--;
					latex_flag = 1;
					break;
				case 'd':
					print_text_ag_records_flag = 0;
					argc--;
					argv++;
					if(strcmp("a",*argv)==0){
						dump_ag_flag = 1;
						argc--;
						break;
					}
					if(strcmp("e",*argv)==0){
						dump_events_flag = 1;
						argc--;
						break;
					}
					fprintf(stderr,"%s\n",USAGE);
					fprintf(stderr,"unrecognized -d option %s\n",*argv);
					exit(1);
				default:
					fprintf(stderr,"recman: Invalid flag\n");
					fprintf(stderr,"%s\n",USAGE);
					exit(1);
			}
		else {
	        	if(!(istream = fopen(*argv,"r"))){
				fprintf(stderr,"recman: error Opening %s\n",*argv);
				exit(1);
 
			}
			argc--;
		}
	}

/* Load the database */

	while(status != MY_EOF){

		p = parse(istream,&status);
		if(p == NULL)parse_error("ag field null return");
		while(status == MY_EOL) /* skip empty lines */
			p = parse(istream,&status);  
		if(status == MY_EOF)break;
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
		if(p == NULL)parse_error("name field null return");
		name_len = strlen(p);
		strncpy(name,p,MAX_FIELD);
		if(status != MY_NORMAL)parse_error(p);

		p = parse(istream,&status);
		if(p == NULL)parse_error("mark field null return");
		mark_len = strlen(p);
		strncpy(mark,p,MAX_FIELD);
		if(status != MY_NORMAL)parse_error(p);
	
		p = parse(istream,&status);
		if(p == NULL)parse_error("date field null return");
		date_len = strlen(p);
		strncpy(date,p,MAX_FIELD);
		if((status != MY_EOL)
			&&(status != MY_EOF))parse_error(p);

		my_db[db_size] = alloc_entry(ag_len+1,event_len+1,
			name_len+1,mark_len+1,date_len+1);
		if(!my_db[db_size]){
			fprintf(stderr,"Unable to allocate db entry\n");
			exit(1);
		}
		strcpy(my_db[db_size]->ag,ag);
		strcpy(my_db[db_size]->event,event);
		strcpy(my_db[db_size]->name,name);
		strcpy(my_db[db_size]->mark,mark);
		strcpy(my_db[db_size]->date,date);
		if(name_len > max_name_len)max_name_len = name_len;
		if(date_len > max_date_len)max_date_len = date_len;
		if(event_len > max_event_len)max_event_len = event_len;
		if(mark_len > max_mark_len)max_mark_len = mark_len;
		if(db_size == MAX_DB_SIZE){ 
			fprintf(stderr,"Maximum data base size reached\n");
			break;
		}
		db_size++;
	}


	/* Do action with database requested by user */
	if(dump_events_flag){

		sort_db_by_event(0);
		i=0;
		strncpy(buffer,my_db[0]->event,MAX_FIELD);
		printf("%s\n",buffer);
		while(i<db_size){

			if(strcmp(buffer,my_db[i]->event)==0)i++;
			else {
				strncpy(buffer,my_db[i]->event,MAX_FIELD);
				printf("%s\n",buffer);
				i++;
			}		
		}
	}
	if(dump_ag_flag){

		sort_db_by_ag(0);
		i=0;
		strncpy(buffer,my_db[0]->ag,MAX_FIELD);
		printf("%s\n",buffer);
		while(i<db_size){

			if(strcmp(buffer,my_db[i]->ag)==0)i++;
			else {
				strncpy(buffer,my_db[i]->ag,MAX_FIELD);
				printf("%s\n",buffer);
				i++;
			}		
		}
	}

	if(print_text_ag_records_flag){
		if(latex_flag)printf("%s",LATEX_BOILER1);
		if(latex_flag)printf("\\section*{Syracuse Chargers Records for %s}\n",age_group);
		else
	        	printf("Syracuse Chargers Records for %s\n\n",age_group);
		if(latex_flag)printf("%s",LATEX_BOILER2);
		for(i=0;i<db_size;i++){

			if(strcmp(my_db[i]->ag,age_group)==0)
				if(latex_flag)
					printf("%s&%s&%s&%s\\\\\n",
					my_db[i]->event,
					my_db[i]->name,
					my_db[i]->mark,
					my_db[i]->date );
				else
				{
					j = strlen(my_db[i]->event);
					printf("%s",my_db[i]->event);
					while(j++ < max_event_len+1)
						printf(" ");
					j = strlen(my_db[i]->name);
					printf("%s",my_db[i]->name);
					while(j++ < max_name_len+1)
						printf(" ");
					j = strlen(my_db[i]->mark);
					printf("%s",my_db[i]->mark);
					while(j++ < max_mark_len+1)
						printf(" ");
					printf("%s\n",my_db[i]->date);
				}
				
		}
		if(latex_flag)printf("%s",LATEX_BOILER3);
		time(&time_now);
		strftime(buffer,(size_t)MAX_FIELD,"%d %b %Y",
			(const struct tm *)localtime(&time_now));
		printf("%s\n",buffer);  /* time stamp */
		if(latex_flag)printf("%s",LATEX_BOILER4);
	}

/* clean up */
	for(i = 0; i<db_size;i++)free_entry(my_db[i]);
}
