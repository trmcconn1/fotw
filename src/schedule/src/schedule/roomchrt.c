/* roomchrt.c: routines to produce a specification for a roomchart. */

/* Creates a generic description of a page by means of fictitious commands
   title, label, and box. This is output on stdout. The task of "implementing"
   these commands is left to one of the programs in the render directory. 
*/


#include "db.h"
#include <string.h>

#define SHELLSTREAMS FILE *shell_istream, FILE *shell_ostream,\
FILE *shell_errorstream, FILE *shell_promptstream

/* The following are imaginary units whose interpretation is up to the
   renderer. Let's call them "quatloos". You might as well leave them as 1.
*/

#define BOX_HEIGHT 1
#define HOR_PER_SEC 1
#define TOO_LONG 172800

#define TITLE "Mathematics Dept. Roomchart: "  /* + further stuff made below */

#define MAX(A,B) ( (A) >= (B) ? (A) : (B) )
#define MIN(A,B) ( (A) >= (B) ? (B) : (A) )

int makeboxes(FILE *, struct RoomStruct *, time_t, time_t, int);

char buffer[BUFSIZ];
char buffer2[BUFSIZ];


int roomchart(SHELLSTREAMS)
{
	time_t start;
	time_t end,r,s;
        time_t units_hor = 0;         /* maximum width of page */

	int meets = 0;
	int i,j,k,l;

	if(numtokens != 1 && numtokens != 5) return 1;

	if(numtokens == 1) {
/* Get info from the user */
	
	fprintf(shell_promptstream,"Enter date of start: ");
	fgets(buffer,MAXLINE,shell_istream);
	fprintf(shell_promptstream,"\nEnter time of start: ");
	fgets(buffer2,MAXLINE,shell_istream);
	buffer[strlen(buffer)-1] = '\0';
	buffer2[strlen(buffer2)-1] = '\0';
	start = ParseDate(buffer,buffer2);


	fprintf(shell_promptstream,"\nEnter date of end: ");
	fgets(buffer,MAXLINE,shell_istream);
	fprintf(shell_promptstream,"\nEnter time of end: ");
	fgets(buffer2,MAXLINE,shell_istream);
	buffer[strlen(buffer)-1] = '\0';
	buffer2[strlen(buffer2)-1] = '\0';
	end = ParseDate(buffer,buffer2);
	fprintf(shell_promptstream,"\n");
	}

	if(numtokens == 5){
	start = ParseDate(Tokens[1]->text,Tokens[2]->text);
	end = ParseDate(Tokens[3]->text,Tokens[4]->text);
	}

        if(start <= 0 || end <= 0 || end <= start || (end - start) > TOO_LONG){
                myerror(shell_errorstream,"Time range %ld -> %ld doesn't make sense\n",
                 start,end);
                return 1;
            }
/* Make a first pass through rooms data to determine dimensions of page */

	l=0;
	for(k=0;k<numrooms;k++){  /* loop over rooms */
	meets = 0;	
/* Loop over classes meeting in the room */

	   for(i=0;i<Rooms[k]->numclasses;i++)	
	
		/* loop over the times that each class meets */

		for(j=0;j<(Rooms[k]->classes[i])->times->number;j++){
		 	r = MAX(start,(Rooms[k]->classes[i])->times->timeslots[j]->start);
		 	s = MIN(end,(Rooms[k]->classes[i])->times->timeslots[j]->end);
			if(r<=end && s>=start){
				units_hor = MAX(s-start,units_hor);
				meets = 1;
			}
		}
	if(meets)l++;
	}
	
/* Print the page dimensions */
	fprintf(shell_ostream,"%d %lu\n",l*BOX_HEIGHT 
		,MIN(units_hor,end-start)*HOR_PER_SEC);

/* Print a title "command" */

	sprintf(buffer,"title \"");
	strcat(buffer,TITLE);
	strftime(buffer2,LONGNAME,"%a %m/%d/%y %I:%M %p",localtime(&start));
	strcat(buffer,buffer2);
	strcat(buffer," ---> ");
	strftime(buffer2,LONGNAME,"%a %m/%d/%y %I:%M %p",localtime(&end));
	strcat(buffer,buffer2);
	strcat(buffer,"\"");
	fprintf(shell_ostream,"%s\n",buffer);

/* Print the time origin command */
/* Used in producing a time scale at the top of a roomchart */

	fprintf(shell_ostream,"timeorigin %lu\n",start);
	
/* loop over rooms. Conceptually, these will be the "lines" of the ultimate
      roomchart printout. */

	j = 0;
	for(i=0;i<numrooms;i++){
		if(Rooms[i]->numclasses > 0){
	          fprintf(shell_ostream,"label %d \"%s\"\n",j,
                      Rooms[i]->name);
		makeboxes(shell_ostream, Rooms[i],start, end, j);
		j++;
		}
	}
	return 0;
}

/* makeboxes: for the given room and time range, prints to file a list
of commands of the form box(a,b,c,d,string), one for each time slot that
some class meets in the room. Each "command" expresses the desire
to have  a box of the form 
      |                 |
      | a               |
      V*-------------*  | b
-----> |             |  |
  c    |   string    |  |
       |             |  |
       *-------------*  V
--------------------->
           d

created on a page. The task of actually rendering such a box on the page
is left to another program.    

*/


int makeboxes(FILE *output, struct RoomStruct *rmptr, time_t start,
                   time_t end, int pos )
{
	int i,j;
	time_t k,l;


       /* Loop over classes that meet in the room */
	for(i=0;i<rmptr->numclasses;i++)	
	
		/* loop over the times that each class meets */
		for(j=0;j<(rmptr->classes[i])->times->number;j++){
		 	k = MAX(start,(rmptr->classes[i])->times->timeslots[j]->start);
		 	l = MIN(end,(rmptr->classes[i])->times->timeslots[j]->end);
			if(k<=end && l>=start)
				fprintf(output,"box %d %d %lu %lu \"%s %s\"\n",
					pos, pos + BOX_HEIGHT,
					(k-start)*HOR_PER_SEC,
					MIN((l-start)*HOR_PER_SEC,end-start),
				        (rmptr->classes[i])->crse_name,
					(rmptr->classes[i])->name);
		}
	return 0;
}
