/* Implementation of the User Interface and other non-portable stuff */


#include "global.h"
#include "ui.h"


char errmsg[BUFSIZ];

/* Dialogue: Manage a dialogue with the user. Print a title, and repeatedly
call a handler until it returns with one of the exit conditions */

int Dialogue(FILE *istream, FILE *ostream, FILE *errstream,FILE *prompt_stream,
                char *title, void *data,
                int ( *DialogueHandler ) ( FILE *, FILE *, FILE *,FILE *,
			char *errmsg, void *data) )

{
        if(title != NULL)
	fprintf(ostream,"%s\n",title);
	while(1){
		switch(DialogueHandler(istream,ostream,errstream,prompt_stream,
			errmsg,data)){
			case DIALOGUE_SUCCESS: /* Do nothing, go round again */
				break;
			case DIALOGUE_FATAL:
				fprintf(errorstream,"%s",errmsg);
				return 1;
			case DIALOGUE_NONFATAL:
				fprintf(errorstream,"%s",errmsg);
				break;
			case DIALOGUE_FINISH:
				return 0;
			case DIALOGUE_ABORT:
				return 1;
		}
	}				
}
