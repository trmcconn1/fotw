/* rch2txt.c Main module for program to convert a roomchart specification
   produced by the roomchart command of the schedule program into a page
   of text. The output goes to stdout. 

usage rch2txt <roomchart specfile>

*/

/* It is intended that the file produced by this program should be 
   printed in landscape mode with compressed font */

/***********************************************************************  
         Author: Terry McConnell June 1996
************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdarg.h>
#include<time.h>

#define USAGE "rch2txt <roomchart specfile>\n"

/* Dimensions of document in characters X lines */
#define CHAR_WIDTH 120
#define CHAR_LENGTH 80

#define BOX_HEIGHT 3
#define FORMFEED '\014'
#define LINES_PER_PAGE 44 
#define MAXLINE 256       /* input line buffer size */
#define MAXTOKEN 256
#define MAXTOKS 80
#define COMMENT '#'
#define METACHARS "\"\n\\"
#define DELIMITER "     \n"    /* space, tab , newline */
#define VERT_CHAR '|';
#define HOR_CHAR '-';
#define TOP_HEIGHT 5
#define LABEL_WIDTH 6



/* The page layout is like this:
----------------------------------------------------------------------
|          |          Title                                          |
|          | ...................time ruler...........................
______________________________________________________________________
|          |                                                         |
|          |                                                         |
|  Labels  |                                                         |
|          |                                                         |
.          .                                                         .
.          .                    BOXES                                .
.          .                                                         .
|          |                                                         |
______________________________________________________________________ 

*/


/* Large Array to contain the image of a page */

char Page[CHAR_LENGTH][CHAR_WIDTH];

void myerror(FILE *, char *fmt, ...);
void fatal(FILE *, char *fmt, ...);


/* Stuff for parsing the lines of the input file */

struct TokenType {
char text[MAXTOKEN];
int position;
};

int tokenize(char *line,struct TokenType **tokens);
extern void krqsort ( void *arrayname[], int, int,
         int (*functionname)(void *, void *));


struct TokenType *Tokens[MAXTOKS];
char ProgramName[MAXLINE];

int
main( int argc, char **argv)
{
	FILE *input;
	char *ptr;
	int offset;
	int last_hour;
	int pg;
	time_t origin;
	struct tm *tmptr;
        time_t temp;
	int numtokens;
        time_t units_hor, units_vert;   /* page dimensions read from file */
        time_t units_per_char, units_per_line;
	char linebuf[MAXLINE];
	struct TokenType **tokens;
	int i,j;
	int a,b,c,d;

	strcpy(ProgramName,argv[0]);

	/* Try to open the input file for read */
	if(argc !=2) fatal(stderr,USAGE);
	if((input = fopen(argv[1],"r"))==NULL)
          fatal(stderr,"Unable to open %s\n", argv[1]);

/* Fill the page with blanks */
	for(i=0;i<CHAR_LENGTH;i++)
		for(j=0;j<CHAR_WIDTH;j++)
			Page[i][j] = ' ';

/* Allocate memory for tokens */

	for(i=0;i<MAXTOKS;i++)
		if((Tokens[i]=(struct TokenType *)malloc(sizeof(struct TokenType)))==NULL)
               		 fatal(stderr,"Unable to allocate token memory\n");
	tokens = &(Tokens[0]);

/* skip over any blank lines at top of input file */

	while(1){
	if(fgets(linebuf,MAXLINE,input) == NULL)
		fatal(stderr,"Empty input file\n");
        if(strlen(linebuf)<=1) continue;
	else break;
	}


/* extract page dimensions in "quatloos" from buffer */
	
	numtokens = tokenize(linebuf, tokens);
	if(numtokens != 2)fatal(stderr,"%s is not a room chart file \n",
		argv[1]);
        units_vert = atol(Tokens[0]->text);
        units_hor = atol(Tokens[1]->text);

	units_per_char = units_hor/(CHAR_WIDTH-LABEL_WIDTH) + 1;
	units_per_line = units_vert/CHAR_LENGTH;
	if(units_per_line == 0)units_per_line = 1;

/* Make sure the page is big enough */
	if(TOP_HEIGHT + units_vert*BOX_HEIGHT >= CHAR_LENGTH)
		fatal(stderr,"Too many rooms: %d\n",units_vert);

/* Loop over lines in input */
	ptr = linebuf;

        while(fgets(ptr,MAXLINE + linebuf -ptr ,input) != NULL){

/* Now parse the contents of linebuf */

        	numtokens = tokenize(linebuf,Tokens);
        	if(numtokens == -1){       /* line continues */
               	  ptr = ptr + strlen(ptr);
               	  continue;
        	}
        	if(numtokens == 0){
               	 ptr = linebuf;
               	 continue;
        	}

	if(strcmp(Tokens[0]->text,"title")==0) {

/* Put title centered in top area */

		offset = (CHAR_WIDTH - strlen(Tokens[1]->text))/2;
		for(j=0;j<strlen(Tokens[1]->text);j++)
			Page[TOP_HEIGHT/2][offset + j] = *(Tokens[1]->text + j);
	}

	if(strcmp(Tokens[0]->text,"timeorigin")==0){
		origin = atol(Tokens[1]->text);
		tmptr = localtime(&origin);
		last_hour = tmptr->tm_hour;
                /* Put a time ruler at the bottom of the top area */
		i = TOP_HEIGHT-1;
		for(j=LABEL_WIDTH-1;j<CHAR_WIDTH-1;j++){
			/* if it's an even hour, print the hour number */
			tmptr = localtime(&origin);
			if(tmptr->tm_hour > last_hour){
				if(tmptr->tm_hour > 12 )
				  Page[i][j] = '0' + tmptr->tm_hour - 12;
				if(tmptr->tm_hour < 10 )
				  Page[i][j] = '0' + tmptr->tm_hour;
				if(tmptr->tm_hour == 10){
					Page[i][j] = '1';
					Page[i][j+1] = '0';
				}
				if(tmptr->tm_hour == 11){
					Page[i][j] = '1';
					Page[i][j+1] = '1';
				}
				if(tmptr->tm_hour == 12){
					Page[i][j] = '1';
					Page[i][j+1] = '2';
				}
			}
			/* Avoid overwriting digit from previous round */
			else if(Page[i][j] == ' ')Page[i][j] = HOR_CHAR;
			origin += units_per_char;
			last_hour = tmptr->tm_hour;
		}
	}


	if(strcmp(Tokens[0]->text,"label")==0) {

/* Put label left-justified in label area */

		i = TOP_HEIGHT + atoi(Tokens[1]->text)*BOX_HEIGHT;
		temp = strlen(Tokens[2]->text);
		if(temp > LABEL_WIDTH) temp = LABEL_WIDTH;
		for(j=0;j<temp;j++)
			Page[i + BOX_HEIGHT/2][j] = *(Tokens[2]->text + j);
	}

	if(strcmp(Tokens[0]->text,"box")==0){

/* vertical offsets in characters from top of page */

	a = TOP_HEIGHT + BOX_HEIGHT*atoi(Tokens[1]->text);
	b = TOP_HEIGHT + BOX_HEIGHT*atoi(Tokens[2]->text);

/* horizontal offsets in chars from side of page */

        c = LABEL_WIDTH + atol(Tokens[3]->text)/units_per_char;
        d = LABEL_WIDTH + atol(Tokens[4]->text)/units_per_char;

	if(d > c){
/* Put top of box on page */

	for(j=c;j<d;j++)
		Page[a][j] = HOR_CHAR;

	for(i=a+1;i<b-1;i++){            /* print text centered in box */
		if(i-a == BOX_HEIGHT/2){  
			temp = strlen(Tokens[5]->text);
			offset = (d-c-temp)/2;
			if(offset <=0) offset = 0;
			if(temp >= d-c) temp = d-c;
			for(j=0;j<temp;j++)
				Page[i][j+c+offset] = *(Tokens[5]->text + j);
		}
		Page[i][c] = VERT_CHAR;
		Page[i][d-1] = VERT_CHAR;
	}

/* Put bottom of box on page */
	for(j=c;j<d;j++)
		Page[b-1][j] = HOR_CHAR;

	}
	}
		

/* prepare for next input line */
        	ptr = linebuf;
	}

/* print the complete document to stdout with pages separated by formfeeds */

	for(pg=0;pg < CHAR_LENGTH/LINES_PER_PAGE + 1;pg++){
                 /* Heading  at top of each page */
		for(i=0;i<TOP_HEIGHT;i++){
			for(j=0;j<CHAR_WIDTH;j++)
	 			printf("%c",Page[i][j]);	
			printf("\n");
		}
                 /* Rest of this page */
		i = (pg == 0 ? TOP_HEIGHT : 0);
		for(;;i++){
			if(i>=LINES_PER_PAGE && pg == 0)break;
			if(i>=LINES_PER_PAGE - TOP_HEIGHT && pg > 0)break;
			if(i+LINES_PER_PAGE*pg >= CHAR_LENGTH)break;
			for(j=0;j<CHAR_WIDTH;j++)
	 			printf("%c",Page[pg*LINES_PER_PAGE + i][j]);	
			printf("\n");
		}
		printf("%c",FORMFEED);
	}
	fclose(input);
	return 0;
	
}

/* myerror: generic error reporting routine */

void myerror(FILE *stream,char *fmt, ...)
{
        va_list args;

        va_start(args,fmt);
        fprintf(stream,"%s: ",ProgramName);
        vfprintf(stream,fmt,args);
        va_end(args);
        return;
}

/* fatal: generic error reporting routine */

void fatal(FILE *stream,char *fmt, ...)
{
        va_list args;

        va_start(args,fmt);
        fprintf(stream,"%s: Fatal Error.\n",ProgramName);
        vfprintf(stream,fmt,args);
        fprintf(stream,"\n");
        va_end(args);
        exit(1);
}


int
CompareTokensByPosition(struct TokenType *first, struct TokenType *second)
{
/* NULL comes before anything */
        if(first == NULL)
                if(second == NULL) return 0;
                else return -1;

        if(second == NULL) return 1;
        if(first->position == second->position) return 0;
        return first->position < second->position ? -1 : 1;
}


/* Break Line into tokens delimited by DELIMITER and return number of
      tokens found. Return -1 if more input is needed. Implements
	metacharacter escapes, quoting, and variable substitution
*/

int tokenize(char *line,struct TokenType **tokens)
{
	int count = 0;
	char *ptr,*ptrhold;
	char buffer[MAXLINE];
	char holdbuffer[MAXLINE];
	enum {NORMAL, ESCAPED};
	char  *bufptr, *cptr;
	int state = NORMAL;


	strcpy(buffer,line);

	if(buffer == NULL) return 0;


	ptr = buffer; 
	bufptr = holdbuffer;


		if(strlen(buffer)>=MAXLINE){ 
		myerror(stderr,"Line %s too long\n",buffer);
			return 0;
		}
		
/* Skip comment lines and lines starting with \n */

		if((buffer[0] == COMMENT)||(buffer[0]=='\n'))
			return 0;


/* First pass: collapse all multiple escapes. Determine whether newline
is escaped and request more input if so. Expand shell variables. */             
/* The processed line ends up in holdbuf */


	while( *ptr != '\0'){
		if(bufptr - holdbuffer >= MAXLINE-1){
			myerror(stderr,"Line %s became too long during variable expansion\n",buffer);
			break;
		}
		if(state == NORMAL)
	  /* Is it a meta char ? if not, just copy */
			if((cptr = strchr(METACHARS,*ptr)) == NULL)
				*bufptr++ = *ptr++;
			else {  /* if so, see which one */
				switch(*cptr) {
				case '\\':
					if(strchr(METACHARS,*(++ptr))==NULL)
					*bufptr++ = '\\';   /* skip it */
					else state = ESCAPED;
					break;
				 default:
					*bufptr++ = *ptr++;
				}
			}
		else  { /* state is ESCAPED */
			switch(*ptr){
				case '\n': /* substitute a space */
				       *bufptr++ = ' ';
					ptr++;
					break;
				case '\\':
					/* if escaped backslash followed
					   by a quote, insert whitespace */
					*bufptr++ = *ptr++;
					if(*ptr=='\"')
					*bufptr++ = ' ';
					break;
				case '\"': /* special case: keep escape in 
					    front of a quote -- it gets
					    handled on the next pass. */
					*bufptr++ = '\\';
					*bufptr++ = *ptr++;
					break;
				default:
				*bufptr++ = *ptr++;
			}
				state = NORMAL;
		}
	}

/* properly terminate buffer */
	*bufptr = '\0';
/* copy the hold buffer back over the original one */
	strcpy(buffer, holdbuffer);

/* Check whether last character was an escaped newline */
	if(*(buffer+strlen(buffer)-1)!='\n')
		return -1; /* need more input */


/* OK, now all escapes have been fixed. */

/* Next find all quote delimited tokens */


/* Find the first unescaped quote */

	ptr = buffer;
	while(((ptr = strchr(ptr,'\"'))!=NULL) &&
		(ptr > buffer) && (*(ptr-1)=='\\')){
			*(ptr-1)=' ';  /* change \ to blank */
			ptr++;
		}
	while(ptr != NULL){   /* ptr pts at leading quote */
		ptrhold = ptr++;  /* save it */

/* advance to next unescaped quote */ 
		while(((ptr = strchr(ptr,'\"'))!=NULL) &&
		  (*(ptr-1)=='\\')){
			*(ptr-1) = ' ';  /* change \ to blank */
			ptr++;
		}

	if(ptr == NULL){
		myerror(stderr,"Unmatched \" in %s\n",buffer);
		return 0;
	}

/* ptr now points at close quote. blank and copy into token buffer */

	*ptr = '\0';         /* mark end of string */
	*ptrhold++ = ' ';    /* convert leading quote to whitespace */
	strcpy((*tokens)->text,ptrhold);
	(*tokens++)->position = ptrhold - buffer;
	if(count >= MAXTOKS){
			myerror(stderr,"Too many tokens. Must be < %d\n",
				MAXTOKS);
			return count;
		}

	count++;
	while(ptrhold <= ptr)   /* convert token just found to whitespace */
	   *ptrhold++ = ' ';
/* advance to next unescaped quote */ 
	while(((ptr = strchr(ptr,'\"'))!=NULL) &&
	  (*(ptr-1)=='\\')){
		*(ptr-1) = ' ';
		ptr++;
		}
	}


/* Get tokens delimited by white space */

	ptr = strtok(buffer,DELIMITER);
	while(ptr != NULL){
		if(count >= MAXTOKS){
			myerror(stderr,"Too many tokens. Must be < %d\n",
				MAXTOKS);
			return count;
		}
		strcpy((*tokens)->text,ptr);
		(*tokens++)->position = ptr -buffer;
		count++;
		ptr = strtok(NULL,DELIMITER);
	}

/* Sort the tokens into the order they occured */
	krqsort((void **)Tokens,0,count-1,
	  (int(*) (void *,void *))CompareTokensByPosition);
	return count;
}
