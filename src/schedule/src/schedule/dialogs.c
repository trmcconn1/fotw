/* Implementation of various Dialogue handlers called by Dialogue user
    interface function. The shell in shell.c is a model for these */ 

#include "global.h"
#include "ui.h"
#include "calendar.h"
#include<string.h>


/* Dialogue to select and display time slot lists from among those
   currently defined */

/* This routine is obsolete, and maintained here only as an example */

int DisplayAllTimeSlotLists(FILE *istream, FILE *ostream, char *errmsg,
	void *data)
{
	enum localstate {MENU , PROMPT};
	static int state = MENU;
	char response[6];
	int i;
	int choice;
	int len;

	strcpy(errmsg,"Invalid response\n");

	switch(state){

	case MENU:  /* Display the names of all currently defined Time Slot
                          Lists */
		
		fprintf(ostream,"Currently Defined Time Slot Lists\n");
		for(i=0;i< NumberOfSlotLists;i++) 
			fprintf(ostream,"%d. %s\n",i+1,TimeSlotLists[i]->name);
		state = PROMPT;
		break;

	case PROMPT: 
		fprintf(ostream,"\nType number of list to view, q to quit\n");
		fprintf(ostream,"dtimes> ");
		fgets(response,5,istream);
		if(strcmp(response,"q\n")==0){
			state = MENU;
			return DIALOGUE_FINISH;
		}
		len = strlen(response);
		if(response[len-1]!='\n')
			return DIALOGUE_NONFATAL;
		response[len-1] = '\0';
		choice = atoi(response);
		if((choice < 1) || choice > NumberOfSlotLists)
			return DIALOGUE_NONFATAL; 
		DumpSlotList(ostream,TimeSlotLists[choice-1]);
		state = MENU;
		return DIALOGUE_SUCCESS;	
	}
	return DIALOGUE_SUCCESS;
}
