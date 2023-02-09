/* User interface specific declarations, other non-portable declarations */

#define DIALOGUE_SUCCESS 0
#define DIALOGUE_FATAL 1
#define DIALOGUE_NONFATAL 2
#define DIALOGUE_FINISH 3
#define DIALOGUE_ABORT 4
 
int Dialogue(FILE *istream, FILE *ostream, FILE *errstream, FILE *prompt_stream,
		char *title, void *data,
		int ( *DialogueHandler ) ( FILE *, FILE *, FILE *,FILE *,
	char *errmsg, void *data) );


