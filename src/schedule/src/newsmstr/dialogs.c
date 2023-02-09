/* Implementation of Dialogue handlers called by Dialogue user
    interface function. */ 

#include "global.h"
#include "ui.h"
#include<string.h>


/* GetUser: dialog to get a line of data from user. 
  */


/* Data structure to exchange data with the main program */

struct UserDialogData {
int verbose;
char longmsg[BUFSIZ];
char shortmsg[BUFSIZ];
char usrinpt[BUFSIZ];
char prompt[LONGNAME];
char quitchar;
int (*test)(char *); /* function to test validity of user input */
};

int GetUser(FILE *istream, FILE *ostream, FILE *errstream, FILE *promptstream,
	char *errmsg,
	struct UserDialogData *data)
{


	if(data->verbose)
		fprintf(ostream,"%s",data->longmsg);

/* Display short message and prompt */
	fprintf(ostream,"%s",data->shortmsg);
	fprintf(promptstream,"%s",data->prompt);

	if(fgets(data->usrinpt,MAXLINE,istream)==NULL)
		return DIALOGUE_ABORT;

	if(strlen(data->usrinpt)==2 && data->usrinpt[0] == data->quitchar)
		return DIALOGUE_ABORT;

/* take off the closing newline from user input */
	data->usrinpt[strlen(data->usrinpt)-1]='\0';

	if(data->test == NULL) return DIALOGUE_FINISH;
	switch(data->test(data->usrinpt)){
	
		case 0: return DIALOGUE_FINISH;

		case 1: return DIALOGUE_NONFATAL;

		case 2: return DIALOGUE_FATAL;

		default: return DIALOGUE_FATAL;
	}
}

/* A specific dialog for getting a list of holidays from the user */

extern int VerifyDate(char *);

struct HolidayData {
int number;
char *name[100];
char *starts[100];
char *ends[100];
};

int HolidayDialog(FILE *istream, FILE *ostream, FILE *errstream, FILE *promptstream,
	char *errmsg,
	struct HolidayData *data)
{

	static char buffer[MAXLINE];
	int c;


	if(data->number >= 100){
		myerror(errstream,"Too many holidays![100 max]\n");
		return DIALOGUE_FINISH;
	}
retry:
	fprintf(ostream,"%s","Enter the name of next holiday[q to quit]\n");
	fprintf(promptstream,"Holidays> ");

	if(fgets(buffer,MAXLINE,istream)==NULL)
		return DIALOGUE_ABORT;

	if(strlen(buffer)==2 && buffer[0] == 'q')
		return DIALOGUE_FINISH;

/* take off the closing newline from user input */
	buffer[strlen(buffer)-1]='\0';

	data->name[data->number] = malloc(strlen(buffer)+1);
	strcpy(data->name[data->number],buffer);

start:
	fprintf(ostream,"Enter the start date of this holiday\n");
	fprintf(promptstream,"Holidays> ");

	if(fgets(buffer,MAXLINE,istream)==NULL)
		return DIALOGUE_ABORT;

/* take off the closing newline from user input */
	buffer[strlen(buffer)-1]='\0';
	
	if(VerifyDate(buffer) !=0) goto start;
	data->starts[data->number] = malloc(strlen(buffer)+1);
	strcpy(data->starts[data->number],buffer);

end: 

	fprintf(ostream,"Enter the end date of this holiday\n");
	fprintf(promptstream,"Holidays> ");

	if(fgets(buffer,MAXLINE,istream)==NULL)
		return DIALOGUE_ABORT;

/* take off the closing newline from user input */
	buffer[strlen(buffer)-1]='\0';
	
	if(VerifyDate(buffer) !=0) goto end;
	data->ends[data->number] = malloc(strlen(buffer)+1);
	strcpy(data->ends[data->number],buffer);

	fprintf(ostream,"Is the following correct[yn]?\n");
	fprintf(ostream,"Name = %s, start = %s, end = %s\n",data->name[data->number], data->starts[data->number],data->ends[data->number]);
	if((c=fgetc(istream))=='n'){
		free(data->name[data->number]);
		free(data->starts[data->number]);
		free(data->ends[data->number]);
		goto retry;
	}
	data->number++;

/* snarf anything else in the input stream */
	fgets(buffer,MAXLINE,istream);

	return DIALOGUE_SUCCESS;
	
}

