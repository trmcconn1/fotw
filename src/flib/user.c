/* user.c: implement routines to support a primitive menu driven
 *         user interface for a program running under a shell in a terminal.
 *
 */
 
#include "global.h"

#define PROMPT ": "

/* Generic routine to get a line of data from the user, presumably
   in answer to a dialogue question. Response is returned though union
   pointer argument. You must tell the routine what type of data response
   is expected using the type argument. The message is printed, followed
   by a prompt.

   Returns 0 if operation was successful, 1 otherwise. */

int getuser(union my_data *data, int type, char *message){
	char buffer[MAXLINE],*p;
	int n;

	if(message && strlen(message)>0)
		fprintf(stderr,"%s",message);
	fprintf(stderr,"%s",PROMPT);
	p = fgets(buffer,MAXLINE,stdin);

	/* do some sanity checks */

	if(!p)return 1;
	n = strlen(buffer);
	if(n<=1)return 1; /* recall that buffer contains at least \n */

	switch(type){

		case INTEGER:
			data->i = atoi(buffer);
			break;
		case DOUBLE:
			data->d = atof(buffer);
			break;
		case STRING:
			if(!(p = (char *)malloc(n*sizeof(char))))
					return 1;
			strcpy(p,buffer);
			p[n-1]='\0'; /* kill newline */
			data->s = p;
			break;
		default:
			return 1;
	}
	return 0;
}
			

/* The following is called when getuser is unsuccessful. It presents the
   venerable MSDOS error message and then returns a value depending on
   how the user responds:

	abort: exit the program from here.
	retry: return 1
	ignore: return 0

   The caller needs to decide what to do based on the return value.

*/
 
int handle_error(){

	int c,d;

	fprintf(stderr,"\n   **** Bad data or other error ****\n\n");
	while(1){
		fprintf(stderr,"\nAbort[a],Retry[r],Ignore[i]?\n");
		fprintf(stderr,"Enter appropriate lower case letter and hit return:");
		c = fgetc(stdin);
		d = fgetc(stdin); /* this should be a new line */
		if(d != '\n')continue;
		fprintf(stderr,"\n");
		switch(c){
			case 'a':
				exit(1);
			case 'r':
				return 1;
			case 'i':
				return 0;
			case EOF:
				exit(1);
			default:
				fprintf(stderr,"Unrecognized response!\n");
				break;
		}
	}
}

