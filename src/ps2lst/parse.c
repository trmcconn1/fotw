/* parse.c return next token in input stream */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"  /* declares parsing states */


/* Change as needed for a given application */
#define COMMENT_CHARS "#"
#define WORD_DELIMS ",;"
#define LINE_DELIMS "\n\r"
#define MY_BUFSIZ 2048


static char my_buffer[MY_BUFSIZ];
static int line_no = 0;


/* This is a very basic parser that returns a pointer to the next
field of an input stream. It returns special status EOL, EOF, ERROR
through the integer pointer.
Returned strings and status value are destroyed
on the next call, so use them or lose them */

char *parse(FILE *input, int *status)
{
	char nxtchr;
	int chrcnt = 0;
	
	if(input == NULL){
		*status = MY_ERROR;
		return NULL;
	}
	if(status == NULL){
		*status = MY_ERROR;
		return NULL;
	}

	*status = MY_NORMAL;

	while((nxtchr = fgetc(input)) != EOF){

		if(!strchr(WORD_DELIMS,nxtchr)){
			/* Is it end of line ?*/
			if(strchr(LINE_DELIMS,nxtchr)){
				*status = MY_EOL;
				line_no++;
				break;
			}
			/* Comment. Same as eol except throw rest of line
                           away */
			if(strchr(COMMENT_CHARS,nxtchr)){
				*status = MY_EOL;
				line_no++;
				while(((nxtchr = fgetc(input))!= EOF)&&
					!(strchr(LINE_DELIMS,nxtchr)));
				break;	
			}
			if(chrcnt >= MY_BUFSIZ){
				*status = MY_ERROR;	
				return NULL;
			}
			my_buffer[chrcnt++] = nxtchr;
			continue;
		}
		*status = MY_NORMAL;
		break;
	}
	if(nxtchr == EOF) *status = MY_EOF; 
	my_buffer[chrcnt] = '\0';
	return my_buffer;
}


void parse_error(char *msg)
{
	fprintf(stderr,"Parse error in input line %d\n",line_no+1);
	fprintf(stderr,"%s\n",msg);
	fflush(stdin);
	exit(1);	
}
