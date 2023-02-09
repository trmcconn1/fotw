/* Dialogs with user implemented here */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"
#include "gpxpoly.h"

#define BACK 0
#define HELP 1
#define QUIT 2
#define STRING 3
/* Should be greater than number of steps in any dialog: */
#define DONE 20 

static char buffer[81];
static char do_step(char,FILE *);
static void bitch(void);
static char get_user(void);

int track_dialog(FILE *out){

	char n=1;


printf("\n\nThis dialog allows you to enter polygon nodes interactively\n");
printf("The following letters, entered alone on a line, have a special meaning:\n");
printf("\tB: Go back to the previous step\n");
printf("\tH: Print help screen for this item\n");
printf("\tQ or ^D: Quit the dialog\n\n");

while((n=do_step(n,out))!=127)if(n==DONE)break;
return 0;
}

/* Do a step of the dialog and return the next step number */

static int node = 1;

char do_step(char n,FILE *out){

	int j,p;

	switch(n){

	case 1:
		printf("Name for this track (at most 80 characters):\n");
		if (!fgets(buffer,80,stdin))return 127;
		j = strlen(buffer);
		buffer[j-1]='\0';
		fprintf(out,"NAME %s\n",buffer);
		return 2;

	case 2:  
		printf("Number of trackpoints per polygon side? \n");
		switch(get_user()){
			case HELP:
printf("Enter a positive integer that tells how many track points\n");
printf("To use between each node. The default is 100. \n");
				return n;
			case BACK:
				return n-1;
			case QUIT:
				return 127;
			default:
				p = atoi(buffer);
				if((p > 0) && (p <= MAX_POINTS)){
					fprintf(out,"POINTS %d\n",p);
					return n+1;
				}
				bitch();
				return n;
			}
	case 3:
		printf("Connection method[G,L,S,B,H,Q]: \n");
		switch(get_user()){
			case HELP:
printf("Select method for connecting on node to another\n");
printf("G=connect nodes with great circle arcs\n");
printf("L=connect nodes with loxodromes (lines of constant heading)\n");
printf("S=connect nodes linearly in the spherical coordinates \n");
				return n;
			case BACK:
				return n-1;
			case QUIT:
				return 127;
			default:
				switch(buffer[0]){

					case 'L':
						fprintf(out,"Method Loxodrome\n");
						return n+1;
					case 'G':
						fprintf(out,"Method Great Circle\n");
						return n+1;	
					case 'S':
						fprintf(out,"Method Simple\n");
						return n+1;
				}
				bitch();
				return n;
			}
		case 4:
printf("You will be prompted for a latitude and then a longitude for each node\n");
printf("There are two common formats for latitude/longitude in Geodesy\n");
printf("Format 1: dd mm' ss.s\" (e.g 45 34' 48.2\")\n");
printf("Format 2: dd mm.mmmm' (e.g 45 34.8033')\n");
printf("N,S,E, or W is normally included at the beginning or end\n");
printf("This dialog accepts either format and reasonable variations too.\n");
printf("Select the Q option when you want to quit\n");
			return 5;
		case 5:
			printf("Node number %d\n",node);
			printf("Latitude ?[String,B,H,Q]: \n");
			switch(get_user()){
				case HELP:
printf("Latitude string can be any of the standard formats with N or S at start or end\n\n");
				return n;
			case BACK:
				return n-1;
			case QUIT:
				return 127;
			default:
				fprintf(out,"%s\n",buffer);
				return n+1;
			}
	case 6:
			printf("Longitude ?[String,B,H,Q]: \n");
			switch(get_user()){
				case HELP:
printf("Longitude string can be any of the standard formats with W or E at start or end\n\n");
				return n;
			case BACK:
				return n-1;
			case QUIT:
				return 127;
			default:
				fprintf(out,"%s\n",buffer);
				node++;
				return 5;
			}
	}
	return 127;
}

/* self-documenting */

void bitch(void){
	printf("Unrecognized input. Please try again.\n\n");
}

/* Get input from the user and remove the newline fgets unhelpfully
puts there. Return something appropriate to caller */

char get_user(void){

	int n;
	if (!fgets(buffer,80,stdin))return QUIT;
	n = strlen(buffer);
	if(n==0)return QUIT;
	buffer[n-1]='\0';
	if(strcmp(buffer,"B")==0)return BACK;
	if(strcmp(buffer,"H")==0)return HELP;
	if(strcmp(buffer,"Q")==0)return QUIT;
	return STRING;
}
